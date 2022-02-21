# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
    'Streaming'     : 'streamer',
    'Monitor'    : 'mon',
    'Beamspot'      : 'beamspot',
    'EnhancedBias'  : 'eb',
    'UnconventionalTracking'  : 'unconvtrk',
    'Test'          : 'TestChain',
}

class ChainStore(dict):
    """Class to hold list of chains for each signature (dictionary with fixed set of keys)"""
    _allowedSignatures = ['Egamma', 'Muon', 'Jet', 'Bjet', 'Bphysics', 'MET', 'Tau',
                          'HeavyIon', 'Beamspot', 'Cosmic', 'EnhancedBias',
                          'Monitor', 'Calib', 'Streaming', 'Combined', 'MinBias',
                          'UnconventionalTracking', 'Test']

    def __init__(self):
        # Create dicionary with fixed set of keys in the orignal order
        super().__init__({s : [] for s in self._allowedSignatures})

    def __setitem__(self, key, value):
        if key not in self:
            raise RuntimeError(f"'{key}' is not in the list of allowed signatures: {self._allowedSignatures}")
        else:
            dict.__setitem__(self, key, value)


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
    'sigDicts' : {},
    'sigFolder'     : [],
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
    'sigFolder'     : ['Test'],
    'subSigs'       : ['Test'],
    'chainPartIndex': list(range(0,10))
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
    'sigFolder'     : ['Test'],
    'subSigs'       : ['Test'],
    'chainPartIndex': 0
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
    'sigFolder'     : ['Jet'],
    'subSigs'       : ['Jet'],
    'chainPartIndex': list(range(0,10)),
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
    'ionopt'       : # Heavy ion configuration
      ['noion','ion'],
    'trkopt'       : # Tracking configuration
      ['notrk','ftf','roiftf'],
    'trkpresel'    : # Tracking preselection
      ['nopresel',
       # Single jet
       'preselj20',
       'preselj50',
       'preselj80',
       'preselj120',
       'preselj140',
       'preselj180',
       'preselj200',
       'preselj225',
       # Multijets
       'presel2j140',
       'presel2j180',
       'presel2j200',
       'presel2j225',
       'presel3j45',
       'presel3j100',
       'presel3j125',
       'presel3j150',
       'presel4j20',
       'presel4c20',
       'presel4c25',
       'presel4c30',
       'presel4c35',
       'presel4c45',
       'presel4j25',
       'presel4c25',
       'presel4j40',
       'presel4c40',
       'presel4j45',
       'presel4j50',
       'presel4j85',
       'presel5j25',
       'presel5j50',
       'presel6j40',
       'presel7j30',
       # Multiple threshold
       'preselj60XXj40',
       'preselj140XXj45',
       'preselj80XX2j45',
       'presel2j180XXj80',
       # Nonstandard eta regions
       'presel6c25',
       'preselj45XX2f40',
       'preselc60XXc45XXc25XXc20', # L1J45p0ETA21_3J15p0ETA25
       'preselc60XXj45XXf40',
       'preselj60XXj45XXf40',
       'presela60XXa40XX2a25',
     ],
    # Hypo information
    #   If hypoScenario is 'simple', then hypo configuration is handled based on the
    #   other dict contents. If it is not 'simple', then the configuration is 100%
    #   from the hypoScenario specification, and all other hypo entries are ignored.
    #   Complete scenario names for aliases can be found in Trigger/TrigHypothesis/TrigHLTJetHypo/python/hypoConfigBuilder.py
    'hypoScenario' : ['simple', # Independent selections on individual jets, multiplicity+threshold cuts
                      # 'fbdj' (forward-backward + dijet) scenario:
                      #   default eta selection for dijet mass cut is 0eta490
                      'FBDJSHARED',  # Forward backward jets + dijet, default parameters, fb and dj can share
                      'FBDJNOSHARED10etXX20etXX34massXX50fbet', # f/b jets + dijet, expl. parameters, fb and dj do not share
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
                      #
                      # et threshold cuts
                      'DIJET80j12etXX0j12eta240XX700djmass', # Test dijet mass sel
                      'DIJET80j12etXX700djmassXXdjdphi260', # Test dijet mass sel including dphi cut
                      'DIJET70j12etXX1000djmassXXdjdphi200XX400djdeta', # dijet mass sel including dphi and deta cuts
                      'DIJET50j12etXX1000djmass',
                      'DIJET50j12etXX1000djmassXXdjdphi240',
                      'DIJET50j12etXX900djmass',
                      'DIJET35j12etXX1000djmass',
                      'DIJET20j12etXX110djmass',  # very loose cuts for testing
                      # pt threshold cuts
                      'DJMASS200j20', # alias, for TLA
                      'DJMASS350j20', # alias, for TLA
                      'DJMASS500j35', # alias
                      'DJMASS700j35', # alias
                      'DJMASS1000j35', # alias
                      'DJMASS700j40', # alias
                      'DJMASS700j50x0eta240', # alias
                      'DJMASS700j80x0eta240', # alias
                      'DJMASS900j50', # alias
                      'DJMASS1000j50', # alias
                      'DJMASS1000j50dphi240', # alias
                      'DJMASS1000j50dphi200x400deta', # alias
                      'DIJET80j12ptXX0j12eta240XX700djmass', # Test dijet mass sel
                      'DIJET80j12ptXX700djmassXXdjdphi260', # Test dijet mass sel including dphi cut
                      'DIJET70j12ptXX1000djmassXXdjdphi200XX400djdeta', # dijet mass sel including dphi and deta cuts
                      'DIJET20j12ptXX110djmass',  # very loose cuts for testing
                      'DIJETaliasExample',        # example of an alias for a dijet scenario with very loose cuts for testing
                      # no explicit pt cuts
                      'DIJET35j12ptXX700djmass',
                      'DIJET35j12ptXX500djmass',
                      # 'ht' category applies a cut on HT (HT>value) computed by aggregation over single jets (default filtering: 30et and 0eta320)
                      'HT0',
                      'HT1000',
                      'HT300',
                      'HT500',
                      'HT1000XX30et',
                      'HT500XX30et',
                      'HT50',
                      'HT50XX010jvt', # example of a HT chain using only jets passing JVT
                      'HT50XX30et',
                      'HT50XX30etXX010jvt', # example of a HT chain using only jets passing JVT
                      'HT100XX10et',
                      'HT50XX10etXX0eta320', # HT selection with explicit jet et/eta cuts
                      'HT50XX10ptXX0eta320' # HT selection with explicit jet et/eta cuts
                      ],

    'exotHypo' : ['emergingPTF0p2dR1p2', 'emergingPTF0p1dR1p2', 'emergingPTF0p09dR1p2', 'emergingPTF0p08dR1p2', 'emergingPTF0p075dR1p2', 'emergingPTF0p07dR1p2', 'emergingPTF0p0dR1p2', 
                  'emergingPTF0p2dR0p4', 'emergingPTF0p1dR0p4', 'emergingPTF0p09dR0p4', 'emergingPTF0p08dR0p4', 'emergingPTF0p075dR0p4', 'emergingPTF0p07dR0p4', 'emergingPTF0p0dR0p4', 
                  'tracklessdR1p2',      'tracklessdR0p4'],

    # Simple hypo configuration. Single property cuts defined as MINvarMAX
    'etaRange'      :
      ['0eta320', '320eta490', '0eta240', '0eta290', '0eta490', '0eta200', '0eta180', '0eta160'],
    'jvt'           : # Jet Vertex Tagger pileup discriminant
      ['010jvt', '011jvt', '015jvt', '020jvt', '050jvt', '059jvt'],
    'momCuts'       : # Generic moment cut on single jets
      ['050momemfrac100', 'momhecfrac010', '050momemfrac100XXmomhecfrac010'],
    'prefilters'      : # Pre-hypo jet selectors (including cleaning)
    ['CLEANlb', 'CLEANllp', 'MASK300ceta210XX300nphi10',
     # ptrangeXrY (X, Y matches regex \d+)  triggers a prehypo selection of
     # jets by ordering by pt, and selecting those with indices in [X,Y]
     'PTRANGE0r1',
     'PTRANGE2r3'],
    'bdips': ['95bdips','90bdips','85bdips','80bdips','77bdips'],
    'smc'           : # "Single mass condition" -- rename?
      ['30smcINF', '35smcINF', '40smcINF', '50smcINF', '60smcINF', 'nosmc'],
    # Setup for alternative data stream readout
    # B-tagging information
    'bTag'         : ['boffperf'  ,
                      'bdl1r60','bdl1r70','bdl1r77','bdl1r85',
                      'bdl1d60', 'bdl1d65','bdl1d70','bdl1d77',
                      'bdl1d80', 'bdl1d85'],
    'bTracking'    : [],
    'bConfig'      : ['split',],
    'bMatching'    : ['antimatchdr05mu'],
    'tboundary'    : ['SHARED'], # simple scenario tree boundary marker
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
    'sigFolder'     : ['Jet'],
    'subSigs'       : ['Jet'],
    'chainPartIndex': 0,
    #
    'recoAlg'       :'a4',
    'constitType'   :'tc',
    'clusterCalib'  :'em',
    'constitMod'    :'',
    'jetCalib'      :'default',
    'scan'          :'FS',
    'ionopt'        : 'noion',
    'trkopt'        : 'notrk',
    'trkpresel'     : 'nopresel',
    #
    'etaRange'      : '0eta320',
    'jvt'           : '',
    'momCuts'       : '',
    'prefilters'    : [],
    'bdips'         : '',
    'hypoScenario'  : 'simple',
    'exotHypo'      : [],
    'smc'           : 'nosmc',
    #
    'bTag'          : '',
    'bTracking'     : '',
    'bConfig'       : [],
    'bMatching'     : [],
    #
    'tboundary'     : '',
}

