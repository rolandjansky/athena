#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""Modify lumiblock, run number and other events quantities."""

import eformat
from eformat import helper
import logging
import sys
import re

class Config:
  """Configuration options for this module"""
  firstLB = 1                 # first LB number
  incLB = 1                   # step-size to increment LB
  eventsPerLB = None          # events per LB
  runNumber = None            # modify run number if set
  bc_sec = None               # event timestamp

class Store:
  """Global variables"""
  eventCounter = 0
  currentLB = 1

logging.basicConfig(level=logging.INFO)
log = logging.getLogger(__name__)

def modify(event):
  from TrigByteStreamTools import CTPfragment

  if Store.eventCounter==0:
    Store.currentLB = Config.firstLB

  newevt = event if isinstance(event,eformat.write.FullEventFragment) else eformat.write.FullEventFragment(event)
  Store.eventCounter += 1

  if Config.eventsPerLB is not None:
    Store.currentLB = Config.firstLB + Config.incLB*((Store.eventCounter-1) // Config.eventsPerLB)

    # Find CTP ROB
    ctp_robs = [rob for rob in newevt.children()
                if rob.source_id().subdetector_id() == helper.SubDetector.TDAQ_CTP]

    # Modify LB in CTP fragmnet
    for ctp_rob in ctp_robs:
      CTPfragment.setLumiBlock(ctp_rob, Store.currentLB)

    # Set LB in event header
    newevt.lumi_block(Store.currentLB)

  # Set run number
  if Config.runNumber is not None:
    newevt.run_no(Config.runNumber)

  # Modify event time stamp
  if Config.bc_sec is not None:
    newevt.bc_time_seconds(Config.bc_sec)
    newevt.bc_time_nanoseconds(0)

  log.info("Event %4d, L1ID %10d assigned LB = %d, run = %d, bc_time_sec = %d",
           Store.eventCounter, newevt.lvl1_id(), newevt.lumi_block(), newevt.run_no(), newevt.bc_time_seconds())

  return newevt if isinstance(event,eformat.write.FullEventFragment) else newevt.readonly()


def main():
  import argparse

  parser = argparse.ArgumentParser(description=__doc__)

  parser.add_argument('file', metavar='FILE', nargs=1, help='input file')
  parser.add_argument('-o', '--output', type=str, help='output base file name')
  parser.add_argument('-n', '--events', type=int, default=-1, help='number of events to process')
  parser.add_argument('-r', '--runNumber', type=int, help='set run number')
  parser.add_argument('-l', '--eventsPerLB', type=int, help='increment lumiblock number in steps [%(default)s]')
  parser.add_argument('--firstLB', type=int, default=1, help='first lumiblock number [%(default)s]')
  parser.add_argument('--incLB', type=int, default=1, help='increment steps for lumiblock number [%(default)s]')
  parser.add_argument('-t', '--timestamp', type=int, help='set timestamp in seconds [%(default)s]')
  parser.add_argument('--removeRobs', metavar='PATTERN', type=str, help='regex for removing specific ROB IDs')

  args = parser.parse_args()

  Config.firstLB = Store.currentLB = args.firstLB
  Config.incLB = args.incLB
  Config.eventsPerLB = args.eventsPerLB
  Config.runNumber = args.runNumber
  Config.bc_sec = args.timestamp

  log.info('Opening file %s', args.file[0])
  dr = eformat.EventStorage.pickDataReader(args.file[0])
  ostr = eformat.ostream(core_name = args.output or dr.fileNameCore(),
                         run_number = Config.runNumber or dr.runNumber(),
                         trigger_type = dr.triggerType(),
                         detector_mask = dr.detectorMask(),
                         beam_type = dr.beamType(),
                         beam_energy = dr.beamEnergy())

  bsfile = eformat.istream([args.file[0]])
  i = 0
  for event in bsfile:
    i +=1
    if args.events>0 and i>args.events:
      break
    ro_event = modify(event)
    if args.removeRobs:
      rw_event = eformat.write.FullEventFragment(ro_event, re.compile(args.removeRobs))
    else:
      rw_event = eformat.write.FullEventFragment(ro_event)
    ostr.write(rw_event)

  return

if __name__ == "__main__":
  sys.exit(main())
