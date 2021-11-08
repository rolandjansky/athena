Modules in this directory
-----

* [GenerateElectronChainDefs](GenerateElectronChainDefs.py), [GeneratePhotonChainDefs](GeneratePhotonChainDefs.py)
  * Called by the menu code to pass the chain dict into the ChainConfiguration object
* [ElectronChainConfiguration](ElectronChainConfiguration.py), [PhotonChainConfiguration](PhotonChainConfiguration.py)
  * Defines the ChainConfiguration object that interprets the chain dict and builds the chain
* [ElectronMenuSequences](ElectronMenuSequences.py), [PhotonMenuSequences](PhotonMenuSequences.py)
  * Defines the top-level sequences containing the input maker and hypothesis alg
* [PrecisionCaloMenuSequences.py](PrecisionCaloMenuSequences.py)
  * Assembles the menu sequence for the precision calo reconstruction step
* [PrecisionTrackingMenuSequences.py](PrecisionTrackingMenuSequences.py)
  * Assembles the menu sequence for the precision track reconstruction step
* [PrecisionElectronMenuSequences.py](PrecisionElectronMenuSequences.py)
  * Assembles the menu sequence for the precision electron reconstruction step
* [PrecisionElectronMenuSequences_GSF.py](PrecisionElectronMenuSequences_GSF.py)
  * Assembles the menu sequence for the precision electron reconstruction step with Gaussian Sum Filter tracks
* [TrigEgammaDefs](TrigEgammaDefs.py)
  * Common definitions
* [TrigEgammaKeys](TrigEgammaKeys.py)
  * Configuration of container names for HLT electrons, HLT photons and identification working points for Cut=Based selection, likelihood, Dnn and Ringer
* [TrigEgammaFactories](TrigEgammaFactories.py)
  * ToolFactories to configure common TrigEgamma Tools
