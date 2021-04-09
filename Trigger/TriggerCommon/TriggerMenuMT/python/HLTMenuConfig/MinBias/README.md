Modules in this directory
-----

* [GenerateMinBiasChainDefs](GenerateMinBiasChainDefs.py)
  * Called by the menu code to pass the chain dict into the ChainConfiguration object
* [MinBiasChainConfiguration](MinBiasChainConfiguration.py)
  * Defines the ChainConfiguration object that interprets the chain dict and builds the chain
* [MinBiasMenuSequences](MinBiasMenuSequences.py)
  * Defines the top-level sequences containing the input maker and hypothesis alg
* [MinBiasRecoSequences](MinBiasRecoSequences.py)
  * Assembles the sequences for each stage of reconstruction
* [generateMinBias](generateMinBias.py)
  * Prototype for NewJO chain configuration