# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this script can be used to create DCube histograms from the output ntuples of NSWPRDValAlg

import os, sys, ROOT, argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='createDCubeHistograms', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--inputFile', help='choose input ROOT file', default='NSWPRDValAlg.sim.ntuple.root', type=str)
    parser.add_argument('-o', '--outputFile', help='choose output ROOT file', default='NSWPRDValAlg.dcube.root', type=str)
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
    #############################################################################

    for i in range(nEntries):
        inputTree.GetEntry(i)

        for ntruth in range(0,len(inputTree.MuEntry_Particle_Pt)):
            truthPtHist.Fill(inputTree.MuEntry_Particle_Pt[ntruth]*0.001)
            truthEtaHist.Fill(inputTree.MuEntry_Particle_Eta[ntruth])
            truthPhiHist.Fill(inputTree.MuEntry_Particle_Phi[ntruth])

        for nrpcHit in range(0,len(inputTree.RPC_hitLocalPositionX)):
            rpcLocalX.Fill(inputTree.RPC_hitLocalPositionX[nrpcHit])
            rpcLocalY.Fill(inputTree.RPC_hitLocalPositionY[nrpcHit])
            rpcLocalZ.Fill(inputTree.RPC_hitLocalPositionZ[nrpcHit])

    ODir.WriteTObject(truthPtHist, truthPtHist.GetName())
    ODir.WriteTObject(truthEtaHist, truthEtaHist.GetName())
    ODir.WriteTObject(truthPhiHist, truthPhiHist.GetName())

    ODir.WriteTObject(rpcLocalX, rpcLocalX.GetName())
    ODir.WriteTObject(rpcLocalY, rpcLocalY.GetName())
    ODir.WriteTObject(rpcLocalZ, rpcLocalZ.GetName())

    print ('INFO: Written histograms to file %s'%(Options.outputFile))

