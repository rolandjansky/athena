"""Define functions to configure HGTD conditions algorithms

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""

def HGTD_DetectorElementCondAlgCfg(flags, name="HGTD_DetectorElementCondAlg", **kwargs):
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  from AthenaConfiguration.ComponentFactory import CompFactory
  """Return a ComponentAccumulator with configured HGTD_DetectorElementCondAlg for HGTD"""
  acc = ComponentAccumulator()
  kwargs.setdefault("DetManagerName", "HGTD")
  kwargs.setdefault("WriteKey", "HGTD_DetectorElementCollection")
  acc.addCondAlgo(CompFactory.HGTD_DetectorElementCondAlg(name, **kwargs))
  return acc
