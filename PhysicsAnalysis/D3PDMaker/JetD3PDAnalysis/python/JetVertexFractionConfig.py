# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file JetD3PDAnalysis/python/JetVertexFractionConfig.py
# @author scott snyder <snyder@bnl.gov>
# @date Jul, 2011
# @brief Configure algorithms to fill JVF information into UserData.
#


from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags
import JetD3PDAnalysis
import D3PDMakerCoreComps

from AthenaCommon.AlgSequence         import AlgSequence
from D3PDMakerCoreComps.resolveSGKey  import resolveSGKey


def JetVertexFractionConfig \
        (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
         prefix = '',
         sgkey = D3PDMakerFlags.JetSGKey(),
         typeName = 'JetCollection'):
    """Configure algorithms required to fill JVF information into UserData.

    SEQ is the Gaudi sequence to which the algorithms should be added.
    Default is that given by PreD3PDAlgSeqName.

    PREFIX is a prefix to add to the names of the algorithms scheduled.

    SGKEY/TYPENAME is the StoreGate key of the input jet container
    and the name of its type.
"""
    DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool

    if (not D3PDMakerFlags.MakeJetUserData() or
        D3PDMakerFlags.HaveJetUserData()):
        return

    resolved_sgkey = resolveSGKey (typeName, sgkey)

    jvfname = 'JetVertexFractionAlg_' + resolved_sgkey
    if not hasattr (seq, jvfname):
        # Need to rebuild track associations...
        from JetD3PDAnalysis.JetMomentCalcConfig import JetMomentCalcConfig
        JetMomentCalcConfig (resolved_sgkey, None, seq=seq)

        from JetMomentTools.SetupJetMomentTools\
             import getJetVertexAssociationTool
        seq += JetD3PDAnalysis.JetVertexFractionAlg \
               (jvfname,
                Getter = DVGetter 
                  (prefix + 'JetVertexFractionGetter',
                   TypeName = typeName,
                   SGKey = sgkey),
                JetVertexAssociationTool =
                  # Parameters are irrelevant; these will work for any jets.
                  getJetVertexAssociationTool ('AntiKt', 0.4, 'Topo'),
                UDPrefix = D3PDMakerFlags.JetUserDataPrefix())

