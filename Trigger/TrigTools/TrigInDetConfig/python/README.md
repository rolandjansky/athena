# Overview
The modules in this directory provide signature-specific configurations of ID algorithms to be included in trigger chains.
Inner Detector tracking consists of the algorithms, and their tools, that perform data preparation (bytestream conversions, clustering and space-point formation), Fast Tracking (seed-making, track-following and track fitting), Precision Tracking (ambiguity solving, track extension, track fitting) and primary vertex reconstruction. In most trigger chains, Precision Tracking starts from the tracks found by Fast Tracking. For specific cases, Precision Tracking can be configured to run EFIDTracking that starts from Raw Data and uses the SiSPSeededTrackFinder (rather than Fast Tracking) to find tracks.

In addition to the modules supporting the current jobOptions configuration, additional modules, [TrigInDetConfig](TrigInDetConfig.py) and [TrigTrackingCutFlags](TrigTrackingCutFlags.py), provide a parallel configuration within the ComponentAccumulator infrastucture.

# Configuration
* [ConfigSettingsBase](ConfigSettingsBase.py)
  * Base class for ConfigSettings   
* [ConfigSettings](ConfigSettings.py)
  * sets the signature-specific configuration settings including collection names, roi dimensions and algorithm properties
* [InDetTrigCollectionKeys](InDetTrigCollectionKeys.py) 
  * Defines the names of collections used internally within the Inner detector Trigger SW. It does not include the output track collection names that are set in [ConfigSettings](ConfigSettings.py).	
# Data Preparation and Fast Tracking
* [InDetSetup](InDetSetup.py)
  * Creates Data Preparation and Fast Tracking algorithms, configured based on the supplied config and returns the list of algorithms to be added to signature trigger chains
# Precision Tracking
* [InDetPT](InDetPT.py)       
  *  Creates Precision tracking algorithms that processes and refine tracks created by Fast Tracking. The algorithms are configured based on the supplied config. A list is returned of ID algorithms to be added to signature trigger chains
* [EFIDTracking](EFIDTracking.py)
  * Creates a sequence of Precision Tracking algorithms that starts from raw data using the SiSPSeededTrackFinder (rather than Fast Tracking) to find tracks.
* [InDetTrigCommon](InDetTrigCommonInDetTrigCommon.py)
  * Configures Precision Tracking algorithms and tools common to both InDetPT and EFIDTracking 
# Vertex Reconstruction
* [TrigInDetPriVtxConfig](TrigInDetPriVtxConfig.py)
  * Configures the primary vertex finding algorithms
# Component Accumulator
* [TrigInDetConfig](TrigInDetConfig.py)
  * ComponentAccumulator configuration of ID trigger algorithms and tools  
* [TrigTrackingCutFlags](TrigTrackingCutFlags.py)
  * Flags used by the ComponentAccumulator based configuration.  


