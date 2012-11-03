# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: __init__.py 288000 2010-03-25 18:09:04Z krasznaa $
#
# @file TrigEgammaD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Sep, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   TrigEgammaD3PDMaker.TrigEgammaD3PDMakerConf.D3PD__TrigElectronFillerTool
#
# we can write
#
#   TrigEgammaD3PDMaker.TrigElectronFillerTool
#

import TrigEgammaD3PDMakerConf
for k, v in TrigEgammaD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v
