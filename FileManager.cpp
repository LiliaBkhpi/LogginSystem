

#include "FileManager.h"
#include <stdlib.h>
#include <fstream>
#include <cstring>

using namespace std;


FileManager::FileManager(char* FileName)
{
	fileName = FileName;
}

FileManager::~FileManager()
{
	if (!file)
	{
		file.close();
	}
}

void FileManager::DataToFile(char* Data)
{
	ofstream file;
	file.open(fileName, std::ios::app);

	file << Data << endl;

	file.close();
}

int FileManager::GetCountLines(void)
{
	int counterLines = 0;

	ifstream is(fileName); // открыть файл

	if (!is)
		return 0;

	char str[100]; // буфер для чтения одной строки

	while (is) // если is ненулевое, то еще не конец файла
	{
		is.getline(str, 100); // прочитать строку из файла в буфер str
		counterLines++;
	}
	is.close(); // закрыть файл

	return counterLines - 1;
}

char* FileManager::GetLine(char* StartLine, uint32_t Counter)
{
	int counterLines = 0;

	ifstream is(fileName); // открыть файл

	if (!is)
		return 0;

	char str[100]; // буфер для чтения одной строки

	while (is) // если is ненулевое, то еще не конец файла
	{
		is.getline(str, 100); // прочитать строку из файла в буфер str
		
		char* pch;
		pch = strstr(str, StartLine);

		if (pch == str)
		{
			if (counterLines == Counter)
			{
				memcpy(_p, str, 34);
				return _p;
			}
			counterLines++;
		}
	}
	is.close(); // закрыть файл

	return NULL;
}