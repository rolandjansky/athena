#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Created by Brian Petersen <Brian.Petersen@cern.ch>
# Reconstruction of the LVL1 infos for the debug event of the Level2 
#
# --2009-10-19:  A. Di Mattia,  first import into the reprocessing scripts.
# --2010-06-07:  A. Di Mattia,  robustness against missing ROD payload incresed


import os
import sys

import eformat
import libpyevent_storage as EventStorage

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


  #work around corrupted events:
  #new_event=eformat.write.FullEventFragment(event)
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
          
          #CTP DAQ ROD
          if idx==0:
              version=rob.rod_minor_version()&0x1f
              if version:
                ctpSkip=(rob.rod_minor_version()>>8)&0x3f
              else:
                ctpSkip=(rob.rod_detev_type()>>16)&0x3f
              try:
                  data=[DaqRobs[idx].rod_data()[0],DaqRobs[idx].rod_data()[1]]
                  for ii in range(30):
                      data+=[DaqRobs[idx].rod_data()[2+ctpSkip*30+ii]]
                  rob.rod_data(data)
              except:
                  pass

          #MUctpi DAQ ROD
          if idx==1:
              data=[]
              # works also when there is no payload
              try:
                  muctpBC=(DaqRobs[idx].rod_data()[0]>>18)&7
                  for word in DaqRobs[idx].rod_data()[1:]:
                      if (word>>14)&7==muctpBC and (word>>26)&1:
                          data+=[(word&0x3fff)|(((word>>17)&0x1ff)<<14)]
              except:
                  pass

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
    output.write(modify(event))
  tmp_file_name = output.last_filename()
  del output
  os.rename(tmp_file_name,output_file)



if __name__ == "__main__":
  if len(sys.argv)!=3:
    print 'usage:',sys.argv[0],'<infile> <outfile>'
    sys.exit(1)
    
  CTPreco(sys.argv[1],sys.argv[2])
