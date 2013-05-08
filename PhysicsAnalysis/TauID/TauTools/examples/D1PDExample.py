# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################################################################
# Name: TauTools/examples/D1PDExample.py
# Author: Jyothsna Rani KOMARAGIRI
# Created: 19th October 2008
#
# Description:
#     This is a ARA python example to look at primary DPDs.
# This looks at calo+track, calo and track seeded reco taus and applies LL/EfficNN cuts.
# Also finds out how many of these taus are matched to Cone4H1TopoJets.
#
# Usage:
# Step 1) checkout TauTools and compile
# %cmt co -r TauTools-00-03-01 PhysicsAnalysis/TauID/TauTools
#
# Step 2) Grab this example python file from:
# http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/PhysicsAnalysis/TauID/TauTools/examples/
#
# Step 3) change the input DPD and run it using:
# %python D1PDExample.py
#
# This example is tested in 14.2.22/23 ATLAS release
#
##############################################################################

import user  # look for .pythonrc.py for user init
import ROOT
import PyCintex
import AthenaROOTAccess.transientTree

branchNames = {}
#### To read a single POOL file
f = ROOT.TFile.Open ('/afs/cern.ch/user/b/blumen/maxidisk/public/TauFiles/Valid/5188/PrimaryDPD_5188_ThinMainEGamma.pool.root')
assert f.IsOpen()
tt = AthenaROOTAccess.transientTree.makeTree(f, branchNames = branchNames)

#### To read list of POOL files
#CollectionTree = ROOT.AthenaROOTAccess.TChainROOTAccess('CollectionTree')
#for myfile in open("list.txt","r"):
#    CollectionTree.Add(myfile.strip())
#tt = AthenaROOTAccess.transientTree.makeTree(CollectionTree, branchNames = branchNames)
##############################################################################

