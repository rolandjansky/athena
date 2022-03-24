# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createJetConfigFlags():
    flags = AthConfigFlags()

    flags.addFlag("Jet.doUpstreamDependencies", False)


    return flags
