# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Legacy module to do the PyG4AtlasAlg top alg sequence addition. Not needed, and soon to be deprecated!
"""

print "G4AtlasApps.SimKernel is deprecated: please replace your imports of " + \
    "it with an explicit instantiation of PyG4AtlasAlg"

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
