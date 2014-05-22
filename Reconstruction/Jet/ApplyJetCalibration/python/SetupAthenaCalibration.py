# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from JetRec.JetAlgConfiguration import checkAndUpdateOptions, editParm
from AthenaCommon.GlobalFlags  import globalflags

def doApplyJES(finder, mainParam, input, precedingCalibs = [], onlyOffset = True,**options):
    ''' Set-up standard EtaJES Correction '''
    
    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
    
    from ApplyJetCalibration.ApplyJetCalibrationConf import ApplyJetCalibrationAthena
    algtype = finder + str(int(mainParam*10))+input
    if algtype not in ["AntiKt4TopoEM", "AntiKt4LCTopo", "AntiKt6TopoEM", "AntiKt6LCTopo"]:
        algtype = "AntiKt4LCTopo" if 'LC' in input else "AntiKt4TopoEM"
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

    pass
