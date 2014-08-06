# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TriggerD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Sep, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   TriggerD3PDMaker.TriggerD3PDMakerConf.D3PD__FourMomFillerTool
#
# we can write
#
#   TriggerD3PDMaker.FourMomFillerTool
#

import TriggerD3PDMakerConf
for k, v in TriggerD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

