#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Produce LAr DQ plots with all inefficiencies summarized
# Author : B.Trocme

import os, sys  
import string
from ROOT import *
from ROOT import gStyle, TCanvas, TString, TPad,TPaveText
from time import asctime,gmtime,localtime
sys.path.append("../MiscLibraries")
import MyCOOLlib
import xmlrpclib

import gb
import time
import runList

sys.path.append("/afs/cern.ch/user/l/larcalib/LArDBTools/python/")

import showEventVeto

########################################################################
def listify(l):
  if len(l)==0: return ''
  elif len(l)==1: return str(l[0]) 
  l.sort()
  interval=False
  a = ''
  for i in range(len(l)-1):
    if interval: 
      if l[i+1]-l[i]==1: pass
      else: a += str(l[i])+' '; interval=False
    else:
      a += str(l[i])
      if l[i+1]-l[i]==1: a += '-'; interval=True
      else: a += ' '
  a += str(l[-1])
  return a

########################################################################
def printBoth(string,boolean,f):
  print string
  if boolean:# This is a single run report or the end of a weekly report - Also write on txt file
    f.write(string+'\n')
  return

########################################################################
def findLB(lbTimeStampS,startOfVetoPeriod):
  for i in xrange(len(lbTimeStampS)):
    if (startOfVetoPeriod<lbTimeStampS[i]):
#      print startOfVetoPeriod,lbTimeStampS[i]
      return i
  return i

########################################################################
from DQDefects import DefectsDB

partcolors = { 'EMBA':kYellow-9,'EMBC':kYellow,'EMECA':kOrange,'EMECC':kOrange-3,'HECA':kRed-3,'HECC':kRed+2,'FCALA':kBlue-3,'FCALC':kBlue+2}
partitions = partcolors.keys()

partIntolDefect = ["HVTRIP","SEVNOISEBURST","SEVCOVERAGE","HVNONNOMINAL","SEVNOISYCHANNEL"] # Order determines what defect is proeminent
partTolDefect = ["NOISEBURST"]
globIntolDefect = ["DATACORRUPT","RECOCORRUPT"]
globTolDefect = []

defectName = {"HVTRIP":"high voltage trip","NOISEBURST":"noise bursts (before veto)","HVNONNOMINAL":"HV non nominal(not corrected)","SEVNOISEBURST":"noise burst (after veto)","SEVNOISYCHANNEL":"noisy channels","SEVCOVERAGE":"coverage","DATACORRUPT":"data corruption","RECOCORRUPT":"corrupted reconstruction"}
defectColor= {"HVTRIP":kBlue,"HVNONNOMINAL":kPink,"SEVNOISEBURST":kOrange,"SEVNOISYCHANNEL":kBlack,"SEVCOVERAGE":kRed,"DATACORRUPT":kGreen,"RECOCORRUPT":kMagenta} # Color only for intolerable defects

# Options to switch by hand
doplot = True;#doplot = False
doSubPeriod = True;#doSubPeriod = False # Plot results in term of data period
doSaveHisto = True;doSaveHisto = False # Save histo at the end
keepVdM_LHC = True;#keepVDM_LHC = True; # Keep the special run (VDM or LHCComm)
exactVetoComput = True;#exactVetoComput = False # By setting this flag to true, ignore veto period if LB affected by intol defect. Slower
defectTag="";#defectTag = "DetStatus-v53-pro13-04"
vetoTag="LARBadChannelsOflEventVeto-UPD4-04"

if len(sys.argv) < 2  :
    print
    print "usage: python %s <run1> [<run2>]"%(sys.argv[0])
    print
    sys.exit()

runlistToUse = runList.runlist2012
runlistToUse = runlistToUse + runList.runlist2012_heavyIon
runlistToUse = runlistToUse + runList.runlist2011


runlist = []
missingSignOff = []

if len(sys.argv) == 2:
  singleRunNumber = int(sys.argv[1])
  doplot=False;doSubPeriod=False; doSaveHisto=False#Only 1 run - Do not produce plots, nor period plots, nor save them! 
  singleRun=True
  if (singleRunNumber not in (runlistToUse)):
    print "------------>Please first add the run in the run list"
    sys.exit()
  runlist.append(singleRunNumber)

if len(sys.argv) == 3:
  startrun = int(sys.argv[1])
  endrun   = int(sys.argv[2])+1
  singleRun = False
  runlistTmp = []
  for runnum in (runlistToUse):
    if (runnum>=startrun and runnum<endrun):
      runlistTmp.append(runnum)
# Retrieve data period - Not useful in single run
  passfile = open("/afs/cern.ch/user/l/larmon/passfile.txt")
  passwd = passfile.read().strip(); passfile.close()
  s = xmlrpclib.ServerProxy('https://%s@atlasdqm.cern.ch'%(passwd))
  run_spec = {'run_list': runlistTmp}
  period = s.get_data_periods(run_spec)
  periodlist = []
  dataPeriod = {}
  for runnum in runlistTmp:
    if (len(period["%d"%(runnum)])>1):
      dataPeriod[runnum]=period["%d"%(runnum)][1]
      if (keepVdM_LHC or not("LHC" in dataPeriod[runnum] or "VdM" in dataPeriod[runnum])):
        if (period["%d"%(runnum)][1] not in periodlist):
          periodlist.append(period["%d"%(runnum)][1])
    else:
      dataPeriod[runnum]="???"
    if (keepVdM_LHC or not("LHC" in dataPeriod[runnum] or "VdM" in dataPeriod[runnum])):
      runlist.append(runnum)

