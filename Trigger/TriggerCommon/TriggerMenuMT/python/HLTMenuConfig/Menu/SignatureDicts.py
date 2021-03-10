# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )
log.info("Importing %s",__name__)

from copy import deepcopy

#==========================================================
# This is stored in chainDict['Signature']
#==========================================================
SliceIDDict = {
    'Electron': 'e',
    'Photon'  : 'g',
    'Jet'     : 'j',
    'Muon'    : 'mu',
    'Tau'     : 'tau',
    'MET'     : 'xe',
    'XS'      : 'xs',
    'TE'      : 'te',
    'MinBias' : 'mb',
    'HeavyIon' : 'hi',
    'Cosmic'  : 'cosmic',
    'Calib'   : 'calib',
    #'Calib'   : 'calib',
    'Streaming'     : 'streamer',
    'Monitor'    : 'mon',
    'Beamspot'      : 'beamspot',
    'EnhancedBias'  : 'eb',
    'UnconventionalTracking'  : 'unconvtrk',
    'Test'          : 'TestChain',
}

AllowedSignatures = ["jet", "bjet", "electron", "photon", "egamma",
                     "muon",
                     "met",
                     "tau",
                     "unconvtrk",
                     "minbias",
                     "heavyion",
                     "cosmic",
                     "calibration", "streaming", "monitoring", 'eb']

#==========================================================
# ---- Generic Template for all chains          ----
# ---- chainParts specific information given in ----
# ---- signature specific dictionaries below    ----
#==========================================================
ChainDictTemplate = {
    'chainName'    : '',
    'L1item'        : '',
    'topo'          : '',
    'signatures'    : [],
    'alignmentGroups' : [],
    'stream'        : '',
    'groups'        : [],
    'EBstep'        : '',
    'chainParts'   : [],
    'topoStartFrom' : False,
    'sigFolder'     : '',
    'subSigs'        : [],
    'extraComboHypos' : []
}

#==========================================================
# Test chains
#==========================================================
# ---- Test Dictionary of all allowed Values ----
TestChainParts = {
    'L1threshold'    : '',
    'signature'      : ['Test'],
    'alignmentGroup' : ['Test'],
    'chainPartName'  : '',
    'multiplicity'   : '',
    'extra'          : ['muv1', 'muv1step', 'muv2', 'ev1', 'ev2', 'ev3', 'gv1', 'muEmpty1', 'muEmpty2', 'ev1dr', 'muv1dr','merge'],
    'trigType'       : ['TestChain'],
    'threshold'      : '',
    'addInfo'        : [''],
    'sigFolder'     : 'Test',
    'subSigs'       : ['Test']
}

# ---- Test Dictionary of default Values ----
TestChainParts_Default = {
    'signature'      : ['Test'],
    'alignmentGroup' : ['Test'],
    'L1threshold'    : '',
    'multiplicity'   : '',
    'trigType'       : '',
    'threshold'      : '',
    'addInfo'        : [],
    'sigFolder'     : 'Test',
    'subSigs'       : ['Test']
}

