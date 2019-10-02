# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logSignatureDict = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Menu.SignatureDicts")
from copy import deepcopy

#==========================================================
# This is stored in chainDict['Signature']
#==========================================================
SliceIDDict = {
    'Electron': 'e',
    'Photon'  : 'g',
    'Jet'     : 'j',
    'HT'      : 'ht',
    'Muon'    : 'mu',
    'Tau'     : 'tau',
    'MET'     : 'xe',
    'XS'      : 'xs',
    'TE'      : 'te',
    'MinBias' : 'mb',
    'HeavyIon' : 'hi',
    'Cosmic'  : 'cosmic',
    'Calibration'   : 'calib',
    'Streaming'     : 'streamer',
    'Monitoring'    : 'mon',
    'Beamspot'      : 'beamspot',
    'EnhancedBias'  : 'eb',
    'Test'          : 'TestChain',
}

AllowedSignatures = ["jet", "bjet", "ht",
                     "electron", "photon", "egamma",
                     "muon", 
                     "met",
                     "tau", 
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
    'stream'        : '',
    'groups'        : [],
    'EBstep'        : '',
    'chainParts'   : [],
    'topoStartFrom' : False,
}

#==========================================================
# Test chains
#==========================================================
# ---- Test Dictinary of all allowed Values ----
TestChainParts = {
    'L1threshold'    : '',
    'signature'      : ['Test'],
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['TestChain'],
    'threshold'      : '',
    'addInfo'        : [''],
    }

# ---- Test Dictionary of default Values ----
TestChainParts_Default = {
    'signature'      : ['Test'],
    'L1threshold'    : '',
    'multiplicity'   : '',    
    'trigType'       : '',
    'threshold'      : '',
    'addInfo'        : [],
    }

#==========================================================
# Jet
#==========================================================
AllowedTopos_jet = []
# ---- Jet Dictinary of all allowed Values ----
JetChainParts = {
    'signature'    : ['Jet'],
    'L1threshold'  : '',
    'chainPartName': '',
    'threshold'    : '',
    'multiplicity' : '',
    'etaRange'     : ['0eta320', '320eta490', '0eta240'],
    'gscThreshold' : ['gsc'],
    'trigType'     : ['j'],
    'extra'        : [],
    'cleaning'     : ['noCleaning',],
    'recoAlg'      : ['a4', 'a10', 'a10r', 'a10t'],
    'dataType'     : ['tc'],
    'calib'        : ['em', 'lcw'],
    'jetCalib'     : ['jes', 'subjes', 'subjesIS', 'nojcalib'],
    'scan'         : ['FS',],
    'addInfo'      : ['perf'],    

    'TLA'          : [],
    'dataScouting' : [],

    'topo'         : AllowedTopos_jet,

    'bTag'         : ['boffperf'  ,
                      'bmv2c2040' , 'bmv2c2050' , 'bmv2c2060' , 'bmv2c2070' , 'bmv2c2077' , 'bmv2c2085' , 
                      'bmv2c1040' , 'bmv2c1050' , 'bmv2c1060' , 'bmv2c1070' , 'bmv2c1077' , 'bmv2c1085' ,
                      'bhmv2c1040', 'bhmv2c1050', 'bhmv2c1060', 'bhmv2c1070', 'bhmv2c1077', 'bhmv2c1085'],
    'bTracking'    : [],
    'bConfig'      : ['split',],
    'bMatching'    : ['antimatchdr05mu'],
    'trkopt'       : [],
    'hypoScenario' : ['simple', 'vbenf',
                      'vbenfSEP30etSEP34mass35SEP50fbet',
                      'dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass'],
    'smc'          : ['30smcINF', '35smcINF', '40smcINF', '50smcINF', '60smcINF', 'nosmc'],
}

# ---- Jet Dictinary of default Values ----
JetChainParts_Default = {
    'signature'    : ['Jet'],
    'L1threshold'  : '',
    'threshold'    : '',
    'multiplicity' : '',
    'etaRange'     : '0eta320',
    'trigType'     :'j',
    'extra'        : '',
    'cleaning'     : 'noCleaning',
    'recoAlg'      :'a4',
    'dataType'     :'tc',
    'calib'        :'em',
    'jetCalib'     :'subjesIS',
    'scan'         :'FS',
    'addInfo'      : [],    
    'TLA'          : '',    
    'topo'         : [],  
    'bTag'         : '',
    'bTracking'    : '',
    'bConfig'      : [],
    'bMatching'    : [],
    'dataScouting' : '',
    'trkopt'       : 'notrk',
    'hypoScenario' : 'simple',
    'smc'          : 'nosmc',
    }

