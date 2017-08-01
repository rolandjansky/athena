# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBjetHypo
from TrigBjetHypo.TrigBjetHypoConf import TrigBjetHypoAllTE

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV

def getBjetHypoInstance( instance, version, cut ):
    return BjetHypo( instance=instance, cut=cut, version=version, name=instance+"BjetHypo"+"_"+cut+"_"+version )

def getBjetHypoAllTEInstance( instance, version, name, btagReqs ):
    return BjetHypoAllTE( instance=instance, version=version, btagReqs=btagReqs, name=instance+"BjetHypoAllTE"+"_"+name.replace(".","_"))

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


        

#
#  All TE
#
class BjetHypoAllTE (TrigBjetHypoAllTE):
    __slots__ = []
    
    def __init__(self, instance, version, btagReqs, name):
        super( BjetHypoAllTE, self ).__init__( name )
        
        mlog = logging.getLogger('BjetHypoAllTEConfig.py')
        mlog.setLevel(0)
        AllowedVersions  = ["2015","2017"]
        AllowedInstances = ["EF"  ]

        mlog.debug("btagReqs were")
        mlog.debug(btagReqs)
        
        #
        # Format cut values
        #
        for req in btagReqs:
            req[0] = float(req[0])*1000
            req[1] = req[1].replace("bmv","mv").replace("boff","")
            req[2] = int(req[2])

        mlog.debug( "btagReqs are")
        mlog.debug( btagReqs)

        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

        #
        # These have to be from tighest to loosest
        #
        if version == "2017":
            self.Tagger = "MV2c10_discriminant"
            AllowedCuts      = ["mv2c1040","mv2c1050","mv2c1060","mv2c1070","mv2c1077","mv2c1085","perf"]
            CutsRequiringBS  = ["mv2c1040","mv2c1050","mv2c1060","mv2c1070","mv2c1077","mv2c1085"       ]

        for req in btagReqs:
            if req[1] not in AllowedCuts :
                mlog.error("Cut "+req[1]+" is not supported!")
                return None

        self.BTaggingKey = "HLTBjetFex"

        if instance=="EF" :
            #self.Instance        = "EF"
            self.UseBeamSpotFlag = False
        
        #
        # Override beamspot valid if not btagging
        #
        self.OverrideBeamSpotValid = True
        for req in btagReqs:
            if req[1] in CutsRequiringBS:
                self.OverrideBeamSpotValid = False
         
        #
        # Get a list of differnt operation points in this chain
        #
        CutsInThisChain = []
        for req in btagReqs:
            if req[1] not in CutsInThisChain: 
                CutsInThisChain.append(AllowedCuts.index(req[1]))
        CutsInThisChain.sort()

        fullTrigReqAND     = {}
        for jetReq in btagReqs:
            thisPt   = jetReq[0]
            thisBTag = AllowedCuts.index(jetReq[1])
            thisMult = jetReq[2]
            thisMaxBTag = 99

            if (thisPt,thisBTag,thisMaxBTag) not in fullTrigReqAND:
                fullTrigReqAND[(thisPt,thisBTag,thisMaxBTag)] = thisMult
            else:
                fullTrigReqAND[(thisPt,thisBTag,thisMaxBTag)] += thisMult

            for point in CutsInThisChain:
                if point > thisBTag:
                    if (thisPt,point,thisMaxBTag) not in fullTrigReqAND:
                        fullTrigReqAND[(thisPt,point,thisMaxBTag)] = thisMult
                    else:
                        fullTrigReqAND[(thisPt,point,thisMaxBTag)] += thisMult

        #
        # Update the pt thresholds                                                                                                                           
        #
        for jetDef in fullTrigReqAND:
            for otherJetDefs in fullTrigReqAND:
                if jetDef == otherJetDefs: continue
                if jetDef[0] > otherJetDefs[0] and jetDef[1] == otherJetDefs[1] :
                    fullTrigReqAND[otherJetDefs] += fullTrigReqAND[jetDef]
                

        mlog.debug( "full btagReqsAND are")
        mlog.debug( fullTrigReqAND)
        
        EtThresholds   = []
        BTagMin        = []
        BTagMax        = []
        Multiplicities = []
        for reqAND in fullTrigReqAND:
            EtThresholds  .append(float(reqAND[0]))
            BTagMin       .append(float(self.getCutValue(AllowedCuts[reqAND[1]])))
            BTagMax       .append(float(reqAND[2]))
            Multiplicities.append(int  (fullTrigReqAND[reqAND]))


        self.EtThresholds   = EtThresholds
        self.BTagMin        = BTagMin
        self.BTagMax        = BTagMax
        self.Multiplicities = Multiplicities


        #
        # Handle the cases where have a higher pt looser leg
        #
        fullTrigReqOR      = []
        for jetDef in fullTrigReqAND:
            for otherJetDefs in fullTrigReqAND:
                if jetDef == otherJetDefs: continue
                if jetDef[0] > otherJetDefs[0] and jetDef[1] > otherJetDefs[1]  :
                    fullTrigReqOR.append({otherJetDefs: (fullTrigReqAND[otherJetDefs] + fullTrigReqAND[jetDef])})
                    fullTrigReqOR.append({otherJetDefs : fullTrigReqAND[otherJetDefs],
                                          (jetDef[0],jetDef[1],otherJetDefs[1]) :  fullTrigReqAND[jetDef]})


        mlog.debug( "full btagReqsOR are")
        mlog.debug( fullTrigReqOR)

        EtThresholdsOR   = []
        BTagMinOR        = []
        BTagMaxOR        = []
        MultiplicitiesOR = []


        for jetReqAndComo in fullTrigReqOR:
            thisEtThresholdsOR   = []
            thisBTagMinOR        = []
            thisBTagMaxOR        = []
            thisMultiplicitiesOR = []

            for jetReq in jetReqAndComo:
                thisEtThresholdsOR    .append(jetReq[0])
                thisBTagMinOR         .append(self.getCutValue(AllowedCuts[jetReq[1]]))
                if jetReq[2] in AllowedCuts:
                    thisBTagMaxOR         .append(self.getCutValue(AllowedCuts[jetReq[2]]))
                else:
                    thisBTagMaxOR         .append(jetReq[2])
                thisMultiplicitiesOR  .append(jetReqAndComo[jetReq])

            EtThresholdsOR   .append(thisEtThresholdsOR)
            BTagMinOR        .append(thisBTagMinOR)
            BTagMaxOR        .append(thisBTagMaxOR)
            MultiplicitiesOR .append(thisMultiplicitiesOR)



        self.EtThresholdsOR    = EtThresholdsOR   
        self.BTagMinOR         = BTagMinOR        
        self.BTagMaxOR         = BTagMaxOR        
        self.MultiplicitiesOR  = MultiplicitiesOR 

#        if instance=="EF" :
#            from TrigBjetHypo.TrigBjetHypoMonitoring import TrigEFBjetHypoValidationMonitoring, TrigEFBjetHypoOnlineMonitoring
#            validation = TrigEFBjetHypoValidationMonitoring()
#            online     = TrigEFBjetHypoOnlineMonitoring()
        
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypo")
#        time.TimerHistLimits = [0,0.4]
        
#        self.AthenaMonTools = [ time, validation, online ]


    def getCutValue(self, cut):

        # These are the offline working points
        if cut=="mv2c1040":
            # Actually ~45% efficient
            return  0.978
        elif cut=="mv2c1050":
            # Actually ~55% efficient
            return  0.948
        elif cut=="mv2c1060":
            # Actually ~65% efficient
            return 0.846
        elif cut=="mv2c1070":
            # Actually ~75% efficient
            return 0.580
        elif cut=="mv2c1077":
            # Actually ~80% efficient
            return 0.162
        elif cut=="mv2c1085":
            # Actually ~90% efficient
            return -0.494
        elif cut=="perf":
            return -99

            
