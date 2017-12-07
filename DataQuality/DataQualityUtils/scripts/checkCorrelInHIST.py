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
#  -t ARG5, --tag ARG5   DAQ tag: data16_13TeV, data16_cos...By default
#                        retrieve it via atlasdqm
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

from ROOT import TFile,TCanvas,TBox,TPaveText,TColor
from ROOT import TH1,TH2,TH1I,TH1D,TH2D
from ROOT import kBlue,kGreen,kRed
from ROOT import gStyle

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
parser.add_argument('-t','--tag',dest='arg5',default='',help="DAQ tag: data16_13TeV, data16_cos...By default retrieve it via atlasdqm",action='store')
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
if args.arg5 != "":
  tag = args.arg5
else: # Try to retrieve the data project tag via atlasdqm
  if (not os.path.isfile("atlasdqmpass.txt")):
    print "To retrieve the data project tag, you need to generate an atlasdqm key and store it in this directory as atlasdqmpass.txt (yourname:key)"
    print "To generate a kay, go here : https://atlasdqm.cern.ch/dqauth/"
    print "You can also define by hand the data project tag wit hthe option -t"
    sys.exit()
  passfile = open("atlasdqmpass.txt")
  passwd = passfile.read().strip(); passfile.close()
  passurl = 'https://%s@atlasdqm.cern.ch'%passwd
  s = xmlrpclib.ServerProxy(passurl)
  run_spec = {'stream': 'physics_CosmicCalo', 'proc_ver': 1,'source': 'tier0', 'low_run': runNumber, 'high_run':runNumber}
  run_info= s.get_run_information(run_spec)
  if '%d'%runNumber not in run_info.keys() or len(run_info['%d'%runNumber])<2:
    print "Unable to retrieve the data project tag via atlasdqm... Please double check your atlasdqmpass.txt or define it by hand with -t option"
    sys.exit()
  tag = run_info['%d'%runNumber][1]

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

      c[tmp_path] = TCanvas(tmp_path,tmp_path)
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
# and store number of hits per histogram
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

# Dump the correlations in histograms to be displayed
hCorrel = {}
cCorrel = {}

hEvol = {}
cEvol = {}
legEvol = {}

hRatio = {}
cRatio = {}
nbHitRatio = {}
paveCorrel = {}

