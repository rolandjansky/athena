# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##------------------------------------------
## 
## Hypothesis for v4, 2012 run
## L2 cuts optimized for mu=30
##
##------------------------------------------
# get defaults
from TrigTauHypo.TrigTauHypoBase import*
from TrigTauHypo.TrigTauHypoMonitoring \
     import setTauT2CoreMonTools, setTauT2IsoMonTools, setHadCalibEFMonTools

## T2IDCoreTau TEST
class T2IDCoreTauHypo_tau29_medium_2stTest (T2IDCoreTauHypo):
    __slots__ = []
    def __init__(self, name = "T2IDCoreTauHypo_tau29_medium_2stTest", var = [], val = []):
        super(T2IDCoreTauHypo_tau29_medium_2stTest, self).__init__(name)
        setTauT2CoreMonTools(self)
        setVarCut(self, var, val)
        
## T2IDIsoTau TEST
class T2IDIsoTauHypo_tau29_medium_2stTest (T2IDIsoTauHypo):
    __slots__ = []
    def __init__(self, name = "T2IDIsoTauHypo_tau29_medium_2stTest", var = [], val = []):
        super(T2IDIsoTauHypo_tau29_medium_2stTest, self).__init__(name)
        setTauT2IsoMonTools(self)
        setVarCut(self, var, val)

## EFTauMV
class EFTauMVHypoBase (EFTauMVHypo):
    __slots__ = []
    def __init__(self, name):
        super(EFTauMVHypoBase, self).__init__(name)
        setTauEFMonTools(self)

class EFTauMVHypo_tauNoCut (EFTauMVHypoBase):
    __slots__ = []
    def __init__(self, name = "EFTauMVHypo_tauNoCut"):
        super(EFTauMVHypo_tauNoCut, self).__init__(name)
        self.NTrackMin   = -1
        self.NTrackMax   = 10000
        self.Level       = -1111 # none
        self.forceAccept = True

class EFTauMVHypo (EFTauMVHypoBase):
    __slots__ = []
    def __init__(self, name = "EFTauMVHypo", var = [], val = []):
        super(EFTauMVHypo, self).__init__(name)
        self.NTrackMin = 1 #for all items
        self.Method    = 2 #default bdt as of March 22
        setVarCut(self, var, val)

class EFTauMVHypo_highpt (EFTauMVHypoBase):
    __slots__ = []
    def __init__(self, name = "EFTauMVHypo_highpt", var = [], val = []):
        super(EFTauMVHypo_highpt, self).__init__(name)
        self.NTrackMin = 1 #for all items
        self.Method    = 2 #default bdt as of March 22
        self.Highpt    = True
        setVarCut(self, var, val)

## EFTauDiKaon
class EFTauDiKaonHypoBase (EFTauDiKaonHypo):
    __slots__ = []
    def __init__(self, name):
        super(EFTauDiKaonHypoBase, self).__init__(name)
        setTauEFMonTools(self)

class EFTauDiKaonHypo (EFTauDiKaonHypoBase):
    __slots__ = []
    def __init__(self, name = "EFTauDiKaonHypo", var = [], val = []):
        super(EFTauDiKaonHypo, self).__init__(name)     
        setVarCut(self, var, val)


## EFHadCalib: from mdacunha
class EFHadCalibHypoBase (EFHadCalibHypo):
    __slots__ = []
    def __init__(self, name):
        super(EFHadCalibHypoBase, self).__init__(name)
        setHadCalibEFMonTools(self)

class EFHadCalibHypo (EFHadCalibHypoBase):
    __slots__ = []
    def __init__(self, name = "EFHadCalibHypo", var = [], val = []):
        super(EFHadCalibHypo, self).__init__(name)
        applyDefaults(self, EFHadCalib_defaults)
        setVarCut(self, var, val)



###################
## T2CaloTau Hypos:
###################
variables  = ['EtRawMin', 'CoreFractionMin']

thresholds = {
    20: [15199.0, 0.8000],    
    29: [20696.0, 0.8000],    
    70: [50000.0, 0.8000],    
    100:[80000.0, 0.8000]   
    }

