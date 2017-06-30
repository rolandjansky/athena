# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from egammaD3PDAnalysis.egammaSwTool          import egammaSwToolNone
from D3PDMakerCoreComps.resolveSGKey          import resolveSGKey
from RecExConfig.RecFlags                     import rec
import egammaD3PDAnalysis
import D3PDMakerCoreComps

from AthenaCommon.AlgSequence import AlgSequence

import cppyy
cppyy.loadDictionary('egammaInterfacesEnumsDict')
from ROOT import egammaIsolationType


def PhotonUserDataConfig \
        (seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
         prefix = '',
         sgkey = D3PDMakerFlags.PhotonSGKey(),
         typeName = 'PhotonContainer'):
    """Configure algorithms required for D3PD making for photons.

    SEQ is the Gaudi sequence to which the algorithms should be added.
    Default is that given by PreD3PDAlgSeqName.

    PREFIX is a prefix to add to the names of the algorithms scheduled.

    SGKEY/TYPENAME is the StoreGate key of the input electron container
    and the name of its type.
"""

    DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool
    
    if (not D3PDMakerFlags.MakeEgammaUserData() or
        D3PDMakerFlags.HaveEgammaUserData()):
        return

#not use MCTruthClassifier to avoid crash
#    if rec.doTruth():
#        ptaname = prefix + 'PhotonTruthAlg'
#        if not hasattr (seq, ptaname):
#            seq += egammaD3PDAnalysis.PhotonTruthAlg \
#                   (ptaname,
#                    PhotonGetter = DVGetter
#                      (prefix + 'PhotonTruthAlgGetter',
#                       TypeName = typeName,
#                       SGKey = sgkey),
#                    UDPrefix = D3PDMakerFlags.EgammaUserDataPrefix())
#

    ptianame = prefix + 'PhotonTopoIsoAlg'
    if not hasattr (seq, ptianame):
        seq += egammaD3PDAnalysis.PhotonTopoIsoAlg \
               (ptianame,
                PhotonGetter = DVGetter
                  (prefix +'PhotonTopoIsoAlgPhotonGetter',
                   TypeName = typeName,
                   SGKey = sgkey),
                ClusterGetter = DVGetter
                  (prefix + 'PhotonTopoIsoAlgClusterGetter',
                   TypeName = 'CaloClusterContainer',
                   SGKey = D3PDMakerFlags.ClusterSGKey()),
                UDPrefix = D3PDMakerFlags.EgammaUserDataPrefix())

        
    emax2gamname = prefix + 'DeltaEmax2GamAlg'
    if not hasattr (seq, emax2gamname):
        seq += egammaD3PDAnalysis.egammaDeltaEmax2Alg \
               (emax2gamname,
                Getter = DVGetter 
                  (prefix + 'DeltaEmax2GamGetter',
                   TypeName = typeName,
                   SGKey = sgkey),
                UDPrefix = D3PDMakerFlags.EgammaUserDataPrefix())


    resname = resolveSGKey (typeName, sgkey)
    gamrawclustname = 'GamRawClusterAlg_' + resname
    if not hasattr (seq, gamrawclustname):
        seq += egammaD3PDAnalysis.RawClusterAlg \
               (gamrawclustname,
                Getter = DVGetter 
                  (prefix + 'GamRawClusterAlgGetter',
                   TypeName = typeName,
                   SGKey = sgkey),
                RawClustersName =
                  resname + D3PDMakerFlags.RawClustersSGKeySuffix(),
                RawClustersAssocName =
                  resname + D3PDMakerFlags.RawClustersAssocSGKeySuffix(),
                ClusterCorrectionTool = egammaSwToolNone())

    isoname = prefix + 'egammaIsIsoAlg'
    if not hasattr (seq, isoname):
        from egammaPIDTools.egammaIsolationMVATopToolBase \
             import egammaIsolationMVATopToolBase
        egammaIsolationMVATopToolBase ("egammaIsolationMVATopTool")

        seq += egammaD3PDAnalysis.egammaIsIsoAlg \
               (isoname,
                Getter = DVGetter
                  (prefix + 'egammaIsIsoGetter',
                   TypeName = typeName,
                   SGKey = sgkey),
                UDPrefix = D3PDMakerFlags.EgammaUserDataPrefix(),
                IsoTypes = [egammaIsolationType.iso99,
                            egammaIsolationType.iso98,
                            egammaIsolationType.iso95,
                            egammaIsolationType.iso90,

                            egammaIsolationType.caloIso99,
                            egammaIsolationType.caloIso98,
                            egammaIsolationType.caloIso95,
                            egammaIsolationType.caloIso90,
                            egammaIsolationType.caloIso80,

                            egammaIsolationType.trackIso99,
                            egammaIsolationType.trackIso98,
                            egammaIsolationType.trackIso95,

                            egammaIsolationType.likelihoodIso95,
                            egammaIsolationType.likelihoodIso90,
                            egammaIsolationType.likelihoodIso80,
                            ])
