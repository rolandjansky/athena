# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this script can be used to create DCube histograms from the output ntuples of NSWPRDValAlg

import os, sys, ROOT, argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='createDCubeHistograms', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--inputFile', help='choose input ROOT file', default='NSWPRDValAlg.sim.ntuple.root', type=str)
    parser.add_argument('-o', '--outputFile', help='choose output ROOT file', default='NSWPRDValAlg.dcube.root', type=str)
    parser.add_argument('--doCSC', help='turn off CSC if using Run4 input ROOT file', default=False, action='store_true')
    parser.add_argument('--doMM', help='turn off MM if using Run2 input ROOT file', default=False, action='store_true')
    parser.add_argument('--doSTGC', help='turn off STGC if using Run2 input ROOT file', default=False, action='store_true')
    Options = parser.parse_args()

    ROOT.gROOT.SetBatch(True)

    if not os.path.exists(Options.inputFile):
        print ('ERROR: File %s does not exist'%Options.inputFile)
        sys.exit(1)

    inputFile = ROOT.TFile(Options.inputFile, "READ")
    if not inputFile:
        print ('ERROR: Failed to open file %s'%Options.inputFile)
        sys.exit(1)
    inputTree = inputFile.Get("NSWValTree")
    if not inputTree:
        print ('ERROR: NSWValTree does not exist in file %s'%Options.inputFile)
        sys.exit(1)

    nEntries = inputTree.GetEntries()
    if nEntries==0:
        print ('ERROR: NSWValTree of file %s has 0 entries'%Options.inputFile)
        sys.exit(1)

    outputFile = ROOT.TFile(Options.outputFile, "RECREATE")
    if not outputFile:
        print ('ERROR: Failed to open file %s'%Options.outputFile)
        sys.exit(1)

    outputFile.cd()
    outputFile.mkdir("simulation/")
    ODir = outputFile.GetDirectory("simulation/")
    ODir.cd()

    #############################################################################
    # truth information
    truthPtHist = ROOT.TH1F("truthPt","truthPt;p_{T} [GeV]",100,0,100)
    truthEtaHist = ROOT.TH1F("truthEta","truthEta;#eta",100,-2.8,2.8)
    truthPhiHist = ROOT.TH1F("truthPhi","truthPhi;#phi",100,-3.15,3.15)
    #############################################################################
    # RPCs
    rpcLocalX = ROOT.TH1F("rpcLocalX","rpcLocalX;RPC_hitLocalPositionX",100,-1.05,1.05)
    rpcLocalY = ROOT.TH1F("rpcLocalY","rpcLocalY;RPC_hitLocalPositionY",100,-1500,1500)
    rpcLocalZ = ROOT.TH1F("rpcLocalZ","rpcLocalZ;RPC_hitLocalPositionZ",100,-700,700)
    rpcStationEta = ROOT.TH1F("rpcStationEta","rpcStationEta;RPC_stationEta",16,-8,8)
    rpcStationPhi = ROOT.TH1F("rpcStationPhi","rpcStationPhi;RPC_stationPhi",9,0,9)
    rpcGlobalX = ROOT.TH1F("rpcGlobalX","rpcGlobalX;RPC_hitGlobalPositionX",100,-13000,12000)
    rpcGlobalY = ROOT.TH1F("rpcGlobalY","rpcGlobalY;RPC_hitGlobalPositionY",100,-15000,14000)
    rpcGlobalZ = ROOT.TH1F("rpcGlobalZ","rpcGlobalZ;RPC_hitGlobalPositionZ",100,-15000,15000)
    rpcGlobalR = ROOT.TH1F("rpcGlobalR","rpcGlobalR;RPC_hitGlobalPositionR",100,4000,14000)
    rpcGlobalP = ROOT.TH1F("rpcGlobalP","rpcGlobalP;RPC_hitGlobalPositionP",100,-3.6,3.6)
    rpcGasGap = ROOT.TH1F("rpcGasGap","rpcGasGap;RPC_Sim_gasGapLayer",3,0,3)
    #############################################################################
    # MDTs
    mdtLocalX = ROOT.TH1F("mdtLocalX","mdtLocalX;MDT_hitLocalPositionX",100,-16,16)
    mdtLocalY = ROOT.TH1F("mdtLocalY","mdtLocalY;MDT_hitLocalPositionY",100,-16,16)
    mdtLocalZ = ROOT.TH1F("mdtLocalZ","mdtLocalZ;MDT_hitLocalPositionZ",100,-2500,2500)
    mdtStationEta = ROOT.TH1F("mdtStationEta","mdtStationEta;MDT_stationEta",16,-8,8)
    mdtStationPhi = ROOT.TH1F("mdtStationPhi","mdtStationPhi;MDT_stationPhi",9,0,9)
    mdtGlobalX = ROOT.TH1F("mdtGlobalX","mdtGlobalX;MDT_hitGlobalPositionX",100,-13000,12000)
    mdtGlobalY = ROOT.TH1F("mdtGlobalY","mdtGlobalY;MDT_hitGlobalPositionY",100,-15000,14000)
    mdtGlobalZ = ROOT.TH1F("mdtGlobalZ","mdtGlobalZ;MDT_hitGlobalPositionZ",100,-26000,26000)
    mdtGlobalR = ROOT.TH1F("mdtGlobalR","mdtGlobalR;MDT_hitGlobalPositionR",100,1000,14000)
    mdtGlobalP = ROOT.TH1F("mdtGlobalP","mdtGlobalP;MDT_hitGlobalPositionP",100,-3.6,3.6)
    mdtTube = ROOT.TH1F("mdtTube","mdtTube;MDT_Sim_tube",100,0,110)  
    #############################################################################
    # CSCs
    if Options.doCSC == True:
        cscStationEta = ROOT.TH1F("cscStationEta","cscStationEta;CSC_stationEta",4,-3,1)
        cscStationPhi = ROOT.TH1F("cscStationPhi","cscStationPhi;CSC_stationPhi",9,0,9)
        cscGlobalX = ROOT.TH1F("cscGlobalX","cscGlobalX;CSC_hitGlobalPositionX",100,-1400,2200)
        cscGlobalY = ROOT.TH1F("cscGlobalY","cscGlobalY;CSC_hitGlobalPositionY",100,-2100,2100)
        cscGlobalZ = ROOT.TH1F("cscGlobalZ","cscGlobalZ;CSC_hitGlobalPositionZ",100,-7900,-7100)
        cscGlobalR = ROOT.TH1F("cscGlobalR","cscGlobalR;CSC_hitGlobalPositionR",100,500,2500)
        cscGlobalP = ROOT.TH1F("cscGlobalP","cscGlobalP;CSC_hitGlobalPositionP",100,-3,3)
        cscWireLayer = ROOT.TH1F("cscWireLayer","cscWireLayer;CSC_Sim_wireLayer",5,0,5)
    #############################################################################  
    # TGCs
    tgcLocalX = ROOT.TH1F("tgcLocalX","tgcLocalX;TGC_hitLocalPositionX",100,-1.5,1.5)
    tgcLocalY = ROOT.TH1F("tgcLocalY","tgcLocalY;TGC_hitLocalPositionY",100,-800,800)
    tgcLocalZ = ROOT.TH1F("tgcLocalZ","tgcLocalZ;TGC_hitLocalPositionZ",100,-1200,1200)
    tgcStationEta = ROOT.TH1F("tgcStationEta","tgcStationEta;TGC_stationEta",12,-6,6)
    tgcStationPhi = ROOT.TH1F("tgcStationPhi","tgcStationPhi;TGC_stationPhi",50,0,50)
    tgcGlobalX = ROOT.TH1F("tgcGlobalX","tgcGlobalX;TGC_hitGlobalPositionX",100,-13000,12000)
    tgcGlobalY = ROOT.TH1F("tgcGlobalY","tgcGlobalY;TGC_hitGlobalPositionY",100,-11000,11000)
    tgcGlobalZ = ROOT.TH1F("tgcGlobalZ","tgcGlobalZ;TGC_hitGlobalPositionZ",100,-18000,18000)
    tgcGlobalR = ROOT.TH1F("tgcGlobalR","tgcGlobalR;TGC_hitGlobalPositionR",100,1000,13000)
    tgcGlobalP = ROOT.TH1F("tgcGlobalP","tgcGlobalP;TGC_hitGlobalPositionP",100,-3.6,3.6)
    tgcGasGap = ROOT.TH1F("tgcGasGap","tgcGasGap;TGC_gasGap",4,0,4)
    #############################################################################
    # MMs
    if Options.doMM == True:
        mmGlobalX = ROOT.TH1F("mmGlobalX","mmGlobalX;MM_hitGlobalPositionX",100,-5000,3000)
        mmGlobalY = ROOT.TH1F("mmGlobalY","mmGlobalY;MM_hitGlobalPositionY",100,-3000,4000)
        mmGlobalZ = ROOT.TH1F("mmGlobalZ","mmGlobalZ;MM_hitGlobalPositionZ",100,7000,7800)
        mmGlobalR = ROOT.TH1F("mmGlobalR","mmGlobalR;MM_hitGlobalPositionR",100,1200,4600)
        mmGlobalP = ROOT.TH1F("mmGlobalP","mmGlobalP;MM_hitGlobalPositionP",100,-3.4,3.4)
    #############################################################################
    # sTGCs
    if Options.doSTGC == True:
        stgcGlobalX = ROOT.TH1F("stgcGlobalX","stgcGlobalX;sTGC_hitGlobalPositionX",100,-5000,4200)
        stgcGlobalY = ROOT.TH1F("stgcGlobalY","stgcGlobalY;sTGC_hitGlobalPositionY",100,-3200,3400)
        stgcGlobalZ = ROOT.TH1F("stgcGlobalZ","stgcGlobalZ;sTGC_hitGlobalPositionZ",100,6900,7900)
        stgcGlobalR = ROOT.TH1F("stgcGlobalR","stgcGlobalR;sTGC_hitGlobalPositionR",100,700,4700)
        stgcGlobalP = ROOT.TH1F("stgcGlobalP","stgcGlobalP;sTGC_hitGlobalPositionP",100,-3.4,3.4)
    #############################################################################

    for i in range(nEntries):
        inputTree.GetEntry(i)
