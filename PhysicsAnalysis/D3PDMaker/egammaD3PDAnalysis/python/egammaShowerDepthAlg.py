# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDMakerAnalysis/python/egammaShowerDepthAlg.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2010
# @brief Configure the shower depth algorithm.
#


from egammaD3PDAnalysis import egammaD3PDAnalysisConf
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaCommon.GlobalFlags   import globalflags

def egammaShowerDepthAlg (name, **kwin):
    # Set up database access.
    if globalflags.DataSource() == 'data':    
        folder  = '/CALO/CaloSwClusterCorrections/calhits'
    else:
        folder  = '/CALO/Ofl/CaloSwClusterCorrections/calhits'
    tag = D3PDMakerFlags.EgammaShowerDepthTag()
    if not tag:
        tag = '@GLOBAL'

    # Create the algorithm.
    kw = {'prefix' : 'ele55.'}
    kw.update (kwin)
    alg = egammaD3PDAnalysisConf.D3PD__egammaShowerDepthAlg (name, **kw)

    from CaloClusterCorrection.common import config_from_cool
    config_from_cool (alg, folder, tag)

    return alg

    
