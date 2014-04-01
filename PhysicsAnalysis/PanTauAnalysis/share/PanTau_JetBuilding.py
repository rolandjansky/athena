class JetBuilding:
        def __init__(self, JetAlgName, TruthJetCont="AntiKt4TruthJets", AllRecoJetCont="AntiKt4LCTopoJets"):

            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()
            
            # Build the list of decay modes that shall be separated (needed to get the case fragment right for weight files)
            # === IMPORTANT NOTE ===
            #NOTE: This order has to match the order in which the DiscTools add their likeliness to the likeliness vector in PanTau::TauJetBuilderAlg::execute.
            #NOTE: Convention for order: Start with Low prong and low neutrals, move up in neutrals, then move to next higher prong. Add fakes at the end.
            # 
            # However, this code should not need to be changed very often. Only if the number of modes change. In that case, one would have to revisit the code anyway...
            # Hence: Hands off, unless you know what you're getting yourself into :)
            from PanTauAnalysis.PanTauJetBuilderAlg_Flags import PanTauJetBuilderAlgFlags as PJB_Flags
            ListOfDecayModes = [PJB_Flags.MVA_ModeName1p0n(),
                                PJB_Flags.MVA_ModeName1p1n(),
                                PJB_Flags.MVA_ModeName1pXn(),
                                PJB_Flags.MVA_ModeName3p0n(),
                                PJB_Flags.MVA_ModeName3pXn()]
            
            # Add the discrimination tools
            from AthenaCommon.AppMgr import ToolSvc
            
            ### ====== Tool for 1p0n
            ItsName = "PanTauDiscrTool_" + JetAlgName + "_" + PJB_Flags.MVA_ModeName1p0n()
            DiscTool_1p0n = getDiscriminantTool(ItsName, JetAlgName, PJB_Flags.MVA_ModeName1p0n(), ListOfDecayModes, PJB_Flags.MVA_UsedFeatures1p0n())
            DiscTool_1p0n.OutputLevel = INFO
            ToolSvc += DiscTool_1p0n
            print DiscTool_1p0n
            
            ### ====== Tool for 1p1n
            ItsName = "PanTauDiscrTool_" + JetAlgName + "_" + PJB_Flags.MVA_ModeName1p1n()
            DiscTool_1p1n = getDiscriminantTool(ItsName, JetAlgName, PJB_Flags.MVA_ModeName1p1n(), ListOfDecayModes, PJB_Flags.MVA_UsedFeatures1p1n())
            DiscTool_1p1n.OutputLevel = INFO
            ToolSvc += DiscTool_1p1n
            print DiscTool_1p1n
            
            ### ====== Tool for 1pXn
            ItsName = "PanTauDiscrTool_" + JetAlgName + "_" + PJB_Flags.MVA_ModeName1pXn()
            DiscTool_1pXn = getDiscriminantTool(ItsName, JetAlgName, PJB_Flags.MVA_ModeName1pXn(), ListOfDecayModes, PJB_Flags.MVA_UsedFeatures1pXn())
            DiscTool_1pXn.OutputLevel = INFO
            ToolSvc += DiscTool_1pXn
            print DiscTool_1pXn
            
            ### ====== Tool for 3p0n
            ItsName = "PanTauDiscrTool_" + JetAlgName + "_" + PJB_Flags.MVA_ModeName3p0n()
            DiscTool_3p0n = getDiscriminantTool(ItsName, JetAlgName, PJB_Flags.MVA_ModeName3p0n(), ListOfDecayModes, PJB_Flags.MVA_UsedFeatures3p0n())
            DiscTool_3p0n.OutputLevel = INFO
            ToolSvc += DiscTool_3p0n
            print DiscTool_3p0n
            
            ### ====== Tool for 3pXn
            ItsName = "PanTauDiscrTool_" + JetAlgName + "_" + PJB_Flags.MVA_ModeName3pXn()
            DiscTool_3pXn = getDiscriminantTool(ItsName, JetAlgName, PJB_Flags.MVA_ModeName3pXn(), ListOfDecayModes, PJB_Flags.MVA_UsedFeatures3pXn())
            DiscTool_3pXn.OutputLevel = INFO
            ToolSvc += DiscTool_3pXn
            print DiscTool_3pXn
            
            
            
            from PanTauAnalysis.PanTauValidationFlags import panTauValidationFlags
            
            from PanTauAnalysis.PanTauSeedBuilderAlg_Flags import PanTauSeedBuilderAlgFlags as PSB_Flags
            PanTauGoodSeeds = PSB_Flags.ContainerName_PanTauGoodOutputSeeds() + JetAlgName
            PanTauBadSeeds  = PSB_Flags.ContainerName_PanTauBadOutputSeeds() + JetAlgName
            
            from PanTauAnalysis.PanTauValidationFlags import panTauValidationFlags
            
            from PanTauAlgs.PanTauAlgsConf import PanTau__TauJetBuilderAlg
            jetBuilderAlg = PanTau__TauJetBuilderAlg(   "TauJetBuild"+JetAlgName,
                                                        #container names...
                                                        ContainerName_PanTauSeedCollection = PanTauGoodSeeds,
                                                        ContainerName_PanTauRejectedSeedCollection = PanTauBadSeeds,
                                                        #the decay mode separation tools...
                                                        MVA_ListOfDecayModeNames = ListOfDecayModes,
                                                        MVA_DiscTool_1p0n = DiscTool_1p0n,
                                                        MVA_DiscTool_1p1n = DiscTool_1p1n,
                                                        MVA_DiscTool_1pXn = DiscTool_1pXn,
                                                        MVA_DiscTool_3p0n = DiscTool_3p0n,
                                                        MVA_DiscTool_3pXn = DiscTool_3pXn,
                                                        FeatureNames                        = panTauValidationFlags.AllFeatureNames(),
                                                        FeatureDefaultValues                = panTauValidationFlags.AllFeatureDefaultValues(),
                                                        FeatureDefaultValueSmearingSigma    = panTauValidationFlags.AllFeatureDefaultValueSmearingSigma(),
                                                        FeatureTypes                        = panTauValidationFlags.AllFeatureTypes(),
                                                        SmearDefaultValuesOfFeatures        = panTauValidationFlags.DoFeatureDefaultSmearing()
                                                         )
            jetBuilderAlg.OutputLevel = INFO
            topSequence += jetBuilderAlg
            print jetBuilderAlg
            
            #from RecExConfig.RecFlags import rec
            
        #end def__init__
