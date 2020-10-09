################################################################################
##
#@file tauRec_jobOptions.py
#
#@brief Main jobOption to setup tau reconstruction chain.
#
#@author Felix Friedrich <felix.friedrich@cern.ch>
################################################################################

from RecExConfig.RecFlags import rec
from tauRec.tauRecFlags import tauFlags

# use Tau Jet Vertex Association Tool
_doTJVA = tauFlags.doTJVA()

# Pi0 cell RoI making and topoclustering
_doPi0Clus = tauFlags.doPi0()

from tauRec.TauRecBuilder import TauRecCoreBuilder
TauRecCoreBuilder(doPi0Clus=_doPi0Clus, doTJVA=_doTJVA)

if _doPi0Clus:
    include("tauRec/Pi0ClusterMaker_jobOptions.py")

from tauRec.TauRecRunner import TauRecRunner
TauRecRunner()
