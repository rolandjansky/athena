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
    # CSCs Sim Histograms
    cscStationEta = ROOT.TH1F("cscStationEta","cscStationEta;CSC_stationEta",4,-3,1)
    cscStationPhi = ROOT.TH1F("cscStationPhi","cscStationPhi;CSC_stationPhi",9,0,9)
    cscGlobalX = ROOT.TH1F("cscGlobalX","cscGlobalX;CSC_hitGlobalPositionX",100,-1400,2200)
    cscGlobalY = ROOT.TH1F("cscGlobalY","cscGlobalY;CSC_hitGlobalPositionY",100,-2100,2100)
    cscGlobalZ = ROOT.TH1F("cscGlobalZ","cscGlobalZ;CSC_hitGlobalPositionZ",100,-7900,-7100)
    cscGlobalR = ROOT.TH1F("cscGlobalR","cscGlobalR;CSC_hitGlobalPositionR",100,500,2500)
    cscGlobalP = ROOT.TH1F("cscGlobalP","cscGlobalP;CSC_hitGlobalPositionP",100,-3,3)
    cscWireLayer = ROOT.TH1F("cscWireLayer","cscWireLayer;CSC_Sim_wireLayer",5,0,5)
    # CSC Digit Histograms
    CSCDigitStationEta = ROOT.TH1F("CSCDigitStationEta","CSCDigitStationEta;Digits_CSC_stationEta",4,-3,1)
    CSCDigitStationPhi = ROOT.TH1F("CSCDigitStationPhi","CSCDigitStationPhi;Digits_CSC_stationPhi",9,0,9)
    CSCDigitWireLayer = ROOT.TH1F("CSCDigitWireLayer","CSCDigitWireLayer;Digits_CSC_wireLayer",5,0,5)
    CSCDigitGasGap = ROOT.TH1F("CSCDigitGasGap","CSCDigitGasGap;Digits_CSC_gas_gap",4,0,4)
    CSCDigitChannel = ROOT.TH1F("CSCDigitChannel","CSCDigitChannel;Digits_CSC_channel",100,0,200)
    CSCDigitTruthBarcode = ROOT.TH1F("CSCDigitTruthBarcode","CSCDigitTruthBarcode;Digits_CSC_truth_barcode",100,0,210000)
    CSCDigitTruthlocalPosX = ROOT.TH1F("CSCDigitTruthlocalPosX","CSCDigitTruthlocalPosX;Digits_CSC_truth_localPosX",100,-110000,10000)
    CSCDigitTruthlocalPosY = ROOT.TH1F("CSCDigitTruthlocalPosY","CSCDigitTruthlocalPosY;Digits_CSC_truth_localPosY",100,-110000,10000)
    CSCDigitTruthglobalPosX = ROOT.TH1F("CSCDigitTruthglobalPosX","CSCDigitTruthglobalPosX;Digits_CSC_truth_globalPosX",100,-170000,170000)
    CSCDigitTruthglobalPosY = ROOT.TH1F("CSCDigitTruthglobalPosY","CSCDigitTruthglobalPosY;Digits_CSC_truth_globalPosY",100,-170000,170000)
    CSCDigitTruthglobalPosZ = ROOT.TH1F("CSCDigitTruthglobalPosZ","CSCDigitTruthglobalPosZ;Digits_CSC_truth_globalPosZ",100,-30000,-6500)
    CSCDigitTruthCharge = ROOT.TH1F("CSCDigitTruthCharge","CSCDigitTruthCharge;Digits_CSC_truth_charge",100,0,2200)
    #############################################################################  
    # TGCs Sim Histograms
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
    TGCDigitglobalZ = ROOT.TH1F("TGCDigitglobalZ","TGCDigitglobalZ;Digits_TGC_globalPosZ",100,18000,18000)
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
                
        if self.__chamber_name == "CSC_Digit":
            if not (self.__eta_sel(TTree) and self.__sector_sel(TTree)):
                return
            else:
                MyHistoFiller.CSCDigitStationEta.Fill(TTree.Digits_CSC_stationEta[n])
                MyHistoFiller.CSCDigitStationPhi.Fill(TTree.Digits_CSC_stationPhi[n])
                MyHistoFiller.CSCDigitWireLayer.Fill(TTree.Digits_CSC_wireLayer[n])
                MyHistoFiller.CSCDigitGasGap.Fill(TTree.Digits_CSC_gas_gap[n])
                MyHistoFiller.CSCDigitChannel.Fill(TTree.Digits_CSC_channel[n])
                MyHistoFiller.CSCDigitTruthBarcode.Fill(TTree.Digits_CSC_truth_barcode[n])
                MyHistoFiller.CSCDigitTruthlocalPosX.Fill(TTree.Digits_CSC_truth_localPosX[n])
                MyHistoFiller.CSCDigitTruthlocalPosY.Fill(TTree.Digits_CSC_truth_localPosY[n])
                MyHistoFiller.CSCDigitTruthglobalPosX.Fill(TTree.Digits_CSC_truth_globalPosX[n])
                MyHistoFiller.CSCDigitTruthglobalPosY.Fill(TTree.Digits_CSC_truth_globalPosY[n])
                MyHistoFiller.CSCDigitTruthglobalPosZ.Fill(TTree.Digits_CSC_truth_globalPosZ[n])
                MyHistoFiller.CSCDigitTruthCharge.Fill(TTree.Digits_CSC_truth_charge[n])

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
            
        if self.__chamber_name == "CSC_Digit":
            outdir.WriteTObject(MyHistoFiller.CSCDigitStationEta, MyHistoFiller.CSCDigitStationEta.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitStationPhi, MyHistoFiller.CSCDigitStationPhi.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitWireLayer, MyHistoFiller.CSCDigitWireLayer.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitGasGap, MyHistoFiller.CSCDigitGasGap.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitChannel, MyHistoFiller.CSCDigitChannel.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitTruthBarcode, MyHistoFiller.CSCDigitTruthBarcode.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitTruthlocalPosX, MyHistoFiller.CSCDigitTruthlocalPosX.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitTruthlocalPosY, MyHistoFiller.CSCDigitTruthlocalPosY.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitTruthglobalPosX, MyHistoFiller.CSCDigitTruthglobalPosX.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitTruthglobalPosY, MyHistoFiller.CSCDigitTruthglobalPosY.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitTruthglobalPosZ, MyHistoFiller.CSCDigitTruthglobalPosZ.GetName())
            outdir.WriteTObject(MyHistoFiller.CSCDigitTruthCharge, MyHistoFiller.CSCDigitTruthCharge.GetName())

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
