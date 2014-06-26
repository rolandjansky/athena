#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#File tauLikelihood_rerunOnDPD.py
#by Martin.Flechl@cern.ch, Marcin.Wolter@cern.ch
#based on Noel's example
#
#usage: tauIDReader_LLH.py D3PD_file_name.root
import PyCintex
import ROOT
import os
import sys

#ifile="DPD.root"
ifile=sys.argv[1];
ofile="FOR_DPD.root"

inputType=0 #0 TauDPD, 1 small TauDPD, 2 Dugan's tiny D4PD


if (inputType==0):
  m_treename="tauPerf"
  m_vartypeF="VF"
  m_vartypeI="VI"
  m_authortype="VI"
if (inputType==1):
  m_treename="tauPerfSmall"
  m_vartypeF="VF"
  m_vartypeI="VI"
  m_authortype="VI"
if (inputType==2):
  m_treename="D4PD"
  m_vartypeF="F"
  m_vartypeI="I"
  m_authortype="F"

print "Setting up..."
output = ROOT.TFile.Open(ofile,"recreate")
### mw app = ROOT.TauID.TauIDReader(output,True)
app = ROOT.TauID.TauIDReader(True)
app.setOutput(output,"")

#Commented out variables are not needed when running the safeLLH
#Comment in to rerun the full LLH
variables = [
              ("author","tau_author",m_authortype),
              ("eta","tau_eta",m_vartypeF),
              ("numTrack","tau_numTrack",m_vartypeI),
              ("et","tau_Et",m_vartypeF),
              ("nPi0","tau_nPi0",m_vartypeI),
              ("NUM_PILEUP_AND_PRIMARY_VERTICES","evt_calcVars_numGoodVertices","I"), # number of good vertices
              ("vxp_n","vxp_n","I"), # not quite correct, these are all vertices
              ("vxp_nTracks","vxp_nTracks","VI"),
#              ("emRadius","tau_seedCalo_EMRadius",m_vartypeF), #0
#              ("isolFrac","tau_seedCalo_isolFrac",m_vartypeF), #1
#              ("stripWidth2","tau_seedCalo_stripWidth2",m_vartypeF), #2
#              ("nStrip","tau_seedCalo_nStrip",m_vartypeI), #3
#              ("etHad2etTracks","tau_calcVars_etHadSumPtTracks",m_vartypeF), #4
#              ("etEM2etTracks","tau_calcVars_etEMSumPtTracks",m_vartypeF), #5
#              ("etTracks2et","tau_calcVars_sumTrkPt",m_vartypeF), #6, code divides the quantity by et!!
#              ("emFractionCalib","tau_calcVars_emFracCalib",m_vartypeF), #7
#              ("emFractionAtEMScale","tau_calcVars_emFracCalib",m_vartypeF), #7 not quite correct, but not in current D3PD
#              ("etOverPtLeadTrk","tau_etOverPtLeadTrk",m_vartypeF), #8
#              ("dRmin","tau_calcVars_drMin",m_vartypeF), #9
              ("dRmax","tau_calcVars_drMax",m_vartypeF), #10
#              ("trkWidth2","tau_trkWidth2",m_vartypeF), #11
              ("massTrkSys","tau_massTrkSys",m_vartypeF), #12
#              ("nIsolTrk","tau_seedTrk_nIsolTrk",m_vartypeI), #13
#              ("MVisEflow","tau_m",m_vartypeF), #14
#              ("ipZ0sinThetaSigLeadTrk","tau_ipZ0SinThetaSigLeadTrk",m_vartypeF), #15
#              ("ipSigLeadLooseTrk","tau_ipSigLeadLooseTrk",m_vartypeF), #16
              ("trFlightPathSig","tau_trFlightPathSig",m_vartypeF), #17
              ("centFrac","tau_seedCalo_centFrac",m_vartypeF), #18
#              ("numEffClus","tau_calcVars_numEffTopoClusters",m_vartypeF), #19
              ("trkAvgDist","tau_seedCalo_trkAvgDist",m_vartypeF), #20
#              ("topoInvMass","tau_topoInvMass",m_vartypeF), #21
	      ("calRadius","tau_calcVars_calRadius",m_vartypeF), #22 
	      
	      ("lead2ClusterEOverAllClusterE","tau_calcVars_lead2ClusterEOverAllClusterE",m_vartypeF), #27 
	      ("numWideTrack","tau_seedCalo_wideTrk_n",m_vartypeI), #50 missing
	      
	      
            ]

for varName,branchName,type in variables:
    app.addVariable(varName,branchName,type)

# "CUTS" or "BDT", name of cuts/BDT file, number of responses
# (i.e. 3 for cuts (loose, medium, tight), 1 for BDT)
print "Finding PDF files"
filePDFtau = "pdfs_tau.root"
filePDFjet = "pdfs_jet.root"
fileLMTCuts = "LMTCutsLLH.root"
dataPathList = os.environ['DATAPATH'].split(os.pathsep)
dataPathList.insert(0, os.curdir)
from AthenaCommon.Utils.unixtools import FindFile
fileNameTau = FindFile(filePDFtau, dataPathList, os.R_OK )
fileNameJet = FindFile(filePDFjet, dataPathList, os.R_OK )
fileNameLMTCuts = FindFile(fileLMTCuts, dataPathList, os.R_OK )
if (not fileNameTau):
  print "Input PDF not found"
print "Tau PDF file name: ", fileNameTau
print "Jet PDF file name: ", fileNameJet
fileNames=fileNameTau+","+fileNameJet+","+fileNameLMTCuts
#print "String: ", fileNames
      
print "Booking Method: LLH"
### mw app.bookMethod("LLH","llhsafe",fileNames,4)
app.bookMethod(ROOT.TauID.Types.LLH,"llhsafe",fileNames,4)
#app.bookMethod("LLH","llhdef",fileNames,4)
#app.bookMethod("LLH","llhall",fileNames,4)

print "Opening input file"
input = ROOT.TFile.Open(ifile)
tree = input.Get(m_treename)

print "Obtaining Discriminant ",tree,m_treename," ",ifile
# mw llhTree = app.classify(tree,"taullh","")
llhTree = app.classify(tree,"taullh",True)

llhTree.AddFriend(m_treename,ifile)
output.cd()

print "Writing output",output.GetName()
llhTree.Write()

output.Close()
