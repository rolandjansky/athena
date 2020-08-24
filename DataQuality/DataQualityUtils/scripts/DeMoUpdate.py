#! /usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Author : Benjamin Trocme (LPSC - Grenoble) - 2017
# Udpates the year stats
##################################################################

import os,sys  

from string import ljust
import time

from ROOT import TFile
from ROOT import TCanvas,TPaveText
from ROOT import kBlack,kOrange,kGreen
from ROOT import gStyle

import six.moves.xmlrpc_client as xmlrpclib

sys.path.append("/afs/cern.ch/user/l/larmon/public/prod/Misc")
from LArMonCoolLib import GetLBTimeStamps,GetOnlineLumiFromCOOL,GetOfflineLumiFromCOOL,GetReadyFlag,GetNumberOfCollidingBunches
from gb import MakeTH1,SetXLabel,MakeTProfile

from DeMoLib import strLumi,plotStack,initialize
import DQDefects

sys.path.append("/afs/cern.ch/user/l/larcalib/LArDBTools/python/")

import showEventVeto,showEventVetoNoLumi

from DQUtils import fetch_iovs

global startrun
global endrun

global debug
debug = False
#debug = True

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

################################################################################################################################################
def printBoth(string0,boolean,f):
  print(string0)
  if boolean:# Also write on txt file
    f.write(string0+'\n')
  return

################################################################################################################################################
def findLB(lbts,startOfVetoPeriod): # Find the lumiblock where a veto period starts
  for i in lbts.keys():
    if (startOfVetoPeriod>lbts[i][0] and startOfVetoPeriod<lbts[i][1]):
      return i
  return i

################################################################################################################################################
# print single run report. Only printing, no computation
def singleRunReport(runNumber,dict1,dict2,directory,defects,veto,exactVetoComput): 
  if dict1['signoff'] == "DONE" or dict1['signoff'] == "FINAL OK":
    repOnDisk = True
    f = open('%s/Run/%s.txt' % (directory,runNumber), 'w')
  else:
    repOnDisk = False
    f = open('%s/Run/dummy.txt'%(directory), 'w')
    
  printBoth('Run start : %s'%dict1['Start'],repOnDisk,f)
  printBoth('Run stop  : %s'%dict1['Stop'],repOnDisk,f)
  printBoth('LB with ATLAS ready = %9s'%(listify(dict1["readyLB"])),repOnDisk,f)
  printBoth('Nb of bunches = %d'%(dict1['nBunches']),repOnDisk,f)
  printBoth('Peak lumi = %.1e'%(dict1['peakLumi']*1e30),repOnDisk,f)
  printBoth('Integrated lumi ~ %s'%(strLumi(dict1['Lumi'],"ub",False,True)),repOnDisk,f)
  printBoth('%s GRL inefficiency                   : %.2f%%'%(options["system"],dict1["ineffDefects"]),repOnDisk,f)
  for idef in (defects["globIntol"]):
    if (len(dict2[idef])>0):
      printBoth('Nb of LBs with %24s: %i -> %.2f%% (%s)'%(ljust(defectVeto["description"][idef],24),len(dict2[idef]),dict1["ineffDefect_%s"%(idef)],str(dict2[idef])),repOnDisk,f)
  for idef in (defects["partIntol"]):
    if (len(dict2[idef]["AllPartitions"])>0):
      printBoth('Nb of LBs with %24s: %i -> %.2f%% (%s)'%(ljust(defectVeto["description"][idef],24),len(dict2[idef]["AllPartitions"]),dict1["ineffDefect_%s"%(idef)],str(dict2[idef]["AllPartitions"])),repOnDisk,f)
      #print "-----> LBs affected : ",dict2[idef]["AllPartitions"]
  if len(veto["all"])>0:
    if exactVetoComput:
      printBoth('LAr veto inefficiency                  : %.3f%%'%(dict1["ineffVetos"]),repOnDisk,f)
    else:
      printBoth('LAr veto inefficiency (rough computat.): %.3f%%'%(dict1["ineffVetos"]),repOnDisk,f)
    for iVeto in veto["all"]:
      printBoth('%s veto inefficiency  : %.3f%%'%(defectVeto["description"][iVeto],dict1["ineffVeto_%s"%(iVeto)]),repOnDisk,f)
  f.close()

  return


################################################################################################################################################
################################################################################################################################################
# Main script
from argparse import RawTextHelpFormatter,ArgumentParser

parser = ArgumentParser(description='',formatter_class=RawTextHelpFormatter)
parser.add_argument('-r','--run',type=int,dest='parser_run',default=[],help='Run or run range',nargs='*',action='store')
parser.add_argument('-y','--year',dest='parser_year',default = "2018",help='Year [Default: 2017]. May also include special conditions such as 5TeV, hi... Check the RunList files',action='store')
parser.add_argument('-t','--tag',dest='parser_tag',default = "Tier0_2018",help='Defect tag [Default: "Tier0_2018"]',action='store')
parser.add_argument('-b','--batch',dest='parser_batchMode',help='Batch mode',action='store_true')
parser.add_argument('-s','--system',dest='parser_system',default="LAr",help='System: LAr, CaloCP [Default: "LAr"]',action='store')
parser.add_argument('--runListUpdate',dest='parser_runListUpdate',help='Run list update. No other action allowed. Exit when done',action='store_true')
parser.add_argument('--weekly',dest='parser_weekly',help='Weekly report. No run range to specify',action='store_true')
parser.add_argument('--allRuns',dest='parser_allRuns',help='All year runs. No run range to specify',action='store_true')
parser.add_argument('--grlUpdate',dest='parser_grlUpdate',help='GRL update. No run range to specify',action='store_true')
parser.add_argument('--noPlot',dest='parser_noPlot',help='Do not plot the results',action='store_false')
parser.add_argument('--resetYS',dest='parser_resetYS',help='Reset year stats',action='store_true')
parser.add_argument('--noUpdateYS',dest='parser_noUpdateYS',help='Do not update year stats with new runs',action='store_false')
parser.add_argument('--skipUnsignedOff',dest='parser_skipUnsignedOff',help='Ignore completely runs that are not signed off',action='store_true')
parser.add_argument('--skipAlreadyUpdated',dest='parser_skipAlreadyUpdated',help='Ignore completely runs that are already in year stats',action='store_true')
parser.add_argument('--noVeto',dest='parser_noVeto',help='Do not consider time-veto information',action='store_true')
parser.add_argument('--vetoLumiEvol',dest='parser_vetoLumiEvol',help='Plot the evolution of veto as a function of lumi',action='store_true')
parser.add_argument('--saveHistos',dest='parser_saveHistos',help='Save all histograms (NOT needed for year stats)',action='store_true')
parser.add_argument('--savePlots',dest='parser_savePlots',help='Save all plots in Weekly dir',action='store_true')
parser.add_argument('--roughVetoComput',dest='parser_exactVetoComput',help='Rough veto computation (no lumi weighting,no correlation with intol defect.)',action='store_false')
parser.add_argument('--deliveredLumiNorm',dest='parser_deliveredLumiNorm',help='Normalize by the delivered lumi (used until 2018)',action='store_true')
parser.add_argument('--onlineLumiNorm',dest='parser_onlineLumiNorm',help='Normalize by the online lumi (used until 2018)',action='store_true')

args = parser.parse_args()
parser.print_help()

# Token to avoid having multiple update in the same time
print("Current time: %s"%(time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime())))

options = {}
options['system'] = args.parser_system
tokenName = "DeMo-%s-%s.token"%(options['system'],args.parser_tag)
if os.path.exists(tokenName):
  print("A different DeMoUpdate is apparently running (or it was not properly ended recently). This may cause trouble when updating yearStats")
  os.system("ls -ltr %s"%tokenName)
  print("If you are sure that it is not the case, you can remove the %s..."%tokenName)
  print("If you are not the owner, contact the DQ coordinator")
  sys.exit()
else:
  os.system("touch %s"%tokenName)

yearTagProperties = {}
partitions = {}
grlDef = {}
defectVeto = {}
veto = {}
signOff = {}

initialize(options['system'],yearTagProperties,partitions,grlDef,defectVeto,veto,signOff)

