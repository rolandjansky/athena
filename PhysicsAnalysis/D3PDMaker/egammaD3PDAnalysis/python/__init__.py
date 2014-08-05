# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDMakerAnalysis/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2009
# @brief Bring configurables into module namespace.
#
# Allows `egammaD3PDAnalysisConf.D3PD__foo' to be referred to as
# `egammaD3PDAnalysis.foo'.
#


import egammaD3PDAnalysisConf
for k, v in egammaD3PDAnalysisConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

# Bring this into this scope.
from egammaShowerDepthAlg import egammaShowerDepthAlg

