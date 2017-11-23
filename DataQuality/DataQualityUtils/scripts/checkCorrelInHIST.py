#!/usr/bin env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Script to browse the unmerged HIST files and correlate the number of entries in a region defined by (x;y,delta) arguments
# Uses the pathExtract library to extract the EOS path
# See the twiki: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/UsingDQInframerge
# Options:
#  -r ARG1, --run ARG1   Run number
#  -ll ARG2, --lowerlb ARG2
#                        Lower lb
#  -ul ARG3, --upperlb ARG3
#                        Upper lb
#  -s ARG4, --stream ARG4
#                        Stream without prefix:
#                        express/CosmicCalo/Main/ZeroBias/MinBias
#  -t ARG5, --tag ARG5   DAQ tag: data16_13TeV, data16_cos...
#  -a ARG6, --amiTag ARG6
#                        First letter of AMI tag: x->express / f->bulk
#  -x ARG7, --globalX ARG7
#                        X region common to all histos
#  -y ARG8, --globalY ARG8
#                        Y region common to all histos
#  -ia ARG9, --integralAbove ARG9
#                        Lower bound of integral - Not used so far
#  -d ARG10, --globalDelta ARG10
#                        Distance to look around x/(x;y) for 1d/2d plot
#  --histo [ARG11 [ARG11 ...]]
#                        ROOT-file path of histograms - As many as you want
#                        with : [type("2d" only so far)] [root path] [x] [y]
#                        [delta] (if not provided use global)
#  --histoWD [ARG12 [ARG12 ...]]
#                        Webdisplay path of histograms - As many as you want
#                        with : [type("2d" only so far)] [root path] [x] [y]
#                        [delta] (if not provided use global)
# Author : Benjamin Trocme (LPSC Grenoble) / 2017

import os, sys  
import string
import argparse
import xmlrpclib

import pathExtract         

import ROOT
from ROOT import *

# Line below commented to work with release 21. 
# Not sure what was its purpose...
# gROOT.Reset()
gStyle.SetPalette(1)
gStyle.SetOptStat("em")

def lbStr(lb):
  if (lb<10):
    lbString = "_lb000%d"%(lb)
  elif (lb<100):
    lbString = "_lb00%d"%(lb)
  elif  (lb<1000):
    lbString = "_lb0%d"%(lb)
  else:
    lbString = "_lb%d"%(lb)
  return lbString
  

# Main===========================================================================================================
parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('-r','--run',type=int,dest='arg1',default='267599',help="Run number",action='store')
parser.add_argument('-ll','--lowerlb',type=int,dest='arg2',default='0',help="Lower lb",action='store')
parser.add_argument('-ul','--upperlb',type=int,dest='arg3',default='999999',help="Upper lb",action='store')
parser.add_argument('-s','--stream',dest='arg4',default='Main',help="Stream without prefix: express/CosmicCalo/Main/ZeroBias/MinBias",action='store')
parser.add_argument('-t','--tag',dest='arg5',default='data17_13TeV',help="DAQ tag: data16_13TeV, data16_cos...",action='store')
parser.add_argument('-a','--amiTag',dest='arg6',default='f',help="First letter of AMI tag: x->express / f->bulk",action='store')
parser.add_argument('-x','--globalX',type=float,dest='arg7',default='-999.',help='X region common to all histos',action='store')
parser.add_argument('-y','--globalY',type=float,dest='arg8',default='-999.',help='Y region common to all histos',action='store')
parser.add_argument('-ia','--integralAbove',type=float,dest='arg9',default='-999.',help='Lower bound of integral - Not used so far',action='store')
parser.add_argument('-d','--globalDelta',type=float,dest='arg10',default='0.15',help='Distance to look around x/(x;y) for 1d/2d plot',action='store')
parser.add_argument('--histo',dest='arg11',default='',help='ROOT-file path of histograms - As many as you want with : [type("2d" only so far)] [root path] [x] [y] [delta] (if not provided use global)',action='store',nargs="*")
parser.add_argument('--histoWD',dest='arg12',default='',help='Webdisplay path of histograms - As many as you want with : [type("2d" only so far)] [root path] [x] [y] [delta] (if not provided use global)',action='store',nargs="*")

args = parser.parse_args()

parser.print_help()

runNumber = args.arg1
lowerLumiBlock = args.arg2
upperLumiBlock = args.arg3
stream = args.arg4
tag = args.arg5
amiTag = args.arg6
globalX = args.arg7
globalY = args.arg8
integralAbove = args.arg9
globalDelta = args.arg10

