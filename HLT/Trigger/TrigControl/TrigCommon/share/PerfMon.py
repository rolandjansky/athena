#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# Job option fragment to enable PerfMon
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg
from AthenaCommon.Logging import logging

_log = logging.getLogger('TrigCommon/PerfMon.py')

if not ConfigFlags.PerfMon.doFastMonMT and not ConfigFlags.PerfMon.doFullMonMT:
    _log.info('Setting flag PerfMon.doFastMonMT=True')
    ConfigFlags.PerfMon.doFastMonMT = True

CAtoGlobalWrapper(PerfMonMTSvcCfg, ConfigFlags)
