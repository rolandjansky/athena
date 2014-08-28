#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### ===>
###
###
###
### Usage: PickEventsCastor.py <run_number> <stream_type> <stream_name> <list of events from run>
###   e.g. PickEventsCastor.py 00120981 debug hlterror [408837,338802,312705]
###
### <===

import sys
import optparse

preamble=\
"""
  *******************
  This is the script to scan whole castor directory to pick selected events. 
  E.g. ./PickEventsCastor.py -r 120981 -t debug -s hlterror -d dat09_900GeV -e \"[408837,338802,312705]\" -l \"[272,278,292]\"
  For info about datatag & streams see http://atlas-runquery.cern.ch
  *******************
"""

optparser = optparse.OptionParser(usage=preamble)
optparser.add_option("-r", "--run"       , help="run number (obligatory)", metavar="RUN")
#optparser.add_option("-d", "--datatag"   ,  help="The data taking conditions tag, for possible values see: nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/ (obligatory)")
optparser.add_option("-s", "--strname"   , help="name of the stream i.e. egamma, ... (obligatory)")
optparser.add_option("-t", "--strtype"   , help="type of the stream i.e. physics, debug, calibration", default="physics")
optparser.add_option("-e", "--events"    , help="list of events global ids, \"[12,56,89]\" (do not leave spaces)")
optparser.add_option("-l", "--lumiblocks", help="list of lumiblocks or lumiblock range, \"[270,278,285]\" or \"[270,+,280]\", if you want to specify a lumiblock range (do not leave spaces)")
optparser.add_option("", "--bctimes"   , help="list of events to extract by their bunch corossing time, \"['123123:546456','2342434:546464','345345:546546']\"")
optparser.add_option("", "--extl1"     , help="list of events to extract by their extended lvl1id (this is ambiguous), \"[1,4,5,]\"")
optparser.add_option("-d", "--datasource", help="data source i.e. online, reprocessing (2010) or tier0", default="online")
optparser.add_option("-g", "--datatag"   , help="The data taking conditions tag, for possible values see: nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/ (obligatory if datasource==\"tier0\" and tag different than default)", default="data10_7TeV")
optparser.add_option("-y", "--year"      , help="year (2009 or 2010)", default="2010")

(opt, args) = optparser.parse_args()
if not opt.run or not opt.strname or not opt.strtype:
  print "basic options not given"
  sys.exit(0)
if not opt.events and not opt.bctimes and not opt.extl1:
  print "events not given"
  sys.exit(0)



EventsList = []
if opt.events:
    EventsList = (opt.events.strip("[]")).split(",")
if opt.bctimes:
    EventsList = (opt.bctimes.strip("[]")).split(",")
if opt.extl1:
    EventsList = (opt.extl1.strip("[]")).split(",")


max_to_write = len(EventsList)
if max_to_write == 0:
  print "empty events list given"
  sys.exit(0)
print "events list: ", EventsList


import eformat
import os
import commands
import time
from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
from PyCool import cool



import libpyevent_storage as EventStorage

from datetime import datetime






#print GetInputEventNumber

#print max_to_write

         

def event_analysis(input, output,  write_counter, EventsList):

## Loops over all events and selects those that have an error in the chain_to_write chain

  #print input
  max = len(EventsList)
  cntEvt=0
  NoL2Result=0
  NoL2ButEFResult=0

  flags_written = (0,0)
  check_it_L2 = ""
  check_it_EF = ""
 
  for event in input:

    cntEvt+=1


    event_number = str(event.global_id())
    if opt.bctimes:
      event_number = str(event.bc_time_seconds()) + ':' + str(event.bc_time_nanoseconds())
    if opt.extl1:
      event_number = str(event.lvl1_id())
    print "..... events read: %7d -- events written: %3d  -- global_id: %9d -- lumi_block: %3d -- lvl1_id: %9d -- selector: %s" %\
          (cntEvt, write_counter, event.global_id(), event.lumi_block(), event.lvl1_id() , event_number )
   
    print event_number, EventsList
    if event_number in EventsList:
      print ".. Writting event number: ",write_counter     
      output.write(event)
      write_counter += 1
      if (flags_written[1]!=1):
        flags_written = (0,1)

#    print 'Flag:',flags_written[0],', Number of events out:',write_counter
    if (int(max)!=-1 and write_counter>=int(max)):
      print ".. Wrote desired number of events: ",write_counter
      flags_written = (1,1)
      break
   
  print '..', cntEvt, 'events scanned'
 
  print

  #### *****

  return (flags_written, write_counter)


 
def main(filelist, EventsList):

####### Input-Output Info follows

  input_file = filelist
  max = len(EventsList)
  tmpdir =commands.getoutput("echo $TMPDIR")

  if (os.path.exists(tmpdir)):
    print '..', tmpdir,"already exists"
  else:
    print ".. Generating",tmpdir
    os.system("mkdir $TMPDIR")


  currentTime = datetime.now().strftime("%Y-%m-%d_%H%M%S")
  os.system("mkdir $TMPDIR/"+currentTime)
 
  output_dir = tmpdir+"/"+currentTime
  print
  print '****** Output dir is:',output_dir,'******'
  print
 
  flag_written=(0,0)
  write_counter = 0
 
  print ".. Opening file: %s" % (input_file)

  file = open(input_file,'r')

  line_counter = 0
   
  for line in file: 
 
