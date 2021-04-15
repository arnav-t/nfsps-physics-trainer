#include <iostream>
#include <vector>
#include <cmath>
#include <windows.h>

const DWORD POSX = 0xfff990;
const DWORD POSY = 0xfff994;
const DWORD POSZ = 0xfff998;
 
const DWORD VX = 0xfff920; 
const DWORD VY = 0xfff924; 
const DWORD VZ = 0xfff928;
 
const DWORD AX = 0xfff980;
const DWORD AY = 0xfff984;
const DWORD AZ = 0xfff988;


const DWORD DMG_CODECAVE = 0x4d2d86;
const DWORD DMG_FUNC = 0x4ce8c0;
std::vector<unsigned char> DMG_JMP_OG = {
  0x83, 0xec, 0x18,                     // sub esp, 18
  0x8b, 0x44, 0x24, 0x1c,               // mov eax, [esp+1c]
};
std::vector<unsigned char> DMG_JMP = {
  0xe9, 0xc1, 0x44, 0x00, 0x00,         // jmp DMG_CODECAVE
  0x90,                                 // nop
  0x90,                                 // nop
};
std::vector<unsigned char> DMG = {
  0x8b, 0x0d, 0x48, 0xf8, 0xa6, 0x00,   // mov ecx, [00a6f848]
  0x39, 0x4c, 0x24, 0x04,               // cmp [esp+4], ecx
  0x75, 0x03,                           // jne short +3
  0xc2, 0x00, 0x00,                     // ret 0
  0x83, 0xec, 0x18,                     // sub esp, 18
  0x8b, 0x44, 0x24, 0x1c,               // mov eax, [esp+1c]
  0xe9, 0x24, 0xbb, 0xff, 0xff,         // jmp 4ce8c5
};


class Trainer {
  DWORD pid;
  HWND hwnd;
  HANDLE handle;
  bool no_dmg = false;

  float magntiude(float x, float y, float z) {
    return std::sqrt(x*x + y*y + z*z);
  }

  void write_byte_array(std::vector<unsigned char>& bytes, DWORD addr) {
    for (int i=0; i<bytes.size(); ++i, addr += sizeof(unsigned char)) {
      WriteProcessMemory(handle, (LPVOID)addr, &bytes[i], sizeof(unsigned char), NULL);
    }
  }

public:
  Trainer() {
    // hwnd = FindWindow(NULL, TEXT("Need for Speed™ ProStreet"));
    hwnd = FindWindow(NULL, TEXT("Need for Speed - ProStreet"));
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) std::cout << "Can't find game\n";
    handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  }

  void set_speed(float v) {
    float vx, vy, vz;
    ReadProcessMemory(handle, (LPVOID)VX, &vx, sizeof(vx), NULL);
    ReadProcessMemory(handle, (LPVOID)VY, &vy, sizeof(vy), NULL);
    ReadProcessMemory(handle, (LPVOID)VZ, &vz, sizeof(vz), NULL);
    
    float V = magntiude(vx, vy, vz);
    if (std::fabs(V) < 1e-9) V = 1.0; 
    vx *= v/V;
    vy *= v/V;
    vz *= v/V;

    WriteProcessMemory(handle, (LPVOID)VX, &vx, sizeof(vx), NULL);
    WriteProcessMemory(handle, (LPVOID)VY, &vy, sizeof(vy), NULL);
    WriteProcessMemory(handle, (LPVOID)VZ, &vz, sizeof(vz), NULL);
  }

  void multiply_speed(float k) {
    float vx, vy, vz;
    ReadProcessMemory(handle, (LPVOID)VX, &vx, sizeof(vx), NULL);
    ReadProcessMemory(handle, (LPVOID)VY, &vy, sizeof(vy), NULL);
    ReadProcessMemory(handle, (LPVOID)VZ, &vz, sizeof(vz), NULL);
    
    vx *= k;
    vy *= k;
    vz *= k;

    WriteProcessMemory(handle, (LPVOID)VX, &vx, sizeof(vx), NULL);
    WriteProcessMemory(handle, (LPVOID)VY, &vy, sizeof(vy), NULL);
    WriteProcessMemory(handle, (LPVOID)VZ, &vz, sizeof(vz), NULL);
  }

  void jump(float dvy) {
    float vy;
    ReadProcessMemory(handle, (LPVOID)VY, &vy, sizeof(vy), NULL);

    vy += dvy;

    WriteProcessMemory(handle, (LPVOID)VY, &vy, sizeof(vy), NULL);
  }

  void lunge(float v) {
    float vx, vy, vz;
    ReadProcessMemory(handle, (LPVOID)VX, &vx, sizeof(vx), NULL);
    ReadProcessMemory(handle, (LPVOID)VY, &vy, sizeof(vy), NULL);
    ReadProcessMemory(handle, (LPVOID)VZ, &vz, sizeof(vz), NULL);

    set_speed(v);
    Sleep(100);

    WriteProcessMemory(handle, (LPVOID)VX, &vx, sizeof(vx), NULL);
    WriteProcessMemory(handle, (LPVOID)VY, &vy, sizeof(vy), NULL);
    WriteProcessMemory(handle, (LPVOID)VZ, &vz, sizeof(vz), NULL);
  }

  void enable_no_dmg() {
    no_dmg = true;
    write_byte_array(DMG, DMG_CODECAVE);
    write_byte_array(DMG_JMP, DMG_FUNC);
  }

  void disable_no_dmg() {
    no_dmg = false;
    write_byte_array(DMG_JMP_OG, DMG_FUNC);
  }

  void loop() {
    while (!GetAsyncKeyState(VK_END) & 1) {
      if (GetAsyncKeyState(VK_NUMPAD8) & 1) {
        std::cout << "2x Speed\n";
        multiply_speed(2.0);
      }

      if (GetAsyncKeyState(VK_NUMPAD0) & 1) {
        std::cout << "Jump\n";
        jump(10.0);
      }

      if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
        std::cout << "Instant Brake\n";
        multiply_speed(0.0);
      }

      if (GetAsyncKeyState(VK_NUMPAD5) & 1) {
        std::cout << "Set Speed\n";
        set_speed(200.0);
      }

      if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
        std::cout << "Lunge\n";
        lunge(250.0);
      }

      if (GetAsyncKeyState(VK_ADD) & 1) {
        if (no_dmg) {
          std::cout << "No Damage Disabled\n";
          disable_no_dmg();
        } else {
          std::cout << "No Damage Enabled\n";
          enable_no_dmg();
        }
      }
    }
  }
};

int main() {
  Trainer t;
  std::cout << 
    "=====================================\n"
    "      NFS:PS Physics Trainer +6\n"
    "=====================================\n"
    "Numpad 0: Jump\n"
    "Numpad 1: Car Slam\n"
    "Numpad 2: Instant Brake\n"
    "Numpad 5: Instant Speed\n"
    "Numpad 8: 2x Speed\n"
    "Numpad +: Enable/Disable No Damage\n"
    "=====================================\n\n";
  t.loop();
  return 0;
}