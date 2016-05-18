# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import string




class InformationHandler:
    
    def __init__(self):
        
        #setup the member variables
        # they are filled in the helper functions setupInfo_*
        self.m_Infos_Int        = {}
        self.m_Infos_Double     = {}
        self.m_Infos_String     = {}
        self.m_Infos_VecDouble  = {}
        self.m_Infos_VecString  = {}
        
        #fill member variables
        self.setupInfo_Int()
        self.setupInfo_Double()
        self.setupInfo_String()
        self.setupInfo_VecDouble()
        self.setupInfo_VecString()
        
        #fill the feature types and default values
        # -> a bit more involved, have extra function for this
        self.setupInfo_Features()
    #end init
    
    
    def dump(self):
        print("PanTau::InformationHandler: Printing stored information")
        
        print("PanTau::InformationHandler: \tIntegers:")
        for iInfo in sorted(self.m_Infos_Int.iterkeys()):
            print("\t\t" + string.ljust(iInfo, 50) + ": " + string.rjust(str(self.m_Infos_Int[iInfo]), 10) )

        print("PanTau::InformationHandler: \tDoubles:")
        for iInfo in sorted(self.m_Infos_Double.iterkeys()):
            print("\t\t" + string.ljust(iInfo, 50) + ": " + string.rjust(str(self.m_Infos_Double[iInfo]), 10) )

        print("PanTau::InformationHandler: \tStrings:")
        for iInfo in sorted(self.m_Infos_String.iterkeys()):
            print("\t\t" + string.ljust(iInfo, 50) + ": " + string.ljust(self.m_Infos_String[iInfo], 80) )

        print("PanTau::InformationHandler: \tVecDoubles:")
        for iInfo in sorted(self.m_Infos_VecDouble.iterkeys()):
            print("InformationHandler: \t\t" + string.ljust(iInfo, 50) + ":")
            for iVal in self.m_Infos_VecDouble[iInfo]:
                print("InformationHandler: \t\t\t" + string.rjust(str(iVal), 10) )

        print("PanTau::InformationHandler: \tVecStrings:")
        for iInfo in sorted(self.m_Infos_VecString.iterkeys()):
            print("InformationHandler: \t\t" + string.ljust(iInfo, 50) + ":")
            for iVal in self.m_Infos_VecString[iInfo]:
                print("InformationHandler: \t\t\t" + string.ljust(str(iVal), 80) )

        print("PanTau::InformationHandler: Done printing stored information")
    #end dump
    
    
    def setupInfo_Int(self):
        from PanTauAlgs.Config_PanTau import config_PanTau
