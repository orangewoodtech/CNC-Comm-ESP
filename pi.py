import json
from socket import *
import select


def hashing(var):
    hash = 0
    i=0
    while(i<len(var)):
        c=var[i]
        i+=1
        hash += ord(c)%101

    return hash

data = None

data = open('C:\\Users\\GAURAV PATIL\\Desktop\\Orangewood\\esp32\\GCode3.txt',"r")
d = data.readlines()
#lines = [] if len(d) <= 10 else all_lines[1:-1]

timeout = 3 # timeout in seconds

host = "192.168.43.148"
print("Connecting to " + host)

port = 80

s = socket(AF_INET, SOCK_STREAM)
print("Socket made")

ready = select.select([s],[],[],timeout)

s.connect((host,port))
print("Connection made")
ip_address=s.getsockname()[0]
print (ip_address)
z='Q'
req=z.encode()
s.sendall(req)
e=s.recv(1)
h=e.decode()
# print(h)

i = 0
dic ={}
while i<len(d):
    d1 = {i:d[i]}
    dic.update(d1)
    i+=1



if(h=='P'):
    i=0
    # flag=2
    while(i<len(dic)):
        # if(flag==2 or flag==1):
            a=json.dumps({"index":i, "GCode":dic[i], "Hash_value":hashing(dic[i])})
            b=a.encode()
            s.send(b)
            c=s.recv(1)
            j=c.decode()
            print(j)
            flag=1;
            if(j=='Y'):
                i=i+1
                j=""
            elif(j=='N'):
                while(j=='N'):
                    b=a.encode()
                    s.send(b)
                    c=s.recv(1)
                    j=c.decode()
                    if(j=='Y'):
                        i=i+1
                        j=""
            else:
                break


data.close()
s.close()
