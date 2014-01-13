#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# This macro scans CASTOR space for output ntuples of online cost data:
# and submits batch jobs to process these ntuples, one job per run.
#
# This is a typical path to online result:
#
# nsls /castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data10_7TeV 
#

#-------------------------------------------------------------------------------------------
# Get directory content on CASTOR
#
def listDir(dpath):

    output = Popen(['nsls',dpath], stdout=PIPE).communicate()
    
    if len(output) == 0 or type(output[0]) != str:
        print "Found no data at: "+dpath
        return ['']

    listDir = output[0].split('\n')
    listOut = []

    for x in listDir:
        if len(x.rstrip('\n')):
            listOut.append(x.rstrip('\n'))
            
    return listOut
    

#-------------------------------------------------------------------------------------------
# Exeecute main macro
#
import os, string, sys
from optparse import OptionParser
from subprocess import Popen, PIPE

p = OptionParser(usage="usage: <castor path>", version="1.0")

p.add_option('-o',        type="string", default=".",          dest="html_path", help="path to output html directory")
p.add_option('-n',        type="int",    default=0,            dest="nevent",    help="number of events")
p.add_option('-j',        type="int",    default=0,            dest="njob",      help="number of jobs" )
p.add_option('-r',        type="int",    default=0,            dest="run",       help="submit jobs for this run" )
p.add_option('-q',        type="string", default="atlastrig",  dest="bqueue",    help="batch queue")
p.add_option('-s',        type="string", default="Cost",       dest="stream",    help="stream keyword")
p.add_option('--tag',     type="string", default="",           dest="tag",       help="match project tag")

p.add_option('--submit', action="store_true",  default=False, dest="submit", help = "submit jobs" )
p.add_option('--noroot', action="store_false", default=True,  dest="noroot", help = "do no write ROOT file" )
p.add_option('-p',       action="store_true",  default=False, dest="printo", help = "print job standard output" )

(opts, args) = p.parse_args()

inputPath = ""
if len(args) > 0:
    inputPath = args[0]
else:
    inputPath = "/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger"
    print "Using default castor path: "+inputPath

testArea = os.environ.get("TestArea")
execPath = testArea+'/Trigger/TrigCost/TrigCostRate/share/./runRates.py'

tags = listDir(inputPath)
njob = 0
nrun = 0
nfile = 0

for itag in tags:
    if not itag.count('data10'):
        continue
    if len(opts.tag) > 0 and opts.tag != itag:
        continue
    if opts.njob > 0 and njob >= opts.njob:
        break
    
    print itag

    currPath = inputPath.rstrip('/')+'/'+itag
    streams = listDir(currPath)

    for istream in streams:
        if istream.count(opts.stream) < 1:
            continue
        if opts.njob > 0 and njob >= opts.njob:
            break
        
        print "   "+istream

        runs = listDir(currPath+'/'+istream)

        for irun in runs:
            if opts.njob > 0 and njob >= opts.njob:
                break
            if opts.run > 0 and int(irun) != opts.run:
                continue

            nrun += 1
            projects = listDir(currPath+'/'+istream+'/'+irun)

            for iproject in projects:
                if iproject.count(itag) == 0 or iproject.count(irun) == 0:
                    print 'Ignore tag, run, project:',itag,irun,iproject
                    continue
                if iproject.count("COSTMON") == 0:
                    continue
                if opts.njob > 0 and njob >= opts.njob:
                    break
                
                print "      project="+iproject
                
                ppath = currPath+'/'+istream+'/'+irun+'/'+iproject
                hpath = opts.html_path.rstrip('/')+'/'+irun

                if istream.count('L2'):
                    hpath += "-L2"
                if istream.count('EF'):
                    hpath += "-EF"
                    
                files = listDir(ppath)
                nfile += len(files)

                subList = ['bsub', '-q', opts.bqueue,
                           testArea+'/Trigger/TrigCost/TrigCostRate/scripts/run_cost.sh',
                           testArea+'/setup_pro.sh', ppath, hpath, str(opts.nevent), str(int(opts.noroot))]

                if not opts.submit:
                    subList = ["echo"] + subList

                output = Popen(subList, stdout=PIPE).communicate()
                njob += 1
                
                if opts.printo and len(output) > 0 and type(output[0]) == str:
                    print output[0].rstrip('\n')
                

print 'njob  = '+str(njob)
print 'nrun  = '+str(nrun)
print 'nfile = '+str(nfile)
