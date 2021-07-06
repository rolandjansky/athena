Modules in this directory
-----

* [GenerateMuonChainDefs](GenerateMuonChainDefs.py)
  * Called by the menu code to pass the chain dict into the ChainConfiguration object
* [MuonChainConfiguration](MuonChainConfiguration.py)
  * Defines the ChainConfiguration object that interprets the chain dict and builds the chain
* [MuonMenuSequences](MuonMenuSequences.py)
  * Defines the top-level sequences containing the input maker and hypothesis alg
* [MuonRecoSequences](MuonRecoSequences.py)
  * Assembles the sequences for each stage of reconstruction
* [generateMuon](generateMuon.py)
  * Prototype for NewJO chain configuration