#        self.m_Infos_Int["EnableXAODMigrationHacks"]        = int(config_PanTau.EnableXAODMigrationHacks())
        self.m_Infos_Int["TauConstituents_UsePionMass"]     = int(config_PanTau.TauConstituents_UsePionMass())
        self.m_Infos_Int["FeatureExtractor_UseEmptySeeds"]  = int(config_PanTau.FeatureExtractor_UseEmptySeeds())
        self.m_Infos_Int["TauConstituents_eflowRec_UseMomentumAsEnergy"]  = int(config_PanTau.TauConstituents_eflowRec_UseMomentumAsEnergy())
        self.m_Infos_Int["TauConstituents_UseShrinkingCone"]              = int(config_PanTau.TauConstituents_UseShrinkingCone())
        #nothing to do here yet (no integer infos...)
    #end def setupInfo_Int
    
    
    def setupInfo_Double(self):
        from PanTauAlgs.Config_PanTau import config_PanTau
        
        self.m_Infos_Double["eflowRec_Assoc_DeltaR"]                            = config_PanTau.eflowRec_Assoc_DeltaR()
        self.m_Infos_Double["TauConstituents_Types_DeltaRCore"]                 = config_PanTau.TauConstituents_Types_DeltaRCore()
        self.m_Infos_Double["TauConstituents_MaxEta"]                           = config_PanTau.TauConstituents_MaxEta()
        self.m_Infos_Double["TauConstituents_PreselectionMinEnergy"]            = config_PanTau.TauConstituents_PreselectionMinEnergy()
        self.m_Infos_Double["TauConstituents_eflowRec_BDTThreshold_Pi0Neut"]    = config_PanTau.TauConstituents_eflowRec_BDTThreshold_Pi0Neut()
        
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R10X_CellBased"] = config_PanTau.DecayModeDeterminator_BDTCutValue_R10X_CellBased()
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R11X_CellBased"] = config_PanTau.DecayModeDeterminator_BDTCutValue_R11X_CellBased()
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R110_CellBased"] = config_PanTau.DecayModeDeterminator_BDTCutValue_R110_CellBased()
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R1XX_CellBased"] = config_PanTau.DecayModeDeterminator_BDTCutValue_R1XX_CellBased()
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R30X_CellBased"] = config_PanTau.DecayModeDeterminator_BDTCutValue_R30X_CellBased()
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R3XX_CellBased"] = config_PanTau.DecayModeDeterminator_BDTCutValue_R3XX_CellBased()

        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R10X_eflowRec"]  = config_PanTau.DecayModeDeterminator_BDTCutValue_R10X_eflowRec()
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R11X_eflowRec"]  = config_PanTau.DecayModeDeterminator_BDTCutValue_R11X_eflowRec()
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R110_eflowRec"]  = config_PanTau.DecayModeDeterminator_BDTCutValue_R110_eflowRec()
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R1XX_eflowRec"]  = config_PanTau.DecayModeDeterminator_BDTCutValue_R1XX_eflowRec()
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R30X_eflowRec"]  = config_PanTau.DecayModeDeterminator_BDTCutValue_R30X_eflowRec()
        self.m_Infos_Double["DecayModeDeterminator_BDTCutValue_R3XX_eflowRec"]  = config_PanTau.DecayModeDeterminator_BDTCutValue_R3XX_eflowRec()
        
        
        #self.m_Infos_Double[""] = config_PanTau.()
    
    #end def setupInfo_Double
    
    
    def setupInfo_String(self):
        from PanTauAlgs.Config_PanTau import config_PanTau
        self.m_Infos_String["Name_TauRecContainer"]             = config_PanTau.Name_TauRecContainer()
        self.m_Infos_String["Name_eflowRecContainer"]           = config_PanTau.Name_eflowRecContainer()
        self.m_Infos_String["Name_TrackParticleContainer"]      = config_PanTau.Name_TrackParticleContainer()
        self.m_Infos_String["Name_PanTauSeedsContainer"]        = config_PanTau.Name_PanTauSeedsContainer()
