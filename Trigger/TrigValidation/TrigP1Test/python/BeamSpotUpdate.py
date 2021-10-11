#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Module to test beamspot updates using a local sqlite file. Two steps are necessary:

1) Create a modified bytestream file that contains the correct COOL update information
   in the CTP fragment. To do this simply run this script with a RAW file and it will
   write a modified file into the current directory with beamspot updates on LBs as
   specified in Config.lb_updateBeamspot.

2) Configure a job with the BeamSpotWriteAlg in the topSequence. This algorithm will
   write a new beamspot into the sqlite file one LB before the actual update is
   triggered via the CTP fragment. See TrigP1Test/share/testHLT_beamspot.py
   for an example.
"""

import os
import sys
import logging
import eformat
from TrigByteStreamTools import trigbs_modifyEvent
from TrigByteStreamTools import CTPfragment
from AthenaPython import PyAthena

class Config:
   """Configuration options for this module"""
   lb_updateBeamspot = {3:(4,4),        # LB : (LB of beamspot update, status)
                        4:(5,7),
                        8:(9,7)
   }

trigbs_modifyEvent.Config.eventsPerLB = 5
folderList = []    # list of COOL folder updates
log = logging.getLogger(__name__)

class BeamSpotWriteAlg(PyAthena.Alg):
   """Algorithm to write a new Beamspot to COOL (sqlite file) for given LBs"""

   def __init__(self, name='BeamSpotWriteAlg', **kwargs):
      super(BeamSpotWriteAlg, self).__init__(name, **kwargs)
      setup()
      return

   def execute(self):
      if self.getContext() is None:
         self.log.info('No EventContext available')
         return PyAthena.StatusCode.Success

      # Write beamspot to sqlite one LB before the update appears in the
      # CTP fragmment. Beamspot position derived from LB number.
      run = self.getContext().eventID().run_number()
      lb = self.getContext().eventID().lumi_block() + 1
      if lb in Config.lb_updateBeamspot:
         l, status = Config.lb_updateBeamspot.pop(lb)
         setBeamSpot(run, l, l/100.0, 1+l/100.0, -4-l/10.0, status)

      return PyAthena.StatusCode.Success


def setBeamSpot(run,lb,x,y,z,
                status=7,
                dbname='sqlite://;schema=beampos.db;dbname=CONDBR2',
                tag='IndetBeamposOnl-HLT-UPD1-001-00'):

   log.info('============================= Creating new beamspot in COOL ===================================')
   log.info('run=%d, lb=%d, x=%f, y=%f, z=%f', run, lb, x, y, z)
   sys.stdout.flush()
   os.system("beamSpotOnl_set.py --output=sqlite:beampos.db --run=%d --lbn=%d %d %f %f %f" % (run,lb,status,x,y,z))
   if log.level<=logging.DEBUG:
      log.info('Current content of beampos.db sqlite file:')
      os.system("AtlCoolConsole.py 'sqlite://;schema=beampos.db;dbname=CONDBR2' <<< 'more Indet/Onl/Beampos'")
   log.info('===============================================================================================')
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
   newevt = trigbs_modifyEvent.modify(event)
   lb = newevt.lumi_block()

   # Write conditions update into CTP fragment
   if lb in Config.lb_updateBeamspot:
      lb_for_update,status = Config.lb_updateBeamspot.pop(lb)

      fe = CTPfragment.FolderEntry()
      fe.folderIndex = 0  # /Indet/Onl/Beampos
      fe.lumiBlock = lb_for_update
      folderList.append(fe)
      log.info('Added COOL folder update to event: folderIndex=%d, LB=%d', fe.folderIndex,fe.lumiBlock)
      
   return addFolderUpdate(newevt).readonly()


def setup():
   """Initial setup"""

   log.info('Will perform beamspot udpate on these LBs (LB,status): %s', sorted(Config.lb_updateBeamspot.values()))

   # Delete any previous sqlite file
   try:
      os.remove('beampos.db')
   except OSError:
      pass

   # Create an open-ended IOV with a default beamspot
   setBeamSpot(1,0,0.06,1.06,-4.6,4)


# For standalone running and writing a new output file
if __name__ == '__main__':
   import argparse
   from eformat import EventStorage

   parser = argparse.ArgumentParser(description=__doc__)
   parser.add_argument('file', metavar='FILE', nargs=1, help='input file')
   parser.add_argument('-n', '--events', type=int, default=-1, help='number of events to process')
   parser.add_argument('-o', '--output', type=str, help='core output file name')

   args = parser.parse_args()
   dr = EventStorage.pickDataReader(args.file[0])
   output = eformat.ostream(core_name = args.output or dr.fileNameCore(),
                            run_number = dr.runNumber(),
                            trigger_type = dr.triggerType(),
                            detector_mask = dr.detectorMask(),
                            beam_type = dr.beamType(),
                            beam_energy = dr.beamEnergy(),
                            meta_data_strings=dr.freeMetaDataStrings(),
                            compression=dr.compression())

   i = 0
   for event in eformat.istream(args.file[0]):
      i += 1
      if args.events>0 and i>args.events:
         break
      newevt = modify(event)
      output.write(newevt)
