# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import ROOT
# utility class for DS meta info
class DSconfig:
    def __init__(self,lumi=20300.,XS=1.,nEvents=-1,colour=None,label="",filepath=""):
        self.Lumi = lumi
        self.Colour=colour
        self.Label = label
        self.Filepath = filepath
        self.XS = XS
        self.nEvents = nEvents


Data_periodB = DSconfig(lumi=3900.,XS=1.,nEvents=-1,colour=ROOT.kBlack,label="Data",filepath="/ptmp/mpp/niko/MCP/GridOutput/150520/data12_8TeV.physics_Muons.periodB.root")


Data_15_firstWeek = DSconfig(lumi=0.220,XS=1.,nEvents=-1,colour=ROOT.kBlack,label="Data 2015",filepath="/ptmp/mpp/goblirsc/MCP2014/GridOut/2015-06-09-Data15_v1b/Data15.root")


# mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.e3601_r6633_r6264
Zmumu_mc15 = DSconfig(Data_periodB.Lumi,XS=1100.,nEvents=3294358,colour=ROOT.kRed,label="Z#rightarrow#mu#mu MC15",filepath="/afs/ipp-garching.mpg.de/home/g/goblirsc/analysis/TP_Rearrangement/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/share/muontp.root")
