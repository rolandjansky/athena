#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#-------------------------------------------------------------------------------------------
# Get directory content on CASTOR
#
def listDir(dpath):
    from subprocess import Popen, PIPE
    output = Popen(['nsls',dpath], stdout=PIPE).communicate()
    
    if len(output) == 0 or type(output[0]) != str:
        print "Found no data at: "+dpath
        return ['']

    return output[0].split('\n')

#-------------------------------------------------------------------------------------------
# Exeecute main macro
#
import os, string, sys
from optparse import OptionParser

p = OptionParser(usage="usage: <castor path>", version="1.0")

p.add_option( '-o', type    = "string", default = ".",
              dest = "html_path", help = "path to output html directory" )

p.add_option( '-n', type = "int", default = 0,
              dest = "nevent", help = "number of events" )

(options, args) = p.parse_args()

if len(args) < 1:
    p.error("Error: need castor path argument")

testArea = os.environ.get("TestArea")
execPath = testArea+'/Trigger/TrigCost/TrigCostRate/share/./runRates.py'

tags = listDir(args[0])

nrun = 0
nfile = 0

for itag in tags:
    if itag.count('data10') != 1:
        continue
    
    print itag

    currPath = args[0]+itag
    streams = listDir(currPath)

    for istream in streams:
        if istream.count('Cost') < 1:
            continue
        
        print "   "+istream

        runs = listDir(currPath+'/'+istream)

        for irun in runs:

            nrun += 1
            projects = listDir(currPath+'/'+istream+'/'+irun)

            for iproject in projects:
                if iproject.count(itag) == 0 or iproject.count(irun) == 0:
                    continue
                
                print "      project="+iproject
                ppath = currPath+'/'+istream+'/'+irun+'/'+iproject
                files = listDir(ppath)
                nfile += len(files)

                exeList = ['bsub', '-q', 'atlastrig',
                           execPath, ppath, '-o', options.html_path+'/'+irun,'-n', str(options.nevent)]

                from subprocess import Popen, PIPE
                output = Popen(exeList, stdout=PIPE).communicate()
                print output 
                break
            break
        break
    break
                
print 'nrun  = '+str(nrun)
print 'nfile = '+str(nfile)
