# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this script can be used to create DCube histograms from the output ntuples of NSWPRDValAlg

import os, sys, ROOT, argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='createDCubeDigiHistograms', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--inputFile', help='choose input ROOT file', default='NSWPRDValAlg.digi.ntuple.root', type=str)
    parser.add_argument('-o', '--outputFile', help='choose output ROOT file', default='NSWPRDValAlg.digi.dcube.root', type=str)
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
    truthPt = ROOT.TH1F("truthPt","truthPt;p_{T} [GeV]",100,0,2000) #standard unit = MeV? 
    truthEta = ROOT.TH1F("truthEta","truthEta;#eta",20,-10,10)
    truthPhi = ROOT.TH1F("truthPhi","truthPhi;#phi",20,-10,10)
    #############################################################################
    # RPCs (all stations)
    rpclocalX = ROOT.TH1F("rpclocalX","rpclocalX;Digits_RPC_localPosX [mm]",100,-1200,1200)
    rpclocalY = ROOT.TH1F("rpclocalY","rpclocalY;Digits_RPC_localPosY [mm]",100,-2,2)
    rpcStationEta = ROOT.TH1F("rpcStationEta","rpcStationEta;Digits_RPC_stationEta",20,-10,10)
    rpcStationPhi = ROOT.TH1F("rpcStationPhi","rpcStationPhi;Digits_RPC_stationPhi",10,0,10)
    rpcglobalX = ROOT.TH1F("rpcglobalX","rpcglobalX;Digits_RPC_globalPosX [mm]",100,-13000,12000)
    rpcglobalY = ROOT.TH1F("rpcglobalY","rpcglobalY;Digits_RPC_globalPosY [mm]",100,-15000,14000)
    rpcglobalZ = ROOT.TH1F("rpcglobalZ","rpcglobalZ;Digits_RPC_globalPosZ [mm]",100,-15000,15000)
    rpcGasGap = ROOT.TH1F("rpcGasGap","rpcGasGap;Digits_RPC_gas_gap",5,0,5)
    rpcdoubletR = ROOT.TH1F("rpcdoubletR","rpcdoubletR;Digits_RPC_doubletR",4,0,4)
    rpcdoubletZ = ROOT.TH1F("rpcdoubletZ","rpcdoubletZ;Digits_RPC_doubletZ",4,0,4)
    rpcdoubletPhi = ROOT.TH1F("rpcdoubletPhi","rpcdoubletPhi;Digits_RPC_doubletPhi",4,0,4)
    rpcTime = ROOT.TH1F("rpcTime","rpcTime;Digits_RPC_time",100,0,100)
    rpcStrip = ROOT.TH1F("rpcStrip","rpcStrip;Digits_RPC_strip",10,0,80)
    rpcStripNumber = ROOT.TH1F("rpcStripNumber","rpcStripNumber;Digits_RPC_stripNumber",10,0,80)
    rpcMeasuresPhi = ROOT.TH1F("rpcMeasuresPhi","rpcMeasuresPhi;Digits_RPC_measuresPhi",2,0,2)
    rpcChannel = ROOT.TH1F("rpcChannel","rpcChannel;Digits_RPC_channel",10,0,10)
    
    # BIS RPCs only
    bis_rpclocalX = ROOT.TH1F("bis_rpclocalX","bis_rpclocalX;Digits_bis_rpc_localPosX [mm]",100,-1200,1200)
    bis_rpclocalY = ROOT.TH1F("bis_rpclocalY","bis_rpclocalY;Digits_bis_rpc_localPosY [mm]",100,-2,2)
    bis_rpcStationEta = ROOT.TH1F("bis_rpcStationEta","bis_rpcStationEta;Digits_bis_rpc_stationEta",20,-10,10)
    bis_rpcStationPhi = ROOT.TH1F("bis_rpcStationPhi","bis_rpcStationPhi;Digits_bis_rpc_stationPhi",10,0,10)
    bis_rpcglobalX = ROOT.TH1F("bis_rpcglobalX","bis_rpcglobalX;Digits_bis_rpc_globalPosX [mm]",100,-13000,12000)
    bis_rpcglobalY = ROOT.TH1F("bis_rpcglobalY","bis_rpcglobalY;Digits_bis_rpc_globalPosY [mm]",100,-15000,14000)
    bis_rpcglobalZ = ROOT.TH1F("bis_rpcglobalZ","bis_rpcglobalZ;Digits_bis_rpc_globalPosZ [mm]",100,-15000,15000)
    bis_rpcGasGap = ROOT.TH1F("bis_rpcGasGap","bis_rpcGasGap;Digits_bis_rpc_gas_gap",5,0,5)
    bis_rpcdoubletR = ROOT.TH1F("bis_rpcdoubletR","bis_rpcdoubletR;Digits_bis_rpc_doubletR",4,0,4)
    bis_rpcdoubletZ = ROOT.TH1F("bis_rpcdoubletZ","bis_rpcdoubletZ;Digits_bis_rpc_doubletZ",4,0,4)
    bis_rpcdoubletPhi = ROOT.TH1F("bis_rpcdoubletPhi","bis_rpcdoubletPhi;Digits_bis_rpc_doubletPhi",4,0,4)
    bis_rpcTime = ROOT.TH1F("bis_rpcTime","bis_rpcTime;Digits_bis_rpc_time",100,0,100)
    bis_rpcStrip = ROOT.TH1F("bis_rpcStrip","bis_rpcStrip;Digits_bis_rpc_strip",10,0,10)
    bis_rpcStripNumber = ROOT.TH1F("bis_rpcStripNumber","bis_rpcStripNumber;Digits_bis_rpc_stripNumber",10,0,80)
    bis_rpcMeasuresPhi = ROOT.TH1F("bis_rpcMeasuresPhi","bis_rpcMeasuresPhi;Digits_bis_rpc_measuresPhi",2,0,2)
    bis_rpcChannel = ROOT.TH1F("bis_rpcChannel","bis_rpcChannel;Digits_bis_rpc_channel",10,0,10)
    #############################################################################
    # MDTs (all stations)
    mdtlocalX = ROOT.TH1F("mdtlocalX","mdtlocalX;Digits_MDT_localPosX [mm]",100,-1500,1500)
    mdtlocalY = ROOT.TH1F("mdtlocalY","mdtlocalY;Digits_MDT_localPosY [mm]",100,-1500,1500)
    mdtStationEta = ROOT.TH1F("mdtStationEta","mdtStationEta;Digits_MDT_stationEta",20,-10,10)
    mdtStationPhi = ROOT.TH1F("mdtStationPhi","mdtStationPhi;Digits_MDT_stationPhi",10,0,10)
    mdtglobalX = ROOT.TH1F("mdtglobalX","mdtglobalX;Digits_MDT_globalPosX [mm]",100,-13000,12000)
    mdtglobalY = ROOT.TH1F("mdtglobalY","mdtglobalY;Digits_MDT_globalPosY [mm]",100,-15000,14000)
    mdtglobalZ = ROOT.TH1F("mdtglobalZ","mdtglobalZ;Digits_MDT_globalPosZ [mm]",100,-26000,26000)
    mdtNumberOfMultilayers = ROOT.TH1F("mdtNumberOfMultilayers","mdtNumberOfMultilayers;Digits_MDT_numberOfMultilayers",4,0,4)
    mdtTube = ROOT.TH1F("mdtTube","mdtTube;Digits_MDT_tube",80,0,80) 
    mdtTubeLayer = ROOT.TH1F("mdtTubeLayer","mdtTubeLayer;Digits_MDT_tubeLayer",6,0,6)  
    mdtMultilayer = ROOT.TH1F("mdtMultilayer","mdtMultilayer;Digits_MDT_Multilayer",4,0,4) 
    mdtTime = ROOT.TH1F("mdtTime","mdtTime;Digits_MDT_time",100,0,2000)
    mdtCharge = ROOT.TH1F("mdtCharge","mdtCharge;Digits_MDT_charge",100,0,600)
    mdtChannel = ROOT.TH1F("mdtChannel","mdtChannel;Digits_MDT_channel",10,0,10)
    
    # BIS MDTs only
    bis_mdtlocalX = ROOT.TH1F("bis_mdtlocalX","bis_mdtlocalX;Digits_bis_mdt_localPosX [mm]",100,-1500,1500)
    bis_mdtlocalY = ROOT.TH1F("bis_mdtlocalY","bis_mdtlocalY;Digits_bis_mdt_localPosY [mm]",100,-1500,1500)
    bis_mdtStationEta = ROOT.TH1F("bis_mdtStationEta","bis_mdtStationEta;Digits_bis_mdt_stationEta",20,-10,10)
    bis_mdtStationPhi = ROOT.TH1F("bis_mdtStationPhi","bis_mdtStationPhi;Digits_bis_mdt_stationPhi",10,0,10)
    bis_mdtglobalX = ROOT.TH1F("bis_mdtglobalX","bis_mdtglobalX;Digits_bis_mdt_globalPosX [mm]",100,-13000,12000)
    bis_mdtglobalY = ROOT.TH1F("bis_mdtglobalY","bis_mdtglobalY;Digits_bis_mdt_globalPosY [mm]",100,-15000,14000)
    bis_mdtglobalZ = ROOT.TH1F("bis_mdtglobalZ","bis_mdtglobalZ;Digits_bis_mdt_globalPosZ [mm]",100,-26000,26000)
    bis_mdtNumberOfMultilayers = ROOT.TH1F("bis_mdtNumberOfMultilayers","bis_mdtNumberOfMultilayers;Digits_bis_mdt_numberOfMultilayers",4,0,4)
    bis_mdtTube = ROOT.TH1F("bis_mdtTube","bis_mdtTube;Digits_bis_mdt_tube",80,0,80) 
    bis_mdtTubeLayer = ROOT.TH1F("bis_mdtTubeLayer","bis_mdtTubeLayer;Digits_bis_mdt_tubeLayer",6,0,6)  
    bis_mdtMultilayer = ROOT.TH1F("bis_mdtMultilayer","bis_mdtMultilayer;Digits_bis_mdt_Multilayer",4,0,4) 
    bis_mdtTime = ROOT.TH1F("bis_mdtTime","bis_mdtTime;Digits_bis_mdt_time",100,0,2000)
    bis_mdtCharge = ROOT.TH1F("bis_mdtCharge","bis_mdtCharge;Digits_bis_mdt_charge",100,0,600)
    bis_mdtChannel = ROOT.TH1F("bis_mdtChannel","bis_mdtChannel;Digits_bis_mdt_channel",10,0,10)
    
    for i in range(nEntries):
        inputTree.GetEntry(i)
        
