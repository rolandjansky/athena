# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this script can be used to create DCube histograms from the output ntuples of NSWPRDValAlg

import os, sys, ROOT, argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='createDCubeDigiHistograms', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--inputFile', help='choose input ROOT file', default='NSWPRDValAlg.digi.ntuple.root', type=str)
    parser.add_argument('-o', '--outputFile', help='choose output ROOT file', default='NSWPRDValAlg.dcube.root', type=str)
    parser.add_argument('--doRPC', help='turn off BIS78 RPC if using Run2 input ROOT file', default=False, action='store_true')
    parser.add_argument('--doMDT', help='turn off sMDT if using Run2 input ROOT file', default=False, action='store_true')
    
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
    outputFile.mkdir("digitisation/")
    ODir = outputFile.GetDirectory("digitisation/")
    ODir.cd()

    #############################################################################
    # truth information
    truthPtHist = ROOT.TH1F("truthPt","truthPt;p_{T} [GeV]",50,0,200)
    truthEtaHist = ROOT.TH1F("truthEta","truthEta;#eta",20,-10,10)
    truthPhiHist = ROOT.TH1F("truthPhi","truthPhi;#phi",20,-10,10)
    #############################################################################
    # RPCs
    rpclocalX = ROOT.TH1F("rpclocalX","rpclocalX;Digits_RPC_localPosX",100,-1200,1200)
    rpclocalY = ROOT.TH1F("rpclocalY","rpclocalY;Digits_RPC_localPosY",100,-100,100)
    rpcStationEta = ROOT.TH1F("rpcStationEta","rpcStationEta;Digits_RPC_stationEta",20,-10,10)
    rpcStationPhi = ROOT.TH1F("rpcStationPhi","rpcStationPhi;Digits_RPC_stationPhi",10,0,10)
    rpcglobalX = ROOT.TH1F("rpcglobalX","rpcglobalX;Digits_RPC_globalPosX",100,-13000,12000)
    rpcglobalY = ROOT.TH1F("rpcglobalY","rpcglobalY;Digits_RPC_globalPosY",100,-15000,14000)
    rpcglobalZ = ROOT.TH1F("rpcglobalZ","rpcglobalZ;Digits_RPC_globalPosZ",100,-15000,15000)
    rpcGasGap = ROOT.TH1F("rpcGasGap","rpcGasGap;Digits_RPC_gas_gap",5,0,5)
    rpcdoubletR = ROOT.TH1F("rpcdoubletR","rpcdoubletR;Digits_RPC_doubletR",100,-20,20)
    rpcdoubletZ = ROOT.TH1F("rpcdoubletZ","rpcdoubletZ;Digits_RPC_doubletZ",100,-20,20)
    rpcdoubletPhi = ROOT.TH1F("rpcdoubletPhi","rpcdoubletPhi;Digits_RPC_doubletPhi",10,0,10)
    rpcTime = ROOT.TH1F("rpcTime","rpcTime;Digits_RPC_time",100,0,100)
    rpcStrip = ROOT.TH1F("rpcStrip","rpcStrip;Digits_RPC_strip",10,0,10)
    rpcStripNumber = ROOT.TH1F("rpcNumber","rpcNumber;Digits_RPC_stripNumber",10,0,10)
    rpcMeasuresPhi = ROOT.TH1F("rpcMeasuresPhi","rpcMeasuresPhi;Digits_RPC_measuresPhi",10,0,10)
    rpcChannel = ROOT.TH1F("rpcChannel","rpcChannel;Digits_RPC_channel",10,0,10)
    #############################################################################
    # MDTs
    mdtlocalX = ROOT.TH1F("mdtlocalX","mdtlocalX;Digits_MDT_localPosX",100,-1500,1500)
    mdtlocalY = ROOT.TH1F("mdtlocalY","mdtlocalY;Digits_MDT_localPosY",100,-1500,1500)
    mdtStationEta = ROOT.TH1F("mdtStationEta","mdtStationEta;Digits_MDT_stationEta",20,-10,10)
    mdtStationPhi = ROOT.TH1F("mdtStationPhi","mdtStationPhi;Digits_MDT_stationPhi",10,0,10)
    mdtglobalX = ROOT.TH1F("mdtglobalX","mdtglobalX;Digits_MDT_globalPosX",100,-13000,12000)
    mdtglobalY = ROOT.TH1F("mdtglobalY","mdtglobalY;Digits_MDT_globalPosY",100,-15000,14000)
    mdtglobalZ = ROOT.TH1F("mdtglobalZ","mdtglobalZ;Digits_MDT_globalPosZ",100,-26000,26000)
    mdtNumberOfMultilayers = ROOT.TH1F("mdtNumberOfMultilayers","mdtNumberOfMultilayers;Digits_MDT_numberOfMultilayers",4,0,4)
    mdtTube = ROOT.TH1F("mdtTube","mdtTube;Digits_MDT_tube",6,0,6) 
    mdtTubeLayer = ROOT.TH1F("mdtTubeLayer","mdtTubeLayer;Digits_MDT_tubeLayer",6,0,6)  
    mdtMultilayer = ROOT.TH1F("mdtMultilayer","mdtMultilayer;Digits_MDT_Multilayer",4,0,4) 
    mdtTime = ROOT.TH1F("mdtTime","mdtTime;Digits_MDT_time",100,0,2000)
    mdtCharge = ROOT.TH1F("mdtCharge","mdtCharge;Digits_MDT_charge",100,0,600)
    mdtChannel = ROOT.TH1F("mdtChannel","mdtChannel;Digits_MDT_channel",10,0,10)
    
    for i in range(nEntries):
        inputTree.GetEntry(i)
        
