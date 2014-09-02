#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Dump beam spot in a COOL SQLite file (using AtlCoolTool).

#Based on InnerDetector/InDetExample/InDetBeamSpotExample/trunk/bin/dumpBeamSpot.py by Juerg Beringer

__author__  = 'James Walder'
__version__ = '$Id$'
__usage__   = '%prog [options] newCon newTag oldCon oldTag'

from CoolConvUtilities import AtlCoolTool
from PyCool import cool
import sys
import string

def getRunLumi(value):
    run = value >> 32
    lumi = value & 0xFFFFFFFF
    return (run, lumi)

class Beamspot:
    def __init__(self, begin,  end):
        self.begin = begin
        self.end   = end
        self.status  = -1
        self.posX    = 0
        self.posY    = 0
        self.posZ    = 0
        self.sigmaX  = 0
        self.sigmaY  = 0
        self.sigmaZ  = 0
        self.tiltX   = 0
        self.tiltY   = 0
        self.sigmaXY = 0
        
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-f', '--folder', dest='folder', default='/Indet/Beampos', help='COOL folder (default: /Indet/Beampos)')
parser.add_option('-d', '--debug' , dest='doDebug',action="store_true",  help='Print full debug information')
parser.add_option('-w', '--write' , dest='doPrint',action="store_true",  help='Dump to terminal differences')
parser.add_option('-r', '--run'   , dest='run'    ,default=None,  help='Specific run selection')
parser.add_option('-s', '--runs'   , dest='runs'    ,default=None,  help='comma-separated list of runs')
parser.add_option('-p', '--plot'   , dest='plot'    ,action="store_true", default=False,  help='Do plots. Requires specific run number to be set.')
parser.add_option('-b', '--batch'   , dest='batch'    ,action="store_true", default=False,  help='Non-interactive batch mode')
parser.add_option('-o', '--options'   , dest='options'    ,default="",  help='Advanced options')
parser.add_option('-l', '--list'   , dest='listOpt'    ,action="store_true",  help='List possible advanced options')

(options,args) = parser.parse_args()

topOptions = ["logy","widerange"]
topOptionsDescp = {}
topOptionsDescp["logy"] = "Set y-axis to log-scale in all plots"
topOptionsDescp["widerange"] = "Set histogram range to wide values"

runs = [] # option for multiple runs
if options.runs != None:
    a = options.runs.split(",")
    for i in a:
        runs.append( int(i) )
    print runs

if options.listOpt:
    print "Printing option types: to use, add -o flag with a comma-separated list of options"
    for i in topOptions:
        print i,
        if topOptionsDescp.has_key(i):
            print " :  ",topOptionsDescp[i]
        else:
            print " "
    sys.exit()

#check any additional input options and quit if not found
myOptions = []
if options.options:
    # print options.options
    myOptions = string.split(options.options,',')
    print "Options: ", myOptions
    for i in myOptions:
        stop = False
        if i not in topOptions:
            print i, "not found in options"
            stop = True
        if stop:
            sys.exit()

#read in the arguments
if len(args) != 4:
    parser.error('wrong number of command line arguments')
dbconnIn   = args[0]
tagIn      = args[1]
dbconnOld  = args[2]
tagOld     = args[3]

folder = options.folder

#tagIn = 'nominal'
#dbconnIn = "sqlite://;schema=beamspot.db;dbname=BEAMSPOT"
#folder = '/Indet/Beampos'
#tagOld = "IndetBeampos-ES1-Cosmic_2008_08-latest"
#dbconnOld = "COOLOFL_INDET/COMP200"

toolNew = AtlCoolTool.AtlCoolTool(dbconnIn)
toolNew.usetag(tagIn)   # Must always set tag

#print toolNew.ls()
#print toolNew.listinfo(folder)

#input =  toolNew.more(folder)



#print input[1][0]
fi =  toolNew.folderInfo(folder)

f =  toolNew.db.getFolder(folder)
print toolNew.db.existsFolder( folder )
print f
chansel=cool.ChannelSelection.all()
#objs =  f.browseObjects(0,100000000,chansel)
print cool.ValidityKeyMin,cool.ValidityKeyMax
objs = f.browseObjects(cool.ValidityKeyMin,cool.ValidityKeyMax,chansel,tagIn)
print objs

inputBeamspots = []

runAverageBeamspotsNew = {}  # the '83' beamspots from the run average values
runAverageBeamspotsOld = {}  # the '83' beamspots from the run average values

