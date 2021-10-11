# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration



class TestTrigAFPDijetHypoAlgConfig( TestTrigAFPDijetHypoAlg ):
    __slots__ = []
    def __init__(self, name = "TestTrigAFPDijetHypoAlgConfig"):

        super( TestTrigAFPDijetHypoAlgConfig, self ).__init__( name )
        
        monTool = GenericMonitoringTool("MonTool_"+name)

        monTool.Histograms = [defineHistogram('DijetMass', type='TH1F', path='EXPERT', title="Dijet mass", xbins=100, xmin=0, xmax=200000)]
        monTool.HistPath = 'AFPComboHypo/'+tool.getName()

        afpProtonTransportToolSideA = AFPProtonTransportTool("AFPProtonTransportToolSideA_"+name)
        afpProtonTransportToolSideC = AFPProtonTransportTool("AFPProtonTransportToolSideC_"+name)

        self.protonTransportParamFileNameA="2017_beta0p4_xAngle170_beam1.txt"
        self.protonTransportParamFileNameC="2017_beta0p4_xAngle170_beam2.txt"
        self.alignmentCorrection_nearA=2.361
        self.alignmentCorrection_nearC=2.172

        self.MonTool = monTool
        self.TransportBeamA = afpProtonTransportToolSideA
        self.TransportBeamC = afpProtonTransportToolSideC
