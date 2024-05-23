from scapy.all import *
import time

# Modbus ADU
class ModbusTCP(Packet):
    name = "Modbus/TCP"
    fields_desc = [ ShortField("Transaction Identifier", 1),
                    ShortField("Protocol Identifier", 0),
                    ShortField("Length", 6),
                    XByteField("Unit Identifier", 247),
                    ]

# Modbus PDU
class Modbus(Packet):
    name = "Modbus"
    fields_desc = [ XByteField("Function Code", 4),
                    ShortField("Reference Number", 1),
                    ShortField("Word Count", 2),
                    ]

# Create a socket and connect
s = socket.socket()
s.connect(("192.168.43.11", 502))   # IP and port
ss = StreamSocket(s, Raw)

while True:
    try:
        # Encapsulate modbus inside the raw data, then send and receive
        # Anything can be done with response
        ss.sr1(Raw(ModbusTCP()/Modbus()))
        time.sleep(1)
    except KeyboardInterrupt:
        break