hypos.update(dict([ ('l2calo_tau%d_loose1'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_loose1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


thresholds = {
    20: [ 15199.0, 0.7485],
    29: [ 20696.0, 0.7755],
    38: [ 29430.0, 0.8175],
    50: [ 40341.0, 0.8310],
    115:[100000.0, 0.8790],
    125:[109233.0, 0.8790]
    }

hypos.update(dict([ ('l2calo_tau%d_medium'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_medium'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

hypos.update(dict([ ('l2calo_tau%d_medium1'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_medium1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    18: [ 10199.0, 0.7485],
    20: [ 10199.0, 0.7485],
    27: [ 15696.0, 0.7755],
    29: [ 15696.0, 0.7755],
    }

hypos.update(dict([ ('l2calo_tau%d_medium1_L2loose'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_medium1_L2loose'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    18: [ 14000.0, 0.758],
    20: [ 14000.0, 0.758],
    27: [ 20000.0, 0.757],
    29: [ 20000.0, 0.757],
    }

hypos.update(dict([ ('l2calo_tau%d_loose2'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_loose2'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    18: [ 13000.0, 0.79],
    20: [ 13000.0, 0.79],
    27: [ 18000.0, 0.789],
    }

hypos.update(dict([ ('l2calo_tau%d_loose3'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_loose3'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))




#################
## T2IDTau Hypos:
#################
variables  = ['NTrkMax', 'SumPtRatioMax1P', 'SumPtRatioMaxMP']


thresholds = {
    20: [4, 0.001, 0.001],
    29: [4, 0.001, 0.001],
    70: [4, 0.002, 0.002],
    100:[4, 0.002, 0.002]    
    }
hypos.update(dict([ ('l2id_tau%d_loose1'%t, T2IDTauHypo('T2IDTauHypo_tau%d_loose1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    20: [7, 0.001, 0.099],
    29: [7, 0.001, 0.001],
    38: [7, 0.001, 0.001],
    50: [7, 0.001, 0.001],
    115:[7, 0.001, 0.001],
    125:[7, 0.001, 0.001]
    }
hypos.update(dict([ ('l2id_tau%d_medium'%t, T2IDTauHypo('T2IDTauHypo_tau%d_medium'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    20: [4, 0.001, 0.099],
    29: [4, 0.001, 0.001],
    38: [4, 0.001, 0.001],
    50: [4, 0.001, 0.001],
    115:[4, 0.001, 0.001],
    125:[4, 0.001, 0.001]
    }

hypos.update(dict([ ('l2id_tau%d_medium1'%t, T2IDTauHypo('T2IDTauHypo_tau%d_medium1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    18: [4, 0.001, 0.099],
    20: [4, 0.001, 0.099],
    27: [4, 0.001, 0.001],
    29: [4, 0.001, 0.001],
    }

hypos.update(dict([ ('l2id_tau%d_medium1_L2loose'%t, T2IDTauHypo('T2IDTauHypo_tau%d_medium1_L2loose'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    18: [4, 0.119, 0.214],
    20: [4, 0.119, 0.214],
    27: [4, 0.103, 0.201],
    29: [4, 0.103, 0.201]
    }

hypos.update(dict([ ('l2id_tau%d_loose2'%t, T2IDTauHypo('T2IDTauHypo_tau%d_loose2'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    18: [4, 0.001, 0.13],
    20: [4, 0.001, 0.13],
    27: [4, 0.001, 0.117],
    }
hypos.update(dict([ ('l2id_tau%d_loose3'%t, T2IDTauHypo('T2IDTauHypo_tau%d_loose3'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

###############
## T2Tau Hypos:
###############
variables=['EtOverPtLeadTrkMax1P', 'EtOverPtLeadTrkMaxMP', 'TrkAvgDistMax1P', 'TrkAvgDistMaxMP']


thresholds={    
    20: [999.99, 999.99, 999.99, 999.99],
    29: [999.99, 999.99, 999.99, 999.99],
    70: [999.99, 999.99, 999.99, 999.99],
    100:[999.99, 999.99, 999.99, 999.99]
    }

hypos.update(dict([ ('l2_tau%d_loose1'%t, T2TauHypo('T2TauHypo_tau%d_loose1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


thresholds={
    20: [  8.84, 2.84, 0.105, 0.073],
    29: [  9.72, 2.92, 0.086, 0.062],
    38: [  9.68, 3.04, 0.076, 0.059],
    50: [  8.22, 3.02, 0.062, 0.054],
    115:[999.99, 4.38, 0.057, 0.053],
    125:[999.99, 4.38, 0.057, 0.053]
    }

hypos.update(dict([ ('l2_tau%d_medium'%t, T2TauHypo('T2TauHypo_tau%d_medium'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

hypos.update(dict([ ('l2_tau%d_medium1'%t, T2TauHypo('T2TauHypo_tau%d_medium1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds={    
    18: [  8.84, 2.84, 0.105, 0.073],
    20: [  8.84, 2.84, 0.105, 0.073],
    29: [  9.72, 2.92, 0.086, 0.062],
    27: [  9.72, 2.92, 0.086, 0.062],
    }

hypos.update(dict([ ('l2_tau%d_medium1_L2loose'%t, T2TauHypo('T2TauHypo_tau%d_medium1_L2loose'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds={    
    18: [  10.68, 4.57, 0.1478, 0.0808],
    20: [  10.68, 4.57, 0.1478, 0.0808],
    27: [  10.99, 4.62, 0.1382, 0.079],
    29: [  10.99, 4.62, 0.1382, 0.079]
    }

hypos.update(dict([ ('l2_tau%d_loose2'%t, T2TauHypo('T2TauHypo_tau%d_loose2'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds={    
    18: [  12.06, 3.64, 0.1188, 0.07],
    20: [  12.06, 3.64, 0.1188, 0.07],
    27: [  12.45, 3.67, 0.1112, 0.0682],
    }

hypos.update(dict([ ('l2_tau%d_loose3'%t, T2TauHypo('T2TauHypo_tau%d_loose3'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))



##################
## EFTauMV Hypos:
##################
variables = ['NTrackMax', 'EtCalibMin', 'Level']

thresholds = {
    20:  [3,   20.0*GeV, -1111],
    29:  [3,   29.0*GeV, -1111],
    70:  [3,   70.0*GeV, -1111],
    100: [3,  100.0*GeV, -1111]   
    }
hypos.update(dict([ ('ef_tau%d_loose1'%t,
                     EFTauMVHypo('EFTauMVHypo_tau%d_loose1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    20: [6,  20.0*GeV, 1],
    29: [6,  29.0*GeV, 1],
    38: [6,  38.0*GeV, 1],
    50: [6,  50.0*GeV, 1],
    115:[6, 115.0*GeV, 1],
    125:[6, 125.0*GeV, 1]
    }
hypos.update(dict([ ('ef_tau%d_medium'%t,
                     EFTauMVHypo('EFTauMVHypo_tau%d_medium'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    20: [3,  20.0*GeV, 2],
    29: [3,  29.0*GeV, 2],
    38: [3,  38.0*GeV, 2],
    50: [3,  50.0*GeV, 2],
    115:[3, 115.0*GeV, 2],
    125:[3, 125.0*GeV, 2]
    }
hypos.update(dict([ ('ef_tau%d_medium1'%t,
                     EFTauMVHypo('EFTauMVHypo_tau%d_medium1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    20: [3,  20.0*GeV, 3],
    29: [3,  29.0*GeV, 3],
    38: [3,  38.0*GeV, 3],
    50: [3,  50.0*GeV, 3],
    115:[3, 115.0*GeV, 3],
    125:[3, 125.0*GeV, 3]
    }
hypos.update(dict([ ('ef_tau%d_tight1'%t,
                     EFTauMVHypo('EFTauMVHypo_tau%d_tight1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds={
    18: [3,  18.0*GeV, 2],
    20: [3,  20.0*GeV, 2],
    27: [3,  27.0*GeV, 2],
    29: [3,  29.0*GeV, 2],
    }
hypos.update(dict([ ('ef_tau%d_medium1_L2loose'%t,
                     EFTauMVHypo('EFTauMVHypo_tau%d_medium1_L2loose'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds={
    18: [3,  18.0*GeV, 1],
    20: [3,  20.0*GeV, 1],
    27: [3,  27.0*GeV, 1],
    29: [3,  29.0*GeV, 1],
    }
hypos.update(dict([ ('ef_tau%d_loose2'%t,
                     EFTauMVHypo('EFTauMVHypo_tau%d_loose2'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds = {
    18: [3,  18.0*GeV, 2],
    20: [3,  20.0*GeV, 2],
    27: [3,  27.0*GeV, 2],
    }
hypos.update(dict([ ('ef_tau%d_loose3'%t,
                     EFTauMVHypo('EFTauMVHypo_tau%d_loose1'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


variables = ['NTrackMax', 'EtCalibMin', 'Level', 'Method']
thresholds = {
    20: [6,  20.0*GeV, 1, 1],
    29: [6,  29.0*GeV, 1, 1],
    38: [6,  38.0*GeV, 1, 1],
    50: [6,  50.0*GeV, 1, 1],
    115:[6, 115.0*GeV, 1, 1],
    125:[6, 125.0*GeV, 1, 1]
    }
hypos.update(dict([ ('ef_tau%d_medium_llh'%t,
                     EFTauMVHypo('EFTauMVHypo_tau%d_medium_LLH'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds={
    20: [3,  20.0*GeV, 2, 1],
    29: [3,  29.0*GeV, 2, 1],
    38: [3,  38.0*GeV, 2, 1],
    50: [3,  50.0*GeV, 2, 1],
    115:[3, 115.0*GeV, 2, 1],
    125:[3, 125.0*GeV, 2, 1]
    }
hypos.update(dict([ ('ef_tau%d_medium1_llh'%t,
                     EFTauMVHypo('EFTauMVHypo_tau%d_medium1_LLH'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

thresholds={
    20: [3,  20.0*GeV, 3, 1],
    29: [3,  29.0*GeV, 3, 1],
    38: [3,  38.0*GeV, 3, 1],
    50: [3,  50.0*GeV, 3, 1],
    115:[3, 115.0*GeV, 3, 1],
    125:[3, 125.0*GeV, 3, 1]
    }
hypos.update(dict([ ('ef_tau%d_tight1_llh'%t,
                     EFTauMVHypo('EFTauMVHypo_tau%d_tight1_LLH'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))

############
# Others
############
## tauConfig_tauNoCut_MV
hypos.update( { 'l2calo_tauNoCut': T2CaloTauHypo_tauNoCut('T2CaloTauHypo_tauNoCut') } )
hypos.update( { 'l2id_tauNoCut'  : T2IDTauHypo_tauNoCut('T2IDTauHypo_tauNoCut')     } )
hypos.update( { 'l2_tauNoCut'    : T2TauHypo_tauNoCut('T2TauHypo_tauNoCut')         } )
hypos.update( { 'ef_tauNoCut'    : EFTauMVHypo_tauNoCut('EFTauMVHypo_tauNoCut')     } )


## tauConfig_tau29_medium_2stTest
hypos.update( { 'l2calo_tau29_medium_2stTest' :
                T2CaloTauHypo('T2CaloTauHypo_tau29_medium_2stTest',
                              ['EtRawMin','CoreFractionMin'],
                              [20696.0, 0.7755]) } )

hypos.update( { 'l2idcore_tau29_medium_2stTest':
                T2IDCoreTauHypo_tau29_medium_2stTest('T2IDCoreTauHypo_tau29_medium_2stTest',
                                                     ['NTrkMin', 'NTrkMax','LdTrkPtMin'],
                                                     [1, 7, -9999.9*GeV] ) } )

hypos.update( { 'l2idiso_tau29_medium_2stTest':
                T2IDIsoTauHypo_tau29_medium_2stTest('T2IDIsoTauHypo_tau29_medium_2stTest',
                                                    ['SumPtRatioMax1P', 'SumPtRatioMaxMP'],
                                                    [0.001, 0.001] ) } )

hypos.update( { 'l2_tau29_medium_2stTest':
                T2TauHypo('T2TauHypo_tau29_medium_2stTest',
                          ['EtOverPtLeadTrkMax1P', 'EtOverPtLeadTrkMaxMP', 'TrkAvgDistMax1P', 'TrkAvgDistMaxMP'],
                          [9.72, 2.92, 0.086, 0.062] ) } )

hypos.update( { 'ef_tau29_medium_2stTest':
                EFTauMVHypo('EFTauMVHypo_tau29_medium_2stTest',
                            ['NTrackMin', 'NTrackMax', 'EtCalibMin', 'Level'],
                            [1, 6, 29.0*GeV, 1]) } )


## _calo chains for IDTrkNoCut
variables=['EtRawMin','CoreFractionMin']
thresholds = {
    29: [ 20696.0, 0.7755],
    125:[109233.0, 0.8790]
    }
hypos.update(dict([ ('l2calo_tau%d_calo'%t, T2CaloTauHypo('T2CaloTauHypo_tau%d_calo'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


variables=['NTrkMin','NTrkMax']
thresholds = {
    29: [0,99],   
    125:[0,99]
    }
hypos.update(dict([ ('l2id_tau%d_calo'%t, T2IDTauHypo('T2IDTauHypo_tau%d_calo'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


hypos.update(dict([ ('l2_tau%d_calo'%t, T2TauHypo('T2TauHypo_tau%d_calo'%t))
                    for t in [29, 125] ]))


## trk_id
variables = ['NTrkMax','LdTrkPtMin','SumPtRatioMax']
thresholds = {
    9: [1,  9.*GeV, 0.1],
    18:[1, 18.*GeV, 0.1]
    }
hypos.update(dict([ ('l2id_trk%d_id'%t, T2IDTauHypo('T2IDTauHypo_trk%d_id'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))


## EFHadCalib hc
variables  = ['CutOnPtHT','CutOnIsolationDeltaR',"CutminSCTHitsHT","CutminPixHitsHT","CutmaxD0HT","CutmaxZ0HT","CutOnEtaHT"]

thresholds = {
    9: [ 9.*GeV, 0.4,6,1,1.5,1.5,2.5],
    18:[18.*GeV, 0.4,6,1,1.5,1.5,2.5]
    }
hypos.update(dict([ ('ef_hadCalibHypo_trk%d'%t, EFHadCalibHypo('EFHadCalibHypo_trk%d'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))
hypos.update(dict([ ('ef_hadCalibHypo_trk%d_L1HA8'%t, EFHadCalibHypo('EFHadCalibHypo_trk%d_L1HA8'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))
hypos.update(dict([ ('ef_hadCalibHypo_trk%d_L1RD0'%t, EFHadCalibHypo('EFHadCalibHypo_trk%d_L1RD0'%t,variables,v))
                    for t,v in thresholds.iteritems() ]))
