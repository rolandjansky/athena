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
    print " lumi blocks and CTPMON counter IDs."
    print " "
    print "  --run <runNo>              run number (single number), eg 134967"    
    print " "
    print " Note: If no run number is given. Find last run in cool (not always ATLAS partition)"
    print " "
    print "  --lb <lb ranges>           lumi block(s)"
    print "       (default='L5',  - last 5 lumiblock only) "
    print "  --counters <counter ranges> CTPMON counter nr(s)"
    print "       (default='MBTS') "
    print "    Can use comma separated Regular expressions for names"
    print " "
    print "  --bcid <bcid ranges>       bc id ranges"
    print "       (default='1') "
    print " "
    print "  --nofillHist               don't fill histograms"
    print "  --nodrawHist               don't fill and Draw histograms"
    print "  --rootbatch                run root in batch mode (no windows)"
    print " "
    print "  --norunname                don't add run number to filenames"
    print ""
    print "  --bcidpattern              print beam bcidpattern:"
    print "                             for beam1, for beam2, and colliding bunches "
    print "         note: recommended to use '--bcid -' to consider all possible bunch numbers"
    print " "
    print "  --nodraw <opt1,opt2,...>"
    print "    where opt: Log           don't draw Log plots"
    print "               Lin           don't draw Lin plots"
    print "               LB1D          don't draw 1D LB plots"
    print "               allBCID       don't draw one BCID plot per counter"
    print ""
    print "  --maxzscale <number>       max z (color) scale range (default:auto)"
    print ""
    print "  --outbase | -o <filename>  Output base name "
    print "       (default='CTPMON_', note: _<runNo>.<extensions> are added)"
    print " "
    print " Text output controls:"
    print "  --verbose                  Do print text results"
    print "  --textbc                   Do print per-bunch information"
    print "  --textsum                  verbose, without per-counter information"
    print " "
    print " Range specifications:"
    print "   <lumiblockNo> and <counterNo> and <bcID> can be specified as single "
    print "   numbers, or ranges like this: "
    print "      syntax:  1,3,10-13   gives:   1,3,10,11,12,13 "
    print "      syntax:  3,10-       gives:   3,10... "
    print "      syntax:  50,-10      gives:   ...10,50      "
    print "      syntax:  -           gives:   all  "
    print "   in addition, for lumiblocks only: "  
    print "      syntax:  L5          gives:   5 latest  "
    print " "
try:
    longopts=['norunname','run=','lb=','counters=','bcid=',
              'help','verbose','textbc','textsum','maxzscale=',
              'outbase=','nodraw=','nofillHist','nodrawHist','nobatch','bcidpattern']
    opts,args=getopt.getopt(sys.argv[1:],'h',longopts)
except getopt.GetoptError,e:
    print "Option parsing error:"
    usage()
    sys.exit(2)

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)

sys.argv[1:]=[] #prevent root to interpret command line parameters
import l1ctpy
# defaults:
OPT={}
OPT["--run"]=int(-1)
OPT["--counters"]=["MBTS"]
OPT["--bcids"]=[1]
OPT["--lb"]=l1ctpy.text_to_ranges('L5')
OPT["--verbose"]=False
OPT["--textbc"]=False
OPT["--nofillHist"]=False
OPT["--nodrawHist"]=False
OPT["--nodraw"]=""
OPT["--textsum"]=False
OPT["--filename"]="CTPMON_"
OPT['--norunname'] = False
OPT['--nobatch'] = False
OPT['--bcidpattern'] = False
OPT['--maxzscale'] = ''

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o == '--run':
        OPT["--run"] = a
    if o == '--lb':                 
        OPT["--lb"]=l1ctpy.text_to_ranges(a)
    if o == '--outbase':                 
        OPT["--filename"]=str(a)
    if o == '--counters':
        OPT["--counters"]=l1ctpy.text_to_ranges(a)
        OPT["--counters"]=l1ctpy.expandlist(OPT["--counters"],0,160-1)
    if o == '--bcid':
        OPT["--bcids"]=l1ctpy.text_to_ranges(a)
        OPT["--bcids"]=l1ctpy.expandlist(OPT["--bcids"],0,3564-1)
    if o == '--textsum':
        OPT["--textsum"] = True
        OPT["--verbose"] = True
    if o == '--textbc':
        OPT["--textbc"] = True
        OPT["--verbose"] = True
    if o in ['--bcidpattern','--verbose','--nofillHist',"--nodrawHist",'--norunname','--nobatch']:
        OPT[o] = True
    if o == "--nodraw":
        OPT["--nodraw"]=OPT["--nodraw"]+str(a)
    if o == "--maxzscale":
        OPT["--maxzscale"]=a    

