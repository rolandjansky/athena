################################################################################
##
#@file tauRec_config.py
#
#@brief Main RecExCommon entry point for the tau reconstruction chain including run of tau identification algorithms.
#
# This file calls the dedicated main jobOptions of tauRec and TauDiscriminant. 
# Checks make sure that detector conditions are valid to run tau reconstruction.
# Tau identification will only be run if tau reconstruction was setup correctly.
#
################################################################################
from RecExConfig.RecFlags import rec 
from tauRec.tauRecFlags import jobproperties
from DiTauRec.DiTauRecFlags import jobproperties
from JetRec.JetRecFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties


#switch off TauDiscriminant for cosmic
if jobproperties.Beam.beamType()=="cosmics":
    jobproperties.tauRecFlags.doRunTauDiscriminant=False

#disable tau in case we run on RDO/RAW and jets are disabled as well:
if jobproperties.tauRecFlags.doTauRec() and rec.readRDO() and not rec.doJetMissingETTag():
    from AthenaCommon.Logging import logging
    logTauRecConfig = logging.getLogger( 'tauRec_config' )
    logTauRecConfig.warning("Running on RDO/RAW files but doJetMissingETTag=False. Tau reco need jets --> tau reconstruction disabled!!")
    jobproperties.tauRecFlags.doTauRec=False


if jobproperties.tauRecFlags.doTauRec() and ( rec.readESD() or  ( DetFlags.haveRIO.ID_on() and DetFlags.haveRIO.Calo_on() ) ) :
    _tauFail=True

    # the main tau reconstruction part
    try:    
        include( "tauRec/tauRec_jobOptions.py" )
        _tauFail=False
    except Exception:
        treatException("Could not set up merged tauRec. Switched off !")
         
    if not _tauFail:  
        # commenting out for now, so we can reactivate possibly later
        # call eflowRec in tau mode now
        #if recAlgs.doEFlow():        
        #    try:
        #        include("eflowRec/eflowRec_config_DC14_Tau.py")
        #    except Exception:
        #        treatException("could not setup eflowRec")
        
        #jobproperties.tauRecFlags.doPanTau=False
        # call PanTau now
        # if jobproperties.tauRecFlags.doPanTau():
        #     try:
        #         include("PanTauAnalysis/JobOptions_Main_PanTau.py")
        #     except Exception:
        #         treatException("Could not setup PanTau")
        #         jobproperties.tauRecFlags.doPanTau = False
        
        # call TauDiscriminant
        # if jobproperties.tauRecFlags.doRunTauDiscriminant():
        #     try:
        #         include("TauDiscriminant/TauDiscri_jobOptions.py" )      
        #     except Exception:
        #         treatException("Could not set up TauDiscriminant. Switched off !")

        # call DiTauRec
        if jobproperties.DiTauRecFlags.doDiTauRec():
            try:
                include("DiTauRec/DiTauRec_config.py")
            except Exception:
                treatException("Could not set up DiTauRec. Switched off!")

    if _tauFail and jobproperties.tauRecFlags.doTauRec():
        jobproperties.tauRecFlags.doTauRec=False
        del _tauFail
else:
    if jobproperties.tauRecFlags.doTauRec():
        jobproperties.tauRecFlags.doTauRec=False

#XXX switch this off until xAOD migration is finished
#if rec.doWritexAOD():
#    from AthenaCommon.AlgSequence import AlgSequence
#    sequence = AlgSequence()
#    # Add the tau converter algorithm:
#    outkey = "TauJets"
#    from xAODTauCnv.xAODTauCnvConf import xAODMaker__TauJetCnvAlg
#    tauAlg = xAODMaker__TauJetCnvAlg()
#    #tauAlg.OutputLevel = 1
#    sequence += tauAlg
#    
