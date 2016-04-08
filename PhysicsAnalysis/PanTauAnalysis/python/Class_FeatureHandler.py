# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import string

class FeatureHandler:
    
    def getFeatureNames(self):
        listFeatNames = []
        for iFeature in self.m_Feature_Names:
            listFeatNames += [self.m_Feature_Names[iFeature]]
        return listFeatNames
    #end getFeatureNames
    
    def getFeatureTypes(self):
        listFeatTypes = []
        for iFeature in self.m_Feature_Types:
            listFeatTypes += [self.m_Feature_Types[iFeature]]
        return listFeatTypes
    #end getFeatureTypes
    
    def getFeatureDefaultValues(self):
        listFeatDefaults = []
        for iFeature in self.m_Feature_Defaults:
            listFeatDefaults += [self.m_Feature_Defaults[iFeature]]
        return listFeatDefaults
    #end getFeatureDefaultValues
    
    def addToFeatures(self, theBaseName, theVarTypeName, theFeatureType, defaultValueOverride=0):
        """ Add a single feature with given name, type and default value """
        
        #set feature name and default value
        featureName     = theVarTypeName + "_" + theBaseName
        defaultValue    = self.m_DefaultValues[theVarTypeName]
        if defaultValueOverride != 0:
            defaultValue    = defaultValueOverride
        
        #add to features
        self.m_Feature_Names[featureName]    = featureName
        self.m_Feature_Defaults[featureName] = defaultValue
        self.m_Feature_Types[featureName]    = theFeatureType
    #end addToFeatures
    
    def addToFeatures_FullName(self, completeFeatureName, theVarTypeName, theFeatureType, defaultValueOverride=0):
        #set feature name and default value
        featureName     = completeFeatureName
        defaultValue    = self.m_DefaultValues[theVarTypeName]
        if defaultValueOverride != 0:
            defaultValue    = defaultValueOverride
        
        #add to features
        self.m_Feature_Names[featureName]    = featureName
        self.m_Feature_Defaults[featureName] = defaultValue
        self.m_Feature_Types[featureName]    = theFeatureType
    #end addToFeatures_FullName
    
    
    def addToFeatures_AllTypes(self, theBaseName, theVarTypeName, theFeatureType, defaultValueOverride=0):
        """ Add the feature described by theName, theType and theDefault for all known constituent types"""
        
        #set default value
        defaultValue    = self.m_DefaultValues[theVarTypeName]
        if defaultValueOverride != 0:
            defaultValue = defaultValueOverride
        
        #fill feature for all types
        for iConstType in self.m_ConstituentTypes:
            featureName     = iConstType + "_" + theVarTypeName + "_" + theBaseName
            
            self.m_Feature_Names[featureName]    = featureName
            self.m_Feature_Defaults[featureName] = defaultValue
            self.m_Feature_Types[featureName]    = theFeatureType
        #end loop over types
    #end addToFeatures_AllTypes
    
    
    def addToFeatures_AllTypes_AllEnergyVariants(self, theBaseName, theVarTypeName, theFeatureType, defaultValueOverride=0):
        """ Add the feature described by theName, theType and theDefault for all known constituent types"""
        for iEnergyType in self.m_EnergyVariantsList:
            featureName     = theBaseName + iEnergyType
            self.addToFeatures_AllTypes(featureName, theVarTypeName, theFeatureType, defaultValueOverride)
        #end loop over energy types
    #end addToFeatures_AllTypes
    
    def __init__(self):
        
        #setup the varType prefixes
        # they are passed to the feature extractor later on - avoids errors due to typos
        self.m_VarTypeName_Sum          = "Sum"
        self.m_VarTypeName_Ratio        = "Ratio"
        self.m_VarTypeName_EtInRing     = "EtInRing"
        self.m_VarTypeName_Isolation    = "Isolation"
        self.m_VarTypeName_Num          = "Num"
        self.m_VarTypeName_Mean         = "Mean"
        self.m_VarTypeName_StdDev       = "StdDev"
        self.m_VarTypeName_HLV          = "HLV"
        self.m_VarTypeName_Angle        = "Angle"
        self.m_VarTypeName_DeltaR       = "DeltaR"
        self.m_VarTypeName_JetMoment    = "JetMoment"
        self.m_VarTypeName_Combined     = "Combined"
        self.m_VarTypeName_JetShape     = "JetShape"
        self.m_VarTypeName_ImpactParams = "ImpactParams"
        self.m_VarTypeName_Basic        = "Basic"
        self.m_VarTypeName_PID          = "PID"
        self.m_VarTypeName_Shots        = "Shots"
        
        #setup the default values for the different variable types
        self.m_DefaultValues = {}
        self.m_DefaultValues[self.m_VarTypeName_Sum]          = -4000.0
        self.m_DefaultValues[self.m_VarTypeName_Ratio]        = -0.2
        self.m_DefaultValues[self.m_VarTypeName_EtInRing]     = -1000.0
        self.m_DefaultValues[self.m_VarTypeName_Isolation]    = -0.2
        self.m_DefaultValues[self.m_VarTypeName_Num]          = -5.0
        self.m_DefaultValues[self.m_VarTypeName_Mean]         = -0.2
        self.m_DefaultValues[self.m_VarTypeName_StdDev]       = -0.2
        self.m_DefaultValues[self.m_VarTypeName_HLV]          = -4000.0
        self.m_DefaultValues[self.m_VarTypeName_Angle]        = -0.2
        self.m_DefaultValues[self.m_VarTypeName_DeltaR]       = -0.2
        self.m_DefaultValues[self.m_VarTypeName_JetMoment]    = -0.2
        self.m_DefaultValues[self.m_VarTypeName_Combined]     = -10.0
        self.m_DefaultValues[self.m_VarTypeName_JetShape]     = -2.0
        self.m_DefaultValues[self.m_VarTypeName_ImpactParams] = -100.
        self.m_DefaultValues[self.m_VarTypeName_Basic]        = -5.0
        self.m_DefaultValues[self.m_VarTypeName_PID]          = -9.0
        self.m_DefaultValues[self.m_VarTypeName_Shots]        = -2.0
        
        #setup the constituent types
        self.m_ConstituentTypeName_All      = "All"
        self.m_ConstituentTypeName_Charged  = "Charged"
        self.m_ConstituentTypeName_Neutral  = "Neutral"
        self.m_ConstituentTypeName_Pi0Neut  = "Pi0Neut"
        self.m_ConstituentTypeName_OutNeut  = "OuterNeut"
        self.m_ConstituentTypeName_OutChrg  = "OuterChrg"
        self.m_ConstituentTypeName_NeutLowA = "NeutLowA"
        self.m_ConstituentTypeName_NeutLowB = "NeutLowB"
        
        self.m_ConstituentTypes = []
        #baseline
        self.m_ConstituentTypes += [self.m_ConstituentTypeName_Charged]
        self.m_ConstituentTypes += [self.m_ConstituentTypeName_Neutral]
        self.m_ConstituentTypes += [self.m_ConstituentTypeName_Pi0Neut]
        #for testing
        self.m_ConstituentTypes += [self.m_ConstituentTypeName_NeutLowA]
        self.m_ConstituentTypes += [self.m_ConstituentTypeName_NeutLowB]
        
        self.m_EnergyVariantsList = []
        self.m_EnergyVariantsList += ["EtAllConsts"]
        self.m_EnergyVariantsList += ["EtNeutLowA"]
        self.m_EnergyVariantsList += ["EtNeutLowB"]
        # ==============================================
        
        
        #setup member variables to hold feature info
        self.m_Feature_Names      = {}
        self.m_Feature_Defaults   = {}
        self.m_Feature_Types      = {}
        
        #fill member variables
        self.addSingleVariables()
        self.addMultiplicities()
        self.addFourMomentum()
        self.addTypeSpecificFeatures_Num()
        self.addTypeSpecificFeatures_PID()
        self.addTypeSpecificFeatures_Shots()
        self.addTypeSpecificFeatures_Ratios()
        #self.addTypeSpecificFeatures_EtRings()     #Note wrt EtRings and Isolations:
        #self.addTypeSpecificFeatures_Isolations()  #     Move to different function in FeatureCalc
        self.addTypeSpecificFeatures_Mean()
        self.addTypeSpecificFeatures_StdDevs()
        self.addTypeSpecificFeatures_HLV()
        self.addTypeSpecificFeatures_Angle()
        self.addTypeSpecificFeatures_DeltaR()
        self.addTypeSpecificFeatures_JetMoment()
        self.addCombinedFeatures_Single()
        self.addCombinedFeatures_TypeVsType()
        self.addCombinedFeatures_SelectedTypes()
        self.addGenericJetFeatures()
        self.addImpactParameterFeatures()
        
    #end init
    
    
    def dump(self):
        print("config_FeatureCalculator: Dumping features:")
        for iFeature in sorted(self.m_Feature_Names.iterkeys()):
            featName    = self.m_Feature_Names[iFeature]
            featType    = self.m_Feature_Types[iFeature]
            featDefault = self.m_Feature_Defaults[iFeature]
            print("\t" + string.ljust(featName, 50) + " of type " + featType + " defaults to " + string.rjust(str(featDefault), 10) )
            #print("\t" + featName + " of type " + featType + " defaults to " + str(featDefault) )
        #end for over features
        print("config_FeatureCalculator: Done dumping features")
    #end dump
    
    
    def addSingleVariables(self):
        Variables = []
        Variables += ["isPanTauCandidate"]
        Variables += ["RecoMode"]
        Variables += ["RecoMode_PanTau"]
        Variables += ["BDTValue_1p0n_vs_1p1n"]
        Variables += ["BDTValue_1p1n_vs_1pXn"]
        Variables += ["BDTValue_3p0n_vs_3pXn"]
        Variables += ["SumCharge"]
        Variables += ["AbsCharge"]
        for iVar in Variables:
            curDefVal = self.m_DefaultValues[self.m_VarTypeName_Basic]
            if iVar == "isPanTauCandidate":
                curDefVal = 0
            if iVar == "RecoMode" or iVar == "RecoMode_PanTau":
                curDefVal = -10
            self.addToFeatures(iVar, self.m_VarTypeName_Basic, "F", curDefVal)
        #end loop over variables
    #end def addSingleVariables
    
    
    def addMultiplicities(self):
        Variables = self.m_ConstituentTypes
        
        for iVar in Variables:
            featName = "N" + iVar + "Consts"
            self.addToFeatures(featName, self.m_VarTypeName_Basic, "F")
        #end loop over variables
        
        #add the inclusive one
        self.addToFeatures("N" + self.m_ConstituentTypeName_All     + "Consts", self.m_VarTypeName_Basic, "F")
        self.addToFeatures("N" + self.m_ConstituentTypeName_OutChrg + "Consts", self.m_VarTypeName_Basic, "F")
        self.addToFeatures("N" + self.m_ConstituentTypeName_OutNeut + "Consts", self.m_VarTypeName_Basic, "F")
        
    #end def addMultiplicities
    
    
    def addFourMomentum(self):
        Variables = []
        Variables += ["pt"]
        Variables += ["eta"]
        Variables += ["phi"]
        Variables += ["m"]
        
        Types = []
        Types += ["ProtoMomentumCore"]
        Types += ["ProtoMomentumWide"]
        #Types += ["ValMomCore2GeV"]
        
        
        for iVar in Variables:
            for iType in Types:
                curFeatName = iType + "_" + iVar
                curDefVal   = -4000.
                if iVar == "eta" or iVar == "phi":
                    curDefVal = -9.
                if iVar == "m":
                    curDefVal = -200.
                self.addToFeatures(curFeatName, self.m_VarTypeName_Basic, "F", curDefVal)
            #end loop types
        #end loop vars 
    #end def addFourMomentum
    
    
    def addTypeSpecificFeatures_Num(self):
        Variables = []
        Variables += ["ConstsIn00To01"]
        Variables += ["ConstsIn01To02"]
        Variables += ["ConstsIn02To03"]
        Variables += ["ConstsIn03To04"]
        for iVar in Variables:
            self.addToFeatures_AllTypes(iVar, self.m_VarTypeName_Num, "F")
    #end addTypeSpecificFeatures
    
    
    def addTypeSpecificFeatures_PID(self):
        Variables = []
        Variables += ["BDTValues"]
        Variables += ["BDTValuesSum"]
        
        Sorts = []
        Sorts += ["BDTSort"]
        Sorts += ["EtSort"]
        
        MaxNum = 4
        
        for iVar in Variables:
            for iSort in Sorts:
                for iNum in range(1, MaxNum):
                    featName = iVar + "_" + iSort + "_" + str(iNum)
                    self.addToFeatures_AllTypes(featName, self.m_VarTypeName_PID, "F")
                #end loop over num
            #end loop over sort
        #end loop over variables
    #end addTypeSpecificFeatures
    
    
    def addTypeSpecificFeatures_Shots(self):
        Variables = []
        Variables += ["nPhotons"]
        Variables += ["nShots"]
        Variables += ["SumShots_Et"]
        Variables += ["SumShots_Eta"]
        Variables += ["SumShots_Phi"]
        Variables += ["SumShots_M"]
        Variables += ["ConstDeltaRToSumShots"]
        Variables += ["EtSumShotsOverConstEt"]
        Variables += ["TauDeltaRToSumShots"]
        Variables += ["EtSumShotsOverTauEt"]
        
        Sorts = []
        Sorts += ["BDTSort"]
        
        MaxNum = 4
        
        for iVar in Variables:
            for iSort in Sorts:
                for iNum in range(1, MaxNum):
                    featName = iVar + "_" + iSort + "_" + str(iNum)
                    curDefVal = self.m_DefaultValues[self.m_VarTypeName_Shots]
                    if iVar == "Et":
                        curDefVal = -1000
                    if iVar == "Eta" or iVar == "Phi":
                        curDefVal = -8
                    if iVar == "M":
                        curDefVal = -200
                    
                    self.addToFeatures_FullName(self.m_ConstituentTypeName_Neutral + "_" + self.m_VarTypeName_Shots + "_" + featName, self.m_VarTypeName_Shots, "F", curDefVal)
                    self.addToFeatures_FullName(self.m_ConstituentTypeName_Pi0Neut + "_" + self.m_VarTypeName_Shots + "_" + featName, self.m_VarTypeName_Shots, "F", curDefVal)
                    self.addToFeatures_FullName(self.m_ConstituentTypeName_NeutLowA + "_" + self.m_VarTypeName_Shots + "_" + featName, self.m_VarTypeName_Shots, "F", curDefVal)
                    self.addToFeatures_FullName(self.m_ConstituentTypeName_NeutLowB + "_" + self.m_VarTypeName_Shots + "_" + featName, self.m_VarTypeName_Shots, "F", curDefVal)
                #end loop over iNum
            #end loop over sorts
        #end loop over variables
        
        Variables = []
        Variables += ["MaxDeltaRSumShotToConst"]
        Variables += ["MinDeltaRSumShotToConst"]
        Variables += ["MaxDeltaRSumShotToTau"]
        Variables += ["MinDeltaRSumShotToTau"]
        Variables += ["DeltaRAllShotsToTau"]
        Variables += ["EtAllShotsOverEtTau"]
        Variables += ["NShotsInSeed"]
        Variables += ["NPhotonsInSeed"]
        Variables += ["BestDiShotMass"]
        Variables += ["MinDiShotMass"]
        Variables += ["MaxDiShotMass"]
        for iVar in Variables:
            featName = iVar
            curDefVal = self.m_DefaultValues[self.m_VarTypeName_Shots]
            if iVar == "BestDiShotMass" or iVar == "MinDiShotMass" or iVar == "MaxDiShotMass":
                curDefVal = -200
            
            self.addToFeatures_FullName(self.m_ConstituentTypeName_Neutral + "_" + self.m_VarTypeName_Shots + "_" + featName, self.m_VarTypeName_Shots, "F", curDefVal)
            self.addToFeatures_FullName(self.m_ConstituentTypeName_Pi0Neut + "_" + self.m_VarTypeName_Shots + "_" + featName, self.m_VarTypeName_Shots, "F", curDefVal)
            self.addToFeatures_FullName(self.m_ConstituentTypeName_NeutLowA + "_" + self.m_VarTypeName_Shots + "_" + featName, self.m_VarTypeName_Shots, "F", curDefVal)
            self.addToFeatures_FullName(self.m_ConstituentTypeName_NeutLowB + "_" + self.m_VarTypeName_Shots + "_" + featName, self.m_VarTypeName_Shots, "F", curDefVal)
            
        #end loop over variables
    #end addTypeSpecificFeatures_Shots
    
    def addTypeSpecificFeatures_Ratios(self):
        
        Variables_WithEnergyTypes = []
        Variables_WithEnergyTypes += ["EtOver"]
        Variables_WithEnergyTypes += ["1stEtOver"]
        Variables_WithEnergyTypes += ["1stBDTEtOver"]
        for iVar in Variables_WithEnergyTypes:
            self.addToFeatures_AllTypes_AllEnergyVariants(iVar, self.m_VarTypeName_Ratio, "F")
        #end loop over variables with energy types
        
        Variables = []
        Variables += ["1stEtOverTypeEt"]
        Variables += ["1stBDTEtOverTypeEt"]
        Variables += ["EFOsOverTotalEFOs"]
        Variables += ["Log1stEtOver2ndEt"]
        Variables += ["Log1stEtOver3rdEt"]
        Variables += ["Log2ndEtOver3rdEt"]
        Variables += ["Log1stEtOver2ndEt_BDTSort"]
        Variables += ["Log1stEtOver3rdEt_BDTSort"]
        Variables += ["Log2ndEtOver3rdEt_BDTSort"]
        for iVar in Variables:
            self.addToFeatures_AllTypes(iVar, self.m_VarTypeName_Ratio, "F")
        #end loop over variables with energy types
    #end addTypeSpecificFeatures_Ratios
    
    
    def addTypeSpecificFeatures_EtRings(self):
        Variables = []
        Variables += ["00To01"]
        Variables += ["01To02"]
        Variables += ["02To03"]
        Variables += ["03To04"]
        Variables += ["04To05"]
        for iVar in Variables:
            completeFeatureName = self.m_ConstituentTypeName_All + "_" + self.m_VarTypeName_EtInRing + "_" + iVar
            self.addToFeatures_FullName(completeFeatureName, self.m_VarTypeName_EtInRing, "F")
        #end loop over variables
    #end addTypeSpecificFeatures_EtRings
    
    
    def addTypeSpecificFeatures_Isolations(self):
        Variables = []
        Variables += ["EtIn01Over"]
        Variables += ["EtIn02Over"]
        Variables += ["EtIn03Over"]
        Variables += ["EtIn04Over"]
        Variables += ["EtIn00To02Over"]
        Variables += ["EtIn02To04Over"]
        
        for iVar in Variables:
            for iEnergyType in self.m_EnergyVariantsList:
                completeFeatureName = self.m_ConstituentTypeName_All + "_" + self.m_VarTypeName_Isolation + "_" + iVar + iEnergyType
                self.addToFeatures_FullName(completeFeatureName, self.m_VarTypeName_Isolation, "F")
        #end loop over variables
        
        Variables = []
        Variables += ["EtIn01OverEtIn02"]
        Variables += ["EtIn01OverEtIn04"]
        for iVar in Variables:
            completeFeatureName = self.m_ConstituentTypeName_All + "_" + self.m_VarTypeName_Isolation + iVar
            self.addToFeatures_FullName(completeFeatureName, self.m_VarTypeName_Isolation, "F")
        #end loop over variables
        
    #end addTypeSpecificFeatures_Isolations
    
    
    def addTypeSpecificFeatures_Mean(self):
        Variables_WithEnergyTypes = []
        Variables_WithEnergyTypes += ["Et_Wrt"]
        Variables_WithEnergyTypes += ["DRToJetAxis_Wrt"]
        Variables_WithEnergyTypes += ["DRToLeading_Wrt"]
        for iVar in Variables_WithEnergyTypes:
            self.addToFeatures_AllTypes_AllEnergyVariants(iVar, self.m_VarTypeName_Mean, "F")
        #end loop over variables
    #end addTypeSpecificFeatures_Mean
    
    
    def addTypeSpecificFeatures_StdDevs(self):
        Variables = []
        Variables += ["E"]
        Variables += ["Et"]
        Variables += ["DRToJetAxis"]
        Variables += ["DRToLeading"]
        for iVar in Variables:
            self.addToFeatures_AllTypes(iVar, self.m_VarTypeName_StdDev, "F")
        #end loop over variables
        
        Variables_WithEnergyTypes = []
        Variables_WithEnergyTypes += ["Et_Wrt"]
        for iVar in Variables_WithEnergyTypes:
            self.addToFeatures_AllTypes_AllEnergyVariants(iVar, self.m_VarTypeName_StdDev, "F")
    #end addTypeSpecificFeatures_StdDevs
    
    
    def addTypeSpecificFeatures_HLV(self):
        Variables = []
        Variables += ["SumPt"]
        Variables += ["SumEta"]
        Variables += ["SumPhi"]
        Variables += ["SumM"]
        for iVar in Variables:
            curDefVal   = -4000.
            if iVar == "SumEta" or iVar == "SumPhi":
                curDefVal = -9.
            if iVar == "SumM":
                curDefVal = -200.
            self.addToFeatures_AllTypes(iVar, self.m_VarTypeName_HLV, "F", curDefVal)
            
            # also add the OutChrg and OutNeut
            featName = self.m_ConstituentTypeName_OutChrg + "_" + self.m_VarTypeName_HLV + "_" + iVar
            self.addToFeatures_FullName(featName, self.m_VarTypeName_HLV, "F", curDefVal)
            
            featName = self.m_ConstituentTypeName_OutNeut + "_" + self.m_VarTypeName_HLV + "_" + iVar
            self.addToFeatures_FullName(featName, self.m_VarTypeName_HLV, "F", curDefVal)
            
        #end loop over variables
        
        VariablesVec = []
        VariablesVec += ["Constituents_pt"]
        VariablesVec += ["Constituents_eta"]
        VariablesVec += ["Constituents_phi"]
        VariablesVec += ["Constituents_m"]
        
        VariablesVecSort = []
        VariablesVecSort += ["EtSort"]
        VariablesVecSort += ["BDTSort"]
        
        for iSort in VariablesVecSort:
            for iVecVar in VariablesVec:
                featName = iSort + "_" + iVecVar
                curDefVal   = -4000.
                if iVecVar == "Constituents_eta" or iVecVar == "Constituents_phi":
                    curDefVal = -9.
                if iVecVar == "Constituents_m":
                    curDefVal = -200.
                self.addToFeatures_AllTypes(featName, self.m_VarTypeName_HLV, "V", curDefVal)
                
                # also add the OutChrg and OutNeut
                featName = self.m_ConstituentTypeName_OutChrg + "_" + self.m_VarTypeName_HLV + "_" + iSort + "_" + iVecVar
                self.addToFeatures_FullName(featName, self.m_VarTypeName_HLV, "V", curDefVal)
                
                featName = self.m_ConstituentTypeName_OutNeut + "_" + self.m_VarTypeName_HLV + "_" + iSort + "_" + iVecVar
                self.addToFeatures_FullName(featName, self.m_VarTypeName_HLV, "V", curDefVal)
                
        #end loop over variables
    #end addTypeSpecificFeatures_HLV
    
    
    def addTypeSpecificFeatures_Angle(self):
        Variables = []
        Variables += ["ToJetAxis"]
        Variables += ["1stToJetAxis"]
        Variables += ["2ndToJetAxis"]
        Variables += ["3rdToJetAxis"]
        Variables += ["1stTo2nd"]
        Variables += ["1stTo3rd"]
        Variables += ["2ndTo3rd"]
        Variables += ["MaxToJetAxis"]
        Variables += ["MeanValue123"]
        Variables += ["Btw1213Planes"]
        for iVar in Variables:
            self.addToFeatures_AllTypes(iVar, self.m_VarTypeName_Angle, "F")
        #end loop over variables
    #end addTypeSpecificFeatures_Angle
    
    
    def addTypeSpecificFeatures_DeltaR(self):
        Variables = []
        Variables += ["ToJetAxis"]
        Variables += ["MaxToJetAxis"]
        Variables += ["1stToJetAxis"]
        Variables += ["2ndToJetAxis"]
        Variables += ["3rdToJetAxis"]
        Variables += ["1stTo2nd"]
        Variables += ["1stTo3rd"]
        Variables += ["2ndTo3rd"]
        
        Sorts = []
        Sorts += ["EtSort"]
        Sorts += ["BDTSort"]
        
        for iSort in Sorts:
            for iVar in Variables:
                featName = iVar + "_" + iSort
                self.addToFeatures_AllTypes(featName, self.m_VarTypeName_DeltaR, "F")
        #end loop over variables
    #end addTypeSpecificFeatures_DeltaR
    
    
    def addTypeSpecificFeatures_JetMoment(self):
        Variables = []
        Variables += ["EtDR"]
        Variables += ["EtDRprime"]
        Variables += ["EtDR2"]
        Variables += ["EtAngle"]
        Variables += ["EtDRxTotalEt"]
        for iVar in Variables:
            self.addToFeatures_AllTypes(iVar, self.m_VarTypeName_JetMoment, "F")
        #end loop over variables
    #end addTypeSpecificFeatures_JetMoment
    
    
    def addCombinedFeatures_Single(self):
        Variables = []
        Variables += ["NumChargedOverNumNeutral"]
        Variables += ["NumChargedOverNumTotal"]
        Variables += ["AnglePlane1stCharged1st2ndNeutral"]
        Variables += ["FarthestNeutral_AngleToCharged"]
        Variables += ["FarthestNeutral_BDTScore"]
        Variables += ["FarthestNeutral_EtOverChargedEt"]
        
        for iVar in Variables:
            self.addToFeatures(iVar, self.m_VarTypeName_Combined, "F")
        #end loop over variables
    #end addCombinedFeatures_Single
    
    
    def addCombinedFeatures_TypeVsType(self):
        Types = []
        Types += ["Charged"]
        Types += ["Neutral"]
        Types += ["Pi0Neut"]
        
        Variables = []
        Variables += [ ["Log1st", "EtOver", "Et"] ]
        Variables += [ ["Log", "EtOver", "Et"] ]
        Variables += [ ["Angle", "To", ""] ]
        Variables += [ ["DeltaR1st", "To1st", ""] ]
        Variables += [ ["Angle1st", "To1st", ""] ]
        for iType in Types:
            for jType in Types:
                
                #skip if types are equal
                if iType == jType:
                    continue
                
                for iVar in Variables:
                    featName = iVar[0] + iType + iVar[1] + jType + iVar[2]
                    #if iVar[2] != "-":
                        #featName = featName + iVar[2]
                    self.addToFeatures(featName, self.m_VarTypeName_Combined, "F")
                #end loop over variables
            #end loop over jType
        #end loop over iType
    #end addCombinedFeatures_TypeVsType
    
    
    def addCombinedFeatures_SelectedTypes(self):
        iTypes = []
        iTypes += ["Charged"]
        
        jTypes = []
        jTypes += ["Pi0Neut"]
        jTypes += ["Neutral"]
        
        Variables_WithEnergyTypes = []
        Variables_WithEnergyTypes += [ ["Mean", "Et_Wrt"] ]
        
        Variables = []
        Variables += [ ["InvMass", ""] ]
        Variables += [ ["Angle1st2nd", ""] ]
        for iType in iTypes:
            for jType in jTypes:
                
                for iVar in Variables:
                    featName = iVar[0] + iType + jType + iVar[1]
                    self.addToFeatures(featName, self.m_VarTypeName_Combined, "F")
                #end loop over variables
                
                for iVar in Variables_WithEnergyTypes:
                    featName = iVar[0] + iType + jType + iVar[1]
                    for iEnergyType in self.m_EnergyVariantsList:
                        featNameWithEnergy = featName + iEnergyType
                        self.addToFeatures(featNameWithEnergy, self.m_VarTypeName_Combined, "F")
                #end loop over variables for energy types
                
            #end loop over jTypes
        #end loop over iTypes
    #end addCombinedFeatures_SelectedTypes
    
    
    def addGenericJetFeatures(self):
        Variables = []
        
        #thrust and such
        Variables += ["JetThrust"]
        Variables += ["JetThrustMajor"]
        Variables += ["JetThrustMinor"]
        Variables += ["JetOblateness"]
        Variables += ["JetSphericity"]
        Variables += ["JetAplanarity"]
        Variables += ["JetPlanarity"]
        
        #fox wolfram moments
        Variables += ["JetFoxWolfram1"]
        Variables += ["JetFoxWolfram1"]
        Variables += ["JetFoxWolfram1"]
        Variables += ["JetFoxWolfram1"]
        Variables += ["JetFoxWolframRatioFW2OverFW1"]
        Variables += ["JetFoxWolframRatioFW4pow4OverFW1"]
        Variables += ["JetFoxWolframRatioFW234OverFW1pow4"]
        Variables += ["JetFoxWolframRatioFW1PlusFW2OverFW4"]
        
        for iVar in Variables:
            self.addToFeatures(iVar, self.m_VarTypeName_JetShape, "F")
        #end loop over variables
    #end addGenericJetFeatures
    
    
    def addImpactParameterFeatures(self):
        maxTrack = 4
        
        Variables = []
        Variables += [ ["TransIPTrack", "_SortByEt"] ]
        Variables += [ ["LongIPTrack", "_SortByEt"] ]
        Variables += [ ["TransSignfIPTrack", "_SortByEt"] ]
        Variables += [ ["LongSignfIPTrack", "_SortByEt"] ]
        Variables += [ ["TransIP", "_SortByValue"] ]
        Variables += [ ["TransSignfIP", "_SortByValue"] ]
        
        for iTrk in range(1, maxTrack):
            for iVar in Variables:
                featName = iVar[0] + str(iTrk) + iVar[1]
                self.addToFeatures(featName, self.m_VarTypeName_ImpactParams, "F")
            #end loop over variables
        #end loop over tracks
    #end addGenericJetFeatures
    
    
#end class config_FeatureCalculator