aveBeamspotsNew = {} # counts and positions of the run-averaged beamspot positions
aveBeamspotsOld = {} 

goodLbsNew = 0
aveLbsNew  = 0
goodLbsOld = 0
aveLbsOld  = 0

# read from db for the 'new' beamspots
while objs.goToNext():
    obj=objs.currentRef()
    payload=obj.payload()
    (run,lumi)   = getRunLumi(obj.since())
    (runE,lumiE) = getRunLumi( obj.until())
    if options.run != None and long(options.run) != run:
        continue
    if options.runs != None and run not in runs:
        continue
    bs = Beamspot(obj.since(), obj.until())
    bs.status  = int(obj.payloadValue('status'))
    bs.posX    = float(obj.payloadValue('posX'))
    bs.posY    = float(obj.payloadValue('posY'))
    bs.posZ    = float(obj.payloadValue('posZ'))
    bs.sigmaX  = float(obj.payloadValue('sigmaX'))
    bs.sigmaY  = float(obj.payloadValue('sigmaY'))
    bs.sigmaZ  = float(obj.payloadValue('sigmaZ'))
    bs.tiltX   = float(obj.payloadValue('tiltX'))
    bs.tiltY   = float(obj.payloadValue('tiltY'))
    bs.sigmaXY = float(obj.payloadValue('sigmaXY'))
    inputBeamspots.append(bs)
    #print bs
    if bs.status == 83 and run not in runAverageBeamspotsNew:
        print "Found new run average for (new) run: ", run
        runAverageBeamspotsNew[run] = bs
        aveBeamspotsNew[run] = []  # set new array
    if bs.status == 83:
        aveBeamspotsNew[run].append((lumi,lumiE))
        if lumi != 0 and lumiE != 4294967295L:
            aveLbsNew = aveLbsNew + lumiE - lumi
    elif bs.status == 59:
        goodLbsNew = goodLbsNew + lumiE - lumi

#tagOld = 'nominal'
#dbconnOld = "sqlite://;schema=beamspot.db;dbname=BEAMSPOT"

doPlots = options.plot


#if plotting setup the hists
if doPlots:
    from ROOT import *
    if "widerange" in myOptions:
        bdposx = 0.1
        bdsigx = 0.5
        bdposz = 8
        bdsigz = 8
        bdtilt = 1000
    else:
        bdposx = 0.01
        bdsigx = 0.05
        bdposz = 4
        bdsigz = 4
        bdtilt = 200
    
    hXdiff  = TH1D("hXdiff","#DeltaX; #DeltaX (mm); Entries;",200, -bdposx,bdposx)
    hYdiff  = TH1D("hYdiff","#DeltaY; #DeltaY (mm); Entries;",200, -bdposx,bdposx)
    hZdiff  = TH1D("hZdiff","#DeltaZ; #DeltaZ (mm); Entries;",200, -bdposz,bdposz)
    hsXdiff = TH1D("hsXdiff","#Delta#sigma(X); #Delta#sigma(X) (mm); Entries;",200, -bdsigx,bdsigx)
    hsYdiff = TH1D("hsYdiff","#Delta#sigma(Y); #Delta#sigma(Y) (mm); Entries;",200, -bdsigx,bdsigx)
    hsZdiff = TH1D("hsZdiff","#Delta#sigma(Z); #Delta#sigma(Z) (mm); Entries;",200, -bdsigz,bdsigz)
    htXdiff  = TH1D("htXdiff","#DeltaTilt x-z; #DeltaTilt x-z (#murad); Entries;",200, -bdtilt,bdtilt)
    htYdiff  = TH1D("htYdiff","#DeltaTilt y-z; #DeltaTilt y-z (#murad); Entries;",200, -bdtilt,bdtilt)



toolOld = AtlCoolTool.AtlCoolTool(dbconnOld)
toolOld.usetag(tagOld)   # Must always set tag

fOld =  toolOld.db.getFolder(folder)

