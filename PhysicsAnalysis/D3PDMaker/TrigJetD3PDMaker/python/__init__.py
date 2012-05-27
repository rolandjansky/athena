# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: __init__.py 307763 2010-06-29 14:25:58Z krasznaa $
#
# @file TrigJetD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Sep, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   TrigJetD3PDMaker.TrigJetD3PDMakerConf.D3PD__TrigElectronFillerTool
#
# we can write
#
#   TrigJetD3PDMaker.TrigElectronFillerTool
#

import TrigJetD3PDMakerConf
for k, v in TrigJetD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v
