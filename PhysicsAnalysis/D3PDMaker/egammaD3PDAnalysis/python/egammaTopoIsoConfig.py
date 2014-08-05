# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDAnalysis/python/egammaTopoIsoConfig.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2011
# @brief Configure egammaTopoIsoAlg to fill UserData.
#


from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey
from AthenaCommon.AlgSequence                import AlgSequence
from egammaCaloTools.egammaTopoIsoToolBase   import egammaTopoIsoToolBase
import D3PDMakerCoreComps
import egammaD3PDAnalysis


def egammaTopoIsoConfig \
        (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
         prefix = '',
         sgkey = D3PDMakerFlags.ElectronSGKey(),
         typeName = 'ElectronContainer',
         allowMissing = False,
         UsePositiveClusters = False,
         UseEMScale = True):
    """Configure egammaTopoIsoAlg for D3PD making.

    SEQ is the Gaudi sequence to which the algorithm should be added.
    Default is that given by PreD3PDAlgSeqName.

    PREFIX is a prefix to add to the name of the algorithm scheduled.

    SGKEY/TYPENAME is the StoreGate key of the input electron container
    and the name of its type.

    If ALLOWMISSING is true, don't fail if the SG key doesn't exist.
"""

    if (not D3PDMakerFlags.MakeEgammaUserData() or
        D3PDMakerFlags.HaveEgammaUserData()):
        return

    DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool
    resolved_sgkey = resolveSGKey (typeName, sgkey)
    auxprefix = (D3PDMakerFlags.EgammaUserDataPrefix() + '_' +
                 resolved_sgkey + '_')

    if not UseEMScale:
        egammaTopoIsoToolName = "egammaCalTopoIsoToolBase" 
        egammaTopoIsoTool = egammaTopoIsoToolBase(egammaTopoIsoToolName) 
        egammaTopoIsoTool.UseEMScale = False
    else:
        egammaTopoIsoToolName = "egammaEMTopoIsoToolBase"
        egammaTopoIsoTool = egammaTopoIsoToolBase(egammaTopoIsoToolName)
        egammaTopoIsoTool.UseEMScale = True
        
    from AthenaCommon.AppMgr import ToolSvc
    if egammaTopoIsoTool not in ToolSvc:
        ToolSvc += egammaTopoIsoTool

    etianame = 'egammaTopoIsoAlg_' + resolved_sgkey
    if not hasattr (seq, etianame):
        seq += egammaD3PDAnalysis.egammaTopoIsoAlg \
               (etianame,
                EgammaGetter = DVGetter
                  (prefix +'egammaTopoIsoAlgEgammaGetter',
                   TypeName = typeName,
                   SGKey = sgkey),
                ClusterGetter = DVGetter
                  (prefix + 'egammaTopoIsoAlgClusterGetter',
                   TypeName = 'DataVector<xAOD::CaloCluster_v1>',
                   SGKey = D3PDMakerFlags.ClusterSGKey()),
                UseEMScale = UseEMScale,
                PositiveEnergyClusters = UsePositiveClusters,
                TopoIsoTool = egammaTopoIsoTool,
                AllowMissing = allowMissing,
                AuxPrefix = auxprefix)

    return
