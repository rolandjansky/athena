# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory


def BCMPrimeGeometryCfg(flags):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    bcmPrimeDetectorTool = CompFactory.BCMPrimeDetectorTool()
    bcmPrimeDetectorTool.Alignable = False # make this a flag? Set true as soon as decided on folder structure
    bcmPrimeDetectorTool.DetectorName = "BCMPrime"
    if flags.GeoModel.useLocalGeometry:
      # Setting this filename triggers reading from local file rather than DB
      bcmPrimeDetectorTool.GmxFilename = flags.ITk.bcmPrimeGeometryFilename
    geoModelSvc.DetectorTools += [ bcmPrimeDetectorTool ]

    return acc
