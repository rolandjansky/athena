# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: addTauD3PDInfo.py
# by Lei Zhou
# add tau info to D3PD stream

# Athena imports
from RecExConfig.RecFlags                           import rec
from AthenaCommon.AlgSequence                       import AlgSequence
topSequence = AlgSequence()


from EventCommonD3PDMaker.DRIndexAssociation        import DRIndexAssociation
from EventCommonD3PDMaker.DRIndexMultiAssociation   import DRIndexMultiAssociation
#from TauD3PDMaker.TauD3PDObject                     import TauD3PDObject
#from JetD3PDMaker.JetD3PDObject                     import JetD3PDObject
from D3PDMakerConfig.D3PDMakerFlags                 import D3PDMakerFlags
from D3PDMakerConfig.CommonD3PDMakerFlags           import CommonD3PDMakerFlags
from TauD3PDMaker.EflowObjectsD3PDObject            import EflowObjectsD3PDObject
#from MuonD3PDMaker.MuonD3PDObject                   import MuonD3PDObject
from D3PDMakerConfig.CoreCommonD3PD                 import myMuonD3PDObject
from TauD3PDMaker.TauAnalysisTriggerBitsD3PDObject  import TauAnalysisTriggerBitsD3PDObject
#from MissingETD3PDMaker.MissingETD3PDTriggerBitsObject import METD3PDTriggerBitsObject
from TauD3PDMaker.EventTauD3PDObject                import EventTauD3PDObject

from D3PDMakerConfig.CoreCommonD3PD                import ExtendedJetD3PDObject
from D3PDMakerConfig.CoreCommonD3PD                import ExtendedTauD3PDObject

#from D3PDMakerCoreComps.SimpleAssociation           import SimpleAssociation
#from D3PDMakerCoreComps.IndexAssociation            import IndexAssociation

if rec.doTruth():
    from TruthD3PDAnalysis.truthParticleConfig          import truthParticleConfig
    from TruthD3PDMaker.TruthParticleD3PDObject         import TruthParticleD3PDObject
    from TauD3PDMaker.TruthTauD3PDObject                import TruthTauD3PDObject
    from TauD3PDMaker.TruthTauFullD3PDObject            import TruthTauFullD3PDObject
    import TruthD3PDMaker

def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw



def addTauD3PDInfo  ( alg = None,
                      file = 'NTUP_PHYSICS.root',
                      tuplename = 'physics',
                      doPhys = True,
                      doCell = False,
                      doHad = True,
                      seq = topSequence,
                      **kw ):

    print "adding tau info..."

    flags=CommonD3PDMakerFlags

    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream(tuplename, file, asAlg=True)
        pass

    #ExtendedTauD3PDObject = TauD3PDObject.copy()

    TauMuonAssoc = DRIndexMultiAssociation(
        parent = ExtendedTauD3PDObject,
        type_name = 'Analysis::MuonContainer',
        default_sgkey = D3PDMakerFlags.MuonSGKey(),
        default_drcut = 0.2,
        prefix = 'muAssoc_',
        target = 'mu_' )
    TauElectronAssoc = DRIndexMultiAssociation(
        parent = ExtendedTauD3PDObject,
        type_name = 'ElectronContainer',
        default_sgkey = D3PDMakerFlags.ElectronSGKey(),
        default_drcut = 0.2,
        prefix = 'elAssoc_',
        target = 'el_' )
    TauJetAssoc = DRIndexMultiAssociation(
        parent = ExtendedTauD3PDObject,
        type_name = 'JetCollection',
        default_sgkey = D3PDMakerFlags.JetSGKey(),
        default_drcut = 0.4,
        prefix = 'jetAssoc_',
        target = 'jet_AntiKt4LCTopo_' )
    #JetMuonAssoc = DRIndexMultiAssociation(
    #    parent = ExtendedJetD3PDObject,
    #    type_name = 'Analysis::MuonContainer',
    #    default_sgkey = D3PDMakerFlags.MuonSGKey(),
    #    default_drcut = 0.4,
    #    prefix = 'muAssoc_',
    #    target = 'mu_' )
    #JetElectronAssoc = DRIndexMultiAssociation(
    #    parent = ExtendedJetD3PDObject,
    #    type_name = 'ElectronContainer',
    #    default_sgkey = D3PDMakerFlags.ElectronSGKey(),
    #    default_drcut = 0.4,
    #    prefix = 'elAssoc_',
    #    target = 'el_' )
    #JetTauAssoc = DRIndexMultiAssociation(
    #    parent = ExtendedJetD3PDObject,
    #    type_name = 'Analysis::TauJetContainer',
    #    default_sgkey = D3PDMakerFlags.TauSGKey(),
    #    default_drcut = 0.4,
    #    prefix = 'tauAssoc_',
    #    target = 'tau_' )

    #associate an EFO to a tau
    EFOToTauAssoc = DRIndexAssociation(
        parent = EflowObjectsD3PDObject, #Parent: D3PD Object that should be associated to other objects
        type_name = 'Analysis::TauJetContainer', #type_name: Type of target container that has the object to assoc
        default_sgkey = D3PDMakerFlags.TauSGKey(),
        default_drcut = 0.4,
        prefix = 'EFOtoTauAssoc_',
        target = 'tau_' )

    #associate a tau to the efo's in a cone
    TauToEFOMultiAssoc = DRIndexMultiAssociation(
        parent = ExtendedTauD3PDObject,
        type_name = 'eflowObjectContainer',
        default_sgkey = 'eflowObjects_tauMode',
        default_drcut = 0.4,
        prefix = 'TauToEFOAssoc_',
        target = 'efo_' )

    if rec.doTruth():
    #    from TauD3PDMaker.TauD3PDMakerConf import TruthTausToSG
    #    if rec.readESD():
    #        TruthTausToSG.truth_container='INav4MomTruthEvent'# default is SpclMC (AOD)
    #    seq += TruthTausToSG()    
        
        #if doHad:
        #    truthParticleConfig(seq, sgkey=D3PDMakerFlags.TruthParticlesSGKey(), writeHadrons=True, writePartons=True, writeGeant=False)
        #    pass
        # tau_trueTauAssoc_index
        TauTrueTauAssoc = DRIndexAssociation(
            parent = ExtendedTauD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = 'TrueHadronicTaus',
            default_drcut = 0.2,
            prefix = 'trueTauAssoc_',
            target = 'trueTau_' )
        # trueTau_tauAssoc_index
        TrueTauTauAssoc = DRIndexAssociation(
            parent = TruthTauD3PDObject,
            type_name = 'Analysis::TauJetContainer',
            default_sgkey = D3PDMakerFlags.TauSGKey(),
            default_drcut = 0.2,
            prefix = 'tauAssoc_',
            target = 'tau_' )
        # tau_truthAssoc_index
        TauTruthAssoc = DRIndexMultiAssociation(
            parent = ExtendedTauD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
            default_drcut = 0.45,
            prefix = 'truthAssoc_',
            target = 'mc_' )
        # trueTau_truthAssoc_index
        TrueTauTruthAssoc = DRIndexMultiAssociation(
            parent = TruthTauD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
            default_drcut = 0.45,
            prefix = 'truthAssoc_',
            target = 'mc_' )

        MuonTruthAssoc = DRIndexMultiAssociation(
            parent = myMuonD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
            default_drcut = 0.2,
            prefix = 'truthAssoc_',
            target = 'mc_' )
        JetTruthAssoc = DRIndexMultiAssociation(
            parent = ExtendedJetD3PDObject,
            type_name = 'TruthParticleContainer',
            default_sgkey = D3PDMakerFlags.TruthParticlesSGKey(),
            default_drcut = 0.4,
            prefix = 'truthAssoc_',
            target = 'mc_' )
        # truth to reco
        TruthMuonAssoc = DRIndexMultiAssociation(
            parent = TruthParticleD3PDObject,
            type_name = 'Analysis::MuonContainer',
            default_sgkey = D3PDMakerFlags.MuonSGKey(),
            default_drcut = 0.2,
            prefix = 'muAssoc_',
            target = 'mu_' )

        #reco tau to truth egamma: tau_truthEgammaAssoc_index
        #TauTruthEgammaAssoc = DRIndexAssociation(
        #    parent = TauD3PDObject,
        #    type_name = 'TruthParticleContainer',
        #    default_sgkey = 'egammaTruth',
        #    default_drcut = 0.2,
        #    prefix = 'truthEgammaAssoc_',
        #    target = 'egtruth_')

        alg += TruthTauD3PDObject(**_args (10, 'TruthTau', kw))
        
    
    alg += EventTauD3PDObject(10)
    
    alg += EflowObjectsD3PDObject(10)

    alg += ExtendedTauD3PDObject              (**_args ( 10, 'Tau', kw, sgkey = D3PDMakerFlags.TauSGKey(),
                                                 include = flags.tauInclude(),
                                                 exclude = ['TauCellsDetails']))


    if D3PDMakerFlags.DoTrigger():
        alg += TauAnalysisTriggerBitsD3PDObject (10)

        

    return alg


