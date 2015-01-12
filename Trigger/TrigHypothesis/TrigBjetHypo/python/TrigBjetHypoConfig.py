# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBjetHypo

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV

def getBjetHypoInstance( instance, version, cut ):
    return BjetHypo( instance=instance, cut=cut, version=version, name=instance+"BjetHypo"+"_"+cut+"_"+version )

def getBjetHypoNoCutInstance( instance):
    return BjetHypoNoCut( instance=instance, name=instance+"BjetHypoNoCut" )

def getBjetHypoSplitInstance( instance, version, cut ):
    return BjetHypoSplit( instance=instance, cut=cut, version=version, name=instance+"BjetHypoSplit"+"_"+cut+"_"+version )

def getBjetHypoSplitNoCutInstance( instance):
    return BjetHypoSplitNoCut( instance=instance, name=instance+"BjetHypoSplitNoCut" )


class BjetHypo (TrigBjetHypo):
    __slots__ = []
    
    def __init__(self, instance, version, cut, name):
        super( BjetHypo, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
        
        AllowedCuts      = ["loose","medium","tight"]
        AllowedVersions  = ["2012"]
        AllowedInstances = ["EF"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None
        
        if cut not in AllowedCuts :
            mlog.error("Cut "+cut+" is not supported!")
            return None

        if instance=="EF" :
            self.AcceptAll = False
            self.Instance  = "EF"
            self.UseBeamSpotFlag = False

        self.JetKey = ""
        
        if instance=="EF" :
            from TrigBjetHypo.TrigBjetHypoMonitoring import TrigEFBjetHypoValidationMonitoring, TrigEFBjetHypoOnlineMonitoring
            validation = TrigEFBjetHypoValidationMonitoring()
            online     = TrigEFBjetHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,0.4]
        
        self.AthenaMonTools = [ time, validation, online ]
            
        if instance=="EF" :
            if version=="2012" :
                self.MethodTag = "COMB"
                if cut=="loose":
                    self.CutXCOMB = 1.099
                elif cut=="medium":
                    self.CutXCOMB = 3.156
                elif cut=="tight":
                    self.CutXCOMB = 4.528



class BjetHypoNoCut (TrigBjetHypo):
    __slots__ = []
    
    def __init__(self, instance, name):
        super( BjetHypoNoCut, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
                
        AllowedInstances = ["EF"]

        self.JetKey = ""
        
        if instance in AllowedInstances :
            
            if instance=="EF" :
                self.AcceptAll = True
                self.Instance  = "EF"
                self.UseBeamSpotFlag = False
                from TrigBjetHypo.TrigBjetHypoMonitoring import TrigEFBjetHypoValidationMonitoring, TrigEFBjetHypoOnlineMonitoring
                validation = TrigEFBjetHypoValidationMonitoring()
                online     = TrigEFBjetHypoOnlineMonitoring()
        
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
            time.TimerHistLimits = [0,0.4]

            self.AthenaMonTools = [ time, validation, online ]

        else :
            mlog.error("Instance "+instance+" is not supported!")
            return None


### Split instances


class BjetHypoSplit (TrigBjetHypo):
    __slots__ = []
    
    def __init__(self, instance, version, cut, name):
        super( BjetHypoSplit, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
        
        AllowedCuts      = ["loose","medium","tight"]
        AllowedVersions  = ["2012"]
        AllowedInstances = ["EF"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None
        
        if cut not in AllowedCuts :
            mlog.error("Cut "+cut+" is not supported!")
            return None

        if instance=="EF" :
            self.AcceptAll = False
            self.Instance  = "EF"
            self.UseBeamSpotFlag = False

        self.JetKey = "SplitJet"
        
        if instance=="EF" :
            from TrigBjetHypo.TrigBjetHypoMonitoring import TrigEFBjetHypoValidationMonitoring, TrigEFBjetHypoOnlineMonitoring
            validation = TrigEFBjetHypoValidationMonitoring()
            online     = TrigEFBjetHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,0.4]
        
        self.AthenaMonTools = [ time, validation, online ]
            
        if instance=="EF" :
            if version=="2012" :
                self.MethodTag = "COMB"
                if cut=="loose":
                    self.CutXCOMB = 1.099
                elif cut=="medium":
                    self.CutXCOMB = 3.156
                elif cut=="tight":
                    self.CutXCOMB = 4.528



class BjetHypoSplitNoCut (TrigBjetHypo):
    __slots__ = []
    
    def __init__(self, instance, name):
        super( BjetHypoSplitNoCut, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
                
        AllowedInstances = ["EF"]

        self.JetKey = "SplitJet"
        
        if instance in AllowedInstances :
            
            if instance=="EF" :
                self.AcceptAll = True
                self.Instance  = "EF"
                self.UseBeamSpotFlag = False
                from TrigBjetHypo.TrigBjetHypoMonitoring import TrigEFBjetHypoValidationMonitoring, TrigEFBjetHypoOnlineMonitoring
                validation = TrigEFBjetHypoValidationMonitoring()
                online     = TrigEFBjetHypoOnlineMonitoring()
        
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
            time.TimerHistLimits = [0,0.4]

            self.AthenaMonTools = [ time, validation, online ]

        else :
            mlog.error("Instance "+instance+" is not supported!")
            return None


        

