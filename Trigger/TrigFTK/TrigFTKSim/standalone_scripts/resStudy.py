# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import TFile, TH1F
import ROOT, math

from optparse import OptionParser
p = OptionParser()
p.add_option('--input',  type = 'string', default = "resStudy.root",   dest = 'inFile', help = 'intput File' )
p.add_option('--output', type = 'string', default = "resStudyHists.root", dest = 'outFile', help = 'output File' )
p.add_option('--etaMax', type = 'float', default = 2.5, dest = 'maxEtaCut', help = '|eta| cut' )
p.add_option('--maxEvents', type = 'int', default = -1, dest = 'maxEvents', help = 'maxEvents' )
p.add_option('--treename', type = 'string', default = "ResStudy", dest = 'treename', help = 'name of tree to be analyzed' )

#
#  Various Different Studies
#
p.add_option('--doPtStudy',  action='store_true', default = False, dest = 'doPtStudy', help = 'Study the Pt dependence' )
p.add_option('--doEtaStudy', action='store_true', default = False, dest = 'doEtaStudy',help = 'Study the Eta dependence' )
p.add_option('--doCovStudy', action='store_true', default = False, dest = 'doCovStudy',help = 'Study the covariance' )

##do offline or ftk for different formats
p.add_option('--noOff',action='store_false',default=True,dest='doOff',help='Do Not study offline')
p.add_option('--noFTK',action='store_false',default=True,dest='doFTK',help='Do Not study offline')


#
#  Get the user input
#
(o,a) = p.parse_args()

#
#  Read the input file
#
inFile = TFile(o.inFile,"READ")
inFile.ls()
resStudyTree = ROOT.TChain(o.treename)
resStudyTree.Add(o.inFile)
resStudyTree.GetEntries()


#
#  Parameter Ranges
#

ptMin  = -6e-4
ptMax  =  6e-4
phiMin = -3.2
phiMax = 3.2
d0Min  = -3
d0Max  = 3
etaMin  = -3.4
etaMax  = 3.4
z0Min  = -110
z0Max  = 110

#
#  Resolution Ranges
#

resPtMin  = -5e-5
resPtMax  =  5e-5
resPhiMin = -0.01
resPhiMax = 0.01
resD0Min  = -0.5
resD0Max  = 0.5
resEtaMin = -0.01
resEtaMax = 0.01
resZ0Min  = -1
resZ0Max  =  1

resZ0Min  = -2
resZ0Max  =  2
resZ0STMin  = -0.75  #use .3 for the mu = 100 samples
resZ0STMax  =  0.75


#
#  Setup the histograms to Fill
#

from TPHists import TPHists
FTKHists   = TPHists("FTK"  )
TruthHists = TPHists("Truth")
TruthWFTK  = TPHists("TruthWFTK")
TruthWOff  = TPHists("TruthWOff")
OffHists   = TPHists("Off")

from TPHists import ResVsTPs
ResPhiHists    = ResVsTPs("phi","#phi",resPhiMin, resPhiMax)
ResZ0Hists     = ResVsTPs("z0", "#z0", resZ0Min,  resZ0Max )
ResZ0STHists     = ResVsTPs("z0st", "#z0 sin(#theta)", resZ0STMin,  resZ0STMax )
ResD0Hists     = ResVsTPs("d0", "#d0", resD0Min,  resD0Max )
ResEtaHists    = ResVsTPs("eta","#eta",resEtaMin, resEtaMax)
ResIptHists    = ResVsTPs("Ipt","#Ipt",resPtMin,  resPtMax )

OffResPhiHists = ResVsTPs("off_phi","#phi",resPhiMin, resPhiMax)
OffResZ0Hists  = ResVsTPs("off_z0", "#z0", resZ0Min,  resZ0Max )
OffResZ0STHists  = ResVsTPs("off_z0st", "#z0 sin(#theta)", resZ0STMin,  resZ0STMax )
OffResD0Hists  = ResVsTPs("off_d0", "#d0", resD0Min,  resD0Max )
OffResEtaHists = ResVsTPs("off_eta","#eta",resEtaMin, resEtaMax)
OffResIptHists = ResVsTPs("off_Ipt","#Ipt",resPtMin,  resPtMax )