#==========================================================
# Jet
#==========================================================
AllowedTopos_jet = []
# ---- Jet Dictionary of all allowed Values ----
JetChainParts = {
    # Information common to all signatures
    'signature'     : ['Jet'],
    'alignmentGroup': ['Jet','JetMET'],
    'L1threshold'   : '',
    'chainPartName' : '',
    'threshold'     : '',
    'multiplicity'  : '',
    'trigType'     : ['j'],
    'topo'          : AllowedTopos_jet,
    'extra'        : [],
    'addInfo'      : ['perf'],
    'sigFolder'     : 'Jet',
    'subSigs'       : ['Jet'],

    # Information unique to the jet slice
    # Reco information
    'recoAlg'      : # Jet clustering algorithm
      ['a4', 'a10', 'a10r', 'a10t', 'a10sd'],
    'constitType'  : # Jet input type
      ['tc','pf'], # 'ufo' might be added at some point
    'clusterCalib' : # Topocluster calibration
      ['em', 'lcw'],
    'constitMod'   : # Constituent modifiers
      ['sk', 'cssk'],
    'jetCalib'     : # Jet calibration
      ['jes', 'subjes', 'subjesIS', 'subjesgscIS', 'subresjesgscIS', 'subjesgsc', 'subresjesgsc', 'nojcalib'],
    'scan'         : # No longer used?
      ['FS',],
    'trkopt'       : # Tracking configuration
      ['notrk','ftf'],
    'trkpresel'    : # Tracking preselection
      ['nopresel','preselj20'],
    # Hypo information
    #   If hypoScenario is 'simple', then hypo configuration is handled based on the
    #   other dict contents. If it is not 'simple', then the configuration is 100%
    #   from the hypoScenario specification, and all other hypo entries are ignored.
    'hypoScenario' : ['simple', # Independent selections on individual jets, multiplicity+threshold cuts
                      # 'agg' (forward-backward + dijet) scenario:
                      #   default eta selection for dijet mass cut is 0eta490
                      'fbdjshared',  # Forward backward jets + dijet, default parameters, fb and dj can share
                      'fbdjnosharedSEP10etSEP20etSEP34massSEP50fbet', # f/b jets + dijet, expl. parameters, fb and dj do not share
                      # 'dijet' scenario applies always a mass cut (deta and dphi cuts are optional)
                      #   0eta490 is the default eta selections for j1/j2
                      #   j12et sets the same et cuts for j1et and j2et
                      #   j12eta sets the same eta cuts for j1eta and j2eta
                      #   order:
                      #     et cuts    (mandatory)
                      #     eta cuts   (optional, if missing will use default)
                      #     djmass sel (mandatory)
                      #     djdphi sel (optional)
                      #     djdeta sel (optional)
                      'dijetSEP80j12etSEP0j12eta240SEP700djmass', # Test dijet mass sel
                      'dijetSEP80j12etSEP700djmassSEP26djdphi', # Test dijet mass sel including dphi cut
                      'dijetSEP70j12etSEP1000djmassSEP20djdphiSEP40djdeta', # dijet mass sel including dphi and deta cuts
                      'dijetSEP50j12etSEP1000djmass',
                      'dijetSEP50j12etSEP1000djmassSEP24djdphi',
                      'dijetSEP50j12etSEP900djmass',
                      'dijetSEP35j12etSEP1000djmass',
                      # 'agg' category is for single variable computed by aggregation over single jets (default filtering: 30et and 0eta320)
                      'aggSEP1000ht',
                      'aggSEP500ht',
                      'aggSEP100htSEP10et',
                      'aggSEP50htSEP10etSEP0eta320' # HT selection with explicit jet et/eta cuts
                      ],

    # Simple hypo configuration. Single property cuts defined as MINvarMAX
    'etaRange'      :
      ['0eta320', '320eta490', '0eta240', '0eta290', '0eta490'],
    'jvt'           : # Jet Vertex Tagger pileup discriminant
      ['010jvt', '011jvt', '015jvt', '020jvt', '050jvt', '059jvt'],
    'momCuts'       : # Generic moment cut on single jets
      ['050momemfrac100','momhecfrac010','050momemfrac100SEPmomhecfrac010'],
    'prefilters'      : # Pre-hypo jet selectors (including cleaning)
    ['cleanLB', 'prefilterSEP300ceta210SEP300nphi10'], 
    'smc'           : # "Single mass condition" -- rename?
      ['30smcINF', '35smcINF', '40smcINF', '50smcINF', '60smcINF', 'nosmc'],
    # Setup for alternative data stream readout
    'TLA'          : [],        # Unused
    'dataScouting' : ['JetDS'], # Triggers TLA jet selection + recording
    # B-tagging information
    'bTag'         : ['boffperf'  ,
                      'bmv2c2040' , 'bmv2c2050' , 'bmv2c2060' , 'bmv2c2070' , 'bmv2c2077' , 'bmv2c2085' ,
                      'bmv2c1040' , 'bmv2c1050' , 'bmv2c1060' , 'bmv2c1070' , 'bmv2c1077' , 'bmv2c1085' ,
                      'bhmv2c1040', 'bhmv2c1050', 'bhmv2c1060', 'bhmv2c1070', 'bhmv2c1077', 'bhmv2c1085',
                      'bdl1r60','bdl1r70','bdl1r77','bdl1r85'],
    'bTracking'    : [],
    'bConfig'      : ['split',],
    'bMatching'    : ['antimatchdr05mu'],
}

# ---- Jet Dictionary of default Values ----
JetChainParts_Default = {
    'signature'     : ['Jet'],
    'alignmentGroup': ['Jet'],
    'L1threshold'   : '',
    'threshold'     : '',
    'multiplicity'  : '',
    'trigType'      :'j',
    'topo'          : [],
    'extra'         : '',
    'addInfo'       : [],
    'sigFolder'     : 'Jet',
    'subSigs'       : ['Jet'],
    #
    'recoAlg'       :'a4',
    'constitType'   :'tc',
    'clusterCalib'  :'em',
    'constitMod'    :'',
    'jetCalib'      :'default',
    'scan'          :'FS',
    'trkopt'        : 'notrk',
    'trkpresel'     : 'nopresel',
    #
    'etaRange'      : '0eta320',
    'jvt'           : '',
    'momCuts'       : '',
    'prefilters'    : [],
    'hypoScenario'  : 'simple',
    'smc'           : 'nosmc',
    #
    'TLA'           : '',
    'dataScouting'  : '',
    #
    'bTag'          : '',
    'bTracking'     : '',
    'bConfig'       : [],
    'bMatching'     : [],
}

# ---- bJet Dictionary of default Values that are different to the ones for normal jet chains ----
bJetChainParts_Default = {
    'etaRange' : '0eta290',
}