options['year'] = args.parser_year
options['batchMode'] = args.parser_batchMode
options['plotResults'] = args.parser_noPlot
options['resetYearStats'] = args.parser_resetYS # Reset the year stats. A priori, not recommended
options['updateYearStats'] = args.parser_noUpdateYS
options['skipUnsignedOff'] = args.parser_skipUnsignedOff
options['skipAlreadyUpdated'] = args.parser_skipAlreadyUpdated
options['vetoLumiEvolution'] = args.parser_vetoLumiEvol # Requires options['exactVetoComput'] = True
options['saveHistos'] = args.parser_saveHistos 
options['savePlots'] = args.parser_savePlots 
options['noVeto'] = args.parser_noVeto # No veto information - Useful for speeding up debugging or for system that do not use time veto
# Exact veto comput : weight the data loss with lumi and ignore data if intolerable defect. By default and recommended.
# Rough veto comput: only fration of time loss (no lumi weighting). To be used only if time out of showEventVeto
options['exactVetoComput'] = args.parser_exactVetoComput 
options['recordedLumiNorm'] = not args.parser_deliveredLumiNorm
options['onlineLumiNorm'] = args.parser_onlineLumiNorm
# tag to access defect database and COOL database (for veto windows) - Defined in DeMoLib.py
options['defectTag']=yearTagProperties["defect"][args.parser_tag]
if len(yearTagProperties["veto"])>0: # The veto tag is not defined for all systems
  options['vetoTag']=yearTagProperties["veto"][args.parser_tag]
else:
  options['vetoTag']=""
options['yearStatsDir'] = "YearStats-%s/%s/%s"%(options['system'],options['year'],args.parser_tag)

passfile = open("/afs/cern.ch/user/l/larmon/public/atlasdqmpass.txt")
passwd = passfile.read().strip(); passfile.close()
s = xmlrpclib.ServerProxy('https://%s@atlasdqm.cern.ch'%(passwd))

############ Update the RunList/all-[year].dat files
if args.parser_runListUpdate:
  allRunListDat = "RunList/all-%s.dat"%(options['year'])
  if os.path.exists(allRunListDat):
    latestRun = 0
    fRunList = open(allRunListDat,'r+')
    for iRun in fRunList.readlines(): # Look for the latest run in RunList/all.dat
      if int(iRun)>latestRun:
        latestRun = int(iRun)
    recentRuns = s.get_run_beamluminfo({'low_run':"%d"%(latestRun+1),'high_run':"%s"%(latestRun+5000)})

    for iRecentRun in sorted(recentRuns.keys()):
      if (recentRuns[iRecentRun][2]): # ATLAS ready
        print("I am adding the new run with ATLAS ready: %s"%iRecentRun)
        fRunList.write("%s\n"%iRecentRun)
    fRunList.close()
  else:
    print("No %s..."%allRunListDat)

  os.system("rm -f %s"%tokenName)
  print("I am exiting...")
  sys.exit()

############ Fill runlist variable and change some options if single run
# available runs are defined in ~larmon/public/prod/LADIeS/RunList directory
runlist = {'all':[],'grl':[],'%s/roughVeto'%(options['system']):[],'range':[]}
for iRunList in runlist.keys():
  iRunListDat = "RunList/%s-%s.dat"%(iRunList,options['year'])
  if os.path.exists(iRunListDat):
    fRunList = open(iRunListDat,'r')
    for iRun in fRunList.readlines():
      runlist[iRunList].append(int(iRun))
    fRunList.close()

if len(veto["all"]) == 0:
  options['noVeto'] = True
  print("No veto information provided in DeMoLib.py")
else:
  if options['noVeto']:
    print("WARNING: I do not consider time veto information...")

if options['updateYearStats']:
  yearStatsArchiveFilename = '%s/TProfiles.root'%options['yearStatsDir']
  if not os.path.exists(yearStatsArchiveFilename):
    print("No archive file found in %s"%options['yearStatsDir'])
    print("I am forcing the year stats reset...")
    options['resetYearStats'] = True
  elif os.path.getsize("%s/runs-ALL.dat"%options['yearStatsDir']) == 0.:
    # runs-ALL.data and runs-[period].dat updated only for runs in GRL
    # Test here relevant at the beginning of the year when some runs have been reviewed at EXPR/BULK level (but not FINAL hence no year stats)
    # In such a case a TProfiles.root file may exist even if no update was made
    # April 18: I am not sure that this situation is still relevant... 
    print("No run found in %s"%options['yearStatsDir'])
    print("I am forcing the year stats reset...")
    options['resetYearStats'] = True

errorLogFile = open("%s/errors.log"%options['yearStatsDir'],'a')

if (options['updateYearStats'] and options['resetYearStats']):
  print("WARNING: I am going to reset the %s stats..."%options['yearStatsDir'])
  if (options['batchMode']): # In batch mode, no confirmation requested
    confirm = "y"
  else:
    confirm = input("Please confirm by typing y: ")

  if ("y" in confirm):
    print("I reseted the %s stats"%options['yearStatsDir'])
    # Delete the dat files that contains the runs updated and the associated lumi
    os.system("rm -f %s/lumi*.dat"%options['yearStatsDir'])
    os.system("rm -f %s/runs*.dat"%options['yearStatsDir'])
    os.system("rm -f %s/errors.log"%options['yearStatsDir'])
    if (args.parser_resetYS): # The loss files are deleted only if explicit request by the user. Relevant when no GRL run found yet (hence reset forced) but defect already set
      os.system("rm -f %s/loss*.dat"%options['yearStatsDir'])
      os.system("rm -f %s/Run/*.txt"%options['yearStatsDir'])
    else:
      print("However, I did NOT delete the loss files to preserve defects set in non-GRL runs")

    # Delete the root file that contains the TProfiles
    os.system("rm -f %s"%(yearStatsArchiveFilename))
  else:
    print("I did NOT reset the %s stats"%options['yearStatsDir'])
    options['resetYearStats'] = False


runSpec = {} # Characteristics of each run: start, stop, data period, luminosity, computed inefficiencies

# Fill the list of runs to be considered 
if args.parser_allRuns: # all year runs
  runlist['toprocess'] = runlist['all']
elif args.parser_weekly: # Weekly report - Look for the last 7-days runs + unsigned off
  print("I am looking for all runs signed off in the past week and the older ones not yet signed off...")
  options['savePlots'] = True
  runlist['toprocess'] = []
  oneWeek = 7*24*3600 # Nb of seconds in one week
  runSignedOffBeyondOneWeek = []
  for iRun in range(len(runlist['all'])):
    if len(runSignedOffBeyondOneWeek) >= 5: # Do not look too far in the past for unsigned off runs...
      continue
    runNb = runlist['all'][-iRun-1] # Start by the most recent run
    if len(signOff['FINAL']) != 0:
      retrievedDefects = s.get_defects_lb({'run_list':[runNb]},signOff['FINAL'],options['defectTag'],True,True)   
    elif len(signOff['BULK']) != 0:
      retrievedDefects = s.get_defects_lb({'run_list':[runNb]},signOff['BULK'],options['defectTag'],True,True)   
    else:
      retrievedDefects = s.get_defects_lb({'run_list':[runNb]},signOff['EXPR.'],options['defectTag'],True,True)   

    fullySignedOff = True
    signOffTime = 0
    if ("%d"%runNb not in retrievedDefects.keys()): # Unsigned ??
      fullySignedOff = False
    else:
      for iDef in retrievedDefects["%d"%runNb].keys():
        if retrievedDefects["%d"%runNb][iDef][0][2] == 'Red': # Unsigned
          fullySignedOff = False
        else:
          if (retrievedDefects["%d"%runNb][iDef][0][3] > signOffTime): # Signed
            signOffTime = retrievedDefects["%d"%runNb][iDef][0][3]
      
    if (fullySignedOff and time.time()-signOffTime > oneWeek):
      runSignedOffBeyondOneWeek.append(runNb)
    runlist['toprocess'].append(runNb)
  for iRun in runSignedOffBeyondOneWeek:
    runlist['toprocess'].remove(iRun)

  runlist['toprocess'].reverse()
  print("I will process these runs :",runlist['toprocess'])
elif args.parser_grlUpdate: # Reprocess all grl runs skipping the ones already updated
  runlist['toprocess'] = runlist['grl']
  options['skipAlreadyUpdated'] = True
elif len(args.parser_run) == 1: # Single run 
  runNb = args.parser_run[0]
  if (runNb not in (runlist['all'])):
    print("------------>Please first add the run in the run list")
    os.system("rm -f %s"%tokenName)
    sys.exit()
  runlist['toprocess'] = [runNb]
  if (not options['vetoLumiEvolution']):
    options['plotResults']=False;
  options['saveHistos']=False #Do not produce plots, nor save histos! 
elif len(args.parser_run) == 2: # Run range
  startrun = min(args.parser_run)
  endrun   = max(args.parser_run)
  runlist['toprocess'] = []
  for runNb in (runlist['all']): # Add all runs of runList.py in the run range
    if (runNb>=startrun and runNb<=endrun):
      runlist['toprocess'].append(runNb)
else:
  print("Please specify run number or run range with -r option")
  os.system("rm -f %s"%tokenName)
  sys.exit()


