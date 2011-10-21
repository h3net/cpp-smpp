/*
 * Copyright (C) 2011 OnlineCity
 * Licensed under the MIT license, which can be read at: http://www.opensource.org/licenses/mit-license.php
 * @author hd@onlinecity.dk & td@onlinecity.dk
 */

#ifndef PDU_H_
#define PDU_H_
#include <boost/numeric/conversion/cast.hpp>
#include "smpp.h"
#include "tlv.h"
#include <stdint.h> // types
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <boost/shared_array.hpp>
#include <netinet/in.h>

namespace smpp {

uint32_t getPduLength(boost::shared_array<uint8_t> pduHeader);
const int HEADERFIELD_SIZE = 4;
const int HEADER_SIZE = HEADERFIELD_SIZE * 4;

/**
 * Class for representing a PDU.
 */
class PDU
{
private:
	std::stringbuf sb;
	std::iostream buf;
	uint32_t cmdId;
	uint32_t cmdStatus;
	uint32_t seqNo;
	bool nullTerminateOctetStrings;
public:
	bool null;

public:

	PDU() :
			sb(""), buf(&sb), cmdId(0), cmdStatus(0), seqNo(0), nullTerminateOctetStrings(true), null(true)
	{
	}

	PDU(const uint32_t &_cmdId, const uint32_t &_cmdStatus, const uint32_t &_seqNo) :
					sb(""),
					buf(&sb),
					cmdId(_cmdId),
					cmdStatus(_cmdStatus),
					seqNo(_seqNo),
					nullTerminateOctetStrings(true),
					null(false)
	{
		(*this) << uint32_t(0);
		(*this) << cmdId;
		(*this) << cmdStatus;
		(*this) << seqNo;
	}

	PDU(const boost::shared_array<uint8_t> &pduLength, const boost::shared_array<uint8_t> &pduBuffer) :
			sb(""), buf(&sb), cmdId(0), cmdStatus(0), seqNo(0), nullTerminateOctetStrings(true), null(false)
	{
		uint32_t bufSize = getPduLength(pduLength);

		buf.write(reinterpret_cast<char*>(pduLength.get()), HEADERFIELD_SIZE);
		buf.write(reinterpret_cast<char*>(pduBuffer.get()), bufSize - HEADERFIELD_SIZE);

		buf.seekg(HEADERFIELD_SIZE, std::ios::cur);

		(*this) >> cmdId;
		(*this) >> cmdStatus;
		(*this) >> seqNo;
	}

	PDU(const PDU &rhs) :
					sb(rhs.sb.str()),
					buf(&sb),
					cmdId(rhs.cmdId),
					cmdStatus(rhs.cmdStatus),
					seqNo(rhs.seqNo),
					nullTerminateOctetStrings(rhs.nullTerminateOctetStrings),
					null(rhs.null)
	{
		resetMarker(); // remember to reset the marker after copying.
	}

	/**
	 * @return All data in this PDU as array of unsigned char array.
	 */
	const boost::shared_array<uint8_t> getOctets();

	/**
	 * @return PDU size in octets.
	 */
	int getSize();

	/**
	 * @return PDU command id.
	 */
	uint32_t getCommandId() const;

	/**
	 * @return PDU command status.
	 */
	uint32_t getCommandStatus() const;

	/**
	 * @return PDU sequence number.
	 */
	uint32_t getSequenceNo() const;

	/**
	 * @return True if null termination is on.
	 */
	bool isNullTerminating() const;

	/**
	 * Turns null termination on or off.
	 * @param True if null termination is on.
	 */
	void setNullTerminateOctetStrings(const bool&);

	/** Adds an integer as an unsigned 8 bit. */
	PDU& operator<<(const int &);
	PDU& operator<<(const uint8_t &i);
	PDU& operator<<(const uint16_t &i);
	PDU& operator<<(const uint32_t &i);
	PDU& operator<<(const std::basic_string<char> &s);
	PDU& operator<<(const smpp::SmppAddress);
	PDU& operator<<(const smpp::TLV);
	PDU& addOctets(const boost::shared_array<uint8_t> &octets, const std::streamsize &len);

	/**
	 * Skips n octets.
	 * @param n Octets to skip.
	 */
	void skip(int n);

	/**
	 * Resets the read marker to the beginning of the PDU.
	 */
	void resetMarker();

	PDU& operator>>(int &);
	PDU& operator>>(uint8_t &i);
	PDU& operator>>(uint16_t &i);
	PDU& operator>>(uint32_t &i);
	PDU& operator>>(std::basic_string<char> &s);

	/**
	 * Copy n octet into an array.
	 * @param array Target array.
	 * @param n Octets to copy.
	 */
	void readOctets(boost::shared_array<uint8_t> &octets, const std::streamsize &n);

	/**
	 * @return True if the read marker is not at the end of the PDU.
	 */
	bool hasMoreData();

	friend std::ostream &smpp::operator<<(std::ostream&, smpp::PDU&);
};
// PDU
std::ostream& operator<<(std::ostream&, smpp::PDU&);
} // namespace smpp

#endif /* PDU_H_ */
