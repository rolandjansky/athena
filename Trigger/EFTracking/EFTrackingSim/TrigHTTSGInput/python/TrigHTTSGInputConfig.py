# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaCommon.SystemOfUnits import GeV
def TrigHTTSGInputCfg(flags, outFile):
    """
    Configure HTT wrapers generation, outFile will be taken from flags in the future
    """
    acc = ComponentAccumulator()
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    acc.merge(PixelReadoutGeometryCfg(flags))
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc.merge(SCT_ReadoutGeometryCfg(flags))

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    acc.addPublicTool(extrapolator)

    HTTSGInputTool = CompFactory.TrigHTTSGToRawHitsTool(maxEta=3.2, minPt=0.8 * GeV,
        Extrapolator = extrapolator
    )
    acc.addPublicTool(HTTSGInputTool)

    wrapperAlg = CompFactory.TrigHTTRawHitsWrapperAlg(
        InputTool=HTTSGInputTool, OutFileName=outFile
    )
    acc.addEventAlgo(wrapperAlg)

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable

    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg

    ConfigFlags.Input.Files = [] # no input, needs to be specified via command line --inputFiles=filename
    ConfigFlags.Exec.MaxEvents = 5
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    acc = MainServicesCfg(ConfigFlags)
    # if need to read POOL file
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    acc.merge(PoolReadCfg(ConfigFlags))

    # if need to read BS file
    # from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg

    # acc.merge(ByteStreamReadCfg(ConfigFlags))

    acc.merge(TrigHTTSGInputCfg(ConfigFlags, "wrapper.root"))

    acc.printConfig(withDetails=True)
    # either
    status = acc.run()
    if status.isFailure():
        import sys

        sys.exit(-1)
    # or
    acc.wasMerged()
