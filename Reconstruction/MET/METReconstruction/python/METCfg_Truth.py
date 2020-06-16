# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoCfg import BuildConfig, METConfig,getMETRecoAlg,getMETRecoTool
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def METTruth_Cfg(configFlags):
    sequencename = "METReconstruction_Truth"

    components = ComponentAccumulator()
    from AthenaCommon.AlgSequence import AthSequencer
    components.addSequence( AthSequencer(sequencename) )
    ## Simple truth terms

    cfg_truth = METConfig('Truth',
                          [BuildConfig('NonInt'),
                           BuildConfig('Int'),
                           BuildConfig('IntOut'),
                           BuildConfig('IntMuons')],
                          doRegions=True
                          )

    recotool = getMETRecoTool(cfg_truth)
    recoAlg=getMETRecoAlg(algName='METRecoAlg_Truth',tools=[recotool])
    components.addEventAlgo(recoAlg, sequencename)
    return components