### Book Histograms - Run inefficiency per partition

h1_partDefect = {}
# Partition defects vs run number
for idef in partIntolDefect+partTolDefect:
  h1_partDefect[idef] = {}
  for ipart in partitions:
    h1_partDefect[idef][ipart] = gb.MakeTH1("h1_%s_%s"%(idef,ipart),"Run Number","Fraction of LBs with %s - %s"%(defectName[idef],ipart),-0.5,-0.5+len(runlist),len(runlist),partcolors[ipart])
    gb.SetXLabel(h1_partDefect[idef][ipart],runlist)
  h1_partDefect[idef]["AllPartitions"] = gb.MakeTProfile("h1_%s_AllPartitions"%(idef),"Run Number","Prop. of LBs with %s"%(defectName[idef]),-0.5,-0.5+len(runlist),len(runlist),1)
  gb.SetXLabel(h1_partDefect[idef]["AllPartitions"],runlist)

### Book Histograms for general plot with intolerable defects
h1_shortrun = gb.MakeTProfile("h1_shortrun","Short Runs","",-0.5,-0.5+len(runlist),len(runlist),kCyan-3)

h1_LArIntolDefect = {}
if (doSubPeriod):
  hProf_LArIntolDefect_period = {}
  h1_LArIntolDefect_period = {} # Will be used later on to display properly the stacked histo
for idef in globIntolDefect+partIntolDefect:#Intolerable defects only
  h1_LArIntolDefect[idef] = gb.MakeTH1("h1_ineff_%s"%(idef),"%s"%(defectName[idef]),"Lost luminosity (%)", -0.5,+0.5+len(runlist),len(runlist)+1,defectColor[idef])
  gb.SetXLabel(h1_LArIntolDefect[idef],runlist)
  h1_LArIntolDefect[idef].GetXaxis().SetBinLabel(len(runlist)+1,"All")
  if (doSubPeriod):
    hProf_LArIntolDefect_period[idef] = gb.MakeTProfile("h1_ineff_%s_period"%(idef),"%s"%(defectName[idef]),"Lost luminosity (%)", -0.5,+0.5+len(periodlist),len(periodlist)+1,defectColor[idef])
    gb.SetXLabel(hProf_LArIntolDefect_period[idef],periodlist)
    hProf_LArIntolDefect_period[idef].GetXaxis().SetBinLabel(len(periodlist)+1,"All")
    

# Book histograms for time window veto
hprof_veto_snb = gb.MakeTProfile("hprof_veto_snb","","Inefficiency - Time veto (%)", -0.5,+0.5+len(runlist),len(runlist)+1,1)
hprof_veto_dc = gb.MakeTProfile("hprof_veto_dc","","", -0.5,+0.5+len(runlist),len(runlist)+1,1)
hprof_veto_snb.SetMarkerStyle(22)
hprof_veto_dc.SetMarkerStyle(23)
hprof_veto_snb.SetMarkerColor(kBlue)
hprof_veto_dc.SetMarkerColor(kRed)
gb.SetXLabel(hprof_veto_snb,runlist)
hprof_veto_snb.GetXaxis().SetBinLabel(len(runlist)+1,"All")
if (doSubPeriod):
  hprof_veto_snb_period = gb.MakeTProfile("hprof_veto_snb_period","","Inefficiency - Time veto (%)", -0.5,+0.5+len(periodlist),len(periodlist)+1,1)
  hprof_veto_dc_period = gb.MakeTProfile("hprof_veto_dc_period","","", -0.5,+0.5+len(periodlist),len(periodlist)+1,1)
  hprof_veto_snb_period.SetMarkerStyle(22)
  hprof_veto_dc_period.SetMarkerStyle(23)
  hprof_veto_snb_period.SetMarkerColor(kBlue)
  hprof_veto_dc_period.SetMarkerColor(kRed)
  gb.SetXLabel(hprof_veto_snb_period,periodlist)
  hprof_veto_snb_period.GetXaxis().SetBinLabel(len(periodlist)+1,"All")
  gb.SetXLabel(hprof_veto_dc_period,periodlist)
  hprof_veto_dc_period.GetXaxis().SetBinLabel(len(periodlist)+1,"All")
 

if (singleRun): pass # File report defined later f = open('Reports/Run/report-%s.txt' % (startrun), 'w')
else: f = open('Reports/Weekly/report-%s-%s.txt' % (startrun, endrun), 'w')

defAffLumiAllRuns = {}
for idef in partIntolDefect+partTolDefect+globIntolDefect+globTolDefect:
  defAffLumiAllRuns[idef] = 0.
