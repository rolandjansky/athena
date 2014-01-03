# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: __init__.py 282498 2010-03-03 15:36:37Z krasznaa $
#
# @file TrigMuonD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Sep, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   TrigMuonD3PDMaker.TrigMuonD3PDMakerConf.D3PD__MuonFeature4MomFillerTool
#
# we can write
#
#   TrigMuonD3PDMaker.MuonFeature4MomFillerTool
#

import TrigMuonD3PDMakerConf
for k, v in TrigMuonD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v