b_WebdisplayPath = False
if len(args.arg11): # The histograms ROOT file paths are directly provided 
  hArgs = args.arg11
elif len(args.arg12): # The histograms paths are provided as webdisplay paths
  print "I will have to retrieve the ROOT file path of histograms"
  b_WebdisplayPath = True
  hArgs = args.arg12
  passfile = open("/afs/cern.ch/user/l/larmon/public/atlasdqmpass.txt")
  passwd = passfile.read().strip(); passfile.close()
  s = xmlrpclib.ServerProxy('https://%s@atlasdqm.cern.ch'%(passwd))  
  prefix = {'express':'express_','Egamma':'physics_','CosmicCalo':'physics_','JetTauEtmiss':'physics_','Main':'physics_','ZeroBias':'physics_','MinBias':'physics_'}
  run_spec = {'run_list':[runNumber],'stream':"%s%s"%(prefix[stream],stream)}
else:
  print "You need to define at least 1 histogram..."
  sys.exit()

histos = {}
# Histograms must be necessary of this predefined type
histoTypes = ["2d"]


runFilePath = "root://eosatlas.cern.ch/%s"%(pathExtract.returnEosHistPath(runNumber,stream,amiTag,tag)).rstrip()
if ("FILE NOT FOUND" in runFilePath):
  print "No merged file found..."
  sys.exit()

f = TFile.Open(runFilePath)

# Display the Tier0 merged histograms
c = {}
box = {}
line = {}
line2 = {}
arrow = {}
histoMerged = {}
nLB=2500
nbHitInHot = {}

for iArg in xrange(len(hArgs)): # Loop on histogram arguments
  if hArgs[iArg] in histoTypes: # I found a new histogram - Process the next arguments
    if hArgs[iArg] == "2d": 
      regionBins = []
      tmp_type = hArgs[iArg]
      tmp_path = hArgs[iArg+1]
      if b_WebdisplayPath: # Replace the webdisplay path bythe ROOT file path
        dqmf_config = s.get_dqmf_configs(run_spec, tmp_path) 
        tmp_path = dqmf_config['%d'%runNumber]['annotations']['inputname']

      if (iArg+2>=len(hArgs) or hArgs[iArg+2] in histoTypes): # No x,y,delta for this histogram choose default
        tmp_x = globalX
        tmp_y = globalY
        tmp_delta = globalDelta
        iArg = iArg +2
      else: # Custom (x,y,delta) for this histogram
        tmp_x = float(hArgs[iArg+2])
        tmp_y = float(hArgs[iArg+3])
        tmp_delta = float(hArgs[iArg+4])
        iArg+5

      histoMerged[tmp_path] = f.Get("run_%d/%s"%(runNumber,tmp_path))
      histoMerged[tmp_path].SetTitle("%s - Run %d"%(histoMerged[tmp_path].GetTitle(),runNumber))

      c[tmp_path] = TCanvas(tmp_path)
      c[tmp_path].SetLogz(1)
      gStyle.SetPalette(1)
      gStyle.SetOptStat("")
      histoMerged[tmp_path].Draw("COLZ")
      box[tmp_path] = TBox(tmp_x-tmp_delta,tmp_y-tmp_delta,tmp_x+tmp_delta,tmp_y+tmp_delta)
      box[tmp_path].SetLineColor(kRed+1)
      box[tmp_path].SetLineWidth(3)
      box[tmp_path].SetFillStyle(0)
      box[tmp_path].Draw()
      
# Extract the list of bins where to count.
# Scans the window to find all bins that fall in the window
# The regionBins is defined for each histogram allowing different binning
      nSteps = 1000
      subStep = 2*tmp_delta/nSteps
      for ix in range(nSteps):
        iX = tmp_x - tmp_delta + ix * subStep 
        for iy in range (nSteps):
          iY = tmp_y - tmp_delta + iy * subStep
          tmp_bin = histoMerged[tmp_path].FindBin(iX,iY)
          if (tmp_bin not in regionBins):
            regionBins.append(tmp_bin)

      histos[tmp_path]={'type':tmp_type,'regionBins':regionBins}
      nbHitInHot[tmp_path] = [0.] * nLB

for iHisto in histos.keys():
  print iHisto,histos[iHisto]

# Extract all the unmerged files available with the LB range
lbFilePathList = pathExtract.returnEosHistPathLB(runNumber,lowerLumiBlock,upperLumiBlock,stream,amiTag,tag)

