# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logSignatureDict = logging.getLogger("TriggerMenu.menu.SignatureDicts")
from copy import deepcopy

#==========================================================
##This is stored in chainDict['Signature']
#==========================================================
SliceIDDict = {
    'Electron': 'e',
    'Jet'     : 'j',
    'HT'      : 'ht',
    'Photon'  : 'g',
    'Muon'    : 'mu',
    'Tau'     : 'tau',
    'MET'     : 'xe',
    'XS'      : 'xs',
    'TE'      : 'te',
    'MinBias' : 'mb',
    'Cosmic'  : 'cosmic',
    'Calibration'   : 'calib',
    'Streaming'     : 'streamer',
    'Monitoring'    : 'mon',
    'Beamspot'      : 'beamspot',
    'EnhancedBias'  : 'eb',
    'Test'          : 'TestChain',
}

#==========================================================
# ---- Generic Template for all chains          ----
# ---- chainParts specific information given in ----
# ---- signature specific dictionaries below    ----
#==========================================================
ChainDictTemplate = {
    'chainName'    : '',
    'L1item'        : '',
    'topo'          : '',
    'signatures'    : '',
    'stream'        : '',
    'groups'        : [],
    'EBstep'        : '',
    'chainParts'   : [],
}



#==========================================================
# ----- Allowed HLT Topo Keywords (in addition to generic topos like DR, DETA, DPHI...)
#==========================================================
AllowedTopos_e = ["Jpsiee","Zeg","Zee"]
AllowedTopos_mu = ['Jpsimumu']
AllowedTopos_xe = ['dphi1','razor140', 'razor170', 'razor200']
# AllowedTopos_bphys = ['bJpsi', 'bTau', 'bDiMu', 
#                       'bJpsiMuMu', 'bUpsiMuMu', 
#                       'bMuMux', 'bMuMu', 
#                       'bMuMuxv2', 
#                       '02dr-2mu6', '2invm', 
#                       'BcMuMuDs' ,   'BcMuMuDsloose' , 
#                       '7invm9', 'noos',
#                       'Trkloose', 'Zmumu']
AllowedTopos_bphys = ['bJpsi', 'bTau', 'bDimu', 
                      'bJpsimumu', 'bUpsimumu', 
                      'bBmumu', 'bBmumux', 
                      'bBmumuxv2', 
                      '02dr-2mu6', '2invm', 
                      'BcmumuDs' ,   'BcmumuDsloose' , 
                      '7invm9', 'noos', 'noid', 'novtx',
                      '11invm60','18invm60', 'bUpsi',
                      'Trkloose', 'Zmumu',
                      'noinvm', 'ss', 'BpmumuKp']
AllowedTopos_jet = ['muvtx',
                    'llp',
                    'deta2', 
                    'invm400']
AllowedTopos_comb = ['taumass', 'dr05']

AllowedTopos = AllowedTopos_e+AllowedTopos_mu+AllowedTopos_bphys+AllowedTopos_jet+AllowedTopos_xe+AllowedTopos_comb


#==========================================================
# Jet
#==========================================================
# ---- Jet Dictinary of all allowed Values ----
JetChainParts = {
    'signature'    : ['Jet'],
    'L1item'       : '',
    'chainPartName': '',
    'threshold'    : '',
    'multiplicity' : '',
    'etaRange'     : ['0eta490','0eta320','0eta240', '240eta490','280eta320', '320eta490'],
    'trigType'     : ['j'],
    'extra'        : ['noL1',],
    'recoAlg'      : ["a4", "a10", "a10r"],
    'dataType'     : ['TT', 'tc', 'cc'],
    'calib'        : ["had","lcw","em"],
    'jetCalib'     : ["jes","sub","subjes", "nojcalib"],
    'scan'         : ['FS','PS'],
    'addInfo'      : ['perf'],    
    'topo'         : AllowedTopos_jet,
    'bTag'         : ['bloose', 'bmedium', 'btight', 'bperf','boffperf'],
    'bTracking'    : ['EFID'],
    'bConfig'      : ['split', 'hybrid', 'singlepass'],
    'dataScouting' : ['ds1', 'ds2'],
}
# ---- Jet Dictinary of default Values ----
JetChainParts_Default = {
    'signature'    : ['Jet'],
    'L1item'       : '',
    'threshold'    : '',
    'multiplicity' : '',
    'etaRange'     : '0eta320',
    'trigType'     :'j',
    'extra'        : '',
    'recoAlg'      :'a4',
    'dataType'     :'tc',
    'calib'        :'em',
    'jetCalib'     :'subjes',
    'scan'         :'FS',
    'addInfo'      : [],    
    'topo'         : [],  
    'bTag'         : '',
    'bTracking'    : '',
    'bConfig'      : [],
    'dataScouting' : '',

    }

