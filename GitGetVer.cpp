/*
Copyright (c) 2017 Twilight Wings Studio

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <ctime>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#pragma warning(disable:4996)
#endif

enum EProgrammingLanguage
{
  EPL_CPP,
  EPL_JAVA,
  EPL_CSHARP
};

// Globals
bool _bAddDate = false;
bool _bCustomOutput = false;
EProgrammingLanguage _eLanguage = EPL_CPP;
std::string _strOutFile = "revision.h";
std::string _strPackageName = "";

// --------------------------------------------------------------------------------------
// Returns filename without exension.
// --------------------------------------------------------------------------------------
std::string removeExtension(const std::string& filename)
{
  unsigned int ulOffset = filename.find_last_of(".");
  if (ulOffset == std::string::npos) return filename;
  return filename.substr(0, ulOffset);
}

// --------------------------------------------------------------------------------------
// Generates content for C/C++ header file.
// --------------------------------------------------------------------------------------
std::string generateHeader(char const* rev_str)
{
  std::ostringstream newData;

  // Add include check.
  newData << "#ifndef __REVISION_H__" << std::endl;
  newData << "#define __REVISION_H__" << std::endl;
  newData << "#define REVISION_ID " << rev_str << std::endl;

  if (_bAddDate)
  {
    time_t t = time(0);
    struct tm *pTime = localtime(&t);

    newData << "#define REVISION_BUILD_YEAR " << (pTime->tm_year + 1900) << std::endl;
    newData << "#define REVISION_BUILD_MONTH " << pTime->tm_mon + 1 << std::endl;
    newData << "#define REVISION_BUILD_DAY " << pTime->tm_mday << std::endl;
  }

  newData << "#endif // __REVISION_H__" << std::endl;
  return newData.str();
}

// --------------------------------------------------------------------------------------
// Generates content for Java source file.
// --------------------------------------------------------------------------------------
std::string generateJava(char const* rev_str)
{
  std::ostringstream newData;
  std::string fileNameNoExt = removeExtension(_strOutFile);

  // If we have package name then print it there.
  if (_strPackageName != "") {
    newData << "package " << _strPackageName << ";" << std::endl;
    newData << std::endl;
  }

  newData << "public class " << fileNameNoExt << " {" << std::endl;

  newData << "  public static int REVISION_ID = " << rev_str << ";" << std::endl;

  if (_bAddDate)
  {
    time_t t = time(0);
    struct tm *pTime = localtime(&t);

    newData << std::endl;
    newData << "  public static int REVISION_BUILD_YEAR = " << (pTime->tm_year + 1900) << ";" << std::endl;
    newData << "  public static int REVISION_BUILD_MONTH = " << pTime->tm_mon + 1 << ";" << std::endl;
    newData << "  public static int REVISION_BUILD_DAY = " << pTime->tm_mday << ";" << std::endl;
  }

  newData << "}" << std::endl;

  return newData.str();
}

// --------------------------------------------------------------------------------------
// The entry point.
// --------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
  std::string strPath;

  // Cycle through arguments.
  for (int k = 1; k <= argc; ++k)
  {
    if (!argv[k] || !*argv[k])
      break;

    if (argv[k][0] != '-')
    {
      strPath = argv[k];
      if (strPath.size() > 0 && (strPath[strPath.size() - 1] != '/' || strPath[strPath.size() - 1] != '\\')) {
        strPath += '/';
      }

      break;
    }

    switch (argv[k][1])
    {
      case 'o': {
        if (++k == argc)
            return 1;
        _strOutFile = argv[k];

        _bCustomOutput = true;
        continue;
      } break;

      case 'd': {
        printf("[GitGetVer] Enabled build date info output.\n");
        _bAddDate = true;
        break;
      }

      case 'p': {
        if (++k == argc)
          return 1;
        _strPackageName = argv[k];

        continue;
      } break;

      case 'j': {
        _eLanguage = EPL_JAVA;

        if (!_bCustomOutput) {
          _strOutFile = "Revision.java";
        }
      } break;

      case 'h': {
        printf("\n");
        printf("[GitGetVer] - an open source version generator utility.\n");
        printf("\n");

        printf("Syntax: gitgetver [options]\n");
        printf("\n");
        printf("Available options are:\n");

        printf(" * -j\n");
        printf("   Switch language to Java.\n");

        printf("\n");

        printf(" * -d\n");
        printf("   Enable build date printout into file.\n");

        printf("\n");

        printf(" * -o <filename>\n");
        printf("   Output file name.\n");

        printf("\n");

        printf(" * -p <package>\n");
        printf("   Java package name which will be print into file.\n");

        printf("\n");

        return 0;

      } break;

      default: {
        printf("[GitGetVer] Error! Unknown option %s.\nUse \"-h\" or \"-help\" option to get list of available options!\n", argv[k]);
        return 1;
      }
    }
  }

  // Output language info.
  switch (_eLanguage)
  {
    case EPL_CPP:    printf("[GitGetVer] Selected Language: C++ (default)\n"); break;

    case EPL_JAVA: {
      printf("[GitGetVer] Selected Language: Java\n");
      printf("[GitGetVer] Java Package Name: %s\n", &_strPackageName);
    } break;

    case EPL_CSHARP: printf("[GitGetVer] Selected Language: C#\n"); break;
  }

  // New data extraction.
  std::string newData;

  FILE * pTempFile;

  char strBuffer[16];
  memset(&strBuffer[0], 0, 16);

  int iExitCode = system("git rev-list --count HEAD >> temp.txt");

  if (iExitCode != 0) {
    printf("[GitGetVer] Error! Got exit code %d from Git!\n", iExitCode);
    return 1;
  }

  pTempFile = fopen("temp.txt", "r");

  if (pTempFile == NULL) {
    printf("[GitGetVer] Failed to read output!\n");
    return 1;
  }

  fseek(pTempFile, 0, SEEK_END);
  unsigned long ulFileSize = ftell(pTempFile);
  fseek(pTempFile, 0, SEEK_SET);

  fread(&strBuffer[0], 1, 15, pTempFile);

  printf("[GitGetVer] Received output: %s\n", &strBuffer[0]);

  fclose(pTempFile);
  remove("temp.txt");

  long iRevNumber = atoi(&strBuffer[0]);

  if (iRevNumber < 1) {
    printf("[GitGetVer] Error! Failed to parse number from output!\n");
    return 1;
  }

  printf("[GitGetVer] Got revision number: %lu\n", iRevNumber);

  std::ostringstream strStream;
  strStream << iRevNumber;

  switch (_eLanguage)
  {
    case EPL_CPP: newData = generateHeader(strStream.str().c_str()); break;
    case EPL_JAVA: newData = generateJava(strStream.str().c_str()); break;
    case EPL_CSHARP: break;
  }

  // Get existed header data for compare.
  std::string oldData;
  FILE* pHeaderFile = fopen(_strOutFile.c_str(), "rb");
  
  if (pHeaderFile)
  {
    while (!feof(pHeaderFile))
    {
      int c = fgetc(pHeaderFile);
      if (c < 0)
        break;
      oldData += (char)c;
    }

    fclose(pHeaderFile);
  }

  // If datas are different then we need to replace old with new one.
  if (newData != oldData)
  {
    FILE* pOutputFile = fopen(_strOutFile.c_str(), "wb");

    if (pOutputFile)
    {
      fprintf(pOutputFile, "%s", newData.c_str());
      fclose(pOutputFile);
      printf("[GitGetVer] Successfully updated version info!\n");
    } else {
      printf("[GitGetVer] Failed to update version info. Access denied.\n");
      return 1;
    }

  } else {
    printf("[GitGetVer] Nothing to update.\n");
  }

  return 0;
}

