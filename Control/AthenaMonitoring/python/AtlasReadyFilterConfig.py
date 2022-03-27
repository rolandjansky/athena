# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Run-3 style config of AtlasReadyFilterTool

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.Enums import BeamType


def AtlasReadyFilterCfg(flags):
   result=ComponentAccumulator()

   if flags.Common.isOnline or flags.Input.isMC or flags.Beam.Type is not BeamType.Collisions or flags.DQ.disableAtlasReadyFilter:
      result.setPrivateTools(CompFactory.DQDummyFilterTool())
   else:
      from IOVDbSvc.IOVDbSvcConfig import addFolders
      result.merge(addFolders(flags,'/TDAQ/RunCtrl/DataTakingMode','TDAQ',className='AthenaAttributeList'))
      result.setPrivateTools(CompFactory.DQAtlasReadyFilterTool())
   return result

                   
