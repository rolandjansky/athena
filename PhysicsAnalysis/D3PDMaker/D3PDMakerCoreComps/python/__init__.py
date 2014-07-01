# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: __init__.py 495152 2012-04-11 15:09:02Z ssnyder $
#
# @file D3PDMakerCoreComps/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   D3PDMakerCoreComps.D3PDMakerCoreCompsConf.D3PD__VectorFillerTool
#
# we can write
#
#   D3PDMakerCoreComps.VectorFillerTool
#

import D3PDMakerCoreCompsConf
for k, v in D3PDMakerCoreCompsConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

# Bring these into this scope as well.
from MakerAlg                import MakerAlg
from ContainerFlagFillerTool import ContainerFlagFillerTool
