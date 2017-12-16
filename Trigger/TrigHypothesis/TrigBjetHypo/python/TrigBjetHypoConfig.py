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
        
        AllowedCuts      = ["loose","medium","tight","offloose","offmedium","offtight",
                            "mv2c2040","mv2c2050","mv2c2060","mv2c2070","mv2c2077","mv2c2085",
                            "mv2c1040","mv2c1050","mv2c1060","mv2c1070","mv2c1077","mv2c1085" ]
        AllowedVersions  = ["2012", "2015", "2017", "MuJetChain_2012", "MuJetChain_2015"]
        AllowedInstances = ["EF", "MuJetChain"]
        
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
            self.AcceptAll       = False
            self.Instance        = "EF"
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
                    self.CutXCOMB = 0.25
                elif cut=="medium":
                    self.CutXCOMB = 1.25
                elif cut=="tight":
                    self.CutXCOMB = 2.65

            if version=="2015" :
                self.MethodTag = "MV2c20"
                # These are the offline working points
                if cut=="mv2c2040":
                    # Actually XX% efficient
                    self.CutMV2c20 =  0.75
                elif cut=="mv2c2050":
                    # Actually XX% efficient
                    self.CutMV2c20 =  0.5
                elif cut=="mv2c2060":
                    # Actually 62% efficient
                    self.CutMV2c20 = -0.0224729
                elif cut=="mv2c2070":
                    # Actually 72% efficient
                    self.CutMV2c20 = -0.509032
                elif cut=="mv2c2077":
                    # Actually 79% efficient
                    self.CutMV2c20 = -0.764668
                elif cut=="mv2c2085":
                    # Actually 87% efficient
                    self.CutMV2c20 = -0.938441

            if version=="2017" :
                self.MethodTag = "MV2c10"
                # These are the offline working points
                if cut=="mv2c1040":
                    # Actually ~45% efficient
                    self.CutMV2c10 =  0.978
                elif cut=="mv2c1050":
                    # Actually ~55% efficient
                    self.CutMV2c10 =  0.948
                elif cut=="mv2c1060":
                    # Actually ~65% efficient
                    self.CutMV2c10 = 0.846
                elif cut=="mv2c1070":
                    # Actually ~75% efficient
                    self.CutMV2c10 = 0.580
                elif cut=="mv2c1077":
                    # Actually ~80% efficient
                    self.CutMV2c10 = 0.162
                elif cut=="mv2c1085":
                    # Actually ~90% efficient
                    self.CutMV2c10 = -0.494
 

class BjetHypoNoCut (TrigBjetHypo):
    __slots__ = []
    
    def __init__(self, instance, name):
        super( BjetHypoNoCut, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
                
        AllowedInstances = ["EF"]

        self.JetKey = ""
        
        if instance in AllowedInstances :
            
            if instance=="EF" :
                self.AcceptAll             = True
                self.Instance              = "EF"
                self.UseBeamSpotFlag       = False
                self.OverrideBeamSpotValid = True
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
        
        AllowedCuts      = ["loose","medium","tight","offloose","offmedium","offtight",
                            "mv2c2040","mv2c2050","mv2c2060","mv2c2070","mv2c2077","mv2c2085",
                            "mv2c1040","mv2c1050","mv2c1060","mv2c1070","mv2c1077","mv2c1085" ]
        AllowedVersions  = ["2012","2015","2017"]
        AllowedInstances = ["EF", "MuJetChain"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None
        
        if cut not in AllowedCuts :
            mlog.error("Cut "+cut+" is not supported!")
            return None

        self.JetKey = "SplitJet"
        if instance=="MuJetChain" :
            self.JetKey = "FarawayJet"
            instance = "EF"

        if instance=="EF" :
            self.AcceptAll       = False
            self.Instance        = "EF"
            self.UseBeamSpotFlag = False
        
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
                    self.CutXCOMB = 0.25
                elif cut=="medium":
                    self.CutXCOMB = 1.25
                elif cut=="tight":
                    self.CutXCOMB = 2.65

            if version=="2015" :
                self.MethodTag = "MV2c20"
                # These are the offline working points
                if cut=="mv2c2040":
                    # Actually XX% efficient
                    self.CutMV2c20 =  0.75
                elif cut=="mv2c2050":
                    # Actually XX% efficient
                    self.CutMV2c20 =  0.5
                elif cut=="mv2c2060":
                    # Actually 62% efficient
                    self.CutMV2c20 = -0.0224729
                elif cut=="mv2c2070":
                    # Actually 72% efficient
                    self.CutMV2c20 = -0.509032
                elif cut=="mv2c2077":
                    # Actually 79% efficient
                    self.CutMV2c20 = -0.764668
                elif cut=="mv2c2085":
                    # Actually 87% efficient
                    self.CutMV2c20 = -0.938441

            if version=="2017" :
                self.MethodTag = "MV2c10"
                # These are the offline working points
                if cut=="mv2c1040":
                    # Actually ~45% efficient
                    self.CutMV2c10 =  0.978
                elif cut=="mv2c1050":
                    # Actually ~55% efficient
                    self.CutMV2c10 =  0.948
                elif cut=="mv2c1060":
                    # Actually ~65% efficient
                    self.CutMV2c10 =  0.847
                elif cut=="mv2c1070":
                    # Actually ~75% efficient
                    self.CutMV2c10 =  0.580
                elif cut=="mv2c1077":
                    # Actually ~80% efficient
                    self.CutMV2c10 =  0.162
                elif cut=="mv2c1085":
                    # Actually ~90% efficient
                    self.CutMV2c10 = -0.494

                    


class BjetHypoSplitNoCut (TrigBjetHypo):
    __slots__ = []
    
    def __init__(self, instance, name):
        super( BjetHypoSplitNoCut, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
                
        AllowedInstances = ["EF", "MuJetChain"]

        self.JetKey = "SplitJet"
        
        if instance in AllowedInstances :
            
            if instance=="MuJetChain" :
                self.JetKey = "FarawayJet"
                instance = "EF"
                
            if instance=="EF" :
                self.AcceptAll             = True
                self.Instance              = "EF"
                self.UseBeamSpotFlag       = False
                self.OverrideBeamSpotValid = True
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


        