if len(runlist['toprocess']) == 0 and len(args.parser_run)>0:
  print("No run found in this run range...")
  print("Please double check or update the runlist file...")
  os.system("rm -f %s"%tokenName)
  sys.exit()

# Retrieve data period of all runs of runlist
period = s.get_data_periods({'run_list': runlist['toprocess']})

periodListCurrent = {} # Dictionary of all runs referenced by period name
newPeriodInYearStats = [] # List of periods not yet considered
bool_newRunsInYearStats = False
for runNb in runlist['toprocess']:
  runSpec[runNb] = {}
  if (len(period["%d"%(runNb)])>1): # Found the data period
    tmp_period = period["%d"%(runNb)][0]
    runSpec[runNb]['period'] = tmp_period
    if (tmp_period not in periodListCurrent.keys()):# New data period
      periodListCurrent[tmp_period] = []
    periodListCurrent[tmp_period].append(runNb)
  else: # Did not find the data period
    runSpec[runNb]['period'] = "???"
    runSpec[runNb]['newInYearStats'] = False
    print("I did not find the data period for run %d"%(runNb))

for iper in periodListCurrent.keys(): # Loop on all periods found and look for new periods/runs
  periodFileName = "%s/runs-%s.dat"%(options['yearStatsDir'],iper)
  if os.path.exists(periodFileName): # This period is already treated in year stats. Look for new runs
    f = open(periodFileName,'r')
    existingRuns = f.readlines()
    for irun in periodListCurrent[iper]:
      if options['updateYearStats']:
        if irun in runlist['grl']:
          runSpec[irun]['newInYearStats'] = True
        else:
          runSpec[irun]['newInYearStats'] = False
          print("Run %d not in GRL run list -> Ignored for YearStats"%irun)
      else:
        runSpec[irun]['newInYearStats'] = False
      if "%d\n"%(irun) in existingRuns:
        runSpec[irun]['newInYearStats'] = False # This run was already treated. Not used in incremental year stats
    f.close()
  else: # This is a period not yet treated in year stats.
    periodToBeAdded = False
    for irun in periodListCurrent[iper]:
      if options['updateYearStats']:
        if irun in runlist['grl']:
          runSpec[irun]['newInYearStats'] = True
          periodToBeAdded = True
        else:
          runSpec[irun]['newInYearStats'] = False
          print("Run %d not in GRL run list -> Ignored for YearStats"%irun)
      else:
        runSpec[irun]['newInYearStats'] = False
    if options['updateYearStats'] and periodToBeAdded:
      print("I am going to add period %s in year stats!"%(iper))
      newPeriodInYearStats.append(iper)


for iper in periodListCurrent.keys(): # Loop on all periods founds and print the runs to be updated
  for irun in periodListCurrent[iper]:
    if runSpec[irun]['newInYearStats']:
      print("I am going to add run %d (period %s) in %s stats (provided that it is fully signed off - Not yet known...)!"%(irun,runSpec[irun]['period'],options['year']))
      bool_newRunsInYearStats = True
    else:
      if (options['skipAlreadyUpdated']):
        runSpec.pop(irun)
        runlist['toprocess'].pop(runlist['toprocess'].index(irun))
        print("%d was already processed in yearStats - I am complety ignoring it..."%(irun))

if (not bool_newRunsInYearStats):
  options['updateYearStats'] = False # No new run -> no update needed

runSpec['AllRuns'] = {}
runSpec['AllRuns']['Start'] = "-"
runSpec['AllRuns']['Stop'] = "-"
runSpec['AllRuns']['readyLB'] = "-"
runSpec['AllRuns']['peakLumi'] = 0.
runSpec['AllRuns']['period'] = "-"
runSpec['AllRuns']['signoff'] = "-"

if debug:
  print(grlDef)

################################################################
# Book Histograms for general plot with intolerable defects/veto
# Fill them with past period inefficiencies
hProfRun_IntolDefect = {}
hProfRun_Veto = {}

if (options['updateYearStats']):
  hProfPeriod_IntolDefect = {}
  hProfPeriod_Veto = {}
 
for idef in grlDef["intol"]+grlDef["intol_recov"]: #Intolerable defects only
  idefName = idef.split("__")[0] # Trick needed to get the defect name for the recoverable defect histogram
  # Histogram of defect ineffiency per run
  hProfRun_IntolDefect[idef] = MakeTProfile("hProfRun_Defect_%s"%(idef),"%s"%(defectVeto["description"][idefName]),"Lost luminosity (%)", -0.5,+0.5+len(runlist['toprocess']),len(runlist['toprocess'])+1,defectVeto["color"][idefName])
  SetXLabel(hProfRun_IntolDefect[idef],runlist['toprocess'])
  hProfRun_IntolDefect[idef].GetXaxis().SetBinLabel(len(runlist['toprocess'])+1,"All")
  # If an update of year stats is requested, define empty histograms, retrieve the already known periods and fill the new histos
  if (options['updateYearStats']):
    periodListYear = [] # This is the period list used for the TProfile
    profPeriodName = "hProfPeriod_IntolDefect_%s"%(idef)
    if (not options['resetYearStats']): # Retrieve the already known period
      file = TFile(yearStatsArchiveFilename)
      hProfPeriod_archive = file.Get("%s_archive"%(profPeriodName))
      for iBin in range(1,hProfPeriod_archive.GetNbinsX()): # Retrieve name of past periods
        periodListYear.append(hProfPeriod_archive.GetXaxis().GetBinLabel(iBin))
      file.Close()
      
    if len(periodListYear) != 0 or len(periodListCurrent) != 0: # At least one period found in current or past runs, otherwise no way to plot year stats
      # Collect all periods (archived ones + new ones)
      periodListYear = sorted(periodListYear + newPeriodInYearStats) 
      periodNbYear = len(periodListYear) # Number of periods      
      # Create the empty year stats TProfile histograms for the updated period list
      hProfPeriod_IntolDefect[idef] = MakeTProfile(profPeriodName,"%s"%(defectVeto["description"][idefName]),"Lost luminosity (%)", -0.5,+0.5+periodNbYear,periodNbYear+1,defectVeto["color"][idefName])
      SetXLabel(hProfPeriod_IntolDefect[idef],periodListYear)
      hProfPeriod_IntolDefect[idef].GetXaxis().SetBinLabel(periodNbYear+1,"All") # In all bins, all runs
      # Fill them with the previous period data
      if (not options['resetYearStats']): 
        file = TFile(yearStatsArchiveFilename)
        hProfPeriod_archive = file.Get("%s_archive"%(profPeriodName))
        for iBin in range(1,periodNbYear+1): # Loop on all extended periods and check if already considered in past runs - Mandatory as periodListYear was sorted
          for iBin2 in range(1,hProfPeriod_archive.GetNbinsX()):
            if (hProfPeriod_IntolDefect[idef].GetXaxis().GetBinLabel(iBin) == hProfPeriod_archive.GetXaxis().GetBinLabel(iBin2)):
              hProfPeriod_IntolDefect[idef].Fill(iBin-1,hProfPeriod_archive.GetBinContent(iBin2),hProfPeriod_archive.GetBinEntries(iBin2))
        # And the last bin for all periods
        hProfPeriod_IntolDefect[idef].Fill(len(periodListYear),hProfPeriod_archive.GetBinContent(hProfPeriod_archive.GetNbinsX()),hProfPeriod_archive.GetBinEntries(hProfPeriod_archive.GetNbinsX()))
        file.Close()

# Book histograms for time window veto
for iVeto in veto["all"]:
  hProfRun_Veto[iVeto] = MakeTProfile("hProfRun_Veto_%s"%(iVeto),"","Inefficiency - Time veto (%)",-0.5,+0.5+len(runlist['toprocess']),len(runlist['toprocess'])+1,defectVeto["color"][iVeto])
  hProfRun_Veto[iVeto].SetMarkerColor(defectVeto["color"][iVeto])
  SetXLabel(hProfRun_Veto[iVeto],runlist['toprocess'])
  hProfRun_Veto[iVeto].GetXaxis().SetBinLabel(len(runlist['toprocess'])+1,"All")

  if (options['updateYearStats']):
    profPeriodName = "hProfPeriod_Veto_%s"%(iVeto)
    # Create the period TProfile - The extended period list is the same as for the defects
    hProfPeriod_Veto[iVeto] = MakeTProfile(profPeriodName,"","Inefficiency - Time veto (%%) %s"%(defectVeto["description"][iVeto]), -0.5,+0.5+len(periodListYear),len(periodListYear)+1,defectVeto["color"][iVeto])
    hProfPeriod_Veto[iVeto].SetMarkerColor(defectVeto["color"][iVeto])
    SetXLabel(hProfPeriod_Veto[iVeto],periodListYear)
    hProfPeriod_Veto[iVeto].GetXaxis().SetBinLabel(len(periodListYear)+1,"All")

    if (not options['resetYearStats']): # Fill with the previous period data
      file = TFile(yearStatsArchiveFilename)
      hProfPeriod_archive = file.Get("%s_archive"%(profPeriodName))
      for iBin in range(1,periodNbYear+1): # Loop on all extended periods and check if already considered in past runs - Mandatory as periodListYear was sorted
        for iBin2 in range(1,hProfPeriod_archive.GetNbinsX()):
          if (hProfPeriod_Veto[iVeto].GetXaxis().GetBinLabel(iBin) == hProfPeriod_archive.GetXaxis().GetBinLabel(iBin2)):
            hProfPeriod_Veto[iVeto].Fill(iBin-1,hProfPeriod_archive.GetBinContent(iBin2),hProfPeriod_archive.GetBinEntries(iBin2))
      # And the last bin for all periods
      hProfPeriod_Veto[iVeto].Fill(len(periodListYear),hProfPeriod_archive.GetBinContent(hProfPeriod_archive.GetNbinsX()),hProfPeriod_archive.GetBinEntries(hProfPeriod_archive.GetNbinsX()))
      file.Close()

