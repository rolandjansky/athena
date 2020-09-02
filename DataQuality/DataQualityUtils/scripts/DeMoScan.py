#! /usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Author : Benjamin Trocme (LPSC - Grenoble) - 2017
# Displays the run affected per defect type
# Perform run by run differences for difference tags
##################################################################

from __future__ import print_function
import os,sys  
from math import fabs
from re import match
from time import strftime,gmtime

from ROOT import TFile
from ROOT import TCanvas
from ROOT import kTeal
from ROOT import gStyle,gPad


sys.path.append("/afs/cern.ch/user/l/larmon/public/prod/Misc")
from LArMonCoolLib import GetReadyFlag
from DeMoLib import strLumi, initialize

from gb import MakeTH1,SetXLabel,MakeLegend

global debug
debug = False
#debug = True

########################################################################
########################################################################
# Main script

from argparse import RawTextHelpFormatter,ArgumentParser

from DQDefects import DefectsDB

parser = ArgumentParser(description='',formatter_class=RawTextHelpFormatter)
parser.add_argument('-y','--year',dest='parser_year',default = ["2018"],nargs='*',help='Year [Default: 2018]',action='store')
parser.add_argument('-t','--tag',dest='parser_tag',default = ["Tier0_2018"],nargs='*',help='Defect tag [Default: "Tier0_2018"]',action='store')
parser.add_argument('-s','--system',dest='parser_system',default="LAr",help='System: LAr, CaloCP [Default: "LAr"]',action='store')
parser.add_argument('-d','--directory',dest='parser_directory',default=".",help='Directory to display',action='store')
parser.add_argument('--run',type=int,dest='parser_run',help='Run or run range (relevant only for lossPerRun)',nargs='*',action='store')
parser.add_argument('--defect',type=str,dest='parser_defect',default="",help='Defect to consider (if not specified: all)',action='store')
parser.add_argument('--veto',type=str,dest='parser_veto',default="",help='Veto to consider (if not specified: all)',action='store')
parser.add_argument('--noRecovPlot',dest='parser_noRecovPlot',help='Do not plot the recoverable histograms',action='store_false')
parser.add_argument('--lossPerRun',dest='parser_plotLPR',help='Plot the loss run and per type',action='store_true')
parser.add_argument('--diffTwoTags',dest='parser_diff2tags',help='Compare run by run the data losses for two tags (same year necessary)',action='store_true')
parser.add_argument('--restrictTagRuns',dest='parser_restrictTagRuns',help='In diffTwotags, restrict comparison to runs considered in this tag',action='store')
parser.add_argument('--minLumi',type=float,dest='parser_minLumiLPR',default = 0.,help='Minimal lumi loss (in pb-1) to consider a run in year stats per defect',action='store')
parser.add_argument('--retrieveComments',dest='parser_retrieveComments',help='Retrieve comments from the defect DB',action='store_true')
parser.add_argument('--recapDefects',dest='parser_recapDefects',help='',action='store_true')
parser.add_argument('--savePage1',dest='parser_savePage1',help='Save yearStats results in ~larmon/public/prod/LADIeS/DeMoPlots',action='store_true')
parser.add_argument('--prepareReproc',dest='parser_reproc',help='Prepare a txt file to be updated for reprocessing signoff with recoverable defect',action='store_true')

args = parser.parse_args()

parser.print_help()

yearTagProperties = {}
partitions = {}
grlDef = {}
defectVeto = {}
veto = {}
signOff = {}
initialize(args.parser_system,yearTagProperties,partitions,grlDef,defectVeto,veto,signOff)

yearTagList = []
yearTagDir = {}
yearTagTag = {}

runGRL = {}

for iYear in args.parser_year:
  for iTag in args.parser_tag:
    directory = "%s/YearStats-%s/%s/%s"%(args.parser_directory,args.parser_system,iYear,iTag)
    if os.path.exists(directory):
      yearTag = "%s%s"%(iYear,yearTagProperties["description"][iTag])
      yearTagList.append(yearTag)
      yearTagDir[yearTag] = directory
      yearTagTag[yearTag] = iTag # Used only to retrieve comments

      RunListDat = "%s/RunList/grl-%s.dat"%(args.parser_directory,iYear)
      if os.path.exists(RunListDat):
        fRunList = open(RunListDat,'r')
        runGRL[yearTag] = []
        for iRun in fRunList.readlines():
          runGRL[yearTag].append(int(iRun)) # used only to determine if a run belongs to GRL in recap defects - Data in loss*.txt file NOT reliable
        fRunList.close()
      else:
        print("No GRL list found... Please create it")
        sys.exit()

if len(args.parser_year) == 1:
  singleYear = True
