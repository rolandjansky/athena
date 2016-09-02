# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import ROOT
from Defs import SampleTypes
# utility class for DS meta info
class DSconfig:
    def __init__(self,lumi=20300.,XS=1.,nEvents=-1,colour=None,label="",filepath="", sampletype = SampleTypes.Data, GRLs=None):
        self.Lumi = lumi
        self.Colour=colour
        self.Label = label
        self.Filepath = filepath
        self.XS = XS
        self.nEvents = nEvents
        self.SampleType = sampletype
        self.GRLs = GRLs

GRLs = ["/afs/cern.ch/user/a/atlasdqm/grlgen/All_Good/data15_13TeV.periodAllYear_HEAD_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml"]

#Data
Data_15_firstWeek = DSconfig(lumi=6.355,XS=1.,nEvents=-1,colour=ROOT.kBlack,label="Data 2015",filepath="/ptmp/mpp/goblirsc/MCP2014/OfficialProd/v001/Data/Data.root",sampletype=SampleTypes.Data)

Data_271595_CAF = DSconfig(lumi=7.13,XS=1.,nEvents=-1,colour=ROOT.kBlack,label="Data 2015",filepath="/ptmp/mpp/niko/MCP/CAF/150721/271595.root",sampletype=SampleTypes.Data)
Data_271595_Grid = DSconfig(lumi=7.13,XS=1.,nEvents=-1,colour=ROOT.kBlack,label="Data 2015",filepath="/ptmp/mpp/niko/MCP/GridOutput/150721/271595.root",sampletype=SampleTypes.Data)

Data_15_50ns = DSconfig(lumi=71.993,XS=1.,nEvents=-1,colour=ROOT.kBlack,label="Data 2015",filepath="/ptmp/mpp/niko/MCP/CAF/merged_50ns.root",sampletype=SampleTypes.Data,GRLs=GRLs)

Data_267638 = DSconfig(lumi=3.4,XS=1.,nEvents=-1,colour=ROOT.kBlack,label="Data 2015",filepath="/ptmp/mpp/goblirsc/MCP2014/OfficialProd/v001/Data/267638.root",sampletype=SampleTypes.Data)
Data_267639 = DSconfig(lumi=3.0,XS=1.,nEvents=-1,colour=ROOT.kBlack,label="Data 2015",filepath="/ptmp/mpp/goblirsc/MCP2014/OfficialProd/v001/Data/267639.root",sampletype=SampleTypes.Data)
# Signal MC
Zmumu_r6633 = DSconfig(Data_15_firstWeek.Lumi,XS=1901.2*1.026,nEvents=1.0994e6,colour=ROOT.kRed,label="Z#rightarrow#mu#mu MC15",filepath="/ptmp/mpp/goblirsc/MCP2014/OfficialProd/v001/r6633/mc361107.root",sampletype=SampleTypes.Signal)

Zmumu_r6633_Closure = DSconfig(Data_15_firstWeek.Lumi,XS=1901.2*1.026,nEvents=1.0994e6,colour=ROOT.kRed,label="Z#rightarrow#mu#mu MC15",filepath="/ptmp/mpp/goblirsc/MCP2014/GridOut/2015-07-02/mc361107_Closure.root",sampletype=SampleTypes.Signal)

Zmumu_r6633_unsmeared = DSconfig(Data_15_firstWeek.Lumi,XS=1901.2*1.026,nEvents=1.0994e6,colour=ROOT.kBlue,label="Z#rightarrow#mu#mu MC15, no smearing",filepath="/ptmp/mpp/goblirsc/MCP2014/OfficialProd/v001/r6633_noSmearing/mc361107.root",sampletype=SampleTypes.Signal)

Zmumu_r6630 = DSconfig(Data_15_firstWeek.Lumi,XS=1901.2*1.026,nEvents=1.98425e7,colour=ROOT.kRed,label="Z#rightarrow#mu#mu MC15",filepath="/ptmp/mpp/goblirsc/MCP2014/OfficialProd/v001/r6630/mc361107.root",sampletype=SampleTypes.Signal)


#Background MC
Ttbar_r6633 = DSconfig(Data_15_firstWeek.Lumi,XS=696.12*0.543*1.1949,nEvents=1.3577e6,colour=ROOT.kOrange-3,label="t#bar{t} MC15",filepath="/ptmp/mpp/goblirsc/MCP2014/OfficialProd/v001/r6633/mc410000.root",sampletype=SampleTypes.Irreducible)

Ztautau_r6633 = DSconfig(Data_15_firstWeek.Lumi,XS=1901.2*1.026,nEvents=1.007e6,colour=ROOT.kGray,label="Z#rightarrow#tau#tau MC15",filepath="/ptmp/mpp/goblirsc/MCP2014/OfficialProd/v001/r6633/mc361108.root",sampletype=SampleTypes.Irreducible)
Wplus_r6633 = DSconfig(Data_15_firstWeek.Lumi,XS=11306.0*1.0172,nEvents=3.007e6,colour=ROOT.kGreen+1,label="W^{+} #rightarrow#mu#nu MC15",filepath="/ptmp/mpp/goblirsc/MCP2014/OfficialProd/v001/r6633/mc361101.root",sampletype=SampleTypes.Irreducible)

Wminus_r6633 = DSconfig(Data_15_firstWeek.Lumi,XS=8282.6 *1.0358,nEvents=2.007e6,colour=ROOT.kGreen+1,label="W^{-} #rightarrow#mu#nu MC15",filepath="/ptmp/mpp/goblirsc/MCP2014/OfficialProd/v001/r6633/mc361104.root",sampletype=SampleTypes.Irreducible)