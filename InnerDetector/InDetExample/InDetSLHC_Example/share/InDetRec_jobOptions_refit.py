# +++++++++++++++++++ beginning of InDetRec_jobOptions_refit.py
# jobOptions Fragment for ID software
# --------------------------------------------------------------
# edward.moyse@cern.ch
# markus.elsing@cern.ch
# modified for use in Fatras refit mode: nicholas.styles@desy.de
# --------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.BeamFlags import jobproperties

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

    InDetFlags.printInfo()
    InDetFlags.print_JobProperties()    
    
    from AthenaCommon.GlobalFlags import globalflags
    # rec flags are needed (e.g. for commissioning steering ...)
    from RecExConfig.RecFlags import rec

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
    if (not 'InDetNewTrackingCuts' in dir()):
      printfunc ("InDetRec_jobOptions: InDetNewTrackingCuts not set before - import them now")
      from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
      if InDetFlags.doCosmics():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Cosmics")
      elif InDetFlags.doHeavyIon():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HeavyIon")
      elif InDetFlags.doSLHC():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("SLHC")
      elif InDetFlags.doMinBias():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("MinBias")        
      else:
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Offline")
    InDetNewTrackingCuts.printInfo()
    # --- backward compatible
    InDetCutValues = InDetNewTrackingCuts

    #
    # --- setup of cut values for  Primary vertexing  
    #
    if (not 'InDetPrimaryVertexingCuts' in dir()):
      printfunc ("InDetRec_jobOptions: InDetPrimaryVertexingCuts not set before - import them now")
      from InDetRecExample.ConfiguredVertexingCuts import ConfiguredVertexingCuts
      if InDetFlags.primaryVertexCutSetup() == "StartUp":
        InDetPrimaryVertexingCuts      = ConfiguredVertexingCuts("StartUp")
      
      elif InDetFlags.primaryVertexCutSetup() == "NoLumi":
        InDetPrimaryVertexingCuts      = ConfiguredVertexingCuts("NoLumi")
      
      else:
        InDetPrimaryVertexingCuts      = ConfiguredVertexingCuts("PileUp")
      InDetPrimaryVertexingCuts.printInfo()

    # ----------- 
    #
    # --- setup key aliases to pass to configurables
    #
    InDetSpSeededTracksKey    = InDetKeys.SiSpSeededTracks()
    InDetResolvedTracksKey    = InDetKeys.ResolvedTracks() 
    InDetExtendedTracksKey    = InDetKeys.ExtendedTracks()
    InDetExtendedTracksMapKey = InDetKeys.ExtendedTracksMap()  
    
    if InDetFlags.doSLHC():
      InDetSpSeededTracksKey    = InDetKeys.SiSpSeededSLHCTracks() 
      InDetResolvedTracksKey    = InDetKeys.ResolvedSLHCTracks() 
      InDetExtendedTracksKey    = InDetKeys.ExtendedSLHCTracks()
      InDetExtendedTracksMapKey = InDetKeys.ExtendedTracksMapSLHC()              
      
    if globalflags.InputFormat() == 'bytestream':
      ServiceMgr.ByteStreamCnvSvc.IsSimulation = (globalflags.DataSource() == 'geant4')
              
    # ------------------------------------------------------------
    # 
    # ----------- Configuring the conditions access
    #
    # ------------------------------------------------------------
    
    include ("InDetRecExample/InDetRecConditionsAccess.py")

    # ------------------------------------------------------------
    # 
    # ----------- Loading the Tracking Tools and Services 
    #
    # ------------------------------------------------------------
    
    include ("InDetRecExample/InDetRecLoadTools.py")
    
    # ------------------------------------------------------------
    #
    # ----------- Data-Preparation stage
    #
    # ------------------------------------------------------------
    # --- silicon
    include ("InDetRecExample/InDetRecPreProcessingSilicon.py")
    include ("InDetRecExample/ConfiguredInDetPreProcessingTRT.py")
    # --- TRT, no drift information if cosmics, do not use extrenal phase in any case
    InDetPreProcessingTRT = ConfiguredInDetPreProcessingTRT(not InDetFlags.doTRTPhaseCalculation() or jobproperties.Beam.beamType()=="collisions",False)

    # ------------------------------------------------------------
    #
    # --- some collection lists (bookkeeping)
    #
    # ------------------------------------------------------------

    # track collection keys for statistics
    if not 'TrackCollectionKeys' in dir():
      TrackCollectionKeys        = []
    if not 'TrackCollectionTruthKeys' in dir():
      TrackCollectionTruthKeys   = []

    # NewTracking collection keys
    InputCombinedInDetTracks = []
    # CTBTracking collection keys
    InputCTBTrackCollections = []


    # ------------------------------------------------------------
    #
    # ----------- Merge track collections
    #
    # ------------------------------------------------------------

    # always merge track collections. even if only one collection ran!
    # (this is because the track merger fills the prd association tool!)
    # *ME* fix, only merge if more than 1 track collection
    if InDetFlags.doNewTrackingPattern() or InDetFlags.doBeamHalo():
      from TrkTrackCollectionMerger.TrkTrackCollectionMergerConf import Trk__TrackCollectionMerger
      TrkTrackCollectionMerger = Trk__TrackCollectionMerger(name                    = "InDetTrackCollectionMerger",
                                                            TracksLocation          = InputCombinedInDetTracks,
                                                            OutputTracksLocation    = InDetKeys.UnslimmedTracks(),
                                                            AssoTool                = InDetPrdAssociationTool,
                                                            SummaryTool             = InDetTrackSummaryToolSharedHits)
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
          
    # ------------------------------------------------------------
    #
    # ----------- prepare output track collection for post processing
    #
    # ------------------------------------------------------------

    #
    # -- Pick one of the result collections and turn it into tracks
    #
    #if InDetFlags.doNewTrackingPattern():
    #  InputTrackCollection = InDetKeys.UnslimmedTracks()
    #elif InDetFlags.doCTBTracking():
      #InputTrackCollection = InDetKeys.UnslimmedTracks_CTB()
    #else:
      # --- in case of reading from ESD, so we just set the Collection and truth
      #InputTrackCollection      = InDetKeys.Tracks()
      #InputTrackCollectionTruth = InDetKeys.TracksTruth()

    InputTrackCollection      = FatrasKeyFlags.RefittedTrackCollection()
    InputTrackCollectionTruth = FatrasKeyFlags.RefittedTrackCollection()+'Truth'
    TrackCollectionKeys      += [ InputTrackCollection ]

    from FatrasExample.FatrasKeys import FatrasKeyFlags
    include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
    InDetTracksTruth = ConfiguredInDetTrackTruth(FatrasKeyFlags.RefittedTrackCollection(),
                                                 FatrasKeyFlags.RefittedTrackCollection()+'DetailedTruth',
                                                 FatrasKeyFlags.RefittedTrackCollection()+'Truth')

    TrackCollectionTruthKeys += [ InputTrackCollectionTruth ]



        
    # ------------------------------------------------------------
    #
    # ----------- now we do post-processing
    #
    # ------------------------------------------------------------
    #
    #
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
      else:
        cuts = InDetNewTrackingCuts
      include("InDetRecExample/ConfiguredInDetValidation.py")
      InDetValidation = ConfiguredInDetValidation("",True,InDetFlags.doTruth(),cuts,TrackCollectionKeys,TrackCollectionTruthKeys)

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