#==========================================================
# HT chains
#==========================================================
# ---- HT Dictinary of all allowed Values ----
HTChainParts = deepcopy(JetChainParts)
HTChainParts['signature']    = ['HT']
HTChainParts['trigType']     = ['ht']
HTChainParts['extra']     = ['j20', 'j25', 'j30', 'test4']

# ---- HTDictinary of default Values ----
HTChainParts_Default = deepcopy(JetChainParts_Default)
HTChainParts_Default['signature']    = ['HT']
HTChainParts_Default['trigType']     = 'ht'
HTChainParts_Default['extra']     = ''

#==========================================================
# Muon 
#==========================================================
AllowedTopos_mu = []

# ---- Muon Dictinary of all allowed Values ----
MuonChainParts = {
    'signature'      : ['Muon'],
    'L1threshold'    : '',
    'chainPartName'  : [],
    'multiplicity'   : '',    
    'trigType'       : ['mu'],
    'etaRange'       : ['0eta2550','0eta105'],
    'threshold'      : '',
    'extra'          : ['noL1', 'Comb', 'fast', 'msonly'],
    'IDinfo'         : [],
    'isoInfo'        : ['ivar','ivarmedium'],
    'invMassInfo'    : ['10invm70'],
    'addInfo'        : ['1step','idperf','3layersEC'],
    'topo'           : AllowedTopos_mu,
    'flavour'        : [],
    }
# ---- MuonDictinary of default Values ----
MuonChainParts_Default = {
    'signature'      : ['Muon'],
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
    }

#==========================================================
# Bphysics
#==========================================================
AllowedTopos_Bphysics = ['bJpsimumu','bUpsimumu','bBmumu','bDimu']

# ---- Bphysics Dictinary of all allowed Values ----
BphysicsChainParts = deepcopy(MuonChainParts)
BphysicsChainParts['signature'] = ['Bphysics']
BphysicsChainParts['topo'] = AllowedTopos_Bphysics

# ---- Bphysics Dictinary of default Values ----
BphysicsChainParts_Default = deepcopy(MuonChainParts_Default)
BphysicsChainParts_Default['signature'] = ['Bphysics']
BphysicsChainParts_Default['topo'] = []

#==========================================================
# Taus
#==========================================================
AllowedTopos_tau = []

# ---- Tau Dictionary of all allowed Values ----
TauChainParts = {
    'signature'    : ['Tau'],
    'L1threshold'  : '',
    'chainPartName': '',
    'threshold'    : '',
    'preselection' : ['tracktwo', 'tracktwoMVA', 'ptonly', ],
    'selection'    : ['medium1', 'mediumRNN', 'perf', ],
    'multiplicity' : '',
    'trigType'     : ['tau'],   
    'trkInfo'      : ['idperf'],
    'extra'        : '',
    'recoAlg'      : '',
    'calib'        : '',
    'addInfo'      : ['IdTest'],
    'topo'         : AllowedTopos_tau,
}
TauChainParts_Default = {
    'signature'    : ['Tau'],
    'L1threshold'  : '',
    'chainPartName': '',
    'threshold'    : '20',
    'preselection' : 'tracktwo',
    'selection'    : 'medium1',
    'multiplicity' : '',
    'trigType'     : ['tau'],   
    'trkInfo'      : [],
    'extra'        : '',
    'recoAlg'      : '',
    'calib'        : '',
    'addInfo'      : '',
    'topo'         : [],
}

#==========================================================
# MET
#==========================================================
AllowedTopos_xe = []
# ---- Met Dictinary of all allowed Values ----
METChainParts = {
    'signature'    : ['MET'],
    'L1threshold'  : '',
    'chainPartName': '',
    'threshold'    : '',
    'multiplicity' : '',
    'topo'         : AllowedTopos_xe,
    'trigType'     : ['xe'],   
    'extra'        : ['noL1'],
    'calib'        : ['lcw',],    
    'L2recoAlg'    : [],
    'EFrecoAlg'    : ['cell', 'tcpufit', 'mht'],
    'L2muonCorr'   : [],
    'EFmuonCorr'   : [],
    'addInfo'      : ['FStracks'],
    }
