
from AthenaCommon.CFElements import parOR
#from AthenaCommon.Constants import DEBUG

def getSecondStageBjetTracking( inputRoI ):
    algSequence = []

    # Second stage of Fast tracking (for precision tracking preparation)
    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs = makeInDetAlgs( whichSignature='Jet',separateTrackParticleCreator="_Bjet", rois=inputRoI )

    algSequence.append( parOR("SecondStageFastTrackingSequence",viewAlgs) )

    # Precision Tracking
    from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking

    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "bjet", inputFTFtracks="TrigFastTrackFinder_Tracks_Bjet" )
    algSequence += PTAlgs

    return [ algSequence, PTTracks, PTTrackParticles ]








