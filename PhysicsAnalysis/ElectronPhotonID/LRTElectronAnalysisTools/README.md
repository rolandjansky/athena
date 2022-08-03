This package containers helpers for analyses using the Large Radius Tracking (LRT) electron collection.


The `ElectronLRTOverlapRemovalTool` is used within the `EgammaAnalysisAlgorithms/ElectronLRTMergingAlg` (a common CP algorithm) to address overlaps between standard and LRT electrons. It can also be operated in standalone mode in order to study the overlaps. This tool is designed to allow for multiple working points to be specified by Egamma, though at this point only a single baseline behavior exists.

ElectronLRTOverlapRemovalTool configuration:
* `overlapStrategy` option for future difference strategies
* `ORThreshold` delta R threshold for overlap check of clusters
* `ElectronLLHTool` electron ID tool used in overlap removal, only electrons passing this threshold are considered in the overlap removal

The method `checkOverlap()` takes in the standard and LRT electron collections and returns, via argument, a list of pointers to electrons failing the overlap removal.

The tool is not affected by any systematic variations nor defines any new variations.