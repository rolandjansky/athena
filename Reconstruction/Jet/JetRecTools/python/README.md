# JetRecTools modules

## Module list

* `ConstModHelpers.py` -- Helper functions for setting up jet/MET reco with constituent modifications
* `ScheduleClusterOriginCorrection.py` -- Scheduler for topocluster origin correction
* `JetRecToolsConfig.py` -- Tool manager for constituent modifiers
* `DefaultTools.py` -- [Unused] from an attempt to modularise jet tool configurations

## Module descriptions

### `ConstModHelpers.py`

* This module provides helper functions for reconstructing jet collections and/or MET with modified constituents as inputs, making use of the `ctm` and `jtm` helper classes defined in `JetRecTools/python/JetRecToolsConfig.py` (see below) and `JetRec/python/JetToolSupport.py`.

### `ScheduleClusterOriginCorrection.py`

* This module provides the `applyClusterOriginCorrection` function, which schedules origin correction operations on topoclusters at a given scale. The algs will only be activated if the collections to be produced are absent from the input file.

### `JetRecToolsConfig.py`

* This module is a configuration helper for tools in this package, primarily for constituent modifiers via the `ConstituentToolManager` helper class. The `ctm` accumulates tools providing constituent modification capabilities, and generates a configured `JetConstitModSequence` tool applying a sequence of constituent modifications to a given collection via its function `buildConstitModSequence`. The tool returned can then be placed in a `JetAlgorithm`.

### `DefaultTools.py`

* [Unused] This module was intended to take over core jet reconstruction configuration code from the `JetRec/python/JetRecStandardTools.py` module, placing the default configurations closer to the package in which the tools are defined. However, this has been superseded by other developments.

> Written with [StackEdit](https://stackedit.io/).