# Truth
        for ntruth in range(0,len(inputTree.MuEntry_Particle_Pt)):
                truthPt.Fill(inputTree.MuEntry_Particle_Pt[ntruth]*0.001)
                truthEta.Fill(inputTree.MuEntry_Particle_Eta[ntruth])
                truthPhi.Fill(inputTree.MuEntry_Particle_Phi[ntruth])
# RPC
        if Options.doRPC == True:
            for nrpcHit in range(0,len(inputTree.Digits_RPC_localPosX)):
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
                
                # Cut on BIS modules only
                if inputTree.Digits_RPC_stationName[nrpcHit]=="BIS":
                    bis_rpclocalX.Fill(inputTree.Digits_RPC_localPosX[nrpcHit])
                    bis_rpclocalY.Fill(inputTree.Digits_RPC_localPosY[nrpcHit])
                    bis_rpcStationEta.Fill(inputTree.Digits_RPC_stationEta[nrpcHit])
                    bis_rpcStationPhi.Fill(inputTree.Digits_RPC_stationPhi[nrpcHit])
                    bis_rpcglobalX.Fill(inputTree.Digits_RPC_globalPosX[nrpcHit])
                    bis_rpcglobalY.Fill(inputTree.Digits_RPC_globalPosY[nrpcHit])
                    bis_rpcglobalZ.Fill(inputTree.Digits_RPC_globalPosZ[nrpcHit])
                    bis_rpcGasGap.Fill(inputTree.Digits_RPC_gas_gap[nrpcHit])
                    bis_rpcdoubletR.Fill(inputTree.Digits_RPC_doubletR[nrpcHit])
                    bis_rpcdoubletZ.Fill(inputTree.Digits_RPC_doubletZ[nrpcHit])
                    bis_rpcdoubletPhi.Fill(inputTree.Digits_RPC_doubletPhi[nrpcHit])
                    bis_rpcMeasuresPhi.Fill(inputTree.Digits_RPC_measuresPhi[nrpcHit])
                    bis_rpcTime.Fill(inputTree.Digits_RPC_time[nrpcHit])
                    bis_rpcStrip.Fill(inputTree.Digits_RPC_strip[nrpcHit])
                    bis_rpcStripNumber.Fill(inputTree.Digits_RPC_stripNumber[nrpcHit])
                else: continue
