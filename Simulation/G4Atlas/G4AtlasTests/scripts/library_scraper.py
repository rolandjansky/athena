#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



"""
  --text  - calls
       pprof --text `which python` my.athena.profile  > GProfile.txt
  --callgrind
       pprof --callgrind `which python` my.athena.profile > callgrind.out.GProfile
  
  --sqlite
  --web

TODO:
  separate into different files for the classes
  allow for some options (argparse)

  extend list of timers to be added to the list.

"""
import os
import shutil
import subprocess

if __name__=="__main__":

    import argparse

    usage= "parses callgrind.out and creates web pages, and fills database with performance info"

    parser = argparse.ArgumentParser(description=usage)

    # dummy default
    librarypathbase='./'
    parser.add_argument('-p','--path', action='store', dest='librarypathbase', default=librarypathbase,
                    help='Location of the base path for the libraries')
    # file names
    dbfile='/afs/cern.ch/atlas/groups/Simulation/rtt/rttProfile.sqlite'
    parser.add_argument('--dbfile', action='store', dest='dbfile',
                      default=dbfile,
                      help='location of the sqlite file ('+dbfile+')')
    args = parser.parse_args()
    
    dd=[]
    def runProcess(infile):
        p = subprocess.Popen(['./libscrape.sh', infile], stdout=subprocess.PIPE, 
                             stderr=subprocess.STDOUT)
        while(True):
          retcode = p.poll() #returns None while subprocess is running
          line = p.stdout.readline()
          yield line
          if(retcode is not None):
            break
    def chomp(s):
        return s[:-1] if s.endswith('\n') else s
    
    import glob
    for infile in glob.glob( os.path.join(args.librarypathbase, 'lib*.so') ):
        import re
        result = re.search('lib\/lib(.*).so', infile)
        packagename = result.group(1)
        for line in runProcess(infile):
            line = chomp(line)
            if not line.startswith('GaudiDll'):
                print line
                dd+= [(line,packagename)] 
    print dd

    ##WHAT TO DO

    ## Find all libraries in the director(y/ies) specified in the path argument
    ## Run the following command on each library
    ## nm --defined-only -s $TestArea/InstallArea/x86_64-slc5-gcc43-opt/lib/libPixelDigitization.so | c++filt | sed '/thunk/d;/[0-9a-f] V/d;/[0-9a-f] W/d;/[0-9a-f] b/d;' | awk '{ print $3 }' | sed '/::/!d' | awk 'BEGIN { FS = "(" } ; { print $1 }' | sort | uniq
    ## Store the output in a series of tuples ("method name string", "library name string")
    ## apply this to the database.

