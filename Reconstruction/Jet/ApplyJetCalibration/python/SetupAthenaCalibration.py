# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration






from AthenaCommon.GlobalFlags  import globalflags

def applyCalibrationTool(finder, mainParam, input, onlyOffset = False,**options):
    from ApplyJetCalibration.ApplyJetCalibrationConf import ApplyJetCalibrationAthena

    
    if input == "EMTopo" :
        input = "TopoEM" # calib constants follow the old conventions.
    # build the relevant name    
    algtype = finder + str(int(mainParam*10))+input
    toolName = "ApplyJetCalib"+algtype
    if onlyOffset:
      toolName += "Offset"
    else:
      toolName += "JES"
    if algtype not in ["AntiKt4TopoEM", "AntiKt4LCTopo", "AntiKt6TopoEM", "AntiKt6LCTopo"]:
        algtype = "AntiKt4LCTopo" if 'LC' in input else "AntiKt4EMTopo"

    mainParam = 0.4 if '4' in algtype else 0.6        

    # check if tool exists 
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc, toolName):
        return getattr(ToolSvc, toolName)


    # We need event density.
    # WARNING the config below is VERY preliminary
    # it won't work if the ApplyJetCalibrationAthena tool is not used in the main Alg sequence
    # it might reschedule several times the same density calc tool
    from EventShapeRec.EventEtDensityConfig import calculateSimpleEventDensity
    calculateSimpleEventDensity(Radius=mainParam, SignalState="CALIBRATED" if "LC" in input else "UNCALIBRATED")
    
    tool = ApplyJetCalibrationAthena(toolName,
                                     AlgoType = algtype,
                                     ConfigFile = "ApplyJetCalibration/CalibrationConfigs/JES_Full2012dataset_Preliminary_Jan13.config",
                                     IsData = globalflags.DataSource()=='data',
                                     OnlyOffsetSubstraction = onlyOffset,
                                     RhoKey = "ClusterRhoKt4" + ( "LC" if "LC" in input else "EM")
                                     )
    ToolSvc += tool
    return tool
    

try:
    from JetRec.JetAlgConfiguration import checkAndUpdateOptions, editParm
    from AthenaCommon.GlobalFlags  import globalflags

    def doApplyJES(finder, mainParam, input, precedingCalibs = [], onlyOffset = True,**options):
        ''' Set-up standard EtaJES Correction '''
        options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
        from ApplyJetCalibration.ApplyJetCalibrationConf import ApplyJetCalibrationAthena
        algtype = finder + str(int(mainParam*10))+input
        if algtype not in ["AntiKt4EMTopo", "AntiKt4LCTopo", "AntiKt6EMTopo", "AntiKt6LCTopo"]:
            algtype = "AntiKt4LCTopo" if 'LC' in input else "AntiKt4EMTopo"
        tool = ApplyJetCalibrationAthena(
            AlgoType = algtype,
            ConfigFile = "ApplyJetCalibration/CalibrationConfigs/JES_Full2012dataset_Preliminary_Jan13.config",
            IsData = globalflags.DataSource()=='data',
            OnlyOffsetSubstraction = onlyOffset,
            RhoKey = "ClusterRhoKt4" + ( "LC" if "LC" in input else "EM")
            )
        return tool

    from functools import partial

    from JetCalibTools.MakeCalibSequences import calibTags

    calibTags[ "ApplyAreaOffsetJES" ] = partial(doApplyJES, onlyOffset=False)
    calibTags[ "ApplyAreaOffset" ] = partial(doApplyJES, onlyOffset=True)

    def applyJESasJetMoment(jetcollname):
        from JetMomentTools.GhostAssociation import interpretJetName
        recognizedJet = interpretJetName(jetcollname)
        if recognizedJet is not None and momAlg is not None:
            finder, input, mainParam = recognizedJet


except:
    pass
