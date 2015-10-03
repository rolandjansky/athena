# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file RedirectProxyProviderSvc.py
# @brief temporary backward compatibility hack
# @details This hack allows to keep backward compatibility for statements like
# @code
# from StoreGate.StoreGateConf import ProxyProviderSvc
# @endcode

from StoreGate.StoreGateConf import *
import SGComps.SGCompsConf
ProxyProviderSvc = SGComps.SGCompsConf.ProxyProviderSvc
#FIXME print a warning when ProxyProviderSvc is used
