


#include "Parser.h"
#include <cstring>
#include <iomanip>
#include <chrono>


ParserEx::ParserEx(const char* Info)
{
	info = Info;
}

const char* ParserEx::ToString(void)
{
	return info;
}


Parser::Parser()
{
	packetOutCounter = 0x00;
}

Parser::~Parser()
{

}

void Parser::SetTime(uint32_t Time)
{
	time = Time;
}

Parser::TypeEvent Parser::GetTypeEvent(void)
{
	return packet.typeEvent;
}

uint16_t Parser::GetCodeEvent(void)
{
	return packet.codeEvent;
}

uint32_t Parser::GetValue(void)
{
	return packet.value;
}

void Parser::OutPacketI_End(void)
{
	Packet_I P;
	P.codeEvent = 0x00;
	P.value = 0x00;
	OutPacketI(P);
}

void Parser::OutPacketI(char* data)
{
	Packet_I _pI;

	Parse(data);

	_pI.codeEvent = (packet.code << 24);
	_pI.codeEvent += (packet.typeEvent << 16);
	_pI.codeEvent += packet.codeEvent;

	_pI.value = packet.value;
	OutPacketI(_pI);
}

void Parser::OutPacketI(Packet_I P)
{
	auto now = std::chrono::system_clock::now().time_since_epoch();

	packetOutCounter++;

	P.code = 0xFF;
	P.number = packetOutCounter;
	P.date = (uint32_t)(now.count() - time);
	P.typeEvent = L;
	P.CRC = calcCrc((uint8_t*)&P, sizeof(Packet_I) - 1);

	std::cout << "Сформирован информационный пакет ";
	printf("%X: " ,packetOutCounter);

	printf("%02X;%02X;%08X;%02X;%08X;%08X;%02X", 
		P.code, P.number, P.date, P.typeEvent, P.codeEvent, P.value, P.CRC);
	std::cout << std::endl;

	std::cout << "Пакет содержит:" << std::endl;
	OutPacketInfo();

	std::cout << "Информационный пакет ";
	std::cout << (unsigned)packetOutCounter;
	std::cout << " отправлен";
	std::cout << std::endl << std::endl;
}

void Parser::OutPacketR(Packet_R P)
{
	packetOutCounter++;
	P.number = packetOutCounter;
	P.typeEvent = R;
	P.code = 0xFF;
	P.CRC = calcCrc((uint8_t*)&P, sizeof(Packet_R) - 1);

	std::cout << std::hex << std::uppercase << (unsigned)P.code << ";";
	std::cout << std::hex << std::uppercase << (unsigned)P.number << ";";
	std::cout << "" << ";";
	std::cout << std::hex << std::uppercase << (unsigned)P.typeEvent << ";";
	std::cout << std::hex << std::uppercase << (unsigned)P.messCode << ";";
	std::cout << "" << ";";
	std::cout << std::hex << std::uppercase << (unsigned)P.CRC;
	std::cout << std::endl;
}

void Parser::Parse(char* Data)
{
	size_t lenP = strlen(Data);

	if ((lenP != 34) && (lenP != 18))
	{
		std::cout << "Неправильная длина пакета" << std::endl;
		throw new ParserEx("Неправильная длина пакета");
	}

	char* pEnd;

	packet.code = (uint8_t)strtoul(&Data[0], &pEnd, 16);
	packet.number = (uint8_t)strtoul((pEnd + 1), &pEnd, 16);
	packet.date = (uint32_t)strtoul((pEnd + 1), &pEnd, 16);

	packet.typeEvent = (TypeEvent)strtoul((pEnd + 1), &pEnd, 16);
	packet.codeEvent = (uint16_t)strtoul((pEnd + 1), &pEnd, 16);
	packet.value = (uint32_t)strtoul((pEnd + 1), &pEnd, 16);
	packet.CRC = (uint8_t)strtoul((pEnd + 1), &pEnd, 16);

	uint8_t _Crc = calcCrc((uint8_t*)&packet, sizeof(packet) - 1);

	if ((packet.CRC != _Crc) && (packet.number != packet.CRC))
	{
		std::cout << "Ошибка CRC полученного пакета" << std::endl;
		std::cout << "Сформирован запрос повторения ошибочного пакета: ";

		Packet_R _pCrc;
		_pCrc.messCode = (packet.code << 8) + packet.number;
		OutPacketR(_pCrc);

		std::cout << "Запрос повтора отправлен" << std::endl << std::endl;

		throw new ParserEx("Ошибка CRC принятого пакета");
	}
}

