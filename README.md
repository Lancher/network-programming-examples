## IPv4 and IPv6 raw sockets programming

### Introduction :

Low level socket programming in C.

--

### IPv4 :

Create a socket with `socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)`. <br>
The kernel will fill the Ethernet and IP header automatically.

| Code | Descriptions  |
| --- | --- |
| [icmp__inet_raw_ipicmp.c](network-programming/icmp__inet_raw_ipicmp.c) | Send ICMPv4 echo request with some payload. |

--

### IPv6 :

--

### Miscellaneous :

| Code | Descriptions  |
| --- | --- |
| [getaddrinfo.c](network-programming/getaddrinfo.c) | Use `getaddrinfo()` to identify an Internet hostname. |
| [bind_dev.c](network-programming/bind_dev.c) | Bind a socket to a network interface(eth0, eth1 ...).  |

--
