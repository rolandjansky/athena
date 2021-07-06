# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from ROOT import MC
dummy = MC.ROOT6_NamespaceAutoloadHook

generatorWeightsPrefix = MC.generatorWeightsPrefix
weightNameCleanup = MC.weightNameCleanup
weightNameWithPrefix = MC.weightNameWithPrefix

__all__ = ['generatorWeightsPrefix', 'weightNameCleanup', 'weightNameWithPrefix']