lumiTotAllRuns = 0
totLumiVetoDB = 0.; totLumi_veto_snb = 0.; totLumi_veto_dc = 0. # Luminosity affected by time window veto - WARNING: the lumi used to normalize may be slightly different from the ones used elsewhere
heavyIonRun=True # Bit to correctly display the luminosity in heavy ions
firstEntry = True

#### Loop over selected runs
for irun,runnum in enumerate(runlist):

  print "=============================================================" 
  print "=============================Run %d======================"%(runnum) 
  # Init variables - List (indexed by partition) of tables of lumi blocks affected by defects
  lbAffected = {}
  for idef in partIntolDefect+partTolDefect:
    lbAffected[idef] = {}
  for idef in globIntolDefect+globTolDefect:
    lbAffected[idef] = [] # Global defect - Simple list and not dictionnary
  lbAffected["IntolDefect"] = [] # List of LBs affected with intolerable defect independant of the defect/partition

  exprSignedOff = True
  bulkSignedOff = True

  # Get run infos
  # Luminosity blocks UTC timestamps
  LBTS = MyCOOLlib.GetLBTimeStamps(runnum)
  # Number of luminosity blocks
  nLB = len(LBTS)-1
  # Online COOL preferred luminosity for this LB
  atlaslumi = MyCOOLlib.GetOnlineLumiFromCOOL(runnum,nLB,0)
#  atlaslumi = MyCOOLlib.GetOfflineLumiFromCOOL(runnum,nLB,0)
  duration = MyCOOLlib.GetLBDuration(runnum)
  PeakLumi = 0.
  # Look for peak lumi
  for lb in atlaslumi: 
    if lb > PeakLumi: PeakLumi=lb 
    if lb > 1: heavyIonRun=False# Lumi in 10^30
  # Atlas Ready
  atlasready=MyCOOLlib.GetReadyFlag(runnum)
  readylist=[lb+1 for lb in xrange(len(atlasready)) if atlasready[lb]>0 ]
  nLBReady = float(len(readylist))    
  nBunches = MyCOOLlib.GetNumberOfCollidingBunches(runnum)
  # Keep track of short runs
  if (nLBReady<100): h1_shortrun.Fill(irun,3.)

  # Get defects
  db = DefectsDB(tag=defectTag)
  lar_defects = [d for d in (db.defect_names | db.virtual_defect_names) if d.startswith("LAR")]
  defects = db.retrieve((runnum, 1), (runnum+1, 0), lar_defects)    
  #    nHVTrips = 0 - STILL RELEVANT??

  # Look over all defects and store in list (1 per partition and type) the affected LB
  # Consider only LB in readylist
  for defect in defects:
    # keep track of runs with missing sign-off
    if 'UNCHECKED' in defect.channel: missingSignOff.append([runnum,defect.channel])
    #print defect.since.run,defect.since.lumi,defect.until.lumi,defect.channel
    defectSplitted = defect.channel.split("_")
    partAffected=defectSplitted[1] # Return dummy value for a non partition defect but not problematic
    defectFound=defectSplitted[len(defectSplitted)-1] # take the last part of the name - May not work with zerobias_unsparsified

    for idef in partIntolDefect+partTolDefect:
      if idef==defectFound:
        for lb in range(defect.since.lumi,defect.until.lumi):
          if((lb in readylist) or nLBReady==0):
            if not lbAffected[idef].has_key(partAffected):
              lbAffected[idef][partAffected]=[]
            lbAffected[idef][partAffected].append(lb)
            if ((idef in partIntolDefect) and lb not in lbAffected["IntolDefect"]):
              lbAffected["IntolDefect"].append(lb)

    for idef in globIntolDefect+globTolDefect:
      if idef==defectFound:
        for lb in range(defect.since.lumi,defect.until.lumi):
          if((lb in readylist) or nLBReady==0):
            lbAffected[idef].append(lb)
          if ((idef in partIntolDefect) and lb not in lbAffected["IntolDefect"]):
            lbAffected["IntolDefect"].append(lb)


# When all defects have been found, fill per partition histos
  for ipart in partitions:
    if nLBReady>0:
      nLBRef = float(nLBReady)
    else:
      nLBRef = float(nLB)
    for idef in partIntolDefect+partTolDefect:
      if lbAffected[idef].has_key(ipart):
        h1_partDefect[idef][ipart].Fill(irun,100.*float(len(lbAffected[idef][ipart]))/nLBRef)
      

