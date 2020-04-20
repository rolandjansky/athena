
from AthenaCommon.CFElements import seqAND
#from AthenaCommon.Constants import DEBUG

def getSecondStageBjetTracking( inputRoI ):
    algSequence = []

    # Second stage of Fast tracking (for precision tracking preparation)
    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs = makeInDetAlgs( whichSignature='Jet',separateTrackParticleCreator="_Bjet", rois=inputRoI )

    algSequence.append( seqAND("SecondStageFastTrackingSequence",viewAlgs) )

    # Precision Tracking
    from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking

    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "bjet", rois=inputRoI, inputFTFtracks="TrigFastTrackFinder_Tracks_Bjet" )
    algSequence += PTAlgs

    return [ algSequence, PTTracks, PTTrackParticles ]








