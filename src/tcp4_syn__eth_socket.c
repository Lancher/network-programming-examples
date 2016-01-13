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

// Send TCP SYN request and contruct the ethernet header, ip header
// and tcp header.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>             // memset()
#include <unistd.h>             // getpid(), sleep()

#include <sys/types.h>          // uint8_t, uint16_t, uint32_t
#include <sys/socket.h>         // socket()
#include <netinet/in.h>         // struct sockaddr_in
#include <arpa/inet.h>          // inet_pton(), inet_ntop(), htons()
#include <net/if.h>             // struct ifreq
#include <sys/ioctl.h>          // ioctl()
#include <netinet/if_ether.h>   // struct ether_header
#include <netinet/ip.h>         // struct ip
#include <netinet/tcp.h>        // struct tcphdr
#include <linux/if_ether.h>     // ETH_P_ALL, ETH_P_IP, ETH_P_IPV6
#include <linux/if_packet.h>    // struct sockaddr_ll
#include <ifaddrs.h>            // struct ifaddrs

#include <errno.h>              // errno, perror()

// Function declarations
uint16_t checksum (void *, int);
uint16_t tcp4_checksum (struct ip, struct tcphdr);

// Ethernet header + IPv4 header + TCP header
struct packet
{
  struct ether_header ethhdr;     // 14 bytes
  struct ip iphdr;                // 20 bytes
  struct tcphdr tcphdr;           // 20 bytes
};

