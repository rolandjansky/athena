# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDAnalysis/python/egammaCalcOQConfig.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2011
# @brief Configure egammaCalcOQAlg to fill UserData.
#


from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey
from AthenaCommon.AlgSequence                import AlgSequence
from egammaTools.egammaOQFlagsBuilderBase    import egammaOQFlagsBuilderBase
import D3PDMakerCoreComps
import egammaD3PDAnalysis


def egammaCalcOQConfig \
        (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
         prefix = '',
         sgkey = D3PDMakerFlags.ElectronSGKey(),
         typeName = 'ElectronContainer',
         allowMissing = False):
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
    udprefix = (D3PDMakerFlags.EgammaUserDataPrefix() + '_' +
                resolved_sgkey + '_')

    oqname = 'egammaCalcOQAlg_' + resolved_sgkey
    if not hasattr (seq, oqname):
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += egammaOQFlagsBuilderBase()
        seq += egammaD3PDAnalysis.egammaCalcOQAlg \
               (oqname,
                egammaOQFlagsBuilder = egammaOQFlagsBuilderBase(),
                Getter = DVGetter
                  (prefix + 'egammaCalcOQGetter',
                   TypeName = typeName,
                   SGKey = sgkey),
                AllowMissing = allowMissing,
                UDPrefix = udprefix
                )

    return
