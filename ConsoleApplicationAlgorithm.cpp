//
//

#include <iostream>
#include <cstring>
#include "Parser.h"
#include "FileManager.h"
#include <chrono>


std::string outInfoNewPacket = "Получен новый пакет: ";
std::string outInfoInPacket = "Содержимое нового пакета : ";

char packet_m[1024];

Parser parser;
//FileManager file((char *)"D:/logs.txt");
FileManager file((char *)"logs.txt");


int main()
{
    setlocale(LC_ALL, "Russian");
    auto now = std::chrono::system_clock::now().time_since_epoch();
    parser.SetTime((uint32_t)now.count());

    std::cout << "Система логирования запущена" << std::endl << std::endl;

    while (true)
    {
        std::string dataIn = "";

        std::cout << "Ожидание пакета: ";
        std::cin >> dataIn;

        std::cout << std::endl << outInfoNewPacket << dataIn << std::endl;


        //strcpy_s(packet_m, dataIn.c_str());
        strcpy(packet_m, dataIn.c_str());

        try
        {
            parser.Parse(packet_m);
        }
        catch (ParserEx* ex)
        {
            if (ex != NULL)
            {
                continue;
            }
        }
        catch (const char* msg)
        {
            std::cout << "Не удалось разобрать пакет..." << std::endl;
            std::cout << msg << std::endl;
            continue;
        }

        parser.OutPacketInfo();

        Parser::TypeEvent _e = parser.GetTypeEvent();

        if (_e == Parser::TypeEvent::L)
        {
            std::cout << std::endl << "Поиск информации..." << std::endl << std::endl;

            char code[4];
            char *packet;
            uint8_t _c = (uint8_t)parser.GetCodeEvent();
            uint32_t countPacket = 0;
            //sprintf_s(code, "%02X", _c);
            sprintf(code, "%02X", _c);
           

            while (1)
            {
                packet = file.GetLine(code, countPacket);
                countPacket++;

                if (packet != NULL)
                {
                    parser.OutPacketI(packet);
                }
                else
                {
                    parser.OutPacketI_End();
                    break;
                }
            }
        }
        else if ((_e == Parser::TypeEvent::E) || 
            (_e == Parser::TypeEvent::C) || 
            (_e == Parser::TypeEvent::N))
        {
            file.DataToFile(packet_m);
            int _c = file.GetCountLines();
            std::cout << "Событие сохранено в лог под номером #" << (unsigned)_c << std::endl << std::endl;
        }
        else if (_e == Parser::TypeEvent::S)
        {
            auto now = std::chrono::system_clock::now().time_since_epoch();

            if (parser.GetCodeEvent() == 1)
            {
                if ((uint32_t)now.count() < parser.GetValue())
                {
                    std::cout << "Ошибка установки времени" << std::endl;
                }
                else
                {
                    parser.SetTime(parser.GetValue());
                    std::cout << "Команда выполнена" << std::endl;
                }
            }
            if (parser.GetCodeEvent() == 2)
            {
                parser.SetTime((uint32_t)now.count());
                std::cout << "Команда выполнена" << std::endl;
            }
        }
        else
        {
            std::cout << "Пакет не обработан!" << std::endl;
        }



    }

}