def example():

    #############
    # pT cut value, default 10 GeV
    taupTCut = 10
    # DeltaR cut
    dRCut = 0.2
    
    nevents = tt.GetEntries()
    #nevents = 500

    # Load the system of units to get GeV etc
    import AthenaCommon.SystemOfUnits as Units

    #############
    # Load RecoHandler from TauTools
    from ROOT import RecoHandler
    f=ROOT.gROOT.GetClass('RecoHandler')
    recoHandler = RecoHandler()

    # Load MatchingTools from TauTools
    from ROOT import MatchingTools
    f=ROOT.gROOT.GetClass('MatchingTools')
    matchtool = MatchingTools(dRCut, 0.0)

    #############
    # Get the containers to be retrieved from transient Tree
    ## Tau Jet Container
    Tau = tt.TauRecContainer
    taubr = tt.GetBranch("TauRecContainer")

    ## Reco Jet container
    Jet = tt.Cone4H1TopoJets
    jetbr = tt.GetBranch("Cone4H1TopoJets")


    #############
    # Define the reco taus depending on the seeds
    TAU_NONE=0
    TAU_REC=1
    TAU_1P3P=2
    TAU_BOTH=3

    #############
    # Define the histograms
    # Tau multiplicity
    tauAllN    = ROOT.TH1F('tauAllN', 'tauAll candidates',  11, -0.5, 10.5)
    tauBothN   = ROOT.TH1F('tauBothN','calo+track seeded candidates', 11, -0.5, 10.5)
    tauRecN    = ROOT.TH1F('tauRecN', 'calo-seeded candidates',  11, -0.5, 10.5)
    tau1P3PN   = ROOT.TH1F('tau1P3PN','track-seeded candidates', 11, -0.5, 10.5)
    tauBothIDN = ROOT.TH1F('tauBothIDN','selected calo+track seeded candidates', 11, -0.5, 10.5)
    tauRecIDN  = ROOT.TH1F('tauRecIDN', 'selected calo-seeded candidates',  11, -0.5, 10.5)
    tau1P3PIDN = ROOT.TH1F('tau1P3PIDN','selected track-seeded candidates', 11, -0.5, 10.5)
    tauBothJetN = ROOT.TH1F('tauBothJetN','calo+track seeded candidates', 11, -0.5, 10.5)
    tauRecJetN  = ROOT.TH1F('tauRecJetN', 'calo-seeded candidates',  11, -0.5, 10.5)
    tau1P3PJetN = ROOT.TH1F('tau1P3PJetN','track-seeded candidates', 11, -0.5, 10.5)

    # Tau ET distributions
    tauBothET       = ROOT.TH1F('tauBothET', 'ET for Both seed (ET)',    100, 0.0, 100.0)
    tauBothETFlow   = ROOT.TH1F('tauBothETFlow', 'ET for Both seed (etEflow)',    100, 0.0, 100.0)
    tauRecET        = ROOT.TH1F('tauRecET',  'ET for calo-seed',  100, 0.0, 100.0)
    tau1P3PET       = ROOT.TH1F('tau1P3PET', 'ET for track-seed', 100, 0.0, 100.0)
    tauBothIDET     = ROOT.TH1F('tauBothIDET', 'ET for selected Both seed (ET)',    100, 0.0, 100.0)
    tauRecIDET      = ROOT.TH1F('tauRecIDET',  'ET for selected calo-seed',  100, 0.0, 100.0)
    tau1P3PIDET     = ROOT.TH1F('tau1P3PIDET', 'ET for selected track-seed', 100, 0.0, 100.0)


    # Tau Discriminant distributions
    tauBothLL  = ROOT.TH1F('tauBothLL', 'Likelihood for both seed', 50, -50, 50)
    tauRecLL   = ROOT.TH1F('tauRecLL', 'Likelihood for calo-seed', 50, -50, 50)
    tau1P3PNN  = ROOT.TH1F('tau1P3PNN', 'EffiNN for track-seed', 50, 0., 1.0)

    #############
    # Loop over events in the transient tree
    for iEvt in range(nevents):
        # Get only needed branches from the tree
        taubr.GetEntry(iEvt)
        jetbr.GetEntry(iEvt)
        
        #print "Event no: " , iEvt+1, " no. of taus: ", Tau.size()

        # Multiplicity counters
        Nall  = Tau.size()      
        NBoth = 0
        NRec  = 0
        N1p3p = 0
        NBothID = 0
        NRecID  = 0
        N1p3pID = 0
        NBothJet = 0
        NRecJet  = 0
        N1p3pJet = 0

        # Loop over taus
        for (i,tau) in enumerate(Tau):
            TJP = ROOT.TauJetParameters
            pid = tau.tauID()
            
            ### Both seeded tau
            if(recoHandler.TauType(tau) == TAU_BOTH):

                et = tau.et() / Units.GeV
                if(et < taupTCut):  continue
                tauBothLL.Fill(pid.discriminant(TJP.Likelihood))

                tauBothET.Fill(et)
                tau1P3PDetails = tau.detail('Analysis::Tau1P3PDetails')
                etflow = tau1P3PDetails.etEflow()/Units.GeV
                tauBothETFlow.Fill(etflow)
                NBoth += 1

                # Apply cut LL > 4.0, also ask for good charge
                if(recoHandler.selectTauRecJet(tau,5,4.0,True)):
                    tauBothIDET.Fill(et)
                    NBothID += 1
                    #print ">>Both tau has LL > 4"

                # Find out how often the reco tau is close to Cone4H1TopoJet
                if( matchtool.matchJet(tau, Jet)):
                    NBothJet += 1
                    #print "Both seeded tau is matched to jet"

 
            ### Calo seeded tau
            if(recoHandler.TauType(tau) == TAU_REC):

                et = tau.et() / Units.GeV
                if(et < taupTCut):  continue
                tauRecLL.Fill(pid.discriminant(TJP.Likelihood))

                tauRecET.Fill(et)
                NRec += 1

                # Apply cut LL > 4.0, also ask for good charge
                if(recoHandler.selectTauRecJet(tau,5,4.0,True)):
                    tauRecIDET.Fill(et)
                    NRecID += 1
                    #print ">>Calo tau has LL > 4"

                # Find out how often the reco tau is close to Cone4H1TopoJet
                if(matchtool.matchJet(tau, Jet)):
                    NRecJet += 1
                    #print "Calo seeded tau is matched to jet"


            ### Track seeded tau
            if(recoHandler.TauType(tau) == TAU_1P3P):
                
                tau1P3PDetails = tau.detail('Analysis::Tau1P3PDetails')
                et = tau1P3PDetails.etEflow() / Units.GeV
                if(et < taupTCut):  continue
                tau1P3PNN.Fill(pid.discriminant(TJP.EfficNN))

                tau1P3PET.Fill(et)
                N1p3p += 1

                # Apply cut EfficNN > 0.3, also ask for good charge, ele and muon veto
                if(recoHandler.selectTau1P3PJet(tau,3,0.3,True, True, True)):
                    tau1P3PIDET.Fill(et)
                    N1p3pID += 1
                    #print ">>Track tau has EfficNN > 0.3"

                # Find out how often the reco tau is close to Cone4H1TopoJet                   
                if(matchtool.matchJet(tau, Jet)):
                    N1p3pJet += 1
                    #print "Track seeded tau is matched to jet"

            ### Reco tau
            if(recoHandler.TauType(tau) == TAU_NONE):
                print "Does this happen???"


        #############
        tauAllN.Fill(float(Nall))
        # Fill all taus
        tauBothN.Fill(float(NBoth))
        tauRecN.Fill(float(NRec))
        tau1P3PN.Fill(float(N1p3p))
        # Fill selected taus
        tauBothIDN.Fill(float(NBothID))
        tauRecIDN.Fill(float(NRecID))
        tau1P3PIDN.Fill(float(N1p3pID))
        # Fill taus matched to jets
        tauBothJetN.Fill(float(NBothJet))
        tauRecJetN.Fill(float(NRecJet))
        tau1P3PJetN.Fill(float(N1p3pJet))
      
        if iEvt % 500 == 0:
            print "+++iEvt: ", iEvt+1, " no. of taus: ", Tau.size()," Both seeded: ", NBoth, " bothID: ", NBothID, " Calo seeded: ", NRec, " RecID: ", NRecID, " Track seeded: ", N1p3p, " 1p3pID: ", N1p3pID
            
    #############
    histfile = ROOT.TFile("D1PDExample.root","RECREATE")
    histfile.cd()

    tauAllN.Write()
    tauBothN.Write()
    tauRecN.Write()
    tau1P3PN.Write()
    tauBothIDN.Write()
    tauRecIDN.Write()
    tau1P3PIDN.Write()
    tauRecJetN.Write()
    tau1P3PJetN.Write()
    tauBothJetN.Write()
    
    tauRecET.Write()
    tau1P3PET.Write()
    tauBothET.Write()
    tauBothETFlow.Write()
    tauRecIDET.Write()
    tau1P3PIDET.Write()
    tauBothIDET.Write()

    tauBothLL.Write()
    tauRecLL.Write()
    tau1P3PNN.Write()
    
    histfile.Write()
    histfile.Close()

###########
#
example()
#
##############################################################################