h1Run_IntLuminosity  = MakeTH1("h1_IntLumi","Run","Integrated luminosity", -0.5,+0.5+len(runlist['toprocess']),len(runlist['toprocess'])+1,kBlack)
SetXLabel(h1Run_IntLuminosity,runlist['toprocess'])
h1Run_IntLuminosity.GetXaxis().SetBinLabel(len(runlist['toprocess'])+1,"All")

if debug:
  print("1",grlDef)

### TO BE MODIFIED WHEN TH1 IS SAVED IN TPROFILE.ROOT. Can be filled in a more logical way
if (options['updateYearStats'] and periodNbYear>0): # If update is required, it is now sure that some periods exist. Create a TH1 to store the integrated luminosity
  h1Per_IntLumi = MakeTH1("periodLuminosity","Period","Luminosity(pb-1)",-0.5,+0.5+periodNbYear,periodNbYear+1,1)
  h1Per_IntLumi.SetTitle("")
  SetXLabel(h1Per_IntLumi,periodListYear)
  h1Per_IntLumi.GetXaxis().SetBinLabel(periodNbYear+1,"All")

  for iBin in range(1,hProfPeriod_IntolDefect[idef].GetNbinsX()+1): # No matter what is idef, we just need the number of entries per bin to get the luminosity of past periods
    h1Per_IntLumi.Fill(iBin-1,hProfPeriod_IntolDefect[idef].GetBinEntries(iBin)/1e6)
#  h1Per_IntLumi.Print("all")

#######################################################################################
# Book histograms for luminosity profile and veto rejection as a function of luminosity
if options['vetoLumiEvolution']:
  h1_vetoInstLumiEvol = {}
  h1_vetoInstLumiEvol['NoVeto'] = MakeTH1("LuminosityProfile","Instantaneous luminosity (10^{33} cm^{-2}s^{-1})","Time length (s)",4,19,60,1)
  for iVeto in veto["all"]:
    h1_vetoInstLumiEvol[iVeto] = MakeTH1("LuminosityVeto_%s"%(iVeto),"Instantaneous luminosity (10^{33} cm^{-2}s^{-1}) - Runs %d-%d"%(runlist['toprocess'][0],runlist['toprocess'][-1]),"#splitline{Inefficiency Time Veto}{(%s) [percent]}"%(defectVeto["description"][iVeto]),4,19,60,1)


#######################################################################################
# Initialize total luminosity affected per defect for all runs
runSpec['AllRuns']['Lumi'] = 0. # Total luminosity
runSpec['AllRuns']['lumiDefects'] = 0.
runSpec['AllRuns']['ineffDefects'] = 0.
for idef in (grlDef["tol"]+grlDef["intol"]+grlDef["intol_recov"]):
  runSpec['AllRuns']['lumiDefect_%s'%(idef)] = 0.
  runSpec['AllRuns']['ineffDefect_%s'%(idef)] = 0.

runSpec['AllRuns']['lumiVetos'] = 0. # Total luminosity rejected by all time vetos
runSpec['AllRuns']['ineffVetos'] = 0.
for iVeto in veto["all"]:
  runSpec['AllRuns']['lumiVeto_%s'%iVeto] = 0. # Total luminosity rejected by each time veto

if (len(runSpec) == 1):
  print("I did not find any run in runList.")
  print("Please check the run range/options")

