#!/usr/bin/env python
"""Run tests on AddressRemappingConfig.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from SGComps.AddressRemappingConfig import AddressRemappingCfg, InputRenameCfg, InputOverwriteCfg

# Set up logging and new style config
Configurable.configurableRun3Behavior = True

# Construct our accumulator to run
acc = ComponentAccumulator()

# Create empty address remapping service
acc.merge(AddressRemappingCfg())

# Create address remapping service with a pre-defined map
acc.merge(AddressRemappingCfg(["Type1#from->to", "Type2#a->b"]))

# Test helper functions
acc.merge(InputRenameCfg("Foo", "foo", "bar"))
acc.merge(InputRenameCfg("Foo", "foo.d1", "bar.x1"))
acc.merge(InputOverwriteCfg("FromType", "fromName", "ToType", "toName"))

# Dump config
acc.printConfig(withDetails=True)

# For test purposes only!
acc.wasMerged()

# Test our services
print(acc.getService("ProxyProviderSvc"))

svc = acc.getService("AddressRemappingSvc")
print(svc)

ref1=['Type1#from->to',
      'Type2#a->b',
      'Foo#foo->bar',
      'Foo#foo.d1->bar.x1']
ref2=['FromType#fromName->ToType#toName'
  ]

assert(set(svc.TypeKeyRenameMaps) == set(ref1))
assert(set(svc.TypeKeyOverwriteMaps) == set(ref2))
