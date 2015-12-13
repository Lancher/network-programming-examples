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

// Periodically IPv6 ping request using IMCP raw socket.
// The kernel will fill Ethernet, IPv4 headers and IMCPv6 header checksum.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>            // memset()
#include <unistd.h>             // getpid(), sleep()

#include <sys/socket.h>         // socket(), AF_NET6
#include <netinet/in.h>         // struct sockaddr_in6, IPPROTO_ICMPV6, INET6_ADDRSTRLEN
#include <arpa/inet.h>          // inet_pton(), inet_ntop(), htons()
#include <netinet/icmp6.h>      // struct icmp6_hdr, ICMP6_ECHO_REQUEST

#include <errno.h>              // errno, perror()

// ICMP6 header + ICMP6 payloads
struct packet
{
  struct icmp6_hdr hdr;
  char msg[64 - sizeof(struct icmp6_hdr)];
};

int
main (int argc, char *argv[])
{
  int i, sd;
  uint16_t seq = 0;
  struct packet pkt;
  struct sockaddr_in6 addr = { AF_INET6, 0, 0, 0, 0 };
  char hostname[INET6_ADDRSTRLEN];

  // Usage.
  if (argc < 2) {
    printf ("Usage: %s <hostname>\n", argv[0]);
    exit (EXIT_SUCCESS);
  }

  // Create a ICMPv6 raw socket.
  // The kernel will fill the IPv6 header automatically.
  if ((sd = socket (AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0) {
    perror ("socket() ");
    exit (EXIT_FAILURE);
  }

  // Convert the hostname to network bytes.
  if (inet_pton (AF_INET6, argv[1], &(addr.sin6_addr)) < 0) {
    perror ("inet_pton() ");
    exit (EXIT_FAILURE);
  }

  // Ping the host every 1s.
  for (;;) {
    memset (&pkt, 0, sizeof(pkt));

    // Set header type.
    pkt.hdr.icmp6_type = ICMP6_ECHO_REQUEST;

    // The code is 0 for ICMP6_ECHO_REQUEST and ICMP6_ECHO_REPLY.
    pkt.hdr.icmp6_code = 0;
    pkt.hdr.icmp6_id = htons (getpid ());
    // When you create a ICMPv6 raw socket, the kernel will calculate and
    // insert the ICMPv6 checksum automatically.
    // pkt.hdr.icmp6_cksum = 0;
    pkt.hdr.icmp6_seq = htons (seq++);

    // Fill the payload with random data.
    for (i = 0; i < sizeof(pkt.msg); i++)
      pkt.msg[i] = '0' + i;

    if (sendto(sd, &pkt, sizeof(pkt), 0, (struct sockaddr*)&addr, sizeof(addr)) <= 0) {
        perror ("sendto() ");
        exit (EXIT_FAILURE);
    }

    // Sleep 1s.
    sleep(1);

    // Use "tcpdump -nnvvvXS -s0 icmp6" to check pinging progress.
  }

}