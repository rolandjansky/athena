# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags


def createHIRecConfigFlags():
  flags=AthConfigFlags()
  flags.addFlag("doGlobal", True)
  flags.addFlag("doJet", True)
  flags.addFlag("doEgamma", True)
  # expand as needed
  return flags