# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDAnalysis/python/PhotonTruthConfig.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2011
# @brief Configure PhotonTruthAlg to fill UserData.
#


from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey
from AthenaCommon.AlgSequence                import AlgSequence
from RecExConfig.RecFlags                    import rec
import D3PDMakerCoreComps
import egammaD3PDAnalysis


def PhotonTruthConfig \
        (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
         prefix = '',
         sgkey = D3PDMakerFlags.ElectronSGKey(),
         typeName = 'ElectronContainer',
         allowMissing = False):
    """Configure PhotonTruthAlg for D3PD making.

    SEQ is the Gaudi sequence to which the algorithm should be added.
    Default is that given by PreD3PDAlgSeqName.

    PREFIX is a prefix to add to the name of the algorithm scheduled.

    SGKEY/TYPENAME is the StoreGate key of the input electron container
    and the name of its type.

    If ALLOWMISSING is true, don't fail if the SG key doesn't exist.
"""

    if not rec.doTruth():
        return

    if (not D3PDMakerFlags.MakeEgammaUserData() or
        D3PDMakerFlags.HaveEgammaUserData()):
        return

    DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool
    resolved_sgkey = resolveSGKey (typeName, sgkey)
    auxprefix = (D3PDMakerFlags.EgammaUserDataPrefix() + '_' +
                 resolved_sgkey + '_')

    from TruthD3PDMaker.MCTruthClassifierConfig \
         import D3PDMCTruthClassifier
    ptaname = 'PhotonTruthAlg_' + resolved_sgkey
    if not hasattr (seq, ptaname):
        seq += egammaD3PDAnalysis.PhotonTruthAlg \
               (ptaname,
                PhotonGetter = DVGetter
                  (prefix + 'PhotonTruthAlgGetter',
                   TypeName = typeName,
                   SGKey = sgkey),
                AllowMissing = allowMissing,
                AuxPrefix = auxprefix,
                TruthTool = egammaD3PDAnalysis.PhotonTruthTool
                  ('D3PD__PhotonTruthTool',
                   Classifier = D3PDMCTruthClassifier))

    return
