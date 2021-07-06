Modules in this directory
-----

* [GenerateBjetChainDefs](GenerateBjetChainDefs.py)
  * Called by the menu code to pass the chain dict into the ChainConfiguration object
* [BjetChainConfiguration](BjetChainConfiguration.py)
  * Defines the ChainConfiguration object that interprets the chain dict and builds the chain
* [BjetMenuSequences](BjetMenuSequences.py)
  * Defines the top-level sequences containing the input maker and hypothesis alg
* [BjetTrackingConfiguration](BjetTrackingConfiguration.py)
  * Configures second stage b-jet tracking sequence
* [BjetFlavourTaggingConfiguration](BjetFlavourTaggingConfiguration.py)
  * Configures flavour tagging algs in ComponentAccumulator
* [generateBjet](generateBjet.py)
  * Prototype for NewJO chain configuration