# MDT
        if Options.doMDT == True:
            for nmdtHit in range(0,len(inputTree.Digits_MDT_globalPosX)):
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
                
                # Cut on BIS modules only
                if inputTree.Digits_MDT_stationName[nmdtHit]=="BIS": # and inputTree.Digits_MDT_stationEta[nmdtHit]==7: # uncomment this to cut on BIS78 MDTs (currently not in ART nightlies for some reason)
                    bis_mdtlocalX.Fill(inputTree.Digits_MDT_localPosX[nmdtHit])
                    bis_mdtlocalY.Fill(inputTree.Digits_MDT_localPosY[nmdtHit])
                    bis_mdtStationEta.Fill(inputTree.Digits_MDT_stationEta[nmdtHit])
                    bis_mdtStationPhi.Fill(inputTree.Digits_MDT_stationPhi[nmdtHit])
                    bis_mdtglobalX.Fill(inputTree.Digits_MDT_globalPosX[nmdtHit])
                    bis_mdtglobalY.Fill(inputTree.Digits_MDT_globalPosY[nmdtHit])
                    bis_mdtglobalZ.Fill(inputTree.Digits_MDT_globalPosZ[nmdtHit])  
                    bis_mdtNumberOfMultilayers.Fill(inputTree.Digits_MDT_numberOfMultilayers[nmdtHit])
                    bis_mdtTube.Fill(inputTree.Digits_MDT_tube[nmdtHit])
                    bis_mdtTubeLayer.Fill(inputTree.Digits_MDT_tubeLayer[nmdtHit])
                    bis_mdtMultilayer.Fill(inputTree.Digits_MDT_multilayer[nmdtHit])
                    bis_mdtTime.Fill(inputTree.Digits_MDT_time[nmdtHit])
                    bis_mdtCharge.Fill(inputTree.Digits_MDT_charge[nmdtHit])
                else: continue    
            