# Truth
        for ntruth in range(0,len(inputTree.MuEntry_Particle_Pt)):
            truthPtHist.Fill(inputTree.MuEntry_Particle_Pt[ntruth]*0.001)
            truthEtaHist.Fill(inputTree.MuEntry_Particle_Eta[ntruth])
            truthPhiHist.Fill(inputTree.MuEntry_Particle_Phi[ntruth])
# RPC
        for nrpcHit in range(0,len(inputTree.RPC_hitLocalPositionX)):
            rpcLocalX.Fill(inputTree.RPC_hitLocalPositionX[nrpcHit])
            rpcLocalY.Fill(inputTree.RPC_hitLocalPositionY[nrpcHit])
            rpcLocalZ.Fill(inputTree.RPC_hitLocalPositionZ[nrpcHit])
            rpcStationEta.Fill(inputTree.RPC_stationEta[nrpcHit])
            rpcStationPhi.Fill(inputTree.RPC_stationPhi[nrpcHit])
            rpcGlobalX.Fill(inputTree.RPC_hitGlobalPositionX[nrpcHit])
            rpcGlobalY.Fill(inputTree.RPC_hitGlobalPositionY[nrpcHit])
            rpcGlobalZ.Fill(inputTree.RPC_hitGlobalPositionZ[nrpcHit])
            rpcGlobalR.Fill(inputTree.RPC_hitGlobalPositionR[nrpcHit])
            rpcGlobalP.Fill(inputTree.RPC_hitGlobalPositionP[nrpcHit])
            rpcGasGap.Fill(inputTree.RPC_Sim_gasGapLayer[nrpcHit])
