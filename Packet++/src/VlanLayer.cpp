#define LOG_MODULE PacketLogModuleVlanLayer

#include <VlanLayer.h>
#include <IPv4Layer.h>
#include <IPv6Layer.h>
#include <PayloadLayer.h>
#include <ArpLayer.h>
#include <PPPoELayer.h>
#include <MplsLayer.h>
#include <PacketUtils.h>
#include <string.h>
#include <sstream>
#ifdef WIN32
#include <winsock2.h>
#elif LINUX
#include <in.h>
#endif


VlanLayer::VlanLayer(const uint16_t vlanID, bool cfi, uint8_t priority, uint16_t etherType)
{
	m_DataLen = sizeof(vlan_header);
	m_Data = new uint8_t[m_DataLen];
	memset(m_Data, 0, sizeof(m_DataLen));
	m_Protocol = VLAN;

	vlan_header* vlanHeader = getVlanHeader();
	setVlanID(vlanID);
	vlanHeader->cfi = cfi;
	vlanHeader->priority = 0x07 & priority;
	vlanHeader->etherType = htons(etherType);
}

void VlanLayer::parseNextLayer()
{
	if (m_DataLen <= sizeof(vlan_header))
		return;

	vlan_header* hdr = getVlanHeader();
	switch (ntohs(hdr->etherType))
	{
	case ETHERTYPE_IP:
		m_NextLayer = new IPv4Layer(m_Data + sizeof(vlan_header), m_DataLen - sizeof(vlan_header), this, m_Packet);
		break;
	case ETHERTYPE_IPV6:
		m_NextLayer = new IPv6Layer(m_Data + sizeof(vlan_header), m_DataLen - sizeof(vlan_header), this, m_Packet);
		break;
	case ETHERTYPE_ARP:
		m_NextLayer = new ArpLayer(m_Data + sizeof(vlan_header), m_DataLen - sizeof(vlan_header), this, m_Packet);
		break;
	case ETHERTYPE_VLAN:
		m_NextLayer = new VlanLayer(m_Data + sizeof(vlan_header), m_DataLen - sizeof(vlan_header), this, m_Packet);
		break;
	case ETHERTYPE_PPPOES:
		m_NextLayer = new PPPoESessionLayer(m_Data + sizeof(vlan_header), m_DataLen - sizeof(vlan_header), this, m_Packet);
		break;
	case ETHERTYPE_PPPOED:
		m_NextLayer = new PPPoEDiscoveryLayer(m_Data + sizeof(vlan_header), m_DataLen - sizeof(vlan_header), this, m_Packet);
		break;
	case ETHERTYPE_MPLS:
		m_NextLayer = new MplsLayer(m_Data + sizeof(vlan_header), m_DataLen - sizeof(vlan_header), this, m_Packet);
		break;
	default:
		m_NextLayer = new PayloadLayer(m_Data + sizeof(vlan_header), m_DataLen - sizeof(vlan_header), this, m_Packet);
	}
}

std::string VlanLayer::toString()
{
	std::ostringstream cfiStream;
	cfiStream << ntohs(getVlanHeader()->cfi);
	std::ostringstream priStream;
	priStream << ntohs(getVlanHeader()->priority);
	std::ostringstream idStream;
	idStream << getVlanID();

	return "VLAN Layer, Priority: " + priStream.str() + ", Vlan ID: " + idStream.str() + ", CFI: " + cfiStream.str();
}

// these static method and const variable are only for the purpose of including and using PacketUtils somewhere in Packet++, or else
// when compiling Pcap++ to a dynamic library, PacketUtils isn't compiled and included as it not used anywhere

static size_t packetUtilsDummy()
{
	Packet p;
	return hash5Tuple(&p);
}

static const size_t packetUtilsDummyConst = packetUtilsDummy();

