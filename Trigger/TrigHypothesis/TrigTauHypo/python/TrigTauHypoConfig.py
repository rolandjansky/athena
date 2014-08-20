# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##------------------------------------------
##
## hypothesis for v2 and v3, meant for MC production
##
##------------------------------------------

 # get defaults
from TrigTauHypo.TrigTauHypoBase import*


## EFTauInv
class EFTauInvHypoBase (EFTauInvHypo):
    __slots__ = []
    def __init__(self, name):
        super(EFTauInvHypoBase, self).__init__(name)
        setTauEFMonTools(self)

class EFTauInvHypo_tauNoCut (EFTauInvHypoBase):
    __slots__ = []
    def __init__(self, name = "EFTauInvHypo_tauNoCut"):
        super(EFTauInvHypo_tauNoCut, self).__init__(name)
        self.NTrackMin = -1
        self.NTrackMax = 100000
        self.forceAccept = True

class EFTauInvHypo (EFTauInvHypoBase):
    __slots__ = []
    def __init__(self, name = "EFTauInvHypo", var = [], val = []):
        super(EFTauInvHypo, self).__init__(name)
        if name.lower().find("loose") != -1:
            applyDefaults(self, EFTau_defaults_loose)
        if name.lower().find("medium") != -1:
            applyDefaults(self, EFTau_defaults_medium)
        if name.lower().find("tight") != -1:
            applyDefaults(self, EFTau_defaults_tight)
        setVarCut(self, var, val)



##
## T2CaloTau Hypos:
##
variables=['EtRawMin','EmRadius3SMax']
thresholds ={
    12: [  9400.,0.1411],
    16: [ 12400.,0.1310],
    20: [ 16700.,0.1179],
    29: [ 23200.,0.1015],
    38: [ 28300.,0.0847],
    50: [ 44300.,0.0779],
    84: [ 72100.,0.0686],
    100:[ 88500.,0.0687],
    125:[104300.,0.0654]
    }
