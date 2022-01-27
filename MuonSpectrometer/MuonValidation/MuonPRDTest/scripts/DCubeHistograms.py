# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this script can be used to create DCube histograms from the output ntuples of NSWPRDValAlg

import os, sys, ROOT


class MyHistoFiller(object):

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
    # CSC Sim Histograms
    cscStationEta = ROOT.TH1F("cscStationEta","cscStationEta;CSC_stationEta",4,-3,1)
    cscStationPhi = ROOT.TH1F("cscStationPhi","cscStationPhi;CSC_stationPhi",9,0,9)
    cscGlobalX = ROOT.TH1F("cscGlobalX","cscGlobalX;CSC_hitGlobalPositionX",100,-1400,2200)
    cscGlobalY = ROOT.TH1F("cscGlobalY","cscGlobalY;CSC_hitGlobalPositionY",100,-2100,2100)
    cscGlobalZ = ROOT.TH1F("cscGlobalZ","cscGlobalZ;CSC_hitGlobalPositionZ",100,-7900,-7100)
    cscGlobalR = ROOT.TH1F("cscGlobalR","cscGlobalR;CSC_hitGlobalPositionR",100,500,2500)
    cscGlobalP = ROOT.TH1F("cscGlobalP","cscGlobalP;CSC_hitGlobalPositionP",100,-3,3)
    cscWireLayer = ROOT.TH1F("cscWireLayer","cscWireLayer;CSC_Sim_wireLayer",5,0,5)
    cscStrip = ROOT.TH1F("cscStrip","cscStrip;CSC_strip",3,0,3)
    cscglobalTime = ROOT.TH1F("cscglobalTime","cscglobalTime;CSC_globalTime",100,0,60)
    cscKineticEnergy = ROOT.TH1F("cscKineticEnergy","cscKineticEnergy;CSC_kineticEnergy",100,0,400000)
    cscDepositEnergy = ROOT.TH1F("cscDepositEnergy","cscDeposityEnergy;CSC_depositEnergy",100,0,0.008)
    cscSimStationEta = ROOT.TH1F("cscSimStationEta","cscSimStationEta;CSC_Sim_stationEta",4,-3,1)
    cscSimStationPhi = ROOT.TH1F("cscSimStationPhi","cscSimStationPhi;CSC_Sim_stationPhi",9,0,9)
    cscSimChamberlayer = ROOT.TH1F("cscSimChamberLayer","cscSimChamberLayer;CSC_Sim_chamberLayer",4,0,4)
    # CSC Digit Histograms
    CSCDigitStationEta = ROOT.TH1F("CSCDigitStationEta","CSCDigitStationEta;Digits_CSC_stationEta",4,-3,1)
    CSCDigitStationPhi = ROOT.TH1F("CSCDigitStationPhi","CSCDigitStationPhi;Digits_CSC_stationPhi",9,0,9)
    CSCDigitWireLayer = ROOT.TH1F("CSCDigitWireLayer","CSCDigitWireLayer;Digits_CSC_wireLayer",5,0,5)
    CSCDigitGasGap = ROOT.TH1F("CSCDigitGasGap","CSCDigitGasGap;Digits_CSC_gas_gap",4,0,4)
    CSCDigitChannel = ROOT.TH1F("CSCDigitChannel","CSCDigitChannel;Digits_CSC_channel",100,0,200)
    CSCDigitlocalPosX = ROOT.TH1F("CSCDigitlocalPosX","CSCDigitlocalPosX;Digits_CSC_localPosX",100,0,100)
    CSCDigitlocalPosY = ROOT.TH1F("CSCDigitlocalPosY","CSCDigitlocalPosY;Digits_CSC_localPosY",100,0,100)
    CSCDigitglobalPosX = ROOT.TH1F("CSCDigitglobalPosX","CSCDigitglobalPosX;Digits_CSC_globalPosX",100,-1600,1600)
    CSCDigitglobalPosY = ROOT.TH1F("CSCDigitglobalPosY","CSCDigitglobalPosY;Digits_CSC_globalPosY",100,-1600,1600)
    CSCDigitglobalPosZ = ROOT.TH1F("CSCDigitglobalPosZ","CSCDigitglobalPosZ;Digits_CSC_globalPosZ",100,-8000,-7000)
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
    CSCRDOLocalX = ROOT.TH1F("CSCRDOLocalX","CSCRDOLocalX;RDO_CSC_localPosX",100,-1000,1000)
    CSCRDOLocalY = ROOT.TH1F("CSCRDOLocalY","CSCRDOLocalY;RDO_CSC_localPosY",100,-1000,1000)
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
    tgcChannel = ROOT.TH1F("tgcChannel","tgcChannel;TGC_channel",3,0,3)
    tgcGlobalTime = ROOT.TH1F("tgcGlobalTime","tgcGlobalTime;TGC_globalTime",100,0,120)
    tgcKineticEnergy = ROOT.TH1F("tgcKineticEnergy","tgcKineticEnergy;TGC_kineticEnergy",100,0,400000)
    tgcDepositEnergy = ROOT.TH1F("tgcDepositEnergy","tgcDepositEnergy;TGC_depositEnergy",100,0,0.0018)
    # TGC Digit Histograms
    TGCDigitStationEta = ROOT.TH1F("TGCDigitStationEta","TGCDigitStationEta;Digits_TGC_stationEta",12,-6,6)
    TGCDigitStationPhi = ROOT.TH1F("TGCDigitStationPhi","TGCDigitStationPhi;Digits_TGC_stationPhi",50,0,50)
    TGCDigitGasGap = ROOT.TH1F("TGCDigitGasGap","TGCDigitGasGap;Digits_TGC_gas_gap",4,0,4)
    TGCDigitChannel = ROOT.TH1F("TGCDigitChannel","TGCDigitChannel;Digits_TGC_channel",120,0,120)
    TGCDigitisStrip = ROOT.TH1F("TGCDigitisStrip","TGCDigitisStrip;Digits_TGC_isStrip",2,0,2)
    TGCDigitlocalX = ROOT.TH1F("TGCDigitlocalX","TGCDigitlocalX;Digits_TGC_localPosX",100,-12000,1000)
    TGCDigitlocalY = ROOT.TH1F("TGCDigitlocalY","TGCDigitlocalY;Digits_TGC_localPosY",100,-12000,1000)
    TGCDigitglobalX = ROOT.TH1F("TGCDigitglobalX","TGCDigitglobalX;Digits_TGC_globalPosX",100,-12000,12000)
    TGCDigitglobalY = ROOT.TH1F("TGCDigitglobalY","TGCDigitglobalY;Digits_TGC_globalPosY",100,-12000,12000)
    TGCDigitglobalZ = ROOT.TH1F("TGCDigitglobalZ","TGCDigitglobalZ;Digits_TGC_globalPosZ",100,-18000,18000)
    # TGC SDO Histograms
    TGCSDOStationEta = ROOT.TH1F("TGCSDOStationEta","TGCSDOStationEta;SDO_TGC_stationEta",12,-6,6)
    TGCSDOStationPhi = ROOT.TH1F("TGCSDOStationPhi","TGCSDOStationPhi;SDO_TGC_stationPhi",50,0,50)
    TGCSDOGasgap = ROOT.TH1F("TGCSDOGasgap","TGCSDOGasgap;SDO_TGC_gas_gap",4,0,4)
    TGCSDOChannel = ROOT.TH1F("TGCSDOChannel","TGCSDOChannel;SDO_TGC_channel",100,0,140)
    TGCSDOisStrip = ROOT.TH1F("TGCSDOisStrip","TGCSDOisStrip;SDO_TGC_isStrip",2,0,2) 
    TGCSDOWord = ROOT.TH1F("TGCSDOWord","TGCSDOWord;SDO_TGC_word",4,-2,2) 
    TGCSDOBarcode = ROOT.TH1F("TGCSDOBarcode","TGCSDOBarcode;SDO_TGC_barcode",100,0,10500) 
    TGCSDOGlobalX = ROOT.TH1F("TGCSDOGlobalX","TGCSDOGlobalX;SDO_TGC_globalPosX",100,-15000,15000)
    TGCSDOGlobalY = ROOT.TH1F("TGCSDOGlobalY","TGCSDOGlobalY;SDO_TGC_globalPosY",100,-15000,15000)
    TGCSDOGlobalZ = ROOT.TH1F("TGCSDOGlobalZ","TGCSDOGlobalZ;SDO_TGC_globalPosZ",100,-20000,20000)
    TGCSDOLocalX = ROOT.TH1F("TGCSDOLocalX","TGCSDOLocalX;SDO_TGC_localPosX",100,0,100)
    TGCSDOLocalY = ROOT.TH1F("TGCSDOLocalY","TGCSDOLocalY;SDO_TGC_localPosY",100,0,100)
    TGCSDOGlotime = ROOT.TH1F("TGCSDOGlotime","TGCSDOGlotime;SDO_TGC_global_time",100,0,100)
    # TGC RDO Histograms
    TGCRDOStationEta = ROOT.TH1F("TGCRDOStationEta","TGCRDOStationEta;RDO_TGC_stationEta",12,-6,6)
    TGCRDOStationPhi = ROOT.TH1F("TGCRDOStationPhi","TGCRDOStationPhi;RDO_TGC_stationPhi",50,0,50)
    TGCRDOGasgap = ROOT.TH1F("TGCRDOGasgap","TGCRDOGasgap;RDO_TGC_gas_gap",3,0,3)
    TGCRDOChannel = ROOT.TH1F("TGCRDOChannel","TGCRDOChannel;RDO_TGC_channel",3,0,3)
    TGCRDOisStrip = ROOT.TH1F("TGCRDOisStrip","TGCRDOisStrip;RDO_TGC_isStrip",4,-2,2) 
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
    mmGlobalX = ROOT.TH1F("mmGlobalX","mmGlobalX;MM_hitGlobalPositionX",100,-5000,3000)
    mmGlobalY = ROOT.TH1F("mmGlobalY","mmGlobalY;MM_hitGlobalPositionY",100,-3000,4000)
    mmGlobalZ = ROOT.TH1F("mmGlobalZ","mmGlobalZ;MM_hitGlobalPositionZ",100,7000,7800)
    mmGlobalR = ROOT.TH1F("mmGlobalR","mmGlobalR;MM_hitGlobalPositionR",100,1200,4600)
    mmGlobalP = ROOT.TH1F("mmGlobalP","mmGlobalP;MM_hitGlobalPositionP",100,-3.4,3.4)
    mmStationEta = ROOT.TH1F("mmStationEta","mmStationEta;MM_off_stationEta",3,0,3)
    mmStationPhi = ROOT.TH1F("mmStationPhi","mmStationPhi;MM_off_stationPhi",9,0,9)
    #############################################################################
    # sTGCs
    stgcGlobalX = ROOT.TH1F("stgcGlobalX","stgcGlobalX;sTGC_hitGlobalPositionX",100,-5000,4200)
    stgcGlobalY = ROOT.TH1F("stgcGlobalY","stgcGlobalY;sTGC_hitGlobalPositionY",100,-3200,3400)
    stgcGlobalZ = ROOT.TH1F("stgcGlobalZ","stgcGlobalZ;sTGC_hitGlobalPositionZ",100,6900,7900)
    stgcGlobalR = ROOT.TH1F("stgcGlobalR","stgcGlobalR;sTGC_hitGlobalPositionR",100,700,4700)
    stgcGlobalP = ROOT.TH1F("stgcGlobalP","stgcGlobalP;sTGC_hitGlobalPositionP",100,-3.4,3.4)
    stgcStationEta = ROOT.TH1F("stgcStationEta","stgcStationEta;sTGC_off_stationEta",4,0,4)
    stgcStationPhi = ROOT.TH1F("stgcStationPhi","stgcStationPhi;sTGC_off_stationPhi",9,0,9)
    #############################################################################



    def __init__(self, chamber_name="", eta_sel = None, sector_sel = None):
        self.__chamber_name = chamber_name
        self.__eta_sel   = eta_sel
        self.__sector_sel   = sector_sel


                     
    def fill(self, TTree, n):
    
        if self.__chamber_name == "TruthInfo":   
            MyHistoFiller.truthPtHist.Fill(TTree.MuEntry_Particle_Pt[n]*0.001)
            MyHistoFiller.truthEtaHist.Fill(TTree.MuEntry_Particle_Eta[n])
            MyHistoFiller.truthPhiHist.Fill(TTree.MuEntry_Particle_Phi[n])

        if self.__chamber_name == "RPC":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return      
            else:
                MyHistoFiller.rpcLocalX.Fill(TTree.RPC_hitLocalPositionX[n])
                MyHistoFiller.rpcLocalY.Fill(TTree.RPC_hitLocalPositionY[n])
                MyHistoFiller.rpcLocalZ.Fill(TTree.RPC_hitLocalPositionZ[n])
                MyHistoFiller.rpcStationEta.Fill(TTree.RPC_stationEta[n])
                MyHistoFiller.rpcStationPhi.Fill(TTree.RPC_stationPhi[n])
                MyHistoFiller.rpcGlobalX.Fill(TTree.RPC_hitGlobalPositionX[n])
                MyHistoFiller.rpcGlobalY.Fill(TTree.RPC_hitGlobalPositionY[n])
                MyHistoFiller.rpcGlobalZ.Fill(TTree.RPC_hitGlobalPositionZ[n])
                MyHistoFiller.rpcGlobalR.Fill(TTree.RPC_hitGlobalPositionR[n])
                MyHistoFiller.rpcGlobalP.Fill(TTree.RPC_hitGlobalPositionP[n])
                MyHistoFiller.rpcGasGap.Fill(TTree.RPC_Sim_gasGapLayer[n])

        if self.__chamber_name == "MDT":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.mdtLocalX.Fill(TTree.MDT_hitLocalPositionX[n])
                MyHistoFiller.mdtLocalY.Fill(TTree.MDT_hitLocalPositionY[n])
                MyHistoFiller.mdtLocalZ.Fill(TTree.MDT_hitLocalPositionZ[n])
                MyHistoFiller.mdtStationEta.Fill(TTree.MDT_stationEta[n])
                MyHistoFiller.mdtStationPhi.Fill(TTree.MDT_stationPhi[n])
                MyHistoFiller.mdtGlobalX.Fill(TTree.MDT_hitGlobalPositionX[n])
                MyHistoFiller.mdtGlobalY.Fill(TTree.MDT_hitGlobalPositionY[n])
                MyHistoFiller.mdtGlobalZ.Fill(TTree.MDT_hitGlobalPositionZ[n])
                MyHistoFiller.mdtGlobalR.Fill(TTree.MDT_hitGlobalPositionR[n])
                MyHistoFiller.mdtGlobalP.Fill(TTree.MDT_hitGlobalPositionP[n])
                MyHistoFiller.mdtTube.Fill(TTree.MDT_Sim_tube[n])

        if self.__chamber_name == "CSC_Sim":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.cscStationEta.Fill(TTree.CSC_stationEta[n])
                MyHistoFiller.cscStationPhi.Fill(TTree.CSC_stationPhi[n])
                MyHistoFiller.cscGlobalX.Fill(TTree.CSC_hitGlobalPositionX[n])
                MyHistoFiller.cscGlobalY.Fill(TTree.CSC_hitGlobalPositionY[n])
                MyHistoFiller.cscGlobalZ.Fill(TTree.CSC_hitGlobalPositionZ[n])
                MyHistoFiller.cscGlobalR.Fill(TTree.CSC_hitGlobalPositionR[n])
                MyHistoFiller.cscGlobalP.Fill(TTree.CSC_hitGlobalPositionP[n])
                MyHistoFiller.cscWireLayer.Fill(TTree.CSC_Sim_wireLayer[n])
                MyHistoFiller.cscStrip.Fill(TTree.CSC_strip[n])
                MyHistoFiller.cscglobalTime.Fill(TTree.CSC_globalTime[n])
                MyHistoFiller.cscKineticEnergy.Fill(TTree.CSC_KineticEnergy[n])
                MyHistoFiller.cscDepositEnergy.Fill(TTree.CSC_DepositEnergy[n])
                MyHistoFiller.cscSimStationEta.Fill(TTree.CSC_Sim_stationEta[n])
                MyHistoFiller.cscSimStationPhi.Fill(TTree.CSC_Sim_stationPhi[n])
                MyHistoFiller.cscSimChamberLayer.Fill(TTree.CSC_Sim_chamberLayer[n])
                
        if self.__chamber_name == "CSC_Digit":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.CSCDigitStationEta.Fill(TTree.Digits_CSC_stationEta[n])
                MyHistoFiller.CSCDigitStationPhi.Fill(TTree.Digits_CSC_stationPhi[n])
                MyHistoFiller.CSCDigitWireLayer.Fill(TTree.Digits_CSC_wireLayer[n])
                MyHistoFiller.CSCDigitGasGap.Fill(TTree.Digits_CSC_gas_gap[n])
                MyHistoFiller.CSCDigitChannel.Fill(TTree.Digits_CSC_channel[n])
                MyHistoFiller.CSCDigitlocalPosX.Fill(TTree.Digits_CSC_localPosX[n])
                MyHistoFiller.CSCDigitlocalPosY.Fill(TTree.Digits_CSC_localPosY[n])
                MyHistoFiller.CSCDigitglobalPosX.Fill(TTree.Digits_CSC_globalPosX[n])
                MyHistoFiller.CSCDigitglobalPosY.Fill(TTree.Digits_CSC_globalPosY[n])
                MyHistoFiller.CSCDigitglobalPosZ.Fill(TTree.Digits_CSC_globalPosZ[n])

        if self.__chamber_name == "CSC_SDO":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.CSCSDOStationEta.Fill(TTree.SDO_CSC_stationEta[n])
                MyHistoFiller.CSCSDOStationPhi.Fill(TTree.SDO_CSC_stationPhi[n])
                MyHistoFiller.CSCSDOChamberLayer.Fill(TTree.SDO_CSC_chamberLayer[n])
                MyHistoFiller.CSCSDOWireLayer.Fill(TTree.SDO_CSC_wireLayer[n])
                MyHistoFiller.CSCSDOStrip.Fill(TTree.SDO_CSC_strip[n])
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
                MyHistoFiller.CSCRDOStationEta.Fill(TTree.RDO_CSC_StationEta[n])
                MyHistoFiller.CSCRDOStationPhi.Fill(TTree.RDO_CSC_StationPhi[n])
                MyHistoFiller.CSCRDOChamberLayer.Fill(TTree.RDO_CSC_chamberLayer[n])
                MyHistoFiller.CSCRDOWireLayer.Fill(TTree.RDO_CSC_wireLayer[n])
                MyHistoFiller.CSCRDOStrip.Fill(TTree.RDO_CSC_strip[n])
                MyHistoFiller.CSCRDOmeasurePhi.Fill(TTree.RDO_CSC_measurePhi[n])
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
                MyHistoFiller.CSCPRDStationEta.Fill(TTree.PRD_CSC_stationEta[n])
                MyHistoFiller.CSCPRDStationPhi.Fill(TTree.PRD_CSC_stationPhi[n])
                MyHistoFiller.CSCPRDChamberLayer.Fill(TTree.PRD_CSC_chlayer[n])
                MyHistoFiller.CSCPRDWireLayer.Fill(TTree.PRD_CSC_wlayer[n])
                MyHistoFiller.CSCPRDStrip.Fill(TTree.PRD_CSC_strip[n])
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
                MyHistoFiller.tgcLocalX.Fill(TTree.TGC_hitLocalPositionX[n])
                MyHistoFiller.tgcLocalY.Fill(TTree.TGC_hitLocalPositionY[n])
                MyHistoFiller.tgcLocalZ.Fill(TTree.TGC_hitLocalPositionZ[n])
                MyHistoFiller.tgcStationEta.Fill(TTree.TGC_stationEta[n])
                MyHistoFiller.tgcStationPhi.Fill(TTree.TGC_stationPhi[n])
                MyHistoFiller.tgcGlobalX.Fill(TTree.TGC_hitGlobalPositionX[n])
                MyHistoFiller.tgcGlobalY.Fill(TTree.TGC_hitGlobalPositionY[n])
                MyHistoFiller.tgcGlobalZ.Fill(TTree.TGC_hitGlobalPositionZ[n])
                MyHistoFiller.tgcGlobalR.Fill(TTree.TGC_hitGlobalPositionR[n])
                MyHistoFiller.tgcGlobalP.Fill(TTree.TGC_hitGlobalPositionP[n])
                MyHistoFiller.tgcGasGap.Fill(TTree.TGC_gasGap[n])
                MyHistoFiller.tgcChannel.Fill(TTree.TGC_channel[n])
                MyHistoFiller.tgcGlobalTime.Fill(TTree.TGC_globalTime[n])
                MyHistoFiller.tgcKineticEnergy.Fill(TTree.TGC_kineticEnergy[n])
                MyHistoFiller.tgcDepositEnergy.Fill(TTree.TGC_depositEnergy[n])

        if self.__chamber_name == "TGC_Digit":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.TGCDigitStationEta.Fill(TTree.Digits_TGC_stationEta[n])
                MyHistoFiller.TGCDigitStationPhi.Fill(TTree.Digits_TGC_stationPhi[n])
                MyHistoFiller.TGCDigitGasGap.Fill(TTree.Digits_TGC_gas_gap[n])
                MyHistoFiller.TGCDigitChannel.Fill(TTree.Digits_TGC_channel[n])
                MyHistoFiller.TGCDigitisStrip.Fill(TTree.Digits_TGC_isStrip[n])
                MyHistoFiller.TGCDigitlocalX.Fill(TTree.Digits_TGC_localPosX[n])
                MyHistoFiller.TGCDigitlocalY.Fill(TTree.Digits_TGC_localPosY[n])
                MyHistoFiller.TGCDigitglobalX.Fill(TTree.Digits_TGC_globalPosX[n])
                MyHistoFiller.TGCDigitglobalY.Fill(TTree.Digits_TGC_globalPosY[n])
                MyHistoFiller.TGCDigitglobalZ.Fill(TTree.Digits_TGC_globalPosZ[n])
                
        if self.__chamber_name == "TGC_SDO":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.TGCSDOStationEta.Fill(TTree.SDO_TGC_stationEta[n])
                MyHistoFiller.TGCSDOStationPhi.Fill(TTree.SDO_TGC_stationPhi[n])
                MyHistoFiller.TGCSDOGasgap.Fill(TTree.SDO_TGC_gas_gap[n])
                MyHistoFiller.TGCSDOChannel.Fill(TTree.SDO_TGC_channel[n])
                MyHistoFiller.TGCSDOisStrip.Fill(TTree.SDO_TGC_isStrip[n])
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
                MyHistoFiller.TGCRDOStationEta.Fill(TTree.RDO_TGC_stationEta[n])
                MyHistoFiller.TGCRDOStationPhi.Fill(TTree.RDO_TGC_stationPhi[n])
                MyHistoFiller.TGCRDOGasgap.Fill(TTree.RDO_TGC_gas_gap[n])
                MyHistoFiller.TGCRDOChannel.Fill(TTree.RDO_TGC_channel[n])
                MyHistoFiller.TGCRDOisStrip.Fill(TTree.RDO_TGC_isStrip[n])
                MyHistoFiller.TGCRDOGlobalX.Fill(TTree.RDO_TGC_globalPosX[n])
                MyHistoFiller.TGCRDOGlobalY.Fill(TTree.RDO_TGC_globalPosY[n])
                MyHistoFiller.TGCRDOGlobalZ.Fill(TTree.RDO_TGC_globalPosZ[n])
                MyHistoFiller.TGCRDOLocalX.Fill(TTree.RDO_TGC_localPosX[n])

        if self.__chamber_name == "TGC_PRD":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.TGCPRDStationEta.Fill(TTree.PRD_TGC_stationEta[n])
                MyHistoFiller.TGCPRDStationPhi.Fill(TTree.PRD_TGC_stationPhi[n])
                MyHistoFiller.TGCPRDGasgap.Fill(TTree.PRD_TGC_gasGap[n])
                MyHistoFiller.TGCPRDChannel.Fill(TTree.PRD_TGC_channel[n])
                MyHistoFiller.TGCPRDisStrip.Fill(TTree.PRD_TGC_isStrip[n])
                MyHistoFiller.TGCPRDGlobalX.Fill(TTree.PRD_TGC_globalPosX[n])
                MyHistoFiller.TGCPRDGlobalY.Fill(TTree.PRD_TGC_globalPosY[n])
                MyHistoFiller.TGCPRDGlobalZ.Fill(TTree.PRD_TGC_globalPosZ[n])
                MyHistoFiller.TGCPRDLocalX.Fill(TTree.PRD_TGC_localPosX[n])

        if self.__chamber_name == "MM":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.mmGlobalX.Fill(TTree.Hits_MM_hitGlobalPositionX[n])
                MyHistoFiller.mmGlobalY.Fill(TTree.Hits_MM_hitGlobalPositionY[n])
                MyHistoFiller.mmGlobalZ.Fill(TTree.Hits_MM_hitGlobalPositionZ[n])
                MyHistoFiller.mmGlobalR.Fill(TTree.Hits_MM_hitGlobalPositionR[n])
                MyHistoFiller.mmGlobalP.Fill(TTree.Hits_MM_hitGlobalPositionP[n])
                MyHistoFiller.mmStationEta.Fill(TTree.Hits_MM_off_stationEta[n])
                MyHistoFiller.mmStationPhi.Fill(TTree.Hits_MM_off_stationPhi[n])

        if self.__chamber_name == "sTGC":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.stgcGlobalX.Fill(TTree.Hits_sTGC_hitGlobalPositionX[n])
                MyHistoFiller.stgcGlobalY.Fill(TTree.Hits_sTGC_hitGlobalPositionY[n])
                MyHistoFiller.stgcGlobalZ.Fill(TTree.Hits_sTGC_hitGlobalPositionZ[n])
                MyHistoFiller.stgcGlobalR.Fill(TTree.Hits_sTGC_hitGlobalPositionR[n])
                MyHistoFiller.stgcGlobalP.Fill(TTree.Hits_sTGC_hitGlobalPositionP[n])
                MyHistoFiller.stgcStationEta.Fill(TTree.Hits_sTGC_off_stationEta[n])
                MyHistoFiller.stgcStationPhi.Fill(TTree.Hits_sTGC_off_stationPhi[n])



    def write(self, outdir):
    
        if self.__chamber_name == "TruthInfo":
            outdir.WriteTObject(MyHistoFiller.truthPtHist, MyHistoFiller.truthPtHist.GetName())
            outdir.WriteTObject(MyHistoFiller.truthEtaHist, MyHistoFiller.truthEtaHist.GetName())
            outdir.WriteTObject(MyHistoFiller.truthPhiHist, MyHistoFiller.truthPhiHist.GetName())
    
        if self.__chamber_name == "RPC":
            outdir.WriteTObject(MyHistoFiller.rpcLocalX, MyHistoFiller.rpcLocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcLocalY, MyHistoFiller.rpcLocalY.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcLocalZ, MyHistoFiller.rpcLocalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcStationEta, MyHistoFiller.rpcStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcStationPhi, MyHistoFiller.rpcStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcGlobalX, MyHistoFiller.rpcGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcGlobalY, MyHistoFiller.rpcGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcGlobalZ, MyHistoFiller.rpcGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcGlobalR, MyHistoFiller.rpcGlobalR.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcGlobalP, MyHistoFiller.rpcGlobalP.GetName())
            outdir.WriteTObject(MyHistoFiller.rpcGasGap, MyHistoFiller.rpcGasGap.GetName())

        if self.__chamber_name == "MDT":
            outdir.WriteTObject(MyHistoFiller.mdtLocalX, MyHistoFiller.mdtLocalX.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtLocalY, MyHistoFiller.mdtLocalY.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtLocalZ, MyHistoFiller.mdtLocalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtStationEta, MyHistoFiller.mdtStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtStationPhi, MyHistoFiller.mdtStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtGlobalX, MyHistoFiller.mdtGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtGlobalY, MyHistoFiller.mdtGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtGlobalZ, MyHistoFiller.mdtGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtGlobalR, MyHistoFiller.mdtGlobalR.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtGlobalP, MyHistoFiller.mdtGlobalP.GetName())
            outdir.WriteTObject(MyHistoFiller.mdtTube, MyHistoFiller.mdtTube.GetName())


        if self.__chamber_name == "CSC_Sim":
            outdir.WriteTObject(MyHistoFiller.cscStationEta, MyHistoFiller.cscStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.cscStationPhi, MyHistoFiller.cscStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.cscGlobalX, MyHistoFiller.cscGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.cscGlobalY, MyHistoFiller.cscGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.cscGlobalZ, MyHistoFiller.cscGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.cscGlobalR, MyHistoFiller.cscGlobalR.GetName())
            outdir.WriteTObject(MyHistoFiller.cscGlobalP, MyHistoFiller.cscGlobalP.GetName())
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
            outdir.WriteTObject(MyHistoFiller.CSCDigitGasGap, MyHistoFiller.CSCDigitGasGap.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitChannel, MyHistoFiller.CSCDigitChannel.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitlocalPosX, MyHistoFiller.CSCDigitlocalPosX.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitlocalPosY, MyHistoFiller.CSCDigitlocalPosY.GetName())
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
            outdir.WriteTObject(MyHistoFiller.tgcGlobalR, MyHistoFiller.tgcGlobalR.GetName())
            outdir.WriteTObject(MyHistoFiller.tgcGlobalP, MyHistoFiller.tgcGlobalP.GetName())
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
            outdir.WriteTObject(MyHistoFiller.TGCDigitisStrip, MyHistoFiller.TGCDigitisStrip.GetName())
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
            outdir.WriteTObject(MyHistoFiller.TGCSDOisStrip, MyHistoFiller.TGCSDOisStrip.GetName())
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
            outdir.WriteTObject(MyHistoFiller.TGCRDOisStrip, MyHistoFiller.TGCRDOisStrip.GetName())
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
       

        if self.__chamber_name == "MM":
            outdir.WriteTObject(MyHistoFiller.mmGlobalX, MyHistoFiller.mmGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.mmGlobalY, MyHistoFiller.mmGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.mmGlobalZ, MyHistoFiller.mmGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.mmGlobalR, MyHistoFiller.mmGlobalR.GetName())
            outdir.WriteTObject(MyHistoFiller.mmGlobalP, MyHistoFiller.mmGlobalP.GetName())
            outdir.WriteTObject(MyHistoFiller.mmStationEta, MyHistoFiller.mmStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.mmStationPhi, MyHistoFiller.mmStationPhi.GetName())

        if self.__chamber_name == "sTGC":
            outdir.WriteTObject(MyHistoFiller.stgcGlobalX, MyHistoFiller.stgcGlobalX.GetName())
            outdir.WriteTObject(MyHistoFiller.stgcGlobalY, MyHistoFiller.stgcGlobalY.GetName())
            outdir.WriteTObject(MyHistoFiller.stgcGlobalZ, MyHistoFiller.stgcGlobalZ.GetName())
            outdir.WriteTObject(MyHistoFiller.stgcGlobalR, MyHistoFiller.stgcGlobalR.GetName())
            outdir.WriteTObject(MyHistoFiller.stgcGlobalP, MyHistoFiller.stgcGlobalP.GetName())
            outdir.WriteTObject(MyHistoFiller.stgcStationEta, MyHistoFiller.stgcStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.stgcStationPhi, MyHistoFiller.stgcStationPhi.GetName())
