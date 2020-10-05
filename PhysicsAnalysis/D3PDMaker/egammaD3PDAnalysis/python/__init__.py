# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file egammaD3PDMakerAnalysis/python/__init__.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2009
# @brief Bring configurables into module namespace.
#
# Allows `egammaD3PDAnalysisConf.D3PD__foo' to be referred to as
# `egammaD3PDAnalysis.foo'.
#


from . import egammaD3PDAnalysisConf
for k, v in egammaD3PDAnalysisConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

