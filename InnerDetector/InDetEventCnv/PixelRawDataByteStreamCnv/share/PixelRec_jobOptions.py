# +++++++++++++++++++ beginning of InDetRec_jobOptions.py
# jobOptions Fragment for ID software
# -----------------------------------
# edward.moyse@cern.ch
# markus.elsing@cern.ch
# -----------------------------------

# +++++++++++++++++++++++
# Control 
# if needed switches are not defined, set them to sensible defaults
# +++++++++++++++++++++++

if not 'InDetFlags' in dir():
   #
   # --- setup flags with default values
   #
   print "InDetRec_jobOptions: InDetFlags not set - setting to defaults"
   include ( "InDetRecExample/ConfiguredInDetFlags.py" )
   if not 'InDetFlagsJobProperty' in dir():
     from InDetRecExample.InDetJobProperties import InDetFlagsJobProperty
   InDetFlags = ConfiguredInDetFlags(InDetFlagsJobProperty)
#
# ----------- printout the setup
#
InDetFlags.printInfo()

if not 'InDetCutValues' in dir():
   #
   # --- setup physics cuts with default values
   #
   print "InDetRec_jobOptions: InDetCutValues not set - setting to defaults"
   include ( "InDetRecExample/ConfiguredInDetCutValues.py" )
   InDetCutValues = ConfiguredInDetCutValues()
#
# ----------- printout the setup
#
InDetCutValues.printInfo()

if not 'InDetKeys' in dir():
   #
   # --- setup StoreGate keys (JobProperties!)
   #
   print "InDetRec_jobOptions: InDetKeys not set - setting to defaults"
   from InDetRecExample.InDetKeys import InDetKeys
   InDetKeys.lock_JobProperties()
#
# ----------- printout the setup
#
InDetKeys.print_JobProperties()

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

include ("InDetRecExample/InDetRecPreProcessing.py")

# ------------------------------------------------------------
#
# --- the name of the input for the next algorithm
#
# ------------------------------------------------------------
InputTrackCollection       = ""
# next needs to be defined always otherwise there is a problem 
# a bit further down when one runs with ReadInDet_jobOptions.py
ForwardTrackCollection     = "" 
if not 'TrackCollectionKeys' in dir():
   TrackCollectionKeys        = []
if not 'TrackCollectionTruthKeys' in dir():
   TrackCollectionTruthKeys   = []



# ------------------------------------------------------------
#
# ----------- now we do the RTF tracking
#
# ------------------------------------------------------------

if InDetFlags.doNewTracking():
   include ("InDetRecExample/InDetRecNewTracking.py")
if InDetFlags.doTrtSegments():
   include ("InDetRecExample/InDetRecTRTSegmentFinding.py")
if InDetFlags.doBackTracking():
   include ("InDetRecExample/InDetRecBackTracking.py")
if InDetFlags.doTRTStandalone():
   include ("InDetRecExample/InDetRecTRTStandalone.py")

if not InDetFlags.doAddCollections():
   # --- final new tracking collection 
   NewTrackingCollection = ForwardTrackCollection
else :
   # Vector of track collections to be merged for further analysis
   InputCombinedInDetTracks = [ ForwardTrackCollection, ResolvedSiExtendedTracks, OutputTRTStandaloneTracks ]
   # Output key for the finder
   OutputCombinedInDetTracks = "CombinedInDetTracks"
   
   from TrkTrackCollectionMerger.TrkTrackCollectionMergerConf import Trk__TrackCollectionMerger
   TrkTrackCollectionMerger = Trk__TrackCollectionMerger(name                    = "TrkTrackCollectionMerger",
                                                         TracksLocation          = InputCombinedInDetTracks,
                                                         OutputTracksLocation    = OutputCombinedInDetTracks,
                                                         MaxNumSharedHits        = InDetCutValues.maxShared()+1)
   topSequence += TrkTrackCollectionMerger
   print          TrkTrackCollectionMerger
   
   #
   # ------------ Track truth.
   #
   if InDetFlags.doTruth():
      #
      # set collection name for truth
      #
      CombinedInDetTracksTruth   = "CombinedInDetTracksTruthCollection"
      CombinedDetailedInDetTrackTruth = "CombinedInDetTracksDetailedTruth"
      #
      # set up the truth info for this container
      #
      include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
      InDetTracksTruth = ConfiguredInDetTrackTruth(OutputCombinedInDetTracks,
                                                   CombinedDetailedInDetTrackTruth,
                                                   CombinedInDetTracksTruth)
      #
      # add final output for statistics
      #
      TrackCollectionKeys      += [ OutputCombinedInDetTracks ]
      TrackCollectionTruthKeys += [ CombinedInDetTracksTruth ]

   # --- final new tracking collection 
   NewTrackingCollection = OutputCombinedInDetTracks
   
