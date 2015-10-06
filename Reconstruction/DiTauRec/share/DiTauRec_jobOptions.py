################################################################################
##
#@file DiTauRec_jobOptions.py
#
#@brief Main jobOption to setup DiTau reconstruction chain.
#
#@author Justin Griffiths, David Kirchmeier
################################################################################

#TODO: everything needed here?
from RecExConfig.RecFlags import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags
import AthenaCommon.SystemOfUnits as Units
from DiTauRec.DiTauRecFlags import jobproperties as ditaujp

# the DiTauBuilder
from DiTauRec.DiTauBuilder import DiTauBuilder
DiTauBuilder(use_cells= (not rec.readAOD()))
