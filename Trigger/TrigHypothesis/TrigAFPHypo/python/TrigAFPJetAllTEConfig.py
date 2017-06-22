#from TrigAFPHypo.TrigAFPHypoConf import TrigAFPJetAllTE
import TrigAFPHypoConf
from TrigAFPHypo.TrigAFPJetAllTEMonitoring import (
    TrigAFPJetAllTEValidationMonitoring,
    TrigAFPJetAllTEOnlineMonitoring)

from AthenaCommon.SystemOfUnits import GeV

from  TrigAFPHypo.TrigAFPHypoConf import TrigAFPJetAllTE


class AFPJetAllTE(TrigAFPJetAllTE):
    __slots__ = []
    def __init__(self,
            name = "AFPJetAllTE"):

        super( TrigAFPJetAllTE, self ).__init__( name )

        validation = TrigAFPJetAllTEValidationMonitoring()
        online = TrigAFPJetAllTEOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("AFPJet_Time")

        self.AthenaMonTools = [ time, validation, online ]
# below is the list of parameters that can be set using python script
#        self.protonTransportParamFileName1="2017_beta0p4_xAngle170_beam1.txt"
#        self.protonTransportParamFileName2="2017_beta0p4_xAngle170_beam2.txt"
        # self.alignmentCorrection_nearA=2.361
        # self.alignmentCorrection_nearC=2.172
        self.protonPosShift_y=1.0
