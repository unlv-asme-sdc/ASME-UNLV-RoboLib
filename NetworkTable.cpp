#include "NetworkTable.h"
#define NETWORK_CMD_HELLOWORLD 0
#define NETWORK_CMD_FLUSHBYTES 1
#define NETWORK_CMD_PS2DATA 2
#define NETWORK_CMD_LOGBYTE 3
#define NETWORK_CMD_LOGFLOAT 4
#define NETWORK_CMD_LOGINT 5
#define NETWORK_CMD_SEND_FLOATS 6
#define NETWORK_CMD_SEND_INTEGRALS 7

#define PACKET_COMMAND 2
#define PACKET_HASHKEY 1
#define PACKET_BYTECOUNT 0

#define HMAP_HELLOWORLD 0 // first index of byteMap is reserved for debugging.

/** \brief Here we construct the NetworkTable.
 *
 * \param byteSize    - This is the size of the NetworkTable byte array.
 * \param floatSize   - This is the size of the NetworkTable float array.
 */
NetworkTable::NetworkTable(int byteSize, int floatSize)
{
	this->byteMap = new byte[byteSize];
	byteMapSize = byteSize;
}

/** \brief This is the function that is binded to 'PacketSerial', it processes the packets.
 *
 * It accepts the packets from PacketSerial, and processes them.
 *
 * \param sender   - This is a Reference to the serial object that's receiving the packets.
 * \param buffer   - This is a Reference to the buffer, where packets are stored.
 * \param size     - This is the Size of a packet, in bytes (i.e. 'size = 6' means 6 bytes).
 */
void NetworkTable::processPacketFromSender(const PacketSerial& sender, const uint8_t* buffer, size_t size)
{
	Serial.println("Packet Recieved");
	Serial.println(buffer[2]);
	if(size < 3)
	{
		return;
	}

	switch(buffer[2])
	{
		case NETWORK_CMD_LOGFLOAT:
			Serial.println(millis());
			Serial.println(",");
			Serial.println(*((float*)(buffer + 3)));
			break;
		case NETWORK_CMD_LOGBYTE:
			Serial.println(millis());
			Serial.println(",");
			Serial.println(buffer[3]);
			break;
		case NETWORK_CMD_LOGINT:
			Serial.println(millis());
			Serial.println(",");
			Serial.println(*((int*)(buffer+3)));
			break;
		case NETWORK_CMD_FLUSHBYTES:
			for(byte i = 0; (i < byteMapSize) && (i < byteMapSize); i++)
			{
				byteMap[i] = buffer[i+3];
			}
			break;
		case NETWORK_CMD_HELLOWORLD:
			byteMap[HMAP_HELLOWORLD] = buffer[3];
			break;
		case NETWORK_CMD_PS2DATA:
			for(byte i = 0; i < 21; i++)
			{
				ps2x->PS2data[i] = buffer[i+3];
				//Serial.println(buffer[i+3], HEX);
			}
			//Serial.println();
			ps2x->read_gamepad();
			time_lastps2packet = millis();
			break;
		case NETWORK_CMD_SEND_FLOATS:
			Serial.println("RECIEVED FLOAT COMMAND");
			processFloatPairs(buffer[size-1], &buffer[3]);
			break;
	}

}

/** \brief This sets a byte's value in the NetworkTable byte-map.
 *
 * This function only sets the value. It does not update the Tables on other devices.
 * \param key     - This defines which table index is the one that should be changed.
 * \param value   - This is the value the index should have.
 */
void NetworkTable::setByte(byte key, byte value)
{
	this->byteMap[key] = value;
}

/** \brief This returns the time since the NetworkTable last received any ps2 data.
 *
 * This function is to be used for watchdog protocols.
 * \return Time   - The amount of time since the last ps2 packet has been received, in milliseconds.
 */
unsigned long NetworkTable::getLastPS2PacketTime()
{
	return millis() - time_lastps2packet;
}

/** \brief Here we retrieve a value from the byte table.
 *
 * \param key   - The index to retrieve.
 * \return  The unsigned char of index.
 */
byte NetworkTable::getByte(byte key)
{
	return this->byteMap[key];
}

/** \brief This sends the entire byte-map through the network.
 * \warning !! Do not send packets concurrently !!
 * \param sender   - This is a reference to the PacketSerial that should be used to send the packet.
 */
void NetworkTable::flushBytes(PacketSerial* sender)
{
	packetBuffer[0] = 3 + byteMapSize;
	packetBuffer[1] = 0;
	packetBuffer[2] = NETWORK_CMD_FLUSHBYTES;
	for(byte i = 0; i < byteMapSize; i++)
	{
		packetBuffer[i+3] = byteMap[i];
	}
	sender->send(packetBuffer, packetBuffer[0]);
}

/** \brief Used to test connectivity.
 */
void NetworkTable::helloWorld(PacketSerial* sender, byte value)
{
	packetBuffer[0] = 3 + 1;
	packetBuffer[1] = 0;
	packetBuffer[2] = NETWORK_CMD_HELLOWORLD;
	packetBuffer[3] = value;
	sender->send(packetBuffer, packetBuffer[0]);
}

/** \brief This pushes the ps2data out across the network.
 *
 * This function is called on by the driver station. A good rate to execute this function is at 20Hz.
 * \param sender   - This is a reference to the PacketSerial that should be used to send the packet.
 */
