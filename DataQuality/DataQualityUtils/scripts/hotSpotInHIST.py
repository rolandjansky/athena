#!/usr/bin env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Script to browse the unmerged HIST files and extract LBs for which at least N occurences of an object is found
# at a position found to be noisy
# Uses the pathExtract library to extract the EOS path
# See the twiki: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/UsingDQInframerge
# Author : Benjamin Trocme (LPSC Grenoble) / 2015-2016

import os, sys
import argparse
from six.moves import xmlrpc_client as xmlrpclib
import math
from DataQualityUtils import pathExtract         

import ROOT as R

def lbStr(lb):
  """Return formatted lumiblock string"""
  return "_lb"+lb.zfill(5)  # is this still the case?

def getStruct(r_in):
  """Produce a list containing the paths of all histograms in the file"""
  # r_in = R.TFile(infile, "READ")
  t_in = r_in.Get(r_in.GetListOfKeys()[0].GetName())
  keys = [k for k in t_in.GetListOfKeys()]
  keynames = [ k.GetName() for k in keys ]

  def itdir(thisdir, histPathList):
    keys = thisdir.GetListOfKeys()
    keynames = [ k.GetName() for k in keys ]
    if any([ k.GetClassName() == "TDirectoryFile" for k in keys ]):
      return [ k.ReadObj() for k in keys if k.GetClassName() == "TDirectoryFile" ]
    else:
      hists = []
      for k in keys:
        if isinstance(k.ReadObj(), R.TH1):
          hists.append(thisdir.GetPathStatic().split(":/")[1]+"/"+k.GetName())
      histPathList.extend(hists)
      return ""
  
  histPathList = []
  for key in keys:
    if "lb_" in key.GetName() or "_LB" in key.GetName(): continue                                                                                                       
    #if "Calo" not in key.GetName() and "Jet" not in key.GetName(): continue
    if key.GetClassName() == "TDirectoryFile":
      d = key.ReadObj()
      while not isinstance(d, str): #d != None:                                                                                                                        
        if isinstance(d, list):
          for di in d:
            d = itdir(di, histPathList)
        else:
          d = itdir(d, histPathList)
  return histPathList
  