# ---- MetDictinary of default Values ----
METChainParts_Default = {
    'signature'      : ['MET'],
    'L1threshold'    : '',
    'trigType'       : ['xe'],
    'threshold'      : '',
    'extra'          : '',
    'calib'          : 'lcw',
    'L2recoAlg'      : '',
    'EFrecoAlg'      : 'cell',
    'L2muonCorr'     : '',
    'EFmuonCorr'     : '',
    'addInfo'        : '',
    }

#==========================================================
# XS
#==========================================================
# ---- xs Dictinary of all allowed Values ----
XSChainParts = METChainParts 
XSChainParts['signature'] = ['XS']
XSChainParts['trigType']  = ['xs'],   

# ---- xs Dictinary of default Values ----
XSChainParts_Default = METChainParts_Default
XSChainParts_Default['signature'] = ['XS']
XSChainParts_Default['trigType']  = ['xs']

#==========================================================
# TE
#==========================================================
# ---- te Dictinary of all allowed Values ----
TEChainParts = METChainParts 
TEChainParts['signature'] = ['TE']
TEChainParts['trigType']  = ['te'],   

# ---- te Dictinary of default Values ----
TEChainParts_Default = METChainParts_Default
TEChainParts_Default['signature'] = ['TE']
TEChainParts_Default['trigType']  = ['te']

