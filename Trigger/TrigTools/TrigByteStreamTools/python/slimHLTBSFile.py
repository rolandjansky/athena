#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
slimHLTBSFile.py

Module for removing raw and time/quality information from LAr data,
raw digits from Tile data, preprocessor information from L1Calo data and
pre-existing L2 and EF results. None of this information is currently used by
the HLT and removing it reduces file size by up to a factor 4.

Note that this requires datafiles to have energy readings
"""

import os
import sys

import eformat
import libpyevent_storage as EventStorage

def hi16(word):
  return (word&0xffff0000)>>16

def lo16(word):
  return word&0xffff

def reducedLARFEB(data):
  sizeenergy=lo16(data[3])
  offenergy=hi16(data[3])
  nsamples=lo16(data[7])
  radd=(nsamples+1)//2
  total=18-8+83+radd+3
  if total>data[0] or sizeenergy==0:
    return [data[ii] for ii in range(data[0])]
  newdata=[data[ii] for ii in range(total-3)+range(data[0]-3,data[0])] #one should actually recalculate checksum
  newdata[0]=total
  newdata[3]=(18<<16)+83+radd
  newdata[4]=0 #no quality
  newdata[5]=0 #no samples
  for ii in range(offenergy,offenergy+9):
    newdata[ii]=0 #zero masks
  return newdata

def reducedTILE(data):
  idx=0
  newdata=[]
  while idx<len(data):
    size=data[idx+1]
    type=data[idx+2]>>16&0xff
    if type>1:
      newdata+=[data[ii] for ii in range(idx,idx+size)]
    idx+=size
  return newdata

def modify(event):
  new_event=eformat.write.FullEventFragment()
  new_event.copy_header(event)
  for rob in event:
    newrob=eformat.write.ROBFragment(rob)
    if rob.source_id().human_detector()=='TDAQ_CALO_PREPROC': continue
    if rob.source_id().human_detector()=='TDAQ_LVL2': continue
    if rob.source_id().human_detector()=='TDAQ_EVENT_FILTER': continue
    if rob.source_id().human_group()=="TILECAL":
      data = rob.rod_data()
      newdata=reducedTILE(data)
      newrob.rod_data(newdata)

    if rob.source_id().human_group()=="LAR" and rob.rod_minor_version()==11:
      #should check that it really is physics format
      data = rob.rod_data()
      newdata=reducedLARFEB(data)
      if len(data)>data[0]:
        newdata+=[data[data[0]+ii] for ii in range(7)] #middle "ROD" header
        data=data[data[0]+7:]
        newdata+=reducedLARFEB(data)
      newrob.rod_data(newdata)
    new_event.append(newrob)
  return new_event.readonly()

if __name__ == "__main__":
  if len(sys.argv)!=3:
    print 'usage:',sys.argv[0],'<infile> <outfile>'
    sys.exit(1)
  input_file = sys.argv[1]
  output_file = sys.argv[2]

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
