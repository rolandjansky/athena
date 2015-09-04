# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TrigMbD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Oct, 2010
# @brief Bring names from the TrigMbD3PD C++ namespace into the
#        TrigMbD3PDMaker module.
#


import TrigMbD3PDMakerConf
for k, v in TrigMbD3PDMakerConf.__dict__.items():
    if k.startswith ('TrigMbD3PD__'):
        globals()[k[12:]] = v

