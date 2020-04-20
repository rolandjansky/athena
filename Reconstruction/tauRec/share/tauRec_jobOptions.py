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

# Pi0-finding algorithm
_doPi0Clus = taujp.tauRecFlags.doPi0() #False by default
_doPi0Clus = True 

# Change jet seed (default AntiKt4LCTopoJets)
# taujp.tauRecFlags.tauRecSeedJetCollection.set_Value_and_Lock("AntiKt4LCTopoJets")

# the TauCoreBuilder
from tauRec.TauRecBuilder import TauRecCoreBuilder
TauRecCoreBuilder(doPi0Clus=_doPi0Clus, doTJVA=_doTJVA)

#include("tauRec/Pi0ClusterMaker_Crakow_jobOptions.py")
if _doPi0Clus:
    include("tauRec/Pi0ClusterMaker_jobOptions.py")

from tauRec.TauRecRunner import TauRecRunner
TauRecRunner(doPi0Clus=_doPi0Clus, doTJVA=_doTJVA)


