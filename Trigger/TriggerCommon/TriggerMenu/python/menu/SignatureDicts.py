# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

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
    'AFP'     : 'afp',
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
    'topoStartFrom' : False,
}



#==========================================================
# ----- Allowed HLT Topo Keywords (in addition to generic topos like DR, DETA, DPHI...)
#==========================================================
AllowedTopos_e = ["Jpsiee","Zeg","Zee","Heg", "bBeexv2", "bBeexM2700", "bBeexM6000", "bBeexM6000t"]
AllowedTopos_mu = ['Jpsimumu']
AllowedTopos_xe = ['1dphi10', '2dphi05', '6dphi05', '6dphi15', '2dphi05', '2dphi10', '2dphi15', 'mt25', 'mt35', 'razor140', 'razor170', 'razor200','razor220','razor100','razor185','razor195']
AllowedTopos_bphys = ['bJpsi', 'bTau', 'bDimu', 'bDimu2700', 'bDimu6000', 'bNocut','bVertex2', 'bVertex3',
                      'bJpsimumu', 'bUpsimumu', 
                      'bBmumu', 'bBmumux', 
                      'bBmumuxv2', 'bBmumuxv3', 
                      '02dr-2mu6', '2invm', 
                      'BcmumuDs' ,   'BcmumuDsloose' ,
                      '7invm9', 'noos', 'noid', 'novtx',
                      '11invm60','18invm60', 'bUpsi',
                      'Trkloose',  'TrkPEB', 'TrkPEBmon', "lowpt",'Zmumu', 'noL2', 'noEFbph', 'PEB',
                      'noinvm', 'ss', 'BpmumuKp', 'Taumumux', 'Dsmumux', 'LbmumuLambda',
                      'trkTau', 'bTauTrk', 'bDsPhiX', 'bPhi','bDsPhiXtight', 'bTauTight',
                      '11invm24', '24invm60',
                      'tightChi2', 'Lxy0', 'Ftk','legacyVtx',
                      'BsmumuPhi', 'BsJpsiPhi']
AllowedTopos_jet = ['muvtx',
                    'revllp',
                    'llp',
		    'trkiso',
		    'noiso',
                    'pufix',
                    'deta2',
                    'deta20',
                    'deta25',
                    'deta30',
                    'deta35',
                    'deta40',
                    'deta45',
                    'dphi20',
                    'dphi24',
                    'dphi26',
                    'dphi28',
                    'invm250',
                    'invm400',
                    'invm400j40',
                    'invm500',
                    'invm600',
                    'invm700',
                    'invm700j50',
                    'invm800',
                    'invm900j0',
                    'invm900j50',
                    'invm900j0',
                    'invm900',
                    'invm1000',
                    'invm1000j50',
                    'invm1100j70',
                    'invm1200j50'
                    ]
AllowedTopos_Tau = ['ditauL', 'ditauM', 'ditauT','tautsf','notautsf','50mVis10000','60mVis10000','03dR27','03dR30']
AllowedTopos_comb = ['taumass', 'dr05', 'dz02','dz99', 'bXemu']

#AllowedTopos = AllowedTopos_e+AllowedTopos_mu+AllowedTopos_bphys+AllowedTopos_jet+AllowedTopos_xe+AllowedTopos_comb

