# Introduction
TrigFastTrackFinder performs fast track finding for the High Level Trigger using pixel and SCT information The tracks reconstructed by TrigFastTrackFinder can subsequently be refined by Precision Tracking that performs track ambiguity resolution and track extension to the TRT. TrigFastTrackFinder takes as input Pixel and SCT clusters and SpacePoints. Track reconstruction is performed in Regions of Interest (RoI) that can be encompass the whole detector (FullScan) or a smaller region in eta, phi and z. 

First track seeds are created from triplets of SpacePoints. This is followed by Track Following that extends seeds to the other silicon layers, and Track Fitting.

According to configuration, the seeds considered can be any of pixel-only (PPP), SCT only (SSS), or mixed (if TripletDoPPS is set True in [ConfigSettings.py](../../../../TrigTool/TrigInDetConfig/python/ConfigSettings.py). In principle PSS mixed seeds can also be included (if TripletDoPSS=True), but this is disabled in [TrigFastTrackFinderConfig.py](../python/TrigFastTrackFinder_Config.py).

## Special Reconstruction Modes for Unconventional Track Triggers:
* **LRT_mode=True**: Configuration used for Large Radius Tracking to reconstruct track starting at large radius i.e. from ~10mm to ~300mm. It configures the seedmaker to only use SSS seeds. If an input track collection is provided, it uses this to remove the already used clusters from the pool of clusters used by track seeding. This supports a workflow where standard tracking is followed by LRT tracking running on the unused clusters. 
* **doHitDV=True**: Uses spacepoints to search for a displaced vertex.
* **dodEdxTrk=True**: Adds TrackStateOnSurface with dE/dx information to tracks. 
* **doDisappearingTrk=True**: Configuration to reconstruct Disappearing Tracks i.e. short tracks starting at the IP but ending within the volume of the Si tracker. The trackMaker tool is configured to return information for failed tracks.

## Special Reconstruction Mode for Beamspot and Minbias
* **doZFinder=True**: Used for beamspot and minbias trigger chains. The [IDScanZFinder](../../../../Trigger/TrigTools/IDScanZFinder) uses pixel spacepoint to find the z coordinates of primary vertices.  Only the N highest occupancy vertices are returned by the IDScanZFinder, where N is determined by the NumberOfPeaks property of the IDScanZFinder that is set in [TrigFastTrackFinder_Config.py](../python/TrigFastTrackFinder_Config.py). 
* **doFastZVertexSeeding=True**: In the BeamSpot trigger chains, the z co-ordinate of the primary vertices (created for doZFinder=True) are used as input to seed making. Only seeds compatible with one of the primary vertices are kept, speeding up track reconstruction. 
* **doZFinderOnly=True**: Used in conjunction with doZFinder=True. The IDScanZfinder is executed and primary vertex information stored in a TrigVertexCollection. The subsequent tracking steps (seeding, following, fitting) are skipped. **Note: the TrigVertexCollection is not currently output**. 

## Special Reconstruction mode using GPU Acceleration 
* **useGPU=True**: GPU acceleration is used for Track Seeding

# Inputs and Outputs

The main inputs (excluding conditions inputs) are: 
* InDet::PixelClusterContainer Pixel Clusters
* SCT_ClusterContainer: SCT Clusters
* SpacePointContainer: Pixel SpacePoints
* SpacePointContainer: SCT SpacePoints
* TrigRoiDescriptorCollection: RoI Descriptors

The main output is:
* TrackCollection : Output tracks

Configuration-dependant inputs:
* Trk::PRDtoTrackMap : map of clusters to tracks 
* TrackCollection InputTracks : Tracks produced by a preceeding tracking step used when LRT_mode=True
* DataVector< LVL1::RecJetRoI > RecJetRoI: Jet RoI information used when doHitDV_Seeding=True

Configuration-dependant outputs:
* xAOD::TrigCompositeContainer> HitDVTrk and xAOD::TrigCompositeContainer> HitDVSP filled when doHitDV=True
* xAOD::TrigCompositeContainer> EdxTrk and xAOD::TrigCompositeContainer> dEdxHit filled when dodEdxTrk=True
* xAOD::TrigCompositeContainer> DisTrkCand filled when doDisappearingTrk=True

# Algorithm Methods
## Athena methods
* **initialize** : Retrieve Tools. In addition, optionally (useGPU=True) retrieve the GPU acceleration service and acceleration tool.
* **start** : Initalises information needed by Track Seeding.
* **execute** : Retrieves the RoIDescriptors and optionally the input tracks. Calls the findTracks method to perform tracking.
* **finalize** : Outputs information on the number RoI processed and number of tracks found to the log file. 

## findTracks method
The findTracks method performs track reconstruction in the following stages. 
* **1: Retrieve SpacePoints**
  * Optionally Fill Cluster map (for LRT_Mode = True and valid ReadHandle for inputTracksName) : fills the siClusterMap with a list of clusters that have already be used to create the tracks in the input collection so that these clusters can be excluded from seedmaking.
  * call the getSpacePoints method of the [TrigOnlineSpacePointTool](../../../../Trigger/TrigTools/TrigOnlineSpacePointTool) that retrieves the input SpacePoint collection and converts it to a vector of TrigSiSpacePoint
    * Processing continues if there are at least *minHits* spacepoints 
* **2: ZFinder** (optional)
  * Optionally (doZFinder=True) use the [IDScanZFinder](../../../../Trigger/TrigTools/IDScanZFinder) to find the z coordinate of primary vertices that are used as input to seed making. The IDScanZfinder has a property NumberOfPeaks that determines how many primary vertices are returned by the tool. 
  * Optionally (doFastZVertexSeeding=True) RoI, internal to TrigFastTrackFinder, are created for each primary vertex with a reduced z extent of +/- 7 mm. 

* **3: Track-Seeding**
  * create track-seeds consisting of a triplet of spacepoints. Track-seeds are created by the [TrigTrackSeedGenerator](../../../../Trigger/TrigTools/TrigInDetPattRecoTools/src/TrigTrackSeedGenerator.cxx) tool or, optionally for *useGPU*=True, on GPU using the acceleration tool and acceleration service that are called from the makeSeedsOnGPU method

* **4: Combinatorial Tracking**
  *   Iterates over the track seeds (triplets of spacepoints) and calls the getTracks method of the Track Maker Tool ([SiTrackMakerTool_xk](../../../../InnerDetector/InDetRecTools/SiTrackMakerTool_xk)) for each seed to perform track-following to try to extend the track to other Silicon layers. Optionally (doSeedRedundancyCheck=True) the triplet is first checked to see if any of the clusters are on a previously reconstructed track, in which case the seed is skipped. If the track extension is successfull, the returned tracks are stored. Tracks failing a d0 requirement (abs(d0)<=initialD0Max) are flagged for later removal. In Disappearing Track mode (doDisappearingTrk=True), information from failed track extensions is processed and stored. 
   * Optionally (doCloneRemoval=True) after all seeds have been processed, tracks are filtered by the filterSharedTracks method which sorts  with less than FreeClustersCut unique (unshared) clusters are filtered by the filterSharedTracks method. In Disappearing Track mode (doDisappearingTrk=True), the disappearing track candidates are also filtered by the filterSharedDisTracks method. 

* **5: Track Fitting**
   * Perform trackfitting using the Track Fitting Tool
   * optionaly (dodEdxTrk=true) call the finddEdxTrk method to fill dEdx information to the corresponding output TrigCompositeContainer.

* **6: Optional UTT processing** 
    * Optionally (doDisappearingTrk=True) call the recoVertexForDisTrack and findDisTracks methods to fill the corresponding output TrigCompositeContainer
    * Optionally (doHitDV) call the findHitDV method to fill the corresponding output TrigCompositeContainer

* **7: Fill Monitoring**
    * call the fillMon method to fill monitoring information.

## Helper methods for standard tracking
* filterSharedTracks: Removes tracks sharing hits based on a cut on the number of un-shared clusters: FreeClustersCut 
* trackQuality
* updateClusterMap: update the internal map with assignment of clusters to reconstructed tracks
* extractClusterIds: extract cluster ids from spacepoints. Used to check for redundant seeds 
* usedByAnyTrack:determines if the clusters in a seed have already been used by a track.
* getBeamSpot: gets the coordinates of the beamspot
## Monitoring
* fillMon: fills monitoring information
* runResidualMonitoring: optionally (doResMonitroing=True) executes the getUnbiasedResiduals method of the [TrigInDetTrackFitter](../../../TrigTools/TrigInDetTrackFitter) to get unbiased residuals and fill monitoring information

## Acceleration
* makeSeedsOnGPU: performs seedmaking on GPU using the [TrigInDetAccelerationTool](../../../TrigAccel/TrigInDetAccel/TrigInDetAccelerationTool)

## Unconventional Track Triggers
* createEmptyUTTEDMs : Creates empty collections for all UTT triggers
### Disappearing Tracks
* recoVertexForDisTrack : reconstruct vertex for Disappearing track search
  * isGoodForDisTrackVertex
* findDisTracks : perform disapearing tracks reconstuction
  * fillDisTrkCand
  * recoAndFillDisTrkCand
  * disTrk_refit
  * isCleaningPassDisTrack
  * getTrkBarrelLayerInfo
  * disTrackQuality
  * filterSharedDisTracks
  * isPreselPassDisTrack
  * extrapolateDisTrackToBS
  * getDisTrkCategory
  * print_disTrk
## Displaced Vertex
 * findHitDV : find Hit-based displaced vertices
   * findSPSeeds  
   * getSPLayer	
   * deltaR	
## dE/dX Trigger
   * finddEdxTrk : find tracks with large dE/dx 
     * dEdx