# Now remove duplication between partition/defects (intolerable only!)
# lbAffected[idef]["AllPartition"] are referred to all partition for this run
  lbAffectedPrevDef = []
  for idef in partIntolDefect:
    lbAffected[idef]["AllPartitions"]=[]
    for ipart in partitions:
      if lbAffected[idef].has_key(ipart):
        for lumiBlock in lbAffected[idef][ipart]:
          if (lumiBlock not in lbAffected[idef]["AllPartitions"]) and (lumiBlock not in lbAffectedPrevDef):
            lbAffected[idef]["AllPartitions"].append(lumiBlock)
            lbAffectedPrevDef.append(lumiBlock)
  for idef in partTolDefect: # In tolerable defect, do not remove overlap with previous defects
    lbAffected[idef]["AllPartitions"]=[]
    for ipart in partitions:
      if lbAffected[idef].has_key(ipart):
        for lumiBlock in lbAffected[idef][ipart]:
          if (lumiBlock not in lbAffected[idef]["AllPartitions"]):
            lbAffected[idef]["AllPartitions"].append(lumiBlock)
        
  # Extract inst. lumi to cumpute affected luminosity
  defAffLumiRun = {}
  for idef in (partIntolDefect+partTolDefect+globIntolDefect+globTolDefect):
    defAffLumiRun[idef] = 0.
  lumiTotRun = 0
  if nLBReady>0:
    lbrange=range(readylist[0],readylist[len(readylist)-1]+1)
  else:
    lbrange=range(1,nLB)
  for lb in lbrange:
    lumiTotRun = lumiTotRun +atlaslumi[lb-1]*duration[lb-1]
    lumiTotAllRuns = lumiTotAllRuns +atlaslumi[lb-1]*duration[lb-1]
    for idef in (partIntolDefect+partTolDefect):
      if lb in lbAffected[idef]["AllPartitions"]:
        defAffLumiRun[idef] = defAffLumiRun[idef] + atlaslumi[lb-1]*duration[lb-1]
        defAffLumiAllRuns[idef] = defAffLumiAllRuns[idef] + atlaslumi[lb-1]*duration[lb-1]

    for idef in (globIntolDefect+globTolDefect): # Duplicate with partdefect to be fixed
      if lb in lbAffected[idef]:
        defAffLumiRun[idef] = defAffLumiRun[idef] + atlaslumi[lb-1]*duration[lb-1]
        defAffLumiAllRuns[idef] = defAffLumiAllRuns[idef] + atlaslumi[lb-1]*duration[lb-1]

  if runnum == 211541: # STILL RELEVANT?
     lumiTotRun = 3.344
     
  if lumiTotRun==0: lumiTotRun=1e-50
  # Fill AllPartitions histos for partition defects
  LArIneffRun = 0
  for idef in (partIntolDefect):
    h1_partDefect[idef]["AllPartitions"].Fill(irun,100*defAffLumiRun[idef]/lumiTotRun)
    LArIneffRun = LArIneffRun + defAffLumiRun[idef]
    h1_LArIntolDefect[idef].Fill(irun,100*defAffLumiRun[idef]/lumiTotRun)
    if (doSubPeriod and dataPeriod[runnum]!="???"):
      hProf_LArIntolDefect_period[idef].Fill(periodlist.index(dataPeriod[runnum]),100*defAffLumiRun[idef]/lumiTotRun,lumiTotRun)
      hProf_LArIntolDefect_period[idef].Fill(len(periodlist),100*defAffLumiRun[idef]/lumiTotRun,lumiTotRun)
  for idef in (partTolDefect):
    h1_partDefect[idef]["AllPartitions"].Fill(irun,100*defAffLumiRun[idef]/lumiTotRun)
  for idef in (globIntolDefect):
    LArIneffRun = LArIneffRun + defAffLumiRun[idef]
    h1_LArIntolDefect[idef].Fill(irun,100*defAffLumiRun[idef]/lumiTotRun) 
    if (doSubPeriod and dataPeriod[runnum]!="???"):
      hProf_LArIntolDefect_period[idef].Fill(periodlist.index(dataPeriod[runnum]),100*defAffLumiRun[idef]/lumiTotRun,lumiTotRun) 
      hProf_LArIntolDefect_period[idef].Fill(len(periodlist),100*defAffLumiRun[idef]/lumiTotRun,lumiTotRun)
  for idef in (globTolDefect):
    h1_partDefect[idef]["AllPartitions"].Fill(irun,100*defAffLumiRun[idef]/lumiTotRun)
   
  LArIneffRun=100*LArIneffRun/lumiTotRun

  # Retrieve the length of time period vetoed
  db="COOLOFL_LAR/COMP200"
  totalVeto = showEventVeto.showEventVetoFolder(db,vetoTag,runnum,runnum,0)
  #print "totalVeto,totalVeto/1.e9:",totalVeto,[y/1.e9 for y in totalVeto]
  lumiUsed = 0.
  if (exactVetoComput):# Computation of veto rejection w/o considering LB already in defect list - Slower...
    veto_snb = 0
    for iVetoedLB in xrange(len(totalVeto["allNoise"])): 
      lb = findLB(LBTS,totalVeto["allNoise"][iVetoedLB][0]/1e9)
#      print iVetoedLB,totalVeto["allNoise"][iVetoedLB][0]/1e9,lb,(totalVeto["allNoise"][iVetoedLB][1]-totalVeto["allNoise"][iVetoedLB][0])/1e9
      if (lb not in lbAffected["IntolDefect"]):
          veto_snb = veto_snb + atlaslumi[lb-1]*((totalVeto["allNoise"][iVetoedLB][1]-totalVeto["allNoise"][iVetoedLB][0])/1e9)
    veto_snb = veto_snb/lumiTotRun*100.

    veto_dc = 0
    for iVetoedLB in xrange(len(totalVeto["allCorruption"])):
      lb = findLB(LBTS,totalVeto["allCorruption"][iVetoedLB][0]/1e9)
