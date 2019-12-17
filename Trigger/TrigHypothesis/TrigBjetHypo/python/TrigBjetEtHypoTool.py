
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV

log = logging.getLogger('TrigBjetEtHypoTool')

####################################################################################################

def TrigBjetEtHypoToolFromDict( chainDict ):

    """ set the name of the EtHypoTool (name=chain) and figure out the threshold and selection from conf """
    name    = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]
    
    conf_dict = { 'threshold' : chainPart['threshold'],
                  'multiplicity' : '1',
                  'bTag' : 'offperf',
                  'bConfig' : 'EF',
                  'minEta' : chainPart['etaRange'].split('eta')[0],
                  'maxEta' : chainPart['etaRange'].split('eta')[1],
                  'L1' : None }
    

    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoTool        
    tool = TrigBjetEtHypoTool( name )
    tool.AcceptAll   = False
    tool.EtThreshold  = float(conf_dict['threshold']) * GeV
    tool.MinEtaThreshold = float(conf_dict['minEta']) / 100
    tool.MaxEtaThreshold = float(conf_dict['maxEta']) / 100

    log.debug("name = %s, cut_j = %s ", name, tool.EtThreshold)
    return tool

def TrigBjetEtHypoToolFromName( name, conf ):
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName   
    
    decodedDict = dictFromChainName(conf)
    decodedDict['chainName'] = name # override

    return TrigBjetEtHypoToolFromDict( decodedDict )
    
####################################################################################################


if __name__ == "__main__":
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.enableMonitoring=['Validation']

    t = TrigBjetEtHypoToolFromName( "HLT_j35_ftf_subjesgscIS_boffperf_split_L1J15","HLT_j35_ftf_subjesgscIS_boffperf_split_L1J15" )
    assert t, "can't configure boffperf split"

    log.info( "\n\n TrigBjetEtHypoToolFromName ALL OK\n\n" )