# Truth
        for ntruth in range(0,len(inputTree.MuEntry_Particle_Pt)):
                truthPtHist.Fill(inputTree.MuEntry_Particle_Pt[ntruth])
                truthEtaHist.Fill(inputTree.MuEntry_Particle_Eta[ntruth])
                truthPhiHist.Fill(inputTree.MuEntry_Particle_Phi[ntruth])
# RPC
        if Options.doRPC == True:
            for nrpcHit in range(0,len(inputTree.Digits_RPC_localPosX)):
                # Cut on BIS modules only
                if inputTree.Digits_RPC_stationName[nrpcHit]=="BIS":
                    rpclocalX.Fill(inputTree.Digits_RPC_localPosX[nrpcHit])
                    rpclocalY.Fill(inputTree.Digits_RPC_localPosY[nrpcHit])
                    rpcStationEta.Fill(inputTree.Digits_RPC_stationEta[nrpcHit])
                    rpcStationPhi.Fill(inputTree.Digits_RPC_stationPhi[nrpcHit])
                    rpcglobalX.Fill(inputTree.Digits_RPC_globalPosX[nrpcHit])
                    rpcglobalY.Fill(inputTree.Digits_RPC_globalPosY[nrpcHit])
                    rpcglobalZ.Fill(inputTree.Digits_RPC_globalPosZ[nrpcHit])
                    rpcGasGap.Fill(inputTree.Digits_RPC_gas_gap[nrpcHit])
                    rpcdoubletR.Fill(inputTree.Digits_RPC_doubletR[nrpcHit])
                    rpcdoubletZ.Fill(inputTree.Digits_RPC_doubletZ[nrpcHit])
                    rpcdoubletPhi.Fill(inputTree.Digits_RPC_doubletPhi[nrpcHit])
                    rpcMeasuresPhi.Fill(inputTree.Digits_RPC_measuresPhi[nrpcHit])
                    rpcTime.Fill(inputTree.Digits_RPC_time[nrpcHit])
                    rpcStrip.Fill(inputTree.Digits_RPC_strip[nrpcHit])
                    rpcStripNumber.Fill(inputTree.Digits_RPC_stripNumber[nrpcHit])
                else: continue