#==========================================================
# Electron Chains
#==========================================================
AllowedTopos_e = ["Jpsiee","Zeg","Zee"]
# ---- Electron Dictinary of all allowed Values ----
ElectronChainParts = {
    'signature'      : ['Electron'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'extra'          : '',
    'multiplicity'   : '',    
    'trigType'       : ['e'],
    'threshold'      : '',
    'etaRange'       : [],
    'IDinfo'         : ['lhtight'],
    'isoInfo'        : [],
    'trkInfo'        : ['idperf'],
    'caloInfo'       : [],
    'lhInfo'         : [],
    'L2IDAlg'        : [],
    'addInfo'        : ['etcut', 'etcut1step',"v2","v3"],
    }
# ---- Egamma Dictinary of default Values ----
ElectronChainParts_Default = {
    'signature'      : ['Electron'],
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
    }

#==========================================================
# Photon chains
#==========================================================
# ---- Photon Dictinary of all allowed Values ----
PhotonChainParts = {
    'L1threshold'    : '',
    'signature'      : ['Photon'],
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['g'],
    'threshold'      : '',
    'extra'          : [],
    'IDinfo'         : ['etcut','loose','medium','tight'],
    'isoInfo'        : [],
    'reccalibInfo'   : [],
    'trkInfo'        : [],
    'caloInfo'       : [],
    'hypoInfo'       : '',
    'recoAlg'        : [],
    'FSinfo'         : [],
    'addInfo'        : ['etcut',],
    }

# ---- Photon Dictinary of default Values ----
PhotonChainParts_Default = {
    'signature'      : ['Photon'],
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
    }

#==========================================================
# MinBias chains
#==========================================================
# ---- MinBias Dictionary of all allowed Values ----
MinBiasChainParts = {
    'signature'      : ['MinBias'],
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
    }
# ---- MinBiasDictinary of default Values ----
MinBiasChainParts_Default = {
    'signature'      : ['MinBias'],
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
    }

#==========================================================
# HeavyIon chains
#==========================================================
# ---- HeavyIon Dictionary of all allowed Values ----
HeavyIonChainParts = {
    'signature'      : ['HeavyIon'],
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
    }

# ---- HeavyIonDictinary of default Values ----
HeavyIonChainParts_Default = { 
    'signature'      : ['HeavyIon'],
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
    'gap'            : ''
    }

#==========================================================
# ---- CosmicDef chains -----
#==========================================================
AllowedCosmicChainIdentifiers = ['larps','larhec',
                                 'tilecalib', 
                                 'sct',  'id',]

# ---- Cosmic Chain Dictinary of all allowed Values ----
CosmicChainParts = {
    'signature'      : ['Cosmic'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'purpose'        : AllowedCosmicChainIdentifiers,
    'addInfo'        : ['cosmicid','noise', 'beam', 'laser', 'AllTE', 'central', 'ds'], #'trtd0cut'
    'trackingAlg'    : ['idscan', 'sitrack', 'trtxk'],
    'hits'           : ['4hits'],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'cosmic', 
    'extra'          : '',
    }

# ---- Cosmic Chain Default Dictinary of all allowed Values ----
CosmicChainParts_Default = {
    'signature'      : ['Cosmic'],
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

    }

#==========================================================
# ---- StreamingDef chains -----
#==========================================================
AllowedStreamingChainIdentifiers = ['noalg']

# ---- Streaming Chain Dictinary of all allowed Values ----
StreamingChainParts = {
    'signature'      : ['Streaming'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'threshold'      : '',
    'multiplicity'   : '',
    'streamingInfo'  : ['bkg', 'idmon', 'mb', 'eb', 'zb','to','standby',
                        'hltpassthrough', 'jettauetmiss', 'larcells', 
                        'cosmiccalo', 'cosmicmuons','idcosmic', 'dcmmon',
                        'zb', 'l1calo', 'l1topo','ftk'],
    'trigType'       : 'streamer', 
    'extra'          : '',
    'streamType'     : AllowedStreamingChainIdentifiers,
    'algo' : ['NoAlg']
    }

# ---- Cosmic Chain Default Dictinary of all allowed Values ----
StreamingChainParts_Default = {
    'signature'      : ['Streaming'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'threshold'      : '',
    'multiplicity'   : '',
    'streamingInfo'  : 'hltpassthrough',
    'trigType'       : '', 
    'extra'          : '',
    'streamType'     : '',
    'algo' : [],
    }

#==========================================================
# ---- CalibDef chains -----
#==========================================================
AllowedCalibChainIdentifiers = ['csccalib',     'larcalib', 
                                'idcalib',      'l1calocalib', 
                                'tilelarcalib', 'alfacalib',
                                'larnoiseburst','ibllumi', 
                                'l1satmon',     'zdcpeb',
                                'calibAFP',
                                ]

# ---- Calib Chain Dictinary of all allowed Values ----
##stramingInfo not use in ChainConfiguration, only to distinguish streaming

CalibChainParts = {
    'signature'      : ['Calibration'],
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
    'extra'          : ['rerun','bs',''],
    }

# ---- Calib Chain Default Dictinary of all allowed Values ----
CalibChainParts_Default = {
    'signature'      : ['Calibration'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'purpose'        : [],
    'addInfo'        : [],
    'hypo'           : '',
    'hits'           : [],
    'streamingInfo'  : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : '',
    'trigType'       : '', 
    'extra'          : '',
    }

#==========================================================
# ---- MonitorDef chains -----
#==========================================================
AllowedMonitorChainIdentifiers = ['robrequest', 'timeburner',  'costmonitor',
                                  'cscmon', 'idmon',
                                  'l1calooverflow', 'l1topodebug',
                                  'mistimemonl1bccorr','mistimemonl1bccorrnomu',
                                  'mistimemoncaltimenomu','mistimemoncaltime',
                                  'mistimemonj400',]

# ---- Monitor Chain Dictinary of all allowed Values ----
MonitorChainParts = {
    'signature'      : ['Monitoring'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'monType'        : AllowedMonitorChainIdentifiers,
    'hypo'           : ['trkFS',],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'mon',
    'extra'          : '',
    }

# ---- Monitor Chain Default Dictinary of all allowed Values ----
MonitorChainParts_Default = {
    'signature'      : ['Monitoring'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'monType'        : [],
    'hypo'           : '',
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : '', 
    'extra'          : '',

    }

#==========================================================
# ---- EB chains -----
#==========================================================
AllowedEBChainIdentifiers = ['eb']

# ---- Enhanced Bias Chain Dictinary of all allowed Values ----
EnhancedBiasChainParts = {
    'signature'      : ['EnhancedBias'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'algType'        : ['high','firstempty','empty','unpairediso','unpairednoniso', 'low'],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : '',
    'extra'          : '',
    }

# ---- EnhancedBias Chain Default Dictinary of all allowed Values ----
EnhancedBiasChainParts_Default = {
    'signature'      : ['EnhancedBias'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'algType'        : 'physics',
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : '', 
    'extra'          : '',
    }

#==========================================================
# ---- BeamspotDef chains -----
#==========================================================
AllowedBeamspotChainIdentifiers = ['beamspot',]
BeamspotChainParts = {
    'signature'      : ['Beamspot'],
    'chainPartName'  : '',
    'L1threshold'    : '',
    'monType'        : AllowedBeamspotChainIdentifiers,
    'location'       : ['vtx'],
    'addInfo'        : ['trkFS', 'allTE', 'activeTE','idperf'],
    'hypo'           : [],
    'l2IDAlg'        : ['L2StarB','trkfast','FTK','FTKRefit'],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'beamspot',
    'extra'          : '',
    }

# ---- Beamspot Chain Default Dictinary of all allowed Values ----
BeamspotChainParts_Default = {
    'signature'      : ['Beamspot'],
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
    }

#==========================================================
# Combined Chains
#==========================================================
AllowedTopos_comb = []

# ---- Combined Dictinary of all allowed Values ----
CombinedChainParts = deepcopy(PhotonChainParts)
CombinedChainParts['signature'] = ['Photon','Muon']
CombinedChainParts['chainParts'] = ['g','mu'],
CombinedChainParts['topo'] = AllowedTopos_comb
# ---- Combined Dictinary of default Values ----
CombinedChainParts_Default = deepcopy(PhotonChainParts_Default)
CombinedChainParts_Default['signature'] = ['Photon','Muon']
CombinedChainParts_Default['chainParts'] = ['g','mu'],
CombinedChainParts_Default['trigType'] =['g','mu']
CombinedChainParts_Default['topo'] = []

#==========================================================
# ----- Allowed HLT Topo Keywords (also: generic topos like DR, DETA, DPHI...)
#==========================================================
#NOTE: removed jets from list, special case for VBF triggers
AllowedTopos = AllowedTopos_e + AllowedTopos_mu + AllowedTopos_Bphysics + AllowedTopos_xe + AllowedTopos_tau + AllowedTopos_comb


#==========================================================
# Obtain signature type
#==========================================================
def getSignatureNameFromToken(chainpart):
    theMatchingTokens = []
    reverseSliceIDDict = dict([(value, key) for key, value in SliceIDDict.iteritems()]) #reversed SliceIDDict
    for sig,token in SliceIDDict.items():
            if (token in chainpart):
                theMatchingTokens += [token]
    theToken = max(theMatchingTokens, key=len) # gets the longest string in t
    if len(theMatchingTokens)>0:
        if len(theMatchingTokens)>1:
            logSignatureDict.info('There are several signatures tokens, %s, matching this chain part %s. Picked %s.',
                                  theMatchingTokens,chainpart,theToken)
        return reverseSliceIDDict[theToken]
    logSignatureDict.error('No signature matching chain part %s was found.', chainpart)
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
    if signature == "HT":
        return [HTChainParts_Default, HTChainParts]
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
    if signature == "Calibration":
        return [CalibChainParts_Default, CalibChainParts]
    if signature == "Streaming":
        return [StreamingChainParts_Default, StreamingChainParts]
    if signature == "Monitoring":
        return [MonitorChainParts_Default, MonitorChainParts]
    if signature == "Beamspot":
        return [BeamspotChainParts_Default, BeamspotChainParts]
    if signature == "EnhancedBias":
        return [EnhancedBiasChainParts_Default, EnhancedBiasChainParts]
    if signature == "Test":
        return [TestChainParts_Default, TestChainParts]
    else:
        raise RuntimeError("ERROR Cannot find corresponding dictionary")

#==========================================================
# Analysis the base pattern: <mult><signatureType><threshold><extraInfo>
#==========================================================            
def getBasePattern():
    import re
    allTrigTypes = SliceIDDict.values()
    possibleTT = '|'.join(allTrigTypes)
    pattern = re.compile("(?P<multiplicity>\d*)(?P<trigType>(%s))(?P<threshold>\d+)(?P<extra>\w*)" % (possibleTT)) # noqa: W605
    return pattern



