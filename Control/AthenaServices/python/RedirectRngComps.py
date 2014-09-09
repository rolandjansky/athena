# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file RedirectRngComps.py
# @brief temporary (ah!) backward compatibility hack
# @details This hack allows to keep backward compatibility for statements like
# @code
# from AthenaServices.AthenaServicesConf import AtDSFMTGenSvc
# @endcode

from AthenaServices.AthenaServicesConf import *
try:
    import RngComps.RngCompsConf
    AtDSFMTGenSvc = RngComps.RngCompsConf.AtDSFMTGenSvc
    AtRanluxGenSvc = RngComps.RngCompsConf.AtRanluxGenSvc
    AtRndmGenSvc = RngComps.RngCompsConf.AtRndmGenSvc
except ImportError:
    # not enough karma ?
    pass
#FIXME print a warning when xyzSvc is used

