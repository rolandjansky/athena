#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Module for creating the MUCTPi fragment normally send through the RoIB
using the information readout from the ROS.
It can be used either standalone or as a "-Z" plugin to athenaMT/PT.

Note that a bunch of information is hardcoded and could change if
the data format of the CTP/MUCTPI/L1Calo ROIB fragments change.
"""

import os
import sys

import eformat
import libpyevent_storage as EventStorage

def modify(event):
  """Fills in the MUCTPi ROB)."""

  map=[[eformat.helper.SubDetector.TDAQ_MUON_CTP_INTERFACE,0,1]]

  DaqRobs=[None]*len(map)
  
  for rob in event:
      cnt=0
      for match in map:
          if rob.source_id()==eformat.helper.SourceIdentifier(match[0],match[1]):
              DaqRobs[cnt]=rob
          cnt+=1


  #work around corrupted events:
  #new_event=eformat.write.FullEventFragment(event)
  new_event=eformat.write.FullEventFragment()
  new_event.copy_header(event)
  for rob in event:
    keep=True
    for match in map:
      if rob.source_id()==eformat.helper.SourceIdentifier(match[0],match[2]):
        keep=False
        break
    if keep:
      new_event.append_unchecked(rob)
  for idx in range(len(map)):
      if DaqRobs[idx]:
          rob=eformat.write.ROBFragment(DaqRobs[idx])
          rob.source_id(eformat.helper.SourceIdentifier(map[idx][0],map[idx][2]))
          if map[idx][0]==eformat.helper.SubDetector.TDAQ_CTP:
              version=rob.rod_minor_version()&0x1f
              if version:
                ctpSkip=(rob.rod_minor_version()>>8)&0x3f
              else:
                ctpSkip=(rob.rod_detev_type()>>16)&0x3f
              data=[DaqRobs[idx].rod_data()[0],DaqRobs[idx].rod_data()[1]]
              for ii in range(30):
                  data+=[DaqRobs[idx].rod_data()[2+ctpSkip*30+ii]]
              rob.rod_data(data)
          if map[idx][0]==eformat.helper.SubDetector.TDAQ_MUON_CTP_INTERFACE:
              data=[]
              if len(DaqRobs[idx].rod_data()):
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


def ReplaceMUCTPI(input_file,output_file):
  input = eformat.istream([input_file])
  dr=EventStorage.pickDataReader(input_file)
  output = eformat.ostream(core_name="subset",
                           run_number=dr.runNumber(),
                           trigger_type=dr.triggerType(),
                           detector_mask=dr.detectorMask(),
                           beam_type=dr.beamType(),
                           beam_energy=dr.beamEnergy())
  for event in input:
    output.write(modify(event))
  tmp_file_name = output.last_filename()
  del output
  os.rename(tmp_file_name,output_file)



if __name__ == "__main__":
  if len(sys.argv)!=3:
    print 'usage:',sys.argv[0],'<infile> <outfile>'
    sys.exit(1)
  input_file = sys.argv[1]
  output_file = sys.argv[2]

  ReplaceMUCTPI(input_file,output_file)