try:
    OPT["--run"]=int(OPT["--run"])

    if OPT["--run"] < 1:
        print "No run specified. Finding last run in cool"
        OPT["--run"]=l1ctpy.get_current_run()
except BaseException,e:
    usage()
    print e
    raise ValueError("You must specify a single run number, not:",OPT["--run"])


if not OPT['--norunname'] :
    OPT["--filename"]+=str(OPT["--run"])
    
#

OPT['--RateTitle']='Run '+str(OPT["--run"])+ " : CTPMON Counter Rates, Hz"
# print "WARN, rate in terms of rates"
# OPT['--RateTitle']='Run '+str(OPT["--run"])+ " : CTPMON Counts"

#print "Options:",OPT
    
import ctpmon_py

#+-------------------------------------------
# Open databases and get the required folders
#+-------------------------------------------

ctpmonfolder=ctpmon_py.getCoolFolder(ctpmon_py.get_dbid_MONP(),'/TRIGGER/LVL1/CTPMONDATA')
lbfolder=ctpmon_py.getCoolFolder(ctpmon_py.get_dbid_COMP(),'/TRIGGER/LUMI/LBLB')

LBs,TimeRanges=ctpmon_py.getLbTimesMulti(lbfolder,OPT["--run"],OPT["--lb"])

l1ctpy.showlbandtime(LBs,OPT)

#+-------------------------------------------
# Counter names
#+-------------------------------------------

import numpy

ROV1=(OPT["--run"]<<32)+1; 
pattern='.*'
CounterType='CTPMON'
dbstring=l1ctpy.get_dbid_MONP()
counterdb=l1ctpy.getcounterConfig(dbstring,ROV1,ROV1,pattern,CounterType)
l1ctpy.backport_counternames(counterdb)
l1ctpy.filter_and_add_counternames(counterdb,OPT)

#+------------------------------------
# Define and apply a blob calculator function
#+------------------------------------

#print "-----------------------------"
#print OPT["--counters"]
#print "-----------------------------"

blobcalc=ctpmon_py.ReadMonBlob_detailed_Factory(OPT["--counters"],OPT["--bcids"],OPT["--verbose"])
monTimeRanges,rates=ctpmon_py.getMONrates(ctpmonfolder,TimeRanges,blobcalc)

print "Read ",len(rates),"monitoring blobs."
if len(rates)<1:
    print "No data found. Try a different run or a bigger LB range"
    sys.exit(1)

#newnames=[]
#
#f#o x in  OPT["--countersnames"]:
#    x=x.replace('MBTS_','Cell ')
#    x=x.replace('BPTX0','Beam 1')
#    x=x.replace('BPTX1','Beam 2')
#    newnames.append(x)#
#
#OPT["--countersnames"]=newnames
#
#f#or x in OPT["--countersnames"]:
# #   print x
#sys.exit()
#
    
#+------------------------------------
# find out which monitoring block is at what lb
#+------------------------------------
monblockLBs=[]
for tt in monTimeRanges:
    monblockLBs.append(ctpmon_py.time2lb(LBs,TimeRanges,tt))

#+------------------------------------
# find out which lb is in which monitoring block
#+------------------------------------
lbmatches=[]
for tt in monTimeRanges:
    lbmatches.append(ctpmon_py.compareranges(LBs,TimeRanges,tt))

#+------------------------------------
# Print to screen
#+------------------------------------
if OPT["--verbose"]:
    ctpmon_py.showresults(monTimeRanges,rates,monblockLBs,OPT["--counters"],OPT["--bcids"],OPT["--textsum"],OPT["--textbc"])


