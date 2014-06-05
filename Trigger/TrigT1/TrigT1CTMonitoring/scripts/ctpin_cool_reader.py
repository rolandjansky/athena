#!/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Level 1 Central Trigger crew,
# Johan.Lundberg@cern.ch, CERN, 2009

import sys,getopt,os

def usage():
    print "%s" % sys.argv[0]
    print "  --help                                   this printout"
    print " "
    print " From COOL database, read rates for specified combination of"
    print " lumi blocks and CTPIN counter IDs."
    print " "
    print " Mandatory: "
    print "  --run <runNo>              run number (single number), eg 131740"    
    print " Note: If no run number is given, we try to read current runNr from"
    print " IS at point 1. Requires the current tdaq software setup."
    print " "
    print " Optional: "
    print "  --lb <lb ranges>           lumi block(s)"
    print "       (default='-', all lumiblocks) "
    print "       Read below on how to specify ranges"
    print " "
    print "   --counters <counter ranges> CTPIN counter nr(s)"
    print "             Can use comma separated Regular expressions for names"
    print "       (default='MBTS') "
    print ""
    print "  --maxbins <nbins> Average records to limit the number of histogram bins."
    print "                    Default is 400. Set to 0 for unlimited"
    print "  --maxzscale <number>       max z (color) scale range (default 200)"

    print ""
    print "  --nofillHist                 Fill histograms"
    print "  --nodrawHist                 Fill and Draw histograms"
    print "  --outbase | -o <filename>  Output base name "
    print "       (default='CTPIN_', note: _<runNo>.<extensions> are added)"
    print " "
    print "  --nodraw <opt1,opt2,...>"
    print "    where opt: Log           don't draw Log plots"
    print "               Lin           don't draw Lin plots"
    print "               LB1D          don't draw 1D LB plots"
    print ""
    print "  --norunname                don't add run number to filenames"
    print ""
    print "  --dbid                     cool dbid for the rates information"
    print "  --dbidLumi                 cool dbid for the Lumi information"
    print " "
    print " Text output controls:"
    print "  --verbose                  Do print text results"
    print "  --textsum                  verbose, without per-counter information"
    print " "
    print " "
    print " Range specifications:"
    print "   <lumiblockNo> and <counterNo> can be specified as single "
    print "   numbers, or ranges like this: "
    print "      syntax:  1,3,10-13   gives:   1,3,10,11,12,13 "
    print "      syntax:  3,10-       gives:   3,10... "
    print "      syntax:  50,-10      gives:   ...10,50      "
    print "      syntax:  -           gives:   all  "
    print "   for lumiblocks only: "  
    print "      syntax:  L5          gives:   5 latest lumiblocks "
    print " "

print "CTPIN cool reader"
try:
    longopts=['norunname','run=','lb=','counters=','maxzscale=',
              'help','verbose','textbc','textsum','maxbins=','dbid=',
              'outbase=','nofillHist','nodraw=','nodrawHist']
    opts,args=getopt.getopt(sys.argv[1:],'h',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2)

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)

import l1ctpy

# defaults:
OPT={}
OPT["--run"]=int(-1)
OPT["--counters"]=['MBTS']
OPT["--bcids"]=None
OPT["--lb"]=l1ctpy.text_to_ranges('-')
OPT["--maxbins"]=400
OPT["--verbose"]=False
OPT["--nofillHist"]=False
OPT["--nodrawHist"]=False
OPT["--textsum"]=False
OPT["--filename"]="CTPIN_"

OPT["--dbid"]=l1ctpy.get_dbid_MONP()
OPT["--dbidLumi"]=l1ctpy.get_dbid_COMP()

OPT["--nodraw"]=""

OPT['--norunname'] = False
OPT['--maxzscale'] = ''
#1000


