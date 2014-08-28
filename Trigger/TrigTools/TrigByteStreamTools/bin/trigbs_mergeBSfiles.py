#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import eformat
import libpyevent_storage as EventStorage
import sys
import os
import getopt
import subprocess as sp

eventsPerFile=500
verbose=False
renumber=False

def pexec(args):
    return sp.Popen(args, stdout=sp.PIPE,stderr=sp.STDOUT).communicate()[0].strip()


def usage():
  print " usage: %s [options] <outbasename> <dir1> [<dir2> ..]" % sys.argv[0]
  print "    Merges events from multiple raw files from one or more directories"
  print "    This is mainly to used on small samples"
  print
  print "    Options:"
  print "       -n <number of events per file> : defaults to 500"
  print "       -r     : renumber event ids from 1"
  print "       -v     : verbose output"  
  sys.exit(1)
  
try:
  opts, args = getopt.getopt(sys.argv[1:], "vn:")
  if len(args)<2: usage()
except getopt:
  usage()

for opt,arg in opts:
  if opt == '-n':
    eventsPerFile=int(arg)
  if opt == '-v':
    verbose=True
  if opt == '-r':
    renumber=True


basename=args[0]
dirs = args[1:]
files=[]

for indir in dirs:
  cmd='ls'
  if indir.startswith('/castor'):
    cmd='nsls'
  fileNames=pexec([cmd,indir]).split('\n')
  for name in fileNames:
    files+=[indir+'/'+name]

print 'Merging',len(files),'files'

ifile = 0
id = 1
outnum=1
dr=EventStorage.pickDataReader(files[0])
numFiles=0
output = eformat.ostream(core_name=basename,
                         run_number=dr.runNumber(),
                         trigger_type=dr.triggerType(),
                         detector_mask=dr.detectorMask(),
                         beam_type=dr.beamType(),
                         beam_energy=dr.beamEnergy())
output.max_events_per_file(eventsPerFile)

for input_file in files:
  ifile += 1
  print "Opening file %d: %s" % (ifile, input_file)

  input = eformat.istream([input_file])

  for event in input:
    if verbose:
      print "Reading event with Lvl1 Id = %ld and rewritting with %ld" % (event.lvl1_id(),id)
    new_event = eformat.write.FullEventFragment(event)
    if renumber:
        new_event.lvl1_id(id)
        new_event.global_id(id)
        for rob in new_event.children():
            rob.rod_lvl1_id(id)
            rob.checksum_type(0)  #remove checksum
            rob.rod_lvl1_trigger_type(event.lvl1_trigger_type())

    output.write(new_event)

    id+=1

print 'Wrote %d events to %d files' % ( id-1, 1+((id-1)//eventsPerFile) )
