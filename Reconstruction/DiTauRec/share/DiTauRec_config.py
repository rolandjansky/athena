################################################################################
##
#@file DiTauRec_config.py
#
#@brief Main RecExCommon entry point for the DiTau reconstruction chain including run of DiTau identification algorithms.
#
# This file calls the dedicated main jobOptions of DiTauRec and TauDiscriminant. 
# Checks make sure that detector conditions are valid to run tau reconstruction.
# Tau identification will only be run if tau reconstruction was setup correctly.
#
################################################################################
from RecExConfig.RecFlags import rec 
from DiTauRec.DiTauRecFlags import jobproperties
from JetRec.JetRecFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.DetFlags import DetFlags

#disable tau in case we run on RDO/RAW and jets are disabled as well:
if jobproperties.DiTauRecFlags.doDiTauRec() and rec.readRDO() and not rec.doJetMissingETTag():
    from AthenaCommon.Logging import logging
    logDiTauRecConfig = logging.getLogger( 'DiTauRec_config' )
    logDiTauRecConfig.warning("Running on RDO/RAW files but doJetMissingETTag=False. DiTau reco need jets --> DiTau reconstruction disabled!!")
    jobproperties.DiTauRecFlags.doDiTauRec=False

    pass

if jobproperties.DiTauRecFlags.doDiTauRec() and ( rec.readESD() or  ( DetFlags.haveRIO.ID_on() and DetFlags.haveRIO.Calo_on() ) ) :
    _DiTauFail=True
    # the main tau reconstruction part
    try:    
        include( "DiTauRec/DiTauRec_jobOptions.py" )
        _DiTauFail=False
    except Exception:
        treatException("Could not set up merged DiTauRec. Switched off !")

else:
    if jobproperties.DiTauRecFlags.doDiTauRec():
        jobproperties.DiTauRecFlags.doDiTauRec=False



