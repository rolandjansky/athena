# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   EventCommonD3PDMaker.EventCommonD3PDMakerConf.D3PD__FourMomFillerTool
#
# we can write
#
#   EventCommonD3PDMaker.FourMomFillerTool
#

import EventCommonD3PDMakerConf
for k, v in EventCommonD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v


