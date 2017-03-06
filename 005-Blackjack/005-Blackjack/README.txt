Blackjack over TCP by Jonathan Ginsburg A01021617.

NAME
    005-Blackjack -- Blackjack over TCP

SYNOPSIS
    executable server <inet_addr> <inet_port>
    executable client <inet_addr> <inet_port>

DESCRIPTION

    This program is both a server and a client, depending on which argument is given (see SYNOPSIS).

    If called as a server, the <inet_addr> argument is the local interface address to bind a TCP socket to for incoming connections. The <inet_port> argument is the local interface port to bind to. After invocation, the socket must be stopped by an ad hoc signal, (e.g. SIGTERM, SIGKILL, SIGHUP, etc.). It will display status updates to standard output.

    If called as a client, the <inet_addr> argument is the address to which the program will try to connect; <inet_port> is the port number for the target socket's connection. Further interaction with the executable is through prompts directed to standard output and user responses read from standard input.

CREDITS
    Jonathan Ginsburg on March 6, 2017.
