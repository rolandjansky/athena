#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
## ------------------------------------
## E.Gallas May 2012
##
## function to return a dictionary of SMKs keyed by Run number
## ------------------------------------
import sys
from PyCool import cool

def Get_SMK_by_Run_Range(run1,run2):
  """For the input Run number range, fill a dictionary with SMKs keyed by Run"""
  ##
  ## COOL Connections
  ##
  dbSvc=cool.DatabaseSvcFactory.databaseService()
  coolname = 'COOLONL_TRIGGER/COMP200'
  cooldb=dbSvc.openDatabase(coolname)
  ##
  folder_SMK = cooldb.getFolder('/TRIGGER/HLT/HltConfigKeys')
  ##
  CoolChAll = cool.ChannelSelection.all()
  itr = folder_SMK.browseObjects(run1<<32,run2<<32,CoolChAll)
  ##
  ## loop over iterator, filling dictionary of SMK keyed by Run
  ##
  SMK_DICT = {}
  while itr.goToNext():
    obj     = itr.currentRef()
    run     = int(obj.since() >> 32)
    payload = obj.payload()
    SMK = payload['MasterConfigurationKey']
    SMK_DICT[run] = int(SMK)
  ##
  ## close the iterator
  ##
  itr.close()
  ##
  ## close the COOL connection
  ##
  cooldb.closeDatabase()
  ##
  ## return the dictionary
  ##
  return SMK_DICT
##
## ------------------------------------
if __name__ == '__main__':
  run1 = 191373
  run2 = 200220
  SMK_DICT = Get_SMK_by_Run_Range(run1,run2+1)
  print str(SMK_DICT)
  sys.exit('\n\tNormal successful completion.\n')

