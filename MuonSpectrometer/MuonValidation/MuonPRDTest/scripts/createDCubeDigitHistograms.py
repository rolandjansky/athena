# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this script can be used to create DCube histograms from the output ntuples of NSWPRDValAlg

import os, sys, ROOT, argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='createDCubeDigiHistograms', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--inputFile', help='choose input ROOT file', default='NSWPRDValAlg.digi.ntuple.root', type=str)
    parser.add_argument('-o', '--outputFile', help='choose output ROOT file', default='NSWPRDValAlg.digi.dcube.root', type=str)
    parser.add_argument('--doRPC', help='turn off BIS78 RPC if using Run2 input ROOT file', default=True, action='store_true')
    parser.add_argument('--doMDT', help='turn off sMDT if using Run2 input ROOT file', default=True, action='store_true')
    
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
    truthPt = ROOT.TH1F("truthPt","truthPt;p_{T} [MeV]",100,0,1000) 
    truthEta = ROOT.TH1F("truthEta","truthEta;#eta",20,-10,10)
    truthPhi = ROOT.TH1F("truthPhi","truthPhi;#phi",20,-10,10)
    #############################################################################
    # RPCs (all stations)
    rpclocalX = ROOT.TH1F("rpclocalX","rpclocalX;Digits_RPC_localPosX [mm]",100,-1200,1200)
    rpclocalY = ROOT.TH1F("rpclocalY","rpclocalY;Digits_RPC_localPosY [mm]",100,-1200,1200)
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
    rpcglobalXY = ROOT.TH2F("rpcglobalXY","rpcglobalXY;Digits_rpc_globalPosX [mm];Digits_rpc_globalPosY [mm]",100,-15000,15000,100,-15000,15000)
    rpcglobalZY = ROOT.TH2F("rpcglobalZY","rpcglobalZY;Digits_rpc_globalPosZ [mm];Digits_rpc_globalPosY [mm]",100,-15000,15000,100,-15000,15000)
    rpcglobalZX = ROOT.TH2F("rpcglobalZX","rpcglobalZX;Digits_rpc_globalPosZ [mm];Digits_rpc_globalPosX [mm]",100,-15000,15000,100,-15000,15000)
    
    # BIS RPCs only
    bis_rpclocalX = ROOT.TH1F("bis_rpclocalX","bis_rpclocalX;Digits_bis_rpc_localPosX [mm]",100,-1000,1000)
    bis_rpclocalY = ROOT.TH1F("bis_rpclocalY","bis_rpclocalY;Digits_bis_rpc_localPosY [mm]",100,-1000,1000)
    bis7_rpclocalX = ROOT.TH1F("bis7_rpclocalX","bis7_rpclocalX;Digits_bis7_rpc_localPosX [mm]",100,-1000,1000) ## will remove the bis7/8 hists and make a separate script for them ##
    bis7_rpclocalY = ROOT.TH1F("bis7_rpclocalY","bis7_rpclocalY;Digits_bis7_rpc_localPosY [mm]",100,-1000,1000)
    bis8_rpclocalX = ROOT.TH1F("bis8_rpclocalX","bis8_rpclocalX;Digits_bis8_rpc_localPosX [mm]",100,-1000,1000)
    bis8_rpclocalY = ROOT.TH1F("bis8_rpclocalY","bis8_rpclocalY;Digits_bis8_rpc_localPosY [mm]",100,-1000,1000)
    bis_rpcStationEta = ROOT.TH1F("bis_rpcStationEta","bis_rpcStationEta;Digits_bis_rpc_stationEta",20,-10,10)
    bis_rpcStationPhi = ROOT.TH1F("bis_rpcStationPhi","bis_rpcStationPhi;Digits_bis_rpc_stationPhi",10,0,10)
    bis_rpcglobalX = ROOT.TH1F("bis_rpcglobalX","bis_rpcglobalX;Digits_bis_rpc_globalPosX [mm]",100,-5000,5000)
    bis_rpcglobalY = ROOT.TH1F("bis_rpcglobalY","bis_rpcglobalY;Digits_bis_rpc_globalPosY [mm]",100,-5000,5000)
    bis_rpcglobalZ = ROOT.TH1F("bis_rpcglobalZ","bis_rpcglobalZ;Digits_bis_rpc_globalPosZ [mm]",100,-10000,10000)
    bis_rpcGasGap = ROOT.TH1F("bis_rpcGasGap","bis_rpcGasGap;Digits_bis_rpc_gas_gap",5,0,5)
    bis_rpcdoubletR = ROOT.TH1F("bis_rpcdoubletR","bis_rpcdoubletR;Digits_bis_rpc_doubletR",4,0,4)
    bis_rpcdoubletZ = ROOT.TH1F("bis_rpcdoubletZ","bis_rpcdoubletZ;Digits_bis_rpc_doubletZ",4,0,4)
    bis_rpcdoubletPhi = ROOT.TH1F("bis_rpcdoubletPhi","bis_rpcdoubletPhi;Digits_bis_rpc_doubletPhi",4,0,4)
    bis_rpcTime = ROOT.TH1F("bis_rpcTime","bis_rpcTime;Digits_bis_rpc_time [ns]",80,0,40)
    bis7_rpcTime = ROOT.TH1F("bis7_rpcTime","bis7_rpcTime;Digits_bis7_rpc_time [ns]",80,0,40)
    bis8_rpcTime = ROOT.TH1F("bis8_rpcTime","bis8_rpcTime;Digits_bis8_rpc_time [ns]",80,0,40)
    bis_rpcStrip = ROOT.TH1F("bis_rpcStrip","bis_rpcStrip;Digits_bis_rpc_strip",10,0,10)
    bis_rpcStripNumber = ROOT.TH1F("bis_rpcStripNumber","bis_rpcStripNumber;Digits_bis_rpc_stripNumber",10,0,80)
    bis_rpcMeasuresPhi = ROOT.TH1F("bis_rpcMeasuresPhi","bis_rpcMeasuresPhi;Digits_bis_rpc_measuresPhi",2,0,2)
    bis_rpcChannel = ROOT.TH1F("bis_rpcChannel","bis_rpcChannel;Digits_bis_rpc_channel",10,0,10)
    bis_rpcglobalXY = ROOT.TH2F("bis_rpcglobalXY","bis_rpcglobalXY;Digits_bis_rpc_globalPosX [mm];Digits_bis_rpc_globalPosY [mm]",100,-5000,5000,100,-5000,5000)
    bis_rpcglobalZY = ROOT.TH2F("bis_rpcglobalZY","bis_rpcglobalZY;Digits_bis_rpc_globalPosZ [mm];Digits_bis_rpc_globalPosY [mm]",100,-15000,15000,100,-5000,5000)
    bis_rpcglobalZX = ROOT.TH2F("bis_rpcglobalZX","bis_rpcglobalZX;Digits_bis_rpc_globalPosZ [mm];Digits_bis_rpc_globalPosX [mm]",100,-15000,15000,100,-5000,5000)    
    
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
    mdtTube = ROOT.TH1F("mdtTube","mdtTube;Digits_MDT_tube",100,0,100) 
    mdtTubeLayer = ROOT.TH1F("mdtTubeLayer","mdtTubeLayer;Digits_MDT_tubeLayer",6,0,6)  
    mdtMultilayer = ROOT.TH1F("mdtMultilayer","mdtMultilayer;Digits_MDT_Multilayer",4,0,4) 
    mdtTime = ROOT.TH1F("mdtTime","mdtTime;Digits_MDT_time [ns]",100,0,2000)
    mdtCharge = ROOT.TH1F("mdtCharge","mdtCharge;Digits_MDT_charge [e]",100,0,600)
    mdtChannel = ROOT.TH1F("mdtChannel","mdtChannel;Digits_MDT_channel",10,0,10)
    mdtglobalXY = ROOT.TH2F("mdtglobalXY","mdtglobalXY;Digits_mdt_globalPosX [mm];Digits_mdt_globalPosY [mm]",100,-15000,15000,100,-15000,15000)
    mdtglobalZY = ROOT.TH2F("mdtglobalZY","mdtglobalZY;Digits_mdt_globalPosZ [mm];Digits_mdt_globalPosY [mm]",100,-15000,15000,100,-15000,15000)
    mdtglobalZX = ROOT.TH2F("mdtglobalZX","mdtglobalZX;Digits_mdt_globalPosZ [mm];Digits_mdt_globalPosX [mm]",100,-15000,15000,100,-15000,15000)
    mdtlocalTubePosX = ROOT.TH1F("mdtlocalTubePosX","mdtlocalTubePosX;Digits_MDT_localTubePosX [mm]",100,-100,100)
    mdtlocalTubePosY = ROOT.TH1F("mdtlocalTubePosY","mdtlocalTubePosY;Digits_MDT_localTubePosY [mm]",100,-100,100)  
    mdtlocalTubePosZ = ROOT.TH1F("mdtlocalTubePosZ","mdtlocalTubePosZ;Digits_MDT_localTubePosZ [mm]",100,-100,100)
    
    # BIS MDTs only
    bis_mdtlocalX = ROOT.TH1F("bis_mdtlocalX","bis_mdtlocalX;Digits_bis_mdt_localPosX [mm]",100,-1500,1500)
    bis_mdtlocalY = ROOT.TH1F("bis_mdtlocalY","bis_mdtlocalY;Digits_bis_mdt_localPosY [mm]",100,-1500,1500)
    bis_mdtlocalZ = ROOT.TH1F("bis_mdtlocalZ","bis_mdtlocalZ;Digits_bis_mdt_localPosZ [mm]",960,-900,900)
    bis_mdtStationEta = ROOT.TH1F("bis_mdtStationEta","bis_mdtStationEta;Digits_bis_mdt_stationEta",20,-10,10)
    bis_mdtStationPhi = ROOT.TH1F("bis_mdtStationPhi","bis_mdtStationPhi;Digits_bis_mdt_stationPhi",10,0,10)
    bis_mdtglobalX = ROOT.TH1F("bis_mdtglobalX","bis_mdtglobalX;Digits_bis_mdt_globalPosX [mm]",100,-5000,5000)
    bis_mdtglobalY = ROOT.TH1F("bis_mdtglobalY","bis_mdtglobalY;Digits_bis_mdt_globalPosY [mm]",100,-5000,5000)
    bis_mdtglobalZ = ROOT.TH1F("bis_mdtglobalZ","bis_mdtglobalZ;Digits_bis_mdt_globalPosZ [mm]",100,-10000,10000)
    bis_mdtNumberOfMultilayers = ROOT.TH1F("bis_mdtNumberOfMultilayers","bis_mdtNumberOfMultilayers;Digits_bis_mdt_numberOfMultilayers",4,0,4)
    bis_mdtTube = ROOT.TH1F("bis_mdtTube","bis_mdtTube;Digits_bis_mdt_tube",100,0,100) 
    bis_mdtTubeLayer = ROOT.TH1F("bis_mdtTubeLayer","bis_mdtTubeLayer;Digits_bis_mdt_tubeLayer",6,0,6)  
    bis_mdtMultilayer = ROOT.TH1F("bis_mdtMultilayer","bis_mdtMultilayer;Digits_bis_mdt_Multilayer",4,0,4) 
    bis_mdtTime = ROOT.TH1F("bis_mdtTime","bis_mdtTime;Digits_bis_mdt_time [ns]",100,0,2000)
    bis_mdtCharge = ROOT.TH1F("bis_mdtCharge","bis_mdtCharge;Digits_bis_mdt_charge [e]",100,0,600)
    bis_mdtChannel = ROOT.TH1F("bis_mdtChannel","bis_mdtChannel;Digits_bis_mdt_channel",10,0,10)
    bis_mdtglobalXY = ROOT.TH2F("bis_mdtglobalXY","bis_mdtglobalXY;Digits_bis_mdt_globalPosX [mm];Digits_bis_mdt_globalPosY [mm]",100,-5000,5000,100,-5000,5000)
    bis_mdtglobalZY = ROOT.TH2F("bis_mdtglobalZY","bis_mdtglobalZY;Digits_bis_mdt_globalPosZ [mm];Digits_bis_mdt_globalPosY [mm]",100,-15000,15000,100,-5000,5000)
    bis_mdtglobalZX = ROOT.TH2F("bis_mdtglobalZX","bis_mdtglobalZX;Digits_bis_mdt_globalPosZ [mm];Digits_bis_mdt_globalPosX [mm]",100,-15000,15000,100,-5000,5000)    
    bis_mdtlocalTubePosX = ROOT.TH1F("bis_mdtlocalTubePosX","bis_mdtlocalTubePosX;Digits_MDT_localTubePosX [mm]",120,-60,60)
    bis_mdtlocalTubePosY = ROOT.TH1F("bis_mdtlocalTubePosY","bis_mdtlocalTubePosY;Digits_MDT_localTubePosY [mm]",960,-900,900)  
    bis_mdtlocalTubePosZ = ROOT.TH1F("bis_mdtlocalTubePosZ","bis_mdtlocalTubePosZ;Digits_MDT_localTubePosZ [mm]",960,-900,900)
    
    bis_mdtlocalTubePosXY = ROOT.TH2F("bis_mdtlocalTubePosXY","bis_mdtlocalTubePosXY;Digits_MDT_localTubePosX [mm];Digits_MDT_localTubePosY [mm]",60,-30,30,100,-100,100)
    bis_mdtlocalTubePosZY = ROOT.TH2F("bis_mdtlocalTubePosZY","bis_mdtlocalTubePosZY;Digits_MDT_localTubePosZ [mm];Digits_MDT_localTubePosY [mm]",100,-1200,1200,100,-100,100)
    bis_mdtlocalTubePosZX = ROOT.TH2F("bis_mdtlocalTubePosZX","bis_mdtlocalTubePosZX;Digits_MDT_localTubePosZ [mm];Digits_MDT_localTubePosX [mm]",48,1,180,60,-30,30)
    
    for i in range(nEntries):
        inputTree.GetEntry(i)
        
