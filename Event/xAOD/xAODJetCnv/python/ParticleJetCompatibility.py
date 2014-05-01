# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# this line is important !
#import JetRec.JetEventSetup

def _setup_remapping():



    from AthenaCommon import CfgMgr
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr(svcMgr, 'AddressRemappingSvc'):
        svcMgr += CfgMgr.AddressRemappingSvc()
    if not hasattr(svcMgr, 'ProxyProviderSvc'):
        svcMgr += CfgMgr.ProxyProviderSvc()
    svcMgr.ProxyProviderSvc.ProviderNames += [ "AddressRemappingSvc" ]
    def conversion_string(old, new):
        # return "ParticleJetContainer#%s->JetCollection#%s"%(old, new)
        _clid = "1118613496" # ParticleJetContainer clid
        return "%s#%s->JetCollection#%s"%(_clid, old, new)
    def conversion_aodtower(old, new):
        _clid = "1162448536" # JetCollection clid 
        return "%s#%s->JetCollection,1324108263#%s"%(_clid, old, new)


    # Temporary Fix... Can we automatically generate the names of the collections?
    # Note: We can't re-map H1TowerJets to two different (TowerAOD, ParticleJets) keys
    # so we have to do it in a two step-process, Particle Jets -> TowerAOD, TowerAOD->Tower
    svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps += [ conversion_string(o,n) for (o,n) in [
        ("Kt6H1TopoParticleJets" ,   "Kt6H1TopoJets"),
        ("Kt4H1TopoParticleJets" ,   "Kt4H1TopoJets"),
        ("Cone7H1TopoParticleJets" , "Cone7H1TopoJets"),
        ("Cone4H1TopoParticleJets" , "Cone4H1TopoJets"),
        ("Kt6H1TowerParticleJets" ,  "Kt6H1TowerAODJets"),
        ("Kt4H1TowerParticleJets" ,  "Kt4H1TowerAODJets"),
        ("Cone7H1TowerParticleJets" ,"Cone7H1TowerAODJets"),
        ("Cone4H1TowerParticleJets" ,"Cone4H1TowerAODJets"),
        ("Kt6TruthParticleJets" ,    "Kt6TruthJets"),
        ("Kt4TruthParticleJets" ,    "Kt4TruthJets"),
        ("Cone7TruthParticleJets" ,  "Cone7TruthJets"),
        ("Cone4TruthParticleJets" ,  "Cone4TruthJets"),
        ("AtlfastParticleJetContainer","AtlfastJetContainer")
        ] ]

    svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps += [ conversion_aodtower(o,n) for (o,n) in [
        ("Kt6H1TowerAODJets" ,  "Kt6H1TowerJets"),
        ("Kt4H1TowerAODJets" ,  "Kt4H1TowerJets"),
        ("Cone7H1TowerAODJets" ,"Cone7H1TowerJets"),
        ("Cone4H1TowerAODJets" ,"Cone4H1TowerJets"),
        ("AntiKt6H1TowerAODJets" ,  "AntiKt6H1TowerJets"),
        ("AntiKt4H1TowerAODJets" ,  "AntiKt4H1TowerJets")
        ] ]

    # read pre16  names
    # complicated for towers because of the H1 suppression and TowerAOD
    svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps += [ conversion_aodtower(o,n) for (o,n) in [
        ("Kt6H1TowerJets" ,  "Kt6TowerAODJets"),
        ("Kt4H1TowerJets" ,  "Kt4TowerAODJets"),
        ("Cone7H1TowerJets" ,"Cone7TowerAODJets"),
        ("Cone4H1TowerJets" ,"Cone4TowerAODJets"),
        ("AntiKt6H1TowerJets" ,  "AntiKt6TowerAODJets"),
        ("AntiKt4H1TowerJets" ,  "AntiKt4TowerAODJets"),

        ("Kt6TowerAODJets" ,  "Kt6TowerJets"),
        ("Kt4TowerAODJets" ,  "Kt4TowerJets"),
        ("Cone7TowerAODJets" ,"Cone7TowerJets"),
        ("Cone4TowerAODJets" ,"Cone4TowerJets"),
        ("AntiKt6TowerAODJets" ,  "AntiKt6TowerJets"),
        ("AntiKt4TowerAODJets" ,  "AntiKt4TowerJets"),

        ("Kt6H1TopoJets" ,  "Kt6TopoJets"),
        ("Kt4H1TopoJets" ,  "Kt4TopoJets"),
        ("Cone7H1TopoJets" ,"Cone7TopoJets"),
        ("Cone4H1TopoJets" ,"Cone4TopoJets"),
        ("AntiKt6H1TopoJets" ,  "AntiKt6TopoJets"),
        ("AntiKt4H1TopoJets" ,  "AntiKt4TopoJets"),

        ] ]

    svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps += [ conversion_aodtower(o,n) for (o,n) in [
        ] ]
                                                         
    return

_setup_remapping()
del _setup_remapping
