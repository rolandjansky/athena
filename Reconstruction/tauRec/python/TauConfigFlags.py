# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createTauConfigFlags():
    flags = AthConfigFlags()
    flags.addFlag('Tau.SeedJetCollection', 'AntiKt4LCTopoJets')
    flags.addFlag('Tau.CalibrateLCConfig', 'CaloTES_R22_v1.root')
    flags.addFlag('Tau.MvaTESConfig', 'MVATES_r22_mar1.weights.root')
    return flags