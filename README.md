#directchat
when compiling pass the argument -pthread to gcc
gcc -pthread -o chat chat.c

USAGE:
./chat <port> <remote_ip>
e.g.
./chat 123 192.168.1.255