#this may be slow!!!
bsPairs = {}
print "New,Old: (run,lb) Status New, Old.  Diff(New - Old): posX, posY, posZ, sigmaX, sigmaY, sigmaZ, tiltX, tiltY, sigmaXY"
for bs in inputBeamspots:
    #count = f.countObjects(bs.begin,bs.end,chansel,tagIn)
    objsOld = fOld.findObjects(bs.begin,chansel,tagOld)
    #print "Found",count," objects that match input IOV"
    #print getRunLumi(bs.begin), getRunLumi(bs.end)
    # if no entries .... print something
    bsPairs[bs] = []
    while objsOld.goToNext():
        obj=objsOld.currentRef()
        payload=obj.payload()
        bsOld = Beamspot(obj.since(), obj.until())
        bsOld.status = int(obj.payloadValue('status'))
        bsOld.posX    = float(obj.payloadValue('posX'))
        bsOld.posY    = float(obj.payloadValue('posY'))
        bsOld.posZ    = float(obj.payloadValue('posZ'))
        bsOld.sigmaX  = float(obj.payloadValue('sigmaX'))
        bsOld.sigmaY  = float(obj.payloadValue('sigmaY'))
        bsOld.sigmaZ  = float(obj.payloadValue('sigmaZ'))
        bsOld.tiltX   = float(obj.payloadValue('tiltX'))
        bsOld.tiltY   = float(obj.payloadValue('tiltY'))
        bsOld.sigmaXY = float(obj.payloadValue('sigmaXY'))
        (run,lumi)    = getRunLumi(obj.since())
        (runE,lumiE)  = getRunLumi(obj.until())        
        if bsOld.status == 83 and run not in runAverageBeamspotsOld:
            print "Found new run average for (old) run: ", run
            runAverageBeamspotsOld[run] = bsOld
            aveBeamspotsOld[run] = []  # set new array
        if bsOld.status == 83:
            aveBeamspotsOld[run].append((lumi,lumiE))
            if lumi != 0 and lumiE != 4294967295L:
                aveLbsOld = aveLbsOld + lumiE - lumi
        elif bsOld.status == 59:
            goodLbsOld = goodLbsOld + lumiE - lumi


        #print getRunLumi(bs.begin), getRunLumi(bs.end), "Old: ", getRunLumi(bsOld.begin), getRunLumi(bsOld.end) , \
        #      bs.posX ,"/", bsOld.posX, bs.posY ,"/", bsOld.posY, bs.posZ ,"/", bsOld.posZ, \
        #      bs.sigmaX,"/", bsOld.sigmaX, bs.sigmaY,"/", bsOld.sigmaY, bs.sigmaZ,"/", bsOld.sigmaZ
        (runS,lumiS)  = getRunLumi(bs.begin)
        (runE, lumiE) = getRunLumi(bs.end)
        if options.doPrint or options.doDebug:
            print getRunLumi(bs.begin), getRunLumi(bs.end), getRunLumi(bsOld.begin), getRunLumi(bsOld.end) ,
            print bs.status, bsOld.status ,
            print bs.posX - bsOld.posX, bs.posY - bsOld.posY, bs.posZ - bsOld.posZ, \
                  bs.sigmaX- bsOld.sigmaX, bs.sigmaY- bsOld.sigmaY, bs.sigmaZ- bsOld.sigmaZ, \
                  bs.tiltX - bsOld.tiltX , bs.tiltY - bsOld.tiltY, \
                  bs.sigmaXY- bsOld.sigmaXY
        if options.doDebug:
            print "New:  ", bs.posX , bs.posY, bs.posZ, \
                  bs.sigmaX, bs.sigmaY, bs.sigmaZ, \
                  bs.tiltX , bs.tiltY, \
                  bs.sigmaXY
            print "Old:  ", bsOld.posX , bsOld.posY, bsOld.posZ, \
                  bsOld.sigmaX, bsOld.sigmaY, bsOld.sigmaZ, \
                  bsOld.tiltX , bsOld.tiltY, \
                  bsOld.sigmaXY
        if bs.status != bsOld.status:
            print runS, lumiS, lumiE, bs.status, bsOld.status
        if doPlots and bs.status != 83 and abs(bs.posX - bsOld.posX) > 1e-7 and abs(bs.sigmaX - bsOld.sigmaX) > 1e-7 and bs.sigmaY !=0 and bsOld.sigmaY != 0 and bs.sigmaZ !=0 and bsOld.sigmaZ != 0 and bs.posX !=0 and bsOld.posX != 0 and bs.posY !=0 and bsOld.posY != 0 and bs.posZ !=0 and bsOld.posZ != 0 and bs.sigmaX !=0 and bsOld.sigmaX != 0:
            hXdiff.Fill(bs.posX - bsOld.posX)
            hYdiff.Fill(bs.posY - bsOld.posY)
            hZdiff.Fill(bs.posZ - bsOld.posZ)
            hsXdiff.Fill(bs.sigmaX - bsOld.sigmaX)
            hsYdiff.Fill(bs.sigmaY - bsOld.sigmaY)
            hsZdiff.Fill(bs.sigmaZ - bsOld.sigmaZ)
            htXdiff.Fill((bs.tiltX - bsOld.tiltX)*1e6)
            htYdiff.Fill((bs.tiltY - bsOld.tiltY)*1e6)


        bsPairs[bs].append( bsOld )


