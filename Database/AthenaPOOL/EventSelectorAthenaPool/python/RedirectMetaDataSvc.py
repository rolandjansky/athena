# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file RedirectMetaDataSvc.py
# @brief temporary backward compatibility hack
# @details This hack allows to keep backward compatibility for statements like
# @code
# from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
# @endcode

from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import *
import AthenaServices.AthenaServicesConf
MetaDataSvc = AthenaServices.AthenaServicesConf.MetaDataSvc
#FIXME print a warning when MetaDataSvc is used
