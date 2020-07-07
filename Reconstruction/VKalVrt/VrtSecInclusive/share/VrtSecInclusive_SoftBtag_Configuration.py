###  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# File for feeding Reco_tf.py vertexing options for the searches 
# looking for low pt b-hadrons in the SUSY group.

## get a handle on the top sequence of algorithms -import AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from VrtSecInclusive.SoftBtagTrackSelector import SoftBtagTrackSelector
SoftBtagTrackSelector = SoftBtagTrackSelector()
# SoftBtagTrackSelector.FillHist             = True
SoftBtagTrackSelector.Jetcontainer         = "AntiKt4EMTopoJets"
SoftBtagTrackSelector.InputTrackContainer  = "InDetTrackParticles"
SoftBtagTrackSelector.OutputTrackContainer = "SoftBtagSeedTrackParticles"


topSequence.insert(-1, SoftBtagTrackSelector)


from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
VrtSecInclusive_SoftB = VrtSecInclusive("VrtSecInclusive_SoftB")

topSequence.insert(-1, VrtSecInclusive_SoftB)


VrtSecInclusive_SoftB.TrackLocation                          = "SoftBtagSeedTrackParticles"
VrtSecInclusive_SoftB.SecondaryVerticesContainerName         = "SoftBtagCandidateVertices"
VrtSecInclusive_SoftB.TruthParticleFilter                    = "Bhadron"
VrtSecInclusive_SoftB.DoTwoTrSoftBtag                        = True
VrtSecInclusive_SoftB.TwoTrVrtAngleCut                       = 0.7
VrtSecInclusive_SoftB.twoTrVrtMinDistFromPV                  = 0.2
VrtSecInclusive_SoftB.PassThroughTrackSelection              = True
VrtSecInclusive_SoftB.ImpactWrtBL                            = False
VrtSecInclusive_SoftB.DoPVcompatibility                      = True
VrtSecInclusive_SoftB.RemoveFake2TrkVrt                      = True
VrtSecInclusive_SoftB.doReassembleVertices                   = False
VrtSecInclusive_SoftB.doMergeByShuffling                     = False
VrtSecInclusive_SoftB.doMergeFinalVerticesDistance           = False
VrtSecInclusive_SoftB.doAssociateNonSelectedTracks           = False
VrtSecInclusive_SoftB.doFinalImproveChi2                     = False
VrtSecInclusive_SoftB.DoTruth                                = False
VrtSecInclusive_SoftB.SelVrtChi2Cut                          = 5.
VrtSecInclusive_SoftB.SelTrkMaxCutoff                        = 30
VrtSecInclusive_SoftB.twoTrkVtxFormingD0Cut                  = 0.

# VrtSecInclusive_SoftB.FillHist                               = True
VrtSecInclusive_SoftB.FillIntermediateVertices               = True

# set options related to the vertex fitter
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
                                              Extrapolator        = ToolSvc.AtlasExtrapolator,
                                              IterationNumber     = 30,
                                              AtlasMagFieldSvc    = "AtlasFieldSvc"
                                             )
ToolSvc +=  InclusiveVxFitterTool;
# InclusiveVxFitterTool.OutputLevel = DEBUG

VrtSecInclusive_SoftB.VertexFitterTool=InclusiveVxFitterTool
VrtSecInclusive_SoftB.Extrapolator = ToolSvc.AtlasExtrapolator



MSMgr.GetStream("StreamDAOD_SUSY15").AddItem( [ 'xAOD::VertexContainer#VrtSecInclusive*',
                                                'xAOD::VertexAuxContainer#VrtSecInclusive*',
                                                'xAOD::TrackParticleContainer#InDetTrackParticles*',
                                                'xAOD::TrackParticleAuxContainer#InDetTrackParticles*',
                                                ])

print "List of items for the DAOD output stream:"
print MSMgr.GetStream("StreamDAOD_SUSY15").GetItems()


# Monitoring histograms writing out
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='HIST.VrtSecInclusive.root' OPT='RECREATE'"]

from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
topSequence.AANTupleStream.ExtraRefNames = [ "StreamDAOD_SUSY15" ]
topSequence.AANTupleStream.OutputName = 'HIST.VrtSecInclusive.root'
topSequence.AANTupleStream.WriteInputDataHeader = True
topSequence.AANTupleStream.OutputLevel = INFO
