# CS4390Project
You will write a protocol (application) that is above the TCP/UDP layer.

Your application will use tcp or udp (both need to be implemented).

You will implement a client and server side of the protocol.

1. Set up a connection between server and protocol using a message

2. Client requests a file with  a filename.

3. Server checks if the file is in the system.  If yes send an acknowledgement along with a checksum of the file

3a the server then sends the file

4. If the file is not there, send a Negative ACK.

You will add a checksum of the file to confirm that the file sent is recieved.  So in the

Bonus:

The client sends a message if the file is received and checksum matches, else sends a message indicating corrupted file

Initial Socket from: http://www.linuxhowtos.org/C_C++/socket.htm
