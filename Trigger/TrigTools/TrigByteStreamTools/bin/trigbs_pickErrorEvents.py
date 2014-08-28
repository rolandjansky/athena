#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### ===>
###
### Created --> August 2008 (Anna).
### Last updated --> 11 October 2009 (Anna).
###
### Usage: PickErrorEvents.py <run_number> <stream_type> <stream_name> <chain_with_error> <max_number_of_events_in_file>
###   e.g. PickErrorEvents.py 00120981 debug hlterror mu6_rpc -1
###    or  PickErrorEvents.py 00120981 debug hlterror mu6_rpc  2  
###
### <===


import eformat
import sys
import os
import commands
import time

from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
from PyCool import cool

import libpyevent_storage as EventStorage

from datetime import datetime





def HLTNames(Run_No):

  ## Getting infor from COOL on Chain ID <--> Chain Name
  ## for a specific run (SMK)
  
  dbconn = TriggerCoolUtil.GetConnection("COMP")
  limmin = Run_No<<32
  limmax = (Run_No+1)<<32

  HLTConn = dbconn.getFolder("/TRIGGER/HLT/Menu" )
  HLTChansel = cool.ChannelSelection.all()
  HLTObjs = HLTConn.browseObjects(limmin,limmax,HLTChansel)

  chainNames = {}
  Chain_Info = []
  ChainCount = 0
  while HLTObjs.goToNext():
    HLTObj = HLTObjs.currentRef()
    HLTPayload = HLTObj.payload()
    HLTLevel   = HLTPayload['TriggerLevel']
    HLTName    = HLTPayload['ChainName']
    HLTCounter = HLTPayload['ChainCounter']
    chainNames[(HLTLevel,HLTCounter)] = HLTName
    if (HLTCounter>ChainCount): ChainCount = HLTCounter

  for i in range(ChainCount+1):
    Chain_Info.insert(i,0)


  for c in chainNames:
    name = chainNames[c]
    if (c[0]=='L2'):
      Chain_Info.pop(int(c[1]))
      Chain_Info.insert(int(c[1]),name.strip("L2_"))

  return Chain_Info

          

def event_analysis(input, output, chain_to_write, max, run_number, write_counter):

## Loops over all events and selects those that have an error in the chain_to_write chain

  print input

  names_HLT = HLTNames(int(run_number))

  cntEvt=0
  NoL2Result=0
  NoL2ButEFResult=0

  flags_written = (0,0)
  check_it_L2 = ""
  check_it_EF = ""
  
  for event in input:

    cntEvt+=1
    print "Event Number", cntEvt
    L2Status=None
    L2Result=None
    EFStatus=None
    EFResult=None
    HLTResult=None

    for rob in event:
      if rob.source_id().subdetector_id()==eformat.helper.SubDetector.TDAQ_LVL2:
        L2Status=rob.rod_status()
        #print '******** L2 Status: ', L2Status
        data = rob.rod_data()
        if len(data)>=14:
          L2Result=data[0:14]
        if L2Result:
          HLTResult=L2Result[4]
      elif rob.source_id().subdetector_id()==eformat.helper.SubDetector.TDAQ_EVENT_FILTER:
        EFStatus=rob.rod_status()
        #print '******** Ef Status: ', EFStatus
        data = rob.rod_data()
        if len(data)>=14:
          EFResult=data[0:14]
        if EFResult:
          HLTResult=EFResult[4]
  
    if L2Status:
      if L2Result:
        L2_chain_name_found = names_HLT[int(L2Result[8])]
 #       print "SMK:", L2Result[12], "Chain ID:", L2Result[8], L2_chain_name_found
        if L2_chain_name_found:
          print 'Chain found with L2 Error: ',L2_chain_name_found
          check_it_L2=L2_chain_name_found

    if EFStatus:
      if EFResult:
        EF_chain_name_found = names_HLT[int(EFResult[8])]
#        print "SMK:", EFResult[12], "Chain ID:", EFResult[8], EF_chain_name_found
        if EF_chain_name_found:
          print  'Chain found with EF Error: ',EF_chain_name_found
          check_it_EF=EF_chain_name_found



    if (check_it_L2==chain_to_write or check_it_EF==chain_to_write):
      output.write(event)
      write_counter += 1
      if (flags_written[1]!=1):
        flags_written = (0,1)
      print "Writting event number: ",write_counter

