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

// const DWORD DMG_FUNC_EPILOGUE = 0x6b1414;
const DWORD DMG_FUNC_JNE = 0x6b1368;
const DWORD DMG_SUBR_JE = 0x4ceb3f;

// Jump to an intermediary conditional short jump to end of function. No wastage of bytes!
std::vector<unsigned char> D1_OG{0x75, 0x27};
std::vector<unsigned char> D1{0x75, 0x4a};

std::vector<unsigned char> D2_OG{0x74};
std::vector<unsigned char> D2{0xeb};


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
    write_byte_array(D1, DMG_FUNC_JNE);
    write_byte_array(D2, DMG_SUBR_JE);
  }

  void disable_no_dmg() {
    no_dmg = false;
    write_byte_array(D1_OG, DMG_FUNC_JNE);
    write_byte_array(D2_OG, DMG_SUBR_JE);
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