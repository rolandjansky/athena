# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

if __name__=="__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    cfgFlags.Concurrency.NumThreads=8
    cfgFlags.Input.isMC=True
    cfgFlags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]
    cfgFlags.Output.AODFileName="output_AOD.root"
    cfgFlags.Output.doWriteAOD=True
    cfgFlags.PF.EOverPMode=True
    cfgFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg=MainServicesCfg(cfgFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(cfgFlags))
    from eflowRec.PFRun3Config import PFFullCfg
    cfg.merge(PFFullCfg(cfgFlags))
    
    from eflowRec.PFRun3Remaps import ListRemaps

    list_remaps=ListRemaps()
    for mapping in list_remaps:
        cfg.merge(mapping)    

    cfg.run()