else:
  singleYear = False
  if args.parser_diff2tags:
    print("To compare two tags, you must choose only one year. Exiting...")
    sys.exit()

yearTagList.sort()

if len(yearTagList) == 0:
  print("No year / tag matching - Please check YearStats directory")
  sys.exit()

options = {}
options['defect'] = [args.parser_defect]
options['veto'] = [args.parser_veto]
if options['defect'] == [""] and options['veto'] == [""]:
  options['defect'] = grlDef["intol"]+grlDef["tol"]
  options['veto'] = veto["all"]
else:
  if options['defect'][0] not in grlDef["intol"] and options['veto'][0] not in veto["all"]:
    print("Defect/veto not found. Please check...")
    print("Defect: ",grlDef["intol"]) 
    print("Veto: ",veto["all"])
    sys.exit()
  if options['defect'] == [""]:
    options['defect'] = []
  if options['veto'] == [""]:
    options['veto'] = []

defVetoType = {}
for iDefVeto in options['defect']+options['veto']:      
  if iDefVeto in options['defect']:
    if (iDefVeto in grlDef["intol"]):
      defVetoType[iDefVeto] = "Intolerable defect"
    else:
      defVetoType[iDefVeto] = "Tolerable defect"
  else:
    defVetoType[iDefVeto] = "Veto"


options['recovPlot'] = args.parser_noRecovPlot
options['plotLossPerRun'] = args.parser_plotLPR
options['plotDiff2tags'] = args.parser_diff2tags
options['restrictTagRuns'] = args.parser_restrictTagRuns
options['recapDefects'] = args.parser_recapDefects 
options['savePage1'] = args.parser_savePage1  
options['prepareReproc'] = args.parser_reproc  
runsFilter = []
# If runs filter is requested, look for the runs of the chosen year/tag
if (options['plotDiff2tags'] and options['restrictTagRuns'] in yearTagProperties["description"].keys() and "%s%s"%(args.parser_year[0],yearTagProperties["description"][options['restrictTagRuns']]) in yearTagList): #if requested, restrict the runs to be considered to the one of the tag option
  fRuns = open("%s/runs-ALL.dat"%yearTagDir["%s%s"%(args.parser_year[0],yearTagProperties["description"][options['restrictTagRuns']])])
  for iline in fRuns.readlines():
    runsFilter.append(int(iline))
  fRuns.close()
  print("I am considering only the %d runs of %s"%(len(runsFilter),options['restrictTagRuns']))
  print(runsFilter)

options['minLumiYearStatsDefect'] = args.parser_minLumiLPR
options['retrieveComments'] = args.parser_retrieveComments
if options['savePage1']:
  options['plotLossPerRun'] = True
  options['retrieveComments'] = True

if options['recapDefects']:
  options['plotLossPerRun'] = True
  options['retrieveComments'] = True

if options['prepareReproc']:
  options['plotLossPerRun'] = True
  options['retrieveComments'] = True

if options['retrieveComments']:
  defRecap = {}
  defRecapHtml = {}
  for iDef in options['defect']:
    defRecap[iDef] = "\n\n===== Recap for %s================================================================================================================================\n"%(iDef.ljust(15))
    defRecap[iDef] += "Description: %s - %s\n"%(defectVeto["description"][iDef],defVetoType[iDefVeto])
    if (defVetoType[iDef] == "Intolerable defect"):
      defRecap[iDef] +="     Run| Tot lumi|GRL|Lost lumi|Recov. L.|  LB range |     Author | "
    else:
      defRecap[iDef] +="     Run| Tot lumi|GRL|Aff. lumi|Recov. L.|  LB range |     Author | "

    defRecapHtml[iDef] ='<tr class="out0" id="%s"> <th colspan="8"> %s - LUMILOSTTOBEREPLACED affected </th></tr>'%(iDef,iDef)
    defRecapHtml[iDef] +='<tr class="out0"> <th colspan="8"> Description: %s - %s</th></tr>'%(defectVeto["description"][iDef],defVetoType[iDef])
    if (defVetoType[iDef] == "Intolerable defect"):
      defRecapHtml[iDef] +='<tr class="out0"> <th> Run </th><th> Tot lumi </th><th> GRL </th><th> Lost lumi </th><th> Recov. L. </th><th>  LB range </th><th>     Author </th><th>     Comment </th></tr> '
    else:
      defRecapHtml[iDef] +='<tr class="out0"> <th> Run </th><th> Tot lumi </th><th> GRL </th><th> Aff. lumi </th><th> Recov. L. </th><th>  LB range </th><th>     Author </th><th>     Comment </th></tr> '

if options['prepareReproc']:
  defReproc = {}
  for iDef in options['defect']:
    defReproc[iDef] = ""