# Truth
    ODir.WriteTObject(truthPt, truthPt.GetName())
    ODir.WriteTObject(truthEta, truthEta.GetName())
    ODir.WriteTObject(truthPhi, truthPhi.GetName())
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
# BIS RPC
    ODir.WriteTObject(bis_rpclocalX, bis_rpclocalX.GetName())
    ODir.WriteTObject(bis_rpclocalY, bis_rpclocalY.GetName())
    ODir.WriteTObject(bis_rpcStationEta, bis_rpcStationEta.GetName())
    ODir.WriteTObject(bis_rpcStationPhi, bis_rpcStationPhi.GetName())
    ODir.WriteTObject(bis_rpcglobalX, bis_rpcglobalX.GetName())
    ODir.WriteTObject(bis_rpcglobalY, bis_rpcglobalY.GetName())
    ODir.WriteTObject(bis_rpcglobalZ, bis_rpcglobalZ.GetName())
    ODir.WriteTObject(bis_rpcGasGap, bis_rpcGasGap.GetName())
    ODir.WriteTObject(bis_rpcStrip, bis_rpcStrip.GetName())
    ODir.WriteTObject(bis_rpcStripNumber, bis_rpcStripNumber.GetName())
    ODir.WriteTObject(bis_rpcMeasuresPhi, bis_rpcMeasuresPhi.GetName())
    ODir.WriteTObject(bis_rpcChannel, bis_rpcChannel.GetName())
    ODir.WriteTObject(bis_rpcTime, bis_rpcTime.GetName())
    ODir.WriteTObject(bis_rpcdoubletR, bis_rpcdoubletR.GetName())
    ODir.WriteTObject(bis_rpcdoubletZ, bis_rpcdoubletZ.GetName())
    ODir.WriteTObject(bis_rpcdoubletPhi, bis_rpcdoubletPhi.GetName())
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
# BIS MDT
    ODir.WriteTObject(bis_mdtlocalX, bis_mdtlocalX.GetName())
    ODir.WriteTObject(bis_mdtlocalY, bis_mdtlocalY.GetName())
    ODir.WriteTObject(bis_mdtStationEta, bis_mdtStationEta.GetName())
    ODir.WriteTObject(bis_mdtStationPhi, bis_mdtStationPhi.GetName())
    ODir.WriteTObject(bis_mdtglobalX, bis_mdtglobalX.GetName())
    ODir.WriteTObject(bis_mdtglobalY, bis_mdtglobalY.GetName())
    ODir.WriteTObject(bis_mdtglobalZ, bis_mdtglobalZ.GetName())
    ODir.WriteTObject(bis_mdtNumberOfMultilayers, bis_mdtNumberOfMultilayers.GetName())
    ODir.WriteTObject(bis_mdtTube, bis_mdtTube.GetName())
    ODir.WriteTObject(bis_mdtTubeLayer, bis_mdtTubeLayer.GetName())
    ODir.WriteTObject(bis_mdtMultilayer, bis_mdtMultilayer.GetName())
    ODir.WriteTObject(bis_mdtTime, bis_mdtTime.GetName())
    ODir.WriteTObject(bis_mdtCharge, bis_mdtCharge.GetName())
    ODir.WriteTObject(bis_mdtChannel, bis_mdtChannel.GetName())

    print ('INFO: Written histograms to file %s'%(Options.outputFile))
