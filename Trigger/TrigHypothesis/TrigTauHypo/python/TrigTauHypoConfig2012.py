# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##------------------------------------------
## 
## Hypothesis for v4, 2012 run
## L2 cuts optimized for mu=30
##
##------------------------------------------
# get defaults
from TrigTauHypo.TrigTauHypoBase import *
from TrigTauHypo.TrigTauHypoMonitoring \
     import setL2TauTopoMonTools, setHLTVertexPreselMonTools, setPhotonTauMonTools, setTauEFDiKaonMonTools, setTauEFMVMonTools, setTauT2CoreMonTools, setTauT2IsoMonTools, setHadCalibEFMonTools, setEFTauTopoMonTools
from TriggerJobOpts.TriggerFlags import TriggerFlags


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
        setTauEFMVMonTools(self)

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

        if TriggerFlags.run2Config=='2016':
            self.Highpt    = False


class EFTauMVHypo_highpt (EFTauMVHypoBase):
    __slots__ = []
    def __init__(self, name = "EFTauMVHypo_highpt", var = [], val = []):
        super(EFTauMVHypo_highpt, self).__init__(name)
        self.NTrackMin = 1 #for all items
        self.Method    = 2 #default bdt as of March 22
        self.Highpt    = True
        setVarCut(self, var, val)

## L2TauTopo
class L2TauTopoHypoBase (L2TauTopoHypo):
    __slots__ = []
    def __init__(self, name):
        super(L2TauTopoHypoBase, self).__init__(name)
        setL2TauTopoMonTools(self)

class L2TauTopoHypo (L2TauTopoHypoBase):
    __slots__ = []
    def __init__(self, name = "L2TauTopoHypo", var = [], val = []):
        super(L2TauTopoHypo, self).__init__(name)

## EFTauTopo
class EFTauTopoHypoBase (EFTauTopoHypo):
    __slots__ = []
    def __init__(self, name):
        super(EFTauTopoHypoBase, self).__init__(name)
        setEFTauTopoMonTools(self)

class EFTauTopoHypo (EFTauTopoHypoBase):
    __slots__ = []
    def __init__(self, name = "EFTauTopoHypo", var = [], val = []):
        super(EFTauTopoHypo, self).__init__(name)
        setVarCut(self, var, val)

class EFTauTopoFexBase (EFTauTopoFex):
    __slots__ = []
    def __init__(self, name):
        super(EFTauTopoFexBase, self).__init__(name)
        
class EFTauTopoFex (EFTauTopoFexBase):
    __slots__ = []
    def __init__(self, name = "EFTauTopoFex", chain_type="ditau"):
        super(EFTauTopoFex, self).__init__(name)
        self.comb    = chain_type

## HLTVertexPreSelHypo
class HLTVertexPreSelHypoBase (HLTVertexPreSelHypo):
    __slots__ = []
    def __init__(self, name):
        super(HLTVertexPreSelHypoBase, self).__init__(name)
        setHLTVertexPreselMonTools(self)

class HLTVertexPreSelHypo (HLTVertexPreSelHypoBase):
    __slots__ = []
    def __init__(self, name = "HLTVertexPreSelHypo", var = [], val = []):
        super(HLTVertexPreSelHypo, self).__init__(name)
        setVarCut(self, var, val)

## EFTauDiKaon
class EFTauDiKaonHypoBase (EFTauDiKaonHypo):
    __slots__ = []
    def __init__(self, name):
        super(EFTauDiKaonHypoBase, self).__init__(name)
        setTauEFDiKaonMonTools(self)

class EFTauDiKaonHypo (EFTauDiKaonHypoBase):
    __slots__ = []
    def __init__(self, name = "EFTauDiKaonHypo", var = [], val = []):
        super(EFTauDiKaonHypo, self).__init__(name)     
        setVarCut(self, var, val)

## EFPhotonTau
class EFPhotonTauHypoBase (EFPhotonTauHypo):
    __slots__ = []
    def __init__(self, name):
        super(EFPhotonTauHypoBase, self).__init__(name)
        setPhotonTauMonTools(self)

class EFPhotonTauHypo (EFPhotonTauHypoBase):
    __slots__ = []
    def __init__(self, name = "EFPhotonTauHypo", var = [], val = []):
        super(EFPhotonTauHypo, self).__init__(name)
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

thresholds = {
    20: [ 15199.0, 0.7485],
    29: [ 20696.0, 0.7755],
    38: [ 29430.0, 0.8175],
    50: [ 40341.0, 0.8310],
    115:[100000.0, 0.8790],
    125:[109233.0, 0.8790]
    }

thresholds = {
    18: [ 10199.0, 0.7485],
    20: [ 10199.0, 0.7485],
    27: [ 15696.0, 0.7755],
    29: [ 15696.0, 0.7755],
    }

thresholds = {
    18: [ 14000.0, 0.758],
    20: [ 14000.0, 0.758],
    27: [ 20000.0, 0.757],
    29: [ 20000.0, 0.757],
    }

thresholds = {
    18: [ 13000.0, 0.79],
    20: [ 13000.0, 0.79],
    27: [ 18000.0, 0.789],
    }


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

thresholds = {
    20: [7, 0.001, 0.099],
    29: [7, 0.001, 0.001],
    38: [7, 0.001, 0.001],
    50: [7, 0.001, 0.001],
    115:[7, 0.001, 0.001],
    125:[7, 0.001, 0.001]
    }