void NetworkTable::sendPS2Data(PacketSerial* sender)
{
	packetBuffer[0] = 3 + 21;
	packetBuffer[1] = 0;
	packetBuffer[2] = NETWORK_CMD_PS2DATA;
	for(byte i = 0; i<21; i++)
	{
		packetBuffer[i+3] = ps2x->PS2data[i];

		//Serial.println(ps2x->PS2data[i], HEX);
	}
	//Serial.println();
	sender->send(packetBuffer, packetBuffer[0]);
}

/** \brief This binds a PS2X object to the NetworkTable.
 *
 * When calling sendPS2Data(PacketSerial* sender), the NetworkTable will copy data from the assigned ps2
 * controller and send it over the network.
 *
 * \param ps2x   - A PS2X object that should be used to control the robot. Please refer to the PS2X file for further documentation.
 */
void NetworkTable::setPS2(PS2X &ps2x)
{
	this->ps2x = &ps2x;
}

/** \brief Prints byte String Data on DriverStation
 *
 * \param value - Byte Datatype to print on driver station.
 * \warning Experimental.
 */
void NetworkTable::logByte(byte value, PacketSerial* sender)
{
	packetBuffer[0] = 3 + 1;
	packetBuffer[1] = 0;
	packetBuffer[2] = NETWORK_CMD_LOGBYTE;
	packetBuffer[3] = value;
	sender->send(packetBuffer, packetBuffer[0]);
}

/** \brief Prints Long String Data on DriverStation
 *
 * \param value - Long Datatype to print on driver station.
 * \warning Experimental.
 */
void NetworkTable::logFloat(float value, PacketSerial* sender)
{
	packetBuffer[0] = 3 + 4;
	packetBuffer[1] = 0;
	packetBuffer[2] = NETWORK_CMD_LOGFLOAT;
	packetBuffer[3] = *((byte*)&value);
	packetBuffer[4] = *(((byte*)&value) + 1);
	packetBuffer[5] = *(((byte*)&value) + 2);
	packetBuffer[6] = *(((byte*)&value) + 3);
	sender->send(packetBuffer, packetBuffer[0]);
}

void NetworkTable::logInt(int value, PacketSerial* sender)
{
	packetBuffer[0] = 3 + 2;
	packetBuffer[1] = 0;
	packetBuffer[2] = NETWORK_CMD_LOGINT;
	packetBuffer[3] = *((byte*)&value);
	packetBuffer[4] = *(((byte*)&value) + 1);
	sender->send(packetBuffer, packetBuffer[0]);
}

void NetworkTable::putBufferByte(const unsigned char & byte)
{
	packetBuffer[bufferIndex] = byte;
	bufferIndex++;
}


void NetworkTable::putBufferBytes(const unsigned char numberOfBytes, const unsigned char * bytes)
{
	for(unsigned char i = 0; i < numberOfBytes; i++)
	{
		putBufferByte(*(bytes + i));
	}
}

void NetworkTable::putBufferInteger(const int& integral)
{
	putBufferBytes(sizeof(int), (unsigned char*)(&integral));
}

void NetworkTable::putBufferFloat(const float& number)
{
	putBufferBytes(sizeof(float), (unsigned char*)(&number));
}

void NetworkTable::sendBuffer(PacketSerial* sender, const unsigned char networkCommand)
{
	putBufferByte(valuePairs);
	packetBuffer[2] = networkCommand;
	packetBuffer[0] = bufferIndex;
	// Consider putting Hash Generator Module here.
	sender->send(packetBuffer, packetBuffer[0]);
	// Reset Counters
	bufferIndex = 3;
	valuePairs = 0;
}

void NetworkTable::putBufferPair(const unsigned char& index, const float& value)
{
	putBufferByte(index);
	putBufferFloat(value);
	valuePairs++;
}

void NetworkTable::putBufferPair(const unsigned char& index, const int& integral)
{
	putBufferByte(index);
	putBufferInteger(integral);
	valuePairs++;
}

void NetworkTable::putBufferPair(const unsigned char& index, const unsigned char& character)
{
	putBufferByte(index);
	putBufferByte(character);
	valuePairs++;
}

void NetworkTable::setFloatMap(float floatMap[], const unsigned char floatMapSize)
{
	this->floatMap = &(floatMap[0]);
	this->floatMapSize = floatMapSize;
}


void NetworkTable::processFloatPairs(unsigned char numberOfPairs, const unsigned char* buffer)
{
	// Stop processing if float map is uninitialized.
	if(!floatMapSize)
		return;

	// Process floats into float map.
	for(unsigned char i = 0; i < numberOfPairs; i++)
	{
		unsigned char index = buffer[0 + (1 + sizeof(float))*i];
		if(index > floatMapSize)
			continue;
		floatMap[index] = *((float*)(&buffer[1 + (sizeof(float) + 1)*i]));
	}
}



void NetworkTable::printFloatMap()
{
	//clear putty
	Serial.write(27);
	Serial.print("[2J");
	Serial.write(27);
	Serial.print("[H");
	if (floatMapSize > 0){
		for (int i = 0; i < floatMapSize; i++){
			Serial.print("Float value at index "); Serial.print(i); Serial.print(" is "); Serial.println(floatMap[i]);
		}
	}
}
