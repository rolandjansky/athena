# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import os
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg
from AthenaPoolUtilities.DumperConfig import Dumper, find_file
#from AtlasGeoModel.ForDetGeoModelConfig import ForDetGeometryCfg
Configurable.configurableRun3Behavior = True

def TPCnvTest(infile, keys, useGeoModelSvc=False, useIOVDbSvc=False, doPixel=False, doSCT=False, doTRT=False, doLAr=False, doTile=False, doMuon=False, doTracks=False, configOnly=False, adjustMessageSvc=True):

    # Needed to prevent spurious root errors about streams in CreateRealData.
    import ROOT
    ROOT.GaudiPython.CallbackStreamBuf

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
    acc.setAppProperty('PrintAlgsSequence', False, overwrite=True)
    acc.merge(PoolReadCfg(ConfigFlags))
    if useIOVDbSvc:
        acc.merge(IOVDbSvcCfg(ConfigFlags))
    EventCnvSuperTool = None
    if useGeoModelSvc:
        if ConfigFlags.Detector.GeometryPixel:
            from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
            acc.merge(PixelReadoutGeometryCfg(ConfigFlags))
        if ConfigFlags.Detector.GeometrySCT:
            from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
            acc.merge(SCT_ReadoutGeometryCfg(ConfigFlags))
        if ConfigFlags.Detector.GeometryTRT:
            from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
            acc.merge(TRT_ReadoutGeometryCfg(ConfigFlags))
        if ConfigFlags.Detector.GeometryITkPixel:
            from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
            acc.merge(ITkPixelReadoutGeometryCfg(ConfigFlags))
        if ConfigFlags.Detector.GeometryITkStrip:
            from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
            acc.merge(ITkStripReadoutGeometryCfg(ConfigFlags))
        if ConfigFlags.Detector.GeometryLAr:
            from LArGeoAlgsNV.LArGMConfig import LArGMCfg
            acc.merge(LArGMCfg(ConfigFlags))
        if ConfigFlags.Detector.GeometryTile:
            from TileGeoModel.TileGMConfig import TileGMCfg
            acc.merge(TileGMCfg(ConfigFlags))
        if ConfigFlags.Detector.GeometryMuon:
            from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
            acc.merge(MuonGeoModelCfg(ConfigFlags))
        #acc.merge(ForDetGeometryCfg(ConfigFlags))
        from AtlasGeoModel.GeoModelConfig import GeoModelCfg
        acc.merge(GeoModelCfg(ConfigFlags))
        acc.getService("GeoModelSvc").IgnoreTagDifference = True
        if doTracks:
            # Doing this here as Trk.EventCnvSuperTool isn't part of all projects
            Trk_EventCnvSuperTool=CompFactory.Trk.EventCnvSuperTool
            EventCnvSuperTool = Trk_EventCnvSuperTool('EventCnvSuperTool', MaxErrorCount=10)
    acc.addEventAlgo(Dumper ('dumper', ConfigFlags.Input.Files[0], keys, refpaths), 'AthAlgSeq')
    if adjustMessageSvc:
        acc.getService("MessageSvc").enableSuppression = True
        acc.getService("MessageSvc").Format = "% F%18W%S%7W%R%T %0W%M"
    if EventCnvSuperTool is not None:
        acc.addPublicTool(EventCnvSuperTool)
    if configOnly:
        f = open('new.pkl', 'wb')
        acc.store(f)
        f.close()
        return
    return acc.run(maxEvents=10)
