# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file egammaD3PDMaker/python/CorrectionClusterD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2011
# @brief D3PD object for EM clusters,
#        specialized for cluster correction studies.
#


from D3PDMakerCoreComps.D3PDObject    import make_SGDataVector_D3PDObject
from egammaD3PDAnalysis.TileGapConfig import TileGapConfig
from D3PDMakerCoreComps.D3PDObject    import DeferArg
from D3PDMakerCoreComps.resolveSGKey  import resolveSGKey # noqa: F401
import EventCommonD3PDMaker
import D3PDMakerCoreComps
import CaloD3PDMaker


auxprefix = DeferArg ('D3PDMakerFlags.EgammaUserDataPrefix() + "_" +'
                      'resolveSGKey (typeName, sgkey) + "_"',
                      globals())


CorrectionClusterD3PDObject = \
           make_SGDataVector_D3PDObject ('DataVector<xAOD::CaloCluster_v1>',
                                         'egClusterCollection',
                                         'cl_',
                                         'CorrectionClusterD3PDObject')

CorrectionClusterD3PDObject.defineBlock (0, 'Kinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteE  = True)
CorrectionClusterD3PDObject.defineBlock (0, 'Samplings',
                                 CaloD3PDMaker.ClusterSamplingFillerTool,
                                 SamplingEnergies = True,
                                 SamplingEtaPhi = True,
                                 SamplingEtaPhiRaw = True,
                                 #SamplingEtamax = True,
                                 Samplings = [0,1,2,3,4,5,6,7])


def _tilegapHook (c, prefix, sgkey, *args, **kw):
    TileGapConfig (sgkey = sgkey, prefix = prefix)
    return
CorrectionClusterD3PDObject.defineBlock (
    0, 'TileGapSum',
    D3PDMakerCoreComps.AuxDataFillerTool,
    AuxPrefix = auxprefix,
    Vars = ['tileGapSum'])
CorrectionClusterD3PDObject.defineHook (_tilegapHook)