# MDT
        for nmdtHit in range(0,len(inputTree.MDT_hitLocalPositionX)):
            mdtLocalX.Fill(inputTree.MDT_hitLocalPositionX[nmdtHit])
            mdtLocalY.Fill(inputTree.MDT_hitLocalPositionY[nmdtHit])
            mdtLocalZ.Fill(inputTree.MDT_hitLocalPositionZ[nmdtHit])
            mdtStationEta.Fill(inputTree.MDT_stationEta[nmdtHit])
            mdtStationPhi.Fill(inputTree.MDT_stationPhi[nmdtHit])
            mdtGlobalX.Fill(inputTree.MDT_hitGlobalPositionX[nmdtHit])
            mdtGlobalY.Fill(inputTree.MDT_hitGlobalPositionY[nmdtHit])
            mdtGlobalZ.Fill(inputTree.MDT_hitGlobalPositionZ[nmdtHit])
            mdtGlobalR.Fill(inputTree.MDT_hitGlobalPositionR[nmdtHit])
            mdtGlobalP.Fill(inputTree.MDT_hitGlobalPositionP[nmdtHit])
            mdtTube.Fill(inputTree.MDT_Sim_tube[nmdtHit])
# CSC
        if Options.doCSC == True:
            for ncscHit in range(0,len(inputTree.CSC_hitGlobalPositionX)):
                cscStationEta.Fill(inputTree.CSC_stationEta[ncscHit])
                cscStationPhi.Fill(inputTree.CSC_stationPhi[ncscHit])
                cscGlobalX.Fill(inputTree.CSC_hitGlobalPositionX[ncscHit])
                cscGlobalY.Fill(inputTree.CSC_hitGlobalPositionY[ncscHit])
                cscGlobalZ.Fill(inputTree.CSC_hitGlobalPositionZ[ncscHit])
                cscGlobalR.Fill(inputTree.CSC_hitGlobalPositionR[ncscHit])
                cscGlobalP.Fill(inputTree.CSC_hitGlobalPositionP[ncscHit])
                cscWireLayer.Fill(inputTree.CSC_Sim_wireLayer[ncscHit])