print "I have found the merged HIST file %s"%(runFilePath)
print "I have found %d unmerged HIST files"%(len(lbFilePathList))
print "The first one is root://eosatlas.cern.ch/%s"%(lbFilePathList[0])
print "The last one is root://eosatlas.cern.ch/%s"%(lbFilePathList[-1])

# Loop on all unmerged files
fLB = {}
listLB = []
for count,lbFile in enumerate(lbFilePathList):
  lbFilePath = "root://eosatlas.cern.ch/%s"%(lbFile).rstrip()
  # Extract lb from the filename and display it
  ilb = int((lbFile.split("_lb")[1]).split("._")[0])
  if ilb not in listLB:
    listLB.append(ilb)
  if (count%100 == 0):
    sys.stdout.write("\n I processed %d/%d files \n LBs:"%(count,len(lbFilePathList)))
  sys.stdout.write("%d "%(ilb))
  sys.stdout.flush()
  fLB[lbFile] = TFile.Open(lbFilePath)
  histoLB = {}
  for iPath in histos.keys():
    histoLB[iPath] = fLB[lbFile].Get("run_%d/%s"%(runNumber,iPath))
    for iBin in histos[iPath]['regionBins']:
      nbHitInHot[iPath][ilb] = nbHitInHot[iPath][ilb] + histoLB[iPath].GetBinContent(iBin)

  fLB[lbFile].Close()

# Finally create final histograms to be displayed
hCorrel = {}
cCorrel = {}
legCorrel = {}

hEvol = {}
cEvol = {}
legEvol = {}

hRatio = {}
cRatio = {}
nbHitRatio = {}
legCorrel = {}

for iPath in histos.keys():
  for iPath2 in histos.keys():
    corr = "%s_%s"%(iPath,iPath2)
    if (iPath != iPath2): # Correlation plots
      print "====== I am checking correlation between %s and %s"%(iPath.split("/")[-1],iPath2.split("/")[-1])
      
      hCorrel[corr] = TH2D("Correlation_%s"%corr,"Correlation_%s"%corr,50,min(nbHitInHot[iPath])-1,max(nbHitInHot[iPath])+1,50,min(nbHitInHot[iPath2])-1,max(nbHitInHot[iPath2])+1)
      hCorrel[corr].SetXTitle(iPath.split("/")[-1])
      hCorrel[corr].SetYTitle(iPath2.split("/")[-1])
      
      nbHitRatio[corr] = [-999.]*nLB
      for iLB in listLB:
        if (nbHitInHot[iPath2][iLB] !=0):
          nbHitRatio[corr][iLB] = nbHitInHot[iPath][iLB]/nbHitInHot[iPath2][iLB]
      
      hRatio[corr] = TH1D("Ratio_%s"%corr,"Ratio_%s"%corr,50,-1.,max(nbHitRatio[corr])+1)
      hRatio[corr].SetXTitle("%s/%s"%(iPath.split("/")[-1],iPath2.split("/")[-1]))
      
      for iLB in listLB:
        if (nbHitInHot[iPath][iLB] !=0 or nbHitInHot[iPath2][iLB] != 0.):
          hCorrel[corr].Fill(nbHitInHot[iPath][iLB],nbHitInHot[iPath2][iLB])
          print "LB: %d -> %.2f / %.2f"%(iLB,nbHitInHot[iPath][iLB],nbHitInHot[iPath2][iLB])
        hRatio[corr].Fill(nbHitRatio[corr][iLB])
        
      cCorrel[corr] = TCanvas()
      hCorrel[corr].Draw("COLZ")     
      legCorrel[corr] = TLegend(0.12,0.75,0.75,0.85)
      legCorrel[corr].SetHeader("Run %d / %d LB in total - %d LB with correl"%(runNumber,len(listLB),hCorrel[corr].GetEntries()))
      legCorrel[corr].Draw()
      
      cRatio[corr] = TCanvas()
      hRatio[corr].Draw("COLZ")     
    elif (iPath not in hEvol.keys()): # Evolution of nb of hit per LB
      hEvol[iPath] = TH1D("Evolution_%s"%iPath,"%s"%(iPath.split("/")[-1]),max(listLB)-min(listLB),min(listLB),max(listLB))
      hEvol[iPath].SetXTitle("Luminosity block")
      hEvol[iPath].SetYTitle("Nb of hits")
      hEvol[iPath].SetMarkerColor(kGreen+2)
      hEvol[iPath].SetMarkerStyle(20)
    
      for iLB in listLB:
        hEvol[iPath].Fill(iLB,nbHitInHot[iPath][iLB])
        
      cEvol[iPath] = TCanvas()
      hEvol[iPath].Draw("P HIST")
      
