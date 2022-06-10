Modules in this directory
-----

* [GenerateUnconventionalTrackingChainDefs](GenerateUnconventionalTrackingChainDefs.py)
  * Called by the menu code to pass the chain dict into the ChainConfiguration object

* [UnconventionalTrackingChainConfiguration](UnconventionalTrackingChainConfiguration.py)
  * Defines the ChainConfiguration object that interprets the chain dict and builds the chain
  * Makes use of respective chain sequence configurator, depending on the type of unconventional triggers.

     |trigger type|description|
     |:---|:---|
     | *isotrk* | Isolated high-pt track trigger, sequence given by `IsoHPtTrackTriggerHypoSequence` |
     | *distrk* | Disappearing track trigger, sequence given by `DisTrkTriggerHypoSequence` |
     | *dedxtrk* | Large-dE/dx trigger, sequence given by `dEdxTriggerHypoSequence` |
     | *hitdvjet* | Hit-based displaced vertex trigger, sequence given by `HitDVHypoSequence` |
     | *dispjet* | Displaced jet trigger, sequence given by `DJPromptStep`, `DJDispStep`, `DJEDStep` |
     | *fslrt* | Full-scan LRT trigger, sequence given by `FullScanLRTTriggerMenuSequence` |
     | *fsvsi* | Displaced vertex trigger, sequence given by `VrtSecInclusiveMenuSequence` |

     
  * The further fields (`IDinfo`, `isoinfo`, `addInfo`) of the chain dictionary are used when configuring hypo alg/tool in the confugrators located under `Trigger/TrigHypothesis/TrigLongLivedParticlesHypo/python`.

* [IsoHPtTrackTriggerHypoSequence](IsoHighPtTrackTriggerConfiguration.py)
  * Provides isolated high-pt track trigger hypo chain sequence

* [DisTrkTriggerHypoSequence](DisTrkTriggerConfiguration.py)
  * Provides disappearing track trigger hypo chain sequence

* [dEdxTriggerHypoSequence](dEdxTriggerConfiguration.py)
  * Provides large-dE/dx track trigger hypo chain sequence

* [HitDVHypoSequence](HitDVConfiguration.py)
  * Provides hit-based displaced vertex trigger hypo chain sequence

* [Displaced jet sequences](DJTriggerConfiguration.py)
  * Provides displaced jet trigger hypo chain sequence

* [FullScanLRTTriggerMenuSequence](FullScanLRTTrackingConfiguration.py)
  * Provides full-scan large-radius-tracking (LRT) track trigger chain sequence

* [VrtSecInclusiveMenuSequence](VrtSecInclusiveConfiguration.py)
  * Provides secondary vertexing (VSI) trigger chain sequence