#==========================================================
# Muon
#==========================================================
AllowedTopos_mu = []

# ---- Muon Dictionary of all allowed Values ----
MuonChainParts = {
    'signature'      : ['Muon'],
    'alignmentGroup' : ['Muon','MuonnoL1'],
    'L1threshold'    : '',
    'chainPartName'  : [],
    'multiplicity'   : '',
    'trigType'       : ['mu'],
    'etaRange'       : ['0eta2550','0eta105'],
    'threshold'      : '',
    'extra'          : ['noL1', 'msonly','lateMu', "Dr", "muoncalib" ,'l2io','l2lrt','l2mt'],
    'IDinfo'         : [],
    'isoInfo'        : ['ivarloose', 'ivarmedium', 'ivarperf',],
    'invMassInfo'    : ['10invm70'],
    'addInfo'        : ['1step','idperf','LRT','3layersEC','cosmic',"muonqual"],
    'topo'           : AllowedTopos_mu,
    'flavour'        : [],
    'sigFolder'     : 'Muon',
    'subSigs'       : ['Muon']
}
# ---- MuonDictionary of default Values ----
MuonChainParts_Default = {
    'signature'      : ['Muon'],
    'alignmentGroup' : ['Muon'],
    'L1threshold'    : '',
    'multiplicity'   : '',
    'trigType'       : ['mu'],
    'etaRange'       : '0eta250',
    'threshold'      : '',
    'extra'          : '',
    'IDinfo'         : '',
    'isoInfo'        : '',
    'addInfo'        : [],
    'invMassInfo'    : '',
    'topo'           : [],
    'flavour'        : '',
    'sigFolder'     : 'Muon',
    'subSigs'       : ['Muon']
}

#==========================================================
# Bphysics
#==========================================================
AllowedTopos_Bphysics = [
    'bJpsimumu','bJpsi','bUpsimumu','bUpsi','bBmumu','bDimu','bDimu2700','bDimu6000','bPhi','bTau','bJpsimumul2io',
    'bBmumux','BpmumuKp','BcmumuPi','BsmumuPhi','BdmumuKst','LbPqKm'
]

# ---- Bphysics Dictionary of all allowed Values ----
BphysicsChainParts = deepcopy(MuonChainParts)
BphysicsChainParts['signature'] = ['Bphysics']
BphysicsChainParts['subFolder'] = 'Bphysics'
BphysicsChainParts['subSigs'] = ['Bphysics']
BphysicsChainParts['topo'] = AllowedTopos_Bphysics

# ---- Bphysics Dictionary of default Values ----
BphysicsChainParts_Default = deepcopy(MuonChainParts_Default)
BphysicsChainParts_Default['signature'] = ['Bphysics']
BphysicsChainParts_Default['subFolder'] = 'Bphysics'
BphysicsChainParts_Default['subSigs'] = ['Bphysics']
BphysicsChainParts_Default['topo'] = []


#==========================================================
# Taus
#==========================================================
AllowedTopos_tau = []

# ---- Tau Dictionary of all allowed Values ----
TauChainParts = {
    'signature'     : ['Tau'],
    'alignmentGroup': ['Tau'],
    'L1threshold'   : '',
    'chainPartName' : '',
    'threshold'     : '',
    'preselection'  : ['track', 'tracktwo', 'tracktwoEF', 'tracktwoMVA', 'tracktwoMVABDT' , 'tracktwoEFmvaTES', 'ptonly', ],
    'selection'     : ['medium1', 'verylooseRNN', 'looseRNN', 'mediumRNN', 'tightRNN', 'perf', 'idperf'],
    'multiplicity'  : '',
    'trigType'      : ['tau'],
    'trkInfo'       : '',
    'extra'         : '',
    'recoAlg'       : '',
    'calib'         : '',
    'addInfo'       : ['IdTest'],
    'topo'          : AllowedTopos_tau,
    'sigFolder'     : 'Tau',
    'subSigs'       : ['Tau']
}
TauChainParts_Default = {
    'signature'     : ['Tau'],
    'alignmentGroup': ['Tau'],
    'L1threshold'   : '',
    'chainPartName' : '',
    'threshold'     : '20',
    'preselection'  : 'tracktwo',
    'selection'     : 'medium1',
    'multiplicity'  :  '',
    'trigType'      : ['tau'],
    'trkInfo'       : [],
    'extra'         : '',
    'recoAlg'       : '',
    'calib'         : '',
    'addInfo'       :  '',
    'topo'          : [],
    'sigFolder'     : 'Tau',
    'subSigs'       : ['Tau']
}

