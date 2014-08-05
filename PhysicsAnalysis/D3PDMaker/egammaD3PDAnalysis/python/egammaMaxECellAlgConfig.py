# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDAnalysis/python/egamaMaxECellAlgConfig.py
# @author Nikiforos K. Nikiforou <nikiforo@cern.ch> modified from egammaIsIsoConfig.py by scott snyder <snyder@bnl.gov>
# @date Jun, 2012
# @brief Configure egamaMaxECellAlg to fill aux data.
#


from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey
from AthenaCommon.AlgSequence                import AlgSequence
import D3PDMakerCoreComps
import egammaD3PDAnalysis


def egammaMaxECellAlgConfig \
    (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
    prefix = '',
    sgkey = D3PDMakerFlags.ElectronSGKey(),
    typeName = 'ElectronContainer',
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

    algName = 'egammaMaxECellAlg' + resolved_sgkey
    if not hasattr (seq, algName):
        
        myAlg = egammaD3PDAnalysis.egammaMaxECellAlg \
                (algName,
                Getter = DVGetter
                    (prefix + 'egammaMaxECellAlgGetter',
                    TypeName = typeName,
                    SGKey = sgkey),
                AllowMissing = allowMissing,
                AuxPrefix = auxprefix,)

        seq += myAlg

    return
