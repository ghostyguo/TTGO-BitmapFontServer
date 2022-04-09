# -*- coding: utf-8 -*-
'''
-----------------
BitmapFont Server 
-----------------

Created on Mon Apr  4 19:44:19 2022

@author: ghosty
'''
import cv2
import numpy as np
import socket
from PIL import ImageFont, ImageDraw, Image
import threading
import time

HOST = '0.0.0.0'
PORT = 8080

global isServerRunning
global fontServerThread, showBitmapThread
global server
global img

isServerRunning = False
    
def getTextWidth(text):
    width = 0
    for _char in text:
        if not '\u4e00' <= _char <= '\u9fa5':
            width += 1
        else:
            width += 2
    return width

# =========================
# ShowBitmap
# =========================
def ShowBitmap():
    print('*** showBitmap()')
    global img, isServerRunning
    time.sleep(0.1) # wait for server up
    
    while (isServerRunning):          
        if (img.shape[0]>0 and img.shape[1]>0):
            cv2.imshow('Bitmap',img)
            key = cv2.waitKey(10) & 0xFF
            if key == ord('q') or key==27: 
                break
        else:
            break
        time.sleep(0.1)
    cv2.destroyAllWindows()
    print('*** showBitmap() Stopped')    

# =========================
# font Client
# =========================
def onNewClient(client,addr):    
    size = 16
    while True:
        inByte = client.recv(1024)
        if len(inByte) == 0: # connection closed
            client.close()
            break
        inText = inByte.decode()
        print('recv: ' + inText)
            
        # parse command
        commands = inText.split('\n')
        for command in commands:
            #size ?
            if (command[0:5]=='size='):
                size = int(command[5:])
                print('*** size=',size)
            # text?
            if (command[0:5]=='text='):
                text = command[5:]
                print('*** text=\"%s\" (%d)' % (text, len(text)))
                
        # show bitmap
        global img
        if len(text)==0:
            print('No Text')
        else:            
            #
            height = size
            width = (int)(getTextWidth(text)*size/2)            
            print('*** height=',height,', width=',width)
            img = np.zeros((height,width,3), np.uint8)
                    
            ## Use simsum.ttc to write Chinese.
            fontpath = './mingliu.ttc'     
            font = ImageFont.truetype(fontpath, size)
            img_pil = Image.fromarray(img)
            draw = ImageDraw.Draw(img_pil)
            draw.text((0, 0),  text, font = font, fill = (255, 255, 255, 255))
            img = np.array(img_pil)    

            pixelByte = bytearray(b'0')*(width+1)*height
            #print(pixelByte)
            for y in range(height):
                for x in range(width):          
                    if (img[y][x][0]>0):
                        pixelByte[y*(width+1)+x] = 49 #'1'
                    #else: 
                    #    pixelByte[y*(width+1)+x] = '0'
                pixelByte[y*(width+1)+width] = 10 #'\n'
            #print(pixelByte)               
            outText = str(width)+'\n'+str(height)+'\n' + pixelByte.decode()
            #print('outText=',outText)             
            client.send(outText.encode())
    # While True:        
    client.close()
    print('client closed connection.')    
    
# =========================
# font server
# =========================
def FontServer():
    global isServerRunning, server

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((HOST, PORT))
    server.listen(5)
    
    isServerRunning = True
    print('server start at: %s:%s' % (HOST, PORT))
    print('wait for connection...')
    while isServerRunning:
        try:
            c, addr = server.accept()     # Establish connection with client.        
            newClientThread = threading.Thread(target = onNewClient(c,addr))
            newClientThread.start()
        except KeyboardInterrupt:
            isServerRunning = False
        except:
            server.close()
    
    print('*** FontServer Stopped')            
    #server.close()
    
# =========================
# font server
# =========================
def StopServer():
    global isServerRunning
    global fontServerThread, showBitmapThread
    global server, img
    isServerRunning = False
    server.close()
    fontServerThread.join(1)
    showBitmapThread.join(1)
    print('fontServerThread alive:',fontServerThread.is_alive())
    print('showBitmapThread alive',showBitmapThread.is_alive())
    del globals()['fontServerThread']
    del globals()['showBitmapThread']
    del globals()['server']
    del globals()['isServerRunning']
    del globals()['img']
    
if __name__ == '__main__':     
    global fontServerThread, showBitmapThread
    # start font server
    if (not 'fontServerThread' in globals()):
        fontServerThread = threading.Thread(target = FontServer)
        fontServerThread.start()
    elif (not fontServerThread.is_alive()):        
        fontServerThread.start()
    isServerRunning = True
    
    # show bitmap    
    if (not 'showBitmapThread' in globals()):
        img = np.zeros((16,16,3), np.uint8)
        showBitmapThread = threading.Thread(target = ShowBitmap)
        showBitmapThread.start()
    elif (not showBitmapThread.is_alive()):        
        showBitmapThread.start()
    
    