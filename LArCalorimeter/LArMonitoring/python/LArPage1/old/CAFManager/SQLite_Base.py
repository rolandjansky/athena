# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
import sqlite3 as sqlite
import os, time, traceback
from Logger import Logger

class SQLite_Base(Logger) : 

  def __init__(self, fileName, mode = '', isolation_level = '', stream = None) : # iso_lvl can be IMMEDIATE OR EXCLUSIVE
    Logger.__init__(self, stream)
    self.m_ok = False
    if mode == '' : 
      if not os.access(fileName, os.F_OK) : 
        self.ERROR('file not found : ' + fileName)
        return
      self.m_ok = True
    if mode == 'new' : 
      if os.access(fileName, os.F_OK) : 
        os.system('rm ' + fileName) 
      self.m_ok = True
    try:
      if isolation_level == '' :  
        self.m_connection = sqlite.connect(fileName)
      else :
        self.m_connection = sqlite.connect(fileName, isolation_level=isolation_level)
    except Exception, inst :
      traceback.print_exc()
      self.ERROR('Could not connect to DB file ' + fileName + ' : ' + str(inst))
      self.m_ok = True
      return
    self.m_cursor = self.m_connection.cursor();
    if mode == 'new' : self.m_ok = self.initialize()

  def execute(self, cmd, data = None) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    if data == None :
      try:
        return self.m_cursor.execute(cmd)
      except Exception, inst :
        return self.ERROR('When trying ' + cmd + ' : ' + str(inst))
    else :
      try:
        return self.m_cursor.execute(cmd, data)
      except Exception, inst :
        traceback.print_exc()
        return self.ERROR('When trying ' + cmd + ' ' + str(data) + ' : ' + str(inst))
  
  def fetchone(self, cmd, data = None) :
    try:
      result = self.execute(cmd, data)
      if result == False : return None
      return result.fetchone()
    except Exception, inst :
      return self.ERROR('When trying fetchone : ' + cmd + ' ' + str(data) + ' : ' + str(inst))

  def fetchall(self, cmd, data = None) :
    try:
      result = self.execute(cmd, data)
      if result == False : return ()
      return result.fetchall()
    except Exception, inst :
      return self.ERROR('When trying fetchall : ' + cmd + ' ' + str(data) + ' : ' + str(inst))

  def commit(self, errMsg) :
    if not self.m_ok :return self.ERROR('DB is not ready')
    try:
      self.m_connection.commit()
      return True
    except:
      if errMsg == '' : errMsg = 'DB Error: changes could not be committed'
      self.ERROR(errMsg)
    try :
      self.m_connection.rollback()
    except :
      return self.ERROR('DB Error: cannot access the DB file, probably due to fs problems - please try again')
    return False

  def rollback(self) :
    if not self.m_ok : return self.ERROR('DB is not ready')
    return self.m_connection.rollback()

  def initialize(self) :
    return True
         
  def size(self, tableName) :
    try:
      # here the ? replacement doesn't work..
      return self.fetchone('select count(*) from ' + tableName)[0]
    except:
      return -1

  def lastId(self, tableName) :
    result = self.fetchone('select * from sqlite_sequence where name=?', (tableName,))
    if result == None : 
      return 0
    return result[1]
  
  # ((1), (2), (3)) -> [ 1,2,3 ]
  def convertToSimpleList(self, t) :
    l = []
    for e in t :
      if len(e) != 1 : return None
      l.append(e[0])
    return l
