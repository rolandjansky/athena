# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigLeptonJetMatchAllTE

from AthenaCommon.Logging import logging


def getLeptonJetMatchAllTEInstance( instance, version ):
    return LeptonJetMatchAllTE( instance=instance, version=version, name="LeptonJetMatchAllTE_"+instance+"_"+version )



class LeptonJetMatchAllTE (TrigLeptonJetMatchAllTE):
    __slots__ = []
    
    def __init__(self, instance, version, name):
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
        
        from TrigBjetHypo.TrigLeptonJetMatchAllTEMonitoring import TrigEFLeptonJetMatchAllTEValidationMonitoring, TrigEFLeptonJetMatchAllTEOnlineMonitoring
        validation = TrigEFLeptonJetMatchAllTEValidationMonitoring()
        online     = TrigEFLeptonJetMatchAllTEOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigEFLeptonJetMatchAllTE")
        time.TimerHistLimits = [0,0.2]
        
        self.AthenaMonTools = [ time, validation, online ]

