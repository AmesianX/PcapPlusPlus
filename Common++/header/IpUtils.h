#ifndef PCAPPP_IP_UTILS
#define PCAPPP_IP_UTILS

#include <stdint.h>
#ifdef LINUX
#include <in.h>
#include <arpa/inet.h>
#endif
#ifdef MAC_OS_X
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <Macros.h>

/// @file

#ifdef WIN32
/**
 * Convert a network format address to presentation format.
 * @param[in] af Address family, can be either AF_INET (IPv4) or AF_INET6 (IPv6)
 * @param[in] src Network address structure, can be either in_addr (IPv4) or in6_addr (IPv6)
 * @param[out] dst Network address string representation
 * @param[in] size 'dst' Maximum size
 * @return pointer to presentation format address ('dst'), or NULL (see errno).
 */
const char* PCAPPP_EXPORT inet_ntop(int af, const void* src, char* dst, size_t size);

/**
 * Convert from presentation format (which usually means ASCII printable)
 * to network format (which is usually some kind of binary format).
 * @param[in] af Address family, can be either AF_INET (IPv4) or AF_INET6 (IPv6)
 * @param[in] src Network address string representation
 * @param[out] dst Network address structure result, can be either in_addr (IPv4) or in6_addr (IPv6)
 * @return
 * 1 if the address was valid for the specified address family;
 * 0 if the address wasn't valid ('dst' is untouched in this case);
 * -1 if some other error occurred ('dst' is untouched in this case, too)
 */
int PCAPPP_EXPORT inet_pton(int af, const char* src, void* dst);
#endif


/**
 * Extract IPv4 address from sockaddr
 * @param[in] sa - input sockaddr
 * @return Address in in_addr format
 */
in_addr* PCAPPP_EXPORT sockaddr2in_addr(struct sockaddr *sa);

/**
 * Extract IPv6 address from sockaddr
 * @param[in] sa - input sockaddr
 * @return Address in in6_addr format
 */
in6_addr* PCAPPP_EXPORT sockaddr2in6_addr(struct sockaddr *sa);

/**
 * Converts a sockaddr format address to its string representation
 * @param[in] sa Address in sockaddr format
 * @param[out]  resultString String representation of the address
 */
void PCAPPP_EXPORT sockaddr2string(struct sockaddr *sa, char* resultString);

/**
 * Convert a in_addr format address to 32bit representation
 * @param[in] inAddr Address in in_addr format
 * @return Address in 32bit format
 */
uint32_t PCAPPP_EXPORT in_addr2int(in_addr inAddr);

/**
 * A struct that represent a single buffer
 */
struct PCAPPP_EXPORT ScalarBuffer
{
	/**
	 * The pointer to the buffer
	 */
	uint16_t* buffer;

	/**
	 * Buffer length
	 */
	size_t len;
};

/**
 * Computes the checksum for a vector of buffers
 * @param[in] vec The vector of buffers
 * @param[in] vecSize Number of ScalarBuffers in vector
 * @return The checksum result
 */
uint16_t PCAPPP_EXPORT compute_checksum(ScalarBuffer vec[], size_t vecSize);

#endif
