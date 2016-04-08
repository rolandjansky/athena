#! /usr/local/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Exception class that carries an optional datum, eg error string"""

class NewRTTError(Exception):
  def __init__(self, msg=''):
    try:
      self.msg = str(msg)
    except:
      self.msg = 'Input message not a string'
  def __str__(self): return self.msg
    
class RTTCodingError(NewRTTError):
    def __init__(self, msg):
        NewRTTError.__init__(self, msg)

class RTTInputError(NewRTTError):
    def __init__(self, msg):
        NewRTTError.__init__(self, msg)

class RTTSystemResponseError(NewRTTError):
    def __init__(self, msg):
        NewRTTError.__init__(self, msg)

class RTTTimeOut(NewRTTError):
    def __init__(self, msg):
        NewRTTError.__init__(self, msg)

