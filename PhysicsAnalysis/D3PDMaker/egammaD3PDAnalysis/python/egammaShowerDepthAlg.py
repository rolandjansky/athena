# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDMakerAnalysis/python/egammaShowerDepthAlg.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2010
# @brief Configure the shower depth algorithm.
#


from egammaD3PDAnalysis import egammaD3PDAnalysisConf
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

def egammaShowerDepthAlg (name, **kwin):
    # Set up database access.
    folder  = '/CALO/CaloSwClusterCorrections/calhits'
    tag = D3PDMakerFlags.EgammaShowerDepthTag()
    if not tag:
        tag = '@GLOBAL'

    # Create the algorithm.
    kw = {'prefix' : 'ele55.',
          'COOLFolder' : '' }
    kw.update (kwin)
    alg = egammaD3PDAnalysisConf.D3PD__egammaShowerDepthAlg (name, **kw)

    from CaloClusterCorrection.common import config_from_cool
    config_from_cool (alg, folder, tag)

    return alg

    