#NOTE: removed jets from list, special case for VBF triggers
AllowedTopos = AllowedTopos_e + AllowedTopos_mu + AllowedTopos_bphys + AllowedTopos_xe + AllowedTopos_Tau + AllowedTopos_comb


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
    'etaRange'     : ['0eta490','0eta320','0eta240', '240eta490', '200eta320',
                      '280eta320', '320eta490', 'n320eta490', 'p320eta490'],
    'gscThreshold' : ['gsc'],
    'trigType'     : ['j'],
    'extra'        : ['noL1','test1','test2','test3', 'test4',
                      'test5', 'test6', 'delayed','AND','noTRT'],
    'cleaning'     : ['cleanL','cleanT','cleanLLP',
                      'cleanLA','cleanTA','cleanLLPA', 'noCleaning'
                      ],
    'recoAlg'      : ["a3","a4", "a10", "a10r", "a10t"],
    'dataType'     : ['TT', 'tc', 'cc', 'ion', 'sktc'],
    'calib'        : ["had","lcw","em"],
    'jetCalib'     : ["jes","sub","subjes","subjesIS", "nojcalib"],
    'scan'         : ['FS','PS'],
    'addInfo'      : ['perf'],    
    'TLA'          : ['0i1c200m400TLA', '0i1c400m600TLA', '0i1c600m800TLA',
                      '0i1c800m1000TLA', '0i1c500m2000TLA',
                      '1i2c100m300TLA','1i2c300m500TLA','1i2c500m700TLA',
                      '1i2c100m8000TLA','1i2c200m8000TLA','0i1c500m900TLA','1i2c500m900TLA','1i2c600m800TLA'],
    'topo'         : AllowedTopos_jet,
    'bTag'         : ['bloose', 'bmedium', 'btight', 'bperf','boffperf','bmv2c1040', 'bmv2c1050', 'bmv2c1060', 'bmv2c1070', 'bmv2c1077', 'bmv2c1085' ,'bmv2c2040', 'bmv2c2050', 'bmv2c2060', 'bmv2c2070', 'bmv2c2077', 'bmv2c2085','bhmv2c1040', 'bhmv2c1050', 'bhmv2c1060', 'bhmv2c1070', 'bhmv2c1077', 'bhmv2c1085', 'b40', 'b50', 'b60', 'b70','b77','b85' ],
    'bTracking'    : ['EFID','FTK','FTKVtx','FTKRefit'],
    'bConfig'      : ['split', 'hybrid', 'singlepass'],
#    'bMatching'    : ['mu4antidr05', 'mu6antidr05'],
    'bMatching'    : ['antimatchdr05mu'],
    'dataScouting' : ['ds1', 'ds2'],
    'smc'          : ['30smcINF', '35smcINF', '40smcINF', '50smcINF', '60smcINF', 'nosmc'],
    'trkopt'       : ['notrk', 'ftk', 'ftkrefit'],
    # Et cut by the jet build tool (ptmin)
    'recoCutUncalib': ['rcuDefault', 'rcu0', 'rcu5'],
    
    # Et cut by the jet build tool (ptminFilter)
    'recoCutCalib': ['rccDefault', 'rcc0', 'rcc5'],
    # 'dPhi': ['dphi24', 'dphi26'],

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
    'smc'          : 'nosmc',
    'trkopt'       : 'notrk',
    'recoCutUncalib': 'rcuDefault',
    'recoCutCalib': 'rccDefault',
    'dj'           : '',
    # 'dPhi'         : ''
    }
from TriggerJobOpts.TriggerFlags import TriggerFlags

