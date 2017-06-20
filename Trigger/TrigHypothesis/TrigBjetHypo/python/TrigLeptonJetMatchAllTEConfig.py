# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigLeptonJetMatchAllTE

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV

thresholdsStartSequence = {
    '0GeV'   : 0,
    '15GeV'  : 15,
    '20GeV'  : 20,
    '25GeV'  : 25,
    '30GeV'  : 30,
    '35GeV'  : 35,
    '40GeV'  : 40,
    '45GeV'  : 45,
    '50GeV'  : 50,
    '55GeV'  : 55,
    '60GeV'  : 60,
    '70GeV'  : 70,
    '85GeV'  : 85,
    '110GeV' : 110,
    '150GeV' : 150,
    '175GeV' : 175,
    '225GeV' : 225,
    '260GeV' : 260,
    '320GeV' : 320,
    '400GeV' : 400,
}


def getLeptonJetMatchAllTEInstance( instance, version, cut ):
    return LeptonJetMatchAllTE( instance=instance, version=version, cut=cut, name="LeptonJetMatchAllTE_"+instance+"_"+version+"_"+cut )



class LeptonJetMatchAllTE (TrigLeptonJetMatchAllTE):
    __slots__ = []
    
    def __init__(self, instance, version, cut, name):
        super( LeptonJetMatchAllTE, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')

        AllowedInstances = ["CloseBy","FarOff"]
        AllowedVersions  = ["R","RZ"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None
        
        if instance=="CloseBy" and version=="R" :
            self.WorkingMode = 1
            self.DeltaRCut   = 0.4
            self.DeltaZCut   = 999
        if instance=="CloseBy" and version=="RZ" :
            self.WorkingMode = 1
            self.DeltaRCut   = 0.4
            self.DeltaZCut   = 2
        elif instance=="FarOff" :
            self.WorkingMode = 2
            self.DeltaRCut   = 0.2
            self.DeltaZCut   = 0

        self.EtThreshold = thresholdsStartSequence[cut]*GeV
        
        from TrigBjetHypo.TrigLeptonJetMatchAllTEMonitoring import TrigEFLeptonJetMatchAllTEValidationMonitoring, TrigEFLeptonJetMatchAllTEOnlineMonitoring
        validation = TrigEFLeptonJetMatchAllTEValidationMonitoring()
        online     = TrigEFLeptonJetMatchAllTEOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigEFLeptonJetMatchAllTE")
        time.TimerHistLimits = [0,0.2]
        
        self.AthenaMonTools = [ time, validation, online ]

