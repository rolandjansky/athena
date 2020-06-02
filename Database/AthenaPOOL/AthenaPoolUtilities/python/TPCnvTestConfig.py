# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg
from AthenaPoolUtilities.DumperConfig import Dumper, find_file
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
from LArGeoAlgsNV.LArGMConfig import LArGMCfg
from TileGeoModel.TileGMConfig import TileGMCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
#from AtlasGeoModel.ForDetGeoModelConfig import ForDetGeometryCfg
Configurable.configurableRun3Behavior = True

def TPCnvTest(infile, keys, useGeoModelSvc=False, useIOVDbSvc=False, doPixel=False, doSCT=False, doTRT=False, doLAr=False, doTile=False, doMuon=False):
    # Make sure we don't have a stale file catalog.
    if os.path.exists ('PoolFileCatalog.xml'):
        os.remove ('PoolFileCatalog.xml')

    if ('ATLAS_REFERENCE_TAG' not in globals() and
        'ATLAS_REFERENCE_TAG' in os.environ):
        ATLAS_REFERENCE_TAG = os.environ['ATLAS_REFERENCE_TAG'] # noqa: F841

    refpaths = [os.environ.get ('ATLAS_REFERENCE_DATA', None),
                '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art',
                '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs',
                '/afs/cern.ch/atlas/maxidisk/d33/referencefiles']

    if infile.startswith ('rtt:'):
        infile = infile[4:]
    infile = find_file (infile, refpaths)

    # Provide MC input
    ConfigFlags.Input.Files = [infile]
    ConfigFlags.GeoModel.AtlasVersion = 'ATLAS-R1-2012-03-01-00'
    ConfigFlags.GeoModel.Align.Dynamic = False
    ConfigFlags.Detector.GeometryPixel = doPixel
    ConfigFlags.Detector.GeometrySCT = doSCT
    ConfigFlags.Detector.GeometryTRT = doTRT
    ConfigFlags.Detector.GeometryLAr = doLAr
    ConfigFlags.Detector.GeometryTile = doTile
    ConfigFlags.Detector.GeometryMuon = doMuon
    ConfigFlags.lock()

    # Construct ComponentAccumulator
    acc = MainServicesCfg(ConfigFlags)
    acc.merge(PoolReadCfg(ConfigFlags))
    if useIOVDbSvc:
        acc.merge(IOVDbSvcCfg(ConfigFlags))
    if useGeoModelSvc:
        if ConfigFlags.Detector.GeometryPixel:
            acc.merge(PixelGeometryCfg(ConfigFlags))
            useGeoModelSvc = True
        if ConfigFlags.Detector.GeometrySCT:
            acc.merge(SCT_GeometryCfg(ConfigFlags))
            useGeoModelSvc = True
        if ConfigFlags.Detector.GeometryTRT:
            acc.merge(TRT_GeometryCfg(ConfigFlags))
            useGeoModelSvc = True
        if ConfigFlags.Detector.GeometryLAr:
            acc.merge(LArGMCfg(ConfigFlags))
            useGeoModelSvc = True
        if ConfigFlags.Detector.GeometryTile:
            acc.merge(TileGMCfg(ConfigFlags))
            useGeoModelSvc = True
        if ConfigFlags.Detector.GeometryMuon:
            acc.merge(MuonGeoModelCfg(ConfigFlags))
            useGeoModelSvc = True
        #acc.merge(ForDetGeometryCfg(ConfigFlags))
        acc.merge(GeoModelCfg(ConfigFlags))
        acc.getService("GeoModelSvc").IgnoreTagDifference = True
    acc.addEventAlgo(Dumper ('dumper', ConfigFlags.Input.Files[0], keys, refpaths), 'AthAlgSeq')
    return acc.run(maxEvents=10)