#        self.m_Infos_String["Name_PanTauDetailsContainer"]      = config_PanTau.Name_PanTauDetailsContainer()
        self.m_Infos_String["ModeDiscriminator_ReaderOption"]   = config_PanTau.ModeDiscriminator_ReaderOption()
        self.m_Infos_String["ModeDiscriminator_TMVAMethod"]     = config_PanTau.ModeDiscriminator_TMVAMethod()
        #self.m_Infos_Double[""]        = config_PanTau.()

    #end def setupInfo_String
    
    
    def setupInfo_VecDouble(self):
        from PanTauAlgs.Config_PanTau import config_PanTau
        self.m_Infos_VecDouble["TauConstituents_BinEdges_Eta"]                       = config_PanTau.TauConstituents_BinEdges_Eta()
        self.m_Infos_VecDouble["TauConstituents_Selection_Neutral_EtaBinned_EtCut"]  = config_PanTau.TauConstituents_Selection_Neutral_EtaBinned_EtCut()
        self.m_Infos_VecDouble["TauConstituents_Selection_Pi0Neut_EtaBinned_EtCut"]  = config_PanTau.TauConstituents_Selection_Pi0Neut_EtaBinned_EtCut()
        self.m_Infos_VecDouble["TauConstituents_Selection_Charged_EtaBinned_EtCut"]  = config_PanTau.TauConstituents_Selection_Charged_EtaBinned_EtCut()
        self.m_Infos_VecDouble["TauConstituents_Selection_OutNeut_EtaBinned_EtCut"]  = config_PanTau.TauConstituents_Selection_OutNeut_EtaBinned_EtCut()
        self.m_Infos_VecDouble["TauConstituents_Selection_OutChrg_EtaBinned_EtCut"]  = config_PanTau.TauConstituents_Selection_OutChrg_EtaBinned_EtCut()
        self.m_Infos_VecDouble["TauConstituents_Selection_NeutLowA_EtaBinned_EtCut"] = config_PanTau.TauConstituents_Selection_NeutLowA_EtaBinned_EtCut()
        self.m_Infos_VecDouble["TauConstituents_Selection_NeutLowB_EtaBinned_EtCut"] = config_PanTau.TauConstituents_Selection_NeutLowB_EtaBinned_EtCut()
        self.m_Infos_VecDouble["eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_1prong"]  = config_PanTau.eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_1prong()
        self.m_Infos_VecDouble["eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_3prong"]  = config_PanTau.eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_3prong()
        self.m_Infos_VecDouble["CellBased_BinEdges_Eta"]                             = config_PanTau.CellBased_BinEdges_Eta()
        self.m_Infos_VecDouble["CellBased_EtaBinned_Pi0MVACut_1prong"]               = config_PanTau.CellBased_EtaBinned_Pi0MVACut_1prong()
        self.m_Infos_VecDouble["CellBased_EtaBinned_Pi0MVACut_3prong"]               = config_PanTau.CellBased_EtaBinned_Pi0MVACut_3prong()
        self.m_Infos_VecDouble["eflowRec_BinEdges_Eta"]                              = config_PanTau.eflowRec_BinEdges_Eta()
        self.m_Infos_VecDouble["eflowRec_EtaBinned_Pi0MVACut_1prong"]                = config_PanTau.eflowRec_EtaBinned_Pi0MVACut_1prong()
        self.m_Infos_VecDouble["eflowRec_EtaBinned_Pi0MVACut_3prong"]                = config_PanTau.eflowRec_EtaBinned_Pi0MVACut_3prong()
        self.m_Infos_VecDouble["ModeDiscriminator_BinEdges_Pt"]                      = config_PanTau.ModeDiscriminator_BinEdges_Pt()
        #self.m_Infos_VecDouble[""] = config_PanTau.()

    #end def setupInfo_VecDouble
    
        
    def setupInfo_VecString(self):
        from PanTauAlgs.Config_PanTau import config_PanTau
        self.m_Infos_VecString["ModeDiscriminator_BDTVariableNames_eflowRec_1p0n_vs_1p1n"] = config_PanTau.ModeDiscriminator_BDTVariableNames_eflowRec_1p0n_vs_1p1n()
        self.m_Infos_VecString["ModeDiscriminator_BDTVariableNames_eflowRec_1p1n_vs_1pXn"] = config_PanTau.ModeDiscriminator_BDTVariableNames_eflowRec_1p1n_vs_1pXn()
        self.m_Infos_VecString["ModeDiscriminator_BDTVariableNames_eflowRec_3p0n_vs_3pXn"] = config_PanTau.ModeDiscriminator_BDTVariableNames_eflowRec_3p0n_vs_3pXn()
        self.m_Infos_VecString["ModeDiscriminator_BDTVariableNames_CellBased_1p0n_vs_1p1n"] = config_PanTau.ModeDiscriminator_BDTVariableNames_CellBased_1p0n_vs_1p1n()
        self.m_Infos_VecString["ModeDiscriminator_BDTVariableNames_CellBased_1p1n_vs_1pXn"] = config_PanTau.ModeDiscriminator_BDTVariableNames_CellBased_1p1n_vs_1pXn()
        self.m_Infos_VecString["ModeDiscriminator_BDTVariableNames_CellBased_3p0n_vs_3pXn"] = config_PanTau.ModeDiscriminator_BDTVariableNames_CellBased_3p0n_vs_3pXn()
        #self.m_Infos_VecString[""] = config_PanTau.()

    #end def setupInfo_VecString
    
    
    def setupInfo_Features(self):
        
        #get list of algorithms and signal modes from config
        from PanTauAlgs.Config_PanTau import config_PanTau
        List_InputAlgs      = config_PanTau.Names_InputAlgorithms()
        List_ModeCases      = config_PanTau.Names_ModeCases()
        
        #get the feature handler
        from PanTauAlgs.Class_FeatureHandler import FeatureHandler
        theFeatureHandler = FeatureHandler()
        
        #for each algorithm and mode, loop over variables to be used in BDT
        # and fetch the type of the variable and the name
        # these are needed for the PanTauFillerTool in TauD3PDMaker
        for iAlg in List_InputAlgs:
            for iModeCase in List_ModeCases:
                
                infoKey_Names = "ModeDiscriminator_BDTVariableNames_" + iAlg + "_" + iModeCase
                curVarList = self.m_Infos_VecString[infoKey_Names]
                
                BDTVariable_TypeList        = []
                BDTVariable_DefaultValList  = []
                
                for iVar in curVarList:
                    theType     = theFeatureHandler.m_Feature_Types[iVar]
                    theDefVal   = theFeatureHandler.m_Feature_Defaults[iVar]
                    BDTVariable_TypeList        += [theType]
                    BDTVariable_DefaultValList  += [theDefVal]
                #end loop over variables
                
                infoKey_Types       = "ModeDiscriminator_BDTVariableTypes_" + iAlg + "_" + iModeCase
                self.m_Infos_VecString[infoKey_Types] = BDTVariable_TypeList
                
                infoKey_Defaults    = "ModeDiscriminator_BDTVariableDefaults_" + iAlg + "_" + iModeCase
                self.m_Infos_VecDouble[infoKey_Defaults] = BDTVariable_DefaultValList
                
            #end loop over signal modes
        #end loop over algs for default value creation for ModeDiscriminator tools
        
        #add the prefixes for the variables
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_Sum"]         = theFeatureHandler.m_VarTypeName_Sum
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_Ratio"]       = theFeatureHandler.m_VarTypeName_Ratio
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_EtInRing"]    = theFeatureHandler.m_VarTypeName_EtInRing
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_Isolation"]   = theFeatureHandler.m_VarTypeName_Isolation
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_Num"]         = theFeatureHandler.m_VarTypeName_Num
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_Mean"]        = theFeatureHandler.m_VarTypeName_Mean
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_StdDev"]      = theFeatureHandler.m_VarTypeName_StdDev
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_HLV"]         = theFeatureHandler.m_VarTypeName_HLV
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_Angle"]       = theFeatureHandler.m_VarTypeName_Angle
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_DeltaR"]      = theFeatureHandler.m_VarTypeName_DeltaR
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_JetMoment"]   = theFeatureHandler.m_VarTypeName_JetMoment
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_Combined"]    = theFeatureHandler.m_VarTypeName_Combined
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_JetShape"]    = theFeatureHandler.m_VarTypeName_JetShape
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_ImpactParams"]= theFeatureHandler.m_VarTypeName_ImpactParams
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_Basic"]       = theFeatureHandler.m_VarTypeName_Basic
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_PID"]         = theFeatureHandler.m_VarTypeName_PID
        self.m_Infos_String["FeatureExtractor_VarTypeName_varTypeName_Shots"]       = theFeatureHandler.m_VarTypeName_Shots
    #end def setupInfo_Features
    
#end class config_FeatureCalculator




