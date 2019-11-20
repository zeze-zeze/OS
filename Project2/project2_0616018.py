#coding=utf-8
import hashlib
import socket
import thread
import random
import string
from multiprocessing import Process

CONNECTION_NUMBER = 100
POW_DIFFICULTY = 4
PROC_THREAD = 50

class Server():
  def __init__(self, connectionNumber, PoWDifficulty, procThread): 
    self.connectionNumber = connectionNumber
    self.PoWDifficulty = PoWDifficulty
    self.procThread = procThread

    self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.server.bind(('127.0.0.1', 20000))
    self.server.listen(self.connectionNumber)

    self.message = [''] * self.connectionNumber
    self.seed = [''] * self.connectionNumber
    self.hash = [''] * self.connectionNumber

    self.printable = string.uppercase + string.lowercase + '0123456789'
  
  def start(self):
    self.mul_proc()

  # multiprocess for each connection
  def mul_proc(self):
    allpro = []
    for index in range(self.connectionNumber):
      p = Process(target = self.proc_con, args = (index,))
      allpro.append(p)
      p.start()
    for p in allpro:
      p.join()

  # each process receive the message and send the seed
  def proc_con(self, index):
    conn, addr = self.server.accept()
    #print addr
    while 1:
      self.message[index] = ''
      self.message[index] = conn.recv(6).strip()
      #print '[{}]: {}'.format(index, self.message[index])
      self.seed[index] = ''
      self.hash[index] = ''
      self.mul_thread(index)
      while not self.seed[index]:
        continue
      response = '{message},{seed},{hash_}\n'.format(message = self.message[index], seed = self.seed[index], hash_ = self.hash[index])
      try:
        conn.send(response)
      except:
        return
  
  # multhread for calculating hash parrallely
  def mul_thread(self, index):
    for i in range(self.procThread):
      thread.start_new_thread(self.cal_hash, (index,))
    
  # calculate hash for corresponding message
  def cal_hash(self, index):
    while 1:
      if self.seed[index]:
        return
      guess = ''.join(random.sample(self.printable, 5))
      res = hashlib.sha256(guess + self.message[index]).hexdigest()
      if (res[:self.PoWDifficulty] == '0' * self.PoWDifficulty) and (not self.seed[index]) and (not self.hash[index]):
        self.hash[index] = res
        self.seed[index] = guess
        #print('message: {}, seed: {}, hash: {}'.format(self.message[index], self.seed[index], self.hash[index]))

s = Server(CONNECTION_NUMBER, POW_DIFFICULTY, PROC_THREAD)
s.start()
