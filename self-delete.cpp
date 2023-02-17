#include <windows.h>

int main() {
  // Get the path to the current executable
  TCHAR szPath[MAX_PATH];
  GetModuleFileName(NULL, szPath, MAX_PATH);

  // Delete the file
  DeleteFile(szPath);

  return 0;
}