def getHistoInfo(objectType, runNumber):
  """Histo path definition base on object type"""
  # histoPath   : Histogram path in the ROOT file
  # histoLegend : Histogram legend
  # histoColor  : Colors for the final summary plot
  # histoName   : Name of object
  # Types of plot
  # 2d_etaPhiHotSpot   : 2D occupancy plots: (eta/phi) required
  # 2d_xyHotSpot       : any 2D plots: (x/y) required
  # 1d_etaHotSpot      : 1D occupancy plot along eta: (eta) required
  # 1d_phiHotSpot      : 1D occupancy plot along phi: (phi) required
  # 1d_integralAbove   : integral between (integralAbove) and infinity. (integralAbove) required

  runstr = "run_"+str(runNumber)

  if "TopoCluster" in objectType:
    #histoPath  = {"Et10GeV":runstr+"/CaloMonitoring/ClusterMon/CaloCalTopoClustersNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh1",
    #              "Et25GeV":runstr+"/CaloMonitoring/ClusterMon/CaloCalTopoClustersNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh2",
    #              "Et50GeV":runstr+"/CaloMonitoring/ClusterMon/CaloCalTopoClustersNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh3"}
    #histoLegend = {"Et10GeV":"Et > 10GeV",
    #               "Et25GeV":"Et > 25GeV",
    #               "Et50GeV":"Et > 50GeV"}
    histoPath = {"ECA_thr0":runstr+"/CaloTopoClusters/CalECA/Thresh0ECAOcc",
                 "ECA_thr1":runstr+"/CaloTopoClusters/CalECA/Thresh1ECAOcc",
                 "ECA_thr2":runstr+"/CaloTopoClusters/CalECA/Thresh2ECAOcc",
                 "ECA_thr3":runstr+"/CaloTopoClusters/CalECA/Thresh3ECAOcc",
                 "ECC_thr0":runstr+"/CaloTopoClusters/CalECC/Thresh0ECCOcc",
                 "ECC_thr1":runstr+"/CaloTopoClusters/CalECC/Thresh1ECCOcc",
                 "ECC_thr2":runstr+"/CaloTopoClusters/CalECC/Thresh2ECCOcc",
                 "ECC_thr3":runstr+"/CaloTopoClusters/CalECC/Thresh3ECCOcc",
                 "BAR_thr0":runstr+"/CaloTopoClusters/CalBAR/Thresh0BAROcc",
                 "BAR_thr1":runstr+"/CaloTopoClusters/CalBAR/Thresh1BAROcc",
                 "BAR_thr2":runstr+"/CaloTopoClusters/CalBAR/Thresh2BAROcc",
                 "BAR_thr3":runstr+"/CaloTopoClusters/CalBAR/Thresh3BAROcc"}
    histoLegend = {"ECA_thr0":"ECA thr0",
                   "ECA_thr1":"ECA thr1",
                   "ECA_thr2":"ECA thr2",
                   "ECA_thr3":"ECA thr3",
                   "ECC_thr0":"ECC thr0",
                   "ECC_thr1":"ECC thr1",
                   "ECC_thr2":"ECC thr2",
                   "ECC_thr3":"ECC thr3",
                   "BAR_thr0":"BAR thr0",
                   "BAR_thr1":"BAR thr1",
                   "BAR_thr2":"BAR thr2",
                   "BAR_thr3":"BAR thr3"}
    histoType = "2d_etaPhiHotSpot"
    histoName = "TopoClusters"
  elif "EMTopoCluster" in objectType:
    histoPath  = {"Et4GeV":runstr+"/CaloMonitoring/ClusterMon/LArClusterEMNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh1",
                  "Et10GeV":runstr+"/CaloMonitoring/ClusterMon/LArClusterEMNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh2",
                  "Et25GeV":runstr+"/CaloMonitoring/ClusterMon/LArClusterEMNoTrigSel/2d_Rates/m_clus_etaphi_Et_thresh3"}
    histoLegend = {"Et4GeV":"Et > 4GeV",
                   "Et10GeV":"Et > 10GeV",
                   "Et25GeV":"Et > 25GeV"}
    histoType = "2d_etaPhiHotSpot"
    histoName = "EMTopoClusters"

  elif "EMTopoJet" in objectType and "_eta" not in objectType:
    histoPath  = {"noCut":runstr+"/Jets/AntiKt4EMTopoJets/OccupancyEtaPhi",
                  "cut1":runstr+"/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_20000_inf_pt_inf_500000",
                  "cut2":runstr+"/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_500000_inf_pt_inf_1000000",
                  "cut3":runstr+"/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_1000000_inf_pt_inf_2000000",
                  "cut4":runstr+"/Jets/AntiKt4EMTopoJets/OccupancyEtaPhisel_2000000_inf_pt_inf_8000000"}
    histoLegend = {"noCut":"No cut",
                   "cut1":"20GeV-500GeV",
                   "cut2":"500GeV-1TeV",
                   "cut3":"1TeV-2TeV",
                   "cut4":"2TeV-8TeV"}
    histoType = "2d_etaPhiHotSpot"
    histoName = "EMTopoJets"
  elif (objectType == "EMTopoJets_eta"):
    histoPath  = {"cut1":runstr+"/Jets/AntiKt4EMTopoJets/etasel_20000_inf_pt_inf_500000",
                  "cut2":runstr+"/Jets/AntiKt4EMTopoJets/etasel_500000_inf_pt_inf_1000000",
                  "cut3":runstr+"/Jets/AntiKt4EMTopoJets/etasel_1000000_inf_pt_inf_2000000",
                  "cut4":runstr+"/Jets/AntiKt4EMTopoJets/etasel_2000000_inf_pt_inf_8000000"}
    histoLegend = {"cut1":"20GeV-500GeV",
                   "cut2":"500GeV-1TeV",
                   "cut3":"1TeV-2TeV",
                   "cut4":"2TeV-8TeV"}
    histoType = "1d_etaHotSpot"
    histoName = "EMTopoJets"

  elif objectType == "Tau":
    histoPath  = {"NoCut":runstr+"/Tau/tauPhiVsEta",
                  "Et15GeV":runstr+"/Tau/tauPhiVsEtaEt15",
                  "Et15GeVBdtLoose":runstr+"/Tau/tauPhiVsEtaEt15BDTLoose"}
    histoLegend = {"NoCut":"Et > 4GeV",
                   "Et15GeV":"Et > 10GeV",
                   "Et15GeVBdtLoose":"Et > 15GeV-BDT loose"}
    histoType = "2d_etaPhiHotSpot"
    histoName = "Tau"
  elif objectType == "Tau_phi":
    histoPath  = {"single":runstr+"/Tau/tauPhi"}
    histoLegend = {"single":"All candidates"}
    histoType = "1d_phiHotSpot"
    histoName = "Tau"
  elif objectType == "Tau_eta":
    histoPath  = {"single":runstr+"/Tau/tauEta"}
    histoLegend = {"single":"All candidates"}
    histoType = "1d_etaHotSpot"
    histoName = "Tau"
  elif objectType == "NumberTau":
    histoPath  = {"allTau":runstr+"/Tau/nTauCandidates",
                  "highPt":runstr+"/Tau/nHighPtTauCandidates"}
    histoLegend = {"allTau":"All candidates",
                   "highPt":"High Pt (>100GeV) candidates"}
    histoType = "1d_integralAbove"
    histoName = "Number of Tau candidates"

  elif objectType == "TightFwdElectrons":
    histoPath  = {"single":runstr+"/egamma/forwardElectrons/forwardElectronTightEtaPhi"}
    histoLegend = {"single":"10GeV"}
    histoType = "2d_etaPhiHotSpot"
    histoName = "Tight electrons"
  elif objectType == "NumberTightFwdElectrons":
    histoPath  = {"single":runstr+"/egamma/forwardElectrons/forwardElectronTightN"}
    histoLegend = {"single":"All candidates"}
    histoType = "1d_integralAbove"
    histoName = "Number of tight forward electrons"
  elif objectType == "forwardElectronEtaPhi":
    histoPath  = {"single":runstr+"/egamma/forwardElectrons/forwardElectronEtaPhi"}
    histoLegend = {"single":"All candidates"}
    histoType = "2d_etaPhiHotSpot"
    histoName = "Forward electrons"

  elif objectType == "NumberHLTJet":
    histoPath  = {"HLTJet":runstr+"/HLT/JetMon/HLT/10j40_L14J20/HLTJet_n"}
    histoLegend = {"HLTJet":"All candidates"}
    histoType = "1d_integralAbove"
    histoName = "Number of HLT jets - 10J40_L14J20 trigger"

  elif objectType == "LArDigits":
    histoPath  = {"Null-EMBA":runstr+"/LAr/Digits/Barrel/NullDigitChan_BarrelA",
                  "Satu-EMBA":runstr+"/LAr/Digits/Barrel/SaturationChan_BarrelA",
                  "Null-EMBC":runstr+"/LAr/Digits/Barrel/NullDigitChan_BarrelC",
                  "Satu-EMBC":runstr+"/LAr/Digits/Barrel/SaturationChan_BarrelC",
    }
    histoLegend = {"Null-EMBA":"Null digit - EMBA",
                   "Satu-EMBA":"Saturated digit - EMBA",
                   "Null-EMBC":"Null digit - EMBC",
                   "Satu-EMBC":"Saturated digit - EMBC",}
    histoType = "2d_xyHotSpot"
    histoName = "LAr saturated/null digits"
  else:
    print("Object type:",objectType,"not recognised!")
    sys.exit()

  cols = [R.kBlue+1, R.kGreen+1, R.kOrange+7, R.kMagenta+2, R.kCyan-3,
          R.kGreen-5, R.kRed, R.kCyan, R.kViolet,
          R.kAzure-4, R.kMagenta-9, R.kGreen-9, R.kYellow]
  histoColor = {k:cols[list(histoPath.keys()).index(k)] for k in histoPath.keys()}

  return histoPath, histoLegend, histoColor, histoType, histoName

