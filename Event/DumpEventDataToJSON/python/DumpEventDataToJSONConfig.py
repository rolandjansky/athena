# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Constants import VERBOSE
DumpEventDataToJsonAlg = CompFactory.DumpEventDataToJsonAlg


def DumpEventDataToJSONAlgCfg(configFlags, doExtrap=False, **kwargs):
    result = ComponentAccumulator()
    extrapolationEngine = ""
    if doExtrap:
        from AtlasGeoModel.GeoModelConfig import GeoModelCfg
        gmsAcc = GeoModelCfg(configFlags)
        result.merge(gmsAcc)

        from TrkConfig.AtlasExtrapolationEngineConfig import AtlasExtrapolationEngineCfg
        extrapAcc = AtlasExtrapolationEngineCfg(configFlags)
        extrapolationEngine = extrapAcc.getPrimary()
        result.merge(extrapAcc)

        kwargs.setdefault('Extrapolator', extrapolationEngine)

    dumpAlg = DumpEventDataToJsonAlg(
        ExtrapolateTrackParticles=doExtrap, **kwargs)
    result.addEventAlgo(dumpAlg)
    return result


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument("input",
                        help="Input pool file", default="../q221/myESD.pool.root")
    parser.add_argument("-o", "--output", dest="output", default='Event.json',
                        help="write JSON to FILE", metavar="FILE")
    parser.add_argument("--prependCalib", help="Prepend a calibartion event with some labelled objects at specific eta/phi.",
                        action="store_true")
    args = parser.parse_args()

    print('Running DumpEventDataToJSON on {} and outputting to {}. Prepend calib event is {}'.format(
        args.input, args.output, args.prependCalib))

    from AthenaCommon.Configurable import Configurable
    from AthenaCommon.Logging import log
    # from AthenaCommon.Constants import DEBUG
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    Configurable.configurableRun3Behavior = 1

    # Uncomment for debugging
    # log.setLevel(DEBUG)

    # To run on MC do e.g.
    ConfigFlags.Input.Files = [args.input]
    # To run on data do e.g.
    # ConfigFlags.Input.Files = ["../q431/myESD.pool.root"]

    # This should run serially for the moment.
    ConfigFlags.Concurrency.NumThreads = 1
    ConfigFlags.Concurrency.NumConcurrentEvents = 1

    ConfigFlags.lock()
    log.debug('Lock config flags now.')
    ConfigFlags.lock()

    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    if ConfigFlags.Detector.GeometryBpipe:
        from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
        cfg.merge(BeamPipeGeometryCfg(ConfigFlags))

    if ConfigFlags.Detector.GeometryPixel:
        from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
        cfg.merge(PixelReadoutGeometryCfg(ConfigFlags))

    if ConfigFlags.Detector.GeometrySCT:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
        cfg.merge(SCT_ReadoutGeometryCfg(ConfigFlags))

    if ConfigFlags.Detector.GeometryTRT:
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
        cfg.merge(TRT_ReadoutGeometryCfg(ConfigFlags))

    if ConfigFlags.Detector.GeometryITkPixel:
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
        cfg.merge(ITkPixelReadoutGeometryCfg(ConfigFlags))

    if ConfigFlags.Detector.GeometryITkStrip:
        from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
        cfg.merge(ITkStripReadoutGeometryCfg(ConfigFlags))

    if ConfigFlags.Detector.GeometryLAr:
        from LArGeoAlgsNV.LArGMConfig import LArGMCfg
        cfg.merge(LArGMCfg(ConfigFlags))

    if ConfigFlags.Detector.GeometryTile:
        from TileGeoModel.TileGMConfig import TileGMCfg
        cfg.merge(TileGMCfg(ConfigFlags))

    if ConfigFlags.Detector.GeometryMuon:
        from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
        cfg.merge(MuonGeoModelCfg(ConfigFlags))

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    cfg.merge(TrackingGeometrySvcCfg(ConfigFlags))

    from TrkConfig.TrackCollectionReadConfig import TrackCollectionReadCfg
    cfg.merge(TrackCollectionReadCfg(ConfigFlags, 'Tracks'))

    muon_edm_helper_svc = CompFactory.Muon.MuonEDMHelperSvc("MuonEDMHelperSvc")
    cfg.addService(muon_edm_helper_svc)

    # Disable doExtrap if you would prefer not to use the extrapolator.
    topoAcc = DumpEventDataToJSONAlgCfg(
        ConfigFlags, doExtrap=False, OutputLevel=VERBOSE, DumpTestEvent=args.prependCalib, OutputLocation=args.output)
    cfg.merge(topoAcc)

    cfg.run(10)
    f = open("DumpEventDataToJSONConfig.pkl", "wb")
    cfg.store(f)
    f.close()