# ---- bJet Dictionary of default Values that are different to the ones for normal jet chains ----
bJetChainParts_Default = {
    'etaRange' : '0eta290',
    'sigFolder'     : ['Bjet'],
    'subSigs'       : ['Bjet'],

}

#==========================================================
# Muon
#==========================================================
AllowedTopos_mu = [
    'b7invmAB9vtx20', 'b11invmAB60vtx20', 'b11invmAB24vtx20', 'b24invmAB60vtx20',
    '50invmAB130' # Zmumu
    ]

# ---- Muon Dictionary of all allowed Values ----
MuonChainParts = {
    'signature'      : ['Muon'],
    'alignmentGroup' : ['Muon','MuonnoL1'],
    'L1threshold'    : '',
    'chainPartName'  : [],
    'multiplicity'   : '',
    'trigType'       : ['mu'],
    'etaRange'       : ['0eta105'],
    'threshold'      : '',
    'tnpInfo'        : ['probe'],
    'extra'          : ['noL1', 'lateMu', "muoncalib" ,'noL2Comb'],
    'IDinfo'         : [],
    'isoInfo'        : ['ivarloose', 'ivarmedium', 'ivarperf','iloosems'],
    'l2AlgInfo'      : ['l2io','l2mt'],
    'lrtInfo'        : ['d0loose','d0medium','d0tight'],
    'invMassInfo'    : ['invmJPsiOS','invmDimu'],
    'msonlyInfo'     : ['msonly'],
    'addInfo'        : ['idperf','LRT','3layersEC','cosmic',"muonqual","nscan"],
    'topo'           : AllowedTopos_mu,
    'flavour'        : [],
    'sigFolder'     : ['Muon'],
    'subSigs'       : ['Muon'],
    'chainPartIndex': list(range(0,10))
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
    'tnpInfo'        : '',
    'extra'          : '',
    'IDinfo'         : '',
    'isoInfo'        : '',
    'l2AlgInfo'      : [],
    'lrtInfo'        : [],
    'addInfo'        : [],
    'invMassInfo'    : '',
    'msonlyInfo'     : [],
    'topo'           : [],
    'flavour'        : '',
    'sigFolder'     : ['Muon'],
    'subSigs'       : ['Muon'],
    'chainPartIndex': 0
}

