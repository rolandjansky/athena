# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory

def ActsGeantFollowerToolCfg(configFlags, name="ActsGeantFollowerTool", **kwargs):
  THistSvc= CompFactory.THistSvc
  result = ComponentAccumulator()
  histsvc = THistSvc(name="THistSvc")
  histsvc.Output = ["val DATAFILE='GeantFollowing.root' OPT='RECREATE'"]
  result.addService(histsvc)
  result.setPrivateTools(CompFactory.ActsGeantFollowerTool(name, **kwargs))
  
  return result
