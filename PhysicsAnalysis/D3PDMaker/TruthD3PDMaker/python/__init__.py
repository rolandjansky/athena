# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TruthD3PDMaker/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2011
# @brief Move configurables into the module namespace.
#

# Install any configurables from this package in the D3PD namespace
# directly in the top-level module, without the namespace prefix.
import TruthD3PDMakerConf
for k, v in TruthD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v
