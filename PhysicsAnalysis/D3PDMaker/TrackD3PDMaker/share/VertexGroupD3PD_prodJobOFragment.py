#
## @file TrackD3PDMaker/share/VertexGroupD3PD_prodjobOFragment.py
## @brief Setup D3PD Maker algorithm for vertex group D3PD when running from Reco_trf.py
## @author Simone Pagan Griso
## @date Mar, 2012
##
## Notes:
## - To include non beam-constrained and split vertices, use as preExec:
##   InDetFlags.doVertexFindingForMonitoring.set_Value_and_Lock(True)
##   InDetFlags.doSplitVertexFindingForMonitoring.set_Value_and_Lock(True)
##   or use as preInclude TrackD3PDMaker/VertexGroupD3PD_configure.py
##   and make sure that VertexD3PDAnalysisFlags.useAllVertexCollections = True
##   (it is by default; if the algorithms are not run, it does not dump info)

"""
Setup Vertex Group D3PD Maker algorithm  when running from Reco_trf.py
"""

from AthenaCommon.JobProperties import jobproperties
vtxprodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

from TrackD3PDMaker.VertexD3PDAnalysisKeys import VertexD3PDAnalysisKeys
from TrackD3PDMaker.VertexD3PDAnalysisFlags import VertexD3PDAnalysisFlags

### Configure Vertex D3PD Maker
VertexD3PDAnalysisFlags.useAllVertexCollections = True
VertexD3PDAnalysisFlags.useEventInfo=True
VertexD3PDAnalysisFlags.useTracks=True
VertexD3PDAnalysisFlags.useBeamspot=True
VertexD3PDAnalysisFlags.useBackgroundWord=True
VertexD3PDAnalysisFlags.useTrigger=True
VertexD3PDAnalysisFlags.useSecondaryVertex=False
VertexD3PDAnalysisFlags.useMET=True
VertexD3PDAnalysisFlags.useElectrons=True
VertexD3PDAnalysisFlags.useMuons=True
VertexD3PDAnalysisFlags.usePhotons=False
VertexD3PDAnalysisFlags.useJets=True
VertexD3PDAnalysisFlags.useTaus=False

include("TrackD3PDMaker/VertexGroupD3PD_loadConfig.py")

### Semi-automatic configuration steps
## The following needs to be given as preExec
##  (it has to be set before including RecExCommon)
#if VertexD3PDAnalysisFlags.useAllVertexCollections:
#    #Need to enable algorithms in IDRE
#    from InDetRecExample.InDetJobProperties import InDetFlags
#    InDetFlags.doVertexFindingForMonitoring.set_Value_and_Lock(True)
#    InDetFlags.doSplitVertexFindingForMonitoring.set_Value_and_Lock(True)

### Setup algorithm
if vtxprodFlags.WriteIDVTXD3PD.isVirtual:
    raise NameError( "IDVTX set to be a virtual stream" )
    pass 

streamName = vtxprodFlags.WriteIDVTXD3PD.StreamName
fileName   = buildFileName( vtxprodFlags.WriteIDVTXD3PD )

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
VertexGroupD3PDMaker = MSMgr.NewRootStream(vtxprodFlags.WriteIDVTXD3PD.StreamName, fileName, VertexD3PDAnalysisKeys.D3PDTreeName())
	
### Add objects to the algorithm
from TrackD3PDMaker.VertexGroupD3PD import VertexGroupD3PD
VertexGroupD3PD(VertexGroupD3PDMaker, **(VertexD3PDAnalysisFlags.D3PDMakerExtraDict()))
