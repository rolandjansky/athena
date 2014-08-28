#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Module for creating the rob fragments normally send through the RoIB
using the information readout from the ROSs.
It can be used either standalone or as a "-Z" plugin to athenaMT/PT.

Note that a bunch of information is hardcoded and could change if
the data format of the CTP/MUCTPI/L1Calo ROIB fragments change.

The payload of the CTP DAQ ROB (0x770000) is as follows:
  1 x NumberTimeWords
  N x CTPdataformat.DAQwordsPerBunch
  1 x numberExtraPayloadWords
This differs from the CTP ROB (0x770001) where only one bunch crossing is saved (N=1).
"""

import os
import sys

import logging
import eformat
import libpyevent_storage as EventStorage
from TrigByteStreamTools import CTPfragment
from TrigByteStreamTools.CTPfragment import CTPdataformat

def modify(event):
  """Fills in the L1R from the most appropriate source (L1/ROS or Dummy)."""

  map=[[eformat.helper.SubDetector.TDAQ_CTP,0,1],
       [eformat.helper.SubDetector.TDAQ_MUON_CTP_INTERFACE,0,1],
       [eformat.helper.SubDetector.TDAQ_CALO_CLUSTER_PROC_ROI,136,168],
       [eformat.helper.SubDetector.TDAQ_CALO_CLUSTER_PROC_ROI,137,169],
       [eformat.helper.SubDetector.TDAQ_CALO_CLUSTER_PROC_ROI,138,170],
       [eformat.helper.SubDetector.TDAQ_CALO_CLUSTER_PROC_ROI,139,171],
       [eformat.helper.SubDetector.TDAQ_CALO_JET_PROC_ROI,140,172],
       [eformat.helper.SubDetector.TDAQ_CALO_JET_PROC_ROI,141,173]]

  DaqRobs=[None]*len(map)
  L2Robs=[None]*len(map)
  

  for rob in event:
    cnt=0
    for match in map:
      if rob.source_id()==eformat.helper.SourceIdentifier(match[0],match[1]):
        DaqRobs[cnt]=rob
      elif rob.source_id()==eformat.helper.SourceIdentifier(match[0],match[2]):
        L2Robs[cnt]=rob
      cnt+=1

  # There are no CTP ROBs, event can not be recovered and should be skipped
  if (not DaqRobs[0]) and (not L2Robs[0]):
    logging.warning(' Plugin "addL1": No DAQ CTP fragment. Event can not be recovered. Event will be skipped. L1 id = %d, Global id = %d ' % (event.lvl1_id(), event.global_id()))
    return False

  # Workaround for corrupted events
  new_event=eformat.write.FullEventFragment()
  new_event.copy_header(event)
  for rob in event:
    new_event.append_unchecked(rob)
    
  for idx in range(len(map)):
      if L2Robs[idx]:
        pass
      elif DaqRobs[idx]:
        
        rob=eformat.write.ROBFragment(DaqRobs[idx])
        rob.source_id(eformat.helper.SourceIdentifier(map[idx][0],map[idx][2]))

        daq_data = [r for r in rob.rod_data()]

        if idx==0:  #CTP DAQ ROD
          # offset to triggered bunch trigger words
          offset = CTPdataformat.NumberTimeWords + CTPfragment.lvl1AcceptBunch(rob)*CTPdataformat.DAQwordsPerBunch
          # Copy leading words (once per fragment)
          data = daq_data[:CTPdataformat.NumberTimeWords]
          # Copy per bunch words
          data += daq_data[offset : offset+CTPdataformat.DAQwordsPerBunch]
          # Copy extra payload
          data += daq_data[-CTPfragment.numberExtraPayloadWords(rob):]                            
          rob.rod_data(data)

        elif idx==1:  #MuCTPi DAQ ROB
          data=[]
          if len(DaqRobs[idx].rod_data()) > 0:
            muctpBC=(DaqRobs[idx].rod_data()[0]>>18)&7
            for word in DaqRobs[idx].rod_data()[1:]:
              if (word>>14)&7==muctpBC and (word>>26)&1:
                data+=[(word&0x3fff)|(((word>>17)&0x1ff)<<14)]
                      
          if len(data)==0:
            rob=eformat.write.ROBFragment()
            rob.copy_header(DaqRobs[idx])
            rob.source_id(eformat.helper.SourceIdentifier(map[idx][0],map[idx][2]))
          else:
            rob.rod_data(data)
            
        new_event.append(rob)
      else:
        rob=eformat.write.ROBFragment()
        rob.source_id(eformat.helper.SourceIdentifier(map[idx][0],map[idx][2]))
        new_event.append(rob)
        
  return new_event.readonly()

def CTPreco(input_file,output_file):
  input = eformat.istream([input_file])
  dr=EventStorage.pickDataReader(input_file)
  output = eformat.ostream(core_name="subset",
                           run_number=dr.runNumber(),
                           trigger_type=dr.triggerType(),
                           detector_mask=dr.detectorMask(),
                           beam_type=dr.beamType(),
                           beam_energy=dr.beamEnergy())
  for event in input:
    new_event = modify(event)
    if not new_event:
      continue
    else:
      output.write(new_event)
  tmp_file_name = output.last_filename()
  del output
  os.rename(tmp_file_name,output_file)



if __name__ == "__main__":
  if len(sys.argv)!=3:
    print 'usage:',sys.argv[0],'<infile> <outfile>'
    sys.exit(1)
    
  CTPreco(sys.argv[1],sys.argv[2])