# TGC
        for ntgcHit in range(0,len(inputTree.TGC_hitLocalPositionX)):
            tgcLocalX.Fill(inputTree.TGC_hitLocalPositionX[ntgcHit])
            tgcLocalY.Fill(inputTree.TGC_hitLocalPositionY[ntgcHit])
            tgcLocalZ.Fill(inputTree.TGC_hitLocalPositionZ[ntgcHit])
            tgcStationEta.Fill(inputTree.TGC_stationEta[ntgcHit])
            tgcStationPhi.Fill(inputTree.TGC_stationPhi[ntgcHit])
            tgcGlobalX.Fill(inputTree.TGC_hitGlobalPositionX[ntgcHit])
            tgcGlobalY.Fill(inputTree.TGC_hitGlobalPositionY[ntgcHit])
            tgcGlobalZ.Fill(inputTree.TGC_hitGlobalPositionZ[ntgcHit])
            tgcGlobalR.Fill(inputTree.TGC_hitGlobalPositionR[ntgcHit])
            tgcGlobalP.Fill(inputTree.TGC_hitGlobalPositionP[ntgcHit])
            tgcGasGap.Fill(inputTree.TGC_gasGap[ntgcHit])
# MM
        if Options.doMM == True:
            for nmmHit in range(0,len(inputTree.Hits_MM_hitGlobalPositionX)):
                mmGlobalX.Fill(inputTree.Hits_MM_hitGlobalPositionX[nmmHit])
                mmGlobalY.Fill(inputTree.Hits_MM_hitGlobalPositionY[nmmHit])
                mmGlobalZ.Fill(inputTree.Hits_MM_hitGlobalPositionZ[nmmHit])
                mmGlobalR.Fill(inputTree.Hits_MM_hitGlobalPositionR[nmmHit])
                mmGlobalP.Fill(inputTree.Hits_MM_hitGlobalPositionP[nmmHit])
# sTGC
        if Options.doSTGC == True:
            for nstgcHit in range(0,len(inputTree.Hits_sTGC_hitGlobalPositionX)):
                stgcGlobalX.Fill(inputTree.Hits_sTGC_hitGlobalPositionX[nstgcHit])
                stgcGlobalY.Fill(inputTree.Hits_sTGC_hitGlobalPositionY[nstgcHit])
                stgcGlobalZ.Fill(inputTree.Hits_sTGC_hitGlobalPositionZ[nstgcHit])
                stgcGlobalR.Fill(inputTree.Hits_sTGC_hitGlobalPositionR[nstgcHit])
                stgcGlobalP.Fill(inputTree.Hits_sTGC_hitGlobalPositionP[nstgcHit])

# Truth
    ODir.WriteTObject(truthPtHist, truthPtHist.GetName())
    ODir.WriteTObject(truthEtaHist, truthEtaHist.GetName())
    ODir.WriteTObject(truthPhiHist, truthPhiHist.GetName())