#    print 'Flag:',flags_written[0],', Number of events out:',write_counter
    if (int(max)!=-1 and write_counter>=int(max)):
      print "*** Wrote desired number of events: ",write_counter
      flags_written = (1,1)
      break
    
  print '',cntEvt,'events scanned'
  
  print

  #### *****
  
  return (flags_written,write_counter)


  
def main(filelist, chain_to_write, max, run_number):

####### Input-Output Info follows

  input_file = filelist

  tmpdir =commands.getoutput("echo $TMPDIR")

  if (os.path.exists(tmpdir)):
    print tmpdir,"already exists"
  else:
    print "Generating",tmpdir
    os.system("mkdir $TMPDIR")


  currentTime = datetime.now().strftime("%Y-%m-%d_%H%M%S")
  os.system("mkdir $TMPDIR/"+currentTime)
  
  output_dir = tmpdir+"/"+currentTime
  print
  print '****** Output dir is:',output_dir,'******'
  print
  
  flag_written=(0,0)
  write_counter = 0
  
  print "Opening file: %s" % (input_file)
  print "Will write to file chain: ", chain_to_write  

  file = open(input_file,'r')

  line_counter = 0
    
  for line in file:  

#    print line.strip(), flag_written

    if (flag_written[0]==1): break
      
    command_cp_from_CAF = 'rfcp '+line.strip()+" "+tmpdir+'/Data.data'
    
    print command_cp_from_CAF
    
    os.system(command_cp_from_CAF)
    
    try:
      
      file_to_read = tmpdir+'/Data.data'
    
      print "Opening file of input file: %s" % line.strip()
      
      line_counter+=1
      
      input = eformat.istream(file_to_read.strip())

      ## Updated from Brian's script - this info needs to be attached in all output files.
      dr=EventStorage.pickDataReader(file_to_read.strip())
      output = eformat.ostream(core_name="subset",
                               directory=output_dir,
                               run_number=dr.runNumber(),
                               trigger_type=dr.triggerType(),
                               detector_mask=dr.detectorMask(),
                               beam_type=dr.beamType(),
                               beam_energy=dr.beamEnergy())
      
      
      (flag_written,write_counter) =  event_analysis(input, output, chain_to_write, max, run_number, write_counter)
      
      
      print '... Processed File #',line_counter
      print '... Events written out so far',write_counter
      print
      
      command_delete = 'rm -rf '+tmpdir+'/Data.data' 
      
      print command_delete
      
      os.system(command_delete)    
      
      print
      print
      
    
      if (flag_written[1]==1):
        if (int(max)==-1):
          print "*** Wrote all available events", write_counter
        tmp_file_name = output.last_filename()
        del output
        output_name = output_dir+"/"+run_number+"_"+chain_to_write+"_"+max+"_"+stream_name+"_"+str(line_counter)
        print "Writting output file: ", output_name,"with",write_counter,"events"
        os.rename(tmp_file_name,output_name)
        
      print
      print
        
      os.system("rm -rf "+filelist)
      
    except NameError, IOError:
      print "OOPS! Input Data File Not Found - or a Bug..!"






def get_file_paths_in_correct_format(run_number,stream_type,stream_name):

    string_to_use = []
    tmp_str = ''
    
    dir_to_check = os.path.join('/castor/cern.ch/grid/atlas/DAQ/2009/',run_number,stream_type+'_'+stream_name)
    dir_list = commands.getoutput('nsls '+dir_to_check)

    for each_File in dir_list.split('\n'):
        file_in_castor = os.path.join(dir_to_check,each_File)
        tmp_str = file_in_castor+'\n'
        string_to_use.append(tmp_str)
        
    fileName = run_number+'_'+stream_name
    file = open(fileName,'w')
    file.writelines(string_to_use)

    return fileName





if len(sys.argv)<6:
  print "*******************"
  print "Usage: %s <run_number> <stream_type> <stream_name> <chain_with_error> <max_number_of_events_in_file>" %sys.argv[0]
  print "E.g. ./PickErrorEvents.py 00120981 debug hlterror mu6_rpc 2"
  print "  or ./PickErrorEvents.py 00120981 debug hlterror mu6_rpc -1 (for all events)"
  print "*******************"
  sys.exit(-1)

run_number = sys.argv[1]
stream_type = sys.argv[2]
stream_name = sys.argv[3]
chain_to_write=sys.argv[4]
max_to_write = sys.argv[5]

filelist = get_file_paths_in_correct_format(run_number,stream_type,stream_name)
main(filelist, chain_to_write, max_to_write, run_number)
