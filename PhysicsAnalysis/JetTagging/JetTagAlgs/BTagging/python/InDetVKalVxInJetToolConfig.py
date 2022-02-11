# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
from JetTagTools.InDetEtaDependentCutsSvcConfig import IDEtaDependentCuts_SV1_SvcCfg


def InDetVKalVxInJetToolCfg(ConfigFlags, name, MSV = False, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetVKalVxInJetTool tool and returns it.

    The following options have BTaggingFlags defaults:

    TrackSummaryTool                    default: None (not needed for b-tagging)

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output. Note however
    that this tool also needs to be added to the main B-tagging tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        if MSV:
            defaults = {
                     'getNegativeTail' : False,
                     'ExistIBL'        : geoFlags.isIBL(),
                     'ConeForTag'      : 1.0,
                     'MultiVertex'     : True,
                     }
        else:
            defaults = {
                     'ExistIBL'         : geoFlags.isIBL(),
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])

        if ConfigFlags.GeoModel.Run not in [LHCPeriod.Run1, LHCPeriod.Run2, LHCPeriod.Run3]:
            acc.merge(IDEtaDependentCuts_SV1_SvcCfg(ConfigFlags, name="IDEtaDepCutsSvc_" + name))
            options.setdefault("InDetEtaDependentCutsSvc", acc.getService("IDEtaDepCutsSvc_" + name))
            options.setdefault("useVertexCleaningPix", False) # Would use hardcoded InDet Pixel geometry
            options.setdefault("useITkMaterialRejection", True)

    options['name'] = name
    acc.setPrivateTools(CompFactory.InDet.InDetVKalVxInJetTool(**options))

    return acc