#      print iVetoedLB,totalVeto["allCorruption"][iVetoedLB][0]/1e9,lb,(totalVeto["allCorruption"][iVetoedLB][1]-totalVeto["allCorruption"][iVetoedLB][0])/1e9
      if (lb not in lbAffected["IntolDefect"]):
          veto_dc = veto_dc + atlaslumi[lb-1]*((totalVeto["allCorruption"][iVetoedLB][1]-totalVeto["allCorruption"][iVetoedLB][0])/1e9)
    veto_dc = veto_dc/lumiTotRun*100.

    lumiUsed = lumiTotRun
  else:
    veto_snb = totalVeto['noiseBurst'][2]*100/totalVeto['totalLumi']
    veto_dc = totalVeto['corruption'][2]*100/totalVeto['totalLumi']
    lumiUsed = totalVeto['totalLumi']

  veto_all = veto_snb+veto_dc
  hprof_veto_snb.Fill(irun,veto_snb)
  hprof_veto_dc.Fill(irun,veto_dc)
  hprof_veto_snb.Fill(len(runlist),veto_snb,lumiUsed) # Fill last bins (all runs)
  hprof_veto_dc.Fill(len(runlist),veto_dc,lumiUsed) # Fill last bins (all runs)
  if (doSubPeriod and dataPeriod[runnum]!="???"):
    hprof_veto_snb_period.Fill(periodlist.index(dataPeriod[runnum]),veto_snb,lumiUsed)
    hprof_veto_dc_period.Fill(periodlist.index(dataPeriod[runnum]),veto_dc,lumiUsed)
    hprof_veto_snb_period.Fill(len(periodlist),veto_snb,lumiUsed) # Fill last bins (all runs)
    hprof_veto_dc_period.Fill(len(periodlist),veto_dc,lumiUsed) # Fill last bins (all runs)
  totLumi_veto_snb = totLumi_veto_snb + lumiUsed/100*veto_snb
  totLumi_veto_dc = totLumi_veto_dc + lumiUsed/100*veto_dc
  totLumiVetoDB = totLumiVetoDB+lumiUsed

  # Writing the report in a text file for WeeklyReports
  normLB = 1. # Set it to 60 by hand if the true duration is not retrieved from LB
  if (firstEntry and not singleRun):
    firstEntry = False
    column = []
    labels_col = ["Run","Run start / stop","LB ready","Peak lumi","Int. lumi","GRL ineff.","Veto ineff.","Period","Status"]
    labels_xlow = [0.01,0.08,0.41,0.49,0.58,0.665,0.755,0.855,0.925,0.99]
    for i in xrange(len(labels_col)):
      column.append(TPaveText(labels_xlow[i],max(.99-0.08*len(runlist),0.01),labels_xlow[i+1],0.99))
      column[i].AddText(labels_col[i])
      if (i%2 == 0):
        column[i].SetFillColor(kOrange-3)
      else:
        column[i].SetFillColor(kGreen+2)
                      
  if (not singleRun and irun<25): # This is a weekly report
    column[0].AddText("%d"%(runnum))
    column[1].AddText("%s / %s"%(time.strftime("%a, %d %b %H:%M",time.localtime(LBTS[0])),(time.strftime("%a, %d %b %H:%M",time.localtime(LBTS[len(LBTS)-1])))))
    column[2].AddText("%s"%(listify(readylist)))
    if heavyIonRun:
      column[3].AddText("%.1f.10^28"%(PeakLumi*1e2))
      column[4].AddText("~%.1f ub-1"%(lumiTotRun*normLB))
    else:
      column[3].AddText("%d.10^32"%(int(PeakLumi)/100))
      column[4].AddText("~%d pb-1"%(int(lumiTotRun*normLB/1e6)))
    column[5].AddText("%.1f %%"%(LArIneffRun))
    column[6].AddText("%.1f %%"%(veto_all))
    column[7].AddText("%s"%(dataPeriod[runnum]))
    if ([runnum,"LAR_UNCHECKED"] in missingSignOff): # Express stream not yet signed off
      column[8].AddText("EXPR.")
    else:
      if ([runnum,"LAR_BULK_UNCHECKED"] in missingSignOff):
        column[8].AddText("BULK")
      else:
        column[8].AddText("DONE")

  if singleRun: # For single run, create a report file if only the run was signed off
    if ([runnum,"LAR_UNCHECKED"] in missingSignOff): # Express stream not yet signed off
      exprSignedOff = False
      bulkSignedOff = False
      f = open('Reports/Run/Dummy.txt', 'w')
    else:
      if ([runnum,"LAR_BULK_UNCHECKED"] in missingSignOff):
        bulkSignedOff = False
        f = open('Reports/Run/%sExpress.txt' % (runnum), 'w')
        f.write('Offline LAr DQ (Express) - Run %d\n'%(runnum))
      else:
        exprSignedOff = False
        f = open('Reports/Run/%sBulk.txt' % (runnum), 'w')          
        f.write('Offline LAr DQ (Bulk) - Run %d\n'%(runnum))

  printBoth('Run start : %s'%(time.strftime("%a, %d %b %H:%M",time.localtime(LBTS[0]))),singleRun and exprSignedOff,f)
  printBoth('Run stop  : %s'%(time.strftime("%a, %d %b %H:%M",time.localtime(LBTS[len(LBTS)-1]))),singleRun and exprSignedOff,f)
  printBoth('LB with ATLAS ready = %9s'%(listify(readylist)),singleRun and exprSignedOff,f)
  if singleRun and exprSignedOff: f.write('Nb of bunches = %d\n'%(nBunches))
  if heavyIonRun:
    printBoth('Peak lumi = %.1f * 10^28'%(PeakLumi*1e2),singleRun and exprSignedOff,f)
  else:
    printBoth('Peak lumi = %d * 10^32'%(int(PeakLumi)/100),singleRun and exprSignedOff,f)
  if singleRun and exprSignedOff:
    if heavyIonRun:
      f.write('Integrated lumi ~ %.1f ub-1\n\n'%(lumiTotRun*normLB))
    else:
      f.write('Integrated lumi ~ %d pb^-1\n\n'%(int(lumiTotRun*normLB/1e6)))
  printBoth('LAr performance=================================================',singleRun and bulkSignedOff,f)
  for idef in (globIntolDefect):
    if (len(lbAffected[idef])>0):
      printBoth('Nb of LBs with %24s: %i -> %.2f%%'%(string.ljust(defectName[idef],24),len(lbAffected[idef]),defAffLumiRun[idef]/lumiTotRun*100),singleRun and bulkSignedOff,f)
      print "-----> LBs affected : ",lbAffected[idef]
  for idef in (partIntolDefect):
    if (len(lbAffected[idef]["AllPartitions"])>0):
      printBoth('Nb of LBs with %24s: %i -> %.2f%%'%(string.ljust(defectName[idef],24),len(lbAffected[idef]["AllPartitions"]),defAffLumiRun[idef]/lumiTotRun*100),singleRun and bulkSignedOff,f)
      print "-----> LBs affected : ",lbAffected[idef]["AllPartitions"]
      
  printBoth('LAr GRL inefficiency                   : %.2f%%'%(LArIneffRun),singleRun and bulkSignedOff,f)
  printBoth('LAr data corruption veto inefficiency  : %.2f%%'%(veto_dc),singleRun and bulkSignedOff,f)
  printBoth('LAr noise burst veto inefficiency      : %.2f%%\n'%(veto_snb),singleRun and (exprSignedOff or bulkSignedOff),f)
  if (veto_snb < 0.01):
    print "WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING"
    print "The veto inefficiency is very low. If this is a high lumi run, this is surprising and this means that the time veto veto was probably not updated."
    print "In this case, make sure that it is done before the end of the calib loop by getting in contact with db expert" 
    print "WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING===WARNING"
  if singleRun and exprSignedOff: f.write('High voltage trips===============================================\n')
  if singleRun and exprSignedOff: f.write('Please detail here the different trips with the LBs affected and\n')
  if singleRun and exprSignedOff: f.write('link to ATLOG.Indicate whether the noise was corrected or not yet.\n')
  if singleRun and exprSignedOff: f.write('Link to HV trip db : https://atlas-larmon.cern.ch/HVTripsDB/index.py?query=select%%20+trips.rowid,*+from+trips+INNER+JOIN+details+ON+trips.HVLINE+=+details.HVLINE+where+trips.TripRunNumber=%d'%(runnum))
  if singleRun and exprSignedOff: f.write('\nUPD4 bad channels flagging during 48 hours calib loop==========\n')
  if singleRun and exprSignedOff: f.write('Total number of sporadicBurstNoise(PS) : ???(???)\n')
  if singleRun and exprSignedOff: f.write('Total number of highNoiseHG(PS)        : ???(???)\n')
  if singleRun and bulkSignedOff: f.write('\nUPD3 bad channels flagging for future reprocessing ============\n')
  if singleRun and bulkSignedOff: f.write('Total number of sporadicBurstNoise(PS) : ???(???)\n')
  if singleRun and bulkSignedOff: f.write('Total number of highNoiseHG(PS)        : ???(???)\n')
  if singleRun and (exprSignedOff or bulkSignedOff): f.write('\nAdditionnal remarks==============================================\n')
  if singleRun and (exprSignedOff or bulkSignedOff): f.write('Any remark related to data taking, CP groups observations...\n')
  
  if (singleRun):
    if (exprSignedOff) : print "Please use the elog skeleton in : Reports/Run/%sExpress.txt"%(runnum)
    if (bulkSignedOff) : print "Please use the elog skeleton in : Reports/Run/%sBulk.txt"%(runnum)
