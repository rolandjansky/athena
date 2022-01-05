Modules in this directory
-----

- [**python/TrigBjetBtagHypoTool**](python/TrigBjetBtagHypoTool.py)
  * Input: Chain-Dictionary
  * Interpretes the chain dictionary and configures the TrigBjetBtagHypoTool
  * Output: TrigBjetBtagHypoTool
- [**python/TrigBjetMonitoringConfig**](python/TrigBjetMonitoringConfig.py)
  * **TrigBjetBtagHypoToolMonitoring**: Class for monitoring b-tagging probabilities
  * **TrigBjetOnlineMonitoring**: Class for monitoring all quantities related to b-tagging


* [**src/TrigBjetBtagHypoAlg**](src/TrigBjetBtagHypoAlg)
  * Main Hypothesis Algorithm
  * Retrieves collections from views
  * Retrieves previous decisions
  * Calls _TrigBjetBtagHypoTool_ for testing b-tag hypo
  * Stores output decisions
  * Monitors Jets, tracks, flavour probabilities, PV, and low-level b-tagging quantities
* [**src/TrigBjetBtagHypoTool**](src/TrigBjetBtagHypoTool)
  * Decides whether a jet passes the b-tagging requirement
* [**src/TrigBjetBtagHypoAlgBase**](src/TrigBjetBtagHypoAlgBase)
  * Helper functions to retrieve
    * previous decision container
    * collections and obejcts from views, StoreGate, Navigation
    * attach object links to decisions

A more detailed documentation of the TrigBjetHypo package can be found at\
[Trigger/TriggerCommon/TriggerMenuMT/python/HLT/Bjet/docs/bjet_configuration.md](https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TriggerCommon/TriggerMenuMT/python/HLT/Bjet/docs/bjet_configuration.md#trigbjethypo)



