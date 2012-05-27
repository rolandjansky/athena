# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: __init__.py 278517 2010-02-10 16:46:44Z krasznaa $
#
# @file TrigMissingETD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Sep, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   TrigMissingETD3PDMaker.TrigMissingETD3PDMakerConf.D3PD__TrigMissingETBaseFillerTool
#
# we can write
#
#   TrigMissingETD3PDMaker.TrigMissingETBaseFillerTool
#

import TrigMissingETD3PDMakerConf
for k, v in TrigMissingETD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v