#==========================================================
# MET
#==========================================================
AllowedTopos_xe = []
# ---- Met Dictionary of all allowed Values ----
METChainParts = {
    'signature'      : ['MET'],
    'alignmentGroup' : ['MET','JetMET'],
    'L1threshold'    : '',
    'chainPartName'  : '',
    'threshold'      : '',
    'multiplicity'   : '',
    'topo'           : AllowedTopos_xe,
    'trigType'       : ['xe'],
    'extra'          : ['noL1'],
    'calib'          : ['lcw','em'],
    'jetCalib'       : JetChainParts['jetCalib'],
    'L2recoAlg'      : [],
    'EFrecoAlg'      : ['cell', 'tc', 'tcpufit', 'mht', 'trkmht', 'pfsum', 'cvfpufit', 'pfopufit', 'mhtpufit'],
    'constitType'    : JetChainParts['constitType'],
    'L2muonCorr'     : [],
    'EFmuonCorr'     : [],
    'addInfo'        : ['FStracks'],
    'sigFolder'      : 'MET',
    'subSigs'        : ['MET'],
    'constitmod'     : ['cssk', 'vssk']
}
# ---- MetDictionary of default Values ----
METChainParts_Default = {
    'signature'      : ['MET'],
    'alignmentGroup' : ['MET'],
    'L1threshold'    : '',
    'trigType'       : ['xe'],
    'threshold'      : '',
    'extra'          : '',
    'calib'          : 'lcw',
    'jetCalib'       : JetChainParts_Default['jetCalib'],
    'L2recoAlg'      : '',
    'EFrecoAlg'      : '',
    'L2muonCorr'     : '',
    'EFmuonCorr'     : '',
    'addInfo'        : '',
    'constitType'    : 'tc',
    'constitmod'     : '',
    'sigFolder'     : 'MET',
    'subSigs'       : ['MET']
}

#==========================================================
# XS
#==========================================================
# ---- xs Dictionary of all allowed Values ----
XSChainParts = METChainParts
XSChainParts['signature'] = ['XS']
XSChainParts['trigType']  = ['xs']

# ---- xs Dictionary of default Values ----
XSChainParts_Default = METChainParts_Default
XSChainParts_Default['signature'] = ['XS']
XSChainParts_Default['trigType']  = ['xs']

#==========================================================
# TE
#==========================================================
# ---- te Dictionary of all allowed Values ----
TEChainParts = METChainParts
TEChainParts['signature'] = ['TE']
TEChainParts['trigType']  = ['te']

# ---- te Dictionary of default Values ----
TEChainParts_Default = METChainParts_Default
TEChainParts_Default['signature'] = ['TE']
TEChainParts_Default['trigType']  = ['te']

