Modules in this directory
-----

* [GenerateBeamspotChainDefs](GenerateBeamspotChainDefs.py), [GenerateCalibChainDefs](GenerateCalibChainDefs.py), [GenerateCosmicChainDefs](GenerateCosmicChainDefs.py), [GenerateEnhancedBiasChainDefs](GenerateEnhancedBiasChainDefs.py), [GenerateMonitorChainDefs](GenerateMonitorChainDefs.py), [GenerateStreamingChainDefs](GenerateStreamingChainDefs.py)
  * Called by the menu code to pass the chain dict into the ChainConfiguration object
* [BeamspotChainConfiguration](BeamspotChainConfiguration.py), [CalibChainConfiguration](CalibChainConfiguration.py), [CosmicChainConfiguration](CosmicChainConfiguration.py), [EnhancedBiasChainConfiguration](EnhancedBiasChainConfiguration.py), [MonitorChainConfiguration](MonitorChainConfiguration.py), [StreamingChainConfiguration](StreamingChainConfiguration.py)
  * Defines the ChainConfiguration object that interprets the chain dict and builds the chain
