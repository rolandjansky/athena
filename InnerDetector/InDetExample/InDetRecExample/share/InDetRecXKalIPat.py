# ------------------------------------------------------------
#
# ----------- now we do legacy pattern if requested
#
# ------------------------------------------------------------
#
# ----------- iPat  
#
if InDetFlags.doiPatRec():
   include ( "iPatRecExample/iPatRec_jobOptions.py" )
   iPatRec.maxSecondaryImpact  = InDetNewTrackingCuts.maxSecondaryImpact()
   iPatRec.minPt               = InDetNewTrackingCuts.minPT()
   #
   # ---- handle converted iPatRec tracks
   #
   if InDetFlags.doTruth():
      #
      # set up the truth info for this container
      #
      include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
      InDetIPatRecTruth = ConfiguredInDetTrackTruth(InDetKeys.IPatConvertedTracks(),
                                                    InDetKeys.IPatConvertedTracksDetailedTruth(),
                                                    InDetKeys.IPatConvertedTracksTruth())
      #
      # add to keys lists for statistics
      #
      TrackCollectionKeys      += [ InDetIPatRecTruth.Tracks() ]
      TrackCollectionTruthKeys += [ InDetIPatRecTruth.TracksTruth() ]
   else:
      TrackCollectionKeys      += [ InDetKeys.IPatConvertedTracks() ]
       
