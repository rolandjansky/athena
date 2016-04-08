# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @purpose backward compatibility hack

from AthenaCommon import CfgMgr
from AthenaServices.AthenaServicesConf import AthenaSealSvc

__all__ = [ "AthenaSealSvc" ]

from AthenaCommon.Logging import logging as _logging
_msg = _logging.getLogger ("AthenaSealSvcConf")
_warn = _msg.warning
_warn ("AthenaSealSvc.AthenaSealSvcConf has been retired !")
_warn ("Please use 'AthenaSealSvc' from the AthenaServices package")
_warn ("ex: ")
_warn (" from AthenaCommon import CfgMgr")
_warn (" svcMgr += CfgMgr.AthenaSealSvc()")
del _warn
