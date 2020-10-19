# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file CaloSysD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   CaloSysD3PDMaker.CaloSysD3PDMakerConf.D3PD__xxxTool
#
# we can write
#
#   CaloSysD3PDMaker.xxxTool
#


import CaloSysD3PDMakerConf
for k, v in list(CaloSysD3PDMakerConf.__dict__.items()):
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

from .ClusterMomentFillerTool import ClusterMomentFillerTool
