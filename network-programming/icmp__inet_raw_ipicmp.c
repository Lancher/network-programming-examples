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

// Periodically IPv4 ping request useing IMCP raw socket.
// The kernel will fill Ethernet and IPv4 headers.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>             // memset()
#include <unistd.h>             // getpid(), sleep()

#include <sys/types.h>          // uint8_t, uint16_t, uint32_t
#include <sys/socket.h>         // socket(), AF_NET
#include <netinet/in.h>         // struct sockaddr_in, IPPROTO_ICMP, INET_ADDRSTRLEN
#include <arpa/inet.h>          // inet_pton(), inet_ntop(), htons()
#include <netinet/ip_icmp.h>    // struct icmphdr, ICMP_ECHO

#include <errno.h>              // errno, perror()

// Function declarations
uint16_t checksum(void *, int);

// ICMP header + ICMP payload
struct packet
{
  struct icmphdr hdr;
  char payload[64 - sizeof(struct icmphdr)];
};

int
main(int argc, char *argv[])
{

  int i, sd, val = 255;
  uint16_t seq = 0;
  struct packet pkt;
  struct sockaddr_in addr = { AF_INET, 0, 0};
  char hostname [INET_ADDRSTRLEN];

  // Usage.
  if (argc < 2) {
    printf ("Usage: %s <hostname>\n", argv[0]);
    exit (EXIT_SUCCESS);
  }

  // Create a ICMP raw socket.
  // The kernel will fill the IPv4 header automatically.
  if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    perror ("socket() ");
    exit (EXIT_FAILURE);
  }

  // Set IPv4 header time-to-live option.
  if (setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0) {
    perror("setsockopt() ");
    exit (EXIT_FAILURE);
  }

  // Convert the hostname to network bytes.
  if (inet_pton (AF_INET, argv[1], &(addr.sin_addr)) < 0) {
    perror ("inet_pton() ");
    exit (EXIT_FAILURE);
  }

  for (;;) {
    memset(&pkt, 0, sizeof(pkt));

    // Set the ICMP type ICMP_ECHO.
    pkt.hdr.type = ICMP_ECHO;

    // The echo id is the process id.
    pkt.hdr.un.echo.id = htons (getpid ());
    pkt.hdr.un.echo.sequence = htons (seq++);

    // Fill the payload with random data.
    for (i = 0; i < sizeof(pkt.payload); i++) {
      pkt.payload[i] = '0' + i;
    }

    // Calculate the checksum;
    pkt.hdr.checksum = checksum(&pkt, sizeof(pkt));

    if (sendto (sd, &pkt, sizeof(pkt), 0, (struct sockaddr*)&addr, sizeof(addr)) <= 0) {
        perror ("sendto() ");
        exit (EXIT_FAILURE);
    }

    // Sleep 1s.
    sleep (1);

    // Use "tcpdump -nnvvvXSs 1540 icmp" to check pinging progress.
  }

}

// calc checksum
uint16_t
checksum(void *buf, int len)
{
  uint16_t *word = buf;
  uint32_t sum = 0;

  for (; len > 1; len -= 2) {
    sum += *word;
    word++;
  }
  if (len == 1)
    sum += *(uint8_t *)word;

  sum = (sum >> 16) + (sum & 0xFFFF);
  return ~sum;
}