thresholds = {
    20: [4, 0.001, 0.099],
    29: [4, 0.001, 0.001],
    38: [4, 0.001, 0.001],
    50: [4, 0.001, 0.001],
    115:[4, 0.001, 0.001],
    125:[4, 0.001, 0.001]
    }

thresholds = {
    18: [4, 0.001, 0.099],
    20: [4, 0.001, 0.099],
    27: [4, 0.001, 0.001],
    29: [4, 0.001, 0.001],
    }

thresholds = {
    18: [4, 0.119, 0.214],
    20: [4, 0.119, 0.214],
    27: [4, 0.103, 0.201],
    29: [4, 0.103, 0.201]
    }

thresholds = {
    18: [4, 0.001, 0.13],
    20: [4, 0.001, 0.13],
    27: [4, 0.001, 0.117],
    }

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

thresholds={
    20: [  8.84, 2.84, 0.105, 0.073],
    29: [  9.72, 2.92, 0.086, 0.062],
    38: [  9.68, 3.04, 0.076, 0.059],
    50: [  8.22, 3.02, 0.062, 0.054],
    115:[999.99, 4.38, 0.057, 0.053],
    125:[999.99, 4.38, 0.057, 0.053]
    }

thresholds={    
    18: [  8.84, 2.84, 0.105, 0.073],
    20: [  8.84, 2.84, 0.105, 0.073],
    29: [  9.72, 2.92, 0.086, 0.062],
    27: [  9.72, 2.92, 0.086, 0.062],
    }

thresholds={    
    18: [  10.68, 4.57, 0.1478, 0.0808],
    20: [  10.68, 4.57, 0.1478, 0.0808],
    27: [  10.99, 4.62, 0.1382, 0.079],
    29: [  10.99, 4.62, 0.1382, 0.079]
    }

thresholds={    
    18: [  12.06, 3.64, 0.1188, 0.07],
    20: [  12.06, 3.64, 0.1188, 0.07],
    27: [  12.45, 3.67, 0.1112, 0.0682],
    }


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

thresholds = {
    20: [6,  20.0*GeV, 1],
    29: [6,  29.0*GeV, 1],
    38: [6,  38.0*GeV, 1],
    50: [6,  50.0*GeV, 1],
    115:[6, 115.0*GeV, 1],
    125:[6, 125.0*GeV, 1]
    }

thresholds = {
    20: [3,  20.0*GeV, 2],
    29: [3,  29.0*GeV, 2],
    38: [3,  38.0*GeV, 2],
    50: [3,  50.0*GeV, 2],
    115:[3, 115.0*GeV, 2],
    125:[3, 125.0*GeV, 2]
    }

thresholds = {
    20: [3,  20.0*GeV, 3],
    29: [3,  29.0*GeV, 3],
    38: [3,  38.0*GeV, 3],
    50: [3,  50.0*GeV, 3],
    115:[3, 115.0*GeV, 3],
    125:[3, 125.0*GeV, 3]
    }

thresholds={
    18: [3,  18.0*GeV, 2],
    20: [3,  20.0*GeV, 2],
    27: [3,  27.0*GeV, 2],
    29: [3,  29.0*GeV, 2],
    }

thresholds={
    18: [3,  18.0*GeV, 1],
    20: [3,  20.0*GeV, 1],
    27: [3,  27.0*GeV, 1],
    29: [3,  29.0*GeV, 1],
    }

thresholds = {
    18: [3,  18.0*GeV, 2],
    20: [3,  20.0*GeV, 2],
    27: [3,  27.0*GeV, 2],
    }

variables = ['NTrackMax', 'EtCalibMin', 'Level', 'Method']
thresholds = {
    20: [6,  20.0*GeV, 1, 1],
    29: [6,  29.0*GeV, 1, 1],
    38: [6,  38.0*GeV, 1, 1],
    50: [6,  50.0*GeV, 1, 1],
    115:[6, 115.0*GeV, 1, 1],
    125:[6, 125.0*GeV, 1, 1]
    }

thresholds={
    20: [3,  20.0*GeV, 2, 1],
    29: [3,  29.0*GeV, 2, 1],
    38: [3,  38.0*GeV, 2, 1],
    50: [3,  50.0*GeV, 2, 1],
    115:[3, 115.0*GeV, 2, 1],
    125:[3, 125.0*GeV, 2, 1]
    }

thresholds={
    20: [3,  20.0*GeV, 3, 1],
    29: [3,  29.0*GeV, 3, 1],
    38: [3,  38.0*GeV, 3, 1],
    50: [3,  50.0*GeV, 3, 1],
    115:[3, 115.0*GeV, 3, 1],
    125:[3, 125.0*GeV, 3, 1]
    }

############
# Others
############

## _calo chains for IDTrkNoCut
variables=['EtRawMin','CoreFractionMin']
thresholds = {
    29: [ 20696.0, 0.7755],
    125:[109233.0, 0.8790]
    }

variables=['NTrkMin','NTrkMax']
thresholds = {
    29: [0,99],   
    125:[0,99]
    }

## trk_id
variables = ['NTrkMax','LdTrkPtMin','SumPtRatioMax']
thresholds = {
    9: [1,  9.*GeV, 0.1],
    18:[1, 18.*GeV, 0.1]
    }

## EFHadCalib hc
variables  = ['CutOnPtHT','CutOnIsolationDeltaR',"CutminSCTHitsHT","CutminPixHitsHT","CutmaxD0HT","CutmaxZ0HT","CutOnEtaHT"]

thresholds = {
    9: [ 9.*GeV, 0.4,6,1,1.5,1.5,2.5],
    18:[18.*GeV, 0.4,6,1,1.5,1.5,2.5]
    }

