# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def getDetectorsFromRunArgs(ConfigFlags, runArgs):
    """Generate detector list based on runtime arguments."""
    if hasattr(runArgs, 'detectors'):
        detectors = runArgs.detectors
    else:
        from AthenaConfiguration.AutoConfigFlags import getDefaultDetectors
        detectors = getDefaultDetectors(ConfigFlags.GeoModel.AtlasVersion)

    # Support switching on Forward Detectors
    if hasattr(runArgs, 'LucidOn'):
        detectors = detectors+['Lucid']
    if hasattr(runArgs, 'ZDCOn'):
        detectors = detectors+['ZDC']
    if hasattr(runArgs, 'AFPOn'):
        detectors = detectors+['AFP']
    if hasattr(runArgs, 'ALFAOn'):
        detectors = detectors+['ALFA']
    if hasattr(runArgs, 'FwdRegionOn'):
        detectors = detectors+['FwdRegion']
    # TODO here support switching on Cavern geometry
    # if hasattr(runArgs, 'CavernOn'):
    #     detectors = detectors+['Cavern']

    return detectors