hypos.update(dict([ ('l2calo_tau%d_loose'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_loose'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


thresholds = {
    16: [ 12400., 0.1189],
    20: [ 16700., 0.1049],
    29: [ 23200., 0.0962],
    38: [ 28300., 0.0823],
    50: [ 44300., 0.0726],
    84: [ 72100., 0.0663],
    100:[ 88500., 0.0640],
    125:[104300., 0.0621]
    }
hypos.update(dict([ ('l2calo_tau%d_medium'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_medium'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


thresholds = {
    20: [ 16700.,0.1049],
    29: [ 23200.,0.0962],
    38: [ 28300.,0.0823],
    125:[104300.,0.0621]
    }
hypos.update(dict([ ('l2calo_tau%d_medium1'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_medium1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))



##
## T2IDTau Hypos:
##
variables=['NTrkMax','SumPtRatioMax']

thresholds = {
    12: [10,0.1],
    16: [10,0.1],
    20: [10,0.1],
    29: [10,0.1],
    38: [10,0.1],
    50: [10,0.1],
    84: [10,0.1],
    100:[10,0.1],
    125:[ 7,0.1]
    }
hypos.update(dict([ ('l2id_tau%d_loose'%t, T2IDTauHypo('T2IDTauHypo_tau%d_loose'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    16: [7,0.05],
    20: [7,0.05],
    29: [7,0.05],
    38: [7,0.05],
    50: [7,0.05],
    84: [7,0.02],
    100:[7,0.02],
    125:[7,0.02]
    }
hypos.update(dict([ ('l2id_tau%d_medium'%t, T2IDTauHypo('T2IDTauHypo_tau%d_medium'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    20: [4,0.05],
    29: [4,0.05],
    38: [4,0.05],
    125:[4,0.02]
    }
hypos.update(dict([ ('l2id_tau%d_medium1'%t, T2IDTauHypo('T2IDTauHypo_tau%d_medium1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


##
## T2Tau Hypos:
##
variables=['EmRadius3S1pMax']
thresholds = {
    12: [0.1074],
    16: [0.0996],
    20: [0.0833],
    29: [0.0705],    
    38: [0.0702],
    50: [0.0587],
    84: [0.0494],
    100:[0.0481],
    125:[0.0463]
    }
hypos.update(dict([ ('l2_tau%d_loose'%t, T2TauHypo('T2TauHypo_tau%d_loose'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


thresholds = {
    16: [0.0809],
    20: [0.0696],
    29: [0.0618],
    38: [0.0521],
    50: [0.0495],
    84: [0.0445],
    100:[0.0426],
    125:[0.0414]
    }
hypos.update(dict([ ('l2_tau%d_medium'%t, T2TauHypo('T2TauHypo_tau%d_medium'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


thresholds = {
    20: [0.0696],
    29: [0.0618],
    38: [0.0521],
    125:[0.0414]
    }
hypos.update(dict([ ('l2_tau%d_medium1'%t, T2TauHypo('T2TauHypo_tau%d_medium1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


##
## EFTauInv Hypos:
##
variables=['NTrackMax','EtCalibMin']

thresholds = {
    12: [10, 12.*GeV],
    16: [10, 16.*GeV],
    20: [10, 20.*GeV],
    29: [10, 29.*GeV],
    38: [20, 38.*GeV],
    50: [20, 50.*GeV],
    84: [20, 84.*GeV],
    100:[20,100.*GeV],
    125:[20,125.*GeV]
    }


hypos.update(dict([ ('ef_tau%d_loose'%t, EFTauInvHypo('EFTauInvHypo_tau%d_loose'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    16: [6, 16.*GeV],
    20: [6, 20.*GeV],
    29: [6, 29.*GeV],
    38: [6, 38.*GeV],
    50: [6, 50.*GeV],
    84: [6, 84.*GeV],
    100:[6,100.*GeV],
    125:[6,125.*GeV]
    }
hypos.update(dict([ ('ef_tau%d_medium'%t, EFTauInvHypo('EFTauInvHypo_tau%d_medium'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    20: [3, 20.*GeV],
    29: [3, 29.*GeV],
    38: [3, 38.*GeV],
    125:[3,125.*GeV]
    }
hypos.update(dict([ ('ef_tau%d_medium1'%t, EFTauInvHypo('EFTauInvHypo_tau%d_medium1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


##
## Others
##
## tauConfig_tauNoCut
hypos.update(dict([ ('l2calo_tauNoCut', T2CaloTauHypo_tauNoCut('T2CaloTauHypo_tauNoCut')) ]))
hypos.update(dict([ ('l2id_tauNoCut',   T2IDTauHypo_tauNoCut  ('T2IDTauHypo_tauNoCut')) ]))
hypos.update(dict([ ('l2_tauNoCut',     T2TauHypo_tauNoCut    ('T2TauHypo_tauNoCut')) ]))
hypos.update(dict([ ('ef_tauNoCut',     EFTauInvHypo_tauNoCut ('EFTauInvHypo_tauNoCut')) ]))


hypos.update(dict([ ('l2calo_tau29_loose1',
                     T2CaloTauHypo('T2CaloTauHypo_tau29_loose1',
                                   ['EtRawMin','EmRadius3SMax'], [0.1015, 23200.])) ]))
hypos.update(dict([ ('l2id_tau29_loose1',
                     T2IDTauHypo('T2IDTauHypo_tau29_loose1',
                                 ['NTrkMax','SumPtRatioMax'], [4, 0.1])) ]))
hypos.update(dict([ ('l2_tau29_loose1',
                     T2TauHypo('T2TauHypo_tau29_loose1',
                               ['EmRadius3S1pMax'], [0.0705])) ]))
hypos.update(dict([ ('ef_tau29_loose1',
                     EFTauInvHypo('EFTauInvHypo_tau29_loose1',
                                  ['NTrackMax','EtCalibMin'], [3, 29.*GeV])) ]))


## _calo chains for IDTrkNoCut
variables=['EtRawMin','EmRadius3SMax']

thresholds = {
    12: [  9400.,0.1411],
    16: [ 12400.,0.1310],
    29: [ 23200.,0.1015],
    50: [ 44300.,0.0779],
    125:[104300.,0.0654]
    
    }
hypos.update(dict([ ('l2calo_tau%d_calo'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_calo'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


variables=['NTrkMin','NTrkMax','SumPtRatioMax']
thresholds = {
    12: [0,100,10.],
    16: [0,100,10.],
    29: [0,100,10.],
    50: [0,100,10.],
    125:[0,100,10.],
    }
hypos.update(dict([ ('l2id_tau%d_calo'%t, T2IDTauHypo('T2IDTauHypo_tau%d_calo'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


variables=['EmRadius3S1pMax']
thresholds = {
    12: [0.1074],
    16: [0.0996],
    29: [0.0705],
    50: [0.0587],
    125:[0.0463]
    }
hypos.update(dict([ ('l2_tau%d_calo'%t, T2TauHypo('T2TauHypo_tau%d_calo'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


variables = ['NTrackMin','NTrackMax','EtCalibMin']
thresholds = {
    12: [0,100, 12.*GeV,],
    16: [0,100, 16.*GeV,],
    29: [0,100, 29.*GeV,],
    50: [0,100, 50.*GeV ],
    125:[0,1000,125.*GeV]
    }
hypos.update(dict([ ('ef_tau%d_calo'%t, EFTauInvHypo('EFTauInvHypo_tau%d_calo'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


## ## trk_id
variables  = ['NTrkMax','LdTrkPtMin','SumPtRatioMax']
thresholds = [1,         9.*GeV,      0.1]

hypos.update({'l2id_trk9_id': T2IDTauHypo('T2IDTauHypo_trk9_id',variables,thresholds)} )



