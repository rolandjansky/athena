#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Level 1 Central Trigger crew,
# Johan.Lundberg@cern.ch, CERN, 2009

import sys,getopt,os

def usage():
    print "%s" % sys.argv[0]
    print "  --help                                   this printout"
    print " "
    print " From COOL database, read busy information for specified"
    print " lumi blocks"
    print " "
    print " Mandatory: "
    print "  --run <runNo>              run number (single number), eg 131740"    
    print " Note: If no run number is given, last run from cool is used."
    print " "
    print " Optional: "
    print "  --lb <lb ranges>           lumi block(s)"
    print "       (default='-', all lumiblocks) "
    print "  --counters <names for busy objects>"
    print "                             Supports comma separated Regular expressions for names"
    print "       (default is to show all information ) "
    print ""
    print "  --overflows   allow inclusion of overflow information"
    print "  --keepnames   do not use _lable records as names for rates"
    print "  "
    print "  --maxbins <nbins> Average records to limit the number of histogram bins."
    print "                    Default is 400. Set to 0 for unlimited"
    print "  "
    print "  --nofillHist               do not Fill histograms"
    print "  --nodrawHist               do not Draw histograms"
    print "  --outbase | -o <filename>  Output base name "
    print "       (default='busy_', note: _<runNo>.<extensions> are added)"
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
    print "      syntax:  L5          gives:   5 latest  "
    print " "

print "busy cool reader"
try:
    longopts=['norunname','run=','lb=','counters=',
              'help','verbose','textsum','maxbins=','dbid=',
              'outbase=','nofillHist','nodrawHist','keepnames','overflows']
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
OPT["--counters"]=['core_moni','ctpmi','out']
OPT["--lb"]=l1ctpy.text_to_ranges("-"); 
OPT["--maxbins"]=400
OPT["--bcids"]=None
OPT["--verbose"]=False
OPT["--nofillHist"]=False
OPT["--nodrawHist"]=False
OPT["--textsum"]=False
OPT["--keepnames"]=False
OPT["--overflows"]=False
OPT["--filename"]="busy_"
OPT["--nodraw"]=""

OPT["--dbid"]=l1ctpy.get_dbid_MONP()
OPT["--dbidLumi"]=l1ctpy.get_dbid_COMP()

OPT['--norunname'] = False

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o == '--lb':                 
        OPT["--lb"]=l1ctpy.text_to_ranges(a)
    if o == '--outbase':                 
        OPT["--filename"]=str(a)
    if o == '--counters':
        OPT["--counters"]=a.split(',')
    if o == '--maxbins':
        OPT["--maxbins"]=int(a)
    if o in ['--dbid','--dbidLumi','--run']:
        OPT[o]=a
    if o == '--textsum':
        OPT["--textsum"] = True
        OPT["--verbose"] = True
    if o == '--nofillHist':                 
        OPT["--nofillHist"] = True
        OPT["--nodrawHist"] = True
    if o == '--nodraw':                 
        OPT["--nodraw"] = OPT["--nodraw"] + str(a)
    if o in ['--verbose',"--nodrawHist",'--keepnames','--overflows','--norunname']:
        OPT[o]= True

try:
    OPT["--run"]=int(OPT["--run"])

    if OPT["--run"] < 1:
        print "No run specified, finding last run from cool"
        OPT["--run"]=l1ctpy.get_current_run()
except BaseException,e:
    usage()
    print e
    raise ValueError("You must specify a single run number, not:",OPT["--run"])

if not OPT['--norunname'] :
    OPT["--filename"]+=str(OPT["--run"])
    
#+-------------------------------------------
# Open databases and get the required folders
#+-------------------------------------------

busyRatefolder=l1ctpy.getCoolFolder(OPT["--dbid"] ,'/TRIGGER/LVL1/BUSYRATES')
busyConffolder=l1ctpy.getCoolFolder(OPT["--dbid"] ,'/TRIGGER/LVL1/BUSYCONF')
lbfolder=l1ctpy.getCoolFolder(OPT['--dbidLumi'] ,'/TRIGGER/LUMI/LBLB')


LBs,TimeRanges=l1ctpy.getLbTimesMulti(lbfolder,OPT["--run"],OPT["--lb"])

print OPT
l1ctpy.showlbandtime(LBs,OPT)

#+-------------------------------------------
# Counter names
#+-------------------------------------------

import numpy

dbstring=l1ctpy.get_dbid_MONP()

# define a counter database. These are used directly as field names
# (as defined in schema) , retrieved from cool
counterdb=[]
myspec = busyRatefolder.payloadSpecification()
for x in reversed(myspec):    
    # the name is called like this just to be able to use some
    # methods developed for CTPIN and CTPMON
    # if this is a label, skip it
    if '_label' in x.name():
        continue
    tmp={}
    tmp['CounterNr']=-1
    tmp['ThresholdName']=x.name()
    if not OPT['--overflows']:
        if 'overflow' in x.name():
            continue
    counterdb.append(tmp)

