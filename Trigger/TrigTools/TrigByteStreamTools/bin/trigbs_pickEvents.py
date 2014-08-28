#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import eformat
import sys
import os
import libpyevent_storage as EventStorage
import getopt

def usage():
  print " usage: %s [options]  <filein> <fileout> <events1> [event2] ..." % sys.argv[0]
  print "    Writes out selected events from a bytestream file"
  print "           Events are specified as individual event number (counting from 0)"
  print "           or as ranges of event numbers (e.g. 3-10)."
  print "           Multiple ranges can be specified"
  print
  print "    options: "
  print "       -1  - select on l1 id instead of event number"
  print "       -g  - select on global id instead of event number"
  print "       -v  - verbose"
  sys.exit(1)
  
try:
  opts, args = getopt.getopt(sys.argv[1:], "1gv")
  if len(args)<3: raise 'notEnoughPars'
except (getopt.GetoptError, 'notEnoughPars'):
  usage()

useL1ID=False
useGlobalID=False
verbose=False
for opt,arg in opts:
  if opt == '-1':
    useL1ID=True
  if opt == '-g':
    useGlobalID=True
  if opt == '-v':
    verbose=True
    
input_file = args[0]
output_file = args[1]
eventNums=args[2:]

if verbose:
  print "Opening file: %s" % ( input_file)

input = eformat.istream([input_file])
dr=EventStorage.pickDataReader(input_file)
output = eformat.ostream(core_name=output_file,
                         run_number=dr.runNumber(),
                         trigger_type=dr.triggerType(),
                         detector_mask=dr.detectorMask(),
                         beam_type=dr.beamType(),
                         beam_energy=dr.beamEnergy())

ranges=[]
for range in eventNums:
  vals=range.split('-')
  if len(vals)==1:
    vals.append(vals[0])
  if vals[0]=='': vals[0]=0
  if vals[1]=='': vals[1]=1<<32
  ranges.append( (int(vals[0]),int(vals[1])) )

cnt=0
numSaved=0
for event in input:
  if verbose:
    print "Reading event %d with Lvl1 Id = %ld and global Id = %ld"% (cnt,event.lvl1_id(),event.global_id())
  num=cnt
  if useL1ID:
    num=event.lvl1_id()
  if useGlobalID:
    num=event.global_id()
  for range in ranges:
    if num>=range[0] and num<=range[1]:
      print " - Saving event %d with Lvl1 Id = %ld and global Id = %ld"% (cnt,event.lvl1_id(),event.global_id())
      output.write(event)
      numSaved+=1
      break
  cnt+=1

print numSaved,'Events saved to',output.last_filename()