# Truth
        for ntruth in range(0,len(inputTree.MuEntry_Particle_Pt)):
                truthPt.Fill(inputTree.MuEntry_Particle_Pt[ntruth])
                truthEta.Fill(inputTree.MuEntry_Particle_Eta[ntruth])
                truthPhi.Fill(inputTree.MuEntry_Particle_Phi[ntruth])
# RPC
        if Options.doRPC == True:
            for nrpcHit in range(0,inputTree.Digits_RPC):
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
                rpcglobalXY.Fill(inputTree.Digits_RPC_globalPosX[nrpcHit],inputTree.Digits_RPC_globalPosY[nrpcHit])
                rpcglobalZY.Fill(inputTree.Digits_RPC_globalPosZ[nrpcHit],inputTree.Digits_RPC_globalPosY[nrpcHit])
                rpcglobalZX.Fill(inputTree.Digits_RPC_globalPosZ[nrpcHit],inputTree.Digits_RPC_globalPosX[nrpcHit])
                
                # Cut on BIS78 modules only -- remove stationEta condition to also plot BIS16 in the future 
                if inputTree.Digits_RPC_stationName[nrpcHit]=="BIS" and inputTree.Digits_RPC_stationEta[nrpcHit]>6: 
                    #use measuresPhi=0 to extract local eta/Y coordinate from localX
                    if inputTree.Digits_RPC_measuresPhi[nrpcHit]==0:
                        bis_rpclocalY.Fill(inputTree.Digits_RPC_localPosX[nrpcHit])
                    #use measuresPhi=1 to extract local phi/X coordinate from localX
                    elif inputTree.Digits_RPC_measuresPhi[nrpcHit]==1:
                        bis_rpclocalX.Fill(inputTree.Digits_RPC_localPosX[nrpcHit]) 
                        
                    #use globalZ<6700 to get BIS7 
                    if inputTree.Digits_RPC_globalPosZ[nrpcHit]<6700:
                        bis7_rpcTime.Fill(inputTree.Digits_RPC_time[nrpcHit])
                        if inputTree.Digits_RPC_measuresPhi[nrpcHit]==0:
                            bis7_rpclocalY.Fill(inputTree.Digits_RPC_localPosX[nrpcHit])
                        elif inputTree.Digits_RPC_measuresPhi[nrpcHit]==1:
                            bis7_rpclocalX.Fill(inputTree.Digits_RPC_localPosX[nrpcHit])
                    else:
                    #use globalZ>6700 to get BIS8 
                        bis8_rpcTime.Fill(inputTree.Digits_RPC_time[nrpcHit])    
                        if inputTree.Digits_RPC_measuresPhi[nrpcHit]==0:
                            bis8_rpclocalY.Fill(inputTree.Digits_RPC_localPosX[nrpcHit])
                        elif inputTree.Digits_RPC_measuresPhi[nrpcHit]==1:
                            bis8_rpclocalX.Fill(inputTree.Digits_RPC_localPosX[nrpcHit])
                    
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
                    bis_rpcglobalXY.Fill(inputTree.Digits_RPC_globalPosX[nrpcHit],inputTree.Digits_RPC_globalPosY[nrpcHit])
                    bis_rpcglobalZY.Fill(inputTree.Digits_RPC_globalPosZ[nrpcHit],inputTree.Digits_RPC_globalPosY[nrpcHit])
                    bis_rpcglobalZX.Fill(inputTree.Digits_RPC_globalPosZ[nrpcHit],inputTree.Digits_RPC_globalPosX[nrpcHit])
 
                else: continue
