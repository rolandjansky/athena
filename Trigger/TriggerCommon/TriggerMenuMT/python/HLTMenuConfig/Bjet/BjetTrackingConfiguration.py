
from AthenaCommon.CFElements import parOR
#from AthenaCommon.Constants import DEBUG

def getFirstStageBjetTracking( jetName,vertexName ):
    return None
    # To be implemented on a later stage

def getSecondStageBjetTracking( inputRoI ):
    algSequence = []

    # Second stage of Fast tracking (for precision tracking preparation)
    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    viewAlgs = makeInDetAlgs( whichSignature='Jet',separateTrackParticleCreator="_bjet", rois=inputRoI )

    algSequence.append( parOR("SecondStageFastTrackingSequence",viewAlgs) )

    # Precision Tracking
    from TrigUpgradeTest.InDetPT import makeInDetPrecisionTracking
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "bjet", inputFTFtracks="TrigFastTrackFinder_Tracks_bjet" )
    algSequence += PTAlgs

    return [ algSequence, PTTracks, PTTrackParticles ]








