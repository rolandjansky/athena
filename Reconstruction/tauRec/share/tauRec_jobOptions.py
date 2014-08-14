################################################################################
##
#@file tauRec_jobOptions.py
#
#@brief Main jobOption to setup tau reconstruction chain.
#
#@author Felix Friedrich <felix.friedrich@cern.ch>
################################################################################

#TODO: everything needed here?
from RecExConfig.RecFlags import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.GlobalFlags import globalflags
import AthenaCommon.SystemOfUnits as Units
from tauRec.tauRecFlags import jobproperties as taujp

# use Tau Jet Vertex Association Tool
# each Tau candidate gets its own primary vertex
# and the tracks are selected accroding to this vertex
_doTJVA = True

# Bonn Pi0-finding algorithm
_doBonnPi0Clus = taujp.tauRecFlags.doBonnPi0() #False by default
_doBonnPi0Clus = True 

# the TauCoreBuilder
from tauRec.TauRecBuilder import TauRecCoreBuilder
TauRecCoreBuilder(doBonnPi0Clus=_doBonnPi0Clus, doTJVA=_doTJVA)


#include("tauRec/Pi0ClusterMaker_Crakow_jobOptions.py")
if _doBonnPi0Clus:
    include("tauRec/Pi0ClusterMaker_Bonn_jobOptions.py")

from tauRec.TauRecBuilder import TauRecPi0EflowProcessor
TauRecPi0EflowProcessor(doBonnPi0Clus=_doBonnPi0Clus)

from tauRec.TauRecBuilder import TauRecVariablesProcessor
TauRecVariablesProcessor(doBonnPi0Clus=_doBonnPi0Clus)


