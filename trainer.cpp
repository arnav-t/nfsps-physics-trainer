#include <iostream>
#include <cmath>
#include <windows.h>

const DWORD POSX = 0xfff990;
const DWORD POSY = 0xfff994;
const DWORD POSZ = 0xfff998;
 
const DWORD VX = 0xfff920; 
const DWORD VY = 0xfff924; 
const DWORD VZ = 0xfff928;
 
const DWORD AX = 0xfff980;
const DWORD AY = 0xfff980;
const DWORD AZ = 0xfff980;


class Trainer {
  DWORD pid;
  HWND hwnd;
  HANDLE handle;

  float magntiude(float x, float y, float z) {
    return std::sqrt(x*x + y*y + z*z);
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
        set_speed(150.0);
      }
    }
  }
};

int main() {
  Trainer t;
  t.loop();
  return 0;
}