# End of loop on runs
    
# Treatment when a run range was considered (weekly report)
if (not singleRun and lumiTotAllRuns!=0):
  # Compute inefficiencies for the whole period
  lostLumiAllRuns = 0.
  for idef in (partIntolDefect+globIntolDefect):
    lostLumiAllRuns = lostLumiAllRuns + defAffLumiAllRuns[idef]


  if heavyIonRun:
    printBoth("Number of runs:%i  Total Luminosity:%.2f ub-1"%(len(runlist),lumiTotAllRuns*normLB),True,f)
  else:
    printBoth("Number of runs:%i  Total Luminosity:%.2f pb-1"%(len(runlist),lumiTotAllRuns*normLB/1e6),True,f)
  printBoth( "Period GRL inefficiency     : %.2f%%"%(lostLumiAllRuns/lumiTotAllRuns*100),True,f)
  for idef in (partIntolDefect+globIntolDefect):
    printBoth( " - %24s : %.2f%%"%(string.ljust(defectName[idef],24),defAffLumiAllRuns[idef]/lumiTotAllRuns*100),True,f)
    h1_LArIntolDefect[idef].Fill(len(runlist),defAffLumiAllRuns[idef]/lumiTotAllRuns*100)
     # The filling for the period TProfile was done before.
     # This is correct as this is a TProfile. It is not possible for run TH1.
