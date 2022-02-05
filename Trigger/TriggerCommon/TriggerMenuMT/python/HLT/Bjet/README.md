Modules in this directory
-----

* [**GenerateBjetChainDefs**](GenerateBjetChainDefs.py)
  * **Input**: Chain-dictionary (Called by the menu code)
  * **Assembles the full Bjet-chain**
    * Creates a chain-dictionary for each chain-part
    * Adds Jet-steps and, if needed, add Bjet-steps by passing the new chain-dictionaries to `BjetChainConfiguration`
    * Merges steps from all chain-parts  
  * **Output**: Full Bjet-chain
* [**BjetChainConfiguration**](BjetChainConfiguration.py)
  * **Input**: Chain-dictionary 
  * **Defines the ChainConfiguration object that interprets the chain-dict and builds the chain**
    * Retrieves steps from `BjetMenuSequences`
  * **Output**: Chain (of one chain-part)
* [**BjetMenuSequences**](BjetMenuSequences.py)
  * **Input**: Jet-Collection name
  * **Defines the top-level sequences**
    * Defines the input maker and creates one view per jet
    * Calls `BjetTrackingConfiguration` to run tracking inside view
    * Calls `BjetFlavourTaggingConfiguration` to run b-tagging inside view
    * Defines the hypothesis algorithms
  * **Output**: MenuSequence
* [**BjetTrackingConfiguration**](BjetTrackingConfiguration.py)
  * **Input**: RoIs, Dataobjects(PV, Jets)
  * **Configures second stage b-jet tracking sequence**
    * ID trigger: Second stage of fast tracking
    * ID trigger: Precision tracking 
  * **Output**: Sequence of tracking algorithms and precision tracking TrackParticles
* [**BjetFlavourTaggingConfiguration**](BjetFlavourTaggingConfiguration.py)
  * **Input**: Jet-Collection name, PV name, Track-Collection name, Output BTagging-Collection name (Muon-Collection name) 
  * **Configures offline flavour-tagging algorithms in ComponentAccumulator**
    * Calls `JetSecVtxFindingAlg` and `JetSecVertexingAlg` (Secondary-Vertexing)
    * Calls `JetBTaggingAlg` (Run2 taggers)
    * Calls `BTagTrackAugmenterAlg` and `BTagHighLevelAugmenterAlg` (Jet and track augmenter)
    * Calls `HighLevelBTagAlg` (Run3 taggers)
  * **Output**: ComponentAccumulator containing flavour-tagging algorithms and output BTagging-Collection name
* [**generateBjet**](generateBjet.py)
  * **Prototype for NewJO chain configuration**
