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
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags

# use Tau Jet Vertex Association Tool
# each Tau candidate gets its own primary vertex
# and the tracks are selected accroding to this vertex
_doTJVA = True

# Pi0-finding algorithm
_doPi0Clus = taujp.tauRecFlags.doPi0() #False by default
_doPi0Clus = True 

# Switch to upgrade configs
if (CommonGeometryFlags.Run() == "RUN4"):
    # if taujp.tauRecFlags.doUpgrade():
    taujp.tauRecFlags.tauRecToolsCVMFSPath = "tauRecTools/00-03-00/"
    taujp.tauRecFlags.tauRecToolsLCCalibFile = "TES_calib_nVtxCorr_upgrade.root"
    taujp.tauRecFlags.tauRecToolsCombP4Weights = "CalibLoopResult_v04-04.root"
    taujp.tauRecFlags.tauRecToolsMvaTESWeights = "MvaTES.ITk19.v20190430.weights.root"
    taujp.tauRecFlags.tauRecJetBDTConfig = [ ["JetBDTScore1P.root",0,1], ["JetBDTScore3P.root",2,1000] ]
    taujp.tauRecFlags.tauRecMVATrackClassificationConfig = [ ["TMVAClassification_BDT_l1.weights.root", -0.0168], ["TMVAClassification_BDT_l2.weights.root", -0.0386], ["TMVAClassification_BDT_l3.weights.root", -0.0916] ]
    taujp.tauRecFlags.tauRecEleBDTConfig = [ ["EleBDT1PBar.root", 1, 1.37], ["EleBDT1PEnd1.root", 1, 1.37, 2.0], ["EleBDT1PEnd23.root", 1, 2.0, 4.0] ] # only changes maxAbsTrackEta for TauEleBDT_end23
    taujp.tauRecFlags.tauRecWPDecoratorJetBDTConfig = ["FlatJetBDT1P.root", "FlatJetBDT3P.root"]

# the TauCoreBuilder
from tauRec.TauRecBuilder import TauRecCoreBuilder
TauRecCoreBuilder(doPi0Clus=_doPi0Clus, doTJVA=_doTJVA)


#include("tauRec/Pi0ClusterMaker_Crakow_jobOptions.py")
if _doPi0Clus:
    include("tauRec/Pi0ClusterMaker_jobOptions.py")

from tauRec.TauRecBuilder import TauRecPi0EflowProcessor
TauRecPi0EflowProcessor(doPi0Clus=_doPi0Clus)

from tauRec.TauRecBuilder import TauRecVariablesProcessor
TauRecVariablesProcessor(doPi0Clus=_doPi0Clus)


