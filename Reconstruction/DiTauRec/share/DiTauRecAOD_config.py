################################################################################
##
#@file DiTauRecAOD_config.py
#
#@brief Main RecExCommon entry point for the DiTau reconstruction to run on AOD
#
# This file calls the dedicated main jobOptions of DiTauRec. 
# Checks make sure that detector conditions are valid to run tau reconstruction.
#
################################################################################
from RecExConfig.RecFlags import rec 
from DiTauRec.DiTauRecFlags import jobproperties
from JetRec.JetRecFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties
import traceback

if rec.readAOD() :
    _DiTauFail=True

    # the main ditau reconstruction part
    try:    
        include( "DiTauRec/DiTauRec_jobOptions.py" )
        _DiTauFail=False
    except Exception:
        print traceback.format_exc()
        treatException("Could not set up merged DiTauRec. Switched off !")

else:
    if jobproperties.DiTauRecFlags.doDiTauRec():
        jobproperties.DiTauRecFlags.doDiTauRec=False























