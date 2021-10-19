# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def InDetGeometryDBSvcCfg(flags):
    acc = ComponentAccumulator()
    acc.addService(CompFactory.GeometryDBSvc("InDetGeometryDBSvc"), primary=True)
    return acc
