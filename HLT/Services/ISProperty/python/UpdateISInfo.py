# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#
#update is information for ISPropTestAlg
#
from ispy import *

#get partition
p = IPCPartition('MyPartition')

#get data object
obj = ISObject(p,'MyServer.MyTestAlg','ISPropTestAlg')

ok=bool(False)
while not ok :
  try :
    myint = input("Enter your integer value:")
  except NameError, SyntaxError : continue 
  if type(myint) == type(int()):
    ok = True

ok=False
while not ok :
  try :
    mydel = input("Enter your delay in seconds:")
  except NameError, SyntaxError : continue 
  if (type(mydel) == type(int())) | (type(mydel) == type(float())):
    ok = True

obj.MyInt = str(myint)
obj.Delay = str(mydel)
obj.checkin(False)
