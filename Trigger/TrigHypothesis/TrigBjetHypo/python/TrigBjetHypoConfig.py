# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBjetHypo

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV



def getBjetHypoInstance( instance, version, cut ):
    return BjetHypo( instance=instance, cut=cut, version=version, name=instance+"BjetHypo"+"_"+cut+"_"+version )

def getBjetHypoNoCutInstance( instance ):
    return BjetHypoNoCut( instance=instance, name=instance+"BjetHypoNoCut" )

# Danilo:
#
# Important note on working points:
# These working points on the pre-L2Star (SiTrack) calibration
# had the following efficiency and rejection:
#
# Tight:
# Efficiency for b-jets with cut at L2 > 3.5624 and cut at EF > 4.5276: 0.385174
# Rejection for light-jets with cut at L2 > 3.5624 and cut at EF > 4.5276: 317.605
#
# Medium:
# Efficiency for b-jets with cut at L2 > 3.0544 and cut at EF > 3.156: 0.474714
# Rejection for light-jets with cut at L2 > 3.0544 and cut at EF > 3.156: 119.14
#
# Loose:
# Efficiency for b-jets with cut at L2 > 1.2764 and cut at EF > 1.0986: 0.624833
# Rejection for light-jets with cut at L2 > 1.2764 and cut at EF > 1.0986: 25.8784
#
#
# For the L2Star calibration, the same cuts give:
#
# Tight:
# Efficiency for b-jets with cut at L2 > 3.5624 and cut at EF > 4.5276: 0.382656
# Rejection for light-jets with cut at L2 > 3.5624 and cut at EF > 4.5276: 350.707
#
# Medium:
# Efficiency for b-jets with cut at L2 > 3.0544 and cut at EF > 3.156: 0.469948
# Rejection for light-jets with cut at L2 > 3.0544 and cut at EF > 3.156: 132.872
#
# Loose:
# Efficiency for b-jets with cut at L2 > 1.2764 and cut at EF > 1.0986: 0.593711
# Rejection for light-jets with cut at L2 > 1.2764 and cut at EF > 1.0986: 38.1087
#
# For only L2:
# Old calibration (derived on SiTrack and applied on L2Star):
# Tight:
# Efficiency for b-jets with cut at L2 > 3.5624 and cut at EF > -50:0.573763
# Rejection for light-jets with cut at L2 > 3.5624 and cut at EF > -50:31.907
#
# Medium:
# Efficiency for b-jets with cut at L2 > 3.0544 and cut at EF > -50:0.599827
# Rejection for light-jets with cut at L2 > 3.0544 and cut at EF > -50:25.2026
#
# Loose:
# Efficiency for b-jets with cut at L2 > 1.2764 and cut at EF > -50:0.683607
# Rejection for light-jets with cut at L2 > 1.2764 and cut at EF > -50:11.3679
#
# New calibration (derived on LKStar and applied on L2Star):
# Tight:
# Efficiency for b-jets with cut at L2 > 3.5624 and cut at EF > -50:0.572812
# Rejection for light-jets with cut at L2 > 3.5624 and cut at EF > -50:37.1867
#
# Medium:
# Efficiency for b-jets with cut at L2 > 3.0544 and cut at EF > -50:0.590469
# Rejection for light-jets with cut at L2 > 3.0544 and cut at EF > -50:31.1019
#
# Loose:
# Efficiency for b-jets with cut at L2 > 1.2764 and cut at EF > -50:0.640437
# Rejection for light-jets with cut at L2 > 1.2764 and cut at EF > -50:18.7276
#
# The working points at L2 are >~ 60% efficiency, and in this region
# there is no big difference in the eff. and rej. curves for the old
# or new calibration: the difference happens mostly at ~< 50 %.

