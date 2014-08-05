# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDAnalysis/python/egammaDeltaEmax2Config.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2011
# @brief Configure egammaDeltaEmax2Alg to fill UserData.
#


from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey         import resolveSGKey
from AthenaCommon.AlgSequence                import AlgSequence
import D3PDMakerCoreComps
import egammaD3PDAnalysis


def egammaDeltaEmax2Config \
        (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
         prefix = '',
         sgkey = D3PDMakerFlags.ElectronSGKey(),
         typeName = 'DataVector<xAOD::Electron_v1>',
         allowMissing = False):
    """Configure egammaDeltaEmax2Alg for D3PD making.

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

    emax2name = 'DeltaEmax2Alg_' + resolved_sgkey
    if not hasattr (seq, emax2name):
        highlum = False
        if typeName == 'ElectronContainer':
            from AthenaCommon.BeamFlags import jobproperties        
            if jobproperties.Beam.numberOfCollisions() >= 20 :
                highlum = True

        seq += egammaD3PDAnalysis.egammaDeltaEmax2Alg \
               (emax2name,
                Getter = DVGetter 
                  (prefix + 'DeltaEmax2Getter',
                   TypeName = typeName,
                   SGKey = sgkey),
                AllowMissing = allowMissing,
                HighLum = highlum,
                AuxPrefix = auxprefix)

    return
