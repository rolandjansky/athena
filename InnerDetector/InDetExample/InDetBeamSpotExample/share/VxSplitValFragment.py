if not 'outputfileprefix' in jobConfig:                   jobConfig['outputfileprefix'] = ''
if not 'histfile' in jobConfig:                           jobConfig['histfile'] =jobConfig['outputfileprefix']+'split.nt.root'

#-------------------------------------------------------------
# vertexing fragment for running split vertices
#------------------------------------------------------------

from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
InDetPriVxFinderToolSplit = InDet__InDetIterativePriVxFinderTool( name   = "InDetIterativePriVxFinderToolSplit",
                                                                  VertexFitterTool = InDetVxFitterTool,
                                                                  TrackSelector    = InDetTrackSelectorTool,
                                                                  SeedFinder = InDetVtxSeedFinder,
                                                                  ImpactPoint3dEstimator = InDetImpactPoint3dEstimator,
                                                                  LinearizedTrackFactory = InDetLinFactory,
                                                                  useBeamConstraint = False,
                                                                  significanceCutSeeding = 12,
                                                                  maximumChi2cutForSeeding = 49,
                                                                  maxVertices = 25,
                                                                  createSplitVertices = True)


ToolSvc += InDetPriVxFinderToolSplit
if (InDetFlags.doPrintConfigurables()):
    print InDetPriVxFinderToolSplit

InDetPriVxFinder.VertexFinderTool       = InDetPriVxFinderToolSplit
InDetPriVxFinder.VxCandidatesOutputName = "SplitVxCandidates"
InDetPriVxFinder.doVertexSorting        = False
if (InDetFlags.doPrintConfigurables()):
    print InDetPriVxFinder

from InDetVertexSplitter.InDetVertexSplitterConf import InDet__InDetVertexSplitterHist
InDetVertexSplitterHist = InDet__InDetVertexSplitterHist()

InDetVertexSplitterHist.OutputLevel = jobConfig['outputlevel']

topSequence += InDetVertexSplitterHist
print          InDetVertexSplitterHist

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='"+jobConfig['histfile']+"' OPT='RECREATE'"] 




