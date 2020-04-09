# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable

Convert2Configurable = False
def GetCompatibleComponent(component):
    if Convert2Configurable:
        return conf2toConfigurable(component)
    return component
