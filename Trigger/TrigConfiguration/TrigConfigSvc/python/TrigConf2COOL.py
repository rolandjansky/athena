# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @file TrigCond2COOL.py
###############################################################
#
# The jobOptions to enable reading and writing of configuration
# data from COOL into AOD
#
#
#==============================================================
#
# Required libs:
from . import TrigConf2COOLLib as _impl

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr += _impl.TrigConf2CoolSyncSvc()
theApp.CreateSvc.insert(0,svcMgr.TrigConf2CoolSyncSvc.getFullJobOptName())

theConfCOOLWriter = _impl.ConfToCoolSQlite()

del _impl

