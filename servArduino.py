SERIAL_PORT_NAME = '/dev/tty3'
SERIAL_PORT_SPEED = 9600

WEB_SERVER_PORT = 8000

import time, BaseHTTPServer, urlparse
import serial
import time
import socket
ser = None

def main():

    global ser
    
    httpd = BaseHTTPServer.HTTPServer(("", WEB_SERVER_PORT), Handler)

    #-- workaround for getting IP address at which serving
    
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(('google.com', 80))
    sData = s.getsockname()

    print "Serving at '%s:%s'" % (sData[0], WEB_SERVER_PORT)
    
    ser = serial.Serial(SERIAL_PORT_NAME, SERIAL_PORT_SPEED, timeout=0.03)

    
    httpd.serve_forever()

    

class Handler(BaseHTTPServer.BaseHTTPRequestHandler):

    # Disable logging DNS lookups
    def address_string(self):
        return str(self.client_address[0])

    def do_GET(self):

        if self.path == '/' or self.path == '/index.html':
            self.process_index()
        #elif self.path == '/get_data':
        #    self.get_data()
        elif self.path == '/get_serial':
            self.process_get_serial()
        else:
            self.process_unknown()

    
    def process_index(self):            
    
        self.send_response(200)
        self.send_header("Content-type", "text/html; charset=utf-8")
        self.end_headers()
        
        self.wfile.write(open('index.html','r').read())
        
        self.wfile.write("\n\n")
        self.wfile.flush()

        
    def process_get_serial(self):
        count = 0.0
        self.send_response(200)
        self.send_header("Content-type", "text/event-stream")
        self.end_headers()

        try:            
            
            while True:
                try:
                    handle = open("dataTest.txt", 'a')
                except IOError:
                    print("An IOError has occurred!")                                                  
                new_serial_line = get_full_line_from_serial()
                if count < 1000.0:
                    #now = datetime.strftime(datetime.now(), "%Y.%m.%d %H:%M:%S")
                    now = str(time.time())
                    count += 0.1
                    if new_serial_line is not None:
                        print(count)
                        handle.write(now)
                        handle.write(',')
                        handle.write(new_serial_line)
                        handle.write('\n')
                        handle.close()
                        self.wfile.write(now)
                        self.wfile.write(',')
                        self.wfile.write(new_serial_line)                        
                        self.wfile.write("\n\n")
                        self.wfile.flush()
                        
                else:
                    count = 0
                    handle.close()                

        except socket.error, e:
            print "Client disconnected.\n"


    def process_unknown(self):
        self.send_response(404)            


captured = ''
def get_full_line_from_serial():
    """ returns full line from serial or None 
        Uses global variables 'ser' and 'captured'
    """
    global captured
    
    part = ser.readline()
    if "," in part: 
        captured += part
        parts = captured.split('\n', 1);
        if len(parts) == 2:
            captured = parts[1]
            return parts[0]
            
    return None
    
    
if __name__ == '__main__':
    main()
