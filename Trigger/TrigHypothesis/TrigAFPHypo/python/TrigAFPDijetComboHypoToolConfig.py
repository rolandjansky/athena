# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TrigAFPHypo.TrigAFPHypoConf import TrigAFPDijetComboHypoTool
from TrigAFPHypo.TrigAFPHypoConf import AFPProtonTransportTool
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigAFPDijetComboHypoToolConfig( TrigAFPDijetComboHypoTool ):
    __slots__ = []
    def __init__(self, name = "TrigAFPDijetComboHypoToolConfig"):

        super( TrigAFPDijetComboHypoToolConfig, self ).__init__( name )
        
        monTool = GenericMonitoringTool("MonTool_"+name)

        afpProtonTransportToolSideA = AFPProtonTransportTool("AFPProtonTransportToolSideA_"+name)
        afpProtonTransportToolSideC = AFPProtonTransportTool("AFPProtonTransportToolSideC_"+name)

        self.protonTransportParamFileNameA="2017_beta0p4_xAngle170_beam1.txt"
        self.protonTransportParamFileNameC="2017_beta0p4_xAngle170_beam2.txt"
        self.alignmentCorrection_nearA=2.361
        self.alignmentCorrection_nearC=2.172
        
        self.MonTool = monTool
        self.TransportBeamA = afpProtonTransportToolSideA
        self.TransportBeamC = afpProtonTransportToolSideC        
