# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from GaudiKernel.GaudiHandles import GaudiHandle
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging
import collections
def loadDefaultComps(allcomps):
    """Attempts to load all default components (those that are not actually configured)"""
    loadedSome = False
    def __load(comp, prop):
        descr = comp._descriptors[prop]
        if descr.default.getType() == "":
            return
        try:
            childComp = CompFactory.getComp(descr.default.getType())(descr.default.getName())
            comp._properties[prop] = childComp
            nonlocal loadedSome
            loadedSome = True
        except Exception:
            msg=logging.getLogger('loadDefaultComps')
            msg.warning("Default component %s can not be loaded", descr.default.typeAndName )
            pass

    for comp in allcomps:
        for propName,value in comp._descriptors.items():
            if propName in comp._properties: continue
            if isinstance(value.default, GaudiHandle):
                __load(comp, propName )
                        
    if loadedSome:
        loadDefaultComps(allcomps)

def exposeHandles(allcomps):
    """Sets all handle keys explicitly"""
    def __getDefault(d):
        if isinstance(d, collections.abc.Sequence):
            return [el  for el in d]
        else:
            return d.Path

    for comp in allcomps:
        for propName, value in comp._descriptors.items():
            if propName in comp._properties: continue
            if "HandleKey" in value.cpp_type:
                comp._properties[propName] = __getDefault(value.default)