if "v6" in TriggerFlags.triggerMenuSetup():
    JetChainParts_Default.update({'jetCalib'     :'subjes',})

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
# ---- Muon Dictinary of all allowed Values ----
MuonChainParts = {
    'signature'      : ['Muon'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['mu'],
    'etaRange'       : ['0eta105', '0eta250','0eta010','0eta500'],
    'threshold'      : '',
    'extra'          : ['noL1', '3layersEC'],
    'IDinfo'         : [],
    'isoInfo'        : ['iloose', 'imedium', 'itight', 'ivarloose', 'ivarmedium','icalo','iloosecalo','imediumcalo','iloosems', 'ivarloosecalo', 'ivarmediumcalo', 'ivartight', 'ivarverytight'],
    'reccalibInfo'   : ['msonly', 'l2msonly', 'l2idonly', 'nomucomb', 'idperf','muoncalib', 'mucombTag','muL2', 'mgonly', 'btrk'],
    'trkInfo'        : ['fasttr', 'hlttr', 'IDT'],
    'hypoInfo'       : [],
    'FSinfo'         : ['FTKFS', 'nscan03', 'l2nscan03', 'nscan05', 'l2nscan05', 'JpsimumuFS', 'JpsimumuL2','calotag','TagandProbe'],
    'L2IDAlg'        : ['L2Star','IdScan','FTK','FTKRefit'],
    'L2SAAlg'        : ['muFast', 'l2muonSA',],
    'L2CBAlg'        : ['muComb',],
    'EFAlg'          : ['SuperEF'],
    'addInfo'        : ['cosmicEF', 'cosmic','IdTest','fsperf', 'ds1', 'ds2','ds3', 'r1extr', 'perf', 'noEF','10invm30','pt2','z10','llns','noComb','zROItest','invm1','inTimeRoI','dRl1','tstight','tsloose'],
    'overlapRemoval' : ['wOvlpRm', 'noOvlpRm', 'noMuCombOvlpRm'],
    'topo'           : AllowedTopos_mu,
    'flavour'        : ['hf'],
    'specialStream'  : ['delayed']
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
    'flavour'        : '',
    'specialStream'  : '',
    }

#==========================================================
# Bphysics
#==========================================================
# ---- Allowed Keywords ----
#AllowedMatchingKeywords = ['antidr']
# ---- Bphysics Dictinary of all allowed Values ----
BphysicsChainParts = deepcopy(MuonChainParts)
#BphysicsChainParts['bTracking'] = ['FTK']
BphysicsChainParts['signature'] = ['Bphysics']
BphysicsChainParts['peb'] = ['peb075']
BphysicsChainParts['topo'] = AllowedTopos_bphys
# ---- Bphysics Dictinary of default Values ----
BphysicsChainParts_Default = deepcopy(MuonChainParts_Default)
BphysicsChainParts_Default['signature'] = ['Bphysics']
BphysicsChainParts_Default['topo'] = []
BphysicsChainParts['peb'] = []
BphysicsChainParts_Default['overlapRemoval'] = ['noOvlpRm']

#==========================================================
# Taus
#==========================================================
# ---- Tau Dictionary of all allowed Values ----
TauChainParts = {
    'signature'    : ['Tau'],
    'L1item'       : '',
    'chainPartName': '',
    'threshold'    : '',
    'preselection' : ['r1', 'FTK', 'FTKRefit', 'FTKNoPrec', 'calo', 'track', 'mvonly', 'ptonly', 'caloonly', 'trackonly', 'tracktwo', 'tracktwoEF', 'tracktwoEFmvaTES', 'trackcalo', 'tracktwocalo','tracktwo2015', 'tracktwoMVA'],
    'selection'    : ['medium0','loose1', 'medium1', 'medium1NoPt', 'tight1', 'perf', 'perf0', 'r1medium1', 'r1perf', 'cosmic', 'kaonpi1', 'kaonpi2', 'dipion1', 'dipion1loose', 'dipion2', 'dipion3', 'dikaon', 'dikaontight', 'dikaonmass', 'dikaonmasstight', 'singlepion', 'singlepiontight', 'verylooseRNN', 'looseRNN', 'mediumRNN', 'tightRNN'],
    'multiplicity' : '',
    'trigType'     : ['tau'],   
    'trkInfo'      : ['idperf'],
    'extra'        : '',
    'recoAlg'      : '',
    'calib'        : '',
    'addInfo'      : ['IdTest'],
    'topo'         : AllowedTopos_Tau,
}
TauChainParts_Default = {
    'signature'    : ['Tau'],
    'L1item'       : '',
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
    'jetCalib'     : JetChainParts['jetCalib'],
    'L2recoAlg'    : ['','l2fsperf','L2FS'],
    'EFrecoAlg'    : ['tc','cell','pueta','mht','trkmht','pufit', 'pufittrack', 'trktc'],
    'L2muonCorr'   : ['','wL2MuFEB','wEFMuFEB'],
    'EFmuonCorr'   : ['','wEFMu'],
    'addInfo'      : ['FStracks','FTK','LArH11offMETphi','LArH12offMETphi','LArH11offLArH12offMETphi','LArH11offMETphiJpt1','LArH11offMETphiJpt2','LArH11offMETphiJpt5','LArH11offMETphiJpt10','LArH11offMETphiJpt20','LArH11offMETphiJpt30','LArH12offMETphiJpt1','LArH12offMETphiJpt2','LArH12offMETphiJpt5','LArH12offMETphiJpt10','LArH12offMETphiJpt20','LArH12offMETphiJpt30','LArH11offLArH12offMETphiJpt1','LArH11offLArH12offMETphiJpt2','LArH11offLArH12offMETphiJpt5','LArH11offLArH12offMETphiJpt10','LArH11offLArH12offMETphiJpt20','LArH11offLArH12offMETphiJpt30','LArH11offJpt1','LArH11offJpt2','LArH11offJpt5','LArH11offJpt10','LArH11offJpt20','LArH11offJpt30','LArH12offJpt1','LArH12offJpt2','LArH12offJpt5','LArH12offJpt10','LArH12offJpt20','LArH12offJpt30','LArH11offLArH12offJpt1','LArH11offLArH12offJpt2','LArH11offLArH12offJpt5','LArH11offLArH12offJpt10','LArH11offLArH12offJpt20','LArH11offLArH12offJpt30'],
    }
# ---- MetDictinary of default Values ----
METChainParts_Default = {
    'signature'      : ['MET'],
    'L1item'         : '',
    'trigType'       : ['xe'],
    'threshold'      : '',
    'extra'          : '',
    'calib'          : 'lcw',
    'jetCalib'       : JetChainParts_Default['jetCalib'],
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
    'extra'          : 'ion',
    'multiplicity'   : '',    
    'trigType'       : ['e'],
    'threshold'      : '',
    'etaRange'       : ['0eta250', '250eta490'],
    'IDinfo'         : ['loose', 'medium', 'tight', 'lhloose', 'lhmedium', 'lhtight', 'loose1', 'medium1', 'tight1', 'vloose', 'lhvloose', 'mergedtight', 'bloose'],
    'isoInfo'        : [ 'iloose','ivarloose','ivarmedium','ivartight','icalovloose','icaloloose','icalomedium','icalotight'],
    'trkInfo'        : ['fasttr', 'hlttr', 'IDTrkNoCut','FwdBackTrk','idperf'],
    'caloInfo'       : ['L2EFCalo','HLTCalo'],
    'lhInfo'         : ['cutd0dphideta','nod0','nodphires','nodeta','smooth'],
    'L2IDAlg'        : ['L2StarA','L2StarB','L2StarC','FTK','TRT','SiTrack','IdScan'],
    'addInfo'        : ['etcut','ringer','noringer','conv','etisem','gsf','trkcut','sc',
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
    'isoInfo'        : [],
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
    'extra'          : ['i', 'i5', 'i6', 'ns', 'Tvh', 'ion'],
    'IDinfo'         : ['loose', 'medium', 'tight', 'NoCut', 'nocut','loose1', 'medium1', 'tight1'],
    'isoInfo'        : ['ivloose','iloose','imedium','itight','icalovloose','icaloloose','icalomedium','icalotight'],
    'reccalibInfo'   : ['MSonly', 'MGonly'],
    'trkInfo'        : ['fasttr', 'hlttr', 'ftk'],
    'caloInfo'       : ['HLTCalo'],
    'hypoInfo'       : '',
    'recoAlg'        : [],
    'FSinfo'         : ['ftkFS',],
    'addInfo'        : ['etcut', 'jetcalibdelayed', 'cosmic', 'perf', 'hiptrt','ringer','conv','larpeb','sc'],
    'topo'           : ['dPhi15'],
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
    'isoInfo'        : [],
    'reccalibInfo'   : '',
    'trkInfo'        : '',
    'caloInfo'       : '',    
    'hypoInfo'       : '',
    'recoAlg'        : '',
    'FSinfo'         : '',
    'addInfo'        : [],
    'topo'           : '',
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
# AFP
#==========================================================
# ---- AFP Dictinary of all allowed Values ----
AFPChainParts = {
    'signature'      : ['AFP'],
    'L1item'         : ['AFP_A_AND_C'],
    'chainPartName'  : '',
    'multiplicity'   : '',
    'calib'        : ['lcw','had','em'],
    'trigType'       : ['afp'],
    'jetCalib'     : JetChainParts['jetCalib'],
    'threshold'      : '',
    'extra'          : ['jetexc'],
    'addInfo'        : []
    }
# ---- AFPDictinary of default Values ----
AFPChainParts_Default = {
    'signature'      : ['AFP'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',
    'calib'          : 'lcw',
    'trigType'       : ['afp'],
    'jetCalib'       : JetChainParts_Default['jetCalib'],
    'threshold'      : '',
    'extra'          : '',
    'addInfo'        : []
    }


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
    'extra'          : ['noisesup', 'vetombts2in', 'vetombts1side2in',  'vetospmbts2in', "vetosp" ,'ion', 'ncb', 'blayer', 'exclusiveloose', 'exclusivetight'], #ncb = non collision background, blayer = only sum innermost pix layer
    'IDinfo'         : [],
    'ZDCinfo'        : ['lg', 'hg'],
    'trkInfo'        : ['hlttr', 'ftk', 'costr'],
    'hypoL2Info'     : ['sp2', 'sp3', 'sp5', 'sp10', 'sp15', 'sp100', 'sp300', 'sp400', 'sp500', 'sp600', 'sp700', 'sp800', 'sp900',
                        'sp1000', 'sp1100', 'sp1200', 'sp1300', 'sp1400', 'sp1500', 'sp1600', 'sp1700', 'sp1800', 'sp1900',
                        'sp2000', 'sp2100', 'sp2200', 'sp2300', 'sp2400', 'sp2500', 'sp2700', 'sp2800', 'sp2900', 'sp3000',
                        'sp3100', 'sp3500', 'sp4100', 'sp4500', 'sp4800', 'sp5000', 'sp5200',],
    'pileupInfo'     : ['pusup200','pusup300','pusup350', 'pusup400', 'pusup450', 'pusup500', 'pusup550', 'pusup600', 'pusup650', 'pusup700', 'pusup750', 'pusup800', 'pusup850', 'pusup900',
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
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['mb'],
    'threshold'      : '',
    'extra'          : '',    
    'IDinfo'         : '',
    'ZDCinfo'        : '',    
    'trkInfo'        : '',    
    'hypoL2Info'       : '',
    'pileupInfo'       : '',
    'hypoEFInfo'       : '',
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
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',
    'trigType'       : ['hi'],
    'threshold'      : '',
    'extra'          : ['th1', 'th2', 'th3', 'th4', 'th5', 'th6', 'th7', 'th8', 'th9', 'th10', 'th11', 'th12', 'th13', 'th14', 'th15', 'th16', 
                        'th0p', 'th005p', 'th01p', 'th025p', 'th05p', 'th10p', 'th15p', 'th20p',
                        'perf', 'perfzdc'],
    'IDinfo'         : [],
    'trkInfo'        : [],
    'eventShape'     : ['v2', 'v3', 'v23', 'v2A', 'v2C'],    
    'eventShapeVeto' : ['veto2', 'veto3'],
    'hypoL2Info'     : ['loose', 'medium', 'tight', 'gg',],
    'pileupInfo'     : ['zdcpu'],
    'hypoEFInfo'     : [],
    'hypoEFsumEtInfo': ['fcalet3000', 'fcalet3306', 'fcalet3391', 'fcalet3516',],
    'recoAlg'        : ['ucc', 'upc'],
    'addInfo'        : [ ],
    'gap'            : [ 'FgapA', 'FgapC', 'FgapAC',   'L2FgapA', 'L2FgapC', 'L2FgapAC',   'EFFgapA', 'EFFgapC', 'EFFgapAC' ],
    }

# ---- HeavyIonDictinary of default Values ----
HeavyIonChainParts_Default = { 
    'signature'      : ['HeavyIon'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',
    'trigType'       : ['hi'],
    'threshold'      : '',
    'extra'          : '',
    'IDinfo'         : '',
    'trkInfo'        : '',
    'eventShape'     : '',
    'eventShapeVeto' : '',
    'hypoL2Info'       : '',
    'pileupInfo'       : '',
    'hypoEFInfo'       : '',
    'hypoEFsumEtInfo': '',    
    'recoAlg'        : [],
    'addInfo'        : [],
    'gap'            : ''
    }


#==========================================================
# ---- CosmicDef chains -----
#==========================================================
AllowedCosmicChainIdentifiers = ['larps',
                                 'larhec',
                                 'tilecalib', 
                                 #'pixel', 
                                 'sct', 
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
    'streamingInfo'  : ['bkg', 'idmon', 'mb', 'eb', 'zb','to','standby',
                        'hltpassthrough', 'jettauetmiss', 'larcells', 
                        'cosmiccalo', 'cosmicmuons','idcosmic', 'dcmmon','zb', 'l1calo', 'l1topo','ftk'],
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
AllowedCalibChainIdentifiers = ['csccalib',
                                'larcalib', 
                                'idcalib', 
                                'l1calocalib', 
                                'tilelarcalib',
                                'alfacalib',
                                'larnoiseburst',
                                'ibllumi', 
                                'lumipeb',
                                #'vdm',
                                'lhcfpeb',
                                'alfaidpeb',
                                'larpebj',
                                'l1satmon',
                                'zdcpeb',
                                'calibAFP',
                                'calibAFPALFA',
                                'rpcpeb',
                                'idpsl1',
                                'larpebcalib'
                                ]

# ---- Calib Chain Dictinary of all allowed Values ----
##stramingInfo not use in ChainConfiguration, only to distinguish streaming

CalibChainParts = {
    'signature'      : ['Calibration'],
    'chainPartName'  : '',
    'L1item'         : '',
    'purpose'        : AllowedCalibChainIdentifiers,
    'location'       : ['central', 'fwd'],
    'calibType'      : [],
    'addInfo'        : ['loose','noise','beam'],
    'hypo'           : ['trk9', 'trk16', 'trk29', 'conej40', 'conej165', 'conej75_320eta490', 'conej140_320eta490','satu20em'],
    'hits'           : [],
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
    'L1item'       : '',
    'purpose'        : [],
    'calibType'      : [],
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
AllowedMonitorChainIdentifiers = ['robrequest', 'timeburner', 'idmon', 'costmonitor','cscmon','l1calooverflow','mistimemonl1bccorr','mistimemonl1bccorrnomu','mistimemoncaltimenomu','mistimemoncaltime','mistimemonj400','l1topodebug']

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
BeamspotChainParts = {
    'signature'      : ['Beamspot'],
    'chainPartName'  : '',
    'L1item'         : '',
    'monType'        : AllowedBeamspotChainIdentifiers,
    'location'       : ['vtx'],
#    'addInfo'        : ['trkFS','idperf'],
    'addInfo'        : ['trkFS', 'allTE', 'activeTE','idperf','mon'],
    'hypo'           : [],
    'l2IDAlg'        : ['L2StarB','trkfast','FTK','FTKRefit'],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'beamspot',
    'extra'          : '',
    'eventBuildType' : ['peb','pebTRT'],
    }

# ---- Beamspot Chain Default Dictinary of all allowed Values ----
BeamspotChainParts_Default = {
    'signature'      : ['Beamspot'],
    'chainPartName'  : '',
    'L1item'       : '',
    'monType'        : [],
    'addInfo'        : [],
    'hypo'           : [],
    'l2IDAlg'        : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : 'vtx',
    'trigType'       : 'beamspot', 
    'extra'          : '',
    'eventBuildType' : '',
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
    if signature == "AFP":
        return [AFPChainParts_Default, AFPChainParts]
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


def getBasePattern():
    import re
    # possibleTT = '|'.join(allowedSignaturePropertiesAndValues['trigType'])
    #print 'SignatureDicts.py: Allowed values for triType in base pattern', SliceIDDict.values()
    allTrigTypes = SliceIDDict.values()
    possibleTT = '|'.join(allTrigTypes)
    pattern = re.compile("(?P<multiplicity>\d*)(?P<trigType>(%s))(?P<threshold>\d+)(?P<extra>\w*)" % (possibleTT))
    return pattern



