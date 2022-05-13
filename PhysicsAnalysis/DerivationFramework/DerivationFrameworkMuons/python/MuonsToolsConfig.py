# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MuonJetDrToolCfg(ConfigFlags, name):
    acc = ComponentAccumulator()
    muonJetDrTool = CompFactory.DerivationFramework.MuonJetDrTool(name)
    acc.addPublicTool(muonJetDrTool, primary=True)
    return acc
