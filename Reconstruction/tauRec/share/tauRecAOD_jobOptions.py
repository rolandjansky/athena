################################################################################
##
#@file tauRecAOD_jobOptions.py
#
#@brief jobOption to setup parts of the tau reconstruction chain to run on AODs.
#
#@author Felix Friedrich <felix.friedrich@cern.ch>
################################################################################
from tauRec.TauRecAODBuilder import TauRecAODProcessor
TauRecAODProcessor(inAODmode=True)