#
#  For the covariances
#
if o.doCovStudy:
    from CovStudy import CovStudy
    ResPhiCovHists    = CovStudy("phi",    "#phi", resPhiMin, resPhiMax)
    ResZ0CovHists     = CovStudy("z0",     "#z0",  resZ0Min,  resZ0Max )
    ResZ0STCovHists     = CovStudy("z0st",     "#z0 sin(#theta)",  resZ0STMin,  resZ0STMax )
    ResD0CovHists     = CovStudy("d0",     "#d0",  resD0Min,  resD0Max )
    ResEtaCovHists    = CovStudy("eta",    "#eta", resEtaMin, resEtaMax)
    ResIptCovHists    = CovStudy("Ipt",    "#Ipt", resPtMin,  resPtMax )
                                                   
    OffResPhiCovHists = CovStudy("off_phi","#phi", resPhiMin, resPhiMax)
    OffResZ0CovHists  = CovStudy("off_z0", "#z0",  resZ0Min,  resZ0Max )
    OffResZ0STCovHists  = CovStudy("off_z0st", "#z0 sin(#theta)",  resZ0STMin,  resZ0STMax )
    OffResD0CovHists  = CovStudy("off_d0", "#d0",  resD0Min,  resD0Max )
    OffResEtaCovHists = CovStudy("off_eta","#eta", resEtaMin, resEtaMax)
    OffResIptCovHists = CovStudy("off_Ipt","#Ipt", resPtMin,  resPtMax )

#
#  For the Pt dependence
#
if o.doPtStudy:
    from PtStudy import PtStudy
    PtStudyPhiHist  = PtStudy("phi","#phi",resPhiMin, resPhiMax)
    PtStudyZ0Hists  = PtStudy("z0", "#z0", resZ0Min,  resZ0Max )
    PtStudyZ0STHists  = PtStudy("z0st", "#z0 sin(#theta)", resZ0STMin,  resZ0STMax )
    PtStudyD0Hists  = PtStudy("d0", "#d0", resD0Min,  resD0Max )
    PtStudyEtaHists = PtStudy("eta","#eta",resEtaMin, resEtaMax)
    PtStudyIptHists = PtStudy("Ipt","#Ipt",resPtMin,  resPtMax )
    
    OffPtStudyPhiHist  = PtStudy("off_phi","#phi",resPhiMin, resPhiMax)
    OffPtStudyZ0Hists  = PtStudy("off_z0", "#z0", resZ0Min,  resZ0Max )
    OffPtStudyZ0STHists  = PtStudy("off_z0st", "#z0 sin(#theta)", resZ0STMin,  resZ0STMax )
    OffPtStudyD0Hists  = PtStudy("off_d0", "#d0", resD0Min,  resD0Max )
    OffPtStudyEtaHists = PtStudy("off_eta","#eta",resEtaMin, resEtaMax)
    OffPtStudyIptHists = PtStudy("off_Ipt","#Ipt",resPtMin,  resPtMax )

#
#  For the Eta dependence
#
if o.doEtaStudy:
    from EtaStudy import EtaStudy
    EtaStudyPhiHist  = EtaStudy("phi","#phi",resPhiMin, resPhiMax)
    EtaStudyZ0Hists  = EtaStudy("z0", "#z0", resZ0Min,  resZ0Max )
    EtaStudyZ0STHists  = EtaStudy("z0st", "#z0 sin(#theta)", resZ0STMin,  resZ0STMax )
    EtaStudyD0Hists  = EtaStudy("d0", "#d0", resD0Min,  resD0Max )
    EtaStudyEtaHists = EtaStudy("eta","#eta",resEtaMin, resEtaMax)
    EtaStudyIptHists = EtaStudy("Ipt","#Ipt",resPtMin,  resPtMax )
    
    OffEtaStudyPhiHist  = EtaStudy("off_phi","#phi",resPhiMin, resPhiMax)
    OffEtaStudyZ0Hists  = EtaStudy("off_z0", "#z0", resZ0Min,  resZ0Max )
    OffEtaStudyZ0STHists  = EtaStudy("off_z0st", "#z0 sin(#theta)", resZ0STMin,  resZ0STMax )
    OffEtaStudyD0Hists  = EtaStudy("off_d0", "#d0", resD0Min,  resD0Max )
    OffEtaStudyEtaHists = EtaStudy("off_eta","#eta",resEtaMin, resEtaMax)
    OffEtaStudyIptHists = EtaStudy("off_Ipt","#Ipt",resPtMin,  resPtMax )


