Modules in this directory
-----

* [GenerateTauChainDefs](GenerateTauChainDefs.py)
  * Called by the menu code to pass the chain dict into the ChainConfiguration object
* [TauChainConfiguration](TauChainConfiguration.py)
  * Defines the ChainConfiguration object that interprets the chain dict and builds the chain
* [TauMenuSequences](TauMenuSequences.py)
  * Defines the top-level sequences containing the input maker and hypothesis alg
* [TauRecoSequences](TauRecoSequences.py)
  * Assembles the sequences for each stage of reconstruction
* [generateTau](generateTau.py)
  * Prototype for NewJO chain configuration