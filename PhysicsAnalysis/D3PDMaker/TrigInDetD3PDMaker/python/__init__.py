# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: __init__.py 287921 2010-03-25 14:32:38Z krasznaa $
#
# @file TriggerD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Sep, 2009
# @brief Define shortcuts for referencing configurables in this package.
#
# With this, instead of writing
#
#   TrigInDetD3PDMaker.TrigInDetD3PDMakerConf.D3PD__TrigInDetTrackFillerTool
#
# we can write
#
#   TrigInDetD3PDMaker.TrigInDetTrackFillerTool
#

import TrigInDetD3PDMakerConf
for k, v in TrigInDetD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

