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

// List all the ip addresses of a hostname using "getaddrinfo()".

#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>       // struct sockaddr, struct sockaddr_in, INET_ADDRSTRLEN, INET6_ADDRSTRLEN
#include <arpa/inet.h>        // inet_ntop()
#include <netdb.h>            // struct addrinfo

#include <errno.h>            // errno, perror()

int
main (int argc, char *argv[])
{
  struct addrinfo *info;
  struct addrinfo *res;
  char addr4[INET_ADDRSTRLEN], addr6[INET6_ADDRSTRLEN];

  if (argc < 2) {
    printf ("Usage: %s <hostname>\n", argv[0]);
    exit (EXIT_SUCCESS);
  }

  // Ger the addr list.
  if (getaddrinfo(argv[1], NULL, NULL, &info) < 0) {
    perror ("getaddrinfo() ");
    exit (EXIT_FAILURE);
  }

  // Print all the addresses.
  for (res = info; res != NULL; res = res->ai_next) {
    if (res->ai_family == AF_INET) {
      inet_ntop (AF_INET, &((struct sockaddr_in *)res->ai_addr)->sin_addr, addr4, sizeof(addr4));
      printf ("%s\n", addr4);
    }
    if (res->ai_family == AF_INET6) {
      inet_ntop (AF_INET6, &((struct sockaddr_in6 *)res->ai_addr)->sin6_addr, addr6, sizeof(addr6));
      printf ("%s\n", addr6);
    }
  }
}