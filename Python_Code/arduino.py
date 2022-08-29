from flask import Flask
import socket
import threading
import logging

HOST = socket.gethostbyname(socket.gethostname())
PORT = 8000

class Arduino:
    def __init__(self, host= HOST, port= PORT):
        self.app = Flask(__name__)
        self.connected = False
        self.data = {}
        self.thread = threading.Thread(target=lambda: self.app.run(host=HOST, port=PORT, use_reloader=False), daemon=True)
        self.url = f"http://{host}:{port}/"

        log = logging.getLogger('werkzeug')
        log.disabled = True
        self.app.logger.disabled = True

    def connect(self):
        if not self.connected:
            try:
                @self.app.route('/')
                def home_page():
                    return self.data

                self.thread.start()
                print(f"Streaming data to url: http://{HOST}:{PORT}/")
                self.connected = True
                return True
            except:
                pass
        return False

    def sendData(self, data: dict):
        self.data = data

if __name__=='__main__':
    a = Arduino()
    a.connect()