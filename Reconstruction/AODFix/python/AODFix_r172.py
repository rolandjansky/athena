# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
from AODFixFlags import aodfix

logAODFix_r172 = logging.getLogger( 'AODFix_r172' )
    
# inside RecExCommon_topOptions.py:
def AODFix_r172_preInclude():

    logAODFix_r172.info("Version %d applied"%aodfix.AODFixPreviousMinorVersion.get_Value())
    
    # run missing ET for AODFix versions v0 and v1
    if aodfix.AODFixPreviousMinorVersion.get_Value() < 2:
        from RecExConfig.RecAlgsFlags import recAlgs
        recAlgs.doMissingET.set_Value_and_Lock(True)

    # run egamma only for AODFix version v0
    if aodfix.AODFixPreviousMinorVersion.get_Value() < 1:
        from egammaRec.egammaRecFlags import jobproperties
        jobproperties.egammaRecFlags.doAODRender.set_Value_and_Lock(True)
    
    return

def AODFix_r172_postAtlfast():
    return

def AODFix_r172_postTrigger():
    return

def AODFix_r172_postSystemRec():

    logAODFix_r172.info("executing postSystemRec")
    # run muon and tau only for AODFix version v0
    if aodfix.AODFixPreviousMinorVersion.get_Value() < 1:
        
        from RecExConfig.InputFilePeeker import inputFileSummary
        if (not 'eventdata_itemsDic' in inputFileSummary)  or (not 'Analysis::MuonContainer' in inputFileSummary['eventdata_itemsDic']):
            logAODFix_r172.warning("No Muons in input file")
        else:
            from AthenaCommon.Include import include
            from AthenaCommon.AppMgr import ToolSvc
            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()
            include( "MuonIsolationTools/MuonIsolationTool_jobOptions.py" )
            from MuonIsolationTools.MuonIsolationToolsConf import MuonIsolationUpdateAlg
            muonIsolationUpdateAlg=MuonIsolationUpdateAlg()
            muonIsolationUpdateAlg.IsolationTool=ToolSvc.MuonIsolationTool
            muonIsolationUpdateAlg.InputParticleCollection=""
            muonIsolationUpdateAlg.OutputParticleCollection=""
            muonIsolationUpdateAlg.MuonContainers = inputFileSummary['eventdata_itemsDic']['Analysis::MuonContainer']
            topSequence+=muonIsolationUpdateAlg
            ToolSvc.MuonIsolationTool.UpdateParameters=True
            ToolSvc.MuonIsolationTool.FillCaloIsolation=False
            ToolSvc.MuonIsolationTool.FillTrackIsolation=True    
            from GaudiKernel.SystemOfUnits import mm
            ToolSvc.MuonIsolationTrackSelectorTool.IPz0Max=5.*mm
            ToolSvc.MuonIsolationTrackSelectorTool.z0Max=5.*mm
            pass
        
        from tauRec.TauRecAODBuilder import TauRecAODProcessor
        TauRecAODProcessor(inAODmode=True)
        from TauDiscriminant.TauDiscriGetter import TauDiscriGetter
        TauDiscriGetter()
    return

def AODFix_r172_postCombinedRec():
    return

def AODFix_r172_postHeavyIon():
    return

# inside SystemRec_config.py:
def AODFix_r172_postCaloRec():
    return

def AODFix_r172_postInDetRec():
    return

def AODFix_r172_postMuonRec():
    return

def AODFix_r172_postForwardRec():
    return

# inside CombinedRec_config.py:
def AODFix_r172_postEgammaRec():
    return

def AODFix_r172_postMuonCombinedRec():
    return

def AODFix_r172_postJetRec():
    return

def AODFix_r172_posttauRec():
    return

def AODFix_r172_postMissingETRec():
    return