#######################################################################################
#### Main loop over selected runs
for irun,runNb in enumerate(runlist['toprocess']):
  print("=================================================================") 
  print("=============================Run %d (%d/%d)======================"%(runNb,irun+1,len(runlist['toprocess']))) 
  # Init variables - List (indexed by partition) of tables of lumi blocks affected by defects
  lbAffected = {}
  for idef in grlDef["part"]+grlDef["partIntol_recov"]: # All partition defects
    lbAffected[idef] = {}
    lbAffected[idef]["AllPartitions"] = []
  for idef in grlDef["glob"]+grlDef["globIntol_recov"]: # All global defects
    lbAffected[idef] = [] # Global defect - Simple list and not dictionnary
  
  lbAffected['IntolDefect'] = [] # List of LBs affected with intolerable defect independant of the defect/partition
  lbAffected['irrecov'] = [] # List of LBs affected with intolerable irrecoverable defect independant of the defect/partition
  

  boolExactVetoComput_run = (options['exactVetoComput']) and (runNb not in runlist['%s/roughVeto'%(options['system'])])

  # Start retrieving the general characteristics of the run
  # Get run infos
  # Luminosity blocks UTC timestamps
  v_lbTimeSt = GetLBTimeStamps(runNb)
  runSpec[runNb]['Start'] = time.strftime("%a, %d %b %H:%M",time.localtime(v_lbTimeSt[1][0]))
  runSpec[runNb]['Stop'] = time.strftime("%a, %d %b %H:%M",time.localtime(v_lbTimeSt[len(v_lbTimeSt)][1]))
  runSpec[runNb]['nBunches'] = GetNumberOfCollidingBunches(runNb)
  # Number of luminosity blocks
  runSpec[runNb]['nLB'] = len(v_lbTimeSt)

  # Atlas Ready
  atlasready=GetReadyFlag(runNb)
  runSpec[runNb]['readyLB']=[]
  for lb in atlasready.keys():
    if atlasready[lb]>0: runSpec[runNb]['readyLB']+=[lb]
  runSpec[runNb]['nLBready'] = float(len(runSpec[runNb]['readyLB']))    

  thisRunPerLB = dict() # Contains various per LB run characteristics retrieved from COOL
  # COOL delivered luminosity
  if options['onlineLumiNorm']:
    thisRunPerLB["deliveredLumi"] = GetOnlineLumiFromCOOL(runNb,0)
  else:
    if runNb in runlist['grl']:
      thisRunPerLB["deliveredLumi"] = GetOfflineLumiFromCOOL(runNb,0,yearTagProperties["offlineLumiTag"]["grl"])
    else:
      thisRunPerLB["deliveredLumi"] = GetOfflineLumiFromCOOL(runNb,0,yearTagProperties["offlineLumiTag"]["preliminary"])
  # Look for peak lumi
  runSpec[runNb]['peakLumi'] = 0.
  for lb in thisRunPerLB['deliveredLumi'].keys(): 
    if thisRunPerLB['deliveredLumi'][lb] > runSpec[runNb]['peakLumi']: runSpec[runNb]['peakLumi']=thisRunPerLB['deliveredLumi'][lb] 
    if thisRunPerLB['deliveredLumi'][lb] > runSpec['AllRuns']['peakLumi']: runSpec['AllRuns']['peakLumi']=thisRunPerLB['deliveredLumi'][lb] 
  # Retrieve the precise LB duration (more precise than GetLBDuration(runNb)) and liveFraction (relevant if recorded lumi normalisation).
  lumiacct=fetch_iovs('COOLOFL_TRIGGER::/TRIGGER/OFLLUMI/LumiAccounting', tag='OflLumiAcct-001', since=v_lbTimeSt[1][0]*1000000000, until=v_lbTimeSt[len(v_lbTimeSt)][1]*1000000000) 
  #thisRunPerLB['liveFraction'] = dict()
  thisRunPerLB['duration'] = dict()
  for iLumiAcct in range(len(lumiacct)):
    #thisRunPerLB['liveFraction'][lumiacct[iLumiAcct].LumiBlock] = lumiacct[iLumiAcct].LiveFraction
    if options['recordedLumiNorm']: # The LB duration is corrected by the live fraction 
      thisRunPerLB['duration'][lumiacct[iLumiAcct].LumiBlock] = lumiacct[iLumiAcct].LBTime*lumiacct[iLumiAcct].LiveFraction
    else:
      thisRunPerLB['duration'][lumiacct[iLumiAcct].LumiBlock] = lumiacct[iLumiAcct].LBTime
  # Store the luminosity used for the efficiency normalisations
  for lb in range(1,runSpec[runNb]['nLB']+1): # Loop on all LB - Protection to set a zero luminosity if not available
    if lb in runSpec[runNb]['readyLB']:
      if lb not in thisRunPerLB["deliveredLumi"].keys():
        thisRunPerLB["deliveredLumi"][lb] = 0.
        errorMsg = "Missing lumi for Run %d - LB %d\n"%(runNb,lb)
        print(errorMsg)
        errorLogFile.write(errorMsg)
      if lb not in thisRunPerLB["duration"].keys():
        thisRunPerLB["duration"][lb] = 0.
        errorMsg = "Missing duration/LiveFraction for Run %d - LB %d\n"%(runNb,lb)
        print(errorMsg)
        errorLogFile.write(errorMsg)
    else:
      if lb not in thisRunPerLB["deliveredLumi"].keys():
        thisRunPerLB["deliveredLumi"][lb] = 0.
      if lb not in thisRunPerLB["deliveredLumi"].keys():
        thisRunPerLB["duration"][lb] = 0.

  if options['vetoLumiEvolution']:
    for lb in runSpec[runNb]['readyLB']: # Fill the luminosity profile
      h1_vetoInstLumiEvol['NoVeto'].Fill(thisRunPerLB["deliveredLumi"][lb]/1e3,v_lbTimeSt[lb][1]-v_lbTimeSt[lb][0])
  
  # Finished retrieving the general characteristics of the run
  
  # Get defects
  defectDatabase = DQDefects.DefectsDB(tag=options['defectTag'])
  system_defects = []
  for iPrefix in grlDef["prefix"]:
    system_defects += [d for d in (defectDatabase.defect_names | defectDatabase.virtual_defect_names) if (d.startswith(iPrefix))]
  retrievedDefects = defectDatabase.retrieve((runNb, 1), (runNb+1, 0), system_defects)   
    
  runSpec[runNb]['exprSignedOff'] = True
  runSpec[runNb]['bulkSignedOff'] = True
  runSpec[runNb]['signoff'] = 'FINAL OK'
  # Loop over all defects and store in list (1 per partition and type) the affected LB
  # Consider only LB in runSpec[runNb]["readyLB"]
  for iRetrievedDefects in retrievedDefects:
    if debug:
      print(iRetrievedDefects)
    # keep track of runs with missing sign-off - Store the earliest stage of the sign off procedure
    for iSignOff in signOff["EXPR."]:
      if iRetrievedDefects.channel == iSignOff:
        runSpec[runNb]['signoff'] = "EXPR."
    if "EXPR." not in runSpec[runNb]['signoff']:
      for iSignOff in signOff["BULK"]:
        if iRetrievedDefects.channel == iSignOff:
          runSpec[runNb]['signoff'] = "BULK"
      if "BULK" not in runSpec[runNb]['signoff']:
        for iSignOff in signOff["FINAL"]:
          if iRetrievedDefects.channel == iSignOff:
            runSpec[runNb]['signoff'] = "DONE"        

    if runSpec[runNb]['signoff'] != 'FINAL OK': # Update year stats only after final signoff
      runSpec[runNb]['newInYearStats'] = False

    # Checks if the defect corresponds to a defect in the system list
    defectFound = ""
    if (iRetrievedDefects.channel.startswith("PIXEL")): # [PIXEL]_[PARTITION]_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",2)
      if len(defectSplitted) > 2:
        partAffected=defectSplitted[1]
        defectFound=defectSplitted[2]      
    elif (iRetrievedDefects.channel.startswith("SCT")): # SCT_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",1)
      if len(defectSplitted) > 1:
        defectFound=defectSplitted[1]      
    elif (iRetrievedDefects.channel.startswith("TRT")): # TRT_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",1)
      if len(defectSplitted) > 1:
        defectFound=defectSplitted[1]      
    elif (iRetrievedDefects.channel.startswith("LAR")): # [LAR]_[PART]_[Name] or [LAR]_[Name] - No "_" in any [NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",2)
      if len(defectSplitted) == 2: # LAR_[NAME]
        defectFound=defectSplitted[1]
      elif len(defectSplitted) == 3: # LAR_[PART]_[NAME]
        partAffected=defectSplitted[1]
        defectFound=defectSplitted[2]
    elif (iRetrievedDefects.channel.startswith("CALO_ONLINEDB")):# CALO_ONLINEDB_[NAME] (only for CALO_ONLINEDB_LOWMU) - Accounted to LAr
      defectSplitted = iRetrievedDefects.channel.split("_",2)
      if len(defectSplitted) > 2:
        defectFound=defectSplitted[2]
    elif (iRetrievedDefects.channel.startswith("TILE")): # [TILE]_[PART]_[Name]
      defectSplitted = iRetrievedDefects.channel.split("_",2)
      if len(defectSplitted) > 2:
        partAffected=defectSplitted[1]
        defectFound=defectSplitted[2]
    elif (iRetrievedDefects.channel.startswith("MS")): # MS_[SUBDETECTOR]_[PARTITION]_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",3)
      if len(defectSplitted) > 3:
        systemAffected=defectSplitted[1]
        partAffected=defectSplitted[2]
        defectFound=defectSplitted[3]
    elif (iRetrievedDefects.channel.startswith("MCP")): # MCP_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",1) 
      if len(defectSplitted) > 1:
        defectFound=defectSplitted[1]
    elif (iRetrievedDefects.channel.startswith("ID")):# ID_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",1)
      if len(defectSplitted) > 1:
        defectFound=defectSplitted[1]
    elif (iRetrievedDefects.channel.startswith("JET")):# JET_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",1)
      if len(defectSplitted) > 1:
        defectFound=defectSplitted[1]
    elif (iRetrievedDefects.channel.startswith("MET")):# MET_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",1)
      if len(defectSplitted) > 1:
        defectFound=defectSplitted[1]
    elif (iRetrievedDefects.channel.startswith("EGAMMA")):# EGAMMA_[NAME]_[PART] or EGAMMA_[NAME]
      if ("BARREL" in iRetrievedDefects.channel or "ENDCAP" in iRetrievedDefects.channel or "FORWARD" in iRetrievedDefects.channel): #EGAMMA_[NAME]_[PART]
        defectSplitted = iRetrievedDefects.channel.split("_",2)
        if len(defectSplitted) > 2:
          defectFound=defectSplitted[1] 
          partAffected=defectSplitted[2]
      else:
        defectSplitted = iRetrievedDefects.channel.split("_",1)
        if len(defectSplitted) > 1:
          defectFound=defectSplitted[1] 
    elif (iRetrievedDefects.channel.startswith("TAU")): # TAU_[PART]_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",2)
      if len(defectSplitted) > 2:
        partAffected=defectSplitted[1]
        defectFound=defectSplitted[2]
    elif (iRetrievedDefects.channel.startswith("CALO")): # CALO_[PART]_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",2)
      if len(defectSplitted) > 2:
        partAffected=defectSplitted[1]
        defectFound=defectSplitted[2]
    elif (iRetrievedDefects.channel.startswith("BTAG")):# BTAG_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",1)
      if len(defectSplitted) > 1:
        defectFound=defectSplitted[1]
    elif (iRetrievedDefects.channel.startswith("TRIG")): # TRIG_L1_[NAME] TRIG_HLT_[NAME]
      defectSplitted = iRetrievedDefects.channel.split("_",2)
      if len(defectSplitted) > 2:
        defectFound=defectSplitted[2]

    # Now stored the affected - Duplication are now treated here and no longer later
    if defectFound in grlDef["part"]:
      for lb in range(iRetrievedDefects.since.lumi,iRetrievedDefects.until.lumi):
        if((lb in runSpec[runNb]['readyLB']) or runSpec[runNb]['nLBready']==0):# The LB is with ATLAS ready

          if partAffected not in lbAffected[defectFound]: # Store the affected partitions
            lbAffected[defectFound][partAffected]=[]
          lbAffected[defectFound][partAffected].append(lb)

          if (defectFound in grlDef["partTol"]): # For tolerable defect, do not removate duplicated betwen defects
            lbAffected[defectFound]["AllPartitions"].append(lb)
          if (defectFound in grlDef["partIntol"]):# Update the LBs affected by an intolerable defect
            if (lb not in lbAffected['IntolDefect']): 
              lbAffected[defectFound]["AllPartitions"].append(lb)
              lbAffected['IntolDefect'].append(lb)
            if (not iRetrievedDefects.recoverable and lb not in lbAffected['irrecov']): # Update the LBs affected by an intolerable irrecoverable defect
              lbAffected['irrecov'].append(lb)

    if defectFound in grlDef["glob"]:
      for lb in range(iRetrievedDefects.since.lumi,iRetrievedDefects.until.lumi):
        if((lb in runSpec[runNb]["readyLB"]) or runSpec[runNb]['nLBready']==0):

          if (defectFound in grlDef["globTol"]): # For tolerable defect, do not removate duplicated betwen defects
            lbAffected[defectFound].append(lb)
          if (defectFound in grlDef["globIntol"]):# Update the LBs affected by an intolerable irrecoverable defect
            if (lb not in lbAffected['IntolDefect']):
              lbAffected[defectFound].append(lb)
              lbAffected['IntolDefect'].append(lb)
            if (not iRetrievedDefects.recoverable and lb not in lbAffected['irrecov']): 
              lbAffected['irrecov'].append(lb)

  # By default, an unsignedoff runs is kept in the final table/plots such that the LADIeS can use the plot. On 
  # request, they can be also ignored.
  # NB: in any way, a non signed off run is never considered in year stats
  if options['skipUnsignedOff'] and runSpec[runNb]['signoff'] != 'FINAL OK':
    print("Run %d is not yet signed off. Skipping it..."%runNb)
    runSpec.pop(runNb)
    continue

