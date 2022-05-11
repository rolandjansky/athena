# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory


def FilledBunchFilterToolCfg(flags: AthConfigFlags):
   fbft_local_logger = logging.getLogger('FilledBunchFilterToolCfg')
   fbft_local_logger.debug('Configuring FilledBunchFilterTool')
   result=ComponentAccumulator()

   if (flags.DQ.disableFilledBunchFilter
       or flags.Input.isMC
       or flags.DQ.DataType != 'collisions'
       or flags.Input.TriggerStream == 'CosmicCalo'):
      fbft_local_logger.info('Filled bunch filter being configured to always return true')
      result.setPrivateTools(CompFactory.DQDummyFilterTool())
      return result
   else:
      fbft_local_logger.debug('Filled bunch filter being configured')
      from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
      result.merge(BunchCrossingCondAlgCfg(flags))
      result.setPrivateTools(CompFactory.DQFilledBunchFilterTool())
      return result