l1ctpy.filter_and_add_counternames(counterdb,OPT)




#+--------------------------------------------
# Define and apply a blob calculator function
#+--------------------------------------------

OPT['--RateTitle']='Run '+str(OPT["--run"])+ " : Busy Counter Rates, %" 

import busy_py

blobcalc=busy_py.ReadBusyRecord_detailed_Factory(OPT["--countersnames"],OPT["--verbose"])
inTimeRanges,rates=busy_py.getBusyrates(busyRatefolder,TimeRanges,blobcalc)

print "Read ",len(rates),"busy Rate records."

if len(rates)<1:
    print "No data found. Try a different run or a bigger LB range"
    sys.exit(1)

if OPT["--maxbins"]>0:
    print "Averaging to limit the time bins to", OPT["--maxbins"]
    inTimeRanges,rates=busy_py.timeAverage(inTimeRanges,rates,OPT["--maxbins"])

#$for b in inTimeRanges:
#$    print " TimeRange:",1e-9*b[0]-t0,1e-9*b[1]-t0," Duration",int(1e-9*(b[1]-b[0])) 


#+------------------------------------
# Modify the lables, some records have their lables in cool: 
#+------------------------------------

print "Selected Busy Counters are:", OPT["--countersnames"]

busyCfg={}
if not OPT["--keepnames"]:
    #+-------------------------------------------
    # Read the busy configuration, names and enabled:on/off
    #+-------------------------------------------
    try:
        from PyCool import cool 
        lbROV0=cool.ValidityKey((OPT["--run"]<<32)+2)   # lb 2 ->
    #    lbROV1=cool.ValidityKey((OPT["--run"]+1)<<32-1) # lb 3
        Cobj=busyConffolder.browseObjects(lbROV0,lbROV0,cool.ChannelSelection.all())
        for tCobj in Cobj:
            b={}
            x=tCobj.payload()
            b['identifier']=x["identifier"]
            b['busyEnabled']=x["busyEnabled"]
            b['description']=x["description"]
            busyCfg[x["identifier"]]=b

        # take info from anywhere in the run 
        t0=TimeRanges[0][0] #first time
        t1=TimeRanges[-1][1] #last time

        obj=busyRatefolder.browseObjects(t0,t1,cool.ChannelSelection.all())
        # get the first record:
        obj.goToNext()

        o=obj.currentRef()           # TODO: catch problems here
        pl=o.payload()

        # there's a record called the same as our current record, but with
        # '_rate' replaced with '_label', we add that to the
        # current name
        for i,n in enumerate(OPT["--countersnames"]) :
            lablecandidate=n
            if lablecandidate in busyCfg:
                newname=(busyCfg[lablecandidate]["description"]).strip()
                if not busyCfg[lablecandidate]["busyEnabled"]:
                    newname="(out:"+newname+")"
                print "replacing name",OPT["--countersnames"][i],' with ',newname
                OPT["--countersnames"][i]=newname
                continue
            lablecandidate=n.replace('_rate','_label')
            if n == lablecandidate :
                continue
            if lablecandidate in pl:
                newname=(pl[lablecandidate]).strip()
                print "replacing name",OPT["--countersnames"][i],' with ',newname
                OPT["--countersnames"][i]=newname

    except BaseException,e:
        print "No busy conf record found, going on without it (",e,")"

if busyCfg:
    print "Read ",len(busyCfg)," config objects"

#+------------------------------------
# find out which lb is in which monitoring block
#+------------------------------------
print "finding lb matches"
lbmatches=[]
for tt in inTimeRanges:
    lbmatches.append(busy_py.compareranges(LBs,TimeRanges,tt))

inblockLBs=[]
for tt in inTimeRanges:
#    print " data time range",tt
#    print " in terms of lb: ",busy_py.time2lb(LBs,TimeRanges,tt)
    inblockLBs.append(busy_py.time2lb(LBs,TimeRanges,tt))

#+------------------------------------
# Print to screen
#+------------------------------------
if OPT["--verbose"]:
    #+------------------------------------
    # find out which monitoring block is at what lb
    #+------------------------------------
    busy_py.showresults(inTimeRanges,rates,inblockLBs,OPT["--counters"],None,OPT["--textsum"])

#+------------------------------------
#+ Write output to python file
#+------------------------------------
print "making output python file"
busy_py.savefile(OPT,inTimeRanges,rates,lbmatches,inblockLBs)

#+------------------------------------
#+ Fill histograms and make plots if requested
#+------------------------------------
print "making plots"
if not OPT["--nofillHist"]:
    busy_py.plot(OPT,inTimeRanges,rates,lbmatches,inblockLBs,opts)
    