#
# ----------- xKalman
#
if InDetFlags.doxKalman():

   #
   # xKlaman magnetic field service
   #
   from xKalman.xKalmanConf import XKaMField
   XKaMField = XKaMField(name                 = 'XKaMField'        ,
                         MagneticFieldService = 4                  ,
                         MagneticField        = [0.,0.,-14.,700.]  )
                          
   topSequence += XKaMField
   if (InDetFlags.doPrintConfigurables()):
     print XKaMField

   #
   # xKalman geomodel  service
   #
   from xKalman.xKalmanConf import  XKaDetectorBuilder
   XKaDetectorBuilder = XKaDetectorBuilder(name                   = 'XKaDetectorBuilder' ,
                                           SilDescrManageLocation = 'SCT'                ,
                                           PixDescrManageLocation = 'Pixel'              ,
                                           TrtDescrManageLocation = "TRT"                ,
                                           PixelClustesContainer  = InDetKeys.PixelClusters(),
                                           SCT_ClustesContainer   = InDetKeys.SCT_Clusters(),
                                           TRT_ClustesContainer   = InDetKeys.TRT_DriftCircles(),
                                           TruthNamePixel         = InDetKeys.PixelClustersTruth(), 
                                           TruthNameSCT           = InDetKeys.SCT_ClustersTruth(),
                                           TruthNameTRT           = InDetKeys.TRT_DriftCirclesTruth(),
                                           DetectorLocation       = "xKalman"            ,
                                           Pixel                  = DetFlags.haveRIO.pixel_on(),
                                           SCT                    = DetFlags.haveRIO.SCT_on(),
                                           TRT                    = DetFlags.haveRIO.TRT_on(),
                                           TruthInformation       = InDetFlags.doTruth() ,
                                           PrintLevelDetector     = 3                    )

   topSequence += XKaDetectorBuilder
   if (InDetFlags.doPrintConfigurables()):
     print XKaDetectorBuilder

   #
   # xKalman seeds service
   #
   from xKalman.xKalmanConf import XKaSeedsManager
   XKaSeedsManager = XKaSeedsManager(name           = 'XKaSeedsManager',
                                     RoadsLocation  = 'XKEvent'        )
                          
   topSequence += XKaSeedsManager
   if (InDetFlags.doPrintConfigurables()):
     print XKaSeedsManager

   #
   # xKalman seed for full event reconstruction
   #
   from xKalman.xKalmanConf import XKaSeedALL
   XKaSeedALL =  XKaSeedALL(name           = 'XKaSeedALL',
                            RoadsLocation  = 'XKEvent'   ,
                            RoadParameters = [InDetNewTrackingCuts.minPT(),3.2,2.6,240.,InDetNewTrackingCuts.maxPrimaryImpact()])
   topSequence += XKaSeedALL
   if (InDetFlags.doPrintConfigurables()):
     print XKaSeedALL

   #
   # xKalman reconstruction menager
   #
   from xKalman.xKalmanConf import XKalMan
   XKalMan =  XKalMan(name                         = 'XKalMan'      ,
                      PrintLevelInformation        = 3              ,
                      ReconstructionStrategy       = 4              ,
                      Selectivity                  = 2              ,
                      PrimaryVertexSearch          = False          ,
                      SimpleTracksProduction       = True           ,
                      PrintMemoryManager           = False          ,
                      TrackModel                   = 0              ,
                      MinNumberOfSilClusters       = 7              ,
                      MinNumberOfUniqueSilClusters = 5              ,
                      MinNumberOfTRTClusters       = 9              ,
                      MaxNumberHoles               = 22             ,
                      Xi2forPrecisionClusters      = 15.            ,
                      TRTClusterRatio              = .5             ,
                      TRTTimeStrawRatio            = .5             ,
                      TRTuseDriftTime              = True           ,
                      DetectorLocation             ='xKalman'       ,
                      RoadsLocation                ='XKEvent'       ,
                      ConditionLocation            ='xKCondition'   ,
                      TracksLocation               ='xKalman'       ,
                      VerticesLocation             ='xKalman'       ,
                      SimpleTracksLocation         ='Tracks_xKalman',
                      BeamConditions               ='BeamCondSvc'   ,
                      ExtraInformation             = 2              )
   topSequence += XKalMan
   if (InDetFlags.doPrintConfigurables()):
     print XKalMan

   #
   # xKalman clusters production
   #
   from xKalman.xKalmanConf import XKaClustersProduction
   XKaClustersProduction =  XKaClustersProduction(name                  = 'XKaClustersProduction',
                                                  DetectorLocation      = 'xKalman'              ,
                                                  ConditionLocation     = 'xKCondition'          , 
                                                  TracksLocation        = 'xKalman'              ,
                                                  PrintLevelClusters = -1                        ,
                                                  PrintLevelDetector = -1                        )
   topSequence += XKaClustersProduction
   if (InDetFlags.doPrintConfigurables()):
     print XKaClustersProduction

   #
   # xKalman space points production
   #
   from xKalman.xKalmanConf import XKaSpacePointsProduction
   XKaSpacePointsProduction = XKaSpacePointsProduction(name               = 'XKaSpacePointsProduction',
                                                       ConditionLocation  = 'xKCondition'             , 
                                                       TracksLocation     = 'xKalman'                 ,
                                                       PrintLevelSpoints  = -1                        ,
                                                       PrintLevelDetector = -1                        )
   topSequence += XKaSpacePointsProduction
   if (InDetFlags.doPrintConfigurables()):
     print XKaSpacePointsProduction

   #
   # xKalman reconstruction started from space points seeds
   #
   from xKalman.xKalmanConf import XKaSILReconstruction
   XKaSILReconstruction = XKaSILReconstruction (name              = 'XKaSILReconstruction',
                                                ConditionLocation = 'xKCondition'         ,  
                                                TracksLocation    = 'xKalman'             ,
                                                PrintLevelTracks  = [-1,1]                )
   topSequence += XKaSILReconstruction
   if (InDetFlags.doPrintConfigurables()):
     print XKaSILReconstruction

   #
   # xKalman reconstruction using only TRT information
   #
   from xKalman.xKalmanConf import XKaTRTReconstruction
   XKaTRTReconstruction = XKaTRTReconstruction (name              = 'XKaTRTReconstruction',
                                                ConditionLocation = 'xKCondition'         ,  
                                                TracksLocation    = 'xKalman'             ,
                                                PrintLevelTracks  = [-1,1]                )
   topSequence += XKaTRTReconstruction
   if (InDetFlags.doPrintConfigurables()):
     print XKaTRTReconstruction

   #
   # xKalman ambiguity solving
   #
   from xKalman.xKalmanConf import XKaTracksComparison
   XKaTracksComparison = XKaTracksComparison (name              = 'XKaTracksComparison',
                                              ConditionLocation = 'xKCondition'        ,  
                                              TracksLocation    = 'xKalman'            ,
                                              PrintLevelTracks  = [-1,1]               )
   topSequence += XKaTracksComparison
   if (InDetFlags.doPrintConfigurables()):
     print XKaTracksComparison

   #
   # xKalman track extension to TRT
   #
   from xKalman.xKalmanConf import XKaTrackTRTExtension
   XKaTrackTRTExtension = XKaTrackTRTExtension(name              = 'XKaTrackTRTExtension',
                                               ConditionLocation = 'xKCondition'         ,  
                                               TracksLocation    = 'xKalman'             ,
                                               PrintLevelTracks  = [-1,1]                )
   topSequence += XKaTrackTRTExtension
   if (InDetFlags.doPrintConfigurables()):
     print XKaTrackTRTExtension
   #
   # xKalman simple tracks production
   #
   from xKalman.xKalmanConf import XKaSimpleTracksProduction
   XKaSimpleTracksProduction =XKaSimpleTracksProduction(name              = 'XKaSimpleTracksProduction',
                                                        ConditionLocation = 'xKCondition'              ,  
                                                        TracksLocation    = 'xKalman'                  ,
                                                        TrackUnits        =  1                         ,
                                                        PrimaryVertex     = False                      ,
                                                        PrintLevelTracks  = -1                         ,
                                                        ConvertToTracks   = True                       ,
                                                        RIOonTrackTool    = InDetRotCreator            )
   topSequence += XKaSimpleTracksProduction        
   if (InDetFlags.doPrintConfigurables()):
     print XKaSimpleTracksProduction

   if InDetFlags.doTruth():
      #
      # set up the truth info for this container
      #
      include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
      InDetXKalmanTruth = ConfiguredInDetTrackTruth(InDetKeys.XKalConvertedTracks(),
                                                    InDetKeys.XKalConvertedTracksDetailedTruth(),
                                                    InDetKeys.XKalConvertedTracksTruth())
      #
      # add to keys lists for statistics
      #
      TrackCollectionKeys      += [ InDetXKalmanTruth.Tracks() ]
      TrackCollectionTruthKeys += [ InDetXKalmanTruth.TracksTruth() ]
   else:
      TrackCollectionKeys      += [ InDetKeys.XKalConvertedTracks() ]
