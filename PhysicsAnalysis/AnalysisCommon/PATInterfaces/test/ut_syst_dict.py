#!/usr/bin/env python

# Import ROOT without graphics
from ROOT import gROOT
gROOT.SetBatch()

# Load lib - just the one we need
from ROOT import gSystem
gSystem.Load('libPATInterfaces.so')
gSystem.Load('libRootCoreUtils.so') # For the TestInvariant dependency

# Import the systematics classes
from ROOT.CP import SystematicVariation, SystematicSet
from ROOT.CP import SystematicCode, SystematicRegistry
from ROOT.CP import make_systematics_vector

# Build a dumb set of systematics
sys_set = SystematicSet()
sys_set.insert(SystematicVariation('sys1', 1))
# TODO: fix this
#sys_set.insert(SystematicVariation('sys2', SystematicVariation.CONTINUOUS))
sys_set.insert(SystematicVariation('sys2__continuous'))

# Test looping
for sys in sys_set:
    pass

# Try to retrieve the systematics registry
sys_reg = SystematicRegistry.getInstance()

# Register the systematics
sys_reg.registerSystematics(sys_set)
code = sys_reg.addSystematicsToRecommended(sys_set)
if code.code() != SystematicCode.Ok:
    raise Exception('Failed to call SystematicRegistry::addSystematicsToRecommended')

# Build a systematics list
sys_list = make_systematics_vector(sys_set)

# Test looping
for sys in sys_list:
    pass
