# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigMultiTrkHypoMT,TrigMultiTrkHypoToolMT
from TrigBphysHypo.TrigMultiTrkHypoMTMonitoringConfig import TrigMultiTrkHypoToolMTMonitoring
from TriggerJobOpts.TriggerFlags import TriggerFlags

def findL2teBaseName(cName,topos):
     indices = []
     for topo in topos:
         indices.append(cName.find(topo))
     firstTopoOccurance = sorted(indices,key=int)[0]
     
     return cName[:firstTopoOccurance-1]         

def TrigMultiTrkHypoToolMTFromDict( chainDict ):

    print chainDict
    config = TrigMultiTrkHypoMTConfig()    
    tool=config.ConfigurationHypoTool( chainDict )

    addMonitoring( tool, TrigMultiTrkHypoToolMTMonitoring, "TrigMultiTrkHypoToolMT", chainDict['chainName'] )
    
    return tool

def TrigMultiTrkHypoToolFromName( name, thresholdsHLT ):    
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName   
    decoder = DictFromChainName()    
    decodedDict = decoder.analyseShortName(thresholdsHLT, [], "") # no L1 info    
    decodedDict['chainName'] = name # override
    print decodedDict
    return TrigMultiTrkHypoToolMTFromDict( decodedDict )
        
class TrigMultiTrkHypoMTConfig():

    def ConfigurationHypoTool( self, chainDict):

        topoAlgs = chainDict['topo']
        topoAlgs = chainDict['chainName']
        print '*************topos******************'
        print topoAlgs
        #print findL2teBaseName(chainDict['chainName'],topoAlgs)

        tool = TrigMultiTrkHypoToolMT( chainDict['chainName'] )
  
        print "TrigMultiTrkHypoToolMTConfig: Set for algorithm ", topoAlgs

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


def addMonitoring(tool, monClass, name, thresholdHLT ):
     try:
         if 'Validation' in TriggerFlags.enableMonitoring() or 'Online' in TriggerFlags.enableMonitoring() or 'Cosmic' in TriggerFlags.enableMonitoring():
             tool.MonTool = monClass( name + "Monitoring_" + thresholdHLT ) 
     except AttributeError:
         tool.MonTool = ""
         print name, ' Monitoring Tool failed'
         
def getBphysThresholds(chainDict) :
    mult = 0
    mult_without_noL1 = 0
    trkmuons = []
    fexNameExt = ""

    for part in chainDict['chainParts'] :
        mult = mult + int(part['multiplicity'])
        #if not 'noL1' in  part['extra'] :
        #    mult_without_noL1 = mult_without_noL1 + int(part['multiplicity'])

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