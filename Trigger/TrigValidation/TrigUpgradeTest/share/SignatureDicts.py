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
    'Photon'  : 'g',
    'Muon'    : 'mu',
}

#==========================================================
# ---- Generic Template for all chains          ----
# ---- chainParts specific information given in ----
# ---- signature specific dictionaries below    ----
#==========================================================
ChainDictTemplate = {
    'chainName'    : '',
    'L1item'        : '',
    'signatures'    : '',
    'chainParts'   : [],
}

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
    'extra'          : ['noL1'],
    'hypo_iso'       : ['iloose', 'imedium', 'itight',],
    'fex_reco'       : ['msonly', 'l2msonly', 'l2idonly',],
    'overlapRemoval' : ['wOvlpRm', 'noOvlpRm', 'noMuCombOvlpRm'],
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
    'hypo_iso'       : '',    
    'fex_reco'       : '',    
    'overlapRemoval' : ['wOvlpRm'],
    }

#==========================================================
# Electron Chains
#==========================================================
# ---- Electron Dictinary of all allowed Values ----
ElectronChainParts = {
    'signature'      : ['Electron'],
    'multiplicity'   : '',    
    'chainPartName'  : '',
    'L1item'         : '',
    'extra'          : 'ion',
    'trigType'       : ['e'],
    'threshold'      : '',
    'etaRange'       : ['0eta250', '250eta490'],
    'IDinfo'         : ['loose', 'medium', 'tight', 'lhloose', 'lhmedium', 'lhtight',],
    'isoInfo'        : ['iloose','ivarloose','ivarmedium','ivartight',],
    'trkInfo'        : ['fasttr', 'hlttr', 'IDTrkNoCut','FwdBackTrk','idperf'],
    'lhInfo'         : ['nod0','nodphires','nodeta','smooth'],
    'addInfo'        : ['etcut','ringer','noringer',]                        
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
    'IDinfo'         : '',
    'isoInfo'        : [],
    'trkInfo'        : '',
    'lhInfo'         : '',
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
    'extra'          : '',
    'IDinfo'         : ['loose', 'medium', 'tight', ],
    'isoInfo'        : ['ivloose','iloose','imedium','itight', ],
    'reccalibInfo'   : ['MSonly', 'MGonly'],
    'trkInfo'        : ['fasttr', 'hlttr', 'ftk'],
    'addInfo'        : ['etcut','ringer'],
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
    'addInfo'        : [],
    }


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

            
#==========================================================
def getSignatureInformation(signature):
    if signature == 'Electron':
        return [ElectronChainParts_Default, ElectronChainParts]
    if signature == 'Photon':
        return [PhotonChainParts_Default, PhotonChainParts]
    if (signature == "Muon"):
        return [MuonChainParts_Default, MuonChainParts]
    else:
        raise RuntimeError("ERROR Cannot find corresponding dictionary")


#==========================================================
def getBasePattern():
    import re
    allTrigTypes = SliceIDDict.values()
    possibleTT = '|'.join(allTrigTypes)
    pattern = re.compile("(?P<multiplicity>\d*)(?P<trigType>(%s))(?P<threshold>\d+)(?P<extra>\w*)" % (possibleTT))
    return pattern


