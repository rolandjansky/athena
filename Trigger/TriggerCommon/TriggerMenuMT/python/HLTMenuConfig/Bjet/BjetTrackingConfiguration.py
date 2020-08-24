# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import seqAND
#from AthenaCommon.Constants import DEBUG

def getSecondStageBjetTracking( inputRoI, dataObjects ):
    algSequence = []

    # Second stage of Fast tracking (for precision tracking preparation)
    from TrigInDetConfig.InDetSetup import makeInDetAlgs

    viewAlgs, viewVerify = makeInDetAlgs( whichSignature='Jet',separateTrackParticleCreator="Bjet", rois=inputRoI )

    viewVerify.DataObjects += dataObjects

    # Make sure the required objects are still available at whole-event level
    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    if not conddb.folderRequested( "PixelClustering/PixelClusNNCalib" ):
      topSequence.SGInputLoader.Load += [( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNN' ),
                                         ( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNNWithTrack' )]
      viewVerify.DataObjects += [( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNN' ),
                                 ( 'TTrainedNetworkCollection' , 'ConditionStore+PixelClusterNNWithTrack' )]

    from AthenaCommon.GlobalFlags import globalflags
    if not globalflags.InputFormat.is_bytestream():
      viewVerify.DataObjects += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]
      topSequence.SGInputLoader.Load += [( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' )]

    algSequence.append( seqAND("SecondStageFastTrackingSequence",viewAlgs) )

    # Precision Tracking
    from TrigInDetConfig.InDetPT import makeInDetPrecisionTracking
    PTTracks, PTTrackParticles, PTAlgs = makeInDetPrecisionTracking( "bjet", rois=inputRoI, inputFTFtracks="TrigFastTrackFinder_Tracks_Bjet" )
    algSequence.append( seqAND("PrecisionTrackingSequence",PTAlgs) )

    return [ algSequence, PTTrackParticles ]