print "Have: ", len(bsPairs), " New beamspot"


#if debug print out the beamspot averages for the runs
if options.doPrint:
    print "******************* Average Beamspot (status 83) report **************"
    for r in runAverageBeamspotsNew:
        bs = runAverageBeamspotsNew[r]
        (run,lb) = getRunLumi(bs.begin)
        print "Ave. beamspot for run: ", run
        print "New:  ", bs.posX , bs.posY, bs.posZ, \
              bs.sigmaX, bs.sigmaY, bs.sigmaZ, \
              bs.tiltX , bs.tiltY, \
              bs.sigmaXY
        if runAverageBeamspotsOld.has_key(r):
            bsOld = runAverageBeamspotsOld[r]
            print "Old:  ", bsOld.posX , bsOld.posY, bsOld.posZ, \
                  bsOld.sigmaX, bsOld.sigmaY, bsOld.sigmaZ, \
                  bsOld.tiltX , bsOld.tiltY, \
                  bsOld.sigmaXY
        print "New: ", aveBeamspotsNew[run] if run in  aveBeamspotsNew else " No average."
        print "Old: ", aveBeamspotsOld[run] if run in  aveBeamspotsOld else " No average."
    print "******************************************************************"
            
#calculate the number and fraction of status 83 lumi blocks: excluding start and end, and wholely bad runs

print "Fraction lbs with aveBeamspots: ignores the firs and final lb ranges"
print "New: Number of good / avg / frac.avg.  lumi blocks: ", goodLbsNew, aveLbsNew, 1.*aveLbsNew/(goodLbsNew + aveLbsNew ) if (goodLbsNew + aveLbsNew) != 0 else 0
print "Old: Number of good / avg / frac.avg.  lumi blocks: ", goodLbsOld, aveLbsOld, 1.*aveLbsOld/(goodLbsOld + aveLbsOld ) if (goodLbsOld + aveLbsOld) != 0 else 0

if False:
    from ROOT import *
    
    gOld  = TGraph( len(bsPairs) )
    gNew  = TGraph( len(bsPairs) )
    gDiff =  TGraph( len(bsPairs) )
    point =0
    for bs in bsPairs.keys():
        (run,lb) = getRunLumi(bs.begin)  
        gDiff.SetPoint(point, float(lb), bs.posX - bsPairs(bs).posX )
        point = point + 1
        print bs.posX
    gDiff.Draw("ap*")
        
    raw_input("Press ENTER to exit")

#do any plots required

if doPlots:
    gROOT.SetStyle("Plain")
    gStyle.SetOptStat(112210)



    if options.run != None:
        name = options.run
    else:
        name = "global"

    cPlot = TCanvas("run_"+name)

    cPlot.Divide(3,4)
    
    if "logy" in myOptions:
        for i in range(4,12):
            cPlot.cd(i).SetLogy(1)


    cPlot.cd(1)
    l  =  TLatex(0.1,0.7,"Run: " + name)
    l1 =  TLatex(0.1,0.6,"New Tag: " + args[0] + " " + args[1])
    l2 =  TLatex(0.1,0.5,"Old Tag: " + args[2] + " " + args[3])
    l.Draw()
    l1.Draw()
    l2.Draw()

    la = TLatex(0.1,0.4,"Average values:  New         Old ")
    

    cPlot.cd(4)
    hXdiff.Draw()
    cPlot.cd(5)
    hYdiff.Draw()
    cPlot.cd(6)
    hZdiff.Draw()
    cPlot.cd(7)
    hsXdiff.Draw()
    cPlot.cd(8)
    hsYdiff.Draw()
    cPlot.cd(9)
    hsZdiff.Draw()
    cPlot.cd(10)
    htXdiff.Draw()
    cPlot.cd(11)
    htYdiff.Draw()


    cPlot.SaveAs("diffPlot_"+name+".png")
    cPlot.SaveAs("diffPlot_"+name+".eps")
    cPlot.SaveAs("diffPlot_"+name+".root")

    if not options.batch:
        raw_input("Press ENTER to exit")


print "Done"


