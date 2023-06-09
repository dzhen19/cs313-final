#include <vector>
#include <string>
#include <iostream>

std::string PruneDir(const std::string &name)
{
   std::string temp = name;

   size_t i = name.rfind("/", name.size() - 2);
   if (i == std::string::npos)
      i = name.rfind("\\", name.size() - 2);
   if (i != std::string::npos)
      temp = temp.replace(0, i + 1, "");

   return temp;
}

std::string PruneName(const std::string &name)
{
   std::string temp = name;

   size_t i = temp.rfind(".");
   if (i != std::string::npos)
      temp = temp.replace(i, temp.size(), "");

   return PruneDir(temp);
}

#ifdef APPLE
#include <dirent.h>
#include <sys/types.h>
#include <algorithm>

using namespace std;

std::string PromptToLoadDir()
{
   std::cout << "Not implemented\n";
   return "";
}

std::string PromptToLoad()
{
   std::cout << "Not implemented\n";
   return "";
}

std::vector<std::string> GetFilenamesInDir(const std::string &dirname,
                                           const std::string &filter)
{
   DIR *dir;
   struct dirent *ent;
   cout << dirname.c_str() << endl;
   vector<string> files;
   if ((dir = opendir(dirname.c_str())) != NULL)
   {
      while ((ent = readdir(dir)) != NULL)
      {
         cout << ent->d_name << endl;
         string name = ent->d_name;
         if (filter.size() > 0 && name.find(filter) != std::string::npos)
         {
            files.push_back(ent->d_name);
         }
      }
   }
   return files;
}

#endif

#ifdef UNIX
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <string>
#include <algorithm>

using namespace std;

std::string PromptToLoadDir()
{
   FILE *f = popen("zenity --file-selection --directory", "r");
   char file[2048];
   fgets(file, 2048, f);
   string result(file);
   result.erase(find_if(result.rbegin(), result.rend(),
                        not1(ptr_fun<int, int>(isspace)))
                    .base(),
                result.end());
   return result;
}

std::string PromptToLoad()
{
   FILE *f = popen("zenity --file-selection", "r");
   char file[2048];
   fgets(file, 2048, f);
   string result(file);
   result.erase(find_if(result.rbegin(), result.rend(),
                        not1(ptr_fun<int, int>(isspace)))
                    .base(),
                result.end());
   return result;
}

std::vector<std::string> GetFilenamesInDir(const std::string &dirname, const std::string &filter)
{
   DIR *dir;
   struct dirent *ent;
   cout << dirname.c_str() << endl;
   vector<string> files;
   if ((dir = opendir(dirname.c_str())) != NULL)
   {
      while ((ent = readdir(dir)) != NULL)
      {
         cout << ent->d_name << endl;
         string name = ent->d_name;
         if (filter.size() > 0 && name.find(filter) != std::string::npos)
         {
            files.push_back(ent->d_name);
         }
      }
   }
   return files;
}
#endif

#ifdef _WIN32
#include <windows.h>
#include <Shlobj.h>
#include <Shobjidl.h>

#ifdef UNICODE
std::wstring s2ws(const std::string &s)
{
   int len;
   int slength = (int)s.length() + 1;
   len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
   wchar_t *buf = new wchar_t[len];
   MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
   std::wstring r(buf);
   delete[] buf;
   return r;
}

std::string ws2s(const std::wstring &wstr)
{
   LPSTR szText = new CHAR[wstr.size() + 1];
   WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szText, (int)wstr.size() + 1, 0, 0);
   std::string tmp = szText;
   delete[] szText;
   return tmp;
}

