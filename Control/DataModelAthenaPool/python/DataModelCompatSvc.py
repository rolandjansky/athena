# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: DataModelCompatSvc.py,v 1.2 2008-03-26 19:23:06 ssnyder Exp $
#
# File: python/DataModelCompatSvc.py
# Author: scott snyder
# Date: Dec 2007, from previous JO version.
# Purpose: Set up backwards compatibility for DataModel.
#
# Just import this module to set up DataModelCompatSvc.
#

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from DataModelAthenaPool.DataModelAthenaPoolConf import DataModelCompatSvc

# We need this service created explicitly, and we need to make
# sure that it is initialized before the input file is opened.
class DataModelCompatSvcCreate (DataModelCompatSvc):
    def setup (self):
        DataModelCompatSvc.setup (self)
        theApp.getHandle()._svcmgr.addService ("", "DataModelCompatSvc", 5)
        return
ServiceMgr += DataModelCompatSvcCreate("DataModelCompatSvc")

