# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file TrigCond2COOL.py
## $Id: TrigConf2COOL.py,v 1.14 2009-05-06 13:09:27 stelzer Exp $
###############################################################
#
# The jobOptions to enable reading and writing of configuration
# data from COOL into AOD
#
#
#==============================================================
#
# Required libs:
import os.path
import threading

from . import TrigConf2COOLLib as _impl

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr += _impl.TrigConf2CoolSyncSvc()
theApp.CreateSvc.insert(0,svcMgr.TrigConf2CoolSyncSvc.getFullJobOptName())

theConfCOOLWriter = _impl.ConfToCoolSQlite()

del _impl