def main(args):
  histoPath, histoLegend, histoColor, histoType, histoName = getHistoInfo(args.objectType, args.runNumber)

  # Depending of the histo/check type, define the summary title and
  # check that the position of the "hot spot" (or lower bound of the integral) is defined
  b_wholeHisto = False
  b_ValueNotEntries = False

  if histoType == "2d_etaPhiHotSpot":
    summaryTitle = "Nb of hits in a region of %.2f around the position (%.2f,%.2f) - %s"%(args.deltaSpot,args.etaSpot,args.phiSpot,histoName)
    statement = "I have looked for LBs with at least %.0f entries at position (%.2f,%.2f) in %s histogram"%(args.minInLB,args.etaSpot,args.phiSpot,histoName)
    if (args.etaSpot==-999. or args.phiSpot==-999.):
      print("No eta/phi defined -> whole histogram considered!")
      b_wholeHisto = True
  if histoType == "2d_xyHotSpot":
    b_ValueNotEntries = True
    if (args.deltaSpot != 0):
      print("Warning: you have been summing over several bins a variable that may be not summable (different from summing hits!)")
    summaryTitle = "Value in a region of %.2f around the position (%.2f,%.2f) - %s"%(args.deltaSpot,args.xSpot,args.ySpot,histoName)
    statement = "I have looked for LBs with at least variable > %.2f at position (%.2f,%.2f) in %s histogram"%(args.minInLB,args.xSpot,args.ySpot,histoName)
    if (args.xSpot==-999. or args.ySpot==-999.):
      print("No x/y defined -> whole histogram considered!")
      print("Warning: you have been summing over several bins a variable that may be not summable (different from summing hits!)")
      b_wholeHisto = True
  elif histoType == "1d_etaHotSpot":
    summaryTitle = "Nb of hits in a region of %.2f around the eta position %.2f - %s"%(args.deltaSpot,args.etaSpot,histoName)
    statement = "I have looked for LBs with at least %.0f entries at eta position %.2f in %s histogram"%(args.minInLB,args.etaSpot,histoName)
    if (args.etaSpot==-999.):
      print("No eta/phi -> whole histogram considered!")
      b_wholeHisto = True
  elif histoType == "1d_phiHotSpot":
    summaryTitle = "Nb of hits in a region of %.2f around the phi position %.2f - %s"%(args.deltaSpot,args.phiSpot,histoName)
    statement = "I have looked for LBs with at least %.0f entries at phi position %.2f in %s histogram"%(args.minInLB,args.phiSpot,histoName)
    if (args.phiSpot==-999.):
      print("No eta/phi defined -> whole histogram considered!")
      b_wholeHisto = True
  elif histoType == "1d_integralAbove":
    summaryTitle = "Nb of hits in the band above %.2f - %s"%(args.integralAbove,histoName)
    statement = "I have looked for LBs with at least %.0f entries in band above %.2f in %s histogram"%(args.minInLB,args.integralAbove,histoName)
    if (args.integralAbove==-999.):
      print("No lwoer bound defined -> whole histogram considered!")
      b_wholeHisto = True
  #    print "You must define the lower bound of your integral"
  #    sys.exit()
  # Definition of Canvas option depending on histogram type
  if (args.objectType == "NumberTightFwdElectrons" or args.objectType == "NumberTau"):
    canvasOption = "logy"
  else:
    canvasOption = ""

  #########################################################################
  # Look for the final merged HIST file
  # and plot the histogram
  runFilePath = "root://eosatlas.cern.ch/%s"%(pathExtract.returnEosHistPath(args.runNumber,args.stream,args.amiTag,args.tag)).rstrip()
  if ("FILE NOT FOUND" in runFilePath):
    print("No merged file found...")
    sys.exit()

  f = R.TFile.Open(runFilePath)

  #histPathList = getStruct(f) # this isn't working as expected with xrootd

  histo = {}
  #hists = [ k.GetName() for k in t.GetListOfKeys() ]
  #print(hists)
  # print("Looking in file",runFilePath)
  for iHisto in histoPath.keys():
    #if histoPath[iHisto] not in histPathList:
    #  print("The desired histo path",histoPath[iHisto],"is not in the input file!")
    #  print(histPathList)
    #  sys.exit()
    histo[iHisto] = f.Get(histoPath[iHisto])
    histo[iHisto].SetTitle("%s (%s) - Run %d"%(histo[iHisto].GetTitle(),histoLegend[iHisto],args.runNumber))

  # Display the Tier0 merged histograms
  c = {}
  box = {}
  line = {}
  line2 = {}
  arrow = {}
  for iHisto in histoPath.keys():
    c[iHisto] = R.TCanvas(histoLegend[iHisto])
    if "logy" in canvasOption:
      c[iHisto].SetLogy(1)
    # draw line, arrows, box to highlight the suspicious region considered
    if histoType.startswith("2d"):
      R.gStyle.SetPalette(1)
      R.gStyle.SetOptStat("")
      print(iHisto, histo[iHisto].GetEntries())
      histo[iHisto].Draw("COLZ")
      if not b_wholeHisto:
        if histoType == "2d_etaPhiHotSpot":
          box[iHisto] = R.TBox(args.etaSpot-args.deltaSpot,args.phiSpot-args.deltaSpot,args.etaSpot+args.deltaSpot,args.phiSpot+args.deltaSpot)
        elif (histoType == "2d_xyHotSpot"):
          box[iHisto] = R.TBox(args.xSpot-args.deltaSpot,args.ySpot-args.deltaSpot,args.xSpot+args.deltaSpot,args.ySpot+args.deltaSpot)
        box[iHisto].SetLineColor(R.kRed+1)
        box[iHisto].SetLineWidth(3)
        box[iHisto].SetFillStyle(0)
        box[iHisto].Draw()

    elif histoType.startswith("1d"):
      maxH = histo[iHisto].GetMaximum()*1.2
      histo[iHisto].SetMaximum(maxH)
      if histoType == "1d_etaHotSpot" or histoType == "1d_phiHotSpot":
        minH = histo[iHisto].GetMinimum()*0.8
        histo[iHisto].SetMinimum(minH)
      histo[iHisto].Draw()

      if not b_wholeHisto:
        if histoType == "1d_etaHotSpot" or histoType == "1d_phiHotSpot":
          if maxH >0.:
            if histoType == "1d_etaHotSpot": 
              box[iHisto] = R.TBox(args.etaSpot-args.deltaSpot,minH,args.etaSpot+args.deltaSpot,maxH)
            if histoType == "1d_phiHotSpot": 
              box[iHisto] = R.TBox(args.phiSpot-args.deltaSpot,minH,args.phiSpot+args.deltaSpot,maxH)
            box[iHisto].SetLineColor(R.kRed+1)
            box[iHisto].SetLineWidth(3)
            box[iHisto].SetFillStyle(0)
            box[iHisto].Draw()
        elif histoType == "1d_integralAbove":
          line[iHisto] = R.TLine(args.integralAbove,0,args.integralAbove,maxH)
          line[iHisto].SetLineColor(R.kRed+1)
          line[iHisto].SetLineWidth(3)
          line[iHisto].Draw()
          arrow[iHisto] = R.TArrow(args.integralAbove,0.2*histo[iHisto].GetMaximum(),histo[iHisto].GetBinLowEdge(histo[iHisto].GetNbinsX()),0.2*histo[iHisto].GetMaximum(),0.02,">")
          arrow[iHisto].SetLineColor(R.kRed+1)
          arrow[iHisto].SetLineWidth(3)
          arrow[iHisto].Draw()

  #########################################################################
  # Extract the list of bins where to count.
  # Scans the window to find all bins that fall in the window
  # The regionBins is defined for each histogram allowing different binning
  regionBins = {}
  for iHisto in histoPath.keys():
    if b_wholeHisto:
      regionBins[iHisto] = []
      if ("2d" in histoType):  
        maxBin = (histo[iHisto].GetNbinsX()+2)*(histo[iHisto].GetNbinsY()+2)
      else:
        maxBin = (histo[iHisto].GetNbinsX()+2)
      for iBin in range(maxBin):
        regionBins[iHisto].append(iBin)
    else:
      nSteps = 1000
      subStep = 2*args.deltaSpot/nSteps
      regionBins[iHisto] = []
      if histoType == "2d_etaPhiHotSpot":
        for ix in range(nSteps):# Assume that eta is on x axis
          iEta = args.etaSpot - args.deltaSpot + ix * subStep 
          for iy in range (nSteps):
            iPhi = args.phiSpot - args.deltaSpot + iy * subStep
            tmpBin = histo[iHisto].FindBin(iEta,iPhi)
            if (tmpBin not in regionBins[iHisto]):
              regionBins[iHisto].append(tmpBin)
      elif histoType == "2d_xyHotSpot":
        for ix in range(nSteps):
          iX = args.xSpot - args.deltaSpot + ix * subStep 
          for iy in range (nSteps):
            iY = args.ySpot - args.deltaSpot + iy * subStep
            tmpBin = histo[iHisto].FindBin(iX,iY)
            if (tmpBin not in regionBins[iHisto]):
              regionBins[iHisto].append(tmpBin)
      elif histoType == "1d_etaHotSpot":
        for ix in range(nSteps):
          iEta = args.etaSpot - args.deltaSpot + ix * subStep
          tmpBin = histo[iHisto].FindBin(iEta)
          if (tmpBin not in regionBins[iHisto]):
              regionBins[iHisto].append(tmpBin)
      elif histoType == "1d_phiHotSpot":
        for ix in range(nSteps):
          iPhi = args.phiSpot - args.deltaSpot + ix * subStep
          tmpBin = histo[iHisto].FindBin(iPhi)
          if (tmpBin not in regionBins[iHisto]):
              regionBins[iHisto].append(tmpBin)
      elif (histoType == "1d_integralAbove"):
        for iBin in range(histo[iHisto].FindBin(args.integralAbove),histo[iHisto].GetNbinsX()):
          regionBins[iHisto].append(iBin)          

  # Extract all the unmerged files available with the LB range
  lbFilePathList = pathExtract.returnEosHistPathLB(args.runNumber,args.lowerLumiBlock,args.upperLumiBlock,args.stream,args.amiTag,args.tag)
  nbHitInHot = []

  nLB=2500
  nbHitInHot = {}
  for iHisto in histoPath.keys():
    nbHitInHot[iHisto] = [0.] * nLB
  lowerLB = 2500
  upperLB = 0
  lbCanvas = []
  histoLBNoisy = []
  fLB = {}

  print("I have found the merged HIST file %s"%(runFilePath))
  print("I have found %d unmerged HIST files"%(len(lbFilePathList)))
  print("The first one is root://eosatlas.cern.ch/%s"%(lbFilePathList[0]))
  print("The last one is root://eosatlas.cern.ch/%s"%(lbFilePathList[len(lbFilePathList)-1]))

  # Loop on all unmerged files

  for count,lbFile in enumerate(lbFilePathList):
    lbFilePath = "root://eosatlas.cern.ch/%s"%(lbFile).rstrip()
    # Extract lb from the filename and display it
    ilb = int((lbFile.split("_lb")[1]).split("._")[0])
    if (count%100 == 0):
      sys.stdout.write("\n I processed %d/%d files \n LBs:"%(count,len(lbFilePathList)))
    sys.stdout.write("%d "%(ilb))
    sys.stdout.flush()
    fLB[lbFile] = R.TFile.Open(lbFilePath)
    histoLB = {}
    for iHisto in histoPath.keys():
      histoLB[iHisto] = fLB[lbFile].Get(histoPath[iHisto])
      for iBin in regionBins[iHisto]:
        nbHitInHot[iHisto][ilb] = nbHitInHot[iHisto][ilb] + histoLB[iHisto].GetBinContent(iBin)
  #  sys.stdout.write("->%d "%(nbHitInHot[ilb]))

    fLB[lbFile].Close()

  #########################################################################
  # Loop on all histos and extract the luminosity block range to be displayed
  for iHisto in histoPath.keys():
    for ilb in range(len(nbHitInHot[iHisto])):
      if (nbHitInHot[iHisto][ilb]>=args.minInLB):
        if ilb<lowerLB : lowerLB = ilb
        if ilb>upperLB : upperLB = ilb  

  if (lowerLB == upperLB):
    lowerLB = lowerLB - 1
    upperLB = upperLB + 4

  print("")
  print(statement)

  maxNbInHot = 0
  totalInRegionRecomp = {} 
  totalInRegion = {} 
  suspiciousLBlist = []

  # Initialize the number of events in suspicious regions for both the merged
  # and the remerged file. 
  for iHisto in histoPath.keys():
    totalInRegionRecomp[iHisto] = 0
    totalInRegion[iHisto] = 0
  # Then count the number of events and check if equal
  # Also sort the LB to highlight most problematic LB
  sortedLB = {}

  for iHisto in histoPath.keys():
    print("======= ",histoLegend[iHisto])
    for iBin in regionBins[iHisto]:
      totalInRegion[iHisto] = totalInRegion[iHisto] + histo[iHisto].GetBinContent(iBin)

    sortedLB[iHisto] = [0] * nLB
    for i in range(nLB):
      totalInRegionRecomp[iHisto] = totalInRegionRecomp[iHisto] + nbHitInHot[iHisto][i]

      sortedLB[iHisto][i] = i
      if (nbHitInHot[iHisto][i]>=args.minInLB):
        suspiciousLBlist.append(i)
      if (nbHitInHot[iHisto][i]>maxNbInHot):
        maxNbInHot = nbHitInHot[iHisto][i]

    sortedLB[iHisto].sort(key=dict(zip(sortedLB[iHisto],nbHitInHot[iHisto])).get,reverse=True)
    for i in range(nLB):
      if nbHitInHot[iHisto][sortedLB[iHisto][i]]>=args.minInLB:
        if not b_ValueNotEntries:
          print("%d-LB: %d -> %d hits"%(i,sortedLB[iHisto][i],nbHitInHot[iHisto][sortedLB[iHisto][i]]))
        else:
          print("%d-LB: %d -> %.2f"%(i,sortedLB[iHisto][i],nbHitInHot[iHisto][sortedLB[iHisto][i]]))

    if not b_ValueNotEntries:
      print("In the whole run, there are %d entries"%(totalInRegion[iHisto]))
      if (totalInRegionRecomp[iHisto] != totalInRegion[iHisto]):
        print("To be compared with %d entries cumulated from unmerged files"%(totalInRegionRecomp[iHisto]))
        if (totalInRegionRecomp[iHisto] < totalInRegion[iHisto]):
          print("This is normal only if you restricted the LB range...")
        if (totalInRegionRecomp[iHisto] > totalInRegion[iHisto]):
          print("This can be also caused by multiple processing, try to filter with -a option")
          print("File path of the first file:",lbFilePathList[0])
    else:
      print("In the whole run, the value is %.2f"%(totalInRegion[iHisto]))

  #########################################################################
  ## Plot evolution vs LB
  #ncoll = math.ceil(len(histoPath.keys())/4)
  leg = R.TLegend(0.72, 0.90-0.05*len(histoPath.keys()),
                  0.98, 0.9 )
  #leg.SetNColumns(ncoll)
  leg.SetBorderSize(0)
  #leg.SetHeader("Run %d"%args.runNumber)

  if (upperLB>=lowerLB): # check that at least one noisy LB was found
    c0 = R.TCanvas()
    R.gStyle.SetOptStat("")
    if histoType != "2d_xyHotSpot":
      c0.SetLogy(1)
    c0.SetRightMargin(.3)
    h0Evol = {}
    first = True
    for iHisto in histoPath.keys():
      h0Evol[iHisto] = R.TH1I("h0Evol%s"%(iHisto),summaryTitle,upperLB-lowerLB+1,lowerLB-0.5,upperLB+0.5)
      h0Evol[iHisto].SetXTitle("LumiBlock (Only LB with >= %.0f entries)"%(args.minInLB))
      h0Evol[iHisto].SetLineColor(histoColor[iHisto])
      h0Evol[iHisto].SetMarkerColor(histoColor[iHisto])
      h0Evol[iHisto].SetMarkerStyle(20)
      leg.AddEntry(h0Evol[iHisto],"%s (%d entries in the run)"%(histoLegend[iHisto],totalInRegion[iHisto]))
      for i in range(lowerLB,upperLB+1):
        h0Evol[iHisto].Fill(i,nbHitInHot[iHisto][i])  
      if first:
        h0Evol[iHisto].Draw("P HIST")
        if histoType != "2d_xyHotSpot":
          h0Evol[iHisto].SetMinimum(args.minInLB-0.8)
          h0Evol[iHisto].SetMaximum(maxNbInHot*1.5)
        first = False
      else:
        h0Evol[iHisto].Draw("PSAME HIST")
    leg.Draw()
    c0.Update()

  if args.defectQuery:
    print("I am looking for LAr/Tile/Calo defects defined for the suspicious LB")
    from DQDefects import DefectsDB
    db = DefectsDB()
    defectList = [d for d in (db.defect_names | db.virtual_defect_names) if ((d.startswith("LAR") and "SEV" in d) or (d.startswith("TILE")) or (d.startswith("CALO")))]
    defects = db.retrieve((args.runNumber, 1), (args.runNumber+1, 0), defectList)
    for iDef in defects:
      associatedSuspicious = False
      for iLB in range(iDef.since.lumi,iDef.until.lumi):
        if iLB in suspiciousLBlist:
          associatedSuspicious = True
      if associatedSuspicious:
        if (iDef.since.lumi == iDef.until.lumi-1):
          print("%s: %d set by %s - %s"%(iDef.channel,iDef.since.lumi,iDef.user,iDef.comment))
        else:
          print("%s: %d->%d set by %s - %s"%(iDef.channel,iDef.since.lumi,iDef.until.lumi-1,iDef.user,iDef.comment))

  input("I am done...")