uint8_t Parser::calcCrc(uint8_t* data, size_t len)
{
	uint32_t crc = 0;

	for (int i = 0; i < len; i++)
	{
		crc += data[i];
	}

	return (uint8_t)(crc % 256);
}



void Parser::OutPacketInfo(void)
{
	CodeTypeOut();
	std::cout << "  Номер события: ";
	std::cout << (unsigned)packet.number;
	std::cout << std::endl;

	OutTime(packet.date);

	EventCodeOut();

	std::cout << "  Код события: ";
	std::cout << (unsigned)packet.codeEvent;
	std::cout << std::endl;

	std::cout << "  Значение: ";
	std::cout << std::setprecision(4) << (float)packet.value;
	std::cout << std::endl;

	std::cout << "  Контрольная сумма: ";
	std::cout << (unsigned)packet.CRC;
	std::cout << std::endl;

}


void Parser::CodeTypeOut(void)
{
	std::cout << "  Подсистема-отправитель: ";

	switch (packet.code)
	{
	case 0x00:
		std::cout << "MPU";
		break;

	case 0x01:
		std::cout << "Engine 1";
		break;

	case 0x02:
		std::cout << "Engine 2";
		break;

	case 0x03:
		std::cout << "Engine 3";
		break;

	case 0x04:
		std::cout << "Engine 4";
		break;

	case 0x10:
		std::cout << "Brakes";
		break;

	case 0x11:
		std::cout << "Brakes";
		break;

	case 0x20:
		std::cout << "Battery Management Subsystem";
		break;

	case 0x30:
		std::cout << "Fuel Subsystem";
		break;

	case 0xFF:
		std::cout << "Logging Subsystem";
		break;

	default:
		std::cout << "Undef code";
		break;
	}

	std::cout << std::endl;
}

void Parser::EventCodeOut(void)
{
	std::cout << "  Тип события: ";

	switch (packet.typeEvent)
	{
	case E:
		std::cout << "Error";
		break;

	case C:
		std::cout << "Critical";
		break;

	case N:
		std::cout << "Normal";
		break;

	case I:
		std::cout << "Info";
		break;

	case S:
		std::cout << "System";
		break;

	case R:
		std::cout << "Repeat";
		break;

	case L:
		std::cout << "List";
		break;

	default:
		std::cout << "Undef code";
		break;
	}

	std::cout << std::endl;
}

void Parser::OutTime(uint32_t Time)
{
	struct tm timeinfo;
	time_t now = Time;
	struct tm* timeI;
	timeI = localtime(&now);

	std::cout << "  Время создания: ";
	std::cout << (unsigned)timeI->tm_hour << "ч:";
	std::cout << (unsigned)timeI->tm_min << "м:";
	std::cout << (unsigned)timeI->tm_sec << "с";
	std::cout << std::endl;

	//struct tm timeinfo;
	//time_t now = Time;
	//localtime_s(&timeinfo, &now);

	//std::cout << "Время: ";
	//std::cout << (unsigned)timeinfo.tm_hour << "ч:";
	//std::cout << (unsigned)timeinfo.tm_min << "м:";
	//std::cout << (unsigned)timeinfo.tm_sec << "с";
	//std::cout << std::endl;
}



