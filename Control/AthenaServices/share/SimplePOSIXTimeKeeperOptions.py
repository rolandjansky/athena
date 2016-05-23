## @file SimplePOSIXTimeKeeperOptions.py
# @brief job opts to activate SimplePOSIXTimeKeeperSvc
#
# $Id: SimplePOSIXTimeKeeperOptions.py,v 1.3 2007-06-16 00:55:21 calaf Exp $

from AthenaCommon.AppMgr import ServiceMgr
from AthenaServices.AthenaServicesConf import SimplePOSIXTimeKeeperSvc
ServiceMgr += SimplePOSIXTimeKeeperSvc()

