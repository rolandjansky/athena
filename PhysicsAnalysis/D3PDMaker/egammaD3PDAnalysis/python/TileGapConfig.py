# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDAnalysis/python/TileGapConfig.py
# @author sss
# @date Jul, 2014
# @brief Configure TileGapSumAlg to fill aux data.
#


from D3PDMakerConfig.D3PDMakerFlags            import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey           import resolveSGKey
from D3PDMakerCoreComps                        import SGDataVectorGetterTool
from egammaD3PDAnalysis.TileGapSelectionGetter import TileGapSelectionGetter
from AthenaCommon.AlgSequence                  import AlgSequence
import D3PDMakerCoreComps
import egammaD3PDAnalysis


def TileGapConfig \
    (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
     prefix = '',
     sgkey = 'egClusterCollection',
     typeName = 'DataVector<xAOD::CaloCluster_v1>',
     allowMissing = False):
    """Configure egammaMaxECellAlg for D3PD making.

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

    algName = 'TileGapSumAlg' + resolved_sgkey
    if not hasattr (seq, algName):

        gapsel = TileGapSelectionGetter (seq)
        myAlg = egammaD3PDAnalysis.TileGapSumAlg \
                (algName,
                 ClusterGetter = DVGetter
                    (prefix + 'TileGapAlgGetter',
                    TypeName = typeName,
                    SGKey = sgkey),
                 CellGetter = SGDataVectorGetterTool
                    (prefix + 'TileGapAlgCellGetter',
                     SGKey = gapsel.outputKey(),
                     TypeName = gapsel.outputType()),
                 AllowMissing = allowMissing,
                 AuxPrefix = auxprefix,)

        seq += myAlg

    return