#
#  Actually Fill the histograms
#

eventCount = 0

for event in resStudyTree:

    eventCount += 1
    if eventCount % 10000 == 0: print "processed...",eventCount,"events" 

    if o.maxEvents > 0 and eventCount > o.maxEvents:
        break

    if len(event.Truth_eta) ==0:
        continue

    if len(event.Truth_eta) > 1:
        continue

    if abs(event.Truth_eta[0]) > o.maxEtaCut:
        continue

#    if event.Truth_Ipt[0]/abs(event.Truth_Ipt[0]) != o.chargeCut and o.chargeCut != 0:
#        continue


    if o.doFTK and len(event.FTK_z0)> 0:
        if abs(1./(2000.*event.FTK_Ipt[0])) > 1.0:
            
            ftkz0st=event.FTK_z0[0] * math.sin(2.0*math.atan(math.exp(-1.0*event.FTK_eta[0])))
            FTKHists  .Fill(event.FTK_Ipt[0],
                            event.FTK_phi[0],
                            event.FTK_d0[0],
                            event.FTK_eta[0],
                            event.FTK_z0[0],
                            ftkz0st)

    truthz0st=event.Truth_z0[0] * math.sin(2.0*math.atan(math.exp(-1.0*event.Truth_eta[0])))
    TruthHists.Fill(event.Truth_Ipt[0],
                    event.Truth_phi[0],
                    event.Truth_d0[0],
                    event.Truth_eta[0],
                    event.Truth_z0[0],
                    truthz0st)



    if o.doOff and len(event.Off_z0) > 0:
        if abs(1./(2000.*event.Off_Ipt[0])) > 1.0:

            offz0st=event.Off_z0[0] * math.sin(2.0*math.atan(math.exp(-1.0*event.Off_eta[0])))
            OffHists.Fill(  event.Off_Ipt[0]/1000,
                            event.Off_phi[0],
                            event.Off_d0[0],
                            event.Off_eta[0],
                            event.Off_z0[0],
                            offz0st)


    #
    # Calculate the Res
    #

    if o.doFTK:
        if len(event.FTK_z0) > 0:
            if abs(1./(2000.*event.FTK_Ipt[0])) > 1.0:
                
                TruthWFTK.Fill(event.Truth_Ipt[0],
                               event.Truth_phi[0],
                               event.Truth_d0[0],
                               event.Truth_eta[0],
                               event.Truth_z0[0],
                               truthz0st)


                resZ0  = (event.FTK_z0 [0]  - event.Truth_z0 [0])
                resIpt = (event.FTK_Ipt[0]  - event.Truth_Ipt[0])                  
                resPhi = (event.FTK_phi[0]  - event.Truth_phi[0])                  
                resD0  = (event.FTK_d0 [0]  - event.Truth_d0 [0])                  
                resEta = (event.FTK_eta[0]  - event.Truth_eta[0])                  
                resZ0st = (ftkz0st  - truthz0st)                  
            
                Z0  = event.Truth_z0 [0]
                Ipt = event.Truth_Ipt[0]                  
                phi = event.Truth_phi[0]                  
                D0  = event.Truth_d0 [0]                  
                eta = event.Truth_eta[0]                  
                pt  = 0.5/abs(Ipt)

                hasBL = (event.FTK_BitMask[0] & 1<<0)
            
                ResPhiHists          .Fill(resPhi, Ipt, eta, phi, hasBL)
                ResD0Hists           .Fill(resD0,  Ipt, eta, phi, hasBL)
                ResZ0Hists           .Fill(resZ0,  Ipt, eta, phi, hasBL)
                ResEtaHists          .Fill(resEta, Ipt, eta, phi, hasBL)
                ResIptHists          .Fill(resIpt, Ipt, eta, phi, hasBL)
                ResZ0STHists          .Fill(resZ0st, Ipt, abs(eta), phi, hasBL)


                if o.doCovStudy:
                    if pt > 5*1000 and abs(eta) < 1.4:
            
                        ResPhiCovHists          .Fill(resPhi, resIpt, resEta, resPhi, resD0,  resZ0)
                        ResD0CovHists           .Fill(resD0,  resIpt, resEta, resPhi, resD0,  resZ0)
                        ResZ0CovHists           .Fill(resZ0,  resIpt, resEta, resPhi, resD0,  resZ0)
                        ResEtaCovHists          .Fill(resEta, resIpt, resEta, resPhi, resD0,  resZ0)
                        ResIptCovHists          .Fill(resIpt, resIpt, resEta, resPhi, resD0,  resZ0)
                        ResZ0stCovHists          .Fill(resZ0st, resIpt, resEta, resPhi, resD0,  resZ0)
                        
                if o.doPtStudy:
                    PtStudyPhiHist       .Fill(resPhi, Ipt, eta, hasBL)
                    PtStudyZ0Hists       .Fill(resZ0,  Ipt, eta, hasBL)
                    PtStudyZ0STHists     .Fill(resZ0st,  Ipt, eta, hasBL)
                    PtStudyD0Hists       .Fill(resD0,  Ipt, eta, hasBL)
                    PtStudyEtaHists      .Fill(resEta, Ipt, eta, hasBL)
                    PtStudyIptHists      .Fill(resIpt, Ipt, eta, hasBL)
                

                if o.doEtaStudy:
                    EtaStudyPhiHist       .Fill(resPhi, Ipt, eta, hasBL)
                    EtaStudyZ0Hists       .Fill(resZ0,  Ipt, eta, hasBL)
                    EtaStudyZ0STHists     .Fill(resZ0st,  Ipt, abs(eta), hasBL)
                    EtaStudyD0Hists       .Fill(resD0,  Ipt, eta, hasBL)
                    EtaStudyEtaHists      .Fill(resEta, Ipt, eta, hasBL)
                    EtaStudyIptHists      .Fill(resIpt, Ipt, eta, hasBL)

    if o.doOff:
        if len(event.Off_z0) > 0:
            if event.Off_npix[0] < 1 or abs(1./(2000.*event.Off_Ipt[0])) < 1.0:
                continue
            
            TruthWOff.Fill(event.Truth_Ipt[0],
                           event.Truth_phi[0],
                           event.Truth_d0[0],
                           event.Truth_eta[0],
                           event.Truth_z0[0],
                           truthz0st)

            resOffZ0st = (offz0st  - truthz0st)                  
            resOffZ0  = (event.Off_z0 [0]  - event.Truth_z0 [0])
            resOffIpt = (event.Off_Ipt[0] - event.Truth_Ipt[0])                  
            resOffPhi = (event.Off_phi[0]  - event.Truth_phi[0])                  
            resOffD0  = (event.Off_d0 [0]  - event.Truth_d0 [0])                  
            resOffEta = (event.Off_eta[0]  - event.Truth_eta[0])                  
        
            Z0  = event.Truth_z0 [0]
            Ipt = event.Truth_Ipt[0]                  
            phi = event.Truth_phi[0]                  
            D0  = event.Truth_d0 [0]                  
            eta = event.Truth_eta[0]                  
            pt  = 0.5/abs(Ipt)
            

            hasBL = event.Off_hasIBL[0]
        
            OffResPhiHists          .Fill(resOffPhi, Ipt, eta, phi, hasBL)
            OffResD0Hists           .Fill(resOffD0,  Ipt, eta, phi, hasBL)
            OffResZ0Hists           .Fill(resOffZ0,  Ipt, eta, phi, hasBL)
            OffResEtaHists          .Fill(resOffEta, Ipt, eta, phi, hasBL)
            OffResIptHists          .Fill(resOffIpt, Ipt, eta, phi, hasBL)
            OffResZ0STHists         .Fill(resOffZ0st, Ipt, abs(eta), phi, hasBL)


            if o.doCovStudy:
                #        if pt > 0*1000:
                if pt > 5*1000 and abs(eta) < 1.4:
                    OffResPhiCovHists       .Fill(resOffPhi, resOffIpt, resOffEta, resOffPhi, resOffD0,  resOffZ0)
                    OffResD0CovHists        .Fill(resOffD0,  resOffIpt, resOffEta, resOffPhi, resOffD0,  resOffZ0)
                    OffResZ0CovHists        .Fill(resOffZ0,  resOffIpt, resOffEta, resOffPhi, resOffD0,  resOffZ0)
                    OffResEtaCovHists       .Fill(resOffEta, resOffIpt, resOffEta, resOffPhi, resOffD0,  resOffZ0)
                    OffResIptCovHists       .Fill(resOffIpt, resOffIpt, resOffEta, resOffPhi, resOffD0,  resOffZ0)
                    OffResZ0STCovHists       .Fill(resOffZ0st, resOffIpt, resOffEta, resOffPhi, resOffD0,  resOffZ0)


            if o.doPtStudy:
                OffPtStudyPhiHist       .Fill(resOffPhi, Ipt, eta, hasBL)
                OffPtStudyZ0Hists       .Fill(resOffZ0,  Ipt, eta, hasBL)
                OffPtStudyD0Hists       .Fill(resOffD0,  Ipt, eta, hasBL)
                OffPtStudyEtaHists      .Fill(resOffEta, Ipt, eta, hasBL)
                OffPtStudyIptHists      .Fill(resOffIpt, Ipt, eta, hasBL)
                OffPtStudyZ0STHists      .Fill(resOffZ0st, Ipt, eta, hasBL)


            if o.doEtaStudy:
                OffEtaStudyPhiHist       .Fill(resOffPhi, Ipt, eta, hasBL)
                OffEtaStudyZ0Hists       .Fill(resOffZ0,  Ipt, eta, hasBL)
                OffEtaStudyD0Hists       .Fill(resOffD0,  Ipt, eta, hasBL)
                OffEtaStudyEtaHists      .Fill(resOffEta, Ipt, eta, hasBL)
                OffEtaStudyIptHists      .Fill(resOffIpt, Ipt, eta, hasBL)
                OffEtaStudyZ0STHists      .Fill(resOffZ0st, Ipt, abs(eta), hasBL)


    
