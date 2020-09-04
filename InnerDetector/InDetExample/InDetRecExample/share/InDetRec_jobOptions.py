
# +++++++++++++++++++ beginning of InDetRec_jobOptions.py
# jobOptions Fragment for ID software
# -----------------------------------
# edward.moyse@cern.ch
# markus.elsing@cern.ch
# -----------------------------------

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.BeamFlags import jobproperties
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags


# check (for robustness) if ID is on at all!
if not DetFlags.detdescr.ID_on():
  printfunc ("InDetRec_jobOptions.py: DetFlags ID is turned off. Not including anything!")
else:
  # +++++++++++++++++++++++
  # Control
  # +++++++++++++++++++++++
  #
  from InDetRecExample.InDetJobProperties import InDetFlags
  InDetFlags.init()
  #
  if InDetFlags.Enabled():

    from AthenaCommon.GlobalFlags import globalflags
    # rec flags are needed (e.g. for commissioning steering ...)
    from RecExConfig.RecFlags import rec

    # --- ensure that Calo clustering is running if we run in calo seeded mode
    if InDetFlags.doBremRecovery() and InDetFlags.doCaloSeededBrem() and not DetFlags.detdescr.Calo_allOn():
      printfunc ("*************************************************************")
      printfunc ("* Major FAILURE : InDet reconstruction in seeded brem mode, *")
      printfunc ("*                 but LAr is off                            *")
      printfunc ("*************************************************************")
      # raise RuntimeError ("Conflict in InDetRec setup: seeded Brem recovery, but Calo is off")
      if InDetFlags.doBremRecovery.is_locked():
        InDetFlags.doBremRecovery.unlock()
      printfunc (" ------------> WARNING: turning off brem recovery")
      InDetFlags.doBremRecovery.set_Value_and_Lock(False)
      # --- ditto for calo seeded TRT segments
    if InDetFlags.doCaloSeededTRTSegments() and not DetFlags.detdescr.Calo_allOn():
      printfunc ("******************************************************")
      printfunc ("* Major FAILURE : Back tracking in Calo seeded mode, *")
      printfunc ("*                 but Calo is off                    *")
      printfunc ("******************************************************")
      if InDetFlags.doCaloSeededTRTSegments.is_locked():
        InDetFlags.doCaloSeededTRTSegments.unlock()
      printfunc (" ------------> WARNING: Using Standard Back Tracking (not calo seeded)")
      InDetFlags.doCaloSeededTRTSegments.set_Value_and_Lock(False)


    # --- print setup
    InDetFlags.printInfo()
    InDetFlags.print_JobProperties()

    #
    # ----------- import, lock and print InDetKeys
    #
    from InDetRecExample.InDetKeys import InDetKeys
    InDetKeys.lockAllExceptAlias()
    printfunc ("Printing InDetKeys")
    InDetKeys.print_JobProperties()

    #
    # --- setup of cut values for NewTracking
    #

    if ('InDetNewTrackingCuts' not in dir()):
      printfunc ("InDetRec_jobOptions: InDetNewTrackingCuts not set before - import them now")
      from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
      if InDetFlags.doDBMstandalone():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("DBM")
      elif InDetFlags.doVtxLumi():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxLumi")
      elif InDetFlags.doVtxBeamSpot():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxBeamSpot")
      elif InDetFlags.doCosmics():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Cosmics")
      elif InDetFlags.doHeavyIon():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HeavyIon")
      elif InDetFlags.doSLHC():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("SLHC")
      elif InDetFlags.doIBL():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("IBL")
      elif InDetFlags.doHighPileup():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HighPileup")
      elif InDetFlags.doMinBias():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("MinBias")
      elif InDetFlags.doDVRetracking():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("R3LargeD0")
      else:
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Offline")
    InDetNewTrackingCuts.printInfo()
    # --- backward compatible
    InDetCutValues = InDetNewTrackingCuts

    #
    # --- setup of cut values for  Primary vertexing
    #
    if ('InDetPrimaryVertexingCuts' not in dir()):
      printfunc ("InDetRec_jobOptions: InDetPrimaryVertexingCuts not set before - import them now")
      from InDetRecExample.ConfiguredVertexingCuts import ConfiguredVertexingCuts
      InDetPrimaryVertexingCuts      = ConfiguredVertexingCuts(InDetFlags.primaryVertexCutSetup())
      InDetPrimaryVertexingCuts.printInfo()

    # -----------
    #
    # --- setup key aliases to pass to configurables
    #
    InDetSpSeededTracksKey    = InDetKeys.SiSpSeededTracks()
    InDetResolvedTracksKey    = InDetKeys.ResolvedTracks()
    InDetExtendedTracksKey    = InDetKeys.ExtendedTracks()
    InDetExtendedTracksMapKey = InDetKeys.ExtendedTracksMap()

    if InDetFlags.doDBMstandalone():
      InDetSpSeededTracksKey    = InDetKeys.SiSpSeededDBMTracks()
      InDetResolvedTracksKey    = InDetKeys.DBMTracks()

    if InDetFlags.doSLHC():
      InDetSpSeededTracksKey    = InDetKeys.SiSpSeededSLHCTracks()
      InDetResolvedTracksKey    = InDetKeys.ResolvedSLHCTracks()
      InDetExtendedTracksKey    = InDetKeys.ExtendedSLHCTracks()
      InDetExtendedTracksMapKey = InDetKeys.ExtendedTracksMapSLHC()

    if globalflags.InputFormat() == 'bytestream':
      ServiceMgr.ByteStreamCnvSvc.IsSimulation = (globalflags.DataSource() == 'geant4')


    # ------------------------------------------------------------
    #
    # ----------- special case for Calo seeded brem recovery
    #
    # ------------------------------------------------------------
    if DetFlags.detdescr.Calo_allOn() and (
         (InDetFlags.doBremRecovery() and InDetFlags.doCaloSeededBrem())
      or (InDetNewTrackingCuts.RoISeededBackTracking() and DetFlags.haveRIO.TRT_on() and InDetFlags.doTRTSeededTrackFinder())) :
      include ("InDetRecExample/InDetRecCaloSeededROISelection.py")

    # ------------------------------------------------------------
    #
    # -----------ROI seeding for SSS seeds or Ambi
    #
    # ------------------------------------------------------------
    #
    if (InDetFlags.doHadCaloSeededSSS() or InDetFlags.doCaloSeededAmbi() or InDetFlags.doCaloSeededRefit()) and DetFlags.detdescr.Calo_allOn():
      include ("InDetRecExample/InDetRecHadCaloSeededROISelection.py")

    # ------------------------------------------------------------
    #
    # ----------- Configuring the conditions access
    #
    # ------------------------------------------------------------
    if not rec.doAODMerging():
      include ("InDetRecExample/InDetRecConditionsAccess.py")

    # ------------------------------------------------------------
    #
    # ----------- Loading the Tracking Tools and Services
    #
    # ------------------------------------------------------------

    if not rec.doAODMerging():
      include ("InDetRecExample/InDetRecLoadTools.py")

    # ------------------------------------------------------------
    #
    # ----------- Data-Preparation stage
    #
    # ------------------------------------------------------------
    # --- silicon
    include ("InDetRecExample/InDetRecPreProcessingSilicon.py")

    # --- TRT, no drift information if cosmics, do not use extrenal phase in any case
    if not InDetFlags.doDBMstandalone():
      include ("InDetRecExample/ConfiguredInDetPreProcessingTRT.py")
      InDetPreProcessingTRT = ConfiguredInDetPreProcessingTRT(not InDetFlags.doTRTPhaseCalculation() or jobproperties.Beam.beamType()=="collisions",False)

    # ------------------------------------------------------------
    #
    # --- some collection lists (bookkeeping)
    #
    # ------------------------------------------------------------

    # track collection keys for statistics
    if 'TrackCollectionKeys' not in dir():
      TrackCollectionKeys        = []

    if 'TrackCollectionTruthKeys' not in dir():
      TrackCollectionTruthKeys   = []

    if InDetFlags.doDBM() and 'TrackCollectionKeysDBM' not in dir():
      TrackCollectionKeysDBM = []
    if InDetFlags.doDBM() and 'TrackCollectionTruthKeysDBM' not in dir():
      TrackCollectionTruthKeysDBM = []

    # NewTracking collection keys
    InputCombinedInDetTracks = []

    # ------------------------------------------------------------
    #
    # ----------- Subdetector pattern from New Tracking
    #
    # ------------------------------------------------------------
    #
    # --- Pixel track segment finding
    #
    if InDetFlags.doTrackSegmentsPixel():
      # --- load cuts for pixel segment finding
      if ('InDetNewTrackingCutsPixel' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsPixel not set before - import them now")
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsPixel = ConfiguredNewTrackingCuts("Pixel")
      InDetNewTrackingCutsPixel.printInfo()
      # --- configure pixel segment finding
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetNewTrackingSiPattern = ConfiguredNewTrackingSiPattern([],InDetKeys.PixelTracks(),
                                                                 InDetKeys.SiSpSeededPixelTracks(),
                                                                 InDetNewTrackingCutsPixel,
                                                                 TrackCollectionKeys,
                                                                 TrackCollectionTruthKeys)

      if InDetFlags.doTruth():
        PixelTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.PixelTracks(),
                                                     InDetKeys.PixelDetailedTracksTruth(),
                                                     InDetKeys.PixelTracksTruth())

    #
    # --- SCT track segment finding
    #
    if InDetFlags.doTrackSegmentsSCT():
      # --- load cuts for SCT segment finding
      if ('InDetNewTrackingCutsSCT' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsSCT not set before - import them now")
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsSCT = ConfiguredNewTrackingCuts("SCT")
      InDetNewTrackingCutsSCT.printInfo()
      # --- configure pixel segment finding
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetNewTrackingSiPattern = ConfiguredNewTrackingSiPattern([],InDetKeys.SCTTracks(),
                                                                 InDetKeys.SiSpSeededSCTTracks(),
                                                                 InDetNewTrackingCutsSCT,
                                                                 TrackCollectionKeys,
                                                                 TrackCollectionTruthKeys)


    # ------------------------------------------------------------
    #
    # ----------- Cosmics Si pattern before we do the TRT phase
    #
    # ------------------------------------------------------------
    #
    # In case of cosmics TRT needs to have the drifttime adjusted for the phase
    #
    if InDetFlags.doTRTPhaseCalculation() and not jobproperties.Beam.beamType()=="collisions":
      #
      # We need to run the silicon tracking already at this stage together with
      # the TRT tracking on uncalibrated PRDs to be able to calculate the phase

      # input collection for TRT Phase
      InDetCosmicSiTrackCollection = ""

      if InDetFlags.doNewTracking():
        #
        # --- run NewTracking Si pattern
        #
        include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
        InDetNewTrackingSiPattern = ConfiguredNewTrackingSiPattern([], InDetResolvedTracksKey,
                                                                   InDetSpSeededTracksKey,
                                                                   InDetNewTrackingCuts,
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys)
        # set output track collection
        InDetCosmicSiTrackCollection = InDetNewTrackingSiPattern.SiTrackCollection()

      # --- we only run the TRT phase, if the TRT is on !
      #
      if InDetFlags.doPRDFormation() and DetFlags.makeRIO.TRT_on():


        include ("InDetRecExample/ConfiguredNewTrackingTRTExtension.py")
        InDetPhaseTRTExtension = ConfiguredNewTrackingTRTExtension(InDetNewTrackingCuts,
                                                                   InDetNewTrackingSiPattern.SiTrackCollection(),
                                                                   InDetKeys.ExtendedTracksPhase(),
                                                                   InDetKeys.ExtendedTracksMapPhase(),
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys,
                                                                   True)


        import copy
        include ("InDetRecExample/ConfiguredTRTSegmentFinding.py")
        InDetPhaseTRTSegementFinding = ConfiguredTRTSegmentFinding("",
                                                                   copy.copy(InputCombinedInDetTracks),
                                                                   InDetNewTrackingCuts,
                                                                   InDetKeys.TRT_Segments_Phase(),
                                                                   None,
                                                                   True)

        # --- making tacks out of segments
        #include ("InDetRecExample/ConfiguredTRTStandalone.py")
        #InDetRecPhaseTRTStandalone = ConfiguredTRTStandalone ("", [],
        #                                                 InDetNewTrackingCuts,
        #                                                 InDetKeys.TRT_Segments_Phase(),
        #                        #                        InDetKeys.TRT_SegmentsTRT_EC(),
        #                                                 TrackCollectionKeys,
        #                                                 TrackCollectionTruthKeys)


        # @TODO seems unused ??
        import InDetRecExample.TrackingCommon   as TrackingCommon
        InDetTRTonly_PRD_AssociationPhase = TrackingCommon.getInDetTrackPRD_Association(namePrefix = 'InDetTRTonly_',
                                                                                        nameSuffix = "Phase",
                                                                                        TracksName = copy.copy(TrackCollectionKeys))

        # asso_tool_phase = TrackingCommon.getConstPRD_AssociationTool(namePrefix = 'InDetTRTonly_',nameSuffix     = "Phase")

        topSequence += InDetTRTonly_PRD_AssociationPhase
        if (InDetFlags.doPrintConfigurables()):
          printfunc (InDetTRTonly_PRD_AssociationPhase)

        from AthenaCommon import CfgGetter
        # @TODO create track summary for the following tracks .i.e. add "SummaryTool = TrackingCommon.getInDetTrackSummaryTool()", ?
        from TRT_SegmentsToTrack.TRT_SegmentsToTrackConf import InDet__TRT_SegmentsToTrack
        InDetTrkSegmenttoTrkPhase = InDet__TRT_SegmentsToTrack(name                      = "InDetTRT_SegmentsToTrack_BarrelPhase",
                                                        InputSegmentsCollection   = InDetKeys.TRT_Segments_Phase(),
                                                        OutputTrackCollection     = InDetKeys.TRT_Tracks_Phase(),
                                                        TrackFitter               = CfgGetter.getPublicTool('InDetTrackFitter'),
                                                        MinNHit                   = InDetNewTrackingCuts.minTRTonly(),
                                                        OutlierRemoval            = True,
                                                        MaterialEffects           = False)
        topSequence += InDetTrkSegmenttoTrkPhase
        if InDetFlags.doPrintConfigurables():
          printfunc (InDetTrkSegmenttoTrkPhase)


        include ("InDetRecExample/ConfiguredInDetTRTPhase.py")
        InDetRecTRTPhase = ConfiguredInDetTRTPhase ([ InDetKeys.TRT_Tracks_Phase(),
                                                      InDetPhaseTRTExtension.ForwardTrackCollection() ])

        # --- now rerun the preprocessing of the TRT with external phase
        #
        include ("InDetRecExample/ConfiguredInDetPreProcessingTRT.py")
        InDetPreProcessingTRT = ConfiguredInDetPreProcessingTRT(True, True)


    # ------------------------------------------------------------
    #
    # ----------- now we do the New Tracking
    #
    # ------------------------------------------------------------

    if InDetFlags.doNewTracking():
      #
      # --- do the Si pattern if not done for the cosmic case above
      #
      if not InDetFlags.doTRTPhaseCalculation() or jobproperties.Beam.beamType()=="collisions":
        include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
        InDetNewTrackingSiPattern = ConfiguredNewTrackingSiPattern([], InDetResolvedTracksKey,
                                                                   InDetSpSeededTracksKey,
                                                                   InDetNewTrackingCuts,
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys)
      #
      # --- do the TRT pattern
      #
      if InDetFlags.doTRTExtension() :

        include ("InDetRecExample/ConfiguredNewTrackingTRTExtension.py")
        InDetNewTrackingTRTExtension = ConfiguredNewTrackingTRTExtension(InDetNewTrackingCuts,
                                                                         InDetNewTrackingSiPattern.SiTrackCollection(),
                                                                         InDetExtendedTracksKey,
                                                                         InDetExtendedTracksMapKey,
                                                                         TrackCollectionKeys,
                                                                         TrackCollectionTruthKeys,
                                                                         False)
        # --- add into list for combination
        InputCombinedInDetTracks += [ InDetNewTrackingTRTExtension.ForwardTrackCollection() ]
      else :
        InputCombinedInDetTracks += [ InDetNewTrackingSiPattern.SiTrackCollection() ]

    # ------------------------------------------------------------
    #
    # --- Now comes Back Tracktracking
    #
    # ------------------------------------------------------------
    #
    # --- TRT segment finding after forward tracking on remaining hits
    #
    if InDetFlags.doTrtSegments():
      include ("InDetRecExample/ConfiguredTRTSegmentFinding.py")
      InDetRecTRTSegementFinding = ConfiguredTRTSegmentFinding ("",
                                                                InputCombinedInDetTracks,
                                                                InDetNewTrackingCuts,
                                                                InDetKeys.TRT_Segments()
                                                                )
    #
    # --- normal Back Tracking
    #
    if InDetFlags.doBackTracking():
      include ("InDetRecExample/ConfiguredBackTracking.py")
      InDetRecBackTracking = ConfiguredBackTracking (InputCombinedInDetTracks,
                                                     InDetNewTrackingCuts,
                                                     TrackCollectionKeys,
                                                     TrackCollectionTruthKeys)
      # --- add into list for combination
      InputCombinedInDetTracks += [ InDetRecBackTracking.BackTrackingTracks() ]


    # ------------------------------------------------------------
    #
    # --- Large-d0 option (FIXME: Here or should be placed
    #     after standard reconstruction...?
    #
    # ------------------------------------------------------------
    if InDetFlags.doLargeD0() or InDetFlags.doR3LargeD0() or InDetFlags.doLowPtLargeD0():
      #
      # --- run Si pattern for high-d0
      #
      if InDetFlags.doDVRetracking():
          # Cuts already defined in the mode, no need to re-load them
          InDetNewTrackingCutsLargeD0 = InDetNewTrackingCuts
      if ('InDetNewTrackingCutsLargeD0' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsLargeD0 not set before - import them now"      )
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        if InDetFlags.doLowPtLargeD0():
          InDetNewTrackingCutsLargeD0 = ConfiguredNewTrackingCuts("LowPtLargeD0")
        elif InDetFlags.doR3LargeD0():
          InDetNewTrackingCutsLargeD0 = ConfiguredNewTrackingCuts("R3LargeD0")
        else:
          InDetNewTrackingCutsLargeD0 = ConfiguredNewTrackingCuts("LargeD0")
      InDetNewTrackingCutsLargeD0.printInfo()
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      # ----- Include (in the case of ESD processing) the standard tracks
      #       in order to use the PRD association tool and use only unused hits
      if InDetFlags.useExistingTracksAsInput():
          InputCombinedInDetTracks += [ InDetKeys.ProcessedESDTracks() ]
      InDetLargeD0SiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks,
                                                            InDetKeys.ResolvedLargeD0Tracks(),
                                                            InDetKeys.SiSpSeededLargeD0Tracks(),
                                                            InDetNewTrackingCutsLargeD0,
                                                            TrackCollectionKeys,
                                                            TrackCollectionTruthKeys)
      #
      # --- do the TRT pattern
      #
      include ("InDetRecExample/ConfiguredNewTrackingTRTExtension.py")
      InDetLargeD0TRTExtension = ConfiguredNewTrackingTRTExtension(NewTrackingCuts = InDetNewTrackingCutsLargeD0,
                                                                 SiTrackCollection= InDetLargeD0SiPattern.SiTrackCollection() if InDetFlags.doAmbiSolving() else None ,
                                                                 ExtendedTrackCollection = InDetKeys.ExtendedLargeD0Tracks(),
                                                                 ExtendedTracksMap = InDetKeys.ExtendedTracksMapLargeD0(),
                                                                 TrackCollectionKeys=TrackCollectionKeys,
                                                                 TrackCollectionTruthKeys=TrackCollectionTruthKeys,
                                                                 doPhase =False)
      # --- remove the standard tracks included some lines before (in the ESD
      #     processing case, those tracks are not part of the re-tracking procedure)
      if InDetFlags.useExistingTracksAsInput():
          _dummy = InputCombinedInDetTracks.pop()

      # --- add into list for combination
      # Add tracks to standard track collection or a separate container?
      if not InDetFlags.storeSeparateLargeD0Container():
        InputCombinedInDetTracks += [ InDetLargeD0TRTExtension.ForwardTrackCollection()]



    # ------------------------------------------------------------
    #
    # --- Low Pt option (after BackTracking)
    #
    # ------------------------------------------------------------

    if InDetFlags.doLowPt():
      #
      # --- configure cuts for Low Pt tracking
      #
      if ('InDetNewTrackingCutsLowPt' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsLowPt not set before - import them now"      )
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsLowPt = ConfiguredNewTrackingCuts("LowPt")
      InDetNewTrackingCutsLowPt.printInfo()
      #
      # --- now run Si pattern for Low Pt
      #
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetLowPtSiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks,
                                                           InDetKeys.ResolvedLowPtTracks(),
                                                           InDetKeys.SiSpSeededLowPtTracks(),
                                                           InDetNewTrackingCutsLowPt,
                                                           TrackCollectionKeys,
                                                           TrackCollectionTruthKeys)
      #
      # --- do the TRT pattern
      #
      include ("InDetRecExample/ConfiguredNewTrackingTRTExtension.py")
      InDetLowPtTRTExtension = ConfiguredNewTrackingTRTExtension(InDetNewTrackingCutsLowPt,
                                                                 InDetLowPtSiPattern.SiTrackCollection(),
                                                                 InDetKeys.ExtendedLowPtTracks(),
                                                                 InDetKeys.ExtendedTracksMapLowPt(),
                                                                 TrackCollectionKeys,
                                                                 TrackCollectionTruthKeys)
      # --- add into list for combination
      InputCombinedInDetTracks += [ InDetLowPtTRTExtension.ForwardTrackCollection() ]

    # ------------------------------------------------------------
    #
    # --- Very Low Pt option (after LowPt)
    #
    # ------------------------------------------------------------

    if InDetFlags.doVeryLowPt():
      #
      # --- configure cuts for very Low Pt tracking
      #
      if ('InDetNewTrackingCutsVeryLowPt' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsVeryLowPt not set before - import them now"      )
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsVeryLowPt = ConfiguredNewTrackingCuts("VeryLowPt")
      InDetNewTrackingCutsVeryLowPt.printInfo()
      #
      # --- now run Si pattern for Low Pt
      #
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetVeryLowPtSiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks,
                                                               InDetKeys.ResolvedVeryLowPtTracks(),
                                                               InDetKeys.SiSpSeededVeryLowPtTracks(),
                                                               InDetNewTrackingCutsVeryLowPt,
                                                               TrackCollectionKeys,
                                                               TrackCollectionTruthKeys)
      # --- add into list for combination
      InputCombinedInDetTracks += [ InDetVeryLowPtSiPattern.SiTrackCollection() ]

    # ------------------------------------------------------------
    #
    # --- TRT standalone (after LowPt)
    #
    # ------------------------------------------------------------
    if InDetFlags.doTRTStandalone():

      include ("InDetRecExample/ConfiguredTRTStandalone.py")
      import copy
      InDetRecTRTStandalone = ConfiguredTRTStandalone ("",
                                                       copy.copy(InputCombinedInDetTracks),
                                                       InDetNewTrackingCuts,
                                                       InDetKeys.TRT_Segments(),
                                                  #     InDetKeys.TRT_Segments_EC(),
                                                       TrackCollectionKeys,
                                                       TrackCollectionTruthKeys)
      # --- add into list for combination
      InputCombinedInDetTracks += [ InDetRecTRTStandalone.TRTStandaloneTracks() ]


    # ------------------------------------------------------------
    #
    # ----------- now we do legacy pattern if requested
    #
    # ------------------------------------------------------------

    #
    # --- TRT track segment finding
    #
    if InDetFlags.doTrackSegmentsTRT():
      # --- load cuts for TRT segment finding
      if ('InDetNewTrackingCutsTRT' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsTRT not set before - import them now")
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsTRT = ConfiguredNewTrackingCuts("TRT")
      InDetNewTrackingCutsTRT.printInfo()
      # --- segment finingd
      include ("InDetRecExample/ConfiguredTRTSegmentFinding.py")
      InDetRecTRTSegementFinding = ConfiguredTRTSegmentFinding ("_TRT", [],
                                                                InDetNewTrackingCutsTRT,
                                                                InDetKeys.TRT_SegmentsTRT()
                                                                )
      # --- making tracks out of segments


      include ("InDetRecExample/ConfiguredTRTStandalone.py")
      InDetRecTRTStandalone = ConfiguredTRTStandalone ("_TRT", [],
                                                       InDetNewTrackingCutsTRT,
                                                       InDetKeys.TRT_SegmentsTRT(),
                                #                       InDetKeys.TRT_SegmentsTRT_EC(),
                                                       TrackCollectionKeys,
                                                       TrackCollectionTruthKeys,
                                                       PRDtoTrackMap='InDetTRTonly_PRDtoTrackMap' if InDetFlags.doTRTStandalone() else '' )

    # ------------------------------------------------------------
    #
    # --- Forward Tracklets (after standard reconstruction)
    #
    # ------------------------------------------------------------

    if InDetFlags.doForwardTracks():
      # Add tracks that are not saved to the InputCombinedInDetTracks
      InputForwardInDetTracks = []
      InputForwardInDetTracks += InputCombinedInDetTracks
      if InDetFlags.doR3LargeD0() and InDetFlags.storeSeparateLargeD0Container():
        InputForwardInDetTracks +=[ InDetLargeD0TRTExtension.ForwardTrackCollection()]

    if InDetFlags.doForwardTracks() and InDetFlags.doSLHC():
      if InDetFlags.doSLHCVeryForward():
       if ('InDetNewTrackingCutsForwardTracks' not in dir()):
         printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsForwardTracks not set before - import them now"       )
         from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
         InDetNewTrackingCutsForwardTracks = ConfiguredNewTrackingCuts("VeryForwardSLHCTracks")
         InDetNewTrackingCutsForwardTracks.printInfo()
         #
         # --- now run Si pattern for Low Pt
         #
         include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
         InDetForwardTracksSiPattern = ConfiguredNewTrackingSiPattern(InputForwardInDetTracks,
 		                                                      InDetKeys.ResolvedForwardTracks(),
 		                                                      InDetKeys.SiSpSeededForwardTracks(),
 		                                                      InDetNewTrackingCutsForwardTracks,
 		                                                      TrackCollectionKeys,
 		                                                      TrackCollectionTruthKeys)
         # for ITK, forward tracks get added to the combined collection
         InputCombinedInDetTracks += [ InDetForwardTracksSiPattern.SiTrackCollection() ]


      else:
       if ('InDetNewTrackingCutsForwardTracks' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsForwardTracks not set before - import them now"      )
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsForwardTracks = ConfiguredNewTrackingCuts("ForwardSLHCTracks")
        InDetNewTrackingCutsForwardTracks.printInfo()
        #
        # --- now run Si pattern for Low Pt
        #
        include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
        InDetForwardTracksSiPattern = ConfiguredNewTrackingSiPattern(InputForwardInDetTracks,
                                                                   InDetKeys.ResolvedForwardTracks(),
                                                                   InDetKeys.SiSpSeededForwardTracks(),
                                                                   InDetNewTrackingCutsForwardTracks,
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys)
        # for ITK, forward tracks get added to the combined collection
        InputCombinedInDetTracks += [ InDetForwardTracksSiPattern.SiTrackCollection() ]




    elif InDetFlags.doForwardTracks():
      #
      # --- configure cuts for forward tracklets
      #
      if ('InDetNewTrackingCutsForwardTracks' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsForwardTracks not set before - import them now"      )
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsForwardTracks = ConfiguredNewTrackingCuts("ForwardTracks")
        InDetNewTrackingCutsForwardTracks.printInfo()
      #
      # --- now run Si pattern for Low Pt
      #
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetForwardTracksSiPattern = ConfiguredNewTrackingSiPattern(InputForwardInDetTracks,
                                                                   InDetKeys.ResolvedForwardTracks(),
                                                                   InDetKeys.SiSpSeededForwardTracks(),
                                                                   InDetNewTrackingCutsForwardTracks,
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys)
      # --- do not add into list for combination
      # InputCombinedInDetTracks += [ InDetForwardTracksSiPattern.SiTrackCollection() ]

    if InDetFlags.doSLHCConversionFinding() and InDetFlags.doSLHC():
      #
      # --- configure cuts for Low Pt tracking
      #
      if ('InDetNewTrackingCutsSLHCConversionFinding' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsSLHCConversionFinding not set before - import them now")
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsSLHCConversionFinding = ConfiguredNewTrackingCuts("SLHCConversionFinding")
      InDetNewTrackingCutsSLHCConversionFinding.printInfo()
      #
      #
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetSLHCConversionFindingSiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks,
                                                           InDetKeys.ResolvedSLHCConversionFindingTracks(),
                                                           InDetKeys.SiSpSeededSLHCConversionFindingTracks(),
                                                           InDetNewTrackingCutsSLHCConversionFinding,
                                                           TrackCollectionKeys,
                                                           TrackCollectionTruthKeys)

      InputCombinedInDetTracks += [ InDetKeys.ResolvedSLHCConversionFindingTracks() ]


    # ------------------------------------------------------------
    #
    # --- Pixel Tracklets on unassociated PRDs (after standard reconstruction + forward tracking)
    #
    # ------------------------------------------------------------

    if InDetFlags.doTrackSegmentsDisappearing():
      InputPixelInDetTracks = []
      InputPixelInDetTracks += InputCombinedInDetTracks
      # Add tracks that are not saved to the InputCombinedInDetTracks
      if InDetFlags.doForwardTracks():
        InputPixelInDetTracks +=[ InDetForwardTracksSiPattern.SiTrackCollection()]
      if InDetFlags.doR3LargeD0() and InDetFlags.storeSeparateLargeD0Container():
        InputPixelInDetTracks +=[ InDetLargeD0TRTExtension.ForwardTrackCollection()]
      # --- load cuts for pixel segment finding
      if ('InDetNewTrackingCutsDisappearing' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsDisappearing not set before - import them now")
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsDisappearing = ConfiguredNewTrackingCuts("Disappearing")
      InDetNewTrackingCutsDisappearing.printInfo()
      # --- configure pixel segment finding
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetPixelTrackingSiPattern = ConfiguredNewTrackingSiPattern(InputPixelInDetTracks,InDetKeys.ResolvedDisappearingTracks(),
                                                                 InDetKeys.SiSpSeededPixelTracks(),
                                                                 InDetNewTrackingCutsDisappearing,
                                                                 TrackCollectionKeys,
                                                                 TrackCollectionTruthKeys)

      if InDetFlags.doTRTExtension() :
        include ("InDetRecExample/ConfiguredNewTrackingTRTExtension.py")
        InDetPixelTrackingTRTExtension = ConfiguredNewTrackingTRTExtension(InDetNewTrackingCutsDisappearing,
                                                                   InDetKeys.ResolvedDisappearingTracks(),
                                                                   InDetKeys.ExtendedTracksDisappearing(),
                                                                   InDetKeys.ExtendedTracksMapDisappearing(),
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys,
                                                                   False)

    # ------------------------------------------------------------
    #
    # --- Beam Gas option (runs after NewT + LowPt)
    #
    # ------------------------------------------------------------

    if InDetFlags.doBeamGas():
      #
      # --- configure cuts
      #
      if ('InDetNewTrackingCutsBeamGas' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsBeamGas not set before - import them now"      )
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsBeamGas = ConfiguredNewTrackingCuts("BeamGas")
      if (InDetFlags.doPrintConfigurables()):
        InDetNewTrackingCutsBeamGas.printInfo()
      #
      # --- now run Si pattern for beam gas
      #
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      InDetBeamGasSiPattern = ConfiguredNewTrackingSiPattern(InputCombinedInDetTracks,
                                                             InDetKeys.ResolvedBeamGasTracks(),
                                                             InDetKeys.SiSpSeededBeamGasTracks(),
                                                             InDetNewTrackingCutsBeamGas,
                                                             TrackCollectionKeys,
                                                             TrackCollectionTruthKeys)
      #
      # --- do the TRT pattern
      #
      include ("InDetRecExample/ConfiguredNewTrackingTRTExtension.py")
      InDetBeamGasTRTExtension = ConfiguredNewTrackingTRTExtension(InDetNewTrackingCutsBeamGas,
                                                                   InDetBeamGasSiPattern.SiTrackCollection(),
                                                                   InDetKeys.ExtendedBeamGasTracks(),
                                                                   InDetKeys.ExtendedTracksMapBeamGas(),
                                                                   TrackCollectionKeys,
                                                                   TrackCollectionTruthKeys)

      # --- add into list for combination for single-beam mode (not if running after collisions)
      if jobproperties.Beam.beamType() == "singlebeam":
        InputCombinedInDetTracks += [ InDetBeamGasTRTExtension.ForwardTrackCollection() ]

    # ------------------------------------------------------------
    #
    # --- DBM
    #
    # ------------------------------------------------------------

    if InDetFlags.doDBM():
      # --- cuts
      if ('InDetNewTrackingCutsDBM' not in dir()):
        printfunc ("InDetRec_jobOptions: InDetNewTrackingCutsDBM not set before - import them now")
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsDBM = ConfiguredNewTrackingCuts("DBM")
        InDetNewTrackingCutsDBM.printInfo()

      # --- DBM versions of tools
      if InDetFlags.loadRotCreator():
        InDetRotCreator = InDetRotCreatorDBM
        if DetFlags.haveRIO.pixel_on():
          PixelClusterOnTrackTool = PixelClusterOnTrackToolDBM
      if InDetFlags.loadFitter():
        from AthenaCommon import CfgGetter
        InDetTrackFitter = CfgGetter.getPublicTool('InDetTrackFitterDBM')
      # if InDetFlags.doPattern():
      #  InDetSiComTrackFinder = InDetSiComTrackFinderDBM

#      InDetSiTrackerSpacePointFinder = InDetSiTrackerSpacePointFinderDBM

      # --- Si Pattern
      include ("InDetRecExample/ConfiguredNewTrackingSiPattern.py")
      DBMTrackingSiPattern = ConfiguredNewTrackingSiPattern([],InDetKeys.DBMTracks(),
                                                                 InDetKeys.SiSpSeededDBMTracks(),
                                                                 InDetNewTrackingCutsDBM,
                                                                 TrackCollectionKeysDBM,
                                                                 TrackCollectionTruthKeysDBM)
    #  InputCombinedInDetTracks += [ DBMTrackingSiPattern.SiTrackCollection() ]


    # ------------------------------------------------------------
    #
    # ----------- Ambi solve cosmic track collections from New Tracking
    #
    # ------------------------------------------------------------

    # --- Ambi solve the extended (Si + TRT) and TRT standalone tracks if both run
    if InDetFlags.doCosmics() and InDetFlags.doNewTracking() and len(InputCombinedInDetTracks) > 1:
      InputCosmicsCombinedAmbiSolver = list(InputCombinedInDetTracks)

      from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguityScore
      InDetAmbiguityScore_combinedCosmics = Trk__TrkAmbiguityScore(name                    = 'InDetCombinedCosmicsAmbiguityScore',
                                                                   TrackInput              = InputCosmicsCombinedAmbiSolver,
                                                                   TrackOutput             = 'ScoredMapCosmics',
                                                                   AmbiguityScoreProcessor = '' )

      topSequence += InDetAmbiguityScore_combinedCosmics

      from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
      InDetAmbiguitySolver_combinedCosmics = Trk__TrkAmbiguitySolver(name               = 'InDetCombinedCosmicsAmbiSolver',
                                                                     TrackInput         = 'ScoredMapCosmics',
                                                                     TrackOutput        = "CombinedCosmicTracks",
                                                                     AmbiguityProcessor = InDetAmbiguityProcessorCosmics)
      topSequence += InDetAmbiguitySolver_combinedCosmics
      if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetAmbiguitySolver_combinedCosmics)

      InputCombinedInDetTracks = [ InDetKeys.CombinedCosmicTracks() ]

    # ------------------------------------------------------------
    #
    # ----------- now we do the Pseudo Tracking
    #
    # ------------------------------------------------------------

    # if new tracking is off, pseudo tracking replaces the output collection
    # if new tracking is on, pseudo tracking simply runs alongside
    # if split reco is on, pseudo tracking runs only on pileup and the output collections are merged
    if InDetFlags.doPseudoTracking() :
        # --- needed for sorting the PrepRawData properly
        from InDetTruthTools.InDetTruthToolsConf import InDet__PRD_TruthTrajectorySorterID
        InDetTruthTrajectorySorter = InDet__PRD_TruthTrajectorySorterID(name='InDetTruthTrajectorySorter')
        ToolSvc += InDetTruthTrajectorySorter

        # --- tool that provides the PRDs to the trajectory builder
        from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_Provider
        # change input collection key if taking only pileup
        if InDetFlags.doSplitReco() :
            PixelClusterCont    = InDetKeys.PixelPUClusters()
            SCT_ClusterCont     = InDetKeys.SCT_PU_Clusters()
            if InDetFlags.doTrackSegmentsTRT() :
                TRT_DriftCircleCont = InDetKeys.TRT_PU_DriftCircles()
            else:
                TRT_DriftCircleCont =""
        else:
            PixelClusterCont    = InDetKeys.PixelClusters()
            SCT_ClusterCont     = InDetKeys.SCT_Clusters()
            if InDetFlags.doTrackSegmentsTRT() :
                TRT_DriftCircleCont = InDetKeys.TRT_DriftCircles()
            else:
                TRT_DriftCircleCont =""
        InDetPRD_Provider = InDet__InDetPRD_Provider(name                     = 'InDetPRD_Provider',
                                                     PixelClusterContainer    = PixelClusterCont,
                                                     SCT_ClusterContainer     = SCT_ClusterCont,
                                                     TRT_DriftCircleContainer = TRT_DriftCircleCont)
        ToolSvc += InDetPRD_Provider

        # --- the truth track builder
        from AthenaCommon import CfgGetter
        from TrkTruthTrackTools.TrkTruthTrackToolsConf import Trk__TruthTrackBuilder
        # @TODO should a track fitter be used which does not require a split cluster map ?
        InDetTruthTrackBuilder = Trk__TruthTrackBuilder(name                = 'InDetTruthTrackBuilder',
                                                        TrackFitter         = CfgGetter.getPublicTool('InDetTrackFitter'),
                                                        ExtrapolationTool   = TrackingCommon.getInDetExtrapolator(),
                                                        RotCreatorTool      = InDetRotCreator,
                                                        BroadRotCreatorTool = BroadInDetRotCreator,
                                                        MinDegreesOfFreedom = 1,
                                                        MatEffects          =  InDetFlags.materialInteractionsType(),
                                                        MinSiHits           =  InDetNewTrackingCuts.minClusters() )
        if InDetFlags.doForwardTracks() and InDetFlags.doSLHC():
            InDetTruthTrackBuilder.MinSiHitsForward = InDetNewTrackingCutsForwardTracks.minClusters()
            InDetTruthTrackBuilder.ForwardBoundary  = InDetNewTrackingCutsForwardTracks.minEta()
#        InDetTruthTrackBuilder.OutputLevel = VERBOSE
        ToolSvc += InDetTruthTrackBuilder

        # --- the truth PRD trajectory builder

        #
        from TrkTruthTrackTools.TrkTruthTrackToolsConf import Trk__PRD_TruthTrajectoryBuilder
        # change input collection key if taking only pileup
        if InDetFlags.doSplitReco() :
            PixelClusterTruth      = InDetKeys.PixelPUClustersTruth()
            SCT_ClusterTruth       = InDetKeys.SCT_PU_ClustersTruth()
            TRT_DriftCircleTruth   = InDetKeys.TRT_PU_DriftCirclesTruth()
        else:
            PixelClusterTruth    = InDetKeys.PixelClustersTruth()
            SCT_ClusterTruth     = InDetKeys.SCT_ClustersTruth()
            TRT_DriftCircleTruth = InDetKeys.TRT_DriftCirclesTruth()
        InDetPRD_TruthTrajectoryBuilder=Trk__PRD_TruthTrajectoryBuilder(name = 'InDetPRD_TruthTrajectoryBuilder',
                                         PRD_MultiTruthCollections       = [ PixelClusterTruth,SCT_ClusterTruth,TRT_DriftCircleTruth],
                                         InDetPRD_Provider               = InDetPRD_Provider,
                                         MinimumPt                       =  InDetNewTrackingCuts.minPT(),
                                         PRD_TruthTrajectoryManipulators = [ InDetTruthTrajectorySorter ])
        if not InDetFlags.doSLHC() and not InDetFlags.doIdealPseudoTracking():
         # --- the trajectory manipulator
            from InDetTruthTools.InDetTruthToolsConf import InDet__PRD_TruthTrajectoryManipulatorID
            InDetTruthTrajectoryManipulator = InDet__PRD_TruthTrajectoryManipulatorID(name='InDetTruthTrajectoryManipulator')
            ToolSvc += InDetTruthTrajectoryManipulator

         # --- the trajectory shared cluster hits fixer
#           from InDetTruthTools.InDetTruthToolsConf import InDet__PRD_TruthTrajectorySharedFixerID
#           InDetTruthTrajectorySharedFixer = InDet__PRD_TruthTrajectorySharedFixerID(name = 'InDetTruthTrajectorySharedFixer' )
#           ToolSvc += InDetTruthTrajectorySharedFixer

            InDetPRD_TruthTrajectoryBuilder.PRD_TruthTrajectoryManipulators = [ InDetTruthTrajectorySorter, InDetTruthTrajectoryManipulator ]

        ToolSvc+=InDetPRD_TruthTrajectoryBuilder
#        InDetPRD_TruthTrajectoryBuilder.OutputLevel = VERBOSE

        # --- the (1st) trajectory selector
        PRD_TruthTrajectorySelector = []
        if not InDetFlags.doSLHC() and not InDetFlags.doSplitReco() :
          from InDetTruthTools.InDetTruthToolsConf import InDet__PRD_TruthTrajectorySelectorID
          InDetTruthTrajectorySelector = InDet__PRD_TruthTrajectorySelectorID(name='InDetTruthTrajectorySelector')
          ToolSvc += InDetTruthTrajectorySelector
          PRD_TruthTrajectorySelector  = [ InDetTruthTrajectorySelector ]

        # --- the truth track creation algorithm
        from InDetRecExample.TrackingCommon import getInDetPRDtoTrackMapToolGangedPixels
        from TrkTruthTrackAlgs.TrkTruthTrackAlgsConf import Trk__TruthTrackCreation
        InDetTruthTrackCreation = Trk__TruthTrackCreation(name = 'InDetTruthTrackCreation',
                                                          PRD_TruthTrajectoryBuilder = InDetPRD_TruthTrajectoryBuilder,
                                                          TruthTrackBuilder          = InDetTruthTrackBuilder,
                                                          OutputTrackCollection      = InDetKeys.PseudoTracks(),
                                                          AssociationTool            = getInDetPRDtoTrackMapToolGangedPixels(),
                                                          TrackSummaryTool           = TrackingCommon.getInDetTrackSummaryToolSharedHits(),
                                                          PRD_TruthTrajectorySelectors  = PRD_TruthTrajectorySelector )
#        InDetTruthTrackCreation.OutputLevel = VERBOSE
        topSequence += InDetTruthTrackCreation

        if  InDetFlags.doSplitReco() or InDetFlags.doIdealPseudoTracking() :
          # --- add the truth to the truth tracks ;-)
          include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
          InDetTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.PseudoTracks(),
                                                     InDetKeys.PseudoDetailedTracksTruth(),
                                                     InDetKeys.PseudoTracksTruth(),
                                                     PixelClusterTruth,
                                                     SCT_ClusterTruth,
                                                     TRT_DriftCircleTruth)

          from TrkTruthToTrack.TrkTruthToTrackConf import Trk__TruthToTrack
          InDetTruthToTrack  = Trk__TruthToTrack(name         = "InDetTruthToTrack",
                                               Extrapolator = TrackingCommon.getInDetExtrapolator())
          ToolSvc += InDetTruthToTrack

        # Register the track collections for further processing - only if new tracking has not been running
        if not InDetFlags.doNewTracking():
            InputTrackCollection      = InDetKeys.PseudoTracks()
            InputTrackCollectionTruth = InDetKeys.PseudoTracksTruth()
            InputDetailedTrackTruth   = InDetKeys.PseudoDetailedTracksTruth()
            # make sure downstream clients get the right tracks
            InDetKeys.Tracks                   = InDetKeys.PseudoTracks()
            InDetKeys.UnslimmedTracks          = InDetKeys.PseudoTracks()
            InDetKeys.TracksTruth              = InDetKeys.PseudoTracksTruth()
            InDetKeys.UnslimmedTracksTruth     = InDetKeys.PseudoTracksTruth()
            # add as the combined ID collection
            InputCombinedInDetTracks = [ InDetKeys.PseudoTracks() ]
        if InDetFlags.doSplitReco() :
            InputCombinedInDetTracks += [ InDetKeys.PseudoTracks() ]


        # add final output for statistics
        TrackCollectionKeys      += [ InDetKeys.PseudoTracks() ]
        TrackCollectionTruthKeys += [ InDetKeys.PseudoTracksTruth() ]
    # ------------------------------------------------------------
    #
    # ----------- Merge track collections from NewTracking
    #
    # ------------------------------------------------------------

    # always merge track collections. even if only one collection ran!
    # (this is because the track merger fills the prd association tool!)
    # *ME* fix, only merge if more than 1 track collection
    if InDetFlags.doNewTrackingPattern() or InDetFlags.doBeamHalo():

      if (InDetFlags.doDBM() or InDetFlags.doDBMstandalone()):
        if InDetFlags.doTruth():
          include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
          InDetTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.DBMTracks(),
                                                       InDetKeys.DBMDetailedTracksTruth(),
                                                       InDetKeys.DBMTracksTruth())

      if InDetFlags.useExistingTracksAsInput():
          InputCombinedInDetTracks +=  [ InDetKeys.ProcessedESDTracks() ]

      if InDetFlags.doDBMstandalone():
        TrackCollectionKeys      += [ InDetKeys.DBMTracks() ]
        TrackCollectionTruthKeys += [ InDetKeys.DBMTracksTruth() ]
      else:
        from TrkTrackCollectionMerger.TrkTrackCollectionMergerConf import Trk__TrackCollectionMerger
        from InDetRecExample.TrackingCommon                        import getInDetPRDtoTrackMapToolGangedPixels
        TrkTrackCollectionMerger = Trk__TrackCollectionMerger(name                    = "InDetTrackCollectionMerger",
                                                              TracksLocation          = InputCombinedInDetTracks,
                                                              OutputTracksLocation    = InDetKeys.UnslimmedTracks(),
                                                              AssociationTool         = getInDetPRDtoTrackMapToolGangedPixels(),
                                                              AssociationMapName      = "PRDtoTrackMap" + InDetKeys.UnslimmedTracks(),
                                                              UpdateSharedHitsOnly    = False,
                                                              UpdateAdditionalInfo    = True,
                                                              SummaryTool             = TrackingCommon.getInDetTrackSummaryToolSharedHits())
        topSequence += TrkTrackCollectionMerger

        if (InDetFlags.doPrintConfigurables()):
          printfunc (TrkTrackCollectionMerger)

      # --- Delete unmerged tracks (Si, back-tracking, TRT)
        from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
        if not InDetFlags.doMonitoringAlignment():
            InDetSGDeletionAlg(key = InputCombinedInDetTracks)
        else:
            InDetSGDeletionAlg(key = [k for k in InputCombinedInDetTracks if not k == "ExtendedTracks"])


      #
      # ------------ Track truth.
      #
        if not InDetFlags.doSGDeletion():
            TrackCollectionKeys      += [ InDetKeys.UnslimmedTracks() ]
        if InDetFlags.doTruth():
          # set up the truth info for this container
          #
            include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
            InDetTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.UnslimmedTracks(),
                                                         InDetKeys.UnslimmedDetailedTracksTruth(),
                                                         InDetKeys.UnslimmedTracksTruth())
          #
          # add final output for statistics
          #
            TrackCollectionTruthKeys += [ InDetKeys.UnslimmedTracksTruth() ]



      # Dummy Merger to fill additional info for PRD-associated pixel tracklets
      if InDetFlags.doTrackSegmentsDisappearing():
       DummyCollection = []
       if InDetFlags.doTRTExtension() :
         DummyCollection += [ InDetKeys.ExtendedTracksDisappearing()]

       from InDetRecExample.TrackingCommon                        import getInDetPRDtoTrackMapToolGangedPixels
       TrkTrackCollectionMerger_pix = Trk__TrackCollectionMerger(name                    = "InDetTrackCollectionMerger_pix",
                                                                 TracksLocation          = DummyCollection,
                                                                 OutputTracksLocation    = InDetKeys.DisappearingTracks(),
                                                                 AssociationTool         = getInDetPRDtoTrackMapToolGangedPixels(),
                                                                 UpdateSharedHitsOnly    = False,
                                                                 UpdateAdditionalInfo    = True,
                                                                 SummaryTool             = TrackingCommon.getInDetTrackSummaryToolSharedHits())
       #TrkTrackCollectionMerger_pix.OutputLevel = VERBOSE
       topSequence += TrkTrackCollectionMerger_pix


       if InDetFlags.doTruth():
          # set up the truth info for this container
          #
            include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
            InDetTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.DisappearingTracks(),
                                                         InDetKeys.DisappearingDetailedTracksTruth(),
                                                         InDetKeys.DisappearingTracksTruth())


       if (InDetFlags.doPrintConfigurables()):
         printfunc (TrkTrackCollectionMerger_pix)


    # ------------------------------------------------------------
    #
    # ----------- prepare output track collection for post processing
    #
    # ------------------------------------------------------------

    #
    # -- Pick one of the result collections and turn it into tracks
    #
    if InDetFlags.doNewTrackingPattern():
      if InDetFlags.doDBMstandalone():
        InputTrackCollection = InDetKeys.DBMTracks()
      else:
        InputTrackCollection = InDetKeys.UnslimmedTracks()
    elif InDetFlags.doPseudoTracking():
      InputTrackCollection = InDetKeys.PseudoTracks()
    else:
      # --- in case of reading from ESD, so we just set the Collection and truth
      InputTrackCollection      = InDetKeys.Tracks()
      InputTrackCollectionTruth = InDetKeys.TracksTruth()

    # ----------------------------------------------------------------
    #
    # --- do we refit all tracks ?
    #
    # ----------------------------------------------------------------
    if InDetFlags.doRefit():
      from InDetRecExample.TrackingCommon import getInDetPRDtoTrackMapToolGangedPixels
      from TrkRefitAlg.TrkRefitAlgConf import Trk__ReFitTrack
      from AthenaCommon import CfgGetter
      # @TODO for the track refit can a track fitter be used which requires a split cluster map ?
      #       Should the scoring+split cluster map building be  executed ?
      InDetReFitTrack = Trk__ReFitTrack (name           = "InDetRefitTrack",
                                         FitterTool     = CfgGetter.getPublicTool('InDetTrackFitter'),
                                         FitterToolTRT  = CfgGetter.getPublicTool('InDetTrackFitterTRT'),
                                         SummaryTool    = TrackingCommon.getInDetTrackSummaryToolSharedHits(),
                                         AssociationTool= getInDetPRDtoTrackMapToolGangedPixels(),
                                         TrackName      = InputTrackCollection,
                                         NewTrackName   = InDetKeys.RefittedTracks(),
                                         fitRIO_OnTrack = InDetFlags.refitROT(),
                                         useParticleHypothesisFromTrack = True)

      if InDetFlags.materialInteractions():
        InDetReFitTrack.matEffects = InDetFlags.materialInteractionsType()
      else:
        InDetReFitTrack.matEffects = 0

      topSequence += InDetReFitTrack
      if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetReFitTrack)
      # set input track name
      InputTrackCollection = InDetKeys.RefittedTracks()
      if InDetFlags.doTruth():
        InputDetailedTrackTruth   = InDetKeys.RefittedDetailedTracksTruth()
        InputTrackCollectionTruth = InDetKeys.RefittedTracksTruth()

    # ----------------------------------------------------------------
    #
    # --- slimm the tracks down before writing them ?
    #
    # ----------------------------------------------------------------
    from TrkTrackSlimmingTool.TrkTrackSlimmingToolConf import Trk__TrackSlimmingTool as ConfigurableTrackSlimmingTool
    InDetTrkSlimmingTool = ConfigurableTrackSlimmingTool(name           = "InDetTrackSlimmingTool",
                                                         KeepParameters = InDetFlags.KeepParameters(),
                                                         KeepOutliers   = True,
                                                         OnlySetPersistificationHints = False)
    ToolSvc += InDetTrkSlimmingTool

    from RecExConfig.AutoConfiguration import IsInInputFile
    if InDetFlags.doSlimming() and (not IsInInputFile('TrackCollection',InDetKeys.Tracks()) or InDetFlags.doSlimPoolTrack() ) :
      # not an "elif" as we may want to slim or alias even if refit, in case pattern runs
      # do not run slimming if the destination collection already exists unless the TPconverter level slimming is enabled.

      #
      # --- do track slimming
      #
      if InDetFlags.doSlimPoolTrack() :
        InDetTrkSlimmingToolTracks = ConfigurableTrackSlimmingTool(name           = "InDetTrackSlimmingToolTPCnv",
                                                                   KeepParameters = InDetFlags.KeepParameters(),
                                                                   KeepOutliers   = True,
                                                                   OnlySetPersistificationHints = InDetFlags.doSlimPoolTrack() )
        ToolSvc += InDetTrkSlimmingToolTracks
      else :
        InDetTrkSlimmingToolTracks=InDetTrkSlimmingTool

      if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetTrkSlimmingTool)

      from TrkTrackSlimmer.TrkTrackSlimmerConf import Trk__TrackSlimmer as ConfigurableTrackSlimmer
      InDetTrkSlimmer = ConfigurableTrackSlimmer(name                 = "InDetTrackSlimmer",
                                                 TrackLocation        = [ InputTrackCollection ],
                                                 SlimmedTrackLocation = [ InDetKeys.Tracks() ],
                                                 TrackSlimmingTool    = InDetTrkSlimmingToolTracks,
                                                 OnlySetPersistificationHints = InDetFlags.doSlimPoolTrack() )
      topSequence += InDetTrkSlimmer
      if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetTrkSlimmer)

      if not (InDetFlags.doMonitoringGlobal() or
              InDetFlags.doNtupleCreation() or
              (InDetFlags.doMonitoringPixel() and not InDetFlags.doTrackSegmentsPixel()) or
              (InDetFlags.doMonitoringSCT()   and not InDetFlags.doTrackSegmentsSCT()  ) or
              (InDetFlags.doMonitoringTRT()   and not InDetFlags.doTrackSegmentsTRT()  )):
        if not InDetFlags.doSlimPoolTrack() :
          # --- Delete unslimmed tracks
          from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
          InDetSGDeletionAlg(key = InputTrackCollection)

      if not InDetFlags.doSlimPoolTrack() :
         # --- for output
         InDetKeys.AliasToTracks = 'none'
         # --- input for next algorithm
         InputTrackCollection    = InDetKeys.Tracks()
         if InDetFlags.doTruth():
            InputDetailedTrackTruth   = InDetKeys.DetailedTracksTruth()
            InputTrackCollectionTruth = InDetKeys.TracksTruth()
         # --- [FIXME JDC: PROVISIONAL PATCH. The final collection
         #      should be the one pointed by InDetKeys.Tracks()? Trying to
         #      find a solution...
         if InDetFlags.useExistingTracksAsInput():
            InDetTrkSlimmer.SlimmedTrackLocation = [ "MergedTracks" ]
            InputTrackCollection = "MergedTracks"
            if InDetFlags.doTruth():
                InputDetailedTrackTruth   = "MergedTracksDetailedTruth"
                InputTrackCollectionTruth = "MergedTracksTruth"
         # --- [FIXME JDC: END PROVISIONAL PATCH

    # ----------------------------------------------------------------
    #
    # --- or just make an alias ?
    #
    # ----------------------------------------------------------------
    if InDetFlags.doPattern() and (not InDetFlags.doSlimming() or InDetFlags.doSlimPoolTrack()):
      if not InDetFlags.doDBMstandalone():
      #
      # --- configure Algorithm to create output alias
      #
        from TrkCollectionAliasAlg.TrkCollectionAliasAlgConf import Trk__TrkCollectionAliasAlg
        InDetCopyAlg = Trk__TrkCollectionAliasAlg (name             = "InDetCopyAlg",
                                                   CollectionName   = InputTrackCollection,
                                                   AliasName        = InDetKeys.Tracks())
        topSequence += InDetCopyAlg
        if (InDetFlags.doPrintConfigurables()) or True:
          printfunc (InDetCopyAlg)
      # --- for output
        InDetKeys.AliasToTracks = InputTrackCollection
      # --- input for next algorithm
        InputTrackCollection    = InDetKeys.Tracks()

      if InDetFlags.doTruth():
        if InDetFlags.doDBMstandalone():
          InputDetailedTrackTruth   = InDetKeys.DBMDetailedTracksTruth()
          InputTrackCollectionTruth = InDetKeys.DBMTracksTruth()
        else:
          InputDetailedTrackTruth   = InDetKeys.DetailedTracksTruth()
          InputTrackCollectionTruth = InDetKeys.TracksTruth()
      # --- [FIXME JDC: PROVISIONAL PATCH. The final collection
      #      should be the one pointed by InDetKeys.Tracks()? Trying
      #      to find a soluction...
      if InDetFlags.useExistingTracksAsInput():
        InDetCopyAlg.AliasName = "MergedTracks"
        InputTrackCollection = "MergedTracks"
        if InDetFlags.doTruth():
            InputDetailedTrackTruth   = "MergedTracksDetailedTruth"
            InputTrackCollectionTruth = "MergedTracksTruth"
      # --- [FIXME JDC: PROVISIONAL PATCH. The final collection


    # -----------------------------------------------------------------
    #
    # --- in case of slimming or alias creation of tracks container, do the truth association
    #
    # -----------------------------------------------------------------
    if (InDetFlags.doRefit() or InDetFlags.doSlimming() or InDetFlags.doPattern()) :
      ## ME drop this for tracks container # TrackCollectionKeys      += [ InputTrackCollection ]
      if InDetFlags.doTruth():
        #
        # set up the truth info for this container
        #
        if not InDetFlags.doDBMstandalone():
          include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
          InDetTracksTruth = ConfiguredInDetTrackTruth(InputTrackCollection,
                                                       InputDetailedTrackTruth,
                                                       InputTrackCollectionTruth)
        #
        # add to keys lists for statistics
        #
        ## ME drop this for tracks contrain # TrackCollectionTruthKeys += [ InputTrackCollectionTruth ]

    # ------------------------------------------------------------
    #
    # ----------- now we do post-processing
    #
    # ------------------------------------------------------------
    #
    #
    # xAOD creation/conversion
    if (InDetFlags.doxAOD() and not rec.doAODMerging()):
      include("InDetRecExample/InDetxAODCreator.py")

    # Do post-processing algorithms (may depend on xAOD objects)
    if not InDetFlags.doDBMstandalone():
      include("InDetRecExample/InDetRecPostProcessing.py")

    # ------------------------------------------------------------
    #
    # ----------- now we do validation and
    #
    # ------------------------------------------------------------
    #

    # statistics
    if InDetFlags.doStatistics():
      if InDetFlags.doVeryLowPt():
        cuts = InDetNewTrackingCutsVeryLowPt
      elif InDetFlags.doLowPt():
        cuts = InDetNewTrackingCutsLowPt
      elif InDetFlags.doSLHCConversionFinding():
        cuts = InDetNewTrackingCutsSLHCConversionFinding
      else:
        cuts = InDetNewTrackingCuts
      include("InDetRecExample/ConfiguredInDetValidation.py")
      InDetValidation = ConfiguredInDetValidation("",True,InDetFlags.doTruth(),cuts,TrackCollectionKeys,TrackCollectionTruthKeys)
      if InDetFlags.doDBM():
        InDetValidationDBM = ConfiguredInDetValidation("DBM",True,InDetFlags.doTruth(),InDetNewTrackingCutsDBM,TrackCollectionKeysDBM,TrackCollectionTruthKeysDBM)
      if InDetFlags.doSplitReco():
        InDetValidationPU = ConfiguredInDetValidation("PU",True,InDetFlags.doTruth(),cuts,[InDetKeys.PseudoTracks()],[InDetKeys.PseudoTracksTruth()],McEventCollectionKey="TruthEvent_PU")

    # ntuple creation for validation purposes
    if (InDetFlags.doNtupleCreation() or InDetFlags.doPhysValMon()):
      include("InDetRecExample/InDetRecNtupleCreation.py")

    # D3PD Creation
    if (InDetFlags.doD3PDCreation()):
      include("InDetRecExample/InDetRecD3PDCreation.py")

    if not InDetFlags.doMonitoring():
      # --- Delete spacepoint collections
      from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
      InDetSGDeletionAlg(container = "SpacePointOverlapCollection#", key = InDetKeys.OverlapSpacePoints())

      # Delete Pixel and Silicon space points
      from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
      InDetSGDeletionAlg(container = "SpacePointContainer#", key = [InDetKeys.PixelSpacePoints(), InDetKeys.SCT_SpacePoints()])

    # +++++++++++++++++++ end of InDetRec_jobOptions.py
  # END if InDetFlags.Enabled()
