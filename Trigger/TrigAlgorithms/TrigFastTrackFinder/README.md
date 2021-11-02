## Overview
This Module contains the TrigFastTrackFinder Algorithm that performs Fast Track Reconstruction for the High Level Trigger. More details are given in the [TrigFastTrackFinderOverview](docs/TrigFastTrackFinderOverview.md)
Configured instances of TrigFastTrackFinder are provided to signatures by the [TrigInDetConfig](../../TrigTools/TrigInDetConfig/python) package. The README in that packackage gives details of how to include TrigFastTrackFinder and other Inner Detector algorithms in signature TriggerMenuMT sequences.
 
## Files in this Module
* Configuration: [TrigFastTrackFinder_Config.py](python/TrigFastTrackFinder_Config.py)    
* Algorithm implementation: [TrigFastTrackFinder.cxx](src/TrigFastTrackFinder.cxx), [TrigFastTrackFinder.h](src/TrigFastTrackFinder.h)
* Module Entries: [TrigFastTrackFinder_entries](src/components/TrigFastTrackFinder_entries.cxx)
* LUT Data file: [trigseed_ml_pixel_barrel_kde.lut](share/trigseed_ml_pixel_barrel_kde.lut) 
* Documentation: [TrigfastTrackFinderOverview](docs/TrigFastTrackFinderOverview)

## Links to Tool packages used by TrigFastTrackFinder
* [TrigInDetToolInterfaces](../../TrigTools/TrigInDetToolInterfaces)
* [IDScanZFinder](../../TrigTools/IDScanZFinder)
* [TrigOnlineSpacePointTool](../../TrigTools/TrigOnlineSpacePointTool)
* [TrigInDetPattRecoTools](../../TrigTools/TrigInDetPattRecoTools)
* [TrigInDetTrackFitter](../../TrigTools/TrigInDetTrackFitter)
* [SiTrackMakerTool_xk](../../../InnerDetector/InDetRecTools/SiTrackMakerTool_xk)
