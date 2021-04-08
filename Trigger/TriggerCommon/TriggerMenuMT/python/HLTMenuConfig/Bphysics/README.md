Modules in this directory
-----

* [GenerateBphysicsChainDefs](GenerateBphysicsChainDefs.py)
  * Called by the menu code to pass the chain dict into the ChainConfiguration object
* [BphysicsChainConfiguration](BphysicsChainConfiguration.py)
  * Defines the ChainConfiguration object that interprets the chain dict and builds the chain
* [BphysicsMenuSequences](BphysicsMenuSequences.py)
  * Defines the top-level sequences containing the input maker and hypothesis alg
* [BphysicsRecoSequences](BphysicsRecoSequences.py)
  * Assembles the sequences for each stage of reconstruction
* [generateBphysics](generateBphysics.py)
  * Prototype for NewJO chain configuration