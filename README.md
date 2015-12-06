## IPv4 and IPv6 raw sockets programming

#### Introduction :

Low level socket programming in C.

--

#### IPv4

##### #1 - A socket using `socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)`:

- The kernel will fill the Ethernet and IP header automatically.

| Code | Descriptions  |
| --- | --- |
| [icmpv4__icmp_socket.c](src/icmpv4__icmp_socket.c) | Send ICMPv4 echo request with some payload(set TTL). |

##### #2 - IPv4 ancillary data:

- Using ancillary data to specify the option content for a single datagram.

- More details on using ancillary data, please refer to `Linux Socket Programming by Example - Chapter 17`.

| Code | Descriptions  |
| --- | --- |
| [icmpv4__icmp_socket_anc.c](src/icmpv4__icmp_socket_anc.c) | Send ICMPv4 echo request with some payload using ancillary data(set TTL). |

--



#### IPv6

- IPv6 sockets api [RFC3493](http://www.ietf.org/rfc/rfc3493.txt) and [RFC3542](http://www.ietf.org/rfc/rfc3542.txt).

##### #1 - A socket using `socket(AF_INET6, SOCK_RAW, IPPROTO_ICMP6)`:

- The kernel will fill the Ethernet, IP header automatically and ICMPv6 header chuckseum.

| Code | Descriptions  |
| --- | --- |
| [icmpv6__icmp_socket.c](src/icmpv6__icmp_socket.c) | Send ICMPv6 echo request with some payload. |

##### #2 - IPv6 ancillary data:

- All the fields in IPv6 header and all the extension headers can be set by `ancillary data` or `setsockopt()`.

- There is no need for a socket option similar to the IPv4 IP_HDRINCL socket option.

- Using ancillary data to specify the option content for a single datagram.


--

#### Miscellaneous

| Code | Descriptions  |
| --- | --- |
| [getaddrinfo.c](network-programming/getaddrinfo.c) | Use `getaddrinfo()` to identify an Internet hostname. |
| [bind_dev.c](network-programming/bind_dev.c) | Bind a socket to a network interface(eth0, eth1 ...).  |
| [iovec.c](network-programming/iovec.c) | How iovec work in ancillary data.  |
--
