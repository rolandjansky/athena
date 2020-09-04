# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file egammaD3PDMaker/python/ClusterCorrectionD3PD.py
# @author scott snyder <snyder@bnl.gov>
# @date Oct, 2012
# @brief D3PD for EM cluster correction studies.
#


from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from egammaD3PDMaker.ElectronD3PDObject       import ElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject         import PhotonD3PDObject
from egammaD3PDMaker.egammaTruthD3PDObject    import egammaTruthD3PDObject
from egammaD3PDMaker.CorrectionClusterD3PDObject import CorrectionClusterD3PDObject
from egammaD3PDMaker.CorrectionClusterGetter  import make_CorrectionClusterGetter
from CaloSysD3PDMaker.ClusterD3PDObject       import ClusterD3PDObject
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from RecExConfig.RecFlags                     import rec
from AthenaCommon.AlgSequence                 import AlgSequence


def addCluster (alg, typ, eta_size, phi_size, prefix):
    prealg = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
    getter = make_CorrectionClusterGetter(typ, eta_size, phi_size)(seq = prealg)
    sgkey = getter.outputKey()
    # Needs to be redone now that CaloRunClusterCorrections is gone.
    # Leave this commented out for now.
    #runner = [c for c in getter.caloClusterMakerHandle().getChildren()
    #          if c.getName().startswith ('CaloRunClusterCorrections')][0]
    #tokeep = ['rfac', 'etaoffb1', 'gap', 'calhits', 'phimod', 'etamod']
    #keeplist = []
    #for c in tokeep:
    #    keeplist.append (c)
    #    keeplist.append ('pre' + c + '_' + sgkey)
    #runner.KeepList = keeplist

    alg += CorrectionClusterD3PDObject (10,
                                        sgkey = sgkey,
                                        prefix = prefix)

    #for c in tokeep:
    #    alg += CorrectionClusterD3PDObject (10,
    #                                        sgkey = 'pre' + c + '_' + sgkey,
    #                                        prefix = prefix + 'pre' + c + '_')
    return
    


def ClusterCorrectionD3PD (alg):

    alg += EventInfoD3PDObject (10)

    alg += ElectronD3PDObject (10,
                               exclude = ['L1Index', 'L2Index', 'EFIndex',
                                          'trackPVPerigeeAssoc',
                                          'trackPerigeeAssoc'])
    alg += PhotonD3PDObject (10,
                             exclude = ['L1Index', 'L2Index', 'EFIndex'])

    alg += ClusterD3PDObject (10, sgkey = 'egClusterCollection')

    addCluster (alg, 'ele', 5, 5, 'cl_ele55_')
    addCluster (alg, 'ele', 3, 5, 'cl_ele35_')
    addCluster (alg, 'ele', 3, 7, 'cl_ele37_')

    addCluster (alg, 'gam', 5, 5, 'cl_gam55_')
    addCluster (alg, 'gam', 3, 5, 'cl_gam35_')
    addCluster (alg, 'gam', 3, 7, 'cl_gam37_')

    if rec.doTruth():
        alg += egammaTruthD3PDObject (10, prefix = 'mc_')
    
    return