#    print line.strip(), flag_written

    if (flag_written[0]==1): break

    command_cp_from_CAF = 'xrdcp root://castoratlas/'+line.strip()+" "+tmpdir+'/Data.data'
   
    print '.... making local copy: ', command_cp_from_CAF
   
    copyOutput = commands.getoutput(command_cp_from_CAF)

    if "Permission denied" in copyOutput:
      print ". Permission denied, continue..."
      continue
    if "No such file or directory" in copyOutput:
      print ". No such file or directory, continue..."
      continue   
    try:
#    no_try = 1
#    if (no_try==1):
     
      file_to_read = tmpdir+'/Data.data'
   
      print ".. Opening local copy of input file: %s" % line.strip()
     
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
     
     
      (flag_written,write_counter) =  event_analysis(input, output,  write_counter, EventsList)
     
     
      print '... Processed File #',line_counter
      print '... Events written out so far',write_counter
      print
     
      command_delete = 'rm -rf '+tmpdir+'/Data.data' 
     
      print '.... cleaning up: ', command_delete
     
      os.system(command_delete)   
     
      print
      print

      
      if (flag_written[1]==1):
        print "*** Wrote", write_counter, "events"
        tmp_file_name = output.last_filename()
        del output
        output_name = output_dir+"/"+opt.run+"_"+str(write_counter)+"_"+opt.strtype+"_"+opt.strname+"_"+str(line_counter)
        print "Writting output file: ", output_name,"with",write_counter,"events"
        os.rename(tmp_file_name,output_name)
       
      else:
        tmp_file_name = output.last_filename()
        del output
        os.remove(tmp_file_name)
 

      print
      print
       
     
    except NameError, IOError:
      print "OOPS! Input Data File Not Found - or a Bug..! (Please report it!)",

  if (line_counter == 0):
    print "*** There were no data files found - please check your input parameters ***"
    print "***************************************************************************"
    print

  os.system("rm -rf "+filelist)





def get_file_paths_in_correct_format(opt):

    string_to_use = []
    tmp_str = ''
    # normalize args
    run_in_dir = '%07d' % int(opt.run)
    run_in_name = '%08d' % int(opt.run)
    stream = opt.strtype+'_'+opt.strname


    if (opt.datasource == "online"):
      if "physics" or "express" in stream:
        dir_to_check = os.path.join('/castor/cern.ch/grid/atlas/DAQ/merge/'+opt.year+'/',run_in_name,stream)
      else:
        dir_to_check = os.path.join('/castor/cern.ch/grid/atlas/DAQ/'+opt.year+'/',run_in_name,stream)

    elif (opt.datasource == "tier0"):
      raw_dir = '.'.join([opt.datatag,run_in_name,stream,'merge','RAW'])
      dir_to_check = os.path.join('/castor/cern.ch/grid/atlas/tzero/prod1/perm/',opt.datatag, stream, run_in_dir, raw_dir)

    elif (opt.datasource == "reprocessing"):
      dir_to_check = os.path.join("/castor/cern.ch/grid/atlas/DAQ/reprocessing/"+opt.year+"/physics/",run_in_name,stream)
      


    print ".. scanning files in dir: ", dir_to_check
    dir_list = commands.getoutput('nsls '+dir_to_check)

    
 
    if opt.lumiblocks:
        print opt.lumiblocks
        LBList = (opt.lumiblocks.strip("[]")).split(",")
        if "physics" or "express" in stream:
            if "+" in LBList:
                for each_File in dir_list.split('\n'):
                    for lb in range(int(LBList[0]),int(LBList[2])):
                       # print lb, each_File
                       if lb<10:
                         lb_str="000"+str(lb)
                       elif lb<100:
                         lb_str="00"+str(lb)
                       elif lb<1000:
                         lb_str="0"+str(lb)
                       elif (lb>=1000):
                         lb_str=str(lb)
                       if "lb"+lb_str in each_File:
                         file_in_castor = os.path.join(dir_to_check,each_File)
                         print file_in_castor
                         tmp_str = file_in_castor+'\n'
                         string_to_use.append(tmp_str)
                            
            else:
                for each_File in dir_list.split('\n'):
                    for lb in LBList:
                      if int(lb)<10:
                        lb_str="000"+str(lb)
                      elif int(lb)<100:
                        lb_str="00"+str(lb)
                      elif int(lb)<1000:
                        lb_str="0"+str(lb)
                      elif (int(lb)>=1000):
                        lb_str=str(lb)
                      if "lb"+lb_str in each_File:          
                        file_in_castor = os.path.join(dir_to_check,each_File)
                        print file_in_castor
                        tmp_str = file_in_castor+'\n'
                        string_to_use.append(tmp_str)

        else: #if not physics in stream_type...
            for each_File in dir_list.split('\n'):
                file_in_castor = os.path.join(dir_to_check,each_File)
                print file_in_castor
                tmp_str = file_in_castor+'\n'
                string_to_use.append(tmp_str)
    else:
        for each_File in dir_list.split('\n'):
            file_in_castor = os.path.join(dir_to_check,each_File)
            print file_in_castor
            tmp_str = file_in_castor+'\n'
            string_to_use.append(tmp_str)
            
       
    fileName = opt.run+'_'+stream
    file = open(fileName,'w')
    file.writelines(string_to_use)
    #print string_to_use
   
    return fileName


filelist = get_file_paths_in_correct_format(opt)
main(filelist, EventsList)
