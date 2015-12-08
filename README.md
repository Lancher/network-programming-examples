## IPv4 and IPv6 raw sockets programming

#### Introduction :

Low level socket programming in C.

--

#### IPv4

##### # ICMPv4:

- When create a ICMP socket `socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)`, the kernel will fill the Ethernet and IP header automatically.

- Using ancillary data to specify the option content for only a single datagram.

- More details on using ancillary data, please refer to `Linux Socket Programming by Example - Chapter 17`.

| Code | Descriptions  |
| --- | --- |
| [icmpv4__icmp_socket.c](src/icmpv4__icmp_socket.c) | Create a ICMP socket to send Echo (with TTL)). |
| [icmpv4__icmp_socket_ancillary.c](src/icmpv4__icmp_socket_ancillary.c) | Create a ICMP socket to send Echo with ancillary data (with TTL)). |
| [icmpv4__raw_socket.c](src/icmpv4__raw_socket.c) | Create a Raw socket(IP_HDRINCL) to send Echo (with TTL). |

--



#### IPv6

##### # ICMPv6:

- IPv6 sockets api [RFC3493](http://www.ietf.org/rfc/rfc3493.txt) and [RFC3542](http://www.ietf.org/rfc/rfc3542.txt).

- When create a ICMPv6 socket `socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)`, the kernel will fill the Ethernet, IP header and ICMPv6 header chuckseum automatically.

- All the fields in IPv6 header and all the extension headers can be set by `ancillary data` or `setsockopt()`.

- There is no need for a socket option similar to the IPv4 IP_HDRINCL socket option.

- Using ancillary data to specify the option content for a single datagram.

| Code | Descriptions  |
| --- | --- |
| [icmpv6__icmpv6_socket.c](src/icmpv6__icmpv6_socket.c) | Create a ICMPv6 socket to send Echo (with hoplimit). |
| [icmpv6__icmpv6_socket_ancillary.c](src/icmpv6__icmpv6_socket_ancillary.c) | Create a ICMPv6 socket to send Echo with ancillary data (with hoplimit). |



--

#### Miscellaneous

| Code | Descriptions  |
| --- | --- |
| [getaddrinfo.c](src/getaddrinfo.c) | Use `getaddrinfo()` to identify an Internet hostname. |
| [bind_dev.c](src/bind_dev.c) | Bind a socket to a network interface(eth0, eth1 ...).  |
| [iovec.c](src/iovec.c) | How iovec work in ancillary data.  |
--
