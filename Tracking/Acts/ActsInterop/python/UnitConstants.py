# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
Wraps the ACTS unit constants loaded from a ROOT dictionary.
To use these values please use

```python
from ActsInterop import UnitConstants
value = 15*UnitConstants.mm
```

"""

from ROOT import ActsUnitConstants
dummy = ActsUnitConstants.ROOT6_NamespaceAutoloadHook

from ROOT import Acts
import inspect

# Pull in all named unit constants into this namespace to improve ergonomics
for name, val in inspect.getmembers(Acts.UnitConstants):
  if name.startswith("_"): continue
  locals()[name] = val

