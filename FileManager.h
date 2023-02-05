
#include <iostream>
#include <fstream> // подключаем библиотеку

class FileManager
{
private:
	char* fileName;
	std::ifstream file;
	char _p[34];

public:

	FileManager(char* FileName);
	~FileManager();

	void DataToFile(char *Data);
	int GetCountLines(void);

	char* GetLine(char* StartLine, uint32_t Counter);


};