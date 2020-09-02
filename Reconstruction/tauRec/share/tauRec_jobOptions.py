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
from tauRec.tauRecFlags import tauFlags

# use Tau Jet Vertex Association Tool
_doTJVA = tauFlags.doTJVA()

# Pi0-finding algorithm
_doPi0Clus = tauFlags.doPi0()

from tauRec.TauRecBuilder import TauRecCoreBuilder
TauRecCoreBuilder(doPi0Clus=_doPi0Clus, doTJVA=_doTJVA)

if _doPi0Clus:
    include("tauRec/Pi0ClusterMaker_jobOptions.py")

from tauRec.TauRecRunner import TauRecRunner
TauRecRunner(doPi0Clus=_doPi0Clus, doTJVA=_doTJVA)