if __name__ == "__main__":
  R.gStyle.SetPalette(1)
  R.gStyle.SetOptStat("em")
  # command-line arguments
  parser = argparse.ArgumentParser(description='Process some integers.')
  parser.add_argument('-r','--run',type=int,dest='runNumber',default='267599',help="Run number",action='store')
  parser.add_argument('-ll','--lowerlb',type=int,dest='lowerLumiBlock',default='0',help="Lower lb",action='store')
  parser.add_argument('-ul','--upperlb',type=int,dest='upperLumiBlock',default='999999',help="Upper lb",action='store')
  parser.add_argument('-s','--stream',dest='stream',default='Main',help="Stream without prefix: express/CosmicCalo/Main/ZeroBias/MinBias",action='store')
  parser.add_argument('-t','--tag',dest='tag',default='',help="DAQ tag: data16_13TeV, data16_cos... By default retrieve it via atlasdqm",action='store')
  parser.add_argument('-a','--amiTag',dest='amiTag',default='f',help="First letter of AMI tag: x->express / f->bulk",action='store')
  parser.add_argument('-e','--eta',type=float,dest='etaSpot',default='-999.',help='Eta of hot spot',action='store')
  parser.add_argument('-p','--phi',type=float,dest='phiSpot',default='-999.',help='Phi of hot spot',action='store')
  parser.add_argument('-x','--x',type=float,dest='xSpot',default='-999.',help='X of hot spot',action='store')
  parser.add_argument('-y','--y',type=float,dest='ySpot',default='-999.',help='Y of hot spot',action='store')
  parser.add_argument('-ia','--integralAbove',type=float,dest='integralAbove',default='-999.',help='Lower bound of integral',action='store')
  parser.add_argument('-d','--delta',type=float,dest='deltaSpot',default='0.1',help='Distance to look around hot spot',action='store')
  parser.add_argument('-o','--object',dest='objectType',default='TopoClusters',help='2D OCCUPANCY: TopoClusters,EMTopoClusters,\n              EMTopoJets,TightFwdElectrons,Tau \n1D OCCUPANCY: EMTopoJets_eta,Tau_eta,Tau_phi \nINTEGRAL    : NumberTau,NumberTightFwdElectrons,NumberHLTJet',action='store')
  parser.add_argument('-m','--min',type=float,dest='minInLB',default='5',help='Min number of occurences in a LB',action='store')
  parser.add_argument('-g','--grl',dest='defectQuery',help='Look for Calo/LAr/Tile defects set in suspicious LBs',action='store_true')
  
  args = parser.parse_args()

  if args.tag == "":
    # Try to retrieve the data project tag via atlasdqm
    if (not os.path.isfile("atlasdqmpass.txt")):
      print("To retrieve the data project tag, you need to generate an atlasdqm key and store it in this directory as atlasdqmpass.txt (yourname:key)")
      print("To generate a key, go here : https://atlasdqm.cern.ch/dqauth/")
      print("You can also define by hand the data project tag wit hthe option -t")
      sys.exit()
    passfile = open("atlasdqmpass.txt")
    passwd = passfile.read().strip(); passfile.close()
    passurl = 'https://%s@atlasdqm.cern.ch'%passwd
    s = xmlrpclib.ServerProxy(passurl)
    run_spec = {'stream': 'physics_CosmicCalo', 'proc_ver': 1,'source': 'tier0', 'low_run': runNumber, 'high_run':runNumber}
    run_info= s.get_run_information(run_spec)
    if '%d'%runNumber not in run_info.keys() or len(run_info['%d'%runNumber])<2:
      print("Unable to retrieve the data project tag via atlasdqm... Please double check your atlasdqmpass.txt or define it by hand with -t option")
      sys.exit()
    args.tag = run_info['%d'%runNumber][1]

  
  # parser.print_help()
  main(args)
