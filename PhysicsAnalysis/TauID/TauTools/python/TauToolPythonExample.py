# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################################################################
# Name    : TauToolPythonExample.py
# Package : offline/PhysicsAnalysis/TauID/TauTools
# Author  : Jyothsna Komaragiri and Dugan O'Neil
#
# Description: 
#          This example shows how to use the tools present in TauTools along with
# accessing TauExamples class in ARA python. 
#
# Usage: 
# % python -i TauToolPythonExample.py
# >>> testExamples()
#
#
## Make the transient tree using ARA
import user  # look for .pythonrc.py for user init
import ROOT
import PyCintex
import AthenaROOTAccess.transientTree

# Put your AOD file here.
###### Do change this line to point to a proper AOD file.
f = ROOT.TFile.Open ('/cluster/data01/jyothsna/005188tid006904/AOD.pool_0_0.root')
assert f.IsOpen()

# Fill this in if you want to change the names of the transient branches.
branchNames = {}
#branchNames['ElectronCollection'] = 'ele'
#branchNames['PhotonCollection'] = 'gam'

tt = AthenaROOTAccess.transientTree.makeTree(f, branchNames = branchNames)
# tt is the transient tree "CollectionTree_trans" containing the (proxies) to
# all available transient data object in the file f.
# The original, persistent tree is declared as a friend
# of CollectionTree_trans, so that the transient tree will provide
# access to both transient data objects and to their persistent counterparts.

##############################################################################
#
# Example usage to look access TruthHandler class from ARA python
#
#
def testExamples() :

   ####################################
   # 0: Instantiate TauExamples class
   # Note the argument "0" in the constructor.
   # Needed by AraTool!!!!  
   ####################################
   from ROOT import TauExamples
   f=ROOT.gROOT.GetClass('TauExamples')
   tauexample = TauExamples()

   ####################################
   # 1: Set/Get the container keys to fetch from transient tree
   ####################################
   tauexample.setTruthContainer("SpclMC")
   tauexample.setTauJetContainer("TauRecContainer")

   mcbr  = tt.GetBranch(tauexample.getTruthContainer()) 
   taubr = tt.GetBranch(tauexample.getTauJetContainer())

   mcpartTES = tt.SpclMC  
   tauRecTES = tt.TauRecContainer                       

   ####################################
   # 2: Create histograms to be filled in
   ####################################
   tauexample.CreateHistos()

   ####################################
   # 3: Loop over the events and call the efficiency/resolution 
   # methods for each event
   ####################################
   for iEvt in range(tt.GetEntries()): 
      mcbr.GetEntry(iEvt)
      taubr.GetEntry(iEvt)
      tauexample.getTauRecProngEfficiency(tauRecTES, mcpartTES)
      tauexample.getTauRecResolution(tauRecTES, mcpartTES)

   ####################################
   # 4: Finalize and dump the histograms in ROOT file
   ####################################
   tauexample.finalizeProng()
   tauexample.finalizeResolution()
#
# Stores all the histos in a single ROOT file
#   exrootfile = ROOT.TFile("TauToolExamples.root","RECREATE")
#   tauexample.StoreHistos(exrootfile)
#
###### uncomment the below line to to execute the above method
testExamples()
#
##############################################################################
#
# Example usage to look access TruthHandler class from ARA python
# This example returns container of having hadronically decaying taus 
# given a truth paticle container
#
def testTruth() :
   from ROOT import TTree
   from ROOT import TruthHandler
   f=ROOT.gROOT.GetClass('TruthHandler')
   truthHandler = TruthHandler()
   for iEvt in range(10):
      tt.GetEntry(iEvt)
      hadTau = truthHandler.getHadronicTruth(tt.SpclMC)

   truthHandler.finalize()
#
# uncomment the below line to to execute the above method
#testTruth()
#
##############################################################################
      
