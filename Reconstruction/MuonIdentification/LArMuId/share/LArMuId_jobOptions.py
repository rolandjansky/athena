from RecExConfig.RecFlags import rec
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# make ImpactInCaloInDet for LArMuID

#Configure the extrapolator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator(name = 'trackToCaloExtrapolator')
theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic    calculation of eloss in calorimeters

ToolSvc+=theAtlasExtrapolator

# add tool ExtrapolateTrackToCalo
from TrackToCalo.TrackToCaloConf import ExtrapolateToCaloTool
exToCalo = ExtrapolateToCaloTool(name         = "exToCalo",
                                     Extrapolator = theAtlasExtrapolator)
ToolSvc+=exToCalo

from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
trackSummaryTool=AtlasTrackSummaryTool()
ToolSvc+=trackSummaryTool 
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetCosmicTrackSelectorTool
trackSelTool = InDet__InDetCosmicTrackSelectorTool("TrackToCaloSelection",
                                                   numberOfSiliconHits=1,
                                                   maxZ0=99999.,
                                                   maxD0=99999.,
                                                   TrackSummaryTool=trackSummaryTool )
ToolSvc+=trackSelTool

from TrackToCalo.TrackToCaloConf import ExtrapolateToCaloTool
svcMgr.ToolSvc+=ExtrapolateToCaloTool()
    
from TrackToCalo.TrackToCaloConf import ImpactInCaloAlg        
impactInCaloAlg = ImpactInCaloAlg("ImpactInCaloAlgInDet",
                                  ExtrapolateToCaloTool = exToCalo,
                                  TrackInputType = "TrackParticleCandidates",
                                  TrackParticleName = "TrackParticleCandidate",
                                  TrackSelectorTool = trackSelTool)

topSequence+=impactInCaloAlg
impactInCaloAlg.ClusterContainerName = ""
impactInCaloAlg.CaloCellContainerName = "AllCalo"
impactInCaloAlg.ImpactInCaloContainerName = "ImpactInCalo"
impactInCaloAlg.ExtrapolateInBothDirections=True

from LArMuId.LArMuIdConf import LArMuIdAlgorithm
topSequence += LArMuIdAlgorithm("LArMuIdAlgorithm")
larMuId = LArMuIdAlgorithm("LArMuIdAlgorithm")
# larMuId.ClustersOutputName="LArMuClusterCandidates"
larMuId.CellsNames= "AllCalo"
larMuId.ImpactNames="ImpactInCalo"
larMuId.Window = 0.1
larMuId.Threshold_High = 100.
larMuId.Threshold_Low = 50.
larMuId.SelectDetector = 3 # 1 for barrel only, 2 endcap, 3 both        
larMuId.StartFromTracks = True
    
# create thinned if there is raw data
if rec.readRDO:
    from LArMuId.LArMuIdConf import LArDigitThinnerFromIIC
    thinner = LArDigitThinnerFromIIC()
    topSequence+=thinner
    
