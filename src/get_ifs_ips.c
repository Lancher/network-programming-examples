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

// List all the interfaces and ips mapping.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>            // memset()

#include <netinet/in.h>       // struct sockaddr, struct sockaddr_in, struct sockaddr_in6, INET_ADDRSTRLEN, INET6_ADDRSTRLEN
#include <arpa/inet.h>        // inet_ntop()
#include <ifaddrs.h>          // struct ifaddrs

#include <errno.h>            // errno, perror()

int
main (int argc, char **argv)
{
  char addr4[INET_ADDRSTRLEN];
  char addr6[INET6_ADDRSTRLEN];

  struct ifaddrs * ifaddrs = NULL;
  struct ifaddrs * ifaddr = NULL;

  // Get interface address list.
  getifaddrs (&ifaddrs);

  // Print all interface and ip address.
  for (ifaddr = ifaddrs; ifaddr != NULL; ifaddr = ifaddr->ifa_next) {
      // IPv4
      if (ifaddr->ifa_addr->sa_family == AF_INET) {
          struct sockaddr_in *addr = (struct sockaddr_in *)ifaddr->ifa_addr;
          inet_ntop(AF_INET, &addr->sin_addr, addr4, INET_ADDRSTRLEN);
          printf("%s: %s\n", ifaddr->ifa_name, addr4);
      }

      // IPv6
      if (ifaddr->ifa_addr->sa_family == AF_INET6) {
          struct sockaddr_in6 *addr = (struct sockaddr_in6 *)ifaddr->ifa_addr;
          inet_ntop(AF_INET6, &addr->sin6_addr, addr6, INET6_ADDRSTRLEN);
          printf("%s: %s\n", ifaddr->ifa_name, addr6);
      }
  }
  // Free the list.
  if (ifaddrs != NULL)
    freeifaddrs(ifaddrs);

  exit (EXIT_SUCCESS);
}