int
main (int argc, char **argv)
{
  int i;
  int sd;
  int bytes;
  char buf[54];
  // Address
  struct sockaddr_in src = { AF_INET, 0, 0, 0, 0 };
  struct sockaddr_in dst = { AF_INET, 0, 0, 0, 0 };
  char src_name[INET_ADDRSTRLEN];
  char dst_name[INET_ADDRSTRLEN];
  struct sockaddr_ll device;
  // Packet
  struct packet pkt;
  // Interface
  struct ifreq ifr;
  // interface address
  struct ifaddrs * ifaddrs = NULL;
  struct ifaddrs * ifaddr = NULL;

  // Clear packet.
  memset (&buf, 0, 54);
  memset (&pkt, 0, sizeof(struct packet));

  // Usage.
  if (argc != 3) {
    printf ("Usage: %s <interface> <ip adddress>\n", argv[0]);
    exit (EXIT_SUCCESS);
  }

  // Create a socket to look up interface.
  if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {
    perror ("socket() ");
    exit (EXIT_FAILURE);
  }

  // Use ioctl() to find interface index.
  memset (&ifr, 0, sizeof(struct ifreq));
  snprintf (ifr.ifr_name, sizeof(ifr.ifr_name), "%s", argv[1]);
  if (ioctl (sd, SIOCGIFINDEX, &ifr) < 0) {
    perror ("ioctl() failed to find interface ");
    return (EXIT_FAILURE);
  }

  printf("Source interface %s is index %d\n", argv[1], ifr.ifr_ifindex);

  // Index of the network device
  memset (&device, 0, sizeof (device));
  device.sll_ifindex = ifr.ifr_ifindex;

  // Use ioctl() to find hardware address.
  memset (&ifr, 0, sizeof(struct ifreq));
  snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", argv[1]);
  if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
    perror ("ioctl() failed to find hardware address ");
    return (EXIT_FAILURE);
  }

  // Copy source MAC address to ethernet header.
  memcpy (&pkt.ethhdr.ether_shost, &ifr.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t));

  // Show source MAC address.
  printf("Source MAC address is ");
  for (i = 0; i < 5; i++)
    printf ("%02X:", pkt.ethhdr.ether_shost[i]);
  printf ("%02X\n", pkt.ethhdr.ether_shost[5]);

  // Use getifaddrs() to find interface address, ioctl() with SIOCGIFADDR is not work for IPv6.

  // Get interface address list.
  getifaddrs (&ifaddrs);

  // Get all interface and ip address.
  for (ifaddr = ifaddrs; ifaddr != NULL; ifaddr = ifaddr->ifa_next) {
      // IPv4
      if ((ifaddr->ifa_addr != NULL) && (ifaddr->ifa_addr->sa_family == AF_INET)) {
          struct sockaddr_in *addr = (struct sockaddr_in *)ifaddr->ifa_addr;
          if (strcmp (argv[1], ifaddr->ifa_name) == 0) {
            src.sin_addr = addr->sin_addr;
            inet_ntop (AF_INET, &addr->sin_addr, src_name, INET_ADDRSTRLEN);
            printf ("Source address is %s\n", src_name);
          }
      }
  }

  // Free the list.
  if (ifaddrs != NULL)
    freeifaddrs(ifaddrs);

  close (sd);

  // Destination Mac address.
  pkt.ethhdr.ether_dhost[0] = 0x00;
  pkt.ethhdr.ether_dhost[1] = 0x00;
  pkt.ethhdr.ether_dhost[2] = 0x0c;
  pkt.ethhdr.ether_dhost[3] = 0x9f;
  pkt.ethhdr.ether_dhost[4] = 0xf0;
  pkt.ethhdr.ether_dhost[5] = 0x13;

  // Destination IPv4 address.
  snprintf (dst_name, sizeof(dst_name), "%s", argv[2]);

  // Convert the hostname to network bytes.
  if (inet_pton (AF_INET, dst_name, &(dst.sin_addr)) < 0) {
    perror ("inet_pton() ");
    exit (EXIT_FAILURE);
  }

  printf("Destination MAC address is ");
  for (i = 0; i < 5; i++)
    printf ("%02X:", pkt.ethhdr.ether_dhost[i]);
  printf ("%02X\n", pkt.ethhdr.ether_dhost[5]);

  printf("Destination address is %s\n", dst_name);

  // 1. Construct ethernet header.

  // Next is ethernet type code (ETH_P_IP for IPv4).
  pkt.ethhdr.ether_type = htons (ETH_P_IP);

  // 2. Construct ip header.

  // Version number 4 for IPv4.
  pkt.iphdr.ip_v = 4;

  // IPv4 header in 4 bytes.
  pkt.iphdr.ip_hl = 20 / sizeof (uint32_t);

  // Total length of IPv4 datagram (ip + tcp).
  pkt.iphdr.ip_len = htons (20 + 20);

  // Type of service (8 bits).
  pkt.iphdr.ip_tos = 0;

  // Flags (3 bits: zero bit + DF bit + MF bit) + Fragment Offset (13 bits). We do not have fragments.
  pkt.iphdr.ip_off = htons (0x4000);

  // Time-to-Live (8 bits): default to maximum value.
  pkt.iphdr.ip_ttl = 255;

  // Transport layer protocol (8 bits)
  pkt.iphdr.ip_p = IPPROTO_TCP;

  // IPv4 src & dst address.
  pkt.iphdr.ip_src = src.sin_addr;
  pkt.iphdr.ip_dst = dst.sin_addr;

  // Checksum.
  pkt.iphdr.ip_sum = 0;
  pkt.iphdr.ip_sum = checksum ((uint16_t *) &pkt.iphdr, 20);

  // 3. Construct tcp header.

  // Source port number.
  pkt.tcphdr.th_sport = htons (10000);

  // Destination port number.
  pkt.tcphdr.th_dport = htons (80);

  // Sequence number.
  pkt.tcphdr.th_seq = htonl (0);

  // Acknowledgement number (32 bits): 0 in first packet of SYN/ACK process
  pkt.tcphdr.th_ack = htonl (0);

  // Reserved (4 bits): should be 0
  pkt.tcphdr.th_x2 = 0;

  // Data offset (4 bits): size of TCP header in 32-bit words
  pkt.tcphdr.th_off = 20 / sizeof (uint32_t);

  // Flags (8 bits)
  pkt.tcphdr.th_flags = 0;

  // FIN flag (1 bit)
  pkt.tcphdr.th_flags += (0 << 0);

  // SYN flag (1 bit): set to 1
  pkt.tcphdr.th_flags += (1 << 1);

  // RST flag (1 bit)
  pkt.tcphdr.th_flags += (0 << 2);

  // PSH flag (1 bit)
  pkt.tcphdr.th_flags += (0 << 3);

  // ACK flag (1 bit)
  pkt.tcphdr.th_flags += (0 << 4);

  // URG flag (1 bit)
  pkt.tcphdr.th_flags += (0 << 5);

  // ECE flag (1 bit)
  pkt.tcphdr.th_flags += (0 << 6);

  // CWR flag (1 bit)
  pkt.tcphdr.th_flags += (0 << 7);

  // Window size (16 bits)
  pkt.tcphdr.th_win = htons (65535);

  // Urgent pointer (16 bits): 0 (only valid if URG flag is set)
  pkt.tcphdr.th_urp = htons (0);

  // TCP checksum (16 bits)
  pkt.tcphdr.th_sum = tcp4_checksum (pkt.iphdr, pkt.tcphdr);

  // 4. Fill out sockaddr_ll.

  // Always AF_PACKET.
  device.sll_family = AF_PACKET;

  // Destination Mac address.
  memcpy (device.sll_addr, &pkt.ethhdr.ether_dhost, ETH_ALEN * sizeof (uint8_t));

  // Mac Address length 6.
  device.sll_halen = ETH_ALEN;

  if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {
    perror ("socket() ");
    exit (EXIT_FAILURE);
  }

  // 5. Send the packet.
  memcpy (buf, &pkt.ethhdr, 14);
  memcpy (buf + 14, &pkt.iphdr, 20);
  memcpy (buf + 14 + 20, &pkt.tcphdr, 20);

  if ((bytes = sendto (sd, &buf, sizeof (buf), 0, (struct sockaddr *) &device, sizeof (struct sockaddr_ll))) <= 0) {
    perror ("sendto() ");
    exit (EXIT_FAILURE);
  }
  printf("Send %d bytes.\n", bytes);

  close (sd);

}