# MDT
        if Options.doMDT == True:
            for nmdtHit in range(0,inputTree.Digits_MDT):
                mdtlocalTubePosX.Fill(inputTree.Digits_MDT_localTubePosX[nmdtHit])
                mdtlocalTubePosY.Fill(inputTree.Digits_MDT_localTubePosY[nmdtHit])
                mdtlocalTubePosZ.Fill(inputTree.Digits_MDT_localTubePosZ[nmdtHit])
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
                mdtglobalXY.Fill(inputTree.Digits_MDT_globalPosX[nmdtHit],inputTree.Digits_MDT_globalPosY[nmdtHit])
                mdtglobalZY.Fill(inputTree.Digits_MDT_globalPosZ[nmdtHit],inputTree.Digits_MDT_globalPosY[nmdtHit])
                mdtglobalZX.Fill(inputTree.Digits_MDT_globalPosZ[nmdtHit],inputTree.Digits_MDT_globalPosX[nmdtHit])
                
                # Cut on BIS78 modules only
                if inputTree.Digits_MDT_stationName[nmdtHit]=="BIS" and inputTree.Digits_MDT_stationEta[nmdtHit]>6: 
                    bis_mdtlocalTubePosX.Fill(inputTree.Digits_MDT_localTubePosX[nmdtHit])
                    bis_mdtlocalTubePosY.Fill(inputTree.Digits_MDT_localTubePosY[nmdtHit])
                    bis_mdtlocalTubePosZ.Fill(inputTree.Digits_MDT_localTubePosZ[nmdtHit])
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
                    bis_mdtglobalXY.Fill(inputTree.Digits_MDT_globalPosX[nmdtHit],inputTree.Digits_MDT_globalPosY[nmdtHit])
                    bis_mdtglobalZY.Fill(inputTree.Digits_MDT_globalPosZ[nmdtHit],inputTree.Digits_MDT_globalPosY[nmdtHit])
                    bis_mdtglobalZX.Fill(inputTree.Digits_MDT_globalPosZ[nmdtHit],inputTree.Digits_MDT_globalPosX[nmdtHit])
                    bis_mdtlocalTubePosXY.Fill(inputTree.Digits_MDT_localTubePosX[nmdtHit],inputTree.Digits_MDT_localTubePosY[nmdtHit])
                    bis_mdtlocalTubePosZY.Fill(inputTree.Digits_MDT_localTubePosZ[nmdtHit],inputTree.Digits_MDT_localTubePosY[nmdtHit])
                    bis_mdtlocalTubePosZX.Fill(inputTree.Digits_MDT_localTubePosZ[nmdtHit],inputTree.Digits_MDT_localTubePosX[nmdtHit])
                    
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
    ODir.WriteTObject(rpcglobalXY, rpcglobalXY.GetName())
    ODir.WriteTObject(rpcglobalZY, rpcglobalZY.GetName())
    ODir.WriteTObject(rpcglobalZX, rpcglobalZX.GetName())
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
    ODir.WriteTObject(bis7_rpcTime, bis7_rpcTime.GetName())
    ODir.WriteTObject(bis8_rpcTime, bis8_rpcTime.GetName())
    ODir.WriteTObject(bis_rpcdoubletR, bis_rpcdoubletR.GetName())
    ODir.WriteTObject(bis_rpcdoubletZ, bis_rpcdoubletZ.GetName())
    ODir.WriteTObject(bis_rpcdoubletPhi, bis_rpcdoubletPhi.GetName())
    ODir.WriteTObject(bis_rpcglobalXY, bis_rpcglobalXY.GetName())
    ODir.WriteTObject(bis_rpcglobalZY, bis_rpcglobalZY.GetName())
    ODir.WriteTObject(bis_rpcglobalZX, bis_rpcglobalZX.GetName())
