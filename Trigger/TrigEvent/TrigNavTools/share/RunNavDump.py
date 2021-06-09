#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

if __name__=='__main__':
    import sys

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    parser = ConfigFlags.getArgumentParser()
    args = parser.parse_args()

    #test input file: --filesInput='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.0/myAOD.pool.root'

    # Set the Athena configuration flags
    ConfigFlags.fillFromArgs()
    ConfigFlags.Output.AODFileName = "outAOD.pool.root"
    ConfigFlags.Detector.GeometryLAr=True
    ConfigFlags.Detector.GeometryTile=True
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaConfiguration.ComponentFactory import CompFactory

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))


    from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg
    cfg.merge(MetaDataSvcCfg(ConfigFlags))

    confSvc = CompFactory.TrigConf.xAODConfigSvc("xAODConfigSvc")
    cfg.addService(confSvc)
    from AthenaCommon.Constants import DEBUG
    alg = CompFactory.Run2ToRun3TrigNavConverter("TrigNavCnv", OutputLevel=DEBUG, TrigConfigSvc=confSvc)
    alg.doPrint = False
    #alg.Collections = ["xAOD::MuonContainer","xAOD::L2StandAloneMuonContainer"]
    alg.Collections = ["xAOD::ElectronContainer","xAOD::TrigEMClusterContainer","xAOD::TrigEMCluster","xAOD::TrigElectron","xAOD::TrigElectronContainer","xAOD::CaloCluster","xAOD::CaloClusterContainer"]
    
    # EXAMPLES OF CHAINS 
    #alg.Chains = ["HLT_mu4"]
    alg.Chains = ["HLT_e5_lhvloose_nod0","HLT_e9_etcut","HLT_e26_lhtight_nod0","HLT_e28_lhtight_nod0"]
    alg.Rois = ["forID","forID1","forID2","forMS","forSA","forTB","forMT","forCB"]

    cfg.addEventAlgo(alg, sequenceName="AthAlgSeq")
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    outputType="AOD"
    toRecord = ["xAOD::TrigCompositeContainer#HLTNav_All", "xAOD::TrigCompositeAuxContainer#HLTNav_AllAux.",
                "xAOD::TrigCompositeContainer#HLTNav_Summary", "xAOD::TrigCompositeAuxContainer#HLTNav_SummaryAux."]
    outputCfg = OutputStreamCfg(ConfigFlags, outputType, ItemList=toRecord, disableEventTag=True)
    streamAlg = outputCfg.getEventAlgo("OutputStream"+outputType)
    # need to expand possible options for the OutputStreamCfg to be able to pass also the metadata containers
    streamAlg.MetadataItemList += ["xAOD::TriggerMenuContainer#TriggerMenu", "xAOD::TriggerMenuAuxContainer#TriggerMenuAux."]
    streamAlg.TakeItemsFromInput = True
    cfg.addPublicTool(CompFactory.xAODMaker.TriggerMenuMetaDataTool("TriggerMenuMetaDataTool"))
    cfg.addService( CompFactory.MetaDataSvc("MetaDataSvc", MetaDataTools = [cfg.getPublicTool("TriggerMenuMetaDataTool")]))

    cfg.merge(outputCfg)

    # input EDM needs calo det descrition for conversion (uff)
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    cfg.merge(LArGMCfg(ConfigFlags))
    cfg.merge(TileGMCfg(ConfigFlags))

    cfg.printConfig(withDetails=True, summariseProps=False) # set True for exhaustive info
    sc = cfg.run(args.evtMax, args.loglevel)
    sys.exit(0 if sc.isSuccess() else 1)