for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o == '--lb':                 
        OPT["--lb"]=l1ctpy.text_to_ranges(a)
    if o == '--outbase':                 
        OPT["--filename"]=str(a)
    if o == '--counters':
        OPT["--counters"]=l1ctpy.text_to_ranges(a)
        OPT["--counters"]=l1ctpy.expandlist(OPT["--counters"],0,678-1)
    if o == '--maxbins':
        OPT["--maxbins"]=int(a)
    if o in ['--dbid','--dbidLumi','--run']:
        OPT[o]=a
    if o == '--textsum':
        OPT["--textsum"] = True
        OPT["--verbose"] = True
    if o in ['--verbose',"--nofillHist","--nodrawHist",'--keepnames','--overflows','--norunname']:
        OPT[o]= True
    if o == '--nodraw':
        OPT["--nodraw"]=OPT["--nodraw"]+str(a)
    if o == "--maxzscale":
        OPT["--maxzscale"]=a    


try:
    OPT["--run"]=int(OPT["--run"])

except BaseException,e:
    usage()
    print e
    raise ValueError("You must specify a single run number, not:",OPT["--run"])

if OPT["--run"] < 1:
    print "No run specified, finding last run nr from cool"
    OPT["--run"]=l1ctpy.get_current_run()

if OPT["--run"] < 1:
    print "no run nr. exit"
    sys.exit(1)
    
if not OPT['--norunname']:
    OPT["--filename"]+=str(OPT["--run"])    

    
#+-------------------------------------------
# Open databases and get the required folders
#+-------------------------------------------

print "Run:                         ",OPT["--run"]
print "Opening db ",OPT["--dbid"]

ctpinRatefolder=l1ctpy.getCoolFolder(OPT["--dbid"] ,'/TRIGGER/LVL1/CTPINRATES')
lbfolder=l1ctpy.getCoolFolder(OPT['--dbidLumi'] ,'/TRIGGER/LUMI/LBLB')

LBs,TimeRanges=l1ctpy.getLbTimesMulti(lbfolder,OPT["--run"],OPT["--lb"])

l1ctpy.showlbandtime(LBs,OPT)

#+-------------------------------------------
# Counter names
#+-------------------------------------------

import numpy

ROV1=(OPT["--run"]<<32)+1; 
pattern='.*'
CounterType='CTPIN'
dbstring=l1ctpy.get_dbid_MONP()
counterdb=l1ctpy.getcounterConfig(dbstring,ROV1,ROV1,pattern,CounterType)
l1ctpy.filter_and_add_counternames(counterdb,OPT)    

#+------------------------------------
# Define and apply a blob calculator function
#+------------------------------------

import ctpin_py

OPT['--RateTitle']='Run '+str(OPT["--run"])+ " : CTPIN Counter Rates, Hz"

blobcalc=ctpin_py.ReadInBlob_detailed_Factory(OPT["--counters"],OPT["--verbose"])
inTimeRanges,rates=ctpin_py.getINrates(ctpinRatefolder,TimeRanges,blobcalc)

#for r in rates:
#    print r

print "Read ",len(rates),"ctpin Rate blobs."

if len(rates)<1:
        print "No data found. Try a different run or a bigger LB range"
        sys.exit(1)

if OPT["--maxbins"]>0:
    print "Averaging to limit the time bins to", OPT["--maxbins"]
    inTimeRanges,rates=l1ctpy.timeAverage(inTimeRanges,rates,OPT["--maxbins"])


#+------------------------------------
# find out which monitoring block is at what lb
#+------------------------------------
inblockLBs=[]
for tt in inTimeRanges:
    inblockLBs.append(l1ctpy.time2lb(LBs,TimeRanges,tt))

#+------------------------------------
# find out which lb is in which monitoring block
#+------------------------------------
lbmatches=[]
for tt in inTimeRanges:
    lbmatches.append(l1ctpy.compareranges(LBs,TimeRanges,tt))

import busy_py

#+------------------------------------
# Print to screen
#+------------------------------------
if OPT["--verbose"]:
    busy_py.showresults(inTimeRanges,rates,inblockLBs,OPT["--counters"],OPT["--bcids"],OPT["--textsum"],OPT["--textbc"])

#+------------------------------------
#+ Write output to python file
#+------------------------------------
busy_py.savefile(OPT,inTimeRanges,rates,lbmatches,inblockLBs)

#+------------------------------------
#+ Fill histograms and make plots if requested
#+------------------------------------
if not OPT["--nofillHist"]:
    busy_py.plot(OPT,inTimeRanges,rates,lbmatches,inblockLBs,opts)
    
