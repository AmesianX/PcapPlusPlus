#define LOG_MODULE PacketLogModuleIPv4Layer

#include <IPv4Layer.h>
#include <IPv6Layer.h>
#include <PayloadLayer.h>
#include <UdpLayer.h>
#include <TcpLayer.h>
#include <IcmpLayer.h>
#include <GreLayer.h>
#include <string.h>
#include <IpUtils.h>

void IPv4Layer::initLayer()
{
	m_DataLen = sizeof(iphdr);
	m_Data = new uint8_t[m_DataLen];
	m_Protocol = IPv4;
	memset(m_Data, 0, sizeof(iphdr));
}

IPv4Layer::IPv4Layer()
{
	initLayer();
}

IPv4Layer::IPv4Layer(const IPv4Address& srcIP, const IPv4Address& dstIP)
{
	initLayer();
	iphdr* ipHdr = getIPv4Header();
	ipHdr->ipSrc = srcIP.toInt();
	ipHdr->ipDst = dstIP.toInt();
}

void IPv4Layer::parseNextLayer()
{
	if (m_DataLen <= sizeof(iphdr))
		return;

	iphdr* ipHdr = getIPv4Header();

	ProtocolType greVer = Unknown;

	uint8_t ipVersion = 0;

	switch (ipHdr->protocol)
	{
	case PACKETPP_IPPROTO_UDP:
		m_NextLayer = new UdpLayer(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr), this, m_Packet);
		break;
	case PACKETPP_IPPROTO_TCP:
		m_NextLayer = new TcpLayer(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr), this, m_Packet);
		break;
	case PACKETPP_IPPROTO_ICMP:
		m_NextLayer = new IcmpLayer(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr), this, m_Packet);
		break;
	case PACKETPP_IPPROTO_IPIP:
		ipVersion = *(m_Data + sizeof(iphdr));
		if (ipVersion >> 4 == 4)
			m_NextLayer = new IPv4Layer(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr), this, m_Packet);
		else if (ipVersion >> 4 == 6)
			m_NextLayer = new IPv6Layer(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr), this, m_Packet);
		else
			m_NextLayer = new PayloadLayer(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr), this, m_Packet);
		break;
	case PACKETPP_IPPROTO_GRE:
		greVer = GreLayer::getGREVersion(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr));
		if (greVer == GREv0)
			m_NextLayer = new GREv0Layer(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr), this, m_Packet);
		else if (greVer == GREv1)
			m_NextLayer = new GREv1Layer(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr), this, m_Packet);
		else
			m_NextLayer = new PayloadLayer(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr), this, m_Packet);
		break;
	default:
		m_NextLayer = new PayloadLayer(m_Data + sizeof(iphdr), m_DataLen - sizeof(iphdr), this, m_Packet);
		return;
	}
}

void IPv4Layer::computeCalculateFields()
{
	iphdr* ipHdr = getIPv4Header();
	ipHdr->internetHeaderLength = (5 & 0xf);
	ipHdr->ipVersion = (4 & 0x0f);
	ipHdr->totalLength = htons(m_DataLen);
	ipHdr->headerChecksum = 0;

	if (m_NextLayer != NULL)
	{
		switch (m_NextLayer->getProtocol())
		{
		case TCP:
			ipHdr->protocol = PACKETPP_IPPROTO_TCP;
			break;
		case UDP:
			ipHdr->protocol = PACKETPP_IPPROTO_UDP;
			break;
		case ICMP:
			ipHdr->protocol = PACKETPP_IPPROTO_ICMP;
			break;
		case GREv0:
		case GREv1:
			ipHdr->protocol = PACKETPP_IPPROTO_GRE;
			break;
		default:
			break;
		}
	}

	ScalarBuffer scalar = { (uint16_t*)ipHdr, ipHdr->internetHeaderLength*4 } ;
	ipHdr->headerChecksum = htons(compute_checksum(&scalar, 1));
}

std::string IPv4Layer::toString()
{
	return "IPv4 Layer, Src: " + getSrcIpAddress().toString() + ", Dst: " + getDstIpAddress().toString();
}