// calc checksum
uint16_t
checksum (void *buf, int len)
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

uint16_t
tcp4_checksum (struct ip iphdr, struct tcphdr tcphdr)
{
  uint16_t svalue;
  char buf[IP_MAXPACKET], cvalue;
  char *ptr;
  int chksumlen = 0;

  ptr = &buf[0];  // ptr points to beginning of buffer buf

  // Copy source IP address into buf (32 bits)
  memcpy (ptr, &iphdr.ip_src.s_addr, sizeof (iphdr.ip_src.s_addr));
  ptr += sizeof (iphdr.ip_src.s_addr);
  chksumlen += sizeof (iphdr.ip_src.s_addr);

  // Copy destination IP address into buf (32 bits)
  memcpy (ptr, &iphdr.ip_dst.s_addr, sizeof (iphdr.ip_dst.s_addr));
  ptr += sizeof (iphdr.ip_dst.s_addr);
  chksumlen += sizeof (iphdr.ip_dst.s_addr);

  // Copy zero field to buf (8 bits)
  *ptr = 0; ptr++;
  chksumlen += 1;

  // Copy transport layer protocol to buf (8 bits)
  memcpy (ptr, &iphdr.ip_p, sizeof (iphdr.ip_p));
  ptr += sizeof (iphdr.ip_p);
  chksumlen += sizeof (iphdr.ip_p);

  // Copy TCP length to buf (16 bits)
  svalue = htons (sizeof (tcphdr));
  memcpy (ptr, &svalue, sizeof (svalue));
  ptr += sizeof (svalue);
  chksumlen += sizeof (svalue);

  // Copy TCP source port to buf (16 bits)
  memcpy (ptr, &tcphdr.th_sport, sizeof (tcphdr.th_sport));
  ptr += sizeof (tcphdr.th_sport);
  chksumlen += sizeof (tcphdr.th_sport);

  // Copy TCP destination port to buf (16 bits)
  memcpy (ptr, &tcphdr.th_dport, sizeof (tcphdr.th_dport));
  ptr += sizeof (tcphdr.th_dport);
  chksumlen += sizeof (tcphdr.th_dport);

  // Copy sequence number to buf (32 bits)
  memcpy (ptr, &tcphdr.th_seq, sizeof (tcphdr.th_seq));
  ptr += sizeof (tcphdr.th_seq);
  chksumlen += sizeof (tcphdr.th_seq);

  // Copy acknowledgement number to buf (32 bits)
  memcpy (ptr, &tcphdr.th_ack, sizeof (tcphdr.th_ack));
  ptr += sizeof (tcphdr.th_ack);
  chksumlen += sizeof (tcphdr.th_ack);

  // Copy data offset to buf (4 bits) and
  // copy reserved bits to buf (4 bits)
  cvalue = (tcphdr.th_off << 4) + tcphdr.th_x2;
  memcpy (ptr, &cvalue, sizeof (cvalue));
  ptr += sizeof (cvalue);
  chksumlen += sizeof (cvalue);

  // Copy TCP flags to buf (8 bits)
  memcpy (ptr, &tcphdr.th_flags, sizeof (tcphdr.th_flags));
  ptr += sizeof (tcphdr.th_flags);
  chksumlen += sizeof (tcphdr.th_flags);

  // Copy TCP window size to buf (16 bits)
  memcpy (ptr, &tcphdr.th_win, sizeof (tcphdr.th_win));
  ptr += sizeof (tcphdr.th_win);
  chksumlen += sizeof (tcphdr.th_win);

  // Copy TCP checksum to buf (16 bits)
  // Zero, since we don't know it yet
  *ptr = 0; ptr++;
  *ptr = 0; ptr++;
  chksumlen += 2;

  // Copy urgent pointer to buf (16 bits)
  memcpy (ptr, &tcphdr.th_urp, sizeof (tcphdr.th_urp));
  ptr += sizeof (tcphdr.th_urp);
  chksumlen += sizeof (tcphdr.th_urp);

  return checksum ((uint16_t *) buf, chksumlen);
}