#    if (doSubPeriod):
#      hProf_LArIntolDefect_period[idef].Fill(len(periodlist),defAffLumiAllRuns[idef]/lumiTotAllRuns*100)
  printBoth( "Period veto inefficiency:",True,f)
  printBoth( " - data corruption: %.2f%%"%(totLumi_veto_dc*100/totLumiVetoDB),True,f)
  printBoth( " - noise bursts   : %.2f%%"%(totLumi_veto_snb*100/totLumiVetoDB),True,f)
#  h1_veto_snb.Fill(len(runlist),totLumi_veto_snb*100/totLumiVetoDB)
#  h1_veto_dc.Fill(len(runlist),totLumi_veto_dc*100/totLumiVetoDB)

  column[0].AddText("All")
  column[1].AddText("-")
  column[2].AddText("-")
  column[3].AddText("-")
  if heavyIonRun:
    column[4].AddText("~%.1f ub-1"%(lumiTotAllRuns*normLB))
  else:
    column[4].AddText("~%d pb-1"%(int(lumiTotAllRuns*normLB/1e6)))
  column[5].AddText("%.2f %%"%(lostLumiAllRuns/lumiTotAllRuns*100))
  column[6].AddText("%.2f %%"%((totLumi_veto_dc+totLumi_veto_snb)*100/totLumiVetoDB))
  column[7].AddText("-")
  column[8].AddText("-")

## Checks runs with missing Sign-Off
  print "---------------------------------"
  print "Runs with missing sign-off"
  print "---------------------------------"
  for item in missingSignOff:
    print "@%s"%(item[0])
    print "%s G # Signed off"%(item[1])
#    print "---> WARNING for run %s: %s"%(item[0],item[1])
  print "---------------------------------"

f.close()
### Show Plots
if doplot:
  c1=TCanvas("Summary","Summary for the whole period",10,10,1000,600)
  for i in xrange(len(column)):
    column[i].Draw()
#  raw_input("Press Enter To Continue: ")
  c1.Print('Plots/Summary-%s-%s.png' % (startrun, endrun))

  gStyle.SetOptStat(0)
  gStyle.SetHistMinimumZero()

  c2= TCanvas( 'c2',"LAr inefficiency (GRL) vs Run Number", 200, 10, 1000, 500)
  c2.SetGridy(1)
  h_stack_ineff = THStack("h_stack_ineff","")
  lg2 = gb.MakeLegend(0.65,0.65,0.8,0.9)
  lg2.AddEntry(h1_shortrun,"short run (nLBs<100)","p")
  for idef in (globIntolDefect+partIntolDefect):
    h_stack_ineff.Add(h1_LArIntolDefect[idef])
    if (h1_LArIntolDefect[idef].GetMaximum() != 0): lg2.AddEntry(h1_LArIntolDefect[idef],defectName[idef],"f")
  mx = h_stack_ineff.GetMaximum()*1.2
  h_stack_ineff.SetMaximum(mx)
  h_stack_ineff.Draw("hist")
  h_stack_ineff.GetYaxis().SetTitle("Run number")
  h_stack_ineff.GetYaxis().SetTitle("Lost luminosity - GRL (%)")
  # mark shortruns
  h1_shortrun.SetMarkerStyle(29)
  h1_shortrun.SetMarkerSize(2.)
  h1_shortrun.Draw("psame")
  lg2.Draw()
#  raw_input("Press Enter To Continue: ")
  c2.Print('Plots/LArIneffVsRunNumber-%s-%s.png' % (startrun, endrun))

  if (doSubPeriod):
    c3= TCanvas( 'c3',"LAr inefficiency (GRL) vs Period", 200, 10, 1000, 500)
    c3.SetGridy(1)
    h_stack_ineff_period = THStack("h_stack_ineff","")
    lg3 = gb.MakeLegend(0.65,0.65,0.8,0.9)
    for idef in (globIntolDefect+partIntolDefect):
      h1_LArIntolDefect_period[idef]=hProf_LArIntolDefect_period[idef].ProjectionX()# Need to project to transform TProfile in TH1 to allow stacking
      h1_LArIntolDefect_period[idef].SetFillColor(defectColor[idef])
      h_stack_ineff_period.Add(h1_LArIntolDefect_period[idef])
      if (hProf_LArIntolDefect_period[idef].GetMaximum() != 0): lg3.AddEntry(hProf_LArIntolDefect_period[idef],defectName[idef],"f")
    mx = h_stack_ineff_period.GetMaximum()*1.2
    h_stack_ineff_period.SetMaximum(mx)
    h_stack_ineff_period.Draw("hist")
    h_stack_ineff_period.GetYaxis().SetTitle("Period")
    h_stack_ineff_period.GetYaxis().SetTitle("Lost luminosity - GRL (%)")
    lg3.Draw()