### Now remove duplication between partition/defects (intolerable only!)
### lbAffected[idef]["AllPartition"] are referred to all partition for this run
### WARNING : the duplication between global and partition defect is not properly treated. Negligible impact though...
##  lbAffectedPrevDef = [] # Used to remove duplicate between defects
##  for idef in grlDef["partIntol"]:
##    lbAffected[idef]["AllPartitions"]=[]
##    for ipart in partitions["list"]:
##      if lbAffected[idef].has_key(ipart):
##        for lumiBlock in lbAffected[idef][ipart]:
##          if (lumiBlock not in lbAffected[idef]["AllPartitions"]) and (lumiBlock not in lbAffectedPrevDef):
##            lbAffected[idef]["AllPartitions"].append(lumiBlock)
##            lbAffectedPrevDef.append(lumiBlock)
##
##  for idef in grlDef["partTol"]: # In tolerable defect, do not remove overlap with previous defects
##    lbAffected[idef]["AllPartitions"]=[]
##    for ipart in partitions["list"]:
##      if lbAffected[idef].has_key(ipart):
##        for lumiBlock in lbAffected[idef][ipart]:
##          if (lumiBlock not in lbAffected[idef]["AllPartitions"]):
##            lbAffected[idef]["AllPartitions"].append(lumiBlock)
  
  # Now treat recoverability - Storing all irrecoverable lb per defect (not partition wise as useless)
  for idef in grlDef["partIntol"]:
    lbAffected["%s__recov"%idef]["AllPartitions"]=[]
    for ilb in lbAffected[idef]["AllPartitions"]:
      if ilb not in lbAffected['irrecov']:
        lbAffected["%s__recov"%idef]["AllPartitions"].append(ilb)
  for idef in grlDef["globIntol"]:
    for ilb in lbAffected[idef]:
      if ilb not in lbAffected['irrecov']:
        lbAffected["%s__recov"%idef].append(ilb)
        
  # Extract inst. lumi to compute affected luminosity
  for idef in (grlDef["tol"]+grlDef["intol"]+grlDef["intol_recov"]):
    runSpec[runNb]["lumiDefect_%s"%(idef)] = 0.
    runSpec[runNb]["ineffDefect_%s"%(idef)] = 0.
  runSpec[runNb]['Lumi'] = 0
  if runSpec[runNb]['nLBready']>0:
    lbsToConsider=runSpec[runNb]["readyLB"]
  else:
    lbsToConsider=list(range(1,runSpec[runNb]['nLB']))

  for lb in lbsToConsider:
    runSpec[runNb]['Lumi'] = runSpec[runNb]['Lumi'] +thisRunPerLB["deliveredLumi"][lb]*thisRunPerLB['duration'][lb]
    runSpec['AllRuns']['Lumi'] = runSpec['AllRuns']['Lumi'] +thisRunPerLB["deliveredLumi"][lb]*thisRunPerLB['duration'][lb]
    for idef in grlDef["part"]+grlDef["partIntol_recov"]:
      if lb in lbAffected[idef]["AllPartitions"]:
        runSpec[runNb]["lumiDefect_%s"%(idef)] = runSpec[runNb]["lumiDefect_%s"%(idef)] + thisRunPerLB["deliveredLumi"][lb]*thisRunPerLB['duration'][lb]
        runSpec["AllRuns"]["lumiDefect_%s"%(idef)] = runSpec["AllRuns"]["lumiDefect_%s"%(idef)] + thisRunPerLB["deliveredLumi"][lb]*thisRunPerLB['duration'][lb]
      if (runSpec[runNb]["Lumi"] != 0.):
        runSpec[runNb]["ineffDefect_%s"%(idef)] = runSpec[runNb]["lumiDefect_%s"%(idef)]/runSpec[runNb]["Lumi"]*100.

    for idef in grlDef["glob"]+grlDef["globIntol_recov"]: # Duplicate with partdefect to be fixed
      if lb in lbAffected[idef]:
        runSpec[runNb]["lumiDefect_%s"%(idef)] = runSpec[runNb]["lumiDefect_%s"%(idef)] + thisRunPerLB["deliveredLumi"][lb]*thisRunPerLB['duration'][lb]
        runSpec["AllRuns"]["lumiDefect_%s"%(idef)] = runSpec["AllRuns"]["lumiDefect_%s"%(idef)] + thisRunPerLB["deliveredLumi"][lb]*thisRunPerLB['duration'][lb]
      if (runSpec[runNb]["Lumi"] != 0.):
        runSpec[runNb]["ineffDefect_%s"%(idef)] = runSpec[runNb]["lumiDefect_%s"%(idef)]/runSpec[runNb]["Lumi"]*100.
     
  if runSpec[runNb]['Lumi']==0: runSpec[runNb]['Lumi']=1e-50
  # Fill LostLumiMergedPart histos for all defects
  runSpec[runNb]["lumiDefect"] = 0 # This is the GRL loss computed only with intolerable defects
  for idef in grlDef["intol"]+grlDef["intol_recov"]:
    # Now computes defect inefficiencies
    if idef in grlDef["intol"]:
      runSpec[runNb]["lumiDefect"] = runSpec[runNb]["lumiDefect"] + runSpec[runNb]["lumiDefect_%s"%(idef)]

    hProfRun_IntolDefect[idef].Fill(irun,runSpec[runNb]["ineffDefect_%s"%(idef)])
    hProfRun_IntolDefect[idef].Fill(len(runlist['toprocess']),runSpec[runNb]["ineffDefect_%s"%(idef)],runSpec[runNb]['Lumi']) # Fill last bins (all runs) - Reminder : this is a profile !
    if (options['updateYearStats'] and runSpec[runNb]['newInYearStats']):
      hProfPeriod_IntolDefect[idef].Fill(periodListYear.index(runSpec[runNb]["period"]),runSpec[runNb]["ineffDefect_%s"%(idef)],runSpec[runNb]['Lumi'])
      hProfPeriod_IntolDefect[idef].Fill(len(periodListYear),runSpec[runNb]["ineffDefect_%s"%(idef)],runSpec[runNb]['Lumi'])

  runSpec[runNb]['ineffDefects']=100*runSpec[runNb]["lumiDefect"]/runSpec[runNb]['Lumi']

  h1Run_IntLuminosity.Fill(irun,runSpec[runNb]['Lumi']/1e6)
  h1Run_IntLuminosity.Fill(len(runlist['toprocess']),runSpec[runNb]['Lumi']/1e6)

  if (options['updateYearStats'] and runSpec[runNb]['newInYearStats']):
    h1Per_IntLumi.Fill(periodListYear.index(runSpec[runNb]["period"]),runSpec[runNb]['Lumi']/1e6)
    h1Per_IntLumi.Fill(h1Per_IntLumi.GetNbinsX()-1,runSpec[runNb]['Lumi']/1e6) # Cumulated for the year    

  # Now starts veto inefficiency 
  # Retrieve the length of time period vetoed
  db2="COOLOFL_LAR/CONDBR2"
  folderName="/LAR/BadChannelsOfl/EventVeto"
  if (not options['noVeto']):
    if (boolExactVetoComput_run):
      totalVeto = showEventVeto.showEventVetoFolder(db2,folderName,options['vetoTag'],runNb,runNb,0) 
    else:
      print("WARNING: you use the rough event veto loss. To be used only if default is too slow...")
      totalVeto = showEventVetoNoLumi.showEventVetoFolder(db2,folderName,options['vetoTag'],runNb,runNb,0) 
  else:
    totalVeto = None
  #print "totalVeto,totalVeto/1.e9:",totalVeto,[y/1.e9 for y in totalVeto]

  runSpec[runNb]['lumiVetos'] = 0.
  runSpec[runNb]['ineffVetos'] = 0.

  for iVeto in veto["all"]:
    runSpec[runNb]["lumiVeto_%s"%(iVeto)] = 0.
    runSpec[runNb]["ineffVeto_%s"%(iVeto)] = 0.

  if (totalVeto is not None):    
    if (boolExactVetoComput_run):# Computation of veto rejection weighting by inst. lumi and ignoring LB already in intolerable defect list
      for iVeto in veto["all"]:
        runSpec[runNb]["lumiVeto_%s"%iVeto] = 0.
        for iVetoedLB in range(len(totalVeto[veto["COOL"][iVeto]])): # Loop on all veto periods
          lb0 = findLB(v_lbTimeSt,totalVeto[veto["COOL"][iVeto]][iVetoedLB][0]/1e9) # Start of veto period
          lb1 = findLB(v_lbTimeSt,totalVeto[veto["COOL"][iVeto]][iVetoedLB][0]/1e9) # End of veto period
          if options['vetoLumiEvolution']:
            h1_vetoInstLumiEvol[iVeto].Fill(thisRunPerLB["deliveredLumi"][lb0]/1e3,(totalVeto[veto["COOL"][iVeto]][iVetoedLB][1]-totalVeto[veto["COOL"][iVeto]][iVetoedLB][0])/1e9)
          if (lb0 not in lbAffected['IntolDefect'] and lb1 not in lbAffected['IntolDefect'] and (lb0 in lbsToConsider or lb1 in lbsToConsider)): # If end/start not in lb with intol defect, add rejection period - Incorrect if > 1 LBs
            runSpec[runNb]["lumiVeto_%s"%iVeto] = runSpec[runNb]["lumiVeto_%s"%iVeto] + thisRunPerLB["deliveredLumi"][lb0]*((totalVeto[veto["COOL"][iVeto]][iVetoedLB][1]-totalVeto[veto["COOL"][iVeto]][iVetoedLB][0])/1e9)
    else:
      for iVeto in veto["all"]:
        if len(runSpec[runNb]["readyLB"]) != 0.:
          runSpec[runNb]["lumiVeto_%s"%(iVeto)] = totalVeto[iVeto][1] # WARNING : the veto inefficiency is computed from time (and not lumi). To be normalized by time
          
  for iVeto in veto["all"]:
    if boolExactVetoComput_run:
      runSpec[runNb]["ineffVeto_%s"%(iVeto)] = runSpec[runNb]["lumiVeto_%s"%(iVeto)]/runSpec[runNb]['Lumi']*100.
    else:# The veto inefficiency is computed from time (and not lumi). The ineff is normalized by time
      runSpec[runNb]["ineffVeto_%s"%(iVeto)] = runSpec[runNb]["lumiVeto_%s"%(iVeto)]/(len(runSpec[runNb]["readyLB"])*60*1e9)*100.
    runSpec[runNb]['ineffVetos'] = runSpec[runNb]['ineffVetos'] + runSpec[runNb]["ineffVeto_%s"%(iVeto)]
    runSpec[runNb]['lumiVetos'] = runSpec[runNb]['lumiVetos'] + runSpec[runNb]["lumiVeto_%s"%(iVeto)]

    hProfRun_Veto[iVeto].Fill(irun,runSpec[runNb]["ineffVeto_%s"%(iVeto)])
    hProfRun_Veto[iVeto].Fill(len(runlist['toprocess']),runSpec[runNb]["ineffVeto_%s"%(iVeto)],runSpec[runNb]['Lumi']) # Fill last bins (all runs) - Reminder : this is a profile !

    if (options['updateYearStats'] and runSpec[runNb]['newInYearStats']):
      hProfPeriod_Veto[iVeto].Fill(periodListYear.index(runSpec[runNb]["period"]),runSpec[runNb]["ineffVeto_%s"%(iVeto)],runSpec[runNb]['Lumi'])
      hProfPeriod_Veto[iVeto].Fill(len(periodListYear),runSpec[runNb]["ineffVeto_%s"%(iVeto)],runSpec[runNb]['Lumi']) # Fill last bins (all periods)

    runSpec['AllRuns']['lumiVeto_%s'%iVeto] = runSpec['AllRuns']['lumiVeto_%s'%iVeto] + runSpec[runNb]["lumiVeto_%s"%iVeto]
    runSpec['AllRuns']['lumiVetos'] = runSpec['AllRuns']['lumiVetos'] + runSpec[runNb]["lumiVeto_%s"%(iVeto)]

  singleRunReport(runNb,runSpec[runNb],lbAffected,options['yearStatsDir'],grlDef,veto,boolExactVetoComput_run)
