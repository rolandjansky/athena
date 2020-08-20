# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoCfg import BuildConfig, METConfig,getMETRecoAlg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def METTruth_Cfg(configFlags):
    sequencename = "METReconstruction_Truth"
    components = ComponentAccumulator()
    from AthenaConfiguration.ComponentFactory import CompFactory
    AthSequencer=CompFactory.AthSequencer
    components.addSequence( AthSequencer(sequencename) )
    ## Simple truth terms
    cfg_truth = METConfig('Truth',configFlags,
                          [BuildConfig('NonInt'),
                           BuildConfig('Int'),
                           BuildConfig('IntOut'),
                           BuildConfig('IntMuons')],
                          doRegions=True
                          )
    recoAlg=getMETRecoAlg(algName='METRecoAlg_Truth',configs={"Truth":cfg_truth})
    components.addEventAlgo(recoAlg, sequencename)
    return components

