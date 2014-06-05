#!/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
## Central Trigger Crew, Johan Lundberg, 2009
#
# Get Partition name and running status: CleanStop, Stop or not stopped
# ---------------------------------------------------------------------
#
# arguments: any number of run numbers

import l1ctpy

run=0


def get_partition_name(runnr):
   try:
      from PyCool import cool
      import time
      folder=l1ctpy.getCoolFolder(l1ctpy.get_dbid_TDAQ_COMP(),'/TDAQ/RunCtrl/EventCounters')
      
      ROV1=cool.ValidityKey((run<<32)+1)
      ROV2=ROV1
      #
      objs=list(folder.browseObjects(ROV1,ROV2,cool.ChannelSelection.all()))
      return objs[0].payload()["PartitionName"]      
   except IndexError:
      return ""
#   except BaseException,e:    
#      print "Exception: ",e,type(e)
   return ""

def get_partition_ongoing(runnr):
   try:
      from PyCool import cool
      import time
      folder=l1ctpy.getCoolFolder(l1ctpy.get_dbid_TDAQ_COMP(),'/TDAQ/RunCtrl/EOR_Params')
      
      ROV1=cool.ValidityKey((run<<32)+1)
      ROV2=ROV1
      #
      objs=list(folder.browseObjects(ROV1,ROV2,cool.ChannelSelection.all()))

      if objs[0].payload()["CleanStop"] :
          return "CleanStop"
      else:
          return "NotCleanStop"      
   except IndexError:
      return ""
#   except BaseException,e:      
#      print "Exception: ",e
   return ""


import sys
firstarg=1
for r in sys.argv[1:]:
    run=int(r)
    print run, get_partition_name(run), get_partition_ongoing(run)

