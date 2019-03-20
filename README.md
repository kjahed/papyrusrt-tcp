# Papyrus-RT TCP Extension
This repository conains an extension to Papyrus-RT to add TCP/IP support. The extension adds a TCP system port that grants Capsules access to a TCP service implemented whithin the Runtime System (RTS) library.

### Installation
The extension can be easily installed via the Eclipse update site https://papyrusrt.jahed.ca/tcp
Make sure to re-build the RTS after installing the extension.

### Usage
The TCP port offers the following functions

| Function                                 | Description                                                 |
|------------------------------------------|-------------------------------------------------------------|
| **bool** connect(**const char*** host, **int** port) | connect to the specified host at the specified port         |
| **bool** listen(**int** port)                    | prepare the port for listening on the specified port number |
| **bool** accept()                            | accept exactly one client connection                        |
| **bool** attach(**int** socketfd)                | attach the port to the specified socket file descriptor. This is used to bind a different tcp system port to a socketfd of a newly connected client after a call to accept()     |
| **bool** send(**const char*** str)               | send a string over the socket                               |
| **bool** send(**char** * payload, **int** length)    | send an arbitrary payload                                   |
| **bool** disconnect()                        | close connection / stop listening                           |

In addition, the TCP service may produce the following signals

| Signal                               | Description                                                            |
|--------------------------------------|------------------------------------------------------------------------|
| connected(**int** socketfd)              | Connection was established in response to a connect() or accept() call |
| disconnected()                       | Socket closed in response to a disconnect() call or an error           |
| received(**char*** payload, **int** length) | Emitted whenever a new message is received by the service              |
| error(**int** errno)                     | Emitted whenever a socket error occurs                                 |
