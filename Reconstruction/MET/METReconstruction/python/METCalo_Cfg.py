# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoCfg import BuildConfig, METConfig,getMETRecoAlg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def METCalo_Cfg(configFlags):
    sequencename = "METReconstruction_Calo"

    components = ComponentAccumulator()
    from AthenaConfiguration.ComponentFactory import CompFactory
    AthSequencer=CompFactory.AthSequencer
    components.addSequence( AthSequencer(sequencename) )
    ############################################################################
    # EMTopo

    cfg_emt = METConfig('EMTopo',configFlags,[BuildConfig('SoftClus','EMTopo')],
                    doRegions=True,
                    doOriginCorrClus=False
                    )

    ############################################################################
    # LocHadTopo
    
    cfg_lht = METConfig('LocHadTopo',configFlags,[BuildConfig('SoftClus','LocHadTopo')],
                    doRegions=True,
                    doOriginCorrClus=False
                    )

    ############################################################################
    # Calo regions
    #SWITCH OFF CELLS WHEN RUNNING ON AOD
    cfg_calo = METConfig('Calo',configFlags,
                     [BuildConfig('CaloReg')],
                     doCells=False
                     )

    recoAlg_calo = getMETRecoAlg(algName='METRecoAlg_Calo',configs={"EMTopo":cfg_emt,"LocHadTopo":cfg_lht,"Calo":cfg_calo})
    components.addEventAlgo(recoAlg_calo,sequencename)
    return components
