/*
 * pkt_base.cpp
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#include "pkt_base.h"
#include "pkt_definition.h"

PktBase::PktBase()
{
	m_incoming_buf = NULL;
	m_incoming_len = 0;

	m_pkt_header.version = PKT_VERSION;
	m_pkt_header.flag = 0;
	m_pkt_header.pkt_id = 0;
    m_pkt_header.seq_no = 0;
}

uchar_t* PktBase::GetBuffer()
{
	if (m_incoming_buf)
		return m_incoming_buf;
	else
		return m_buf.GetBuffer();
}

uint32_t PktBase::GetLength()
{
	if (m_incoming_buf)
		return m_incoming_len;
	else
		return m_buf.GetWriteOffset();
}

void PktBase::WriteHeader()
{
	uchar_t* buf = GetBuffer();

	CByteStream::WriteInt32(buf, GetLength());
	CByteStream::WriteUint16(buf + 4, m_pkt_header.version);
	CByteStream::WriteUint16(buf + 6, m_pkt_header.flag);
	CByteStream::WriteUint32(buf + 8, m_pkt_header.pkt_id);
    CByteStream::WriteUint32(buf + 12, m_pkt_header.seq_no);
}

void PktBase::SetVersion(uint16_t version)
{
	uchar_t* buf = GetBuffer();
	CByteStream::WriteUint16(buf + 4, version);
    m_pkt_header.version = version;
}

void PktBase::SetFlag(uint16_t flag)
{
	uchar_t* buf = GetBuffer();
	CByteStream::WriteUint16(buf + 6, flag);
    m_pkt_header.flag = flag;
}

void PktBase::SetSeqNo(uint32_t seq_no)
{
    uchar_t* buf = GetBuffer();
	CByteStream::WriteUint32(buf + 12, seq_no);
    m_pkt_header.seq_no = seq_no;
}

int PktBase::ReadPktHeader(uchar_t* buf, uint32_t len, pkt_header_t* header)
{
	int ret = -1;
	if (len >= PKT_HEADER_LEN && buf && header) {
		CByteStream is(buf, len);

		is >> header->length;
		is >> header->version;
		is >> header->flag;
		is >> header->pkt_id;
        is >> header->seq_no;

		ret = 0;
	}

	return ret;
}

PktBase* PktBase::ReadPacket(uchar_t *buf, uint32_t len)
{
	uint32_t pkt_len = 0;
	if (!IsPktAvailable(buf, len, pkt_len))
		return NULL;

	uint32_t pkt_id = CByteStream::ReadUint32(buf + 8);
	PktBase* pPkt = NULL;

	switch (pkt_id) {
        case PKT_ID_HEARTBEAT:
            pPkt = new PktHeartBeat(buf, pkt_len);
            break;
        default:
            throw PktException(ERROR_CODE_UNKNOWN_PKT_ID, "unknown pkt_id");
	}

	pPkt->_SetIncomingLen(pkt_len);
	pPkt->_SetIncomingBuf(buf);
	return pPkt;
}

bool PktBase::IsPktAvailable(uchar_t* buf, uint32_t len, uint32_t& pkt_len)
{
    if (len < PKT_HEADER_LEN)
		return false;

	pkt_len = CByteStream::ReadUint32(buf);
    
    if (pkt_len < PKT_HEADER_LEN)
        throw PktException(ERROR_CODE_WRONG_PKT_LEN, "wrong pkt length");
    
    if (pkt_len > len)
		return false;

	return true;
}