if args.parser_run:
  options['runMinLossPerRun'] = min(args.parser_run)
  options['runMaxLossPerRun'] = max(args.parser_run)
else:
  options['runMinLossPerRun'] = 0
  options['runMaxLossPerRun'] = 1e9  

gStyle.SetOptStat(0)

canvasResults = {}
legendResults = {}
stackResults = {}
file = {}

h1Period_IntLuminosity = {}
subperiodNb = {}
runsLPR = {}
lossLPR = {}
loss_rLPR = {}
h1_lossLPR = {}
h1_loss_rLPR = {}

atlasReady = {}

for iYT in yearTagList:
  print("I am treating the following year/tag:%s"%iYT)

  canvasResults[iYT] = {}
  legendResults[iYT] = {}
  stackResults[iYT] = {}

  # Open the Archive filename to compare the luminosity of each period
  if options['plotDiff2tags']:
    yearStatsArchiveFilename = '%s/TProfiles.root'%(yearTagDir[iYT])
    if not (os.path.exists(yearStatsArchiveFilename)):
      print("No %s found - > Skipping"%yearStatsArchiveFilename)
      continue
    
    file[iYT] = TFile(yearStatsArchiveFilename)
    h1Period_IntLuminosity[iYT] = file[iYT].Get("h1Period_IntLuminosity_archive")
    subperiodNb[iYT] = h1Period_IntLuminosity[iYT].GetNbinsX()
    
  if options['plotLossPerRun'] or options['plotDiff2tags']:
    runsLPR[iYT] = {}
    lossLPR[iYT] = {}
    loss_rLPR[iYT] = {}
    h1_lossLPR[iYT] = {}
    h1_loss_rLPR[iYT] = {}
    # Loop on defect+veto (all or single ones)
    for iDefVeto in options['defect']+options['veto']:      
        
      # Define the file name containing the loss per run for this defect
      if "defect" in defVetoType[iDefVeto]:
        lossFileName = "%s/loss-%s.dat"%(yearTagDir[iYT],iDefVeto)
        xAxisTitle = "#splitline{Luminosity loss due to}{%s [pb^{-1}]}"%(defectVeto["description"][iDefVeto])
        if defVetoType[iDefVeto] == "Intolerable defect":
          color = defectVeto["color"][iDefVeto]
        else:
          color = kTeal
      else:
        lossFileName = "%s/loss-%sVETO.dat"%(yearTagDir[iYT],iDefVeto)
        xAxisTitle = "Luminosity loss due to %s[Veto] [pb^{-1}]"%(defectVeto["description"][iDefVeto])
        color = defectVeto["color"][iDefVeto]
        
      # Check if a dat loss file is associated to this veto/defect
      if os.path.exists(lossFileName):
        print("I am reading the %s file"%lossFileName)
        runsLPR[iYT][iDefVeto] = []
        lossLPR[iYT][iDefVeto] = []
        loss_rLPR[iYT][iDefVeto] = []
        f2 = open(lossFileName,'r')
        tmpLines = sorted(f2.readlines())
        for iline in tmpLines: # Loop on all lines of the loss-[defect/veto].dat files
          if defVetoType[iDefVeto] == "Intolerable defect":
            read = match(r"(\d+) \((\d+) ub-1.*\) -> (\d+.\d+) pb-1 \D+(\d+.\d+)\D+",iline)
          else:# Veto loss is never recoverable (not tolerable defects)
            read = match(r"(\d+) \((\d+) ub-1.*\) -> (\d+.\d+) pb-1",iline)
          # retrieve the run number
          runnumber = int(read.group(1))
          # If the runs filter is activated (i.e. runsFilter != 0), check if the runs must be filtered
          if (len(runsFilter) != 0 and runnumber not in runsFilter):
            continue
          # retrieve the total luminosity and the lost luminosity
          luminosity = int(read.group(2))
          lostLumi = float(read.group(3))
          # If defect, look for potential recoverable luminosity
          if defVetoType[iDefVeto] == "Intolerable defect":
            recovLumi = float(read.group(4))
          else: # Veto loss is never recoverable
            recovLumi = 0.
          if runnumber not in atlasReady.keys():
            atlasready_tmp=GetReadyFlag(runnumber)
            atlasReady[runnumber] = []
            for lb in atlasready_tmp.keys():
              if atlasready_tmp[lb]>0: atlasReady[runnumber] += [lb]

          # if the loss is above the required minimum (0 by default), store it
          if (runnumber>options['runMinLossPerRun'] and runnumber<=options['runMaxLossPerRun'] and lostLumi > options['minLumiYearStatsDefect']):
            runsLPR[iYT][iDefVeto].append(runnumber)
            lossLPR[iYT][iDefVeto].append(lostLumi)
            loss_rLPR[iYT][iDefVeto].append(recovLumi)
            if options['retrieveComments'] and "defect" in defVetoType[iDefVeto]: # retrieve comments for defects
              print("@%d"%(runnumber))
              db = DefectsDB(tag=yearTagProperties["defect"][yearTagTag[iYT]])
              system_defects = []
              for iPrefix in grlDef["prefix"]:
                system_defects += [d for d in (db.defect_names | db.virtual_defect_names) if (d.startswith(iPrefix) and iDefVeto in d)]
              #lar_defects = [d for d in (db.defect_names | db.virtual_defect_names) if ((d.startswith("LAR") or d.startswith("CALO_ONLINEDB")) and iDefVeto in d)]
              defects = db.retrieve((runnumber, 1), (runnumber+1, 0), system_defects)   
              defectCompact = {}
              for defect in defects:
                if ("SEVNOISEBURST" in defect.channel and ("HEC" in defect.channel or "FCAL" in defect.channel)): # Skip the HEC/FCAL SEVNOISEBURST defect as they also appear in EMEC
                  continue
                for iDef in options['defect']:
                  if (iDef in defect.channel): # NB : some problem may arise from this incomplete test (if similar name for 2 defects) but there is a protection later when recaping
                    defectSinceLumiAtlasReady = -1
                    defectUntilLumiAtlasReady = -1
                    for iLumiBlock in range(defect.since.lumi,defect.until.lumi):
                      if iLumiBlock in atlasReady[runnumber]:
                        defectUntilLumiAtlasReady = iLumiBlock+1
                        if defectSinceLumiAtlasReady == -1:
                          defectSinceLumiAtlasReady = iLumiBlock
                    print(defectSinceLumiAtlasReady,defectUntilLumiAtlasReady)
                    if defectSinceLumiAtlasReady == -1: # Whole defect was outside ATLAS ready - Skip it
                      continue
                    
                    if defectSinceLumiAtlasReady == defectUntilLumiAtlasReady-1:
                      lbRange = "%10d"%(defectSinceLumiAtlasReady)
                      lbRangeReproc = defectSinceLumiAtlasReady
                    else:
                      lbRange = "%4d->%4d"%(defectSinceLumiAtlasReady,defectUntilLumiAtlasReady-1)
                      lbRangeReproc = "%d-%d"%(defectSinceLumiAtlasReady,defectUntilLumiAtlasReady-1)
                    if ("\n %d |"%runnumber not in defRecap[iDef]):
                      defRecap[iDef] += "\n %d |%s|%3d|%s|%s|%s |%s| %s"%(runnumber,strLumi(luminosity,"ub",False).rjust(9),(runnumber in runGRL[iYT]),(strLumi(lostLumi,"pb",False)).rjust(9),(strLumi(recovLumi,"pb",False)).rjust(9),lbRange,defect.user.rjust(12),defect.comment)
                      defRecapHtml[iDef] += '<tr class="out1"><th> %d </th> <th> %s </th><th> %3d </th><th> %s </th><th> %s </th><th> %s </th><th> %s </th><th> %s </th><tr>'%(runnumber,strLumi(luminosity,"ub",False).rjust(9),(runnumber in runGRL[iYT]),(strLumi(lostLumi,"pb",False)).rjust(9),(strLumi(recovLumi,"pb",False)).rjust(9),lbRange,defect.user.rjust(12),defect.comment)
                      if (options['prepareReproc'] and recovLumi>0.):
                        defReproc[iDef] += "\n@%d"%runnumber
                    else:
                      defRecap[iDef] += "\n -----------------------------------------|%s |%s| %s"%(lbRange,defect.user.rjust(12),defect.comment)
                      defRecapHtml[iDef] += '<tr class="out1"><th colspan="5"><th> %s </th><th> %s </th><th> %s </th><tr>'%(lbRange,defect.user.rjust(12),defect.comment)
                    for iPart in ["EMBA","EMBC","EMECA","EMECC","HECA","HECC","FCALA","FCALC"]:
                      if iPart in defect.channel and "SEVNOISEBURST" not in defect.channel: # Add the affected partition (except for SEVNOISEBURST, where the comment should contain it)
                        defRecap[iDef] += " - %s"%iPart

                    if (options['prepareReproc'] and defect.recoverable):
                      defReproc[iDef] += "\n%s %s G/R # [Originally set by %s:%s] Now fixed/irrecoverable"%(defect.channel,lbRangeReproc,defect.user,defect.comment)
                      if ("SEVNOISEBURST" in defect.channel and "EMEC" in defect.channel):
                        defReproc[iDef] += "\n%s %s G/R # [Originally set by %s:%s] Now fixed/irrecoverable"%(defect.channel.replace("EMEC","HEC"),lbRangeReproc,defect.user,defect.comment)
                        defReproc[iDef] += "\n%s %s G/R # [Originally set by %s:%s] Now fixed/irrecoverable"%(defect.channel.replace("EMEC","FCAL"),lbRangeReproc,defect.user,defect.comment)

        f2.close()

        # if no loss found or if only diff2tags, stop the loop
        if (len(runsLPR[iYT][iDefVeto]) == 0 or not options['plotLossPerRun']): continue

        # If loss found, create histogram to display them for the restricted run with a loss
        h1_lossLPR[iYT][iDefVeto] = MakeTH1("h1_lossLPR_%s_%s"%(iYT,iDefVeto),"Run Number",xAxisTitle,-0.5,-0.5+len(runsLPR[iYT][iDefVeto]),len(runsLPR[iYT][iDefVeto]),color)
        for iX in range(1,h1_lossLPR[iYT][iDefVeto].GetNbinsX()+1):
          if (runsLPR[iYT][iDefVeto][iX-1] not in runGRL[iYT]):
            h1_lossLPR[iYT][iDefVeto].GetXaxis().SetBinLabel(iX,"%d $"%runsLPR[iYT][iDefVeto][iX-1])                
            h1_lossLPR[iYT][iDefVeto].GetXaxis().SetTitle("Run Number ($: run not in GRL)")
          else:
            h1_lossLPR[iYT][iDefVeto].GetXaxis().SetBinLabel(iX,"%d"%runsLPR[iYT][iDefVeto][iX-1])                            

        if defVetoType[iDefVeto] == "Intolerable defect": # recoverable loss for defect
          h1_loss_rLPR[iYT][iDefVeto] = MakeTH1("h1_loss_rLPR_%s_%s"%(iYT,iDefVeto),"Run Number",xAxisTitle,-0.5,-0.5+len(runsLPR[iYT][iDefVeto]),len(runsLPR[iYT][iDefVeto]),kTeal-7)

        # Fill the new histogram(s)
        for irun in range(len(runsLPR[iYT][iDefVeto])):
          h1_lossLPR[iYT][iDefVeto].Fill(irun,lossLPR[iYT][iDefVeto][irun])
          if defVetoType[iDefVeto] == "Intolerable defect":
            h1_loss_rLPR[iYT][iDefVeto].Fill(irun,loss_rLPR[iYT][iDefVeto][irun])

        # Display the new histograms
        index = "LPR_%s"%iDefVeto
        canvasResults[iYT][index] = TCanvas("cLPR_%s_%s"%(iYT,iDefVeto),"%s (only run losses >%.1f pb-1)"%(xAxisTitle,options['minLumiYearStatsDefect']), 200, 10, 1200, 500)
        canvasResults[iYT][index].SetGridy(1)
        h1_lossLPR[iYT][iDefVeto].SetTitle("")
        h1_lossLPR[iYT][iDefVeto].SetMinimum(0.)
        h1_lossLPR[iYT][iDefVeto].Draw("HIST")
        if (options['recovPlot'] and defVetoType[iDefVeto] == "Intolerable defect"):
          h1_lossLPR[iYT][iDefVeto].Draw("HIST SAME")
        legendResults[iYT][index] = MakeLegend(0.855,0.75,0.95,0.95)
        legendResults[iYT][index].SetHeader("%s"%iYT)
        legendResults[iYT][index].AddEntry(h1_lossLPR[iYT][iDefVeto],"Total:%s"%(strLumi(h1_lossLPR[iYT][iDefVeto].Integral(),"pb^{-1}")),"f")
        if (options['recovPlot'] and defVetoType[iDefVeto] == "Intolerable defect"):
          legendResults[iYT][index].AddEntry(h1_lossLPR[iYT][iDefVeto],"Recov:%s"%(strLumi(h1_loss_rLPR[iYT][iDefVeto].Integral(),"pb^{-1}")),"f")
        legendResults[iYT][index].Draw()

