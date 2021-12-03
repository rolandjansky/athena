# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags


def createHIRecConfigFlags():
  flags=AthConfigFlags()
  flags.addFlag("HeavyIon.doGlobal", True)
  flags.addFlag("HeavyIon.Global.doEventShapeSummary", True)

  flags.addFlag("HeavyIon.doJet", True)

  flags.addFlag("HeavyIon.doEgamma", True)
  # expand as needed
  return flags