#==========================================================
# Bphysics
#==========================================================
AllowedTopos_Bphysics = [
    'bJpsimumu','bJpsi','bJpsimutrk','bUpsimumu','bUpsi','bBmumu','bDimu','bDimu2700','bDimu6000','bPhi','bTau','b3mu',
    'bBmumux', 'bBmux', 'b0dRAB12vtx20', 'b0dRAB127invmAB22vtx20', 'b0dRAB207invmAB22vtx20', 'b7invmAB22vtx20', 
    
    ##### TO BE REMOVED ONCE IMPLEMENTED IN SIGNATURE CODE
    # topoVariants
    'BsmumuPhi','BpmumuKp','BcmumuPi','BdmumuKst','LbPqKm','BcmumuDsloose','BcmumuDploose','BcmumuD0Xloose','BcmumuDstarloose',
    'BpmuD0X','BdmuDpX','BdmuDstarX','BsmuDsX','LbmuLcX',
    # topoExtras
    'Lxy0','noos','nocut'
    #########Remove until here############

]
AllowedTopos_Bphysics_topoVariant = [
    'BsmumuPhi','BpmumuKp','BcmumuPi','BdmumuKst','LbPqKm','BcmumuDsloose','BcmumuDploose','BcmumuD0Xloose','BcmumuDstarloose',
    'BpmuD0X','BdmuDpX','BdmuDstarX','BsmuDsX','LbmuLcX'
]
AllowedTopos_Bphysics_topoExtra = ['Lxy0','noos','nocut']
AllAllowedTopos_Bphysics = AllowedTopos_Bphysics_topoVariant+AllowedTopos_Bphysics_topoExtra+AllowedTopos_Bphysics