#
# Write the output
#    

outFile = TFile(o.outFile,"RECREATE")
outFile.cd()

if o.doFTK:
    FTKHists  .Write()
    TruthWFTK.Write()
if o.doOff:
    TruthWOff.Write()
    OffHists.Write()

TruthHists.Write()

if o.doFTK:
    ResPhiHists   .Write()
    ResD0Hists    .Write()
    ResZ0Hists    .Write()
    ResZ0STHists    .Write()
    ResEtaHists   .Write()
    ResIptHists   .Write()

if o.doOff:
    OffResPhiHists   .Write()
    OffResD0Hists    .Write()
    OffResZ0Hists    .Write()
    OffResZ0STHists    .Write()
    OffResEtaHists   .Write()
    OffResIptHists   .Write()

if o.doCovStudy:
    if o.doFTK:
        ResPhiCovHists   .Write()
        ResD0CovHists    .Write()
        ResZ0CovHists    .Write()
        ResZ0STCovHists    .Write()
        ResEtaCovHists   .Write()
        ResIptCovHists   .Write()

    if o.doOff:
        OffResPhiCovHists   .Write()
        OffResD0CovHists    .Write()
        OffResZ0CovHists    .Write()
        OffResZ0STCovHists    .Write()
        OffResEtaCovHists   .Write()
        OffResIptCovHists   .Write()