# ------------------------------------------------------------
#
# ----------- now we copy over the output track collection
#
# ------------------------------------------------------------

if InDetFlags.doNewTracking():
   #
   # Pick one of the result collections and turn it into tracks
   #
   if InDetFlags.doNewTracking():
      InputTrackCollection = NewTrackingCollection
   #
   # --- we either refit or copy the input
   #
   if InDetFlags.doRefit():
      #
      # set output track name
      #
      OutputTrackCollection = "RefittedTracks"
      #
      # @TODO set SummaryTool property
      from InDetRecExample.TrackingCommon import getInDetPRDtoTrackMapToolGangedPixels
      from TrkRefitAlg.TrkRefitAlgConf import Trk__ReFitTrack
      InDetReFitTrack = Trk__ReFitTrack (name           = "InDetRefitTrack",
                                         FitterTool     = InDetTrackFitter,
                                         AssociationTool= getInDetPRDtoTrackMapToolGangedPixels(),
                                         TrackName      = InputTrackCollection,
                                         NewTrackName   = OutputTrackCollection,
                                         fitRIO_OnTrack = InDetFlags.refitROT())
      if InDetFlags.materialInteractions():
         InDetReFitTrack.matEffects = 3 # default in code is 4!!
      else :
         InDetReFitTrack.matEffects = 0

      topSequence += InDetReFitTrack
      print          InDetReFitTrack
      # set input track name
      InputTrackCollection = OutputTrackCollection
      
   #
   # set output track name
   #
   OutputTrackCollection = InDetKeys.Tracks()
   #
   # --- slimm the tracks down before writing them
   #
   if InDetFlags.doSlimming():
      from TrkTrackSlimmingTool.TrkTrackSlimmingToolConf import Trk__TrackSlimmingTool as ConfigurableTrackSlimmingTool
      InDetTrkSlimmingTool = ConfigurableTrackSlimmingTool(name           = "InDetTrackSlimmingTool",
                                                           KeepParameters = True,
                                                           KeepOutliers   = True )
      ToolSvc += InDetTrkSlimmingTool
      print      InDetTrkSlimmingTool
      
      from TrkTrackSlimmer.TrkTrackSlimmerConf import Trk__TrackSlimmer as ConfigurableTrackSlimmer
      InDetTrkSlimmer = ConfigurableTrackSlimmer(name                 = "InDetTrackSlimmer",
                                                 TrackLocation        = [ InputTrackCollection ],
                                                 SlimmedTrackLocation = [ OutputTrackCollection ],
                                                 TrackSlimmingTool    = InDetTrkSlimmingTool)
      #InDetTrkSlimmer.OutputLevel=VERBOSE
      topSequence += InDetTrkSlimmer
      print          InDetTrkSlimmer
      # --- for output
      InDetKeys.AliasToTracks = 'none'
                                                                              
   else:
      #
      # configure Algorithm to create output alias
      #
      from TrkCollectionAliasAlg.TrkCollectionAliasAlgConf import Trk__TrkCollectionAliasAlg
      InDetCopyAlg = Trk__TrkCollectionAliasAlg (name             = "InDetCopyAlg",
                                                 CollectionName   = InputTrackCollection,
                                                 AliasName        = InDetKeys.Tracks()) 
      topSequence += InDetCopyAlg
      print          InDetCopyAlg
      # --- for output
      InDetKeys.AliasToTracks = InputTrackCollection
   #
   # Input of the next algorithm
   #
   InputTrackCollection = OutputTrackCollection
   #
   # now do truth for the output
   #
   if InDetFlags.doTruth():
      #
      # this is the name of the truth
      #
      InputDetailedTrackTruth   = InDetKeys.DetailedTracksTruth()
      InputTrackCollectionTruth = InDetKeys.TracksTruth()
      #
      # set up the truth info for this container
      #
      include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
      InDetTracksTruth = ConfiguredInDetTrackTruth(InputTrackCollection,
                                                   InputDetailedTrackTruth,
                                                   InputTrackCollectionTruth)
      #
      # add to keys lists for statistics
      #
      TrackCollectionKeys      += [ InputTrackCollection ]
      TrackCollectionTruthKeys += [ InputTrackCollectionTruth ]
      
# ------------------------------------------------------------
#
# ----------- now we do post-processing
#
# ------------------------------------------------------------
#
#
include("InDetRecExample/InDetRecPostProcessing.py")
   
# +++++++++++++++++++ end of InDetRec_jobOptions.py
   
