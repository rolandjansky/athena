# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigMultiTrkHypoMT, TrigMultiTrkHypoToolMT  # noqa: F401
from TrigBphysHypo.TrigMultiTrkHypoMTMonitoringConfig import TrigMultiTrkHypoToolMTMonitoring


from AthenaCommon.Logging import logging
log = logging.getLogger('TrigMultiTrkHypoMTConfig')

def findL2teBaseName(cName,topos):
    indices = []
    for topo in topos:
        indices.append(cName.find(topo))
    firstTopoOccurance = sorted(indices,key=int)[0]
     
    return cName[:firstTopoOccurance-1]

def TrigMultiTrkHypoToolMTFromDict( chainDict ):

    config = TrigMultiTrkHypoMTConfig()
    tool = config.ConfigurationHypoTool( chainDict )

    tool.MonTool = TrigMultiTrkHypoToolMTMonitoring('MonTool')

    return tool

def TrigMultiTrkHypoToolFromName( name, thresholdsHLT ):
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    
    decodedDict = dictFromChainName(thresholdsHLT)
    decodedDict['chainName'] = name # override
    return TrigMultiTrkHypoToolMTFromDict( decodedDict )
        
class TrigMultiTrkHypoMTConfig(object):

    def ConfigurationHypoTool( self, chainDict):

        topoAlgs = chainDict['topo']
        topoAlgs = chainDict['chainName']
        #print findL2teBaseName(chainDict['chainName'],topoAlgs)

        tool = TrigMultiTrkHypoToolMT( chainDict['chainName'] )
  
        log.debug("Set for algorithm %s", topoAlgs)

        if 'nocut' in topoAlgs:
            tool.AcceptAll = True
        
        if 'noos' in topoAlgs:
            tool.OppositeSign = True
        
        tool.ApplyUpperMassCut = True
        tool.ApplyChi2Cut = True
        tool.Chi2VtxCut = 20
        tool.nBphysObjects = 1
        
        trkmuons = getBphysThresholds(chainDict)
        tool.trkPtThresholds = trkmuons
        
        if 'bJpsimumu' in topoAlgs:
            tool.LowerMassCut = 2500 #MeV
            tool.UpperMassCut = 4300 #MeV
            
        elif 'bUpsimumu' in topoAlgs:
            tool.LowerMassCut = 8000 #MeV
            tool.UpperMassCut = 12000 #MeV

        elif 'bBmumu' in topoAlgs:
            tool.LowerMassCut = 4000 #MeV
            tool.UpperMassCut = 8500 #MeV
            tool.Chi2VtxCut = 60

        elif 'bDimu' in topoAlgs:
            tool.LowerMassCut = 100 #MeV
            tool.UpperMassCut = 14000 #MeV        
        
        return tool 


def getBphysThresholds(chainDict) :
    mult = 0
    trkmuons = []
    fexNameExt = ""

    for part in chainDict['chainParts'] :
        mult = mult + int(part['multiplicity'])

    for dictpart in chainDict['chainParts']:
        if 'mu' in dictpart['trigType']:
            for x in range(0,int(dictpart['multiplicity'])):
                if dictpart['threshold']!='0':
                    dthr = float(dictpart['threshold'] ) 
                    thr= dthr * 1000.  # in MeV; 
                    
                    #lower to match EF muon threshols
                    if dthr < 9.5 :
                        thr = thr - 350.
                    elif dthr < 11.5 :
                        thr = thr - 550. 
                    elif dthr < 21.5  :
                        thr = thr - 750.                         
                    else :
                        thr = thr -1000.

                else :
                    thr = 900.
                trkmuons.append(thr)
                fexNameExt = fexNameExt + "_"+str(int(dictpart['threshold']))
    return trkmuons
