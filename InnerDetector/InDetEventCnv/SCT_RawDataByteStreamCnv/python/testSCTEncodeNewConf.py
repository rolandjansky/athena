#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.isMC = True
    ConfigFlags.Input.Files = ["/afs/cern.ch/atlas/project/rig/referencefiles/RTTinputFiles/MC15_13TeV/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r6699_10evt.pool.root"]
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-RUN12-SDR-31"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2015-03-01-00"
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    # For POOL file reading
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))

    # For ByteStream file writing
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamWriteCfg
    writingAcc = ByteStreamWriteCfg(ConfigFlags, [ "SCT_RDO_Container#SCT_RDOs" ] )
    writingAcc.getService("ByteStreamEventStorageOutputSvc").StreamType = "EventStorage"
    writingAcc.getService("ByteStreamEventStorageOutputSvc").StreamName = "StreamBSFileOutput"
    acc.merge(writingAcc)

    # For SCT geometry and cabling
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    acc.merge(SCT_GeometryCfg(ConfigFlags))
    from InDetConfig.InDetRecToolConfig import SCT_CablingToolCfg
    acc.popToolsAndMerge(SCT_CablingToolCfg(ConfigFlags))

    # For EventInfo necessary for ByteStream file writing
    from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
    acc.merge(EventInfoCnvAlgCfg(ConfigFlags,
                                 inputKey="McEventInfo",
                                 outputKey="EventInfo"))

    acc.run(maxEvents=10)
