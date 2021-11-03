Modules in this directory
-----

* [GenerateUnconventionalTrackingChainDefs](GenerateUnconventionalTrackingChainDefs.py)
  * Called by the menu code to pass the chain dict into the ChainConfiguration object

* [UnconventionalTrackingChainConfiguration](UnconventionalTrackingChainConfiguration.py)
  * Defines the ChainConfiguration object that interprets the chain dict and builds the chain
  * Makes use of respective chain sequence configurator, depending on the type of unconventional trigger triggers specified in the `extra` field.

     |extra field|description|
     |:---|:---|
     | *isohpttrack* | Isolated high-pt track trigger, sequence given by `IsoHPtTrackTriggerHypoSequence` |
     | *fslrt* | Full-scan LRT trigger, sequence given by `FullScanLRTTriggerMenuSequence` |
     | *distrk* | Disappearing track trigger, sequence given by `DisTrkTriggerHypoSequence` |
     | *hitdv* | Hit-based displaced vertex trigger, sequence given by `HitDVHypoSequence` |
     | *dedx* | Large-dE/dx trigger, sequence given by `dEdxTriggerHypoSequence` |
     
     
  * The further fields (`IDinfo`, `isoinfo`) of the chain dictionary are used when configuring hypo alg/tool in the confugrators located under `Trigger/TrigHypothesis/TrigLongLivedParticlesHypo/python`.

* [IsoHPtTrackTriggerHypoSequence](IsoHighPtTrackTriggerConfiguration.py)
  * Privides isolated high-pt track trigger hypo chain sequence

* [FullScanLRTTriggerMenuSequence](FullScanLRTTrackingConfiguration.py)
  * Privides full-scan large-radius-tracking (LRT) track trigger chain sequence

* [DisTrkTriggerHypoSequence](DisTrkTriggerConfiguration.py)
  * Privides disappearing track trigger hypo chain sequence

* [HitDVHypoSequence](HitDVConfiguration.py)
  * Privides hit-based displaced vertex trigger hypo chain sequence

* [dEdxTriggerHypoSequence](dEdxTriggerConfiguration.py)
  * Privides large-dE/dx track trigger hypo chain sequence