#OPT['--general_minimum']=1e-3
#+------------------------------------
#+ Write output to python file
#+------------------------------------
ctpmon_py.savefile(OPT,monTimeRanges,rates,lbmatches,monblockLBs)

def boolify(vector,threshold=10000):
    return map(lambda _ : 1 if _>threshold else 0,vector)

def boolcoinsidence(vector1,vector2):
    return map(lambda a,b : 1 if (a and b) else 0,vector1,vector2)

def getfilled(bcidc,vector1):
    results=[]    
    for i,v in enumerate(vector1):
        if v:
            results.append(bcidc[i])
    return results

#Print bcid pattern
def showBCIDS(monTimeRanges,rates,monblockLBs,counters,bcids):
    bptx0_filled=[]
    bptx1_filled=[]
    bptxB_filled=[]
    starttime=0
    startlb=0
    import time
    import operator
    print ">Bunch pattern for RUN: ", OPT['--run'] 
#    print "bunch list: ",bcids
    for tt,rr,ctpmon_i in zip(monTimeRanges,rates,range(len(rates))):

        bptx0_filled_old=bptx0_filled;
        bptx1_filled_old=bptx1_filled;
        bptxB_filled_old=bptxB_filled;
            
#        starttime=time.ctime(tt[0]/1e9)
        stoptime =time.ctime(tt[1]/1e9)
#        print "CTPMON data time range: ",starttime,stoptime," seconds: ", '%.1f' % ((tt[1]-tt[0])*1e-9)
       
        bptxindex0=-1
        bptxindex1=-1
        for i,thisname in enumerate(OPT["--countersnames"]):
            if 'BPTX0' in  thisname:
                bptxindex0=i
            if 'BPTX1' in  thisname:
                bptxindex1=i

        if bptxindex0<0 or         bptxindex1<0 :
            print "can not look at filling scheme if both bptx is not there"
            break

        bptx0=boolify(rr[bptxindex0])
        bptx1=boolify(rr[bptxindex1])
        bptxB=boolcoinsidence(bptx0,bptx1)
#        print bptx0
#        print bptx1
#        print bptx_both
        bptx0_filled=getfilled(OPT["--bcids"],bptx0)
        bptx1_filled=getfilled(OPT["--bcids"],bptx1)
        bptxB_filled=getfilled(OPT["--bcids"],bptxB)

        if  (bptx0_filled_old!=bptx0_filled or
             bptx1_filled_old!=bptx1_filled or 
             bptxB_filled_old!=bptxB_filled ) or starttime==0 :

            duration=0
            if starttime != 0:
                duration = tt[1]-starttime
                print '>End of record: record length was [s] ',(tt[0]-starttime)/1e9
                print ">-----------------------------------------"
                print ">Update at: ",time.ctime(tt[0]/1e9)
            else:
                print ">-----------------------------------------"
                print ">First record at: ",time.ctime(tt[0]/1e9)

            print ">Record start in terms of Lumiblocks:", '%.1f' % monblockLBs[ctpmon_i][0]
            # ,'to', '%.1f' % monblockLBs[ctpmon_i][1]        
            print ">#Beam1:",len(bptx0_filled)," #Beam2:", len(bptx1_filled), " #Coincidence: ",len(bptxB_filled)
            print "> Beam1:       ",bptx0_filled
            print "> Beam2:       ", bptx1_filled
            print "> Coincidence: ",bptxB_filled        

            if starttime == 0:
                starttime=tt[0]
if OPT['--bcidpattern']:
    showBCIDS(monTimeRanges,rates,monblockLBs,OPT["--counters"],OPT["--bcids"])

#+------------------------------------
#+ Fill histograms and make plots if requested
#+------------------------------------
if not OPT["--nofillHist"]:
#    ctpmon_py.plot(OPT,monblockLBs,rates,lbmatches,monblockLBs,opts)
    ctpmon_py.plot(OPT,monTimeRanges,rates,lbmatches,monblockLBs,opts)

    