# End of loop on runs

if options['vetoLumiEvolution']:
  for iVeto in veto["all"]:
    h1_vetoInstLumiEvol[iVeto].Divide(h1_vetoInstLumiEvol[iVeto],h1_vetoInstLumiEvol['NoVeto'],100.,1.)
    
######################### Treatment when a run range was considered (weekly report)
if (len(runSpec)>2 and runSpec['AllRuns']['Lumi']!=0):
  # Compute inefficiencies for the whole period
  
  # Defect inefficencies first
  for iDef in grlDef["intol"]+grlDef["intol_recov"]:
    runSpec['AllRuns']['ineffDefect_%s'%iDef] = hProfRun_IntolDefect[iDef].GetBinContent(len(runlist['toprocess'])+1)
    if iDef not in grlDef["intol_recov"]:
      runSpec['AllRuns']['ineffDefects'] = runSpec['AllRuns']['ineffDefects'] + hProfRun_IntolDefect[iDef].GetBinContent(len(runlist['toprocess'])+1)

#    runSpec['AllRuns']['lumiDefects'] = runSpec['AllRuns']['lumiDefects'] + runSpec['AllRuns']['lumiDefect_%s'%(idef)]
#    runSpec['AllRuns']['ineffDefect_%s'%(idef)] = runSpec['AllRuns']['lumiDefect_%s'%(idef)]/runSpec['AllRuns']['Lumi']*100.
#  runSpec['AllRuns']['ineffDefects'] = runSpec['AllRuns']['lumiDefects']/runSpec['AllRuns']['Lumi']*100.
  
  for iVeto in veto["all"]:
    runSpec['AllRuns']['ineffVeto_%s'%iVeto] = hProfRun_Veto[iVeto].GetBinContent(len(runlist['toprocess'])+1)
    runSpec['AllRuns']['ineffVetos'] = runSpec['AllRuns']['ineffVetos'] + hProfRun_Veto[iVeto].GetBinContent(len(runlist['toprocess'])+1)
  
  # Prepare the summary tables
  lineNb = {}
  column = {}
  lineNb = {}
  c1 = {}
  
  canvasIndex = 0
  newCanvas = True
  for runNb in runlist['toprocess']+["AllRuns"]:
    if runNb not in runSpec.keys():
      continue # Protection in case of the runs was not yet signed off and removed (with Unsignedoff option) from the list
          
    if newCanvas:
      #c1[canvasIndex] = TCanvas("runSummary_%s"%canvasIndex,"Run collection - %s"%canvasIndex,10,10,1000,600)
      # NewCanvas facility almost removed (50 runs cut) Size of the last TCanvas not properly computed
      c1[canvasIndex] = TCanvas("runSummary_%s"%canvasIndex,"Run collection - %s"%canvasIndex,10,10,1000,(len(runlist['toprocess'])+1)*22)
      column[canvasIndex] = []
      lineNb[canvasIndex] = 0
      labels_col = ["Run","Run start / stop","LB ready","Peak lumi","Int. lumi","GRL ineff.","Veto ineff.","Period","Status"]
      labels_xlow = [0.01,0.13,0.44,0.51,0.59,0.65,0.72,0.855,0.925,0.99]
      labels_xlow = [0.01,0.08,0.41,0.49,0.575,0.655,0.74,0.835,0.9,0.99]
      
      for i in range(len(labels_col)):
