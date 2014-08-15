# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   CaloD3PDMaker.CaloD3PDMakerConf.D3PD__xxxTool
#
# we can write
#
#   CaloD3PDMaker.xxxTool
#


import CaloD3PDMakerConf
for k, v in CaloD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

from ClusterMomentFillerTool import ClusterMomentFillerTool
