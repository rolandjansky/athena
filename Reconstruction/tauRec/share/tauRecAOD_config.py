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
from JetRec.JetRecFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties
import traceback

#switch off TauDiscriminant for cosmic
if jobproperties.Beam.beamType()=="cosmics":
    jobproperties.tauRecFlags.doRunTauDiscriminant=False

# #disable tau in case we run on RDO/RAW and jets are disabled as well:
# if jobproperties.tauRecFlags.doTauRec() and rec.readRDO() and not rec.doJetMissingETTag():
#     from AthenaCommon.Logging import logging
#     logTauRecConfig = logging.getLogger( 'tauRecAOD_config' )
#     logTauRecConfig.warning("Running on RDO/RAW files but doJetMissingETTag=False. Tau reco need jets --> tau reconstruction disabled!!")
#     jobproperties.tauRecFlags.doTauRec=False


if rec.readAOD() :
    _tauFail=True

    # the main tau reconstruction part
    try:    
        include( "tauRec/tauRecAOD_jobOptions.py" )
        _tauFail=False
    except Exception:
        print traceback.format_exc()        
        treatException("Could not set up merged tauRec. Switched off !")
         
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
