/*  Copyright (C) 2015  Steve (unicornonair@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Periodically IPv4 ping request using Raw socket.
// Use IP_HDRINCL to provide our IPv4 header.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>             // memset()
#include <unistd.h>             // getpid(), sleep()

#include <sys/types.h>          // uint8_t, uint16_t, uint32_t
#include <sys/socket.h>         // socket(), AF_NET
#include <netinet/in.h>         // struct sockaddr_in, IPPROTO_ICMP, INET_ADDRSTRLEN, IPPROTO_IP, IP_TTL
#include <arpa/inet.h>          // inet_pton(), inet_ntop(), htons()
#include <netinet/ip.h>         // struct ip
#include <netinet/ip_icmp.h>    // struct icmphdr, ICMP_ECHO

#include <errno.h>              // errno, perror()

// Function declarations
uint16_t checksum (void *, int);

// IP header + ICMP header + ICMP payload.
struct packet
{
  struct ip iphdr;
  struct icmphdr icmphdr;
  char payload[64 - sizeof (struct icmphdr) - sizeof (struct ip)];
};

int
main (int argc, char *argv[]) {
  int i, sd, on = 1, val = 255;
  uint16_t seq = 0;
  struct packet pkt;
  struct sockaddr_in addr = { AF_INET, 0, 0 };
  char hostname[INET_ADDRSTRLEN];

  // Usage.
  if (argc < 2) {
    printf ("Usage: %s <hostname>\n", argv[0]);
    exit (EXIT_SUCCESS);
  }

  // The kernel will fill the IPv4 header automatically.
  if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror ("socket() ");
    exit (EXIT_FAILURE);
  }

  // Tell the kernel we will provide IPv4 header.
  if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on)) < 0) {
    perror ("setsockopt() ");
    exit (EXIT_FAILURE);
  }

  // Convert the hostname to network bytes.
  if (inet_pton (AF_INET, argv[1], &(addr.sin_addr)) < 0) {
    perror ("inet_pton() ");
    exit (EXIT_FAILURE);
  }

  // Ping the host every 1s.
  for (;;) {
    memset (&pkt, 0, sizeof(pkt));

    // Set IPv4 header.

    // Version number 4 for IPv4.
    pkt.iphdr.ip_v = 4;

    // IPv4 header in 4 bytes.
    pkt.iphdr.ip_hl = 20 / sizeof (uint32_t);

    // Total length of IPv4 datagram.
    pkt.iphdr.ip_len = htons (sizeof (struct packet));

    // Type of service (8 bits).
    pkt.iphdr.ip_tos = 0;

    // Flags (3 bits: zero bit + DF bit + MF bit) + Fragment Offset (13 bits). We do not have fragments.
    pkt.iphdr.ip_off = htons (0x4000);

    // Time-to-Live (8 bits): default to maximum value.
    pkt.iphdr.ip_ttl = 255;

    // Let it 0 and kernel will fill it.
    pkt.iphdr.ip_sum = htons (0);

    // Transport layer protocol (8 bits)
    pkt.iphdr.ip_p = IPPROTO_ICMP;

    // Set ICMPv4 header.

    // Set the ICMP type ICMP_ECHO.
    pkt.icmphdr.type = ICMP_ECHO;

    // The echo id is the process id.
    pkt.icmphdr.un.echo.id = htons (getpid ());
    pkt.icmphdr.un.echo.sequence = htons (seq++);

    // Fill the payload with random data.
    for (i = 0; i < sizeof (pkt.payload); i++) {
      pkt.payload[i] = '0' + i;
    }

    // Calculate the checksum;
    pkt.icmphdr.checksum = checksum (&pkt.icmphdr, sizeof (pkt.icmphdr) + sizeof (pkt.payload));

    if (sendto (sd, &pkt, sizeof(pkt), 0, (struct sockaddr*)&addr, sizeof(addr)) <= 0) {
        perror ("sendto() ");
        exit (EXIT_FAILURE);
    }

    // Sleep 1s.
    sleep (1);

    // Use "tcpdump -nnvvvXS -s0 icmp" to check pinging progress.
  }

}

// calc checksum
uint16_t
checksum(void *buf, int len)
{
  uint16_t *word = buf;
  uint32_t sum = 0;

  for (; len > 1; len -= 2) {
    sum += *word++;
  }
  if (len == 1)
    sum += *(uint8_t *)word;

  sum = (sum >> 16) + (sum & 0xFFFF);
  return ~sum;
}