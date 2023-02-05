
#include <iostream>


class ParserEx
{
private: 
	const char* info;

public :
	ParserEx(const char * Info);

	const char* ToString(void);

};


	class Parser
	{
	public:
		enum TypeEvent : uint8_t
		{
			E = 0x45, 
			C = 0x43, 
			N = 0x4E,
			I = 0x49,
			S = 0x53,
			R = 0x52,
			L = 0x4C,
		};

	private:

#pragma pack(push, 1)
		struct Packet_R
		{
			uint8_t code;
			uint8_t number;
			TypeEvent typeEvent;
			uint16_t messCode;
			uint8_t CRC;
		};
#pragma pack(pop)

#pragma pack(push, 1)
		struct Packet_I
		{
			uint8_t code;
			uint8_t number;
			uint32_t date;
			TypeEvent typeEvent;
			uint32_t codeEvent;
			uint32_t value;
			uint8_t CRC;
		};
#pragma pack(pop)

#pragma pack(push, 1)
		struct Packet
		{
			uint8_t code;
			uint8_t number;
			uint32_t date;
			TypeEvent typeEvent;
			uint16_t codeEvent;
			uint32_t value;
			uint8_t CRC;
		};
#pragma pack(pop)

		uint8_t packetOutCounter;
		Packet packet;
		uint32_t time;
		uint8_t calcCrc(uint8_t* data, size_t len);

	public:
		Parser();
		~Parser();

		void SetTime(uint32_t Time);

		TypeEvent GetTypeEvent(void);
		uint16_t GetCodeEvent(void);
		uint32_t GetValue(void);

		void OutPacketR(Packet_R P);
		void OutPacketI(char* data);
		void OutPacketI(Packet_I P);
		void OutPacketI_End(void);

		void Parse(char* Data);
		
		void CodeTypeOut(void);
		void EventCodeOut(void);
		void OutTime(uint32_t Time);

		void OutPacketInfo(void);


	};
