from socket import *
import numpy as np
import cv2

class SocketInfo():
    HOST = "000.000.000.000"
    PORT = 1234
    ADDR = (HOST, PORT)
    print(ADDR)

def recvall(sock, count):
    buf = b''
    while count:
        newbuf = sock.recv(count)

        if not newbuf: return None
        buf += newbuf
        count -= len(newbuf)
    return buf

client_sock = socket(AF_INET, SOCK_STREAM)
client_sock.connect(SocketInfo.ADDR)
print("connect is success")

a = [600, 600, 3]
b = np.prod(a)

# OpenCV code
while True:
    stringData = recvall(client_sock, b)
    data = np.frombuffer(stringData, dtype='uint8')
    rgb = np.reshape(data, a)

    cv2.namedWindow('Client_recv', cv2.WINDOW_NORMAL)
    cv2.imshow('Client_recv', rgb)

    key = cv2.waitKey(1)
    if key == 27:
        break

client_sock.close()
print("연결 종료")
exit()
