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

// Get MTU of the interface.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>           // memset()

#include <sys/socket.h>       // socket(), AF_NET, SOCK_RAW, SOCK_STREAM
#include <netinet/in.h>       // struct sockaddr, struct sockaddr_in, IPPROTO_RAW, IPPROTO_TCP, INET_ADDRSTRLEN
#include <net/if.h>           // struct ifreq
#include <arpa/inet.h>        // htons(), inet_pton(), inet_ntop()
#include <sys/ioctl.h>        // ioctl()

#include <errno.h>            // errno, perror()

int
main(int argc, char **argv)
{
  int sd;
  char interface[] = "eth0";
  struct ifreq ifr;

  // Create a socket to look up interface.
  if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror ("socket() ");
    exit (EXIT_FAILURE);
  }

  // Use ioctl() to find interface index.
  memset (&ifr, 0, sizeof(struct ifreq));
  snprintf (ifr.ifr_name, sizeof(ifr.ifr_name), "%s", interface);
  if (ioctl (sd, SIOCGIFMTU, &ifr) < 0) {
    perror ("ioctl() failed to find interface MTU ");
    return (EXIT_FAILURE);
  }
  printf ("MTU for interface %s is %d\n", interface, ifr.ifr_mtu);

  // close socket
  if (close (sd) < 0)
    perror ("close() ");

}