#    raw_input("Press Enter To Continue: "
    c3.Print('Plots/LArIneffVsPeriod-%s-%s.png' % (startrun, endrun))    

  c4= TCanvas('c4',"LAr inefficiency (Time veto) vs Run Number", 200, 10, 1000, 500)
  c4.SetGridy(1)
  c4.SetLogy(1)
  hprof_veto_snb.SetMaximum(max(hprof_veto_snb.GetMaximum(),hprof_veto_dc.GetMaximum())*2)
  hprof_veto_snb.SetTitle("")
  hprof_veto_snb.Draw("P")
  hprof_veto_dc.Draw("PSAME")
  h1_shortrun.Draw("psame")
  lg4 = gb.MakeLegend(0.65,0.65,0.8,0.9)
  lg4.AddEntry(hprof_veto_snb,"Noise bursts")
  lg4.AddEntry(hprof_veto_dc,"Data corruption")
  lg4.AddEntry(h1_shortrun,"short run (nLBs<100)","p")
  lg4.Draw()
#  raw_input("Press Enter To Continue: ")
  c4.Print('Plots/LArIneffTimevetoVsRunNumber-%s-%s.png' % (startrun, endrun))

  if (doSubPeriod):
    c5= TCanvas('c5',"LAr inefficiency (Time veto) vs Period", 200, 10, 1000, 500)
    c5.SetGridy(1)
    c5.SetLogy(1)
    hprof_veto_snb_period.SetMaximum(max(hprof_veto_snb_period.GetMaximum(),hprof_veto_dc_period.GetMaximum())*2)
    hprof_veto_snb_period.SetTitle("")
    hprof_veto_snb_period.Draw("P")
    hprof_veto_dc_period.Draw("PSAME")
    lg5 = gb.MakeLegend(0.65,0.65,0.8,0.9)
    lg5.AddEntry(hprof_veto_snb_period,"Noise bursts")
    lg5.AddEntry(hprof_veto_dc_period,"Data corruption")
    lg5.Draw()
#    raw_input("Press Enter To Continue: ")
    c5.Print('Plots/LArIneffTimevetoVsPeriod-%s-%s.png' % (startrun, endrun))


  hStackPartDefect = {}
  cStack = {}
  lg = gb.MakeLegend(0.70,0.55,0.80,0.85)
  for ipart in partitions:
    lg.AddEntry(h1_partDefect["HVTRIP"][ipart],ipart,"f")
      
  for idef in partIntolDefect+partTolDefect:
    hStackPartDefect[idef] = THStack("hs_%s"%(idef),"%s"%(defectName[idef]))
    for ipart in partitions:
      hStackPartDefect[idef].Add(h1_partDefect[idef][ipart])
      hStackPartDefect[idef].SetMaximum(hStackPartDefect[idef].GetMaximum()*1.4)
    if (hStackPartDefect[idef].GetMaximum() != 0.):
      cStack[idef]=TCanvas( "canvas%s"%(idef),"%s vs Run Number"%(defectName[idef]), 200, 10, 1000, 650)
      cStack[idef].Divide(1,2)
      cStack[idef].cd(1)
      hStackPartDefect[idef].Draw("hist")
      hStackPartDefect[idef].GetXaxis().SetLabelSize(0.08)
      hStackPartDefect[idef].GetYaxis().SetTitle("% of unrecoverable LBs       ")
      lg.Draw()
      cStack[idef].cd(2)
      h1_partDefect[idef]["AllPartitions"].Draw("p")
      h1_partDefect[idef]["AllPartitions"].GetXaxis().SetLabelSize(0.08)
      cStack[idef].Update()
      cStack[idef].Print('Plots/%sVsRunNumber-%s-%s.png' % (idef,startrun, endrun))
#      raw_input("Press Enter To Continue: ")


if (doSaveHisto):
  f = TFile("weeklyHisto.root","recreate")
  for idef in (globIntolDefect+partIntolDefect):
    h1_LArIntolDefect[idef].Write()
    if (doSubPeriod):
      h1_LArIntolDefect_period[idef].Write()
  h_stack_ineff.Write()
  hprof_veto_snb.Write()
  hprof_veto_dc.Write()
  if (doSubPeriod):
    h_stack_ineff_period.Write()
    hprof_veto_snb_period.Write()
    hprof_veto_dc_period.Write()
  f.Close()
  print "Histos saved!"

  
#raw_input("Press Enter To Continue: ")

    
