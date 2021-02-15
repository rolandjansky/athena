"""Define methods to configure SCT_StripVetoTool

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg

def SCT_StripVetoCfg(flags, name="SCT_StripVeto", **kwargs):
    """Return a ComponentAccumulator configured for SCT_StripVetoTool
    """
    acc = ComponentAccumulator()

    # For SCT_ID used in SCT_StripVetoTool
    acc.merge(GeoModelCfg(flags))

    # Condition tool
    # Identifiers should be given as strings and decimal.
    # Identifier::compact() method returns unsigned long long.
    kwargs.setdefault("BadStripIdentifiers", ["576522359582752768",
                                              "576522475009998848",
                                              "576522475278434304",
                                              # "576522475546869760",
                                              # "576522475815305216",
                                              # "576522476083740672",
                                             ])
    acc.setPrivateTools(CompFactory.SCT_StripVetoTool(name=name+"Tool", **kwargs))

    return acc