# MDT
    ODir.WriteTObject(mdtlocalTubePosX, mdtlocalTubePosX.GetName())
    ODir.WriteTObject(mdtlocalTubePosY, mdtlocalTubePosY.GetName())
    ODir.WriteTObject(mdtlocalTubePosZ, mdtlocalTubePosZ.GetName())
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
    ODir.WriteTObject(mdtglobalXY, mdtglobalXY.GetName())
    ODir.WriteTObject(mdtglobalZY, mdtglobalZY.GetName())
    ODir.WriteTObject(mdtglobalZX, mdtglobalZX.GetName())
# BIS MDT
    ODir.WriteTObject(bis_mdtlocalTubePosX, bis_mdtlocalTubePosX.GetName())
    ODir.WriteTObject(bis_mdtlocalTubePosY, bis_mdtlocalTubePosY.GetName())
    ODir.WriteTObject(bis_mdtlocalTubePosZ, bis_mdtlocalTubePosZ.GetName())
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
    ODir.WriteTObject(bis_mdtglobalXY, bis_mdtglobalXY.GetName())
    ODir.WriteTObject(bis_mdtglobalZY, bis_mdtglobalZY.GetName())
    ODir.WriteTObject(bis_mdtglobalZX, bis_mdtglobalZX.GetName())
    ODir.WriteTObject(bis_mdtlocalTubePosXY, bis_mdtlocalTubePosXY.GetName())
    ODir.WriteTObject(bis_mdtlocalTubePosZY, bis_mdtlocalTubePosZY.GetName())
    ODir.WriteTObject(bis_mdtlocalTubePosZX, bis_mdtlocalTubePosZX.GetName())
    
    print ('INFO: Written histograms to file %s'%(Options.outputFile))