# RPC
    ODir.WriteTObject(rpcLocalX, rpcLocalX.GetName())
    ODir.WriteTObject(rpcLocalY, rpcLocalY.GetName())
    ODir.WriteTObject(rpcLocalZ, rpcLocalZ.GetName())
    ODir.WriteTObject(rpcStationEta, rpcStationEta.GetName())
    ODir.WriteTObject(rpcStationPhi, rpcStationPhi.GetName())
    ODir.WriteTObject(rpcGlobalX, rpcGlobalX.GetName())
    ODir.WriteTObject(rpcGlobalY, rpcGlobalY.GetName())
    ODir.WriteTObject(rpcGlobalZ, rpcGlobalZ.GetName())
    ODir.WriteTObject(rpcGlobalR, rpcGlobalR.GetName())
    ODir.WriteTObject(rpcGlobalP, rpcGlobalP.GetName())
    ODir.WriteTObject(rpcGasGap, rpcGasGap.GetName())
# MDT
    ODir.WriteTObject(mdtLocalX, mdtLocalX.GetName())
    ODir.WriteTObject(mdtLocalY, mdtLocalY.GetName())
    ODir.WriteTObject(mdtLocalZ, mdtLocalZ.GetName())
    ODir.WriteTObject(mdtStationEta, mdtStationEta.GetName())
    ODir.WriteTObject(mdtStationPhi, mdtStationPhi.GetName())
    ODir.WriteTObject(mdtGlobalX, mdtGlobalX.GetName())
    ODir.WriteTObject(mdtGlobalY, mdtGlobalY.GetName())
    ODir.WriteTObject(mdtGlobalZ, mdtGlobalZ.GetName())
    ODir.WriteTObject(mdtGlobalR, mdtGlobalR.GetName())
    ODir.WriteTObject(mdtGlobalP, mdtGlobalP.GetName())
    ODir.WriteTObject(mdtTube, mdtTube.GetName())
# CSC
    if Options.doCSC == True:
        ODir.WriteTObject(cscStationEta, cscStationEta.GetName())
        ODir.WriteTObject(cscStationPhi, cscStationPhi.GetName())
        ODir.WriteTObject(cscGlobalX, cscGlobalX.GetName())
        ODir.WriteTObject(cscGlobalY, cscGlobalY.GetName())
        ODir.WriteTObject(cscGlobalZ, cscGlobalZ.GetName())
        ODir.WriteTObject(cscGlobalR, cscGlobalR.GetName())
        ODir.WriteTObject(cscGlobalP, cscGlobalP.GetName())
        ODir.WriteTObject(cscWireLayer,cscWireLayer.GetName())
# TGC
    ODir.WriteTObject(tgcLocalX, tgcLocalX.GetName())
    ODir.WriteTObject(tgcLocalY, tgcLocalY.GetName())
    ODir.WriteTObject(tgcLocalZ, tgcLocalZ.GetName())
    ODir.WriteTObject(tgcStationEta, tgcStationEta.GetName())
    ODir.WriteTObject(tgcStationPhi, tgcStationPhi.GetName())
    ODir.WriteTObject(tgcGlobalX, tgcGlobalX.GetName())
    ODir.WriteTObject(tgcGlobalY, tgcGlobalY.GetName())
    ODir.WriteTObject(tgcGlobalZ, tgcGlobalZ.GetName())
    ODir.WriteTObject(tgcGlobalR, tgcGlobalR.GetName())
    ODir.WriteTObject(tgcGlobalP, tgcGlobalP.GetName())
    ODir.WriteTObject(tgcGasGap, tgcGasGap.GetName())
# MM
    if Options.doMM == True:
        ODir.WriteTObject(mmGlobalX, mmGlobalX.GetName())
        ODir.WriteTObject(mmGlobalY, mmGlobalY.GetName())
        ODir.WriteTObject(mmGlobalZ, mmGlobalZ.GetName())
        ODir.WriteTObject(mmGlobalR, mmGlobalR.GetName())
        ODir.WriteTObject(mmGlobalP, mmGlobalP.GetName())
# STGC
    if Options.doSTGC == True:
        ODir.WriteTObject(stgcGlobalX, stgcGlobalX.GetName())
        ODir.WriteTObject(stgcGlobalY, stgcGlobalY.GetName())
        ODir.WriteTObject(stgcGlobalZ, stgcGlobalZ.GetName())
        ODir.WriteTObject(stgcGlobalR, stgcGlobalR.GetName())
        ODir.WriteTObject(stgcGlobalP, stgcGlobalP.GetName())

    print ('INFO: Written histograms to file %s'%(Options.outputFile))