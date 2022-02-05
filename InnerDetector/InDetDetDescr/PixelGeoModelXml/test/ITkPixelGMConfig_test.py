#!/usr/bin/env python
"""Run tests on PixelGeoModelXml configuration

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/ttbar.HITS.pool.root"]
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
    ConfigFlags.GeoModel.Align.Dynamic = False
    ConfigFlags.lock()

    if ConfigFlags.Common.Project == "AthSimulation":
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelSimulationGeometryCfg
        acc = ITkPixelSimulationGeometryCfg(ConfigFlags)
        f=open('ITkPixelSimulationGeometryCfg.pkl','wb')
    else:
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
        acc = ITkPixelReadoutGeometryCfg(ConfigFlags)
        f=open('ITkPixelReadoutGeometryCfg.pkl','wb')
    acc.store(f)
    f.close()
