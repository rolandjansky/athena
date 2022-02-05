#!/usr/bin/env python
"""Run tests on StripGeoModelXml configuration

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
        from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripSimulationGeometryCfg
        acc = ITkStripSimulationGeometryCfg(ConfigFlags)
        f=open('ITkStripSimulationGeometryCfg.pkl','wb')
    else:
        from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
        acc = ITkStripReadoutGeometryCfg(ConfigFlags)
        f=open('ITkStripReadoutGeometryCfg.pkl','wb')
    acc.store(f)
    f.close()