#        column[canvasIndex].append(TPaveText(labels_xlow[i],max(.99-0.08*len(runlist['toprocess']),0.01),labels_xlow[i+1],0.99))
        column[canvasIndex].append(TPaveText(labels_xlow[i],0.01,labels_xlow[i+1],0.99))
        column[canvasIndex][i].AddText(labels_col[i])
        if (i%2 == 0):
          column[canvasIndex][i].SetFillColor(kOrange-3)
        else:
          column[canvasIndex][i].SetFillColor(kGreen+2)
      newCanvas = False
    if runNb == "AllRuns":
      column[canvasIndex][0].AddText("ALL")
    else:
      column[canvasIndex][0].AddText("%d"%(runNb))
    column[canvasIndex][1].AddText("%s / %s"%(runSpec[runNb]['Start'],runSpec[runNb]['Stop']))
    column[canvasIndex][2].AddText("%s"%(listify(runSpec[runNb]["readyLB"])))
    column[canvasIndex][3].AddText("%.1e"%(runSpec[runNb]['peakLumi']*1e30))
    column[canvasIndex][4].AddText("%s"%(strLumi(runSpec[runNb]['Lumi'])))
    column[canvasIndex][5].AddText("%.2f %%"%(runSpec[runNb]['ineffDefects']))
    column[canvasIndex][6].AddText("%.2f %%"%(runSpec[runNb]['ineffVetos']))
    column[canvasIndex][7].AddText("%s"%(runSpec[runNb]["period"]))
    column[canvasIndex][8].AddText("%10s"%(runSpec[runNb]["signoff"]))
    lineNb[canvasIndex] += 1
    if (lineNb[canvasIndex]==50 or runNb == "AllRuns"):
      for i in range(len(column[canvasIndex])):
        if i == 1:
          column[canvasIndex][i].AddText("Completed at %s"%(time.strftime("%H:%M (%d %b)", time.localtime())))
        else:
          column[canvasIndex][i].AddText("")
        column[canvasIndex][i].Draw()
      c1[canvasIndex].SetWindowSize(1000,lineNb[canvasIndex]*40)
      c1[canvasIndex].Update()
      newCanvas = True
      canvasIndex += 1
      
    if runSpec[runNb]["signoff"] != "FINAL OK" and runNb != "AllRuns":
      print("Run %d not fully signed off -> no year stats update. Current status: %s"%(runNb,runSpec[runNb]["signoff"]))

  if options['savePlots']:
    for iCanvas in range(len(c1)):
      c1[iCanvas].SaveAs("%s/Weekly/summary-%d.png"%(options['yearStatsDir'],iCanvas))


canvasResults = {}
legendResults = {}
stackResults = {}

### Show Plots only for considered runs
gStyle.SetOptStat(0)
if options['plotResults']:
  gStyle.SetHistMinimumZero()

  plotStack("defects--Run--%s"%(args.parser_tag),hProfRun_IntolDefect,grlDef["intol"],defectVeto["description"],h1Run_IntLuminosity,False,stackResults,canvasResults,legendResults)
  if (len(veto["all"])):
    plotStack("veto--Run--%s"%(args.parser_tag),hProfRun_Veto,veto["all"],defectVeto["description"],h1Run_IntLuminosity,False,stackResults,canvasResults,legendResults)

  if options['vetoLumiEvolution']:
    for iVeto in veto["all"]:
      canvasResults["%s_veto_evol"%(iVeto)] = TCanvas("%s_veto_evol"%(iVeto),"%s inefficiency (%s time veto) vs Inst.Lumi."%(options["system"],defectVeto["description"][iVeto]), 200, 10, 1000, 500)
      canvasResults["%s_veto_evol"%(iVeto)].SetGridy(1)
      h1_vetoInstLumiEvol[iVeto].Draw()

  if options['savePlots']:
    for iCanvas in canvasResults.keys():
      canvasResults[iCanvas].SaveAs("%s/Weekly/%s.png"%(options['yearStatsDir'],iCanvas))
    
# End of plots
# Save the histograms when requested. NB:This is different from yearStats update
if (options['saveHistos']):
  filename = 'Files/weeklyHisto-%s-%s.root'%(startrun, endrun)
  f = TFile(filename,"recreate")
  for idef in grlDef["intol"]:
    hProfRun_IntolDefect[idef].Write()
#  h_stack_ineff.Write()
  if options['vetoLumiEvolution']:
    h1_vetoInstLumiEvol["NoVeto"].Write()
  for iVeto in veto["all"]:
    hProfRun_Veto[iVeto].Write()
    if options['vetoLumiEvolution']:
      h1_vetoInstLumiEvol[iVeto].Write()
  f.Close()
  print("Histos saved in %s"%(filename))

# yearStats update
# If new runs were added to period plots, save them
if (options['updateYearStats'] and bool_newRunsInYearStats):
  print("WARNING: I am going to update the %s stats with the following runs:"%(options['year']))
  print("NB: only runs fully signed off are considered")
  for irun in runSpec.keys():
    if (irun != "AllRuns"):
      if runSpec[irun]['newInYearStats']:
        print(irun)
        
  if (options['batchMode']): # In batch mode, no confirmation requested
    confirm = "y"
  else:
    confirm = input("Are you sure ([y]/n)?: ")
    
  if ("n" not in confirm):
    f = TFile(yearStatsArchiveFilename,"recreate")
    for idef in grlDef["intol"] + grlDef["intol_recov"]:#Intolerable defects only
      hProfPeriod_IntolDefect[idef].SetName("%s_archive"%(hProfPeriod_IntolDefect[idef].GetName()))
      hProfPeriod_IntolDefect[idef].Write()
    for iVeto in veto["all"]:
      hProfPeriod_Veto[iVeto].SetName("%s_archive"%(hProfPeriod_Veto[iVeto].GetName()))
      hProfPeriod_Veto[iVeto].Write()
    h1Per_IntLumi.SetName("h1Period_IntLuminosity_archive")
    h1Per_IntLumi.Write()
    f.Close()
    
    # Creating empty files for new period
    for iper in newPeriodInYearStats:
      periodFileName = "%s/runs-%s.dat"%(options['yearStatsDir'],iper)
      f = open(periodFileName,'w')
      f.close()
    
    # Adding all runs not treated in year stats
    fAll = open("%s/runs-ALL.dat"%options['yearStatsDir'],'a')
    for iper in periodListCurrent.keys(): # Loop on all periods founds
      periodFileName = "%s/runs-%s.dat"%(options['yearStatsDir'],iper)
      f = open(periodFileName,'a')
      for irun in periodListCurrent[iper]:
        if (irun in runSpec.keys() and runSpec[irun]['newInYearStats']): # Runs not yet considered in yearStats
          f.write("%d\n"%(irun))
          fAll.write("%d\n"%(irun))
      f.close()
    fAll.close()
    print("I have updated year stats")

# The update of the defect dat files is now decoupled from the yearStatsUpdate to allows to also monitor runs (special runs notably)
# that are not in the GRL.  
for irun in runSpec.keys():
  if runSpec[irun]['signoff'] == 'FINAL OK' and irun != "AllRuns":
    # NB : GRL information no longer stored here as this information is now stored in runlist['toprocess']
    # Backwards compatibility with DeMoScan to be checked
    irun_string = "%d (%.0f ub-1)"%(irun,runSpec[irun]['Lumi'])
    
    for idef in grlDef["intol"]+grlDef["tol"]:
      if (runSpec[irun]["lumiDefect_%s"%(idef)]>0.):
        defectFileName = "%s/loss-%s.dat"%(options['yearStatsDir'],idef)
        if idef in grlDef["intol"]:
          defString = "%s -> %.6f pb-1 (recov: %.6f pb-1)\n"%(irun_string,runSpec[irun]["lumiDefect_%s"%(idef)]/1e6,runSpec[irun]["lumiDefect_%s__recov"%(idef)]/1e6)
        else:
          defString = "%s -> %.6f pb-1\n"%(irun_string,runSpec[irun]["lumiDefect_%s"%(idef)]/1e6)
        toAdd = True
        if (os.path.exists(defectFileName)):# Check that the same defect was not already stored
          f2 = open(defectFileName,'r')
          if defString in f2.readlines(): 
            toAdd = False
          f2.close()
        if toAdd:
          f2 = open(defectFileName,'a')
          f2.write(defString)
          f2.close()

    if runSpec[irun]["ineffVetos"]>0. : # Loss due to veto. Update the veto dat files
      for iVeto in veto["all"]:
        if (runSpec[irun]["lumiVeto_%s"%(iVeto)]>0.):
          vetoFileName = "%s/loss-%sVETO.dat"%(options['yearStatsDir'],iVeto)
          vetoString = "%s -> %.6f pb-1 \n"%(irun_string,runSpec[irun]["lumiVeto_%s"%(iVeto)]/1e6)
          toAdd = True
          if (os.path.exists(vetoFileName)):# Check that the same veto was not already stored
            f2 = open(vetoFileName,'r')
            if vetoString in f2.readlines(): 
              toAdd = False
            f2.close()
          if toAdd:
            f2 = open(vetoFileName,'a')
            f2.write(vetoString)
            f2.close()

errorLogFile.close()

os.system("rm -f %s"%tokenName)
if not options['batchMode']:
  input("I am done. Type <return> to exit...")
