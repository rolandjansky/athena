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

        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None


        mlog.debug("btagReqs were")
        mlog.debug(btagReqs)

        #
        #  Main idea here is that the btagReqs contains a list of the different btagging requiremnts
        #    eg: 2b55_2j55  would contain a list of two requirements (55,b,2) and (55,j,2) 
        #   The following python code:
        #     - updates the requirements to properly handle overlap 
        #     - formats these requirements as expected by c++
        #     - checks if the beamspot is needed 
        #   The c++ code then recieves a list of requirements to be "AND"ed
        #     and a list of requrirements to be "OR"ed
        #     to handle chains like 2b55_j100 the "OR" requirments need sub-requirements that are "AND"ed
        
        #
        # Format cut values
        #
        for req in btagReqs:

            # Et in MeV
            req[0] = float(req[0])*1000

            # req[1] has to be one of possiblities in AllowedCuts below
            req[1] = req[1].replace("bmv","mv").replace("boff","")
            
            # Mulitplicity Requirement
            req[2] = int(req[2])

        mlog.debug( "btagReqs are")
        mlog.debug( btagReqs)

        #
        # These have to be ordered from tighest to loosest
        #
        if version == "2017":
            self.Tagger = "MV2c10_discriminant"
            # All possible cuts
            AllowedCuts      = ["mv2c1040","mv2c1050","mv2c1060","mv2c1070","mv2c1077","mv2c1085","perf"]
            # Subset of cuts requireing a BS to be valid
            CutsRequiringBS  = ["mv2c1040","mv2c1050","mv2c1060","mv2c1070","mv2c1077","mv2c1085"       ]

        #
        # Check that the b-tagging operating points are supported
        #
        for req in btagReqs:
            if req[1] not in AllowedCuts :
                mlog.error("Cut "+req[1]+" is not supported!")
                return None

        #
        # Set the b-tag key
        #
        self.BTaggingKey = "HLTBjetFex"

        #
        # Use the BS for EF
        #
        if instance=="EF" :
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
        #   (CutsInThisChain is a unique list of indices for of AllowedCuts)
        CutsInThisChain = []
        for req in btagReqs:
            if req[1] not in CutsInThisChain: 
                CutsInThisChain.append(AllowedCuts.index(req[1]))
        CutsInThisChain.sort()

        #
        #  Make a list of jet multiplicities requirements
        #    fullTrigReqAND is the AND of them
        #      eg: 2b35_2j25  is the AND of 2b35 and 4j35)
        fullTrigReqAND     = {}
        for jetReq in btagReqs:

            thisPt   = jetReq[0]
            thisBTag = AllowedCuts.index(jetReq[1])
            thisMult = jetReq[2]

            # Upper cut on btagging 
            #   (Used below for cases when the btagging is on lower pt jets, set it to 99 now) 
            #      eg: j100_2b55 
            thisMaxBTag = 99

            #
            # Add/Increment this combination of pt and btag.
            #
            if (thisPt,thisBTag,thisMaxBTag) not in fullTrigReqAND:
                fullTrigReqAND[(thisPt,thisBTag,thisMaxBTag)] = thisMult
            else:
                fullTrigReqAND[(thisPt,thisBTag,thisMaxBTag)] += thisMult

            #
            # For each btagging point requested add to the multiplicity of the looser btag requirments
            #  This is effectively dealing with the overlap
            #   eg: for 2b_2j, the multiplicity of the 2b reqirement is added the "j" multiplcity 
            #
            for point in CutsInThisChain:
                if point > thisBTag:
                    if (thisPt,point,thisMaxBTag) not in fullTrigReqAND:
                        fullTrigReqAND[(thisPt,point,thisMaxBTag)] = thisMult
                    else:
                        fullTrigReqAND[(thisPt,point,thisMaxBTag)] += thisMult

        #
        # Update the pt thresholds                                                                                                                           
        #  (for tagging requirements with the same operating point but differnet pTs,
        #   add the larger pt multiplcity to the smaller Pt to deal with overlap)
        #  eg: b150_b50  gives two requirements 150b  and 2b50
        for jetDef in fullTrigReqAND:
            for otherJetDefs in fullTrigReqAND:
                if jetDef == otherJetDefs: continue
                if jetDef[0] > otherJetDefs[0] and jetDef[1] == otherJetDefs[1] :
                    fullTrigReqAND[otherJetDefs] += fullTrigReqAND[jetDef]
                

        mlog.debug( "full btagReqsAND are")
        mlog.debug( fullTrigReqAND)
        
        #
        #  Format lists in a way that can be passed to the c++
        #
        EtThresholds   = []
        BTagMin        = []
        BTagMax        = []
        Multiplicities = []
        for reqAND in fullTrigReqAND:
            EtThresholds  .append(float(reqAND[0]))
            BTagMin       .append(float(self.getCutValue(AllowedCuts[reqAND[1]])))
            BTagMax       .append(float(reqAND[2]))
            Multiplicities.append(int  (fullTrigReqAND[reqAND]))

        mlog.debug("EtThresholds:")
        mlog.debug(EtThresholds)
        self.EtThresholds   = EtThresholds

        mlog.debug("BTagMin:")
        mlog.debug(BTagMin)
        self.BTagMin        = BTagMin

        mlog.debug("BTagMax:")
        mlog.debug(BTagMax)
        self.BTagMax        = BTagMax

        mlog.debug("Multiplicities:")
        mlog.debug(Multiplicities)
        self.Multiplicities = Multiplicities

        #
        # Handle the cases where have a higher pt looser leg
        #  Here we need to make requirements that get OR
        #  eg: j100_2b55 passes only if:
        #        - (j100 & 3b55) || (j100 & 2b55 & !b100)
        #
        #  NOTE: At the moment the following logic only hold for one highpt/looser requiremnt 
        #        It will need to be extending to handle more than 2 operating points if needed.
        #         (eg: j100_2btight55 work fine, but j100_bloose55_btight35 would not)
        # 
        fullTrigReqOR      = []

        # Loop on unique pairs
        for jetDef in fullTrigReqAND:
            for otherJetDefs in fullTrigReqAND:
                if jetDef == otherJetDefs: continue

                # check if pt is harder
                jetDef_ptBigger = jetDef[0] > otherJetDefs[0] 

                # check if btagging is looser (higher AllowedCuts index means looser btagging)
                jetDef_btagLooser = jetDef[1] > otherJetDefs[1]

                # if both harder and looser, add two cases to be ORed as example above
                if jetDef_ptBigger and jetDef_ptBigger :
                    
                    # 1st) both requirements pass the tighter btagging
                    #  eg: j100 & 3b55 (in our example above)
                    fullTrigReqOR.append({otherJetDefs: (fullTrigReqAND[otherJetDefs] + fullTrigReqAND[jetDef])})

                    # 2nd) this requriemnts has two sub requirements
                    #      - the tighter btagging requirement passes with its multiplicity
                    #            eg: 2b55
                    #      - the looser btagging requirement passes with its multiplicity, and an upper cut on btagging
                    #            eg: j100 & !b100
                    #  (upper cut is given by the 3rd index in the requirement
                    fullTrigReqOR.append({ otherJetDefs                          : fullTrigReqAND[otherJetDefs],
                                          (jetDef[0],jetDef[1],otherJetDefs[1])  : fullTrigReqAND[jetDef]})
                    

        #
        #  Format lists of ORs in a way that can be passed to the c++
        #
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
                if jetReq[2] < len(AllowedCuts):
                    thisBTagMaxOR         .append(self.getCutValue(AllowedCuts[jetReq[2]]))
                else:
                    thisBTagMaxOR         .append(jetReq[2])
                thisMultiplicitiesOR  .append(jetReqAndComo[jetReq])

            EtThresholdsOR   .append(thisEtThresholdsOR)
            BTagMinOR        .append(thisBTagMinOR)
            BTagMaxOR        .append(thisBTagMaxOR)
            MultiplicitiesOR .append(thisMultiplicitiesOR)


        mlog.debug("EtThresholdsOR:")
        mlog.debug(EtThresholdsOR)
        self.EtThresholdsOR    = EtThresholdsOR   

        mlog.debug("BTagMinOR:")
        mlog.debug(BTagMinOR)
        self.BTagMinOR         = BTagMinOR        

        mlog.debug("BTagMaxOR:")
        mlog.debug(BTagMaxOR)
        self.BTagMaxOR         = BTagMaxOR        

        mlog.debug("MultiplicitiesOR:")
        mlog.debug(MultiplicitiesOR)
        self.MultiplicitiesOR  = MultiplicitiesOR 
        
        from TrigBjetHypo.TrigBjetHypoMonitoring import TrigEFBjetHypoAllTEValidationMonitoring, TrigEFBjetHypoAllTEOnlineMonitoring
        validation = TrigEFBjetHypoAllTEValidationMonitoring()
        online     = TrigEFBjetHypoAllTEOnlineMonitoring()
       
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBjetHypoAllTE")
        time.TimerHistLimits = [0,0.4]
       
        self.AthenaMonTools = [ time, validation, online ]


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

            