#==========================================================
# Bjet
#==========================================================
# # ---- Bjet Dictinary of all allowed Values ----
# BjetChainParts = deepcopy(JetChainParts)
# AllowedBtaggingCuts = ['bloose', 'bmedium', 'btight']
# BjetChainParts['signature']    = ['Bjet']
# BjetChainParts['etaRange']    = ['0eta250']
# BjetChainParts['bTag'] = AllowedBtaggingCuts

# # ---- Bjet Dictinary of default Values ----
# BjetChainParts_Default = deepcopy(JetChainParts_Default)
# BjetChainParts_Default['signature']=['Bjet']
# BjetChainParts_Default['etaRange']    = '0eta250'
# BjetChainParts_Default['bTag'] = ''

#==========================================================
# HT chains
#==========================================================
# ---- HT Dictinary of all allowed Values ----
HTChainParts = deepcopy(JetChainParts)
HTChainParts['signature']    = ['HT']
HTChainParts['trigType']     = ['ht']
# ---- HTDictinary of default Values ----
HTChainParts_Default = deepcopy(JetChainParts_Default)
HTChainParts_Default['signature']    = ['HT']
HTChainParts_Default['trigType']     = 'ht'

#==========================================================
# Muon 
#==========================================================
# ---- Muon Dictinary of all allowed Values ----
MuonChainParts = {
    'signature'      : ['Muon'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['mu'],
    'etaRange'       : ['0eta105', '0eta250'],
    'threshold'      : '',
    'extra'          : ['noL1'],
    'IDinfo'         : [],
    'isoInfo'        : ['iloose', 'imedium', 'itight',],
    'reccalibInfo'   : ['msonly', 'idperf','muoncalib'],
    'trkInfo'        : ['fasttr', 'hlttr', 'ftk', 'IDT'],
    'hypoInfo'       : [],
    'FSinfo'         : ['ftkFS', 'nscan03', 'l2nscan03', 'nscan05', 'l2nscan05', 'JpsimumuFS', 'JpsimumuL2'],
    'L2IDAlg'        : ['L2Star','IdScan','FTK'],
    'L2SAAlg'        : ['muFast', 'l2muonSA',],
    'L2CBAlg'        : ['muComb',],
    'EFAlg'          : ['SuperEF'],
    'addInfo'        : ['cosmicEF', 'cosmic','IdTest','fsperf', 'ds1', 'ds2','ds3', 'r1extr', 'perf'],
    'overlapRemoval' : ['wOvlpRm', 'noOvlpRm'],
    'topo'           : AllowedTopos_mu,
    }
# ---- MuonDictinary of default Values ----
MuonChainParts_Default = {
    'signature'      : ['Muon'],
    'L1item'         : '',
    'multiplicity'   : '',    
    'trigType'       : ['mu'],
    'etaRange'       : '0eta250',    
    'threshold'      : '',
    'extra'          : '',    
    'IDinfo'         : '',    
    'isoInfo'        : '',    
    'reccalibInfo'   : '',    
    'trkInfo'        : '',    
    'hypoInfo'       : '',    
    'FSinfo'         : '',
    'L2IDAlg'        : ['L2StarA'],
    'L2SAAlg'        : ['l2muonSA'],
    'L2CBAlg'        : ['muComb'],
    'EFAlg'          : ['SuperEF'],
    'overlapRemoval' : ['wOvlpRm'],
    'addInfo'        : [],
    'topo'           : [],
    }

#==========================================================
# Bphysics
#==========================================================
# ---- Bphysics Dictinary of all allowed Values ----
BphysicsChainParts = deepcopy(MuonChainParts)
BphysicsChainParts['signature'] = ['Bphysics']
BphysicsChainParts['topo'] = AllowedTopos_bphys
# ---- Bphysics Dictinary of default Values ----
BphysicsChainParts_Default = deepcopy(MuonChainParts_Default)
BphysicsChainParts_Default['signature'] = ['Bphysics']
BphysicsChainParts_Default['topo'] = []


#==========================================================
# Taus
#==========================================================
# ---- Tau Dictionary of all allowed Values ----
TauChainParts = {
    'signature'    : ['Tau'],
    'L1item'       : '',
    'chainPartName': '',
    'threshold'    : '',
    'preselection' : ['r1', 'FTK', 'calo', 'track', 'mvonly', 'ptonly', 'caloonly', 'trackonly', 'tracktwo'],
    'selection'    : ['loose1', 'medium1', 'perf', 'r1medium1', 'r1perf', 'cosmic'],
    'multiplicity' : '',
    'trigType'     : ['tau'],   
    'trkInfo'      : ['idperf'],
    'extra'        : '',
    'recoAlg'      : '',
    'calib'        : '',
    'addInfo'      : ['IdTest'],
}
TauChainParts_Default = {
    'signature'    : ['Tau'],
    'L1item'       : '',
    'chainPartName': '',
    'threshold'    : '20',
    'preselection' : 'calo',
    'selection'    : 'medium1',
    'multiplicity' : '',
    'trigType'     : ['tau'],   
    'trkInfo'      : [],
    'extra'        : '',
    'recoAlg'      : '',
    'calib'        : '',
    'addInfo'      : '',
}

#==========================================================
# MET
#==========================================================
# ---- Met Dictinary of all allowed Values ----
METChainParts = {
    'signature'    : ['MET'],
    'L1item'       : '',
    'chainPartName': '',
    'threshold'    : '',
    'multiplicity' : '',
    'topo'         : AllowedTopos_xe,
    'trigType'     : ['xe'],   
    'extra'        : ['noL1'],
    'calib'        : ['lcw','had','em'],    
    'L2recoAlg'    : ['','l2fsperf','L2FS'],
    'EFrecoAlg'    : ['tc','cell','pueta','mht','pufit'],
    'L2muonCorr'   : ['','wL2MuFEB','wEFMuFEB'],
    'EFmuonCorr'   : ['','wEFMu'],
    'addInfo'      : [],
    }
# ---- MetDictinary of default Values ----
METChainParts_Default = {
    'signature'      : ['MET'],
    'L1item'         : '',
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
# ---- Electron Dictinary of all allowed Values ----
ElectronChainParts = {
    'signature'      : ['Electron'],
    'chainPartName'  : '',
    'L1item'         : '',
    'extra'          : '',
    'multiplicity'   : '',    
    'trigType'       : ['e'],
    'threshold'      : '',
    'etaRange'       : ['0eta250', '250eta490'],
    'IDinfo'         : ['loose', 'medium', 'tight', 'lhloose', 'lhmedium', 'lhtight', 'loose1', 'medium1', 'tight1', 'vloose', 'lhvloose'],
    'isoInfo'        : [ 'iloose'],
    'trkInfo'        : ['fasttr', 'hlttr', 'IDTrkNoCut','FwdBackTrk','idperf'],
    'caloInfo'       : ['L2EFCalo','HLTCalo'],
    'lhInfo'         : ['cutd0dphi','nod0','nodphi','nodphires'],
    'L2IDAlg'        : ['L2StarA','L2StarB','L2StarC','FTK','TRT','SiTrack','IdScan'],
    'addInfo'        : ['etcut','ringer','conv','etisem','gsf',
                        'L2Star','perf','IdTest'],
    }
# ---- Egamma Dictinary of default Values ----
ElectronChainParts_Default = {
    'signature'      : ['Electron'],
    'multiplicity'   : '',    
    'L1item'         : '',
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
    'L1item'         : '',
    'signature'      : ['Photon'],
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['g'],
    'threshold'      : '',
    'extra'          : ['i', 'i5', 'i6', 'ns', 'Tvh'],
    'IDinfo'         : [ 'loose', 'medium', 'tight', 'NoCut', 'nocut','loose1', 'medium1', 'tight1'],
    'isoInfo'        : '',
    'reccalibInfo'   : 'MSonly',
    'trkInfo'        : ['fasttr', 'hlttr', 'ftk'],
    'caloInfo'       : ['HLTCalo'],
    'hypoInfo'       : '',
    'recoAlg'        : [],
    'FSinfo'         : ['ftkFS',],
    'addInfo'        : ['etcut', 'jetcalibdelayed', 'cosmic', 'perf', 'hiptrt','ringer','conv'],
    }

# ---- Photon Dictinary of default Values ----
PhotonChainParts_Default = {
    'signature'      : ['Photon'],
    'L1item'         : '',
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
# Test chains
#==========================================================
# ---- Test Dictinary of all allowed Values ----
TestChainParts = {
    'L1item'         : '',
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
    'L1item'         : '',
    'multiplicity'   : '',    
    'trigType'       : '',
    'threshold'      : '',
    'addInfo'        : [],
    }




#==========================================================
# Combined Chains
#==========================================================
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
# MinBias chains
#==========================================================
# ---- MinBias Dictionary of all allowed Values ----
MinBiasChainParts = {
    'signature'      : ['MinBias'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['mb'],
    'threshold'      : '',
    'extra'          : ['noisesup', ],
    'IDinfo'         : [],
    'trkInfo'        : ['hlttr', 'ftk', 'costr'],
    'hypoL2Info'     : ['sp1800', 'sp2000', 'sp2500', 'sp3000', ],
    'pileupInfo'     : ['pusup600', 'pusup700', 'pusup750', 'pusup800',],
    'hypoEFInfo'     : ['trk50', 'trk60', 'trk70', 'trk90', 'trk120',],
    'hypoEFsumEtInfo': ['sumet70', 'sumet110', 'sumet150',],
    'recoAlg'        : ['mbts', 'sptrk', 'noalg', 'perf', 'hmt', 'hmtperf', 'idperf'],
    'addInfo'        : [ ],
    }
# ---- MinBiasDictinary of default Values ----
MinBiasChainParts_Default = {
    'signature'      : ['MinBias'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['mb'],
    'threshold'      : '',
    'extra'          : '',    
    'IDinfo'         : '',    
    'trkInfo'        : '',    
    'hypoL2Info'       : '',
    'pileupInfo'       : '',
    'hypoEFInfo'       : '',
    'hypoEFsumEtInfo': '',    
    'recoAlg'        : [],
    'addInfo'        : [],
    }





#==========================================================
# ---- CosmicDef chains -----
#==========================================================
AllowedCosmicChainIdentifiers = ['larps',
                                 'larhec',
                                 'tilecalib', 
                                 'pixel', 'sct', 
                                 'id',]

# ---- Cosmic Chain Dictinary of all allowed Values ----
CosmicChainParts = {
    'signature'      : ['Cosmic'],
    'chainPartName'  : '',
    'L1item'       : '',
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
    'L1item'       : '',
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
    'L1item'       : '',
    'threshold'      : '',
    'multiplicity'   : '',
    'streamingInfo'  : ['bkg', 'idmon', 'mb', 'eb', 
                        'hltpassthrough', 'jettauetmiss',
                        'cosmiccalo', 'cosmicmuons','idcosmic', 'dcmmon'],
    'trigType'       : 'streamer', 
    'extra'          : '',
    'streamType'        : AllowedStreamingChainIdentifiers,
    'algo' : ['NoAlg']
    }

# ---- Cosmic Chain Default Dictinary of all allowed Values ----
StreamingChainParts_Default = {
    'signature'      : ['Streaming'],
    'chainPartName'  : '',
    'L1item'       : '',
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
AllowedCalibChainIdentifiers = ['csccalib','larcalib', 'idcalib', 'l1calocalib', 'tilelarcalib','alfacalib','larnoiseburst']

# ---- Calib Chain Dictinary of all allowed Values ----
CalibChainParts = {
    'signature'      : ['Calibration'],
    'chainPartName'  : '',
    'L1item'         : '',
    'purpose'        : AllowedCalibChainIdentifiers,
    'location'       : ['central', 'fwd'],
    'calibType'      : [],
    'addInfo'        : [],
    'hypo'           : ['trk9', 'trk16', 'trk29'],
    'hits'           : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : ['trk'], 
    'extra'          : '',
    }

# ---- Calib Chain Default Dictinary of all allowed Values ----
CalibChainParts_Default = {
    'signature'      : ['Calibration'],
    'chainPartName'  : '',
    'L1item'       : '',
    'purpose'        : [],
    'calibType'      : [],
    'addInfo'        : [],
    'hypo'           : '',
    'hits'           : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : '',
    'trigType'       : '', 
    'extra'          : '',

    }
#==========================================================
# ---- MonitorDef chains -----
#==========================================================
AllowedMonitorChainIdentifiers = ['robrequest', 'timeburner', 'idmon', 'costmonitor','cscmon']

# ---- Monitor Chain Dictinary of all allowed Values ----
MonitorChainParts = {
    'signature'      : ['Monitoring'],
    'chainPartName'  : '',
    'L1item'       : '',
    'monType'        : AllowedMonitorChainIdentifiers,
    'location'       : [],
    'calibType'      : [],
    'addInfo'        : [],
    'hypo'           : ['trkFS',],
    'hits'           : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'mon',
    'extra'          : '',
    }

# ---- Monitor Chain Default Dictinary of all allowed Values ----
MonitorChainParts_Default = {
    'signature'      : ['Monitoring'],
    'chainPartName'  : '',
    'L1item'       : '',
    'monType'        : [],
    'calibType'      : [],
    'addInfo'        : [],
    'hypo'           : '',
    'hits'           : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : '',
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
    'L1item'       : '',
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
    'L1item'         : '',
    'algType'        : 'physics',
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : '',
    'trigType'       : '', 
    'extra'          : '',

    }

#==========================================================
# ---- BeamspotDef chains -----
#==========================================================
AllowedBeamspotChainIdentifiers = ['beamspot',]

# ---- Beamspot Chain Dictinary of all allowed Values ----
BeamspotChainParts = {
    'signature'      : ['Beamspot'],
    'chainPartName'  : '',
    'L1item'       : '',
    'monType'        : AllowedBeamspotChainIdentifiers,
    'location'       : ['vtx'],
    'addInfo'        : ['trkFS', 'allTE', 'activeTE'],
    'hypo'           : [],
    'l2IDAlg'        : ['L2StarB',],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'beamspot',
    'extra'          : '',
    'eventBuildType' : ['peb'],
    }

# ---- Beamspot Chain Default Dictinary of all allowed Values ----
BeamspotChainParts_Default = {
    'signature'      : ['Beamspot'],
    'chainPartName'  : '',
    'L1item'       : '',
    'monType'        : [],
    'addInfo'        : ['activeTE'],
    'hypo'           : [],
    'l2IDAlg'        : ['L2StarB'],
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : 'vtx',
    'trigType'       : 'beamspot', 
    'extra'          : '',
    'eventBuildType' : ['peb'],
    }

#==========================================================
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
            logSignatureDict.info('There are several signatures tokens, %s, matching this chain part %s. Picked %s.' % (theMatchingTokens,chainpart,theToken))            
        return reverseSliceIDDict[theToken]
    logSignatureDict.error('No signature matching chain part %s was found.' % (chainpart))
    return False


            
def getSignatureInformation(signature):
    if signature == 'Electron':
        return [ElectronChainParts_Default, ElectronChainParts]
    if signature == 'Photon':
        return [PhotonChainParts_Default, PhotonChainParts]
    if signature == "Jet":
        return [JetChainParts_Default, JetChainParts]
#    if signature == "Bjet":
#        return [BjetChainParts_Default, BjetChainParts]
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


def getBasePattern():
    import re
    import itertools
    # possibleTT = '|'.join(allowedSignaturePropertiesAndValues['trigType'])
    #print 'SignatureDicts.py: Allowed values for triType in base pattern', SliceIDDict.values()
    allTrigTypes = SliceIDDict.values()
    possibleTT = '|'.join(allTrigTypes)
    pattern = re.compile("(?P<multiplicity>\d*)(?P<trigType>(%s))(?P<threshold>\d+)(?P<extra>\w*)" % (possibleTT))
    return pattern



