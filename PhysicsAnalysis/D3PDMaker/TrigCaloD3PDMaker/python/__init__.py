# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: __init__.py 287965 2010-03-25 16:14:16Z krasznaa $
#
# @file TrigCaloD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Sep, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   TrigCaloD3PDMaker.TrigCaloD3PDMakerConf.D3PD__TrigEMClusterFillerTool
#
# we can write
#
#   TrigCaloD3PDMaker.TrigEMClusterFillerTool
#

import TrigCaloD3PDMakerConf
for k, v in TrigCaloD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v
