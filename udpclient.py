# import socket
# import subprocess
#
# s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
# s.setsockopt(socket.SOL_IP, socket.IP_MULTICAST_TTL, 20)
# s.setsockopt(socket.SOL_IP, socket.IP_MULTICAST_LOOP, 1)
# ip="192.168.43.255"
# # sock = socket(AF_INET, SOCK_DGRAM)
# s.bind(("", 5005))
#
# intf = socket.gethostbyname(socket.gethostname())
# s.setsockopt(socket.SOL_IP, socket.IP_MULTICAST_IF, socket.inet_aton(intf))
# # s.setsockopt(socket.SOL_IP, socket.IP_ADD_MEMBERSHIP, socket.inet_aton(addr) + socket.inet_aton(intf))
# msg="hello"
# # s.sendto(msg.encode(),("0.0.0.0", 5005))
# print("done")
# while True:
#     data, addr = s.recv(16)
#     print ("received message: ", data)
import socket

# bind all IP
HOST = '0.0.0.0'
# Listen on Port
PORT = 44444
#Size of receive buffer
BUFFER_SIZE = 1024
# Create a TCP/IP socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# Bind the socket to the host and port
s.bind((HOST, PORT))
while True:
    # Receive BUFFER_SIZE bytes data
    # data is a list with 2 elements
    # first is data
    #second is client address
    data = s.recvfrom(BUFFER_SIZE)
    if data:
        #print received data
        print('Client to Server: ' , data)
        # Convert to upper case and send back to Client
        s.sendto(data[0].upper(), data[1])
# Close connection
s.close()
