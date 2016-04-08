# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: __init__.py 485019 2012-02-24 16:16:04Z krasznaa $
#
# @file HiggsD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Sep, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   HiggsD3PDMaker.HiggsD3PDMakerConf.D3PD__QuadrupletFillerTool
#
# we can write
#
#   HiggsD3PDMaker.QuadrupletFillerTool
#

import HiggsD3PDMakerConf
for k, v in HiggsD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v
