# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file D3PDMakerTest/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Jan, 2010
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   D3PDMakerTest.D3PDMakerTestConf.D3PDTest__FillerAlg
#
# we can write
#
#   D3PDMakerTest.FillerAlg
#

import D3PDMakerTestConf
for k, v in D3PDMakerTestConf.__dict__.items():
    if k.startswith ('D3PDTest__'):
        globals()[k[10:]] = v