# Enf of comment recap
if options['plotLossPerRun'] and options['retrieveComments']:
  f = open("%s/YearStats-%s/%s/%s/recapDefects.txt"%(args.parser_directory,args.parser_system,args.parser_year[0],args.parser_tag[0]),'w')
  fHtml = open("%s/YearStats-%s/%s/%s/recapDefects.html"%(args.parser_directory,args.parser_system,args.parser_year[0],args.parser_tag[0]),'w')
  fHtml.write('<table class="report">')
  fHtml.write('<tr class="out0"> <th width="60pix"></th><th width="90pix"></th><th width="45pix"></th><th width="90pix"></th><th width="90pix"></th><th width="90pix"></th><th width="90pix"></th><th></th></tr>')
  if options['savePage1']:
    fPage1 = open("../LArPage1/DeMoPlots/recapDefects.txt",'w')
    fPage1.write("This list contains all defect in LAr calorimeter, including all for runs not used in GRL\n")
    fPage1.write("It is different from the incremental inefficiency produced by DeMoStatus. Produced by DeMoScan --savePage1\n")
    fPage1.write("Last update : %s"%(strftime("%a, %d %b %Y %H:%M", gmtime())))
  if options['prepareReproc']:
    fReproc = open("YearStats-%s/%s/%s/defectsForReproc.txt"%(args.parser_system,args.parser_year[0],args.parser_tag[0]),'w')

  defTocHtml = '<div style="text-align:left" class="rectangle">'
  for iDef in options['defect']:
    if (iDef in h1_lossLPR[iYT].keys()): # This protection is needed as defRecap may have duplication in some rare cases. See Muon system with "MDT_ROD_PROBLEM_1" and "RPC_PROBLEM_1"
      if ("b-1" in defRecap[iDef]):# At least one data loss in the whole YearStats for this defect 
        defTocHtml += "Direct link to <a href='#%s' target='_self'> %s </a> (%s - %s affected) <br>"%(iDef,iDef,defVetoType[iDef],strLumi(h1_lossLPR[iYT][iDef].Integral(),"pb^{-1}"))
  defTocHtml += '</div>'

  fHtml.write(defTocHtml)
  for iDef in options['defect']:
    if (iDef in h1_lossLPR[iYT].keys()): # This protection is needed as defRecap may have duplication in some rare cases. See Muon system with "MDT_ROD_PROBLEM_1" and "RPC_PROBLEM_1"
      if ("b-1" in defRecap[iDef]):# At least one data loss in the whole YearStats for this defect 
        print(defRecap[iDef])
        f.write(defRecap[iDef])
        fHtml.write("%s</tr>"%defRecapHtml[iDef].replace("LUMILOSTTOBEREPLACED",strLumi(h1_lossLPR[iYT][iDef].Integral(),"pb^{-1}")))
        if options['savePage1']:
          fPage1.write(defRecap[iDef])
          fPage1.write("\nTotal loss: %s (recov:%s)"%(strLumi(h1_lossLPR[iYT][iDef].Integral(),"pb^{-1}"),strLumi(h1_loss_rLPR[iYT][iDef].Integral(),"pb^{-1}")))
        if options['prepareReproc']:
          fReproc.write(defReproc[iDef])

  f.close()
  fHtml.write('</table>')
  fHtml.close()
  if options['savePage1']:
    fPage1.close()    
  if options['prepareReproc']:
    fReproc.close()