# ---- Bphysics Dictionary of all allowed Values ----
BphysicsChainParts = deepcopy(MuonChainParts)
BphysicsChainParts['signature'] = ['Bphysics']
BphysicsChainParts['sigFolder'] = ['Bphysics']
BphysicsChainParts['subSigs'] = ['Bphysics']
BphysicsChainParts['topo'] = AllowedTopos_Bphysics

# ---- Bphysics Dictionary of default Values ----
BphysicsChainParts_Default = deepcopy(MuonChainParts_Default)
BphysicsChainParts_Default['signature'] = ['Bphysics']
BphysicsChainParts_Default['sigFolder'] = ['Bphysics']
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
    'preselection'  : ['tracktwoMVA', 'tracktwoMVABDT', 'tracktwoLLP', 'ptonly', ],
    'selection'     : ['looseRNN', 'mediumRNN', 'tightRNN', 'perf', 'idperf',
                       'kaonpi1', 'kaonpi2', 'dipion1', 'dipion2', 'dipion3', 'dipion4', 'dikaonmass', 'singlepion'],
    'multiplicity'  : '',
    'trigType'      : ['tau'],
    'trkInfo'       : '',
    'tnpInfo'       : ['probe'],
    'extra'         : '',
    'recoAlg'       : '',
    'calib'         : '',
    'addInfo'       : ['IdTest'],
    'topo'          : AllowedTopos_tau,
    'sigFolder'     : ['Tau'],
    'subSigs'       : ['Tau'],
    'chainPartIndex': list(range(0,10))
}
TauChainParts_Default = {
    'signature'     : ['Tau'],
    'alignmentGroup': ['Tau'],
    'L1threshold'   : '',
    'chainPartName' : '',
    'threshold'     : '20',
    'preselection'  : '',
    'selection'     : '',
    'multiplicity'  :  '',
    'trigType'      : ['tau'],
    'trkInfo'       : [],
    'tnpInfo'       : '',
    'extra'         : '',
    'recoAlg'       : '',
    'calib'         : '',
    'addInfo'       :  '',
    'topo'          : [],
    'sigFolder'     : ['Tau'],
    'subSigs'       : ['Tau'],
    'chainPartIndex': 0
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
    'sigFolder'      : ['MET'],
    'subSigs'        : ['MET'],
    'constitmod'     : ['cssk', 'vssk'],
    'chainPartIndex': list(range(0,10))
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
    'sigFolder'     : ['MET'],
    'subSigs'       : ['MET'],
    'chainPartIndex': 0
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
AllowedTopos_e = ['Jpsiee','Zeg','Zee','Heg','bBeeM6000']
# ---- Electron Dictionary of all allowed Values ----
ElectronChainParts = {
    'signature'      : ['Electron'],
    'alignmentGroup' : ['Electron','Egamma'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'tnpInfo'        : ['probe'],
    'extra'          : ['ion'],
    'multiplicity'   : '',
    'trigType'       : ['e'],
    'threshold'      : '',
    'etaRange'       : [],
    'IDinfo'         : ['dnnloose','dnnmedium','dnntight','lhvloose','lhloose','lhmedium','lhtight','vloose','loose','medium','tight', 'mergedtight'],
    'isoInfo'        : ['ivarloose','ivarmedium','ivartight'],
    'idperfInfo'     : ['idperf'],
    'gsfInfo'        : ['gsf'],
    'lrtInfo'        : ['lrtloose','lrtmedium','lrttight'],
    'caloInfo'       : [],
    'lhInfo'         : ['nod0', 'nopix'],
    'L2IDAlg'        : ['noringer'],
    'addInfo'        : [ 'etcut', 'etcut1step',"fwd"],
    'sigFolder'     : ['Egamma'],
    'subSigs'       : ['Electron'],
    'topo'          : AllowedTopos_e,
    'chainPartIndex': list(range(0,10))
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
    'tnpInfo'        : '',
    'extra'          : '',
    'IDinfoType'     : '',
    'IDinfo'         : '',
    'isoInfo'        : '',
    'reccalibInfo'   : '',
    'idperfInfo'     : '',
    'gsfInfo'        : '',
    'lrtInfo'        : '', 
    'caloInfo'       : '',
    'lhInfo'         : '',
    'L2IDAlg'        : '',
    'hypoInfo'       : '',
    'recoAlg'        : '',
    'FSinfo'         : '',
    'addInfo'        : [],
    'sigFolder'     : ['Egamma'],
    'subSigs'       : ['Electron'],
    'topo'          : [],
    'chainPartIndex': 0
}

#==========================================================
# Photon chains
#==========================================================
# ---- Photon Dictionary of all allowed Values ----
AllowedTopos_g = ['dPhi25', 'm80']
PhotonChainParts = {
    'L1threshold'    : '',
    'signature'      : ['Photon'],
    'alignmentGroup' : ['Photon','Egamma'],
    'chainPartName'  : '',
    'multiplicity'   : '',
    'trigType'       : ['g'],
    'threshold'      : '',
    'extra'          : ['hiptrt', 'ion'],
    'IDinfo'         : ['etcut','loose','medium','tight'],
    'isoInfo'        : ['icaloloose','icalomedium','icalotight'],
    'reccalibInfo'   : [],
    'trkInfo'        : [],
    'caloInfo'       : [],
    'L2IDAlg'        : ['ringer'],
    'hypoInfo'       : '',
    'recoAlg'        : [],
    'FSinfo'         : [],
    'addInfo'        : ['etcut',],
    'sigFolder'     : ['Egamma'],
    'subSigs'       : ['Photon'],
    'topo'          : AllowedTopos_g,
    'chainPartIndex': list(range(0,10)),
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
    'L2IDAlg'        : '',
    'hypoInfo'       : '',
    'recoAlg'        : '',
    'FSinfo'         : '',
    'addInfo'        : [],
    'sigFolder'     : ['Egamma'],
    'subSigs'       : ['Photon'],
    'topo'          : [],
    'chainPartIndex': 0
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
    'extra'          : ['noisesup', 'vetombts2in', 'vetombts1side2in',  'vetospmbts2in', "vetosp" ,'ion', 'ncb', 'blayer', 'dijet'], #ncb = non collision background, blayer = only sum innermost pix layer
    'IDinfo'         : [],
    'ZDCinfo'        : ['lg', 'hg'],
    'trkInfo'        : ['hlttr', 'ftk', 'costr'],
    'hypoSPInfo'     : ['sp2', 'sp3', 'sp5', 'sp10', 'sp15', 'sp100', 'sp300', 'sp400', 'sp500', 'sp600', 'sp700', 'sp800', 'sp900',
                        'sp1000', 'sp1100', 'sp1200', 'sp1300', 'sp1400', 'sp1500', 'sp1600', 'sp1700', 'sp1800',
                        'sp2000', 'sp2100', 'sp2200', 'sp2300', 'sp2400', 'sp2500', 'sp2700', 'sp2800', 'sp2900', 'sp3000',
                        'sp3100', 'sp3500', 'sp4100', 'sp4500', 'sp4800', 'sp5000', 'sp5200',],
    'pileupInfo'     : ['pusup0','pusup40','pusup50','pusup60', 'pusup70', 'pusup80', 'pusup90', 'pusup100', 'pusup110', 'pusup120', 'pusup130', 'pusup180', 'pusup190',
                        'pusup200', 'pusup240', 'pusup250', 'pusup260', 'pusup270', 'pusup280', 'pusup290', 'pusup300'],
    'hypoTrkInfo'    : ['trk3','trk5','trk10','trk15',  'trk20',  'trk30',  'trk40', 'trk45', 'trk50', 'trk55', 'trk60', 'trk65', 'trk70', 'trk75', 'trk80', 'trk90',
                        'trk100', 'trk110', 'trk120', 'trk130', 'trk140', 'trk150', 'trk160', 'trk180', 'trk200', 'trk220', 'trk240', 'trk260', 'trk280', 'trk290',
                         '2trk6', '1trk5'], #ranges for exclusive tracks
    'hypoPtInfo'     : [ 'pt05', 'pt1', 'pt2', 'pt4', 'pt6', 'pt8', ],
    'hypoSumEtInfo'  : ['sumet40', 'sumet50', 'sumet60', 'sumet70', 'sumet80', 'sumet90', 'sumet110', 'sumet150',],
    'recoAlg'        : ['mbts', 'sptrk', 'sp', 'noalg', 'perf', 'hmt', 'hmtperf', 'idperf', 'zdcperf', 'alfaperf', 'afprec', 'afptof', 'excl'],
    'addInfo'        : ['peb'],
    'sigFolder'     : ['MinBias'],
    'subSigs'       : ['MinBias'],
    'chainPartIndex': list(range(0,10))
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
    'hypoSPInfo'     : '',
    'pileupInfo'     : '',
    'hypoTrkInfo'     : '',
    'hypoPtInfo'     : '',
    'hypoSumEtInfo': '',
    'recoAlg'        : [],
    'addInfo'        : [],
    'sigFolder'     : ['MinBias'],
    'subSigs'       : ['MinBias'],
    'chainPartIndex': 0
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
    'sigFolder'     : ['HeavyIon'],
    'subSigs'       : ['HeavyIon'],
    'chainPartIndex': list(range(0,10))
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
    'sigFolder'     : ['HeavyIon'],
    'subSigs'       : ['HeavyIon'],
    'chainPartIndex': 0
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
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['Cosmic'],
    'chainPartIndex': list(range(0,10))
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
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['Cosmic'],
    'chainPartIndex': 0
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
    # No effect on configuration, used in special cases for
    # disambiguation or to allow events from the same L1 seed
    # to be written to different streams
    # New cases should be discussed with Menu Coordinators
    'streamingInfo'  : ['laser', 'CIS','idmon','mb','l1calo', 'cosmicmuons', 'bkg'],
    'trigType'       : 'streamer',
    'extra'          : '',
    'streamType'     : AllowedStreamingChainIdentifiers,
    'algo' : ['NoAlg'],
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['Streaming'],
    'chainPartIndex': list(range(0,10))
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
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['Streaming'],
    'chainPartIndex': 0
    }

#==========================================================
# ---- CalibDef chains -----
#==========================================================
AllowedCalibChainIdentifiers = ['csccalib',     'larcalib',
                                'idcalib',      'l1calocalib',
                                'tilelarcalib',
                                'larnoiseburst','ibllumi',
                                'l1satmon',     'zdcpeb',
                                'calibAFP', 'larpsallem', 'larpsall', 
                                'acceptedevts',
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
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['Calib'],
    'chainPartIndex': list(range(0,10))
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
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['Calib'],
    'chainPartIndex': 0
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
    'isLegacyL1'     : ['legacy'],
    'trigType'       : 'mon',
    'extra'          : '',
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['Monitor'],
    'chainPartIndex': list(range(0,10))
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
    'isLegacyL1'     : [],
    'trigType'       : '',
    'extra'          : '',
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['Monitor'],
    'chainPartIndex': 0
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
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['EnhancedBias'],
    'chainPartIndex': list(range(0,10))
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
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['EnhancedBias'],
    'chainPartIndex': 0
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
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['Beamspot'],
    'chainPartIndex': list(range(0,10))
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
    'sigFolder'     : ['CalibCosmicMon'],
    'subSigs'       : ['Beamspot'],
    'chainPartIndex': 0
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
    'IDinfo'         : ['loose','medium','tight','vloose'],
    'isoInfo'        : ['iaggrmedium','iaggrloose','imedium','iloose'],
    'extra'          : ["isohpttrack", "fslrt", "dedx", "hitdv", "distrk", "dispj"],
    'addInfo'        : [],
    'sigFolder'     : ['UnconventionalTracking'],
    'subSigs'       : ['UnconventionalTracking'],
    'chainPartIndex': list(range(0,10))
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
    'sigFolder'     : ['UnconventionalTracking'],
    'subSigs'       : ['UnconventionalTracking'],
    'chainPartIndex': 0
}

#==========================================================
# Combined Chains
#==========================================================
AllowedTopos_comb = [
    'dRAA12', 'dRAB15', '03dRAB','03dRAB30','dRAB03','dRAB04','02dRAB','02dRAC','02dRBC','50invmAB','60invmAB','afpdijet','18dphiAB','18dphiAC','80mTAC',
    '90invmAB',# TEST
    '1invmAB5','50invmAB130', # Jpsiee, Zee/Zeg
    '25dphiAA','invmAA80', # Low-mass diphoton
    '10invmAA70', # Low-mass dimuon
    'invmAB10', '10invmAB70',
    '7invmAB9', '11invmAB60', '11invmAB24', '24invmAB60', '7invmAA9', '11invmAA60', '11invmAA24', '24invmAA60', # Low mass Drell-Yan
    ]

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