for iPath in histos.keys():
  for iPath2 in histos.keys():
    corr = "%s_%s"%(iPath,iPath2)
    corr2 = "%s_%s"%(iPath2,iPath)
    if (iPath != iPath2 and corr2 not in hCorrel.keys()): # Correlation plots
      print "====== I am checking correlation between %s and %s"%(iPath.split("/")[-1],iPath2.split("/")[-1])
      
      hCorrel[corr] = TH2D("Correlation_%s"%corr,"Correlation_%s"%corr,50,min(nbHitInHot[iPath])-1,max(nbHitInHot[iPath])+1,50,min(nbHitInHot[iPath2])-1,max(nbHitInHot[iPath2])+1)
      hCorrel[corr].SetXTitle(iPath.split("/")[-1])
      hCorrel[corr].SetYTitle(iPath2.split("/")[-1])      
      
      nbHitRatio[corr] = [-999.]*nLB
      nbHitRatio[corr2] = [-999.]*nLB
      for iLB in listLB:
        if (nbHitInHot[iPath2][iLB] !=0):
          nbHitRatio[corr][iLB] = nbHitInHot[iPath][iLB]/nbHitInHot[iPath2][iLB]
        if (nbHitInHot[iPath][iLB] !=0):
          nbHitRatio[corr2][iLB] = nbHitInHot[iPath2][iLB]/nbHitInHot[iPath][iLB]
          
      hRatio[corr] = TH1D("Ratio_%s"%corr,"Ratio_%s"%corr,100,0.,max(nbHitRatio[corr])+1)
      hRatio[corr].SetXTitle("%s/%s"%(iPath.split("/")[-1],iPath2.split("/")[-1]))
      hRatio[corr].SetMarkerColor(kBlue+2)
      hRatio[corr].SetMarkerStyle(20)
      hRatio[corr2] = TH1D("Ratio_%s"%corr2,"Ratio_%s"%corr2,100,0.,max(nbHitRatio[corr2])+1)
      hRatio[corr2].SetXTitle("%s/%s"%(iPath2.split("/")[-1],iPath.split("/")[-1]))
      hRatio[corr2].SetMarkerColor(kBlue+2)
      hRatio[corr2].SetMarkerStyle(20)
            
      for iLB in listLB:
        if (nbHitInHot[iPath][iLB] !=0 or nbHitInHot[iPath2][iLB] != 0.):
          hCorrel[corr].Fill(nbHitInHot[iPath][iLB],nbHitInHot[iPath2][iLB])
          print "LB: %d -> %.2f / %.2f"%(iLB,nbHitInHot[iPath][iLB],nbHitInHot[iPath2][iLB])
        if nbHitRatio[corr][iLB]!= -999:
          hRatio[corr].Fill(nbHitRatio[corr][iLB])
        if nbHitRatio[corr2][iLB]!= -999:
          hRatio[corr2].Fill(nbHitRatio[corr2][iLB])
        
      cCorrel[corr] = TCanvas("Correl-%s"%corr,"Correl-%s"%corr)
      hCorrel[corr].Draw("COLZ")     
      paveCorrel[corr] = TPaveText(.1,.72,.9,.9,"NDC")
      paveCorrel[corr].SetFillColor(kBlue-10)
      paveCorrel[corr].AddText("Run %d / %d LBs in total - %d LBs with >=1 entry in either plot"%(runNumber,len(listLB),hCorrel[corr].GetEntries()))
      paveCorrel[corr].AddText("Correlation factor:%.3f"%(hCorrel[corr].GetCorrelationFactor()))
      fractionNonZero = hRatio[corr].Integral(2,100)/hRatio[corr].Integral(1,100)
      if fractionNonZero != 0.:
        meanNonZero = hRatio[corr].GetMean()/fractionNonZero
      else:
        meanNonZero = 0.
      paveCorrel[corr].AddText("When >=1 entry in X plot(%d LBs), %.0f %% events with >=1 entry in Y plot(<ratio>=%.2f)"%(hRatio[corr].Integral(1,100),fractionNonZero*100.,meanNonZero))      
      fractionNonZero = hRatio[corr2].Integral(2,100)/hRatio[corr2].Integral(1,100)
      if fractionNonZero != 0.:
        meanNonZero = hRatio[corr2].GetMean()/fractionNonZero
      else:
        meanNonZero = 0.
      paveCorrel[corr].AddText("When >=1 entry in Y plot(%d LBs), %.0f %% events with >=1 entry in X plot(<ratio>=%.2f)"%(hRatio[corr2].Integral(1,100),fractionNonZero*100.,meanNonZero))
      paveCorrel[corr].Draw()
                               
      
      cRatio[corr] = TCanvas("Ratio-%s"%corr,"Ratio-%s"%corr)
      hRatio[corr].Draw("P HIST")     
      cRatio[corr2] = TCanvas("Ratio-%s"%corr2,"Ratio-%s"%corr2)
      hRatio[corr2].Draw("P HIST")     
    elif (iPath not in hEvol.keys()): # Evolution of nb of hit per LB
      hEvol[iPath] = TH1D("Evolution_%s"%iPath,"%s"%(iPath.split("/")[-1]),max(listLB)-min(listLB),min(listLB),max(listLB))
      hEvol[iPath].SetXTitle("Luminosity block")
      hEvol[iPath].SetYTitle("Nb of hits")
      hEvol[iPath].SetMarkerColor(kGreen+2)
      hEvol[iPath].SetMarkerStyle(20)
    
      for iLB in listLB:
        hEvol[iPath].Fill(iLB,nbHitInHot[iPath][iLB])
        
      cEvol[iPath] = TCanvas("LB evol - %s"%iPath)
      hEvol[iPath].Draw("P HIST")

  
print "====== Summary data"
already = []
for iPath in histos.keys():
  for iPath2 in histos.keys():
    corr = "%s_%s"%(iPath,iPath2)
    corr2 = "%s_%s"%(iPath2,iPath)
    if (iPath != iPath2 and corr2 not in already): # Correlation plots
      print "====== %s vs %s"%(iPath.split("/")[-1],iPath2.split("/")[-1])
      print "Correlation factor: %.3f"%(hCorrel[corr].GetCorrelationFactor())
      
      fractionNonZero = hRatio[corr].Integral(2,100)/hRatio[corr].Integral(1,100)
      if fractionNonZero != 0.:
        meanNonZero = hRatio[corr].GetMean()/fractionNonZero
      else:
        meanNonZero = 0.
      print "When there is at least one entry in %s (%d LBs), there are %.1f %% of events with an entry in %s - Mean ratio: %.2f"%(iPath2.split("/")[-1],hRatio[corr].Integral(1,100),fractionNonZero*100.,iPath.split("/")[-1],meanNonZero)
      
      fractionNonZero = hRatio[corr2].Integral(2,100)/hRatio[corr2].Integral(1,100)
      if fractionNonZero != 0.:
        meanNonZero = hRatio[corr2].GetMean()/fractionNonZero
      else:
        meanNonZero = 0.
      print "When there is at least one entry in %s (%d LBs), there are %.1f %% of events with an entry in %s - Mean ratio: %.2f"%(iPath.split("/")[-1],hRatio[corr2].Integral(1,100),fractionNonZero*100.,iPath2.split("/")[-1],meanNonZero)
      
      already.append(corr)

raw_input("I am done...")