# Compare defects/veto run by run (if the year is the same for both) 
if (len(yearTagList) == 2 and options['plotDiff2tags'] and singleYear):
  print("I am now comparing run by run the defects and their affected luminosity")
  
  # First basic check about the datasets used in both tags
  YT0 = yearTagList[0]
  YT1 = yearTagList[1]
  if (subperiodNb[YT0] != subperiodNb[YT1]):
    print("Warning : different number of subperiods...")
  else:
    for iBin in range(subperiodNb[YT0]):
      if (h1Period_IntLuminosity[YT0].GetBinContent(iBin) != h1Period_IntLuminosity[YT1].GetBinContent(iBin)):
        print("Warning : different luminosities in bin %s/%s: %f vs %f"%(h1Period_IntLuminosity[YT0].GetXaxis().GetBinLabel(iBin),h1Period_IntLuminosity[YT1].GetXaxis().GetBinLabel(iBin),h1Period_IntLuminosity[YT0].GetBinContent(iBin),h1Period_IntLuminosity[YT1].GetBinContent(iBin)))
  
  runs_diff2tags = {}
  lumi_diff2tags = {}
  suffixList = ["miss_%s"%yearTagList[0],"miss_%s"%yearTagList[1],"diff"]
  suffixTitle = {"miss_%s"%yearTagList[0]:"No loss in %s"%yearTagList[0],"miss_%s"%yearTagList[1]:"No loss in %s"%yearTagList[1],"diff":"Different losses in both year/tag"}

  for iDefOrVeto in ["defect","veto"]:
    for iSuffix in suffixList:
      runs_diff2tags["%s_%s"%(iDefOrVeto,iSuffix)] = []
  
  for iDefVeto in options['defect']+options['veto']:
    for iSuffix in suffixList:
      lumi_diff2tags["%s_%s"%(iDefVeto,iSuffix)] = {}
    
    if "defect" in defVetoType[iDefVeto]:
      defOrVeto = "defect"
    else:
      defOrVeto = "veto"
      
    firstYT = True # Bool used to avoid displaying twice the same disagreement
    for iYT in yearTagList:
      if iYT==yearTagList[0]: 
        iYT2=yearTagList[1]
        sign = +1
      else: 
        iYT2=yearTagList[0]
        sign = -1      
      if (iDefVeto in runsLPR[iYT] and iDefVeto not in runsLPR[iYT2]):
        for irun in range(len(runsLPR[iYT][iDefVeto])):
          print("%s contains %s %s (%.6f pb-1) for run %d but %s does not!"%(iYT,defOrVeto,iDefVeto,lossLPR[iYT][iDefVeto][irun],runsLPR[iYT][iDefVeto][irun],iYT2))
          defVeto_type = "%s_miss_%s"%(iDefVeto,iYT2)
          defOrVeto_type = "%s_miss_%s"%(defOrVeto,iYT2)

          lumi_diff2tags[defVeto_type][runsLPR[iYT][iDefVeto][irun]] = sign * lossLPR[iYT][iDefVeto][irun]
          if runsLPR[iYT][iDefVeto][irun] not in runs_diff2tags[defOrVeto_type]:
            runs_diff2tags[defOrVeto_type].append(runsLPR[iYT][iDefVeto][irun])
            
      if (iDefVeto in runsLPR[iYT] and iDefVeto in runsLPR[iYT2]):
        for irun in range(len(runsLPR[iYT][iDefVeto])):
          if runsLPR[iYT][iDefVeto][irun] not in runsLPR[iYT2][iDefVeto]:
            print("%s contains %s %s (%.6f pb-1) for run %d but %s does not!"%(iYT,defOrVeto,iDefVeto,lossLPR[iYT][iDefVeto][irun],runsLPR[iYT][iDefVeto][irun],iYT2))
            defVeto_type = "%s_miss_%s"%(iDefVeto,iYT2)
            defOrVeto_type = "%s_miss_%s"%(defOrVeto,iYT2)

            lumi_diff2tags[defVeto_type][runsLPR[iYT][iDefVeto][irun]] = sign * lossLPR[iYT][iDefVeto][irun]
            if runsLPR[iYT][iDefVeto][irun] not in runs_diff2tags[defOrVeto_type]:
              runs_diff2tags[defOrVeto_type].append(runsLPR[iYT][iDefVeto][irun])
          else:
            irun2 = runsLPR[iYT2][iDefVeto].index(runsLPR[iYT][iDefVeto][irun])
            if (lossLPR[iYT][iDefVeto][irun] != lossLPR[iYT2][iDefVeto][irun2] and firstYT):
              print("%s contains %s %s (%.6f pb-1) for run %d; %s also but with a different luminosity %.6f pb-1!"%(iYT,defOrVeto,iDefVeto,lossLPR[iYT][iDefVeto][irun],runsLPR[iYT][iDefVeto][irun],iYT2,lossLPR[iYT2][iDefVeto][irun2]))
              defVeto_type = "%s_diff"%(iDefVeto)
              defOrVeto_type = "%s_diff"%(defOrVeto)

              lumi_diff2tags[defVeto_type][runsLPR[iYT][iDefVeto][irun]] = sign * (lossLPR[iYT][iDefVeto][irun]-lossLPR[iYT2][iDefVeto][irun2])
              if runsLPR[iYT][iDefVeto][irun] not in runs_diff2tags[defOrVeto_type]:
                runs_diff2tags[defOrVeto_type].append(runsLPR[iYT][iDefVeto][irun])
                
      firstYT = False
                
  h1Run_diffTwoYT = {}
  h1_diffTwoYT = {}
  maxAbsLumiDiff = {}
  for iDefOrVeto in ["defect","veto"]:
    for iSuffix in suffixList:
      runs_diff2tags["%s_%s"%(iDefOrVeto,iSuffix)].sort()
      maxAbsLumiDiff["%s_%s"%(iDefOrVeto,iSuffix)] = 0.
  
  for iDefVeto in options['defect']+options['veto']:    
    if "defect" in defVetoType[iDefVeto]:
      defOrVeto = "defect"
      color = defectVeto["color"][iDefVeto]
    else:
      defOrVeto = "veto"
      color = defectVeto["color"][iDefVeto]

    for iSuffix in suffixList:
      defVeto_type = "%s_%s"%(iDefVeto,iSuffix)
      defOrVeto_type = "%s_%s"%(defOrVeto,iSuffix)
      if len(runs_diff2tags[defOrVeto_type]) != 0:
        h1Run_diffTwoYT[defVeto_type] = MakeTH1("h1Run_diffTwoYT_%s"%(defVeto_type),"Run","Defect luminosity loss difference (pb^{-1})", -0.5,-0.5+len(runs_diff2tags[defOrVeto_type]),len(runs_diff2tags[defOrVeto_type]),color)
        h1Run_diffTwoYT[defVeto_type].SetTitle("")
        SetXLabel(h1Run_diffTwoYT[defVeto_type],runs_diff2tags[defOrVeto_type])
        h1_diffTwoYT[defVeto_type] = MakeTH1("h1_diffTwoYT_%s"%(defVeto_type),"Defect luminosity loss difference %s - %s (pb^{-1})"%(YT0,YT1),"Nb of runs",-10.,10.,500,color)
        h1_diffTwoYT[defVeto_type].SetTitle("")
        h1_diffTwoYT[defVeto_type].SetMaximum(len(runs_diff2tags[defOrVeto_type]))
      
      for iRun in range(len(runs_diff2tags[defOrVeto_type])):
        if runs_diff2tags[defOrVeto_type][iRun] in lumi_diff2tags[defVeto_type].keys():
          h1Run_diffTwoYT[defVeto_type].Fill(iRun,lumi_diff2tags[defVeto_type][runs_diff2tags[defOrVeto_type][iRun]])
          h1_diffTwoYT[defVeto_type].Fill(lumi_diff2tags[defVeto_type][runs_diff2tags[defOrVeto_type][iRun]])
          if fabs(lumi_diff2tags[defVeto_type][runs_diff2tags[defOrVeto_type][iRun]]) > maxAbsLumiDiff[defOrVeto_type]:
            maxAbsLumiDiff[defOrVeto_type] = fabs(lumi_diff2tags[defVeto_type][runs_diff2tags[defOrVeto_type][iRun]])
          if fabs(lumi_diff2tags[defVeto_type][runs_diff2tags[defOrVeto_type][iRun]]) > maxAbsLumiDiff[defOrVeto_type]:
            maxAbsLumiDiff[defOrVeto_type] = fabs(lumi_diff2tags[defVeto_type][runs_diff2tags[defOrVeto_type][iRun]])
 
  c_diffTwoYT = {}
  leg_diffTwoYT = {}
  for iDefOrVeto in ["defect","veto"]:
    if iDefOrVeto == "defect":
      listOfDefVeto = options['defect']
    else:
      listOfDefVeto = options['veto']

    for iSuffix in suffixList:
      defOrVeto_type = "%s_%s"%(iDefOrVeto,iSuffix)          
      if (len(runs_diff2tags[defOrVeto_type]) != 0):
        c_diffTwoYT[defOrVeto_type] = TCanvas( "c_%s"%defOrVeto_type,"Differences between %s and %s"%(iYT2,iYT), 200, 10, 1400, 800) 
        c_diffTwoYT[defOrVeto_type].Divide(1,2)
        c_diffTwoYT[defOrVeto_type].cd(2)
        leg_diffTwoYT[defOrVeto_type] = MakeLegend(0.855,0.25,0.95,0.95)
        first = True

        for iDefVeto in listOfDefVeto:
          c_diffTwoYT[defOrVeto_type].cd(1)
          defVeto_type = "%s_%s"%(iDefVeto,iSuffix)

          if (defVeto_type in h1Run_diffTwoYT.keys() and h1Run_diffTwoYT[defVeto_type].GetEntries() != 0):
            leg_diffTwoYT[defOrVeto_type].AddEntry(h1_diffTwoYT[defVeto_type],"#splitline{%s: %d runs}{Underflow: %d Overflow: %d}"%(defectVeto["description"][iDefVeto],h1_diffTwoYT[defVeto_type].GetEntries(),h1_diffTwoYT[defVeto_type].GetBinContent(0),h1_diffTwoYT[defVeto_type].GetBinContent(h1_diffTwoYT[defVeto_type].GetNbinsX()+1)),"P")
            if (first):
              gPad.SetGridy(1)
              h1Run_diffTwoYT[defVeto_type].SetMinimum(-1.2*maxAbsLumiDiff[defOrVeto_type])
              h1Run_diffTwoYT[defVeto_type].SetMaximum(1.2*maxAbsLumiDiff[defOrVeto_type])
              h1Run_diffTwoYT[defVeto_type].Draw("L")
              c_diffTwoYT[defOrVeto_type].cd(2)
              gPad.SetGrid(1)
              h1_diffTwoYT[defVeto_type].Draw("L")
              first = False
            else:
              h1Run_diffTwoYT[defVeto_type].Draw("L SAME")
              c_diffTwoYT[defOrVeto_type].cd(2)
              h1_diffTwoYT[defVeto_type].Draw("L SAME")

        c_diffTwoYT[defOrVeto_type].cd(2)
        leg_diffTwoYT[defOrVeto_type].SetHeader(suffixTitle[iSuffix])
        leg_diffTwoYT[defOrVeto_type].Draw()
