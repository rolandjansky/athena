# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# this script can be used to create DCube histograms from the output ntuples of NSWPRDValAlg

import os, sys, ROOT

class MyHistoFiller(object):

    #############################################################################
    # truth information
    truthPtHist = ROOT.TH1F("truthPt","truthPt;p_{T} [GeV]",100,0,100)
    truthEtaHist = ROOT.TH1F("truthEta","truthEta;#eta",100,-2.8,2.8)
    truthPhiHist = ROOT.TH1F("truthPhi","truthPhi;#phi",100,-3.15,3.15)
    #############################################################################
    # RPC Sim Histograms
    rpcLocalX = ROOT.TH1F("rpcLocalX","rpcLocalX;RPC_SIM_hitLocalPositionX",100,-1.05,1.05)
    rpcLocalY = ROOT.TH1F("rpcLocalY","rpcLocalY;RPC_SIM_hitLocalPositionY",100,-1500,1500)
    rpcStationEta = ROOT.TH1F("rpcStationEta","rpcStationEta;RPC_SIM_stationEta",16,-8,8)
    rpcStationPhi = ROOT.TH1F("rpcStationPhi","rpcStationPhi;RPC_SIM_stationPhi",9,0,9)
    rpcGlobalX = ROOT.TH1F("rpcGlobalX","rpcGlobalX;RPC_SIM_hitGlobalPositionX",100,-15000,15000)
    rpcGlobalY = ROOT.TH1F("rpcGlobalY","rpcGlobalY;RPC_SIM_hitGlobalPositionY",100,-15000,15000)
    rpcGlobalZ = ROOT.TH1F("rpcGlobalZ","rpcGlobalZ;RPC_SIM_hitGlobalPositionZ",100,-15000,15000)
    rpcGasGap = ROOT.TH1F("rpcGasGap","rpcGasGap;RPC_SIM_gasGap",4,0,4)
    
    # RPC Digit Histograms (all stations)
    rpcDigitlocalX = ROOT.TH1F("rpcDigitlocalX","rpcDigitlocalX;Digits_RPC_localPosX [mm]",100,-1200,1200)
    rpcDigitlocalY = ROOT.TH1F("rpcDigitlocalY","rpcDigitlocalY;Digits_RPC_localPosY [mm]",100,-1200,1200)
    rpcDigitStationEta = ROOT.TH1F("rpcDigitStationEta","rpcDigitStationEta;Digits_RPC_stationEta",20,-10,10)
    rpcDigitStationPhi = ROOT.TH1F("rpcDigitStationPhi","rpcDigitStationPhi;Digits_RPC_stationPhi",10,0,10)
    rpcDigitglobalX = ROOT.TH1F("rpcDigitglobalX","rpcDigitglobalX;Digits_RPC_globalPosX [mm]",100,-13000,12000)
    rpcDigitglobalY = ROOT.TH1F("rpcDigitglobalY","rpcDigitglobalY;Digits_RPC_globalPosY [mm]",100,-15000,15000)
    rpcDigitglobalZ = ROOT.TH1F("rpcDigitglobalZ","rpcDigitglobalZ;Digits_RPC_globalPosZ [mm]",100,-15000,15000)
    rpcDigitGasGap = ROOT.TH1F("rpcDigitGasGap","rpcDigitGasGap;Digits_RPC_gas_gap",5,0,5)
    rpcDigitdoubletR = ROOT.TH1F("rpcDigitdoubletR","rpcDigitdoubletR;Digits_RPC_doubletR",4,0,4)
    rpcDigitdoubletZ = ROOT.TH1F("rpcDigitdoubletZ","rpcDigitdoubletZ;Digits_RPC_doubletZ",4,0,4)
    rpcDigitdoubletPhi = ROOT.TH1F("rpcDigitdoubletPhi","rpcDigitdoubletPhi;Digits_RPC_doubletPhi",4,0,4)
    rpcDigitTime = ROOT.TH1F("rpcDigitTime","rpcDigitTime;Digits_RPC_time",100,0,150)
    rpcDigitStrip = ROOT.TH1F("rpcDigitStrip","rpcDigitStrip;Digits_RPC_strip",10,0,80)
    rpcDigitStripNumber = ROOT.TH1F("rpcDigitStripNumber","rpcDigitStripNumber;Digits_RPC_stripNumber",10,0,80)
    rpcDigitMeasuresPhi = ROOT.TH1F("rpcDigitMeasuresPhi","rpcDigitMeasuresPhi;Digits_RPC_measuresPhi",2,0,2)
    rpcDigitglobalXY = ROOT.TH2F("rpcDigitglobalXY","rpcDigitglobalXY;Digits_RPC_globalPosX [mm];Digits_RPC_globalPosY [mm]",100,-15000,15000,100,-15000,15000)
    rpcDigitglobalZY = ROOT.TH2F("rpcDigitglobalZY","rpcDigitglobalZY;Digits_RPC_globalPosZ [mm];Digits_RPC_globalPosY [mm]",100,-15000,15000,100,-15000,15000)
    rpcDigitglobalZX = ROOT.TH2F("rpcDigitglobalZX","rpcDigitglobalZX;Digits_RPC_globalPosZ [mm];Digits_RPC_globalPosX [mm]",100,-15000,15000,100,-15000,15000)

    # RPC Digit Histograms (BIS RPCs only)
    bis_rpcDigitlocalX = ROOT.TH1F("bis_rpcDigitlocalX","bis_rpcDigitlocalX;Digits_bis_RPC_localPosX [mm]",100,-1000,1000)
    bis_rpcDigitlocalY = ROOT.TH1F("bis_rpcDigitlocalY","bis_rpcDigitlocalY;Digits_bis_RPC_localPosY [mm]",100,-1000,1000)
    bis7_rpcDigitlocalX = ROOT.TH1F("bis7_rpcDigitlocalX","bis7_rpcDigitlocalX;Digits_bis7_RPC_localPosX [mm]",100,-1000,1000) ## will remove the bis7/8 hists and make a separate script for them ##
    bis7_rpcDigitlocalY = ROOT.TH1F("bis7_rpcDigitlocalY","bis7_rpcDigitlocalY;Digits_bis7_RPC_localPosY [mm]",100,-1000,1000)
    bis8_rpcDigitlocalX = ROOT.TH1F("bis8_rpcDigitlocalX","bis8_rpcDigitlocalX;Digits_bis8_RPC_localPosX [mm]",100,-1000,1000)
    bis8_rpcDigitlocalY = ROOT.TH1F("bis8_rpcDigitlocalY","bis8_rpcDigitlocalY;Digits_bis8_RPC_localPosY [mm]",100,-1000,1000)
    bis_rpcDigitStationEta = ROOT.TH1F("bis_rpcDigitStationEta","bis_rpcDigitStationEta;Digits_bis_RPC_stationEta",20,-10,10)
    bis_rpcDigitStationPhi = ROOT.TH1F("bis_rpcDigitStationPhi","bis_rpcDigitStationPhi;Digits_bis_RPC_stationPhi",10,0,10)
    bis_rpcDigitglobalX = ROOT.TH1F("bis_rpcDigitglobalX","bis_rpcDigitglobalX;Digits_bis_RPC_globalPosX [mm]",100,-5000,5000)
    bis_rpcDigitglobalY = ROOT.TH1F("bis_rpcDigitglobalY","bis_rpcDigitglobalY;Digits_bis_RPC_globalPosY [mm]",100,-5000,5000)
    bis_rpcDigitglobalZ = ROOT.TH1F("bis_rpcDigitglobalZ","bis_rpcDigitglobalZ;Digits_bis_RPC_globalPosZ [mm]",100,-10000,10000)
    bis_rpcDigitGasGap = ROOT.TH1F("bis_rpcDigitGasGap","bis_rpcDigitGasGap;Digits_bis_RPC_gas_gap",5,0,5)
    bis_rpcDigitdoubletR = ROOT.TH1F("bis_rpcDigitdoubletR","bis_rpcDigitdoubletR;Digits_bis_RPC_doubletR",4,0,4)
    bis_rpcDigitdoubletZ = ROOT.TH1F("bis_rpcDigitdoubletZ","bis_rpcDigitdoubletZ;Digits_bis_RPC_doubletZ",4,0,4)
    bis_rpcDigitdoubletPhi = ROOT.TH1F("bis_rpcDigitdoubletPhi","bis_rpcDigitdoubletPhi;Digits_bis_RPC_doubletPhi",4,0,4)
    bis_rpcDigitTime = ROOT.TH1F("bis_rpcDigitTime","bis_rpcDigitTime;Digits_bis_RPC_time [ns]",80,0,40)
    bis7_rpcDigitTime = ROOT.TH1F("bis7_rpcDigitTime","bis7_rpcDigitTime;Digits_bis7_RPC_time [ns]",80,0,40)
    bis8_rpcDigitTime = ROOT.TH1F("bis8_rpcDigitTime","bis8_rpcDigitTime;Digits_bis8_RPC_time [ns]",80,0,40)
    bis_rpcDigitStrip = ROOT.TH1F("bis_rpcDigitStrip","bis_rpcDigitStrip;Digits_bis_RPC_strip",10,0,10)
    bis_rpcDigitStripNumber = ROOT.TH1F("bis_rpcDigitStripNumber","bis_rpcDigitStripNumber;Digits_bis_RPC_stripNumber",10,0,80)
    bis_rpcDigitMeasuresPhi = ROOT.TH1F("bis_rpcDigitMeasuresPhi","bis_rpcDigitMeasuresPhi;Digits_bis_RPC_measuresPhi",2,0,2)
    bis_rpcDigitglobalXY = ROOT.TH2F("bis_rpcDigitglobalXY","bis_rpcDigitglobalXY;Digits_bis_RPC_globalPosX [mm];Digits_bis_RPC_globalPosY [mm]",100,-5000,5000,100,-5000,5000)
    bis_rpcDigitglobalZY = ROOT.TH2F("bis_rpcDigitglobalZY","bis_rpcDigitglobalZY;Digits_bis_RPC_globalPosZ [mm];Digits_bis_RPC_globalPosY [mm]",100,-15000,15000,100,-5000,5000)
    bis_rpcDigitglobalZX = ROOT.TH2F("bis_rpcDigitglobalZX","bis_rpcDigitglobalZX;Digits_bis_RPC_globalPosZ [mm];Digits_bis_RPC_globalPosX [mm]",100,-15000,15000,100,-5000,5000)    

    
    #############################################################################
    # MDT Sim Histograms
    mdtLocalX = ROOT.TH1F("mdtLocalX","mdtLocalX;MDT_Sim_hitLocalPositionX",100,-15,15)
    mdtLocalY = ROOT.TH1F("mdtLocalY","mdtLocalY;MDT_Sim_hitLocalPositionY",100,-16,16)
    mdtLocalZ = ROOT.TH1F("mdtLocalZ","mdtLocalZ;MDT_Sim_hitLocalPositionZ",100,-2500,2500)
    mdtStationEta = ROOT.TH1F("mdtStationEta","mdtStationEta;MDT_Sim_stationEta",16,-8,8)
    mdtStationPhi = ROOT.TH1F("mdtStationPhi","mdtStationPhi;MDT_Sim_stationPhi",9,0,9)
    mdtGlobalX = ROOT.TH1F("mdtGlobalX","mdtGlobalX;MDT_Sim_hitGlobalPositionX",100,-15000,15000)
    mdtGlobalY = ROOT.TH1F("mdtGlobalY","mdtGlobalY;MDT_Sim_hitGlobalPositionY",100,-15000,15000)
    mdtGlobalZ = ROOT.TH1F("mdtGlobalZ","mdtGlobalZ;MDT_Sim_hitGlobalPositionZ",100,-26000,26000)
    mdtTube = ROOT.TH1F("mdtTube","mdtTube;MDT_Sim_tube",100,0,100)
    
    # MDT Digit Histograms (all stations)
    mdtDigitStationEta = ROOT.TH1F("mdtDigitStationEta","mdtDigitStationEta;Digits_MDT_stationEta",20,-10,10)
    mdtDigitStationPhi = ROOT.TH1F("mdtDigitStationPhi","mdtDigitStationPhi;Digits_MDT_stationPhi",10,0,10)
    mdtDigitglobalX = ROOT.TH1F("mdtDigitglobalX","mdtDigitglobalX;Digits_MDT_globalPosX [mm]",100,-15000,15000)
    mdtDigitglobalY = ROOT.TH1F("mdtDigitglobalY","mdtDigitglobalY;Digits_MDT_globalPosY [mm]",100,-15000,15000)
    mdtDigitglobalZ = ROOT.TH1F("mdtDigitglobalZ","mdtDigitglobalZ;Digits_MDT_globalPosZ [mm]",100,-25000,25000)
    mdtDigitNumberOfMultilayers = ROOT.TH1F("mdtDigitNumberOfMultilayers","mdtDigitNumberOfMultilayers;Digits_MDT_numberOfMultilayers",4,0,4)
    mdtDigitTube = ROOT.TH1F("mdtDigitTube","mdtDigitTube;Digits_MDT_tube",100,0,100)
    mdtDigitTubeLayer = ROOT.TH1F("mdtDigitTubeLayer","mdtDigitTubeLayer;Digits_MDT_tubeLayer",6,0,6)
    mdtDigitMultilayer = ROOT.TH1F("mdtDigitMultilayer","mdtDigitMultilayer;Digits_MDT_Multilayer",4,0,4) 
    mdtDigitTime = ROOT.TH1F("mdtDigitTime","mdtDigitTime;Digits_MDT_time [ns]",100,0,2000)
    mdtDigitCharge = ROOT.TH1F("mdtDigitCharge","mdtDigitCharge;Digits_MDT_charge [e]",100,0,600)
    mdtDigitglobalXY = ROOT.TH2F("mdtDigitglobalXY","mdtDigitglobalXY;Digits_MDT_globalPosX [mm];Digits_MDT_globalPosY [mm]",100,-15000,15000,100,-15000,15000)
    mdtDigitglobalZY = ROOT.TH2F("mdtDigitglobalZY","mdtDigitglobalZY;Digits_MDT_globalPosZ [mm];Digits_MDT_globalPosY [mm]",100,-15000,15000,100,-15000,15000)
    mdtDigitglobalZX = ROOT.TH2F("mdtDigitglobalZX","mdtDigitglobalZX;Digits_MDT_globalPosZ [mm];Digits_MDT_globalPosX [mm]",100,-15000,15000,100,-15000,15000)
    mdtDigitlocalTubePosX = ROOT.TH1F("mdtDigitlocalTubePosX","mdtDigitlocalTubePosX;Digits_MDT_localTubePosX [mm]",100,-100,500)
    mdtDigitlocalTubePosY = ROOT.TH1F("mdtDigitlocalTubePosY","mdtDigitlocalTubePosY;Digits_MDT_localTubePosY [mm]",100,-100,500)
    mdtDigitlocalTubePosZ = ROOT.TH1F("mdtDigitlocalTubePosZ","mdtDigitlocalTubePosZ;Digits_MDT_localTubePosZ [mm]",100,-1200,1200)
    
    # MDT Digit Histograms (BIS mdtDigits only)
    bis_mdtDigitStationEta = ROOT.TH1F("bis_mdtDigitStationEta","bis_mdtDigitStationEta;Digits_bis_MDT_stationEta",20,-10,10)
    bis_mdtDigitStationPhi = ROOT.TH1F("bis_mdtDigitStationPhi","bis_mdtDigitStationPhi;Digits_bis_MDT_stationPhi",10,0,10)
    bis_mdtDigitglobalX = ROOT.TH1F("bis_mdtDigitglobalX","bis_mdtDigitglobalX;Digits_bis_MDT_globalPosX [mm]",100,-5000,5000)
    bis_mdtDigitglobalY = ROOT.TH1F("bis_mdtDigitglobalY","bis_mdtDigitglobalY;Digits_bis_MDT_globalPosY [mm]",100,-5000,5000)
    bis_mdtDigitglobalZ = ROOT.TH1F("bis_mdtDigitglobalZ","bis_mdtDigitglobalZ;Digits_bis_MDT_globalPosZ [mm]",100,-10000,10000)
    bis_mdtDigitNumberOfMultilayers = ROOT.TH1F("bis_mdtDigitNumberOfMultilayers","bis_mdtDigitNumberOfMultilayers;Digits_bis_MDT_numberOfMultilayers",4,0,4)
    bis_mdtDigitTube = ROOT.TH1F("bis_mdtDigitTube","bis_mdtDigitTube;Digits_bis_MDT_tube",100,0,100) 
    bis_mdtDigitTubeLayer = ROOT.TH1F("bis_mdtDigitTubeLayer","bis_mdtDigitTubeLayer;Digits_bis_MDT_tubeLayer",6,0,6)  
    bis_mdtDigitMultilayer = ROOT.TH1F("bis_mdtDigitMultilayer","bis_mdtDigitMultilayer;Digits_bis_MDT_Multilayer",4,0,4) 
    bis_mdtDigitTime = ROOT.TH1F("bis_mdtDigitTime","bis_mdtDigitTime;Digits_bis_MDT_time [ns]",100,0,2000)
    bis_mdtDigitCharge = ROOT.TH1F("bis_mdtDigitCharge","bis_mdtDigitCharge;Digits_bis_MDT_charge [e]",100,0,600)
    bis_mdtDigitChannel = ROOT.TH1F("bis_mdtDigitChannel","bis_mdtDigitChannel;Digits_bis_MDT_channel",10,0,10)
    bis_mdtDigitglobalXY = ROOT.TH2F("bis_mdtDigitglobalXY","bis_mdtDigitglobalXY;Digits_bis_MDT_globalPosX [mm];Digits_bis_MDT_globalPosY [mm]",100,-5000,5000,100,-5000,5000)
    bis_mdtDigitglobalZY = ROOT.TH2F("bis_mdtDigitglobalZY","bis_mdtDigitglobalZY;Digits_bis_MDT_globalPosZ [mm];Digits_bis_MDT_globalPosY [mm]",100,-15000,15000,100,-5000,5000)
    bis_mdtDigitglobalZX = ROOT.TH2F("bis_mdtDigitglobalZX","bis_mdtDigitglobalZX;Digits_bis_MDT_globalPosZ [mm];Digits_bis_MDT_globalPosX [mm]",100,-15000,15000,100,-5000,5000)    
    bis_mdtDigitlocalTubePosX = ROOT.TH1F("bis_mdtDigitlocalTubePosX","bis_mdtDigitlocalTubePosX;Digits_MDT_localTubePosX [mm]",120,-60,60)
    bis_mdtDigitlocalTubePosY = ROOT.TH1F("bis_mdtDigitlocalTubePosY","bis_mdtDigitlocalTubePosY;Digits_MDT_localTubePosY [mm]",960,-900,900)  
    bis_mdtDigitlocalTubePosZ = ROOT.TH1F("bis_mdtDigitlocalTubePosZ","bis_mdtDigitlocalTubePosZ;Digits_MDT_localTubePosZ [mm]",960,-900,900)
    
    bis_mdtDigitlocalTubePosXY = ROOT.TH2F("bis_mdtDigitlocalTubePosXY","bis_mdtDigitlocalTubePosXY;Digits_MDT_localTubePosX [mm];Digits_MDT_localTubePosY [mm]",60,-30,30,100,-100,100)
    bis_mdtDigitlocalTubePosZY = ROOT.TH2F("bis_mdtDigitlocalTubePosZY","bis_mdtDigitlocalTubePosZY;Digits_MDT_localTubePosZ [mm];Digits_MDT_localTubePosY [mm]",100,-1200,1200,100,-100,100)
    bis_mdtDigitlocalTubePosZX = ROOT.TH2F("bis_mdtDigitlocalTubePosZX","bis_mdtDigitlocalTubePosZX;Digits_MDT_localTubePosZ [mm];Digits_MDT_localTubePosX [mm]",48,1,180,60,-30,30)

    
    #############################################################################
    # CSC Sim Histograms
    cscGlobalX = ROOT.TH1F("cscGlobalX","cscGlobalX;CSC_Sim_hitGlobalPositionX",100,-1500,2200)
    cscGlobalY = ROOT.TH1F("cscGlobalY","cscGlobalY;CSC_Sim_hitGlobalPositionY",100,-2000,2000)
    cscGlobalZ = ROOT.TH1F("cscGlobalZ","cscGlobalZ;CSC_Sim_hitGlobalPositionZ",100,-8000,-8000)
    cscWireLayer = ROOT.TH1F("cscWireLayer","cscWireLayer;CSC_Sim_wireLayer",5,0,5)
    cscStrip = ROOT.TH1F("cscStrip","cscStrip;CSC_Sim_strip",3,0,3)
    cscglobalTime = ROOT.TH1F("cscglobalTime","cscglobalTime;CSC_Sim_globalTime",100,0,60)
    cscKineticEnergy = ROOT.TH1F("cscKineticEnergy","cscKineticEnergy;CSC_Sim_kineticEnergy",100,0,400000)
    cscDepositEnergy = ROOT.TH1F("cscDepositEnergy","cscDeposityEnergy;CSC_Sim_depositEnergy",100,0,0.008)
    cscSimStationEta = ROOT.TH1F("cscSimStationEta","cscSimStationEta;CSC_Sim_stationEta",4,-3,1)
    cscSimStationPhi = ROOT.TH1F("cscSimStationPhi","cscSimStationPhi;CSC_Sim_stationPhi",9,0,9)
    cscSimChamberLayer = ROOT.TH1F("cscSimChamberLayer","cscSimChamberLayer;CSC_Sim_chamberLayer",4,0,4)
    # CSC Digit Histograms
    CSCDigitStationEta = ROOT.TH1F("CSCDigitStationEta","CSCDigitStationEta;Digits_CSC_stationEta",4,-3,1)
    CSCDigitStationPhi = ROOT.TH1F("CSCDigitStationPhi","CSCDigitStationPhi;Digits_CSC_stationPhi",9,0,9)
    CSCDigitWireLayer = ROOT.TH1F("CSCDigitWireLayer","CSCDigitWireLayer;Digits_CSC_wireLayer",5,0,5)
    CSCDigitlocalPosX = ROOT.TH1F("CSCDigitlocalPosX","CSCDigitlocalPosX;Digits_CSC_localPosX",100,0,100)
    CSCDigitglobalPosX = ROOT.TH1F("CSCDigitglobalPosX","CSCDigitglobalPosX;Digits_CSC_globalPosX",100,-1600,1600)
    CSCDigitglobalPosY = ROOT.TH1F("CSCDigitglobalPosY","CSCDigitglobalPosY;Digits_CSC_globalPosY",100,-1600,1600)
    CSCDigitglobalPosZ = ROOT.TH1F("CSCDigitglobalPosZ","CSCDigitglobalPosZ;Digits_CSC_globalPosZ",100,-8000,-8000)
    # CSC SDO Histograms
    CSCSDOStationEta = ROOT.TH1F("CSCSDOStationEta","CSCSDOStationEta;SDO_CSC_stationEta",4,-3,1)
    CSCSDOStationPhi = ROOT.TH1F("CSCSDOStationPhi","CSCSDOStationPhi;SDO_CSC_stationPhi",9,0,9)
    CSCSDOChamberLayer = ROOT.TH1F("CSCSDOChamberLayer","CSCSDOChamberLayer;SDO_CSC_chamberLayer",4,0,4)
    CSCSDOWireLayer = ROOT.TH1F("CSCSDOWireLayer","CSCSDOWireLayer;SDO_CSC_wireLayer",5,0,5)
    CSCSDOStrip = ROOT.TH1F("CSCSDOStrip","CSCSDOStrip;SDO_CSC_strip",100,0,200)
    CSCSDOBarcode = ROOT.TH1F("CSCSDOBarcode","CSCSDOBarcode;SDO_CSC_barcode",100,0,250000)
    CSCSDOGlobalX = ROOT.TH1F("CSCSDOGlobalX","CSCSDOGlobalX;SDO_CSC_globalPosX",100,-2000,2000)
    CSCSDOGlobalY = ROOT.TH1F("CSCSDOGlobalY","CSCSDOGlobalY;SDO_CSC_globalPosY",100,-2000,2000)
    CSCSDOGlobalZ = ROOT.TH1F("CSCSDOGlobalZ","CSCSDOGlobalZ;SDO_CSC_globalPosZ",100,-8000,8000)
    CSCSDOLocalX = ROOT.TH1F("CSCSDOLocalX","CSCSDOLocalX;SDO_CSC_localPosX",100,-1000,1000)
    CSCSDOLocalY = ROOT.TH1F("CSCSDOLocalY","CSCSDOLocalY;SDO_CSC_localPosY",100,-1000,1000)
    # CSC RDO Histograms
    CSCRDOStationEta = ROOT.TH1F("CSCRDOStationEta","CSCRDOStationEta;RDO_CSC_stationEta",4,-3,1)
    CSCRDOStationPhi = ROOT.TH1F("CSCRDOStationPhi","CSCRDOStationPhi;RDO_CSC_stationPhi",9,0,9)
    CSCRDOChamberLayer = ROOT.TH1F("CSCRDOChamberLayer","CSCRDOChamberLayer;RDO_CSC_chamberLayer",4,0,4)
    CSCRDOWireLayer = ROOT.TH1F("CSCRDOWireLayer","CSCRDOWireLayer;RDO_CSC_wireLayer",5,0,5)
    CSCRDOStrip = ROOT.TH1F("CSCRDOStrip","CSCRDOStrip;RDO_CSC_strip",100,0,200)
    CSCRDOmeasurePhi = ROOT.TH1F("CSCRDOmeasurePhi","CSCRDOmeasurePhi;RDO_CSC_measurePhi",2,0,2) 
    CSCRDOTime = ROOT.TH1F("CSCRDOTime","CSCRDOTime;RDO_CSC_time",4,-2,2)
    CSCRDOGlobalX = ROOT.TH1F("CSCRDOGlobalX","CSCRDOGlobalX;RDO_CSC_globalPosX",100,-2500,2500)
    CSCRDOGlobalY = ROOT.TH1F("CSCRDOGlobalY","CSCRDOGlobalY;RDO_CSC_globalPosY",100,-2500,2500)
    CSCRDOGlobalZ = ROOT.TH1F("CSCRDOGlobalZ","CSCRDOGlobalZ;RDO_CSC_globalPosZ",100,-8000,8000)
    CSCRDOLocalX = ROOT.TH1F("CSCRDOLocalX","CSCRDOLocalX;RDO_CSC_localPosX",100,0,100)
    CSCRDOLocalY = ROOT.TH1F("CSCRDOLocalY","CSCRDOLocalY;RDO_CSC_localPosY",100,0,100)
    # CSC PRD Histograms
    CSCPRDStationEta = ROOT.TH1F("CSCPRDStationEta","CSCPRDStationEta;PRD_CSC_stationEta",4,-3,1)
    CSCPRDStationPhi = ROOT.TH1F("CSCPRDStationPhi","CSCPRDStationPhi;PRD_CSC_stationPhi",9,0,9)
    CSCPRDChamberLayer = ROOT.TH1F("CSCPRDChamberLayer","CSCPRDChamberLayer;PRD_CSC_chamberLayer",4,0,4)
    CSCPRDWireLayer = ROOT.TH1F("CSCPRDWireLayer","CSCPRDWireLayer;PRD_CSC_wireLayer",5,0,5)
    CSCPRDStrip = ROOT.TH1F("CSCPRDStrip","CSCPRDStrip;PRD_CSC_strip",100,0,200) 
    CSCPRDmeasurePhi = ROOT.TH1F("CSCPRDmeasurePhi","CSCPRDmeasurePhi;PRD_CSC_measurePhi",2,0,2) 
    CSCPRDTime = ROOT.TH1F("CSCPRDTime","CSCPRDTime;PRD_CSC_time",100,-100,1000)
    CSCPRDCharge = ROOT.TH1F("CSCPRDCharge","CSCPRDCharge;PRD_CSC_charge",100,0,10000000)
    CSCPRDGlobalX = ROOT.TH1F("CSCPRDGlobalX","CSCPRDGlobalX;PRD_CSC_globalPosX",100,-2500,2500)
    CSCPRDGlobalY = ROOT.TH1F("CSCPRDGlobalY","CSCPRDGlobalY;PRD_CSC_globalPosY",100,-2500,2500)
    CSCPRDGlobalZ = ROOT.TH1F("CSCPRDGlobalZ","CSCPRDGlobalZ;PRD_CSC_globalPosZ",100,-8000,8000)
    CSCPRDLocalX = ROOT.TH1F("CSCPRDLocalX","CSCPRDLocalX;PRD_CSC_localPosX",100,-1000,1000)
    #############################################################################  
    # TGC Sim Histograms
    tgcLocalX = ROOT.TH1F("tgcLocalX","tgcLocalX;TGC_Sim_hitLocalPositionX",100,-1.5,1.5)
    tgcLocalY = ROOT.TH1F("tgcLocalY","tgcLocalY;TGC_Sim_hitLocalPositionY",100,-800,800)
    tgcLocalZ = ROOT.TH1F("tgcLocalZ","tgcLocalZ;TGC_Sim_hitLocalPositionZ",100,-1200,1200)
    tgcStationEta = ROOT.TH1F("tgcStationEta","tgcStationEta;TGC_Sim_stationEta",12,-6,6)
    tgcStationPhi = ROOT.TH1F("tgcStationPhi","tgcStationPhi;TGC_Sim_stationPhi",50,0,50)
    tgcGlobalX = ROOT.TH1F("tgcGlobalX","tgcGlobalX;TGC_Sim_hitGlobalPositionX",100,-12000,12000)
    tgcGlobalY = ROOT.TH1F("tgcGlobalY","tgcGlobalY;TGC_Sim_hitGlobalPositionY",100,-12000,12000)
    tgcGlobalZ = ROOT.TH1F("tgcGlobalZ","tgcGlobalZ;TGC_Sim_hitGlobalPositionZ",100,-20000,20000)
    tgcGasGap = ROOT.TH1F("tgcGasGap","tgcGasGap;TGC_Sim_gasGap",4,0,4)
    tgcChannel = ROOT.TH1F("tgcChannel","tgcChannel;TGC_Sim_channel",3,0,3)
    tgcGlobalTime = ROOT.TH1F("tgcGlobalTime","tgcGlobalTime;TGC_sim_globalTime",100,0,500)
    tgcKineticEnergy = ROOT.TH1F("tgcKineticEnergy","tgcKineticEnergy;TGC_Sim_kineticEnergy",100,0,400000)
    tgcDepositEnergy = ROOT.TH1F("tgcDepositEnergy","tgcDepositEnergy;TGC_Sim_depositEnergy",100,0,0.002)
    # TGC Digit Histograms
    TGCDigitStationEta = ROOT.TH1F("TGCDigitStationEta","TGCDigitStationEta;Digits_TGC_stationEta",12,-6,6)
    TGCDigitStationPhi = ROOT.TH1F("TGCDigitStationPhi","TGCDigitStationPhi;Digits_TGC_stationPhi",50,0,50)
    TGCDigitGasGap = ROOT.TH1F("TGCDigitGasGap","TGCDigitGasGap;Digits_TGC_GasGap",4,0,4)
    TGCDigitChannel = ROOT.TH1F("TGCDigitChannel","TGCDigitChannel;Digits_TGC_channel",120,0,120)
    TGCDigitlocalX = ROOT.TH1F("TGCDigitlocalX","TGCDigitlocalX;Digits_TGC_localPosX",100,-1500,1500)
    TGCDigitlocalY = ROOT.TH1F("TGCDigitlocalY","TGCDigitlocalY;Digits_TGC_localPosY",100,-1500,1500)
    TGCDigitglobalX = ROOT.TH1F("TGCDigitglobalX","TGCDigitglobalX;Digits_TGC_globalPosX",100,-12000,12000)
    TGCDigitglobalY = ROOT.TH1F("TGCDigitglobalY","TGCDigitglobalY;Digits_TGC_globalPosY",100,-12000,12000)
    TGCDigitglobalZ = ROOT.TH1F("TGCDigitglobalZ","TGCDigitglobalZ;Digits_TGC_globalPosZ",100,-18000,18000)
    # TGC SDO Histograms
    TGCSDOStationEta = ROOT.TH1F("TGCSDOStationEta","TGCSDOStationEta;SDO_TGC_stationEta",12,-6,6)
    TGCSDOStationPhi = ROOT.TH1F("TGCSDOStationPhi","TGCSDOStationPhi;SDO_TGC_stationPhi",50,0,50)
    TGCSDOGasgap = ROOT.TH1F("TGCSDOGasgap","TGCSDOGasgap;SDO_TGC_GasGap",4,0,4)
    TGCSDOChannel = ROOT.TH1F("TGCSDOChannel","TGCSDOChannel;SDO_TGC_channel",100,0,150)
    TGCSDOWord = ROOT.TH1F("TGCSDOWord","TGCSDOWord;SDO_TGC_word",4,-2,2)
    TGCSDOBarcode = ROOT.TH1F("TGCSDOBarcode","TGCSDOBarcode;SDO_TGC_barcode",100,0,200000) 
    TGCSDOGlobalX = ROOT.TH1F("TGCSDOGlobalX","TGCSDOGlobalX;SDO_TGC_globalPosX",100,-15000,15000)
    TGCSDOGlobalY = ROOT.TH1F("TGCSDOGlobalY","TGCSDOGlobalY;SDO_TGC_globalPosY",100,-15000,15000)
    TGCSDOGlobalZ = ROOT.TH1F("TGCSDOGlobalZ","TGCSDOGlobalZ;SDO_TGC_globalPosZ",100,-20000,20000)
    TGCSDOLocalX = ROOT.TH1F("TGCSDOLocalX","TGCSDOLocalX;SDO_TGC_localPosX",100,0,100)
    TGCSDOLocalY = ROOT.TH1F("TGCSDOLocalY","TGCSDOLocalY;SDO_TGC_localPosY",100,0,100)
    TGCSDOGlotime = ROOT.TH1F("TGCSDOGlotime","TGCSDOGlotime;SDO_TGC_global_time",100,0,100)
    # TGC RDO Histograms
    TGCRDOStationEta = ROOT.TH1F("TGCRDOStationEta","TGCRDOStationEta;RDO_TGC_stationEta",12,-6,6)
    TGCRDOStationPhi = ROOT.TH1F("TGCRDOStationPhi","TGCRDOStationPhi;RDO_TGC_stationPhi",50,0,50)
    TGCRDOGasgap = ROOT.TH1F("TGCRDOGasgap","TGCRDOGasgap;RDO_TGC_GasGap",3,0,3)
    TGCRDOChannel = ROOT.TH1F("TGCRDOChannel","TGCRDOChannel;RDO_TGC_channel",3,0,3)
    TGCRDOGlobalX = ROOT.TH1F("TGCRDOGlobalX","TGCRDOGlobalX;RDO_TGC_globalPosX",100,-10000,10000)
    TGCRDOGlobalY = ROOT.TH1F("TGCRDOGlobalY","TGCRDOGlobalY;RDO_TGC_globalPosY",100,-10000,10000)
    TGCRDOGlobalZ = ROOT.TH1F("TGCRDOGlobalZ","TGCRDOGlobalZ;RDO_TGC_globalPosZ",100,-16000,16000)
    TGCRDOLocalX = ROOT.TH1F("TGCRDOLocalX","TGCRDOLocalX;RDO_TGC_localPosX",100,-1200,-400)
    # TGC PRD Histograms
    TGCPRDStationEta = ROOT.TH1F("TGCPRDStationEta","TGCPRDStationEta;PRD_TGC_stationEta",12,-6,6)
    TGCPRDStationPhi = ROOT.TH1F("TGCPRDStationPhi","TGCPRDStationPhi;PRD_TGC_stationPhi",50,0,50)
    TGCPRDGasgap = ROOT.TH1F("TGCPRDGasgap","TGCPRDGasgap;PRD_TGC_gas_gap",4,0,4)
    TGCPRDChannel = ROOT.TH1F("TGCPRDChannel","TGCPRDChannel;PRD_TGC_channel",100,0,150)
    TGCPRDisStrip = ROOT.TH1F("TGCPRDisStrip","TGCPRDisStrip;PRD_TGC_isStrip",2,0,2) 
    TGCPRDGlobalX = ROOT.TH1F("TGCPRDGlobalX","TGCPRDGlobalX;PRD_TGC_globalPosX",100,-15000,15000)
    TGCPRDGlobalY = ROOT.TH1F("TGCPRDGlobalY","TGCPRDGlobalY;PRD_TGC_globalPosY",100,-15000,15000)
    TGCPRDGlobalZ = ROOT.TH1F("TGCPRDGlobalZ","TGCPRDGlobalZ;PRD_TGC_globalPosZ",100,-18000,18000)
    TGCPRDLocalX = ROOT.TH1F("TGCPRDLocalX","TGCPRDLocalX;PRD_TGC_localPosX",100,-1500,1500)
    #############################################################################
    # MMs
    mmGlobalX = ROOT.TH1F("mmGlobalX","mmGlobalX;MM_Sim_hitGlobalPositionX",100,-5000,5000)
    mmGlobalY = ROOT.TH1F("mmGlobalY","mmGlobalY;MM_Sim_hitGlobalPositionY",100,-5000,5000)
    mmGlobalZ = ROOT.TH1F("mmGlobalZ","mmGlobalZ;MM_Sim_hitGlobalPositionZ",100,7000,8000)
    mmStationEta = ROOT.TH1F("mmStationEta","mmStationEta;Hits_MM_off_stationEta",3,0,3)
    mmStationPhi = ROOT.TH1F("mmStationPhi","mmStationPhi;Hits_MM_off_stationPhi",9,0,9)
    #############################################################################
    # sTGCs
    stgcGlobalX = ROOT.TH1F("stgcGlobalX","stgcGlobalX;sTGC_Sim_hitGlobalPositionX",100,-5000,5000)
    stgcGlobalY = ROOT.TH1F("stgcGlobalY","stgcGlobalY;sTGC_Sim_hitGlobalPositionY",100,-5000,5000)
    stgcGlobalZ = ROOT.TH1F("stgcGlobalZ","stgcGlobalZ;sTGC_Sim_hitGlobalPositionZ",100,6000,8000)
    stgcStationEta = ROOT.TH1F("stgcStationEta","stgcStationEta;Hits_sTGC_off_stationEta",4,0,4)
    stgcStationPhi = ROOT.TH1F("stgcStationPhi","stgcStationPhi;Hits_sTGC_off_stationPhi",9,0,9)
    #############################################################################



    def __init__(self, chamber_name="", eta_sel = None, sector_sel = None):
        self.__chamber_name = chamber_name
        self.__eta_sel   = eta_sel
        self.__sector_sel   = sector_sel

    # For negative eta x, ord() have returned values of (256 + x).
    def Eta(eta):
        if eta > 9:
            return (eta - 256)
        else:
            return eta
                     
    def fill(self, TTree, n):
    
        if self.__chamber_name == "TruthInfo":   
            MyHistoFiller.truthPtHist.Fill(TTree.MuEntry_Particle_Pt[n]*0.001)
            MyHistoFiller.truthEtaHist.Fill(TTree.MuEntry_Particle_Eta[n])
            MyHistoFiller.truthPhiHist.Fill(TTree.MuEntry_Particle_Phi[n])

        if self.__chamber_name == "RPC_Sim":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return      
            else:
                MyHistoFiller.rpcLocalX.Fill(TTree.RPC_SIM_localPostionX[n])
                MyHistoFiller.rpcLocalY.Fill(TTree.RPC_SIM_localPostionY[n])
                MyHistoFiller.rpcStationEta.Fill(MyHistoFiller.Eta(ord(TTree.RPC_SIM_stationEta[n])))
                MyHistoFiller.rpcStationPhi.Fill(ord(TTree.RPC_SIM_stationPhi[n]))
                MyHistoFiller.rpcGlobalX.Fill(TTree.RPC_SIM_GlobalPositionX[n])
                MyHistoFiller.rpcGlobalY.Fill(TTree.RPC_SIM_GlobalPositionY[n])
                MyHistoFiller.rpcGlobalZ.Fill(TTree.RPC_SIM_GlobalPositionZ[n])
                MyHistoFiller.rpcGasGap.Fill(ord(TTree.RPC_SIM_gasGap[n]))
                
        if self.__chamber_name == "RPC_Digit":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return      
            else:
                MyHistoFiller.rpcDigitlocalX.Fill(TTree.Digits_RPC_localPosX[n])
                MyHistoFiller.rpcDigitlocalY.Fill(TTree.Digits_RPC_localPosY[n])
                MyHistoFiller.rpcDigitStationEta.Fill(MyHistoFiller.Eta(ord(TTree.Digits_RPC_stationEta[n])))
                MyHistoFiller.rpcDigitStationPhi.Fill(ord(TTree.Digits_RPC_stationPhi[n]))
                MyHistoFiller.rpcDigitglobalX.Fill(TTree.Digits_RPC_globalPosX[n])
                MyHistoFiller.rpcDigitglobalY.Fill(TTree.Digits_RPC_globalPosY[n])
                MyHistoFiller.rpcDigitglobalZ.Fill(TTree.Digits_RPC_globalPosZ[n])
                MyHistoFiller.rpcDigitGasGap.Fill(ord(TTree.Digits_RPC_gasGap[n]))
                MyHistoFiller.rpcDigitdoubletR.Fill(ord(TTree.Digits_RPC_doubletR[n]))
                MyHistoFiller.rpcDigitdoubletZ.Fill(ord(TTree.Digits_RPC_doubletZ[n]))
                MyHistoFiller.rpcDigitdoubletPhi.Fill(ord(TTree.Digits_RPC_doubletPhi[n]))
                MyHistoFiller.rpcDigitMeasuresPhi.Fill(TTree.Digits_RPC_measuresPhi[n])
                MyHistoFiller.rpcDigitTime.Fill(TTree.Digits_RPC_time[n])
                MyHistoFiller.rpcDigitStrip.Fill(ord(TTree.Digits_RPC_strip[n]))
                MyHistoFiller.rpcDigitStripNumber.Fill(TTree.Digits_RPC_stripNumber[n])
                MyHistoFiller.rpcDigitglobalXY.Fill(TTree.Digits_RPC_globalPosX[n],TTree.Digits_RPC_globalPosY[n])
                MyHistoFiller.rpcDigitglobalZY.Fill(TTree.Digits_RPC_globalPosZ[n],TTree.Digits_RPC_globalPosY[n])
                MyHistoFiller.rpcDigitglobalZX.Fill(TTree.Digits_RPC_globalPosZ[n],TTree.Digits_RPC_globalPosX[n])

                # Cut on BIS78 modules only -- remove stationEta condition to also plot BIS16 in the future 
                if TTree.Digits_RPC_stationName[n]=="BIS" and MyHistoFiller.Eta(ord(TTree.Digits_RPC_stationEta[n]))>6: 
                    #use measuresPhi=0 to extract local eta/Y coordinate from localX
                    if TTree.Digits_RPC_measuresPhi[n]==0:
                        MyHistoFiller.bis_rpcDigitlocalY.Fill(TTree.Digits_RPC_localPosX[n])
                    #use measuresPhi=1 to extract local phi/X coordinate from localX
                    elif TTree.Digits_RPC_measuresPhi[n]==1:
                        MyHistoFiller.bis_rpcDigitlocalX.Fill(TTree.Digits_RPC_localPosX[n]) 
                        
                    #use globalZ<6700 to get BIS7 
                    if TTree.Digits_RPC_globalPosZ[n]<6700:
                        MyHistoFiller.bis7_rpcDigitTime.Fill(TTree.Digits_RPC_time[n])
                        if TTree.Digits_RPC_measuresPhi[n]==0:
                            MyHistoFiller.bis7_rpcDigitlocalY.Fill(TTree.Digits_RPC_localPosX[n])
                        elif TTree.Digits_RPC_measuresPhi[n]==1:
                            MyHistoFiller.bis7_rpcDigitlocalX.Fill(TTree.Digits_RPC_localPosX[n])
                    else:
                    #use globalZ>6700 to get BIS8 
                        MyHistoFiller.bis8_rpcDigitTime.Fill(TTree.Digits_RPC_time[n])    
                        if TTree.Digits_RPC_measuresPhi[n]==0:
                            MyHistoFiller.bis8_rpcDigitlocalY.Fill(TTree.Digits_RPC_localPosX[n])
                        elif TTree.Digits_RPC_measuresPhi[n]==1:
                            MyHistoFiller.bis8_rpcDigitlocalX.Fill(TTree.Digits_RPC_localPosX[n])
                            
                    MyHistoFiller.bis_rpcDigitStationEta.Fill(MyHistoFiller.Eta(ord(TTree.Digits_RPC_stationEta[n])))
                    MyHistoFiller.bis_rpcDigitStationPhi.Fill(ord(TTree.Digits_RPC_stationPhi[n]))
                    MyHistoFiller.bis_rpcDigitglobalX.Fill(TTree.Digits_RPC_globalPosX[n])
                    MyHistoFiller.bis_rpcDigitglobalY.Fill(TTree.Digits_RPC_globalPosY[n])
                    MyHistoFiller.bis_rpcDigitglobalZ.Fill(TTree.Digits_RPC_globalPosZ[n])
                    MyHistoFiller.bis_rpcDigitGasGap.Fill(ord(TTree.Digits_RPC_gasGap[n]))
                    MyHistoFiller.bis_rpcDigitdoubletR.Fill(ord(TTree.Digits_RPC_doubletR[n]))
                    MyHistoFiller.bis_rpcDigitdoubletZ.Fill(ord(TTree.Digits_RPC_doubletZ[n]))
                    MyHistoFiller.bis_rpcDigitdoubletPhi.Fill(ord(TTree.Digits_RPC_doubletPhi[n]))
                    MyHistoFiller.bis_rpcDigitMeasuresPhi.Fill(TTree.Digits_RPC_measuresPhi[n])
                    MyHistoFiller.bis_rpcDigitTime.Fill(TTree.Digits_RPC_time[n])
                    MyHistoFiller.bis_rpcDigitStrip.Fill(ord(TTree.Digits_RPC_strip[n]))
                    MyHistoFiller.bis_rpcDigitStripNumber.Fill(TTree.Digits_RPC_stripNumber[n])
                    MyHistoFiller.bis_rpcDigitglobalXY.Fill(TTree.Digits_RPC_globalPosX[n],TTree.Digits_RPC_globalPosY[n])
                    MyHistoFiller.bis_rpcDigitglobalZY.Fill(TTree.Digits_RPC_globalPosZ[n],TTree.Digits_RPC_globalPosY[n])
                    MyHistoFiller.bis_rpcDigitglobalZX.Fill(TTree.Digits_RPC_globalPosZ[n],TTree.Digits_RPC_globalPosX[n])
            

        if self.__chamber_name == "MDT_Sim":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.mdtLocalX.Fill(TTree.MDT_Sim_hitLocalPositionX[n])
                MyHistoFiller.mdtLocalY.Fill(TTree.MDT_Sim_hitLocalPositionY[n])
                MyHistoFiller.mdtLocalZ.Fill(TTree.MDT_Sim_hitLocalPositionZ[n])
                MyHistoFiller.mdtStationEta.Fill(MyHistoFiller.Eta(ord(TTree.MDT_Sim_stationEta[n])))
                MyHistoFiller.mdtStationPhi.Fill(ord(TTree.MDT_Sim_stationPhi[n]))
                MyHistoFiller.mdtGlobalX.Fill(TTree.MDT_Sim_hitGlobalPositionX[n])
                MyHistoFiller.mdtGlobalY.Fill(TTree.MDT_Sim_hitGlobalPositionY[n])
                MyHistoFiller.mdtGlobalZ.Fill(TTree.MDT_Sim_hitGlobalPositionZ[n])
                MyHistoFiller.mdtTube.Fill(ord(TTree.MDT_Sim_tube[n]))
                
        if self.__chamber_name == "MDT_Digit":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.mdtDigitlocalTubePosX.Fill(TTree.Digits_MDT_localTubePosX[n])
                MyHistoFiller.mdtDigitlocalTubePosY.Fill(TTree.Digits_MDT_localTubePosY[n])
                MyHistoFiller.mdtDigitlocalTubePosZ.Fill(TTree.Digits_MDT_localTubePosZ[n])
                MyHistoFiller.mdtDigitStationEta.Fill(MyHistoFiller.Eta(ord(TTree.Digits_MDT_stationEta[n])))
                MyHistoFiller.mdtDigitStationPhi.Fill(ord(TTree.Digits_MDT_stationPhi[n]))
                MyHistoFiller.mdtDigitglobalX.Fill(TTree.Digits_MDT_globalPosX[n])
                MyHistoFiller.mdtDigitglobalY.Fill(TTree.Digits_MDT_globalPosY[n])
                MyHistoFiller.mdtDigitglobalZ.Fill(TTree.Digits_MDT_globalPosZ[n])  
                MyHistoFiller.mdtDigitNumberOfMultilayers.Fill(TTree.Digits_MDT_numberOfMultilayers[n])
                MyHistoFiller.mdtDigitTube.Fill(ord(TTree.Digits_MDT_tube[n]))
                MyHistoFiller.mdtDigitTubeLayer.Fill(ord(TTree.Digits_MDT_tubeLayer[n]))
                MyHistoFiller.mdtDigitMultilayer.Fill(ord(TTree.Digits_MDT_multiLayer[n]))
                MyHistoFiller.mdtDigitTime.Fill(TTree.Digits_MDT_time[n])
                MyHistoFiller.mdtDigitCharge.Fill(TTree.Digits_MDT_charge[n])
                MyHistoFiller.mdtDigitglobalXY.Fill(TTree.Digits_MDT_globalPosX[n],TTree.Digits_MDT_globalPosY[n])
                MyHistoFiller.mdtDigitglobalZY.Fill(TTree.Digits_MDT_globalPosZ[n],TTree.Digits_MDT_globalPosY[n])
                MyHistoFiller.mdtDigitglobalZX.Fill(TTree.Digits_MDT_globalPosZ[n],TTree.Digits_MDT_globalPosX[n])
                # Cut on BIS78 modules only
                if TTree.Digits_MDT_stationName[n]=="BIS" and MyHistoFiller.Eta(ord(TTree.Digits_MDT_stationEta[n]))>6: 
                   MyHistoFiller.bis_mdtDigitlocalTubePosX.Fill(TTree.Digits_MDT_localTubePosX[n])
                   MyHistoFiller.bis_mdtDigitlocalTubePosY.Fill(TTree.Digits_MDT_localTubePosY[n])
                   MyHistoFiller.bis_mdtDigitlocalTubePosZ.Fill(TTree.Digits_MDT_localTubePosZ[n])
                   MyHistoFiller.bis_mdtDigitStationEta.Fill(MyHistoFiller.Eta(ord(TTree.Digits_MDT_stationEta[n])))
                   MyHistoFiller.bis_mdtDigitStationPhi.Fill(ord(TTree.Digits_MDT_stationPhi[n]))
                   MyHistoFiller.bis_mdtDigitglobalX.Fill(TTree.Digits_MDT_globalPosX[n])
                   MyHistoFiller.bis_mdtDigitglobalY.Fill(TTree.Digits_MDT_globalPosY[n])
                   MyHistoFiller.bis_mdtDigitglobalZ.Fill(TTree.Digits_MDT_globalPosZ[n])  
                   MyHistoFiller.bis_mdtDigitNumberOfMultilayers.Fill(TTree.Digits_MDT_numberOfMultilayers[n])
                   MyHistoFiller.bis_mdtDigitTube.Fill(ord(TTree.Digits_MDT_tube[n]))
                   MyHistoFiller.bis_mdtDigitTubeLayer.Fill(ord(TTree.Digits_MDT_tubeLayer[n]))
                   MyHistoFiller.bis_mdtDigitMultilayer.Fill(ord(TTree.Digits_MDT_multiLayer[n]))
                   MyHistoFiller.bis_mdtDigitTime.Fill(TTree.Digits_MDT_time[n])
                   MyHistoFiller.bis_mdtDigitCharge.Fill(TTree.Digits_MDT_charge[n])
                   MyHistoFiller.bis_mdtDigitglobalXY.Fill(TTree.Digits_MDT_globalPosX[n],TTree.Digits_MDT_globalPosY[n])
                   MyHistoFiller.bis_mdtDigitglobalZY.Fill(TTree.Digits_MDT_globalPosZ[n],TTree.Digits_MDT_globalPosY[n])
                   MyHistoFiller.bis_mdtDigitglobalZX.Fill(TTree.Digits_MDT_globalPosZ[n],TTree.Digits_MDT_globalPosX[n])
                   MyHistoFiller.bis_mdtDigitlocalTubePosXY.Fill(TTree.Digits_MDT_localTubePosX[n],TTree.Digits_MDT_localTubePosY[n])
                   MyHistoFiller.bis_mdtDigitlocalTubePosZY.Fill(TTree.Digits_MDT_localTubePosZ[n],TTree.Digits_MDT_localTubePosY[n])
                   MyHistoFiller.bis_mdtDigitlocalTubePosZX.Fill(TTree.Digits_MDT_localTubePosZ[n],TTree.Digits_MDT_localTubePosX[n])
            

        if self.__chamber_name == "CSC_Sim":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.cscGlobalX.Fill(TTree.CSC_Sim_hitGlobalPositionX[n])
                MyHistoFiller.cscGlobalY.Fill(TTree.CSC_Sim_hitGlobalPositionY[n])
                MyHistoFiller.cscGlobalZ.Fill(TTree.CSC_Sim_hitGlobalPositionZ[n])
                MyHistoFiller.cscWireLayer.Fill(ord(TTree.CSC_Sim_wireLayer[n]))
                MyHistoFiller.cscStrip.Fill(ord(TTree.CSC_Sim_strip[n]))
                MyHistoFiller.cscglobalTime.Fill(TTree.CSC_Sim_globalTime[n])
                MyHistoFiller.cscKineticEnergy.Fill(TTree.CSC_Sim_kineticEnergy[n])
                MyHistoFiller.cscDepositEnergy.Fill(TTree.CSC_Sim_depositEnergy[n])
                MyHistoFiller.cscSimStationEta.Fill(MyHistoFiller.Eta(ord(TTree.CSC_Sim_stationEta[n])))
                MyHistoFiller.cscSimStationPhi.Fill(ord(TTree.CSC_Sim_stationPhi[n]))
                MyHistoFiller.cscSimChamberLayer.Fill(ord(TTree.CSC_Sim_chamberLayer[n]))
                
        if self.__chamber_name == "CSC_Digit":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.CSCDigitStationEta.Fill(MyHistoFiller.Eta(ord(TTree.Digits_CSC_stationEta[n])))
                MyHistoFiller.CSCDigitStationPhi.Fill(ord(TTree.Digits_CSC_stationPhi[n]))
                MyHistoFiller.CSCDigitWireLayer.Fill(ord(TTree.Digits_CSC_wireLayer[n]))
                MyHistoFiller.CSCDigitlocalPosX.Fill(TTree.Digits_CSC_localPosX[n])
                MyHistoFiller.CSCDigitglobalPosX.Fill(TTree.Digits_CSC_globalPosX[n])
                MyHistoFiller.CSCDigitglobalPosY.Fill(TTree.Digits_CSC_globalPosY[n])
                MyHistoFiller.CSCDigitglobalPosZ.Fill(TTree.Digits_CSC_globalPosZ[n])

        if self.__chamber_name == "CSC_SDO":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.CSCSDOStationEta.Fill(MyHistoFiller.Eta(ord(TTree.SDO_CSC_stationEta[n])))
                MyHistoFiller.CSCSDOStationPhi.Fill(ord(TTree.SDO_CSC_stationPhi[n]))
                MyHistoFiller.CSCSDOChamberLayer.Fill(ord(TTree.SDO_CSC_chamberLayer[n]))
                MyHistoFiller.CSCSDOWireLayer.Fill(ord(TTree.SDO_CSC_wireLayer[n]))
                MyHistoFiller.CSCSDOStrip.Fill(ord(TTree.SDO_CSC_strip[n]))
                MyHistoFiller.CSCSDOBarcode.Fill(TTree.SDO_CSC_barcode[n])
                MyHistoFiller.CSCSDOGlobalX.Fill(TTree.SDO_CSC_globalPosX[n])
                MyHistoFiller.CSCSDOGlobalY.Fill(TTree.SDO_CSC_globalPosY[n])
                MyHistoFiller.CSCSDOGlobalZ.Fill(TTree.SDO_CSC_globalPosZ[n])
                MyHistoFiller.CSCSDOLocalX.Fill(TTree.SDO_CSC_localPosX[n])
                MyHistoFiller.CSCSDOLocalY.Fill(TTree.SDO_CSC_localPosY[n])

        if self.__chamber_name == "CSC_RDO":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.CSCRDOStationEta.Fill(MyHistoFiller.Eta(ord(TTree.RDO_CSC_stationEta[n])))
                MyHistoFiller.CSCRDOStationPhi.Fill(ord(TTree.RDO_CSC_stationPhi[n]))
                MyHistoFiller.CSCRDOChamberLayer.Fill(ord(TTree.RDO_CSC_chamberLayer[n]))
                MyHistoFiller.CSCRDOWireLayer.Fill(ord(TTree.RDO_CSC_wireLayer[n]))
                MyHistoFiller.CSCRDOStrip.Fill(ord(TTree.RDO_CSC_strip[n]))
                MyHistoFiller.CSCRDOmeasurePhi.Fill(TTree.RDO_CSC_measuresPhi[n])
                MyHistoFiller.CSCRDOTime.Fill(TTree.RDO_CSC_time[n])
                MyHistoFiller.CSCRDOGlobalX.Fill(TTree.RDO_CSC_globalPosX[n])
                MyHistoFiller.CSCRDOGlobalY.Fill(TTree.RDO_CSC_globalPosY[n])
                MyHistoFiller.CSCRDOGlobalZ.Fill(TTree.RDO_CSC_globalPosZ[n])
                MyHistoFiller.CSCRDOLocalX.Fill(TTree.RDO_CSC_localPosX[n])
                MyHistoFiller.CSCRDOLocalY.Fill(TTree.RDO_CSC_localPosY[n])
                
        if self.__chamber_name == "CSC_PRD":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.CSCPRDStationEta.Fill(MyHistoFiller.Eta(ord(TTree.PRD_CSC_stationEta[n])))
                MyHistoFiller.CSCPRDStationPhi.Fill(ord(TTree.PRD_CSC_stationPhi[n]))
                MyHistoFiller.CSCPRDChamberLayer.Fill(ord(TTree.PRD_CSC_chlayer[n]))
                MyHistoFiller.CSCPRDWireLayer.Fill(ord(TTree.PRD_CSC_wlayer[n]))
                MyHistoFiller.CSCPRDStrip.Fill(ord(TTree.PRD_CSC_strip[n]))
                MyHistoFiller.CSCPRDmeasurePhi.Fill(TTree.PRD_CSC_measuresPhi[n])
                MyHistoFiller.CSCPRDTime.Fill(TTree.PRD_CSC_time[n])
                MyHistoFiller.CSCPRDCharge.Fill(TTree.PRD_CSC_charge[n])
                MyHistoFiller.CSCPRDGlobalX.Fill(TTree.PRD_CSC_globalPosX[n])
                MyHistoFiller.CSCPRDGlobalY.Fill(TTree.PRD_CSC_globalPosY[n])
                MyHistoFiller.CSCPRDGlobalZ.Fill(TTree.PRD_CSC_globalPosZ[n])
                MyHistoFiller.CSCPRDLocalX.Fill(TTree.PRD_CSC_localPosX[n])

        if self.__chamber_name == "TGC_Sim":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.tgcLocalX.Fill(TTree.TGC_Sim_hitLocalPositionX[n])
                MyHistoFiller.tgcLocalY.Fill(TTree.TGC_Sim_hitLocalPositionY[n])
                MyHistoFiller.tgcLocalZ.Fill(TTree.TGC_Sim_hitLocalPositionZ[n])
                MyHistoFiller.tgcStationEta.Fill(MyHistoFiller.Eta(ord(TTree.TGC_Sim_stationEta[n])))
                MyHistoFiller.tgcStationPhi.Fill(ord(TTree.TGC_Sim_stationPhi[n]))
                MyHistoFiller.tgcGlobalX.Fill(TTree.TGC_Sim_hitGlobalPositionX[n])
                MyHistoFiller.tgcGlobalY.Fill(TTree.TGC_Sim_hitGlobalPositionY[n])
                MyHistoFiller.tgcGlobalZ.Fill(TTree.TGC_Sim_hitGlobalPositionZ[n])
                MyHistoFiller.tgcGasGap.Fill(ord(TTree.TGC_Sim_GasGap[n]))
                MyHistoFiller.tgcChannel.Fill(ord(TTree.TGC_Sim_channel[n]))
                MyHistoFiller.tgcGlobalTime.Fill(TTree.TGC_Sim_globalTime[n])
                MyHistoFiller.tgcKineticEnergy.Fill(TTree.TGC_Sim_kineticEnergy[n])
                MyHistoFiller.tgcDepositEnergy.Fill(TTree.TGC_Sim_depositEnergy[n])

        if self.__chamber_name == "TGC_Digit":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.TGCDigitStationEta.Fill(MyHistoFiller.Eta(ord(TTree.Digits_TGC_stationEta[n])))
                MyHistoFiller.TGCDigitStationPhi.Fill(ord(TTree.Digits_TGC_stationPhi[n]))
                MyHistoFiller.TGCDigitGasGap.Fill(ord(TTree.Digits_TGC_GasGap[n]))
                MyHistoFiller.TGCDigitChannel.Fill(ord(TTree.Digits_TGC_channel[n]))
                MyHistoFiller.TGCDigitlocalX.Fill(TTree.Digits_TGC_localPosX[n])
                MyHistoFiller.TGCDigitlocalY.Fill(TTree.Digits_TGC_localPosY[n])
                MyHistoFiller.TGCDigitglobalX.Fill(TTree.Digits_TGC_globalPosX[n])
                MyHistoFiller.TGCDigitglobalY.Fill(TTree.Digits_TGC_globalPosY[n])
                MyHistoFiller.TGCDigitglobalZ.Fill(TTree.Digits_TGC_globalPosZ[n])
                
        if self.__chamber_name == "TGC_SDO":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.TGCSDOStationEta.Fill(MyHistoFiller.Eta(ord(TTree.SDO_TGC_stationEta[n])))
                MyHistoFiller.TGCSDOStationPhi.Fill(ord(TTree.SDO_TGC_stationPhi[n]))
                MyHistoFiller.TGCSDOGasgap.Fill(ord(TTree.SDO_TGC_GasGap[n]))
                MyHistoFiller.TGCSDOChannel.Fill(ord(TTree.SDO_TGC_channel[n]))
                MyHistoFiller.TGCSDOWord.Fill(TTree.SDO_TGC_word[n])
                MyHistoFiller.TGCSDOBarcode.Fill(TTree.SDO_TGC_barcode[n])
                MyHistoFiller.TGCSDOGlobalX.Fill(TTree.SDO_TGC_globalPosX[n])
                MyHistoFiller.TGCSDOGlobalY.Fill(TTree.SDO_TGC_globalPosY[n])
                MyHistoFiller.TGCSDOGlobalZ.Fill(TTree.SDO_TGC_globalPosZ[n])
                MyHistoFiller.TGCSDOLocalX.Fill(TTree.SDO_TGC_localPosX[n])
                MyHistoFiller.TGCSDOLocalY.Fill(TTree.SDO_TGC_localPosY[n])
                MyHistoFiller.TGCSDOGlotime.Fill(TTree.SDO_TGC_global_time[n])
                
        if self.__chamber_name == "TGC_RDO":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.TGCRDOStationEta.Fill(MyHistoFiller.Eta(ord(TTree.RDO_TGC_stationEta[n])))
                MyHistoFiller.TGCRDOStationPhi.Fill(ord(TTree.RDO_TGC_stationPhi[n]))
                MyHistoFiller.TGCRDOGasgap.Fill(ord(TTree.RDO_TGC_GasGap[n]))
                MyHistoFiller.TGCRDOChannel.Fill(ord(TTree.RDO_TGC_channel[n]))
                MyHistoFiller.TGCRDOGlobalX.Fill(TTree.RDO_TGC_globalPosX[n])
                MyHistoFiller.TGCRDOGlobalY.Fill(TTree.RDO_TGC_globalPosY[n])
                MyHistoFiller.TGCRDOGlobalZ.Fill(TTree.RDO_TGC_globalPosZ[n])
                MyHistoFiller.TGCRDOLocalX.Fill(TTree.RDO_TGC_localPosX[n])

        if self.__chamber_name == "TGC_PRD":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.TGCPRDStationEta.Fill(MyHistoFiller.Eta(ord(TTree.PRD_TGC_stationEta[n])))
                MyHistoFiller.TGCPRDStationPhi.Fill(ord(TTree.PRD_TGC_stationPhi[n]))
                MyHistoFiller.TGCPRDGasgap.Fill(ord(TTree.PRD_TGC_gasGap[n]))
                MyHistoFiller.TGCPRDChannel.Fill(ord(TTree.PRD_TGC_channel[n]))
                MyHistoFiller.TGCPRDisStrip.Fill(TTree.PRD_TGC_isStrip[n])
                MyHistoFiller.TGCPRDGlobalX.Fill(TTree.PRD_TGC_globalPosX[n])
                MyHistoFiller.TGCPRDGlobalY.Fill(TTree.PRD_TGC_globalPosY[n])
                MyHistoFiller.TGCPRDGlobalZ.Fill(TTree.PRD_TGC_globalPosZ[n])
                MyHistoFiller.TGCPRDLocalX.Fill(TTree.PRD_TGC_localPosX[n])

        if self.__chamber_name == "MM_Sim":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.mmGlobalX.Fill(TTree.Hits_MM_GlobalPositionX[n])
                MyHistoFiller.mmGlobalY.Fill(TTree.Hits_MM_GlobalPositionY[n])
                MyHistoFiller.mmGlobalZ.Fill(TTree.Hits_MM_GlobalPositionZ[n])
                MyHistoFiller.mmStationEta.Fill(MyHistoFiller.Eta(ord(TTree.Hits_MM_off_stationEta[n])))
                MyHistoFiller.mmStationPhi.Fill(ord(TTree.Hits_MM_off_stationPhi[n]))

        if self.__chamber_name == "sTGC_Sim":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.stgcGlobalX.Fill(TTree.Hits_sTGC_GlobalPositionX[n])
                MyHistoFiller.stgcGlobalY.Fill(TTree.Hits_sTGC_GlobalPositionY[n])
                MyHistoFiller.stgcGlobalZ.Fill(TTree.Hits_sTGC_GlobalPositionZ[n])
                MyHistoFiller.stgcStationEta.Fill(MyHistoFiller.Eta(ord(TTree.Hits_sTGC_off_stationEta[n])))
                MyHistoFiller.stgcStationPhi.Fill(ord(TTree.Hits_sTGC_off_stationPhi[n]))


    def write(self, outdir):
    
        if self.__chamber_name == "TruthInfo":
            outdir.WriteTObject(MyHistoFiller.truthPtHist, MyHistoFiller.truthPtHist.GetName())
            outdir.WriteTObject(MyHistoFiller.truthEtaHist, MyHistoFiller.truthEtaHist.GetName())
            outdir.WriteTObject(MyHistoFiller.truthPhiHist, MyHistoFiller.truthPhiHist.GetName())
    
        if self.__chamber_name == "RPC_Sim":
            outdir.WriteTObject(MyHistoFiller.rpcLocalX, MyHistoFiller.rpcLocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcLocalY, MyHistoFiller.rpcLocalY.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcStationEta, MyHistoFiller.rpcStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcStationPhi, MyHistoFiller.rpcStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcGlobalX, MyHistoFiller.rpcGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcGlobalY, MyHistoFiller.rpcGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcGlobalZ, MyHistoFiller.rpcGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcGasGap, MyHistoFiller.rpcGasGap.GetName())
            
        if self.__chamber_name == "RPC_Digit":
            outdir.WriteTObject(MyHistoFiller.rpcDigitlocalX, MyHistoFiller.rpcDigitlocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitlocalY, MyHistoFiller.rpcDigitlocalY.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitStationEta, MyHistoFiller.rpcDigitStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitStationPhi, MyHistoFiller.rpcDigitStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitglobalX, MyHistoFiller.rpcDigitglobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitglobalY, MyHistoFiller.rpcDigitglobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitglobalZ, MyHistoFiller.rpcDigitglobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitGasGap, MyHistoFiller.rpcDigitGasGap.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitStrip, MyHistoFiller.rpcDigitStrip.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitStripNumber, MyHistoFiller.rpcDigitStripNumber.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitMeasuresPhi, MyHistoFiller.rpcDigitMeasuresPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitTime, MyHistoFiller.rpcDigitTime.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitdoubletR, MyHistoFiller.rpcDigitdoubletR.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitdoubletZ, MyHistoFiller.rpcDigitdoubletZ.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitdoubletPhi, MyHistoFiller.rpcDigitdoubletPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitglobalXY, MyHistoFiller.rpcDigitglobalXY.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitglobalZY, MyHistoFiller.rpcDigitglobalZY.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcDigitglobalZX, MyHistoFiller.rpcDigitglobalZX.GetName())
            # BIS rpcDigit
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitlocalX, MyHistoFiller.bis_rpcDigitlocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitlocalY, MyHistoFiller.bis_rpcDigitlocalY.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitStationEta, MyHistoFiller.bis_rpcDigitStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitStationPhi, MyHistoFiller.bis_rpcDigitStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitglobalX, MyHistoFiller.bis_rpcDigitglobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitglobalY, MyHistoFiller.bis_rpcDigitglobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitglobalZ, MyHistoFiller.bis_rpcDigitglobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitGasGap, MyHistoFiller.bis_rpcDigitGasGap.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitStrip, MyHistoFiller.bis_rpcDigitStrip.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitStripNumber, MyHistoFiller.bis_rpcDigitStripNumber.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitMeasuresPhi, MyHistoFiller.bis_rpcDigitMeasuresPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitTime, MyHistoFiller.bis_rpcDigitTime.GetName())
            outdir.WriteTObject(MyHistoFiller.bis7_rpcDigitTime, MyHistoFiller.bis7_rpcDigitTime.GetName())
            outdir.WriteTObject(MyHistoFiller.bis8_rpcDigitTime, MyHistoFiller.bis8_rpcDigitTime.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitdoubletR, MyHistoFiller.bis_rpcDigitdoubletR.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitdoubletZ, MyHistoFiller.bis_rpcDigitdoubletZ.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitdoubletPhi, MyHistoFiller.bis_rpcDigitdoubletPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitglobalXY, MyHistoFiller.bis_rpcDigitglobalXY.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitglobalZY, MyHistoFiller.bis_rpcDigitglobalZY.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_rpcDigitglobalZX, MyHistoFiller.bis_rpcDigitglobalZX.GetName())

        if self.__chamber_name == "MDT_Sim":
            outdir.WriteTObject(MyHistoFiller.mdtLocalX, MyHistoFiller.mdtLocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtLocalY, MyHistoFiller.mdtLocalY.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtLocalZ, MyHistoFiller.mdtLocalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtStationEta, MyHistoFiller.mdtStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtStationPhi, MyHistoFiller.mdtStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtGlobalX, MyHistoFiller.mdtGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtGlobalY, MyHistoFiller.mdtGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtGlobalZ, MyHistoFiller.mdtGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtTube, MyHistoFiller.mdtTube.GetName())

        if self.__chamber_name == "MDT_Digit":
            outdir.WriteTObject(MyHistoFiller.mdtDigitlocalTubePosX, MyHistoFiller.mdtDigitlocalTubePosX.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitlocalTubePosY, MyHistoFiller.mdtDigitlocalTubePosY.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitlocalTubePosZ, MyHistoFiller.mdtDigitlocalTubePosZ.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitStationEta, MyHistoFiller.mdtDigitStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitStationPhi, MyHistoFiller.mdtDigitStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitglobalX, MyHistoFiller.mdtDigitglobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitglobalY, MyHistoFiller.mdtDigitglobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitglobalZ, MyHistoFiller.mdtDigitglobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitNumberOfMultilayers, MyHistoFiller.mdtDigitNumberOfMultilayers.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitTube, MyHistoFiller.mdtDigitTube.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitTubeLayer, MyHistoFiller.mdtDigitTubeLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitMultilayer, MyHistoFiller.mdtDigitMultilayer.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitTime, MyHistoFiller.mdtDigitTime.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitCharge, MyHistoFiller.mdtDigitCharge.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitglobalXY, MyHistoFiller.mdtDigitglobalXY.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitglobalZY, MyHistoFiller.mdtDigitglobalZY.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtDigitglobalZX, MyHistoFiller.mdtDigitglobalZX.GetName())
            # BIS mdtDigit
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitlocalTubePosX, MyHistoFiller.bis_mdtDigitlocalTubePosX.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitlocalTubePosY, MyHistoFiller.bis_mdtDigitlocalTubePosY.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitlocalTubePosZ, MyHistoFiller.bis_mdtDigitlocalTubePosZ.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitStationEta, MyHistoFiller.bis_mdtDigitStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitStationPhi, MyHistoFiller.bis_mdtDigitStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitglobalX, MyHistoFiller.bis_mdtDigitglobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitglobalY, MyHistoFiller.bis_mdtDigitglobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitglobalZ, MyHistoFiller.bis_mdtDigitglobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitNumberOfMultilayers, MyHistoFiller.bis_mdtDigitNumberOfMultilayers.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitTube, MyHistoFiller.bis_mdtDigitTube.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitTubeLayer, MyHistoFiller.bis_mdtDigitTubeLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitMultilayer, MyHistoFiller.bis_mdtDigitMultilayer.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitTime, MyHistoFiller.bis_mdtDigitTime.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitCharge, MyHistoFiller.bis_mdtDigitCharge.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitChannel, MyHistoFiller.bis_mdtDigitChannel.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitglobalXY, MyHistoFiller.bis_mdtDigitglobalXY.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitglobalZY, MyHistoFiller.bis_mdtDigitglobalZY.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitglobalZX, MyHistoFiller.bis_mdtDigitglobalZX.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitlocalTubePosXY, MyHistoFiller.bis_mdtDigitlocalTubePosXY.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitlocalTubePosZY, MyHistoFiller.bis_mdtDigitlocalTubePosZY.GetName())
            outdir.WriteTObject(MyHistoFiller.bis_mdtDigitlocalTubePosZX, MyHistoFiller.bis_mdtDigitlocalTubePosZX.GetName())


        if self.__chamber_name == "CSC_Sim":
            outdir.WriteTObject(MyHistoFiller.cscGlobalX, MyHistoFiller.cscGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.cscGlobalY, MyHistoFiller.cscGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.cscGlobalZ, MyHistoFiller.cscGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.cscWireLayer, MyHistoFiller.cscWireLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.cscStrip, MyHistoFiller.cscStrip.GetName())
            outdir.WriteTObject(MyHistoFiller.cscglobalTime, MyHistoFiller.cscglobalTime.GetName())
            outdir.WriteTObject(MyHistoFiller.cscKineticEnergy, MyHistoFiller.cscKineticEnergy.GetName())
            outdir.WriteTObject(MyHistoFiller.cscDepositEnergy, MyHistoFiller.cscDepositEnergy.GetName())
            outdir.WriteTObject(MyHistoFiller.cscSimStationEta, MyHistoFiller.cscSimStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.cscSimStationPhi, MyHistoFiller.cscSimStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.cscSimChamberLayer, MyHistoFiller.cscSimChamberLayer.GetName())

            
        if self.__chamber_name == "CSC_Digit":
            outdir.WriteTObject(MyHistoFiller.CSCDigitStationEta, MyHistoFiller.CSCDigitStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitStationPhi, MyHistoFiller.CSCDigitStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitWireLayer, MyHistoFiller.CSCDigitWireLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitlocalPosX, MyHistoFiller.CSCDigitlocalPosX.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitglobalPosX, MyHistoFiller.CSCDigitglobalPosX.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitglobalPosY, MyHistoFiller.CSCDigitglobalPosY.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitglobalPosZ, MyHistoFiller.CSCDigitglobalPosZ.GetName())
            
        if self.__chamber_name == "CSC_SDO":
            outdir.WriteTObject(MyHistoFiller.CSCSDOStationEta, MyHistoFiller.CSCSDOStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCSDOStationPhi, MyHistoFiller.CSCSDOStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCSDOChamberLayer, MyHistoFiller.CSCSDOChamberLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCSDOWireLayer, MyHistoFiller.CSCSDOWireLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCSDOStrip, MyHistoFiller.CSCSDOStrip.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCSDOBarcode, MyHistoFiller.CSCSDOBarcode.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCSDOGlobalX, MyHistoFiller.CSCSDOGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCSDOGlobalY, MyHistoFiller.CSCSDOGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCSDOGlobalZ, MyHistoFiller.CSCSDOGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCSDOLocalX, MyHistoFiller.CSCSDOLocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCSDOLocalY, MyHistoFiller.CSCSDOLocalY.GetName())
            
        if self.__chamber_name == "CSC_RDO":
            outdir.WriteTObject(MyHistoFiller.CSCRDOStationEta, MyHistoFiller.CSCRDOStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOStationPhi, MyHistoFiller.CSCRDOStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOChamberLayer, MyHistoFiller.CSCRDOChamberLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOWireLayer, MyHistoFiller.CSCRDOWireLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOStrip, MyHistoFiller.CSCRDOStrip.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOmeasurePhi, MyHistoFiller.CSCRDOmeasurePhi.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOTime, MyHistoFiller.CSCRDOTime.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOGlobalX, MyHistoFiller.CSCRDOGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOGlobalY, MyHistoFiller.CSCRDOGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOGlobalZ, MyHistoFiller.CSCRDOGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOLocalX, MyHistoFiller.CSCRDOLocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCRDOLocalY, MyHistoFiller.CSCRDOLocalY.GetName())

        if self.__chamber_name == "CSC_PRD":
            outdir.WriteTObject(MyHistoFiller.CSCPRDStationEta, MyHistoFiller.CSCPRDStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDStationPhi, MyHistoFiller.CSCPRDStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDChamberLayer, MyHistoFiller.CSCPRDChamberLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDWireLayer, MyHistoFiller.CSCPRDWireLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDStrip, MyHistoFiller.CSCPRDStrip.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDmeasurePhi, MyHistoFiller.CSCPRDmeasurePhi.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDTime, MyHistoFiller.CSCPRDTime.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDCharge, MyHistoFiller.CSCPRDCharge.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDGlobalX, MyHistoFiller.CSCPRDGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDGlobalY, MyHistoFiller.CSCPRDGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDGlobalZ, MyHistoFiller.CSCPRDGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCPRDLocalX, MyHistoFiller.CSCPRDLocalX.GetName())


        if self.__chamber_name == "TGC_Sim":
            outdir.WriteTObject(MyHistoFiller.tgcLocalX, MyHistoFiller.tgcLocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcLocalY, MyHistoFiller.tgcLocalY.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcLocalZ, MyHistoFiller.tgcLocalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcStationEta, MyHistoFiller.tgcStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcStationPhi, MyHistoFiller.tgcStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcGlobalX, MyHistoFiller.tgcGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcGlobalY, MyHistoFiller.tgcGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcGlobalZ, MyHistoFiller.tgcGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcGasGap, MyHistoFiller.tgcGasGap.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcChannel, MyHistoFiller.tgcChannel.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcGlobalTime, MyHistoFiller.tgcGlobalTime.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcKineticEnergy, MyHistoFiller.tgcKineticEnergy.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcDepositEnergy, MyHistoFiller.tgcDepositEnergy.GetName())
            
        if self.__chamber_name == "TGC_Digit":
            outdir.WriteTObject(MyHistoFiller.TGCDigitStationEta, MyHistoFiller.TGCDigitStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCDigitStationPhi, MyHistoFiller.TGCDigitStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCDigitGasGap, MyHistoFiller.TGCDigitGasGap.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCDigitChannel, MyHistoFiller.TGCDigitChannel.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCDigitlocalX, MyHistoFiller.TGCDigitlocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCDigitlocalY, MyHistoFiller.TGCDigitlocalY.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCDigitglobalX, MyHistoFiller.TGCDigitglobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCDigitglobalY, MyHistoFiller.TGCDigitglobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCDigitglobalZ, MyHistoFiller.TGCDigitglobalZ.GetName())
            
        if self.__chamber_name == "TGC_SDO":
            outdir.WriteTObject(MyHistoFiller.TGCSDOStationEta, MyHistoFiller.TGCSDOStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOStationPhi, MyHistoFiller.TGCSDOStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOGasgap, MyHistoFiller.TGCSDOGasgap.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOChannel, MyHistoFiller.TGCSDOChannel.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOWord, MyHistoFiller.TGCSDOWord.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOBarcode, MyHistoFiller.TGCSDOBarcode.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOGlobalX, MyHistoFiller.TGCSDOGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOGlobalY, MyHistoFiller.TGCSDOGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOGlobalZ, MyHistoFiller.TGCSDOGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOLocalX, MyHistoFiller.TGCSDOLocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOLocalY, MyHistoFiller.TGCSDOLocalY.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCSDOGlotime, MyHistoFiller.TGCSDOGlotime.GetName())
            
        if self.__chamber_name == "TGC_RDO":
            outdir.WriteTObject(MyHistoFiller.TGCRDOStationEta, MyHistoFiller.TGCRDOStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCRDOStationPhi, MyHistoFiller.TGCRDOStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCRDOGasgap, MyHistoFiller.TGCRDOGasgap.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCRDOChannel, MyHistoFiller.TGCRDOChannel.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCRDOGlobalX, MyHistoFiller.TGCRDOGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCRDOGlobalY, MyHistoFiller.TGCRDOGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCRDOGlobalZ, MyHistoFiller.TGCRDOGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCRDOLocalX, MyHistoFiller.TGCRDOLocalX.GetName())

        if self.__chamber_name == "TGC_PRD":
            outdir.WriteTObject(MyHistoFiller.TGCPRDStationEta, MyHistoFiller.TGCPRDStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCPRDStationPhi, MyHistoFiller.TGCPRDStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCPRDGasgap, MyHistoFiller.TGCPRDGasgap.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCPRDChannel, MyHistoFiller.TGCPRDChannel.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCPRDisStrip, MyHistoFiller.TGCPRDisStrip.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCPRDGlobalX, MyHistoFiller.TGCPRDGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCPRDGlobalY, MyHistoFiller.TGCPRDGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCPRDGlobalZ, MyHistoFiller.TGCPRDGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.TGCPRDLocalX, MyHistoFiller.TGCPRDLocalX.GetName())
       

        if self.__chamber_name == "MM_Sim":
            outdir.WriteTObject(MyHistoFiller.mmGlobalX, MyHistoFiller.mmGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.mmGlobalY, MyHistoFiller.mmGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.mmGlobalZ, MyHistoFiller.mmGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.mmStationEta, MyHistoFiller.mmStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.mmStationPhi, MyHistoFiller.mmStationPhi.GetName())

        if self.__chamber_name == "sTGC_Sim":
            outdir.WriteTObject(MyHistoFiller.stgcGlobalX, MyHistoFiller.stgcGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.stgcGlobalY, MyHistoFiller.stgcGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.stgcGlobalZ, MyHistoFiller.stgcGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.stgcStationEta, MyHistoFiller.stgcStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.stgcStationPhi, MyHistoFiller.stgcStationPhi.GetName())
