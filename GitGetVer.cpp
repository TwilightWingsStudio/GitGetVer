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

#include <sstream>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#pragma warning(disable:4996)
#endif

std::string generateHeader(char const* rev_str)
{
  std::ostringstream newData;
  newData << "#ifndef __REVISION_H__" << std::endl;
  newData << "#define __REVISION_H__" << std::endl;
  newData << "#define REVISION_ID " << rev_str << std::endl;
  newData << "#endif // __REVISION_H__" << std::endl;
  return newData.str();
}

// --------------------------------------------------------------------------------------
// The entry point.
// --------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
  std::string strPath;
  std::string strOutFile = "revision.h";

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
      case 'o':
        if (++k == argc)
          return 1;
        strOutFile = argv[k];
        continue;

      default:
        printf("Unknown option %s", argv[k]);
        return 1;
    }
  }

  // New data extraction.
  std::string newData;

  bool bHasResult = false;

  FILE * pTempFile;
    
  char strBuffer[16];
  memset(&strBuffer[0], 0, 16);

  system("git rev-list --count HEAD >> temp.txt");
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

  memset(&strBuffer[0], 0, 16);
  itoa(iRevNumber, &strBuffer[0], 10);

  newData = generateHeader(&strBuffer[0]);

  // Get existed header data for compare.
  std::string oldData;

  if (FILE* HeaderFile = fopen(strOutFile.c_str(), "rb"))
  {
    while (!feof(HeaderFile))
    {
      int c = fgetc(HeaderFile);
      if (c < 0)
        break;
      oldData += (char)c;
    }

    fclose(HeaderFile);
  }

  // If datas are different then we need to replace old with new one.
  if (newData != oldData)
  {
    FILE* pOutputFile = fopen(strOutFile.c_str(), "wb");

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

