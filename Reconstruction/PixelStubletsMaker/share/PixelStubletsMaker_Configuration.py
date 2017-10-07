# File for feeding Reco_tf.py vertexing options for the searches 
# looking for displaced vertices in the SUSY and Exotics groups.
# The options here are needed both when running on RAW and ESD inputs.
# Must run after the large-radius tracking to use large-d0 tracks.

## get a handle on the top sequence of algorithms -import AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#----------------------------------
#
# Pixel-stublet maker algorithm.
#
#----------------------------------
streamESDidx = filter(lambda (i,x): x.find('AthenaOutputStream') != -1,
                      enumerate(topSequence.getSequence()))[0][0]

from InDetTrackPRD_Association.InDetTrackPRD_AssociationConf import InDet__InDetTrackPRD_Association
InputCombinedInDetTracksForTwoPluPVTracking = []
InputCombinedInDetTracksForTwoPluPVTracking += [InDetExtendedTracksKey]
InDetPRD_AssociationForPixelStublets = InDet__InDetTrackPRD_Association(name            = 'InDetPRD_AssociationInDetTwoPluPVTracking',
                                                               AssociationTool = InDetPrdAssociationTool,
                                                               TracksName      = InputCombinedInDetTracksForTwoPluPVTracking,
                                                               OutputLevel     = VERBOSE)
topSequence.insert(streamESDidx, InDetPRD_AssociationForPixelStublets)
print InDetPRD_AssociationForPixelStublets

from PixelStubletsMaker.PixelStubletsMakerConf import PixelStubletsMakerAlg
pixelStubletsMakerAlg = PixelStubletsMakerAlg(name                 = 'PixelStubletsMakerAlg',
                                              TrackParticleCreator = InDetxAODParticleCreatorTool,
                                              OutputContainerName  = "InDetTwoPlusPVTrackParticles",
                                              DEdxThreshold        = -999,
                                              ChisqThresholdPhi    = 20.0,
                                              ChisqThresholdEta    = 20.0,
                                              UseAssociationTool   = True,
                                              AssociationTool      = InDetPrdAssociationTool,
                                              OutputLevel          = INFO)
topSequence.insert(streamESDidx, pixelStubletsMakerAlg)
print pixelStubletsMakerAlg

include("PixelStubletsMaker/PixelStubletsMaker_postInclude.py")
