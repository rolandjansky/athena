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

# Pi0 RoI making and topoclustering
_doPi0Clus = tauFlags.doPi0()

from tauRec.TauRecBuilder import TauRecCoreBuilder
TauRecCoreBuilder(doPi0Clus=_doPi0Clus, doTJVA=_doTJVA)

if _doPi0Clus:
    # run pi0 topoclustering using cells from TauCommonPi0Cells
    include("tauRec/Pi0ClusterMaker_jobOptions.py")

    # recreate pi0 clusters using cells from AllCalo
    from tauRec.tauRecConf import ClusterCellRelinkAlg 
    alg = ClusterCellRelinkAlg('ClusterCellRelinkAlg',
                               Cells = 'AllCalo',
                               ClustersInput = 'TauInitialPi0Clusters',
                               ClustersOutput = 'TauPi0Clusters',
                               CellLinksOutput = 'TauPi0Clusters_links')

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += alg

from tauRec.TauRecRunner import TauRecRunner
TauRecRunner()
