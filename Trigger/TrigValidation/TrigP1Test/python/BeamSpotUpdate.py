#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
athenaMT event modifier to test beamspot update.
On import of this module, it creates an sqlite file with the beamspot COOL folder
and an initial value. The udpates are configured through Config.hltCounterChange
and on each update a new IOV is added to the COOL folder. Use this together with
the BeamspotFromSqlite modifier in runHLT_standalone.py.

Usage: athenaMT -Z TrigP1Test.BeamSpotUpdate -c BeamspotFromSqlite=True ...
"""

import os
import sys
import logging
import eformat
from TrigByteStreamTools import trigbs_replaceLB
from TrigByteStreamTools import CTPfragment

class Config:
  """Configuration options for this module"""
  lb_updateBeamspot = {2:(2,4),        # LB : (LB of beamspot update, status)
                       4:(5,7),
                       6:(6,7)}  

trigbs_replaceLB.Config.eventsPerLB = 5

log = logging.getLogger(__name__)
log.info('Will perform beamspot udpate on these LBs (LB,status): %s' % sorted(Config.lb_updateBeamspot.values()))

folderList = []    # list of COOL folder updates

def setBeamSpot(run,lb,x,y,z,
                status=7,
                dbname='sqlite://;schema=beampos.db;dbname=CONDBR2',
                tag='IndetBeamposOnl-HLT-UPD1-001-00'):

   print('============================= Creating new beamspot in COOL ===================================')
   sys.stdout.flush()
   os.system("beamSpotOnl_set.py --rs=%d --ls=%d '%s' '%s' %d %f %f %f" % (run,lb,dbname,tag,status,x,y,z))
   print('===============================================================================================')
   sys.stdout.flush()   


def addFolderUpdate(event):
   """Add COOL folder update to event"""
   
   ctp_robs = [rob for rob in event if rob.source_id().subdetector_id()==eformat.helper.SubDetector.TDAQ_CTP]

   if len(ctp_robs)==0:
      log.error("Cannot find CTP ROB in event")
      return event

   # Copy event except CTP ROBs
   new_event = eformat.write.FullEventFragment()
   new_event.copy_header(event)
   for r in event:
      if r.source_id().subdetector_id() != eformat.helper.SubDetector.TDAQ_CTP:
         new_event.append(eformat.write.ROBFragment(r))

   # Set the CTP extra payload      
   for rob in ctp_robs:
      x = CTPfragment.ExtraPayload()  # start from empty payload object
      for f in folderList: x.updateFolder(f)
      new_ctp_rob = CTPfragment.setHltExtraPayloadWords(rob, [d for d in x.serialize()])
      new_event.append(eformat.write.ROBFragment(new_ctp_rob))
      
   return new_event

   
def modify(event):
   event = eformat.write.FullEventFragment(event)
    
   # Modify LB and HLT counter in CTP fragment
   newevt = trigbs_replaceLB.modify(event)
   lb = newevt.lumi_block()

   # Set new beamspot (randomized by LB number)   
   if lb in Config.lb_updateBeamspot:
      lb_for_update,status = Config.lb_updateBeamspot.pop(lb)
      setBeamSpot(newevt.run_no(), lb_for_update,
                  lb/100.0, 1+lb/100.0, -4-lb/10.0,
                  status)

      fe = CTPfragment.FolderEntry()
      fe.folderIndex = 0  # /Indet/Onl/Beampos
      fe.lumiBlock = lb_for_update
      folderList.append(fe)
      log.info('Added COOL folder update to event: folderIndex=%d, LB=%d' % (fe.folderIndex,fe.lumiBlock))
      
   return addFolderUpdate(newevt).readonly()

# Create an open-ended IOV with a default beamspot
# This happens at the very beginning when athenaMT/PT imports this module
setBeamSpot(1,0,0.06,1.06,-4.6,4)

# For standalone running and writing a new output file
if __name__ == '__main__':
  import argparse
  from eformat import EventStorage

  parser = argparse.ArgumentParser(description=__doc__)

  parser.add_argument('file', metavar='FILE', nargs=1, help='input file')
  args = parser.parse_args()
  dr = EventStorage.pickDataReader(args.file[0])
  output = eformat.ostream(core_name = 'test',
                           run_number = dr.runNumber(),
                           trigger_type = dr.triggerType(),
                           detector_mask = dr.detectorMask(),
                           beam_type = dr.beamType(),
                           beam_energy = dr.beamEnergy(),
                           meta_data_strings=dr.freeMetaDataStrings(),
                           compression=dr.compression())

  for event in eformat.istream(args.file[0]):
    newevt = modify(event)
    output.write(newevt)
