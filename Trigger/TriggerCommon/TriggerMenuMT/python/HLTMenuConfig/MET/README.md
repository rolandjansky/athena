Modules in this directory
-----

* [GenerateMETChainDefs](GenerateMETChainDefs.py)
  * Called by the menu code to pass the chain dict into the ChainConfiguration object
* [METChainConfiguration](METChainConfiguration.py)
  * Defines the ChainConfiguration object that interprets the chain dict and builds the chain
* [METMenuSequences](METMenuSequences.py)
  * Defines the top-level sequences containing the input maker and hypothesis alg
* [METRecoSequences](METRecoSequences.py)
  * Assembles the sequences for each stage of reconstruction
* [ConfigHelpers](ConfigHelpers.py)
  * Base configuration class for generating MET reco algorithm configuration
* [AlgConfigs](AlgConfigs.py)
  * Specifies configuration of the MET reco algorithms
* [AlgInputConfig](AlgInputConfig.py)
  * Helpers for creating reco sequences to generate the inputs to MET reco
* [generateMET](generateMET.py)
  * Prototype for NewJO chain configuration