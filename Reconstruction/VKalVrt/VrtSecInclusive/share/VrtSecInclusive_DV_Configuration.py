# File for feeding Reco_tf.py vertexing options for the searches 
# looking for displaced vertices in the SUSY and Exotics groups.
# The options here are needed both when running on RAW and ESD inputs.
# Must run after the large-radius tracking to use large-d0 tracks.

## get a handle on the top sequence of algorithms -import AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# instantiate the vertexing alg
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
topSequence.insert(-1, VrtSecInclusive())

# set options for vertexing
topSequence.VrtSecInclusive.ImpactWrtBL = True
topSequence.VrtSecInclusive.CutPixelHits = 0
topSequence.VrtSecInclusive.CutSctHits = 2
topSequence.VrtSecInclusive.TrkA0ErrCut = 200000
topSequence.VrtSecInclusive.TrkZErrCut = 200000
topSequence.VrtSecInclusive.a0TrkPVDstMinCut=2.0  # track d0 min
topSequence.VrtSecInclusive.a0TrkPVDstMaxCut=300.0 # track d0 max: default is 1000.0
topSequence.VrtSecInclusive.zTrkPVDstMinCut=0.0    # track z0 min: default is 0.0, just for clarification
topSequence.VrtSecInclusive.zTrkPVDstMaxCut=1500.0 # track z0 max: default is 1000.0
topSequence.VrtSecInclusive.TrkPtCut = 1000
topSequence.VrtSecInclusive.SelVrtChi2Cut=5
topSequence.VrtSecInclusive.CutSharedHits=2
topSequence.VrtSecInclusive.TrkChi2Cut=50
topSequence.VrtSecInclusive.TruthTrkLen=1
topSequence.VrtSecInclusive.SelTrkMaxCutoff=300
topSequence.VrtSecInclusive.DoSAloneTRT=False
topSequence.VrtSecInclusive.DoTruth = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
topSequence.VrtSecInclusive.RemoveFake2TrkVrt = True
topSequence.VrtSecInclusive.doTRTPixCut=True
topSequence.VrtSecInclusive.doMergeFinalVerticesDistance=True
topSequence.VrtSecInclusive.VertexMergeFinalDistCut=1.0
topSequence.VrtSecInclusive.VertexMergeFinalDistScaling=0.00
topSequence.VrtSecInclusive.SelTrkMaxCutoff = 300

# set options related to the vertex fitter
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
                                              Extrapolator        = ToolSvc.AtlasExtrapolator,
                                              IterationNumber     = 30,
                                              AtlasMagFieldSvc    = "AtlasFieldSvc"
                                             )
ToolSvc +=  InclusiveVxFitterTool;
InclusiveVxFitterTool.OutputLevel = INFO
topSequence.VrtSecInclusive.VertexFitterTool=InclusiveVxFitterTool
topSequence.VrtSecInclusive.Extrapolator = ToolSvc.AtlasExtrapolator

# tell VrtSecInclusive the interface name for Trk::IVertexMapper
from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__VertexMapper
HadronicVertexMapper = Trk__VertexMapper("HadronicVertexMapper")
ToolSvc += HadronicVertexMapper
topSequence.VrtSecInclusive.VertexMapper = HadronicVertexMapper

include("VrtSecInclusive/VrtSecInclusive_DV_postInclude.py")
