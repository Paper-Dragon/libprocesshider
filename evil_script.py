#!/usr/bin/python3
import socket
import sys
import time

def send_traffic(ip, port, delay=1):
    """
    Send a controlled flow of traffic to the specified IP and port.
    
    :param ip: IP address to send data to.
    :param port: Port number to send data to.
    :param delay: Time in seconds to wait between each send (default 1 second).
    """
    print(f"Sending controlled traffic to {ip}:{str(port)} every {delay} second(s)")
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    try:
        while True:
            sock.sendto(b"This is a controlled message", (ip, port))
            time.sleep(delay)
    except KeyboardInterrupt:
        print("\nTraffic sending interrupted by user.")
    finally:
        sock.close()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} IP PORT [DELAY]")
        sys.exit(1)
    
    ip = sys.argv[1]
    port = int(sys.argv[2])
    delay = 1
    if len(sys.argv) == 4:
        delay = float(sys.argv[3])
    
    send_traffic(ip, port, delay)