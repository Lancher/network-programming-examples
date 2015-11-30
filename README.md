## IPv4 and IPv6 raw sockets programming

#### Introduction :

Low level socket programming in C.

--

#### IPv4 Sockets:

##### Create a socket with `socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)`.

- The kernel will fill the Ethernet and IP header automatically.

| Code | Descriptions  |
| --- | --- |
| [icmp__inet_raw_ipicmp.c](network-programming/icmp__inet_raw_ipicmp.c) | Send ICMPv4 echo request with some payload. |

--

#### IPv6 Sockets:

- IPv6 sockets api [RFC3493](http://www.ietf.org/rfc/rfc3493.txt) and [RFC3542](http://www.ietf.org/rfc/rfc3542.txt).

##### Create a socket with `socket(AF_INET6, SOCK_RAW, IPPROTO_ICMP6)`.

- The kernel will fill the Ethernet, IP header automatically and ICMPv6 header chuckseum.

| Code | Descriptions  |
| --- | --- |
| [icmp__inet_raw_ipicmpv6.c](network-programming/icmp__inet_raw_ipicmpv6.c) | Send ICMPv6 echo request with some payload. |


--

#### Miscellaneous :

| Code | Descriptions  |
| --- | --- |
| [getaddrinfo.c](network-programming/getaddrinfo.c) | Use `getaddrinfo()` to identify an Internet hostname. |
| [bind_dev.c](network-programming/bind_dev.c) | Bind a socket to a network interface(eth0, eth1 ...).  |

--