#end class JetBuilding





def getDiscriminantTool(name, JetAlgName, WeightFilePrefixClass, ListOfAllModes, TauFeatureNames) :
    """
    Factory function to create a TauDiscriminantTool
    """
    from PanTauAnalysis.PanTauJetBuilderAlg_Flags import PanTauJetBuilderAlgFlags as PJB_Flags
    import math

    #set feature types and default values for the features
    # to be used in this disc tool
    featureDefaultValues=[]
    featureTypes=[]
    for v in TauFeatureNames:
        if (not (v in featureDefaultValue)) :
            print "ERROR: could not get default value for ", v
            break
        if (not (v in featureType)) :
            print "ERROR: could not get type for ", v
            break
        featureDefaultValues.append(featureDefaultValue[v])
        featureTypes.append(featureType[v])
    
    #build the currentCase string
    currentCase = WeightFilePrefixClass + "_vs_"
    for iMode in ListOfAllModes:
        if (WeightFilePrefixClass != iMode) :
            currentCase = currentCase + iMode
    
    #build the weight file names
    weightFilePrefixList = []
    for ptBin in range(1, len(PJB_Flags.MVA_PtBins())):
        Fragment_Base       = PJB_Flags.MVA_GeneralWeightFilePrefix()
        Fragment_EtBin      = "ET_" + str( int(math.floor(PJB_Flags.MVA_PtBins()[ptBin-1]/1000.)) ) + "_" + str( int(math.floor(PJB_Flags.MVA_PtBins()[ptBin]/1000.)) )
        Fragment_Case       = currentCase
        Fragment_Method     = PJB_Flags.MVA_Method()
        Fragment_Version    = PJB_Flags.MVA_VersionName()
        FileToAdd = Fragment_Base + "_" + Fragment_EtBin + "_" + Fragment_Case + "_" + Fragment_Method + "_" + Fragment_Version + ".weights.xml"
        if (Fragment_Version == "") :
            FileToAdd = Fragment_Base + "_" + Fragment_EtBin + "_" + Fragment_Case + "_" + Fragment_Method + ".weights.xml"
        weightFilePrefixList.append(FileToAdd)
    
    #create and return the TauDiscriminant Tool
    from PanTauAlgs.PanTauAlgsConf import PanTau__TauDiscriminantTool
    return PanTau__TauDiscriminantTool( name,
                                        WeightFileList = weightFilePrefixList,
                                        PtBins = PJB_Flags.MVA_PtBins(),
                                        TauFeatureNames = TauFeatureNames,
                                        ItsCase = currentCase,
                                        FeatureDefaultValues = featureDefaultValues,
                                        FeatureTypes = featureTypes,
                                        ReaderOption = "Silent:!Color",
                                        EtaMax = 2.5,
                                        MVA_Method  = PJB_Flags.MVA_Method())
    
#getDiscriminantTool ends




# end of file
