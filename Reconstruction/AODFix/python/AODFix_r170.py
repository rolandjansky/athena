# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging

logAODFix_r170 = logging.getLogger( 'AODFix_r170' )
    
# inside RecExCommon_topOptions.py:
def AODFix_r170_preInclude():
    return

def AODFix_r170_postAtlfast():
    return

def AODFix_r170_postTrigger():
    return

def AODFix_r170_postSystemRec():
    return

def AODFix_r170_postCombinedRec():
    return

def AODFix_r170_postHeavyIon():
    return

# inside SystemRec_config.py:
def AODFix_r170_postCaloRec():
    return

def AODFix_r170_postInDetRec():
    logAODFix_r170.info("executing postInDetRec")
    ToolSvc.InDetSummaryHelper.usePixel  = True
    from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixeldEdxAODFix
    ToolSvc.InDetSummaryHelper.OverwriteIDSummary=True
    InDetPixeldEdxAODFix=InDet__PixeldEdxAODFix(name="InDetPixeldEdxAODFix")
    InDetPixeldEdxAODFix.SummaryHelperTool=ToolSvc.InDetSummaryHelper
    InDetPixeldEdxAODFix.TrackParticleContainers=["TrackParticleCandidate","MuTagIMOTrackParticles","StacoTrackParticles","MuGirlRefittedTrackParticles","MuidCombTrackParticles","MuTagTrackParticles"]
    topSequence+=InDetPixeldEdxAODFix
    if not 'InDetPixelToTPIDTool' in dir():
        from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixelToTPIDTool
        InDetPixelToTPIDTool = InDet__PixelToTPIDTool(name = "InDetPixelToTPIDTool")
        ToolSvc+=InDetPixelToTPIDTool
    ToolSvc.InDetSummaryHelper.PixelToTPIDTool = InDetPixelToTPIDTool
    InDetPixelToTPIDTool.ReadFromCOOL=False
    if ( globalflags.DataSource == 'data' ):
        InDetPixelToTPIDTool.CalibrationFile="dtpar_cb.txt"
    else:
        InDetPixelToTPIDTool.CalibrationFile="mcpar_cb.txt"
        
    return

def AODFix_r170_postMuonRec():
    return

def AODFix_r170_postForwardRec():
    return

# inside CombinedRec_config.py:
def AODFix_r170_postEgammaRec():
    return

def AODFix_r170_postMuonCombinedRec():
    return

def AODFix_r170_postJetRec():
    return

def AODFix_r170_posttauRec():
    return

def AODFix_r170_postMissingETRec():
    return