#==========================================================
# Electron Chains
#==========================================================
AllowedTopos_e = ["Jpsiee","Zeg","Zee"]
# ---- Electron Dictionary of all allowed Values ----
ElectronChainParts = {
    'signature'      : ['Electron'],
    'alignmentGroup' : ['Electron','Egamma'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'extra'          : '',
    'multiplicity'   : '',
    'trigType'       : ['e'],
    'threshold'      : '',
    'etaRange'       : [],
    'IDinfo'         : ['lhvloose','lhloose','lhmedium','lhtight'],
    'isoInfo'        : ['ivarloose','ivarmedium','ivartight'],
    'trkInfo'        : ['idperf', 'gsf'],
    'caloInfo'       : [],
    'lhInfo'         : ['nod0'],
    'L2IDAlg'        : ['noringer'],
    'addInfo'        : [ 'etcut', 'etcut1step',"v2","v3"],
    'sigFolder'     : 'Egamma',
    'subSigs'       : ['Electron','Photon'],
    'topo'          : AllowedTopos_e
}

# ---- Egamma Dictionary of default Values ----
ElectronChainParts_Default = {
    'signature'      : ['Electron'],
    'alignmentGroup' : ['Electron'],
    'multiplicity'   : '',
    'L1threshold'         : '',
    'trigType'       : '',
    'threshold'      : '',
    'etaRange'       : '0eta250',
    'extra'          : '',
    'IDinfoType'     : '',
    'IDinfo'         : '',
    'isoInfo'        : '',
    'reccalibInfo'   : '',
    'trkInfo'        : '',
    'caloInfo'       : '',
    'lhInfo'         : '',
    'L2IDAlg'        : '',
    'hypoInfo'       : '',
    'recoAlg'        : '',
    'FSinfo'         : '',
    'addInfo'        : [],
    'sigFolder'     : 'Egamma',
    'subSigs'       : ['Electron','Photon']
}

#==========================================================
# Photon chains
#==========================================================
# ---- Photon Dictionary of all allowed Values ----
AllowedTopos_g = ['dPhi15']
PhotonChainParts = {
    'L1threshold'    : '',
    'signature'      : ['Photon'],
    'alignmentGroup' : ['Photon','Egamma'],
    'chainPartName'  : '',
    'multiplicity'   : '',
    'trigType'       : ['g'],
    'threshold'      : '',
    'extra'          : [],
    'IDinfo'         : ['etcut','loose','medium','tight'],
    'isoInfo'        : ['icaloloose','icalomedium','icalotight'],
    'reccalibInfo'   : [],
    'trkInfo'        : [],
    'caloInfo'       : [],
    'hypoInfo'       : '',
    'recoAlg'        : [],
    'FSinfo'         : [],
    'addInfo'        : ['etcut',],
    'sigFolder'     : 'Egamma',
    'subSigs'       : ['Electron','Photon'],
    'topo'          : AllowedTopos_g
    }

# ---- Photon Dictionary of default Values ----
PhotonChainParts_Default = {
    'signature'      : ['Photon'],
    'alignmentGroup' : ['Photon'],
    'L1threshold'    : '',
    'multiplicity'   : '',
    'trigType'       : '',
    'threshold'      : '',
    'extra'          : '',
    'IDinfo'         : '',
    'isoInfo'        : '',
    'reccalibInfo'   : '',
    'trkInfo'        : '',
    'caloInfo'       : '',
    'hypoInfo'       : '',
    'recoAlg'        : '',
    'FSinfo'         : '',
    'addInfo'        : [],
    'sigFolder'     : 'Egamma',
    'subSigs'       : ['Electron','Photon']
    }

#==========================================================
# MinBias chains
#==========================================================
# ---- MinBias Dictionary of all allowed Values ----
MinBiasChainParts = {
    'signature'      : ['MinBias'],
    'alignmentGroup' : ['MinBias'],
    'L1threshold'    : '',
    'chainPartName'  : '',
    'multiplicity'   : '',
    'trigType'       : ['mb'],
    'threshold'      : '',
    'extra'          : ['noisesup', 'vetombts2in', 'vetombts1side2in',  'vetospmbts2in', "vetosp" ,'ion', 'ncb', 'blayer', 'exclusiveloose', 'exclusivetight'], #ncb = non collision background, blayer = only sum innermost pix layer
    'IDinfo'         : [],
    'ZDCinfo'        : ['lg', 'hg'],
    'trkInfo'        : ['hlttr', 'ftk', 'costr'],
    'hypoL2Info'     : ['sp2', 'sp3', 'sp5', 'sp10', 'sp15', 'sp100', 'sp300', 'sp400', 'sp500', 'sp600', 'sp700', 'sp800', 'sp900',
                        'sp1000', 'sp1200', 'sp1300', 'sp1400', 'sp1500', 'sp1600', 'sp1700', 'sp1800',
                        'sp2000', 'sp2100', 'sp2200', 'sp2300', 'sp2400', 'sp2500', 'sp2700', 'sp2800', 'sp2900', 'sp3000',
                        'sp3100', 'sp3500', 'sp4100', 'sp4500', 'sp4800', 'sp5000', 'sp5200',],
    'pileupInfo'     : ['pusup200','pusup300','pusup350', 'pusup400', 'pusup450', 'pusup500', 'pusup550', 'pusup600', 'pusup700', 'pusup750', 'pusup800', 'pusup900',
                        'pusup1000', 'pusup1100', 'pusup1200', 'pusup1300', 'pusup1400', 'pusup1500',],
    'hypoEFInfo'     : ['trk3','trk5','trk10','trk15',  'trk20',  'trk30',  'trk40', 'trk45', 'trk50', 'trk55', 'trk60', 'trk65', 'trk70', 'trk75', 'trk80', 'trk90',
                        'trk100', 'trk110', 'trk120', 'trk130', 'trk140', 'trk150', 'trk160', 'trk180', 'trk200', 'trk220', 'trk240', 'trk260', 'trk280',
                        'pt2', 'pt4', 'pt6', 'pt8', ],
    'hypoEFsumEtInfo': ['sumet40', 'sumet50', 'sumet60', 'sumet70', 'sumet80', 'sumet90', 'sumet110', 'sumet150',],
    'recoAlg'        : ['mbts', 'sptrk', 'sp', 'noalg', 'perf', 'hmt', 'hmtperf', 'idperf', 'zdcperf'],
    'addInfo'        : ['peb'],
    'sigFolder'     : 'MinBias',
    'subSigs'       : ['MinBias']
    }
# ---- MinBiasDictionary of default Values ----
MinBiasChainParts_Default = {
    'signature'      : ['MinBias'],
    'alignmentGroup' : ['MinBias'],
    'L1threshold'    : '',
    'chainPartName'  : '',
    'multiplicity'   : '',
    'trigType'       : ['mb'],
    'threshold'      : '',
    'extra'          : '',
    'IDinfo'         : '',
    'ZDCinfo'        : '',
    'trkInfo'        : '',
    'hypoL2Info'     : '',
    'pileupInfo'     : '',
    'hypoEFInfo'     : '',
    'hypoEFsumEtInfo': '',
    'recoAlg'        : [],
    'addInfo'        : [],
    'sigFolder'     : 'MinBias',
    'subSigs'       : ['MinBias']
    }

#==========================================================
# HeavyIon chains
#==========================================================
# ---- HeavyIon Dictionary of all allowed Values ----
HeavyIonChainParts = {
    'signature'      : ['HeavyIon'],
    'alignmentGroup' : ['HeavyIon'],
    'L1threshold'    : '',
    'chainPartName'  : '',
    'multiplicity'   : '',
    'trigType'       : ['hi'],
    'threshold'      : '',
    'extra'          : [],
    'IDinfo'         : [],
    'trkInfo'        : [],
    'eventShape'     : [],
    'eventShapeVeto' : [],
    'hypoL2Info'     : [],
    'pileupInfo'     : [],
    'hypoEFInfo'     : [],
    'hypoEFsumEtInfo': [],
    'recoAlg'        : [],
    'addInfo'        : [],
    'gap'            : [],
    'sigFolder'     : 'HeavyIon',
    'subSigs'       : ['HeavyIon']
    }

# ---- HeavyIonDictionary of default Values ----
HeavyIonChainParts_Default = {
    'signature'      : ['HeavyIon'],
    'alignmentGroup' : ['HeavyIon'],
    'L1threshold'    : '',
    'chainPartName'  : '',
    'multiplicity'   : '',
    'trigType'       : ['hi'],
    'threshold'      : '',
    'extra'          : '',
    'IDinfo'         : '',
    'trkInfo'        : '',
    'eventShape'     : '',
    'eventShapeVeto' : '',
    'hypoL2Info'     : '',
    'pileupInfo'     : '',
    'hypoEFInfo'     : '',
    'hypoEFsumEtInfo': '',
    'recoAlg'        : [],
    'addInfo'        : [],
    'gap'            : '',
    'sigFolder'     : 'HeavyIon',
    'subSigs'       : ['HeavyIon']
    }

#==========================================================
# ---- CosmicDef chains -----
#==========================================================
AllowedCosmicChainIdentifiers = ['larps','larhec',
                                 'sct',  'id',]

# ---- Cosmic Chain Dictionary of all allowed Values ----
CosmicChainParts = {
    'signature'      : ['Cosmic'],
    'alignmentGroup' : ['Cosmic'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'purpose'        : AllowedCosmicChainIdentifiers,
    'addInfo'        : ['cosmicid','noise', 'beam', 'laser', 'AllTE', 'central', 'ds','CIS'], #'trtd0cut'
    'trackingAlg'    : ['idscan', 'sitrack', 'trtxk'],
    'hits'           : ['4hits'],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'cosmic',
    'extra'          : '',
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['Cosmic']
    }

# ---- Cosmic Chain Default Dictionary of all allowed Values ----
CosmicChainParts_Default = {
    'signature'      : ['Cosmic'],
    'alignmentGroup' : ['Cosmic'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'purpose'        : [],
    'addInfo'        : [],
    'trackingAlg'    : [],
    'hits'           : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : '',
    'extra'          : '',
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['Cosmic']
    }

#==========================================================
# ---- StreamingDef chains -----
#==========================================================
AllowedStreamingChainIdentifiers = ['noalg']

# ---- Streaming Chain Dictionary of all allowed Values ----
StreamingChainParts = {
    'signature'      : ['Streaming'],
    'alignmentGroup' : ['Streaming'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'threshold'      : '',
    'multiplicity'   : '',
    'streamingInfo'  : ['bkg', 'idmon', 'mb', 'eb', 'zb','to','standby',
                        'jettauetmiss', 'larcells',
                        'cosmiccalo', 'cosmicmuons','idcosmic', 'dcmmon',
                        'zb', 'l1calo', 'l1topo','ftk'],
    'trigType'       : 'streamer',
    'extra'          : '',
    'streamType'     : AllowedStreamingChainIdentifiers,
    'algo' : ['NoAlg'],
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['Streaming']
    }

# ---- Cosmic Chain Default Dictionary of all allowed Values ----
StreamingChainParts_Default = {
    'signature'      : ['Streaming'],
    'alignmentGroup' : ['Streaming'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'threshold'      : '',
    'multiplicity'   : '',
    'streamingInfo'  : '',
    'trigType'       : '',
    'extra'          : '',
    'streamType'     : '',
    'algo' : [],
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['Streaming']
    }

#==========================================================
# ---- CalibDef chains -----
#==========================================================
AllowedCalibChainIdentifiers = ['csccalib',     'larcalib',
                                'idcalib',      'l1calocalib',
                                'tilelarcalib',
                                'larnoiseburst','ibllumi',
                                'l1satmon',     'zdcpeb',
                                'calibAFP',
                                ]

# ---- Calib Chain Dictionary of all allowed Values ----
##stramingInfo not use in ChainConfiguration, only to distinguish streaming

CalibChainParts = {
    'signature'      : ['Calib'],
    'alignmentGroup' : ['Calib'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'purpose'        : AllowedCalibChainIdentifiers,
    'location'       : ['central', 'fwd'],
    'addInfo'        : ['loose','noise','beam'],
    'hypo'           : ['trk9', 'trk16', 'trk29', 'conej40', 'conej165', 'conej75_320eta490', 'conej140_320eta490','satu20em'],
    'streamingInfo'  : ['vdm',],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : ['trk'],
    'extra'          : ['bs',''],
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['Calib']
    }


# ---- Calib Chain Default Dictionary of all allowed Values ----
CalibChainParts_Default = {
    'signature'      : ['Calib'],
    'alignmentGroup' : ['Calib'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'purpose'        : [],
    'addInfo'        : [],
    'hypo'           : '',
    # 'hits'           : [],
    'streamingInfo'  : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : '',
    'trigType'       : '',
    'extra'          : '',
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['Calib']
    }

#==========================================================
# ---- MonitorDef chains -----
#==========================================================
AllowedMonitorChainIdentifiers = ['robrequest', 'timeburner',
                                  'idmon',
                                  'l1calooverflow', 'l1topodebug',
                                  'mistimemonl1bccorr','mistimemonl1bccorrnomu',
                                  'mistimemoncaltimenomu','mistimemoncaltime',
                                  'mistimemonj400',]

# ---- Monitor Chain Dictionary of all allowed Values ----
MonitorChainParts = {
    'signature'      : ['Monitor'],
    'alignmentGroup' : ['Monitor'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'monType'        : AllowedMonitorChainIdentifiers,
    'hypo'           : ['trkFS',],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'mon',
    'extra'          : '',
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['Monitor']
    }

# ---- Monitor Chain Default Dictionary of all allowed Values ----
MonitorChainParts_Default = {
    'signature'      : ['Monitor'],
    'alignmentGroup' : ['Monitor'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'monType'        : [],
    'hypo'           : '',
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : '',
    'extra'          : '',
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['Monitor']
    }

#==========================================================
# ---- EB chains -----
#==========================================================
AllowedEBChainIdentifiers = ['eb']

# ---- Enhanced Bias Chain Dictionary of all allowed Values ----
EnhancedBiasChainParts = {
    'signature'      : ['EnhancedBias'],
    'alignmentGroup' : ['EnhancedBias'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'algType'        : ['medium','firstempty','empty','unpairediso','unpairednoniso', 'low'],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : '',
    'extra'          : '',
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['EnhancedBias']
    }

# ---- EnhancedBias Chain Default Dictionary of all allowed Values ----
EnhancedBiasChainParts_Default = {
    'signature'      : ['EnhancedBias'],
    'alignmentGroup' : ['EnhancedBias'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'algType'        : 'physics',
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : '',
    'extra'          : '',
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['EnhancedBias']
    }

#==========================================================
# ---- BeamspotDef chains -----
#==========================================================
AllowedBeamspotChainIdentifiers = ['beamspot',]
BeamspotChainParts = {
    'signature'      : ['Beamspot'],
    'alignmentGroup' : ['Beamspot'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'monType'        : AllowedBeamspotChainIdentifiers,
    'location'       : ['vtx'],
    'addInfo'        : ['trkFS', 'allTE', 'activeTE','idperf'],
    'hypo'           : [],
    'l2IDAlg'        : ['L2StarB','trkfast'],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'beamspot',
    'extra'          : '',
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['Beamspot']
    }

# ---- Beamspot Chain Default Dictionary of all allowed Values ----
BeamspotChainParts_Default = {
    'signature'      : ['Beamspot'],
    'alignmentGroup' : ['Beamspot'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'monType'        : [],
    'addInfo'        : [],
    'hypo'           : [],
    'l2IDAlg'        : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'location'       : 'vtx',
    'trigType'       : 'beamspot',
    'extra'          : '',
    'sigFolder'     : 'CalibCosmicMon',
    'subSigs'       : ['Beamspot']
    }

#==========================================================
# Unconventional Tracking
#==========================================================
# ---- Unconventional Tracking Dictionary of all allowed Values ----
UnconventionalTrackingChainParts = {
    'signature'      : ['UnconventionalTracking'],
    'alignmentGroup' : ['UnconventionalTracking'],
    'L1threshold'    : '',
    'chainPartName'  : [],
    'multiplicity'   : '',
    'trigType'       : ['unconvtrk'],
    'threshold'      : '',
    'IDinfo'         : ['loose','medium','tight'],
    'isoInfo'        : ['iaggrmedium','iaggrloose','imedium','iloose'],
    'extra'          : ["isohpttrack", "fslrt"],
    'addInfo'        : [],
    'sigFolder'     : 'UnconventionalTracking',
    'subSigs'       : ['UnconventionalTracking']
}
# ---- Unconventional Tracking Dictionary of default Values ----
UnconventionalTrackingChainParts_Default = {
    'signature'      : ['UnconventionalTracking'],
    'alignmentGroup' : ['UnconventionalTracking'],
    'L1threshold'    : '',
    'chainPartName'  : [],
    'multiplicity'   : '',
    'IDinfo'         : '',
    'trigType'       : ['unconvtrk'],
    'isoInfo'        : '',
    'threshold'      : '',
    'extra'          : '',
    'addInfo'        : [],
    'sigFolder'     : 'UnconventionalTracking',
    'subSigs'       : ['UnconventionalTracking']
}

#==========================================================
# Combined Chains
#==========================================================
AllowedTopos_comb = ['03dRleg000leg00110']

# ---- Combined Dictionary of all allowed Values ----
CombinedChainParts = deepcopy(PhotonChainParts)
CombinedChainParts['signature'] = ['Photon','Muon']
CombinedChainParts['chainParts'] = ['g','mu'],
CombinedChainParts['topo'] = AllowedTopos_comb
# ---- Combined Dictionary of default Values ----
CombinedChainParts_Default = deepcopy(PhotonChainParts_Default)
CombinedChainParts_Default['signature'] = ['Photon','Muon']
CombinedChainParts_Default['chainParts'] = ['g','mu'],
CombinedChainParts_Default['trigType'] =['g','mu']
CombinedChainParts_Default['topo'] = []

#==========================================================
# ----- Allowed HLT Topo Keywords (also: generic topos like DR, DETA, DPHI...)
#==========================================================
#NOTE: removed jets from list, special case for VBF triggers
AllowedTopos = AllowedTopos_e + AllowedTopos_g + AllowedTopos_mu + AllowedTopos_Bphysics + AllowedTopos_xe + AllowedTopos_tau + AllowedTopos_comb

#==========================================================
# Obtain signature type
#==========================================================
def getSignatureNameFromToken(chainpart):
    import re
    theMatchingTokens = []
    reverseSliceIDDict = { value: key for key, value in SliceIDDict.items() } #reversed SliceIDDict
    for sig,token in SliceIDDict.items():
        if re.match(r'^\d*'+token+r'\d*\w*$', chainpart):
            theMatchingTokens += [token]
    if len(theMatchingTokens) == 1:
        return reverseSliceIDDict[theMatchingTokens[0]]
    elif len(theMatchingTokens)>1:
        log.error('[getSignatureNameFromToken] There are several signatures tokens, %s, matching the chain part %s. I don\'t know which one to use!',
                                  theMatchingTokens,chainpart)
    else:
        log.error('No signature matching chain part %s was found.', chainpart)
    
    raise Exception('[getSignatureNameFromToken] Cannot find signature from chain name, exiting.')
    
    return False


#==========================================================
# Signature dictionaries to use
#==========================================================
def getSignatureInformation(signature):
    if signature == 'Electron':
        return [ElectronChainParts_Default, ElectronChainParts]
    if signature == 'Photon':
        return [PhotonChainParts_Default, PhotonChainParts]
    if signature == "Jet":
        return [JetChainParts_Default, JetChainParts]
    if signature == "Bjet":
        return [bJetChainParts_Default, JetChainParts]
    if signature == "Tau":
        return [TauChainParts_Default, TauChainParts]
    if (signature == "Muon"):
        return [MuonChainParts_Default, MuonChainParts]
    if  (signature == "Bphysics"):
        return [BphysicsChainParts_Default, BphysicsChainParts]
    if  (signature == "Combined"):
        return [CombinedChainParts_Default, CombinedChainParts]
    if signature == "MET":
        return [METChainParts_Default, METChainParts]
    if signature == "XS":
        return [XSChainParts_Default, XSChainParts]
    if signature == "TE":
        return [TEChainParts_Default, TEChainParts]
    if signature == "MinBias":
        return [MinBiasChainParts_Default, MinBiasChainParts]
    if signature == "HeavyIon":
        return [HeavyIonChainParts_Default, HeavyIonChainParts]
    if signature == "Cosmic":
        return [CosmicChainParts_Default, CosmicChainParts]
    if signature == "Calib":
        return [CalibChainParts_Default, CalibChainParts]
    if signature == "Streaming":
        return [StreamingChainParts_Default, StreamingChainParts]
    if signature == "Monitor":
        return [MonitorChainParts_Default, MonitorChainParts]
    if signature == "Beamspot":
        return [BeamspotChainParts_Default, BeamspotChainParts]
    if signature == "EnhancedBias":
        return [EnhancedBiasChainParts_Default, EnhancedBiasChainParts]
    if signature == "UnconventionalTracking":
        return [UnconventionalTrackingChainParts_Default, UnconventionalTrackingChainParts]
    if signature == "Test":
        return [TestChainParts_Default, TestChainParts]
    else:
        raise RuntimeError("ERROR Cannot find corresponding dictionary for signature", signature)

#==========================================================
# Analysis the base pattern: <mult><signatureType><threshold><extraInfo>
#==========================================================
def getBasePattern():
    import re
    allTrigTypes = SliceIDDict.values()
    possibleTT = '|'.join(allTrigTypes)
    pattern = re.compile(r"(?P<multiplicity>\d*)(?P<trigType>(%s))(?P<threshold>\d+)(?P<extra>\w*)" % (possibleTT))
    return pattern