class BjetHypo (TrigBjetHypo):
    __slots__ = []
    
    def __init__(self, instance, version, cut, name):
        super( BjetHypo, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
        
        AllowedCuts      = ["loose","medium","tight","looseEF","mediumEF","tightEF"]
        AllowedVersions  = ["2012","2011-2","2011"]
        AllowedInstances = ["L2","EF"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None
        
        if cut not in AllowedCuts :
            mlog.error("Cut "+cut+" is not supported!")
            return None

        if instance=="L2" and (cut=="looseEF" or cut=="mediumEF" or cut=="tightEF") :
            mlog.error("Combination of instance "+instance+" and cut "+cut+" is not supported!")
            return None
        
        if (version=="2011" or version=="2011-2") and (cut=="looseEF" or cut=="mediumEF" or cut=="tightEF") :
            mlog.error("Combination of version "+version+" and cut "+cut+" is not supported!")
            return None
        
        if instance=="L2" :
            self.AcceptAll = False
            self.Instance  = "L2"
            self.UseBeamSpotFlag = False
        elif instance=="EF" :
            self.AcceptAll = False
            self.Instance  = "EF"
            self.UseBeamSpotFlag = False
        
        if instance=="L2" :
            from TrigBjetHypo.TrigBjetHypoMonitoring import TrigL2BjetHypoValidationMonitoring, TrigL2BjetHypoOnlineMonitoring
            validation = TrigL2BjetHypoValidationMonitoring()
            online     = TrigL2BjetHypoOnlineMonitoring()
        
        elif instance=="EF" :
            from TrigBjetHypo.TrigBjetHypoMonitoring import TrigEFBjetHypoValidationMonitoring, TrigEFBjetHypoOnlineMonitoring
            validation = TrigEFBjetHypoValidationMonitoring()
            online     = TrigEFBjetHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
        time.TimerHistLimits = [0,0.4]
        
        self.AthenaMonTools = [ time, validation, online ]
            
        if instance=="L2" :
            if version=="2012" :
                self.MethodTag = "COMB"
                if cut=="loose":
                    self.CutXCOMB = 1.276
                elif cut=="medium":
                    self.CutXCOMB = 3.054
                elif cut=="tight":
                    self.CutXCOMB = 3.562
            if version=="2011-2" :    
                self.MethodTag = "CHI2"
                if cut=="loose":
                    self.CutXCHI2 = 0.247
                elif cut=="medium":
                    self.CutXCHI2 = 0.927
                elif cut=="tight":
                    self.CutXCHI2 = 0.962                        
            if version=="2011" :
                self.MethodTag = "CHI2"
                if cut=="loose":
                    self.CutXCHI2 = 0.24
                elif cut=="medium":
                    self.CutXCHI2 = 0.93
                elif cut=="tight":
                    self.CutXCHI2 = 0.98
                    
        if instance=="EF" :
            if version=="2012" :
                self.MethodTag = "COMB"
                if cut=="loose":
                    self.CutXCOMB = 1.099
                elif cut=="medium":
                    self.CutXCOMB = 3.156
                elif cut=="tight":
                    self.CutXCOMB = 4.528
                elif cut=="looseEF":
                    self.CutXCOMB = 4.807
                elif cut=="mediumEF":
                    self.CutXCOMB = 3.994
                elif cut=="tightEF":
                    self.CutXCOMB = 2.394            
            if version=="2011-2" :    
                self.MethodTag = "CHI2"
                if cut in AllowedCuts :
                    if cut=="loose":
                        self.CutXCHI2 = 0.827
                    elif cut=="medium":
                        self.CutXCHI2 = 0.964
                    elif cut=="tight":
                        self.CutXCHI2 = 0.996
            if version=="2011" :
                self.MethodTag = "CHI2"
                if cut in AllowedCuts :
                    if cut=="loose":
                        self.CutXCHI2 = 0.69
                    elif cut=="medium":
                        self.CutXCHI2 = 0.93
                    elif cut=="tight":
                        self.CutXCHI2 = 0.99



class BjetHypoNoCut (TrigBjetHypo):
    __slots__ = []
    
    def __init__(self, instance, name):
        super( BjetHypoNoCut, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoConfig.py')
                
        AllowedInstances = ["L2","EF"]
        
        if instance in AllowedInstances :
            
            if instance=="L2" :
                self.AcceptAll = True
                self.Instance  = "L2"
                self.UseBeamSpotFlag = False
            elif instance=="EF" :
                self.AcceptAll = True
                self.Instance  = "EF"
                self.UseBeamSpotFlag = False
        
            if instance=="L2" :
                from TrigBjetHypo.TrigBjetHypoMonitoring import TrigL2BjetHypoValidationMonitoring, TrigL2BjetHypoOnlineMonitoring
                validation = TrigL2BjetHypoValidationMonitoring()
                online     = TrigL2BjetHypoOnlineMonitoring()
        
            elif instance=="EF" :
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




