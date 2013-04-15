
# Only include this file once
include.block ("AODSelect/AODSelect_commonOptions.py")



# =============================================================================
# We need to schedule the impact parameter flagging in case the electron likelihood tool is scheduled (for Medium and higher)
# =============================================================================
if AODSelectFlags.doElectronLikelihoodIDFlagging_VeryTight():
    for inTypeAndKey in AODSelectFlags.doElectronLikelihoodIDFlagging_VeryTight.inputTypeAndKeyList :
        AODSelectFlags.doImpactParameterFlagging.add( inputItem = inTypeAndKey )
        pass
    pass
if AODSelectFlags.doElectronLikelihoodIDFlagging_Tight():
    for inTypeAndKey in AODSelectFlags.doElectronLikelihoodIDFlagging_Tight.inputTypeAndKeyList :
        AODSelectFlags.doImpactParameterFlagging.add( inputItem = inTypeAndKey )
        pass
    pass
if AODSelectFlags.doElectronLikelihoodIDFlagging_Medium():
    for inTypeAndKey in AODSelectFlags.doElectronLikelihoodIDFlagging_Medium.inputTypeAndKeyList :
        AODSelectFlags.doImpactParameterFlagging.add( inputItem = inTypeAndKey )
        pass
    pass




# =============================================================================
# We need to schedule the kt5 jet finding for the egamma calorimeter isolation correction
# =============================================================================
if AODSelectFlags.doEGammaCaloIsolationFlagging():
    AODSelectFlags.doRunJetFinding = True
    pass



# =============================================================================
# Schedule special jet finding for calculating the ambient energy density
# =============================================================================
if AODSelectFlags.doRunJetFinding():
    # Let's first see if the Kt5Jets already exist
    from RecExConfig.ObjKeyStore import objKeyStore
    hasKt5TopoJets = False
    if objKeyStore.isInInput("JetCollection", "Kt5TopoJets") \
           or objKeyStore['transient'].has_item("JetCollection#Kt5TopoJets"):
        hasKt5TopoJets = True
        pass
    if AODSelectFlags.doRunJetFinding.forceRerunJetFinding == True : hasKt5TopoJets = False

    if not hasKt5TopoJets :
        # Groan ... LowPtJetFinder clobbers jetFlags.inputFileType to AOD.
        # This causes failures building Kt5Truth jets when reading ESDs.
        # Try to work around...
        from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
        from RecExConfig.RecFlags import rec
        from JetRec.JetRecFlags import jetFlags
        if rec.readESD(): jetFlags.inputFileType = "ESD"
        
        from PhotonAnalysisUtils.LowPtJetFinder import LowPtJetFinder
        LowPtJetFinder ( AODSelectSeq )
        kt5topojets = AODSelectSeq.Kt5TopoJets
        
        # Work around jet area moment name screwups...
        finder = kt5topojets.AlgTools['JetFastJetFinder']
        # finder = kt5topojets.JetFastJetFinder
        if finder.properties().has_key ('AreaMomentName'):
            areaname = 'FJetArea'
            if hasattr (PAUflags, 'JetAreaMomentName'):
                areaname = PAUflags.JetAreaMomentName()
                pass
            finder.AreaMomentName = areaname
            pass
        pass
    pass



# =============================================================================
# Get or create all needed tools
# =============================================================================

# ----------------------------
# general selector flagging
# ----------------------------

# Get the selector tool for flagging 
if AODSelectFlags.doImpactParameterFlagging():
    if not hasattr(ToolSvc, "AODSelect_ImpactParameterCalcTool"):
        from UserDataUtils.UserDataUtilsConf import ImpactParameterCalcTool
        ToolSvc += ImpactParameterCalcTool("AODSelect_ImpactParameterCalcTool")
        pass
    configurationHelper.addUserDataTool( AODSelectFlags.doImpactParameterFlagging, ToolSvc.AODSelect_ImpactParameterCalcTool )
    pass


