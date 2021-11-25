import socket
import threading
from threading import Thread
from socket import *


class MessageReceiver(Thread):


    def run(self):
        print("Starting run()")
        self.s = socket(AF_INET, SOCK_DGRAM)
        self.s.bind(('', 4210))
        print("socket bound")

        while True:
            m = self.s.recvfrom(1024)
            print(m)

    def __init__(self):
        super().__init__()
        self.run()
        print("MessageReceiver running")


currentMessageFrequency = 1




def requestFunc():
    s = socket()
    s.bind(('0.0.0.0', 8090))
    s.listen(10)
    print("now listening")
    while True:
        client, addr = s.accept()
        while True:
            content = client.recv(32)
            if len(content) == 0:
                break
            else:
                print(content)
                client.send("20".encode("utf-8"))
        print("Closing connection")
        client.close()


t = threading.Thread(target=requestFunc)
t.start()
receiver = MessageReceiver()