std::string PromptToLoadMotion()
{
   OPENFILENAME ofn;

   wchar_t filename[1024];
   memset(filename, 0, sizeof(filename));

   memset(&ofn, 0, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.lpstrFilter = TEXT("AMC Files\0*.amc\0BVH Files\0*.bvh\0All Files\0*.*\0\0");
   ofn.lpstrFile = filename; //(LPWSTR)
   ofn.nMaxFile = 1024;
   ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

   BOOL result = GetOpenFileName(&ofn);
   if (result)
   {
      // for (int i = 0; i < 2014; i++) printf("%s ", filename[i]);
      std::string result = ws2s(filename);
      return result;
   }
   else
   {
      DWORD err = CommDlgExtendedError();
      err = err;
   }
   return "";
}

std::vector<std::string> GetFilenamesInDir(const std::string &dirname, const std::string &filter)
{
   printf("Target directory is %s\n", dirname.c_str());
   std::string dirnamestr = dirname;
   dirnamestr += "\\*";

   WIN32_FIND_DATA ffd;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   LARGE_INTEGER filesize;

   hFind = FindFirstFile((LPCWSTR)s2ws(dirnamestr).c_str(), &ffd);
   if (INVALID_HANDLE_VALUE == hFind)
   {
      printf("Cannot open drectory: %s\n", dirname);
      return std::vector<std::string>();
   }

   std::vector<std::string> names;
   // List all the files in the directory with some info about them.
   do
   {
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         continue;
      }
      else
      {
         filesize.LowPart = ffd.nFileSizeLow;
         filesize.HighPart = ffd.nFileSizeHigh;
         std::string filename = ws2s(ffd.cFileName);
         printf("  %s   %ld bytes\n", filename.c_str(), filesize.QuadPart);
         if (filter.size() > 0 && filename.find(filter) != std::string::npos)
         {
            names.push_back(filename);
         }
      }
   } while (FindNextFile(hFind, &ffd) != 0);

   FindClose(hFind);
   return names;
}

#else
std::string PromptToLoad()
{
   OPENFILENAME ofn;
   char filename[1024];
   memset(filename, 0, sizeof(filename));

   memset(&ofn, 0, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.lpstrFilter = (LPCSTR)TEXT("BVH Files\0*.bvh\0All Files\0*.*\0\0");
   ofn.lpstrFile = filename;
   ofn.lpstrInitialDir = (LPCSTR) ""; // startdir.c_str();
   ofn.nMaxFile = 1024;
   ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

   BOOL result = GetOpenFileName(&ofn);
   if (result)
   {
      return filename;
   }
   else
   {
      DWORD err = CommDlgExtendedError();
      err = err;
   }
   return "";
}

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
   // If the BFFM_INITIALIZED message is received
   // set the path to the start path.
   switch (uMsg)
   {
   case BFFM_INITIALIZED:
   {
      if (NULL != lpData)
      {
         SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
      }
   }
   }
   return 0; // The function should always return 0.
}

std::string PromptToLoadDir()
{
   char cwd[1024];
   GetCurrentDirectory(1024, cwd);

   BROWSEINFO bi;
   TCHAR Buffer[MAX_PATH];
   ZeroMemory(Buffer, MAX_PATH);
   ZeroMemory(&bi, sizeof(bi));
   bi.hwndOwner = NULL;
   bi.pszDisplayName = Buffer;
   bi.lpszTitle = "";
   bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_SHAREABLE;
   bi.lpfn = BrowseCallbackProc;
   bi.lParam = (LPARAM)cwd;

   LPCITEMIDLIST pFolder = SHBrowseForFolder(&bi);
   if (pFolder)
   {
      SHGetPathFromIDList(pFolder, Buffer);
      std::string filename = Buffer;
      return filename;
   }
   else
   {
      DWORD err = CommDlgExtendedError();
      err = err;
   }
   return "";
}

std::vector<std::string> GetFilenamesInDir(const std::string &dirname, const std::string &filter)
{
   printf("Target directory is %s\n", dirname.c_str());
   std::string dirnamestr = dirname;
   dirnamestr += "\\*";

   WIN32_FIND_DATA ffd;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   LARGE_INTEGER filesize;

   hFind = FindFirstFile(dirnamestr.c_str(), &ffd);
   if (INVALID_HANDLE_VALUE == hFind)
   {
      printf("Cannot open drectory: %s\n", dirname.c_str());
      return std::vector<std::string>();
   }

   std::vector<std::string> names;
   // List all the files in the directory with some info about them.
   do
   {
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         continue;
      }
      else
      {
         filesize.LowPart = ffd.nFileSizeLow;
         filesize.HighPart = ffd.nFileSizeHigh;
         std::string filename = ffd.cFileName;
         // printf("  %s   %ld bytes\n", filename.c_str(), filesize.QuadPart);
         if (filter.size() > 0 && filename.find(filter) != std::string::npos)
         {
            names.push_back(filename);
         }
      }
   } while (FindNextFile(hFind, &ffd) != 0);

   FindClose(hFind);
   return names;
}

#endif

#endif