if o.doPtStudy:
    print "Writing out pt study"

    if o.doFTK:
        PtStudyPhiHist  .Write()
        PtStudyZ0Hists  .Write()
        PtStudyZ0STHists  .Write()
        PtStudyD0Hists  .Write()
        PtStudyEtaHists .Write()
        PtStudyIptHists .Write()

    if o.doOff:
        OffPtStudyPhiHist  .Write()
        OffPtStudyZ0Hists  .Write()
        OffPtStudyZ0STHists  .Write()
        OffPtStudyD0Hists  .Write()
        OffPtStudyEtaHists .Write()
        OffPtStudyIptHists .Write()

if o.doEtaStudy:
    print "Writing out eta study"

    if o.doFTK:
        EtaStudyPhiHist  .Write()
        EtaStudyZ0Hists  .Write()
        EtaStudyZ0STHists  .Write()
        EtaStudyD0Hists  .Write()
        EtaStudyEtaHists .Write()
        EtaStudyIptHists .Write()

    if o.doOff:
        OffEtaStudyPhiHist  .Write()
        OffEtaStudyZ0Hists  .Write()
        OffEtaStudyZ0STHists  .Write()
        OffEtaStudyD0Hists  .Write()
        OffEtaStudyEtaHists .Write()
        OffEtaStudyIptHists .Write()

outFile.Close()