# MDT
        if Options.doMDT == True:
            for nmdtHit in range(0,len(inputTree.Digits_MDT_globalPosX)):
            # Cut on BIS modules only
                if inputTree.Digits_MDT_stationName[nmdtHit]=="BIS":
                    mdtlocalX.Fill(inputTree.Digits_MDT_localPosX[nmdtHit])
                    mdtlocalY.Fill(inputTree.Digits_MDT_localPosY[nmdtHit])
                    mdtStationEta.Fill(inputTree.Digits_MDT_stationEta[nmdtHit])
                    mdtStationPhi.Fill(inputTree.Digits_MDT_stationPhi[nmdtHit])
                    mdtglobalX.Fill(inputTree.Digits_MDT_globalPosX[nmdtHit])
                    mdtglobalY.Fill(inputTree.Digits_MDT_globalPosY[nmdtHit])
                    mdtglobalZ.Fill(inputTree.Digits_MDT_globalPosZ[nmdtHit])  
                    mdtNumberOfMultilayers.Fill(inputTree.Digits_MDT_numberOfMultilayers[nmdtHit])
                    mdtTube.Fill(inputTree.Digits_MDT_tube[nmdtHit])
                    mdtTubeLayer.Fill(inputTree.Digits_MDT_tubeLayer[nmdtHit])
                    mdtMultilayer.Fill(inputTree.Digits_MDT_multilayer[nmdtHit])
                    mdtTime.Fill(inputTree.Digits_MDT_time[nmdtHit])
                    mdtCharge.Fill(inputTree.Digits_MDT_charge[nmdtHit])
                else: continue    
            
# Truth
    ODir.WriteTObject(truthPtHist, truthPtHist.GetName())
    ODir.WriteTObject(truthEtaHist, truthEtaHist.GetName())
    ODir.WriteTObject(truthPhiHist, truthPhiHist.GetName())
# RPC
    ODir.WriteTObject(rpclocalX, rpclocalX.GetName())
    ODir.WriteTObject(rpclocalY, rpclocalY.GetName())
    ODir.WriteTObject(rpcStationEta, rpcStationEta.GetName())
    ODir.WriteTObject(rpcStationPhi, rpcStationPhi.GetName())
    ODir.WriteTObject(rpcglobalX, rpcglobalX.GetName())
    ODir.WriteTObject(rpcglobalY, rpcglobalY.GetName())
    ODir.WriteTObject(rpcglobalZ, rpcglobalZ.GetName())
    ODir.WriteTObject(rpcGasGap, rpcGasGap.GetName())
    ODir.WriteTObject(rpcStrip, rpcStrip.GetName())
    ODir.WriteTObject(rpcStripNumber, rpcStripNumber.GetName())
    ODir.WriteTObject(rpcMeasuresPhi, rpcMeasuresPhi.GetName())
    ODir.WriteTObject(rpcChannel, rpcChannel.GetName())
    ODir.WriteTObject(rpcTime, rpcTime.GetName())
    ODir.WriteTObject(rpcdoubletR, rpcdoubletR.GetName())
    ODir.WriteTObject(rpcdoubletZ, rpcdoubletZ.GetName())
    ODir.WriteTObject(rpcdoubletPhi, rpcdoubletPhi.GetName())
# MDT
    ODir.WriteTObject(mdtlocalX, mdtlocalX.GetName())
    ODir.WriteTObject(mdtlocalY, mdtlocalY.GetName())
    ODir.WriteTObject(mdtStationEta, mdtStationEta.GetName())
    ODir.WriteTObject(mdtStationPhi, mdtStationPhi.GetName())
    ODir.WriteTObject(mdtglobalX, mdtglobalX.GetName())
    ODir.WriteTObject(mdtglobalY, mdtglobalY.GetName())
    ODir.WriteTObject(mdtglobalZ, mdtglobalZ.GetName())
    ODir.WriteTObject(mdtNumberOfMultilayers, mdtNumberOfMultilayers.GetName())
    ODir.WriteTObject(mdtTube, mdtTube.GetName())
    ODir.WriteTObject(mdtTubeLayer, mdtTubeLayer.GetName())
    ODir.WriteTObject(mdtMultilayer, mdtMultilayer.GetName())
    ODir.WriteTObject(mdtTime, mdtTime.GetName())
    ODir.WriteTObject(mdtCharge, mdtCharge.GetName())
    ODir.WriteTObject(mdtChannel, mdtChannel.GetName())

    print ('INFO: Written histograms to file %s'%(Options.outputFile))