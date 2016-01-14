## IPv4 and IPv6 raw sockets programming

#### Introduction :

Low level socket programming in C.

--

#### ARP

--

#### IPv4

##### # ICMPv4:

- When create a ICMP socket `socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)`, the kernel will fill the Ethernet and IP header automatically.

- Using ancillary data to specify the option content for only a single datagram.

- More details on using ancillary data, please refer to `Linux Socket Programming by Example - Chapter 17`.

| Code | Descriptions  |
| --- | --- |
| [icmpv4_echo__icmp_socket.c](src/icmpv4_echo__icmp_socket.c) | Send echo request (with TTL)). |
| [icmpv4_echo__icmp_socket_ancillary.c](src/icmpv4_echo__icmp_socket_ancillary.c) | Send echo request using ancillary data (with TTL)). |
| [icmpv4_echo__raw_socket.c](src/icmpv4_echo__raw_socket.c) | Create a Raw socket(IP_HDRINCL) to send echo request (with TTL). |

--

#### IPv6

##### # IPv6:

- Fragmentation.

- Hop-by-Hop Extension Header.

##### # ICMPv6:

- IPv6 sockets api [RFC3493](http://www.ietf.org/rfc/rfc3493.txt) and [RFC3542](http://www.ietf.org/rfc/rfc3542.txt).

- When create a ICMPv6 socket `socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)`, the kernel will fill the Ethernet, IP header and ICMPv6 header chuckseum automatically.

- All the fields in IPv6 header and all the extension headers can be set by `ancillary data` or `setsockopt()`.

- There is No option similar to the `IPv4 IP_HDRINCL` socket option.

- Using ancillary data to specify the option content for a single datagram.

| Code | Descriptions  |
| --- | --- |
| [icmpv6_echo__icmpv6_socket.c](src/icmpv6_echo__icmpv6_socket.c) | Send echo request (with hoplimit). |
| [icmpv6_echo__icmpv6_socket_ancillary.c](src/icmpv6_echo__icmpv6_socket_ancillary.c) | Send echo request using ancillary data (with hoplimit). |

- Neighbor Discovery [RFC4861](https://tools.ietf.org/html/rfc4861).

- IPv6 Neighbor Discovery (using multicast address, IPv6 do NOT have broadcast address).

- Neighbor Solicitation/Advertisement and Router Solicitation/Advertisement

| Code | Descriptions  |
| --- | --- |
| [icmpv6_ns__icmpv6_socket_ancillary.c](src/icmpv6_ns__icmpv6_socket_ancillary.c) | Send neighbor solicitation request (with hoplimit). |
| [icmpv6_na__icmpv6_socket_ancillary.c](src/icmpv6_na__icmpv6_socket_ancillary.c) | Send neighbor advertisement request (with hoplimit). |
| [icmpv6_rs__icmpv6_socket_ancillary.c](src/icmpv6_rs__icmpv6_socket_ancillary.c) | Send router solicitation request (with hoplimit). |
| [icmpv6_ra__icmpv6_socket_ancillary.c](src/icmpv6_ra__icmpv6_socket_ancillary.c) | Send router advertisement request (with hoplimit). |

##### # Tunneling IPv6 over IPv4:

- [RFC4213](http://www.ietf.org/rfc/rfc4213.txt).

--

#### TCP4

| Code | Descriptions  |
| --- | --- |
| [tcp4_syn__eth_socket.c](src/tcp4_syn__eth_socket.c) | Send TCP SYN packet (construct from ethernet layer). |

--

#### Miscellaneous

| Code | Descriptions  |
| --- | --- |
| [getaddrinfo.c](src/getaddrinfo.c) | Use `getaddrinfo()` to identify an Internet hostname. |
| [bind_dev.c](src/bind_dev.c) | Bind a socket to a network interface(eth0, eth1 ...).  |
| [iovec.c](src/iovec.c) | How iovec work in ancillary data.  |
| [get_ifs_ips.c](src/get_ifs_ips.c) | List all the interfaces and ip address.  |
| [get_mtu.c](src/get_mtu.c) | Retrieve MTU of a interface.  |

--
