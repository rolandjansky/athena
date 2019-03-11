# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import re
re_Bjet = re.compile(r'^HLT_(?P<multiplicity>\d+)?j(?P<threshold>\d+)(?:_gsc(?P<gscThreshold>\d+))?(?:_b(?P<bTag>[^_]+)(?:_(?P<bConfig>split))?(?:_(?P<minEta>\d+)eta(?P<maxEta>\d+))?)?(?:_L1(?P<L1>.*))?$')

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Constants import VERBOSE,DEBUG

####################################################################################################

def TrigBjetEtHypoToolFromDict_j( chainDict ):

    from AthenaCommon.Constants import DEBUG
    """ set the name of the EtHypoTool (name=chain) and figure out the threshold and selection from conf """
    name    = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]
    
    conf_dict = { 'threshold' : chainPart['threshold'],
                  'multiplicity' : '1',
                  'gscThreshold' : '0',
                  'bTag' : 'offperf',
                  'bConfig' : 'EF',
                  'minEta' : chainPart['etaRange'].split('eta')[0],
                  'maxEta' : chainPart['etaRange'].split('eta')[1],
                  'L1' : None }
    
    # chain = conf
    # match = re_Bjet.match( chain )
    # conf_dict = match.groupdict()

    # for k, v in default_conf.items():
    #     if k not in conf_dict: conf_dict[k] = v
    #     if conf_dict[k] == None: conf_dict[k] = v

    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoTool        
    tool = TrigBjetEtHypoTool( name )
    tool.OutputLevel = DEBUG
    tool.AcceptAll   = False
    tool.EtThreshold  = float(conf_dict['threshold']) * GeV
    tool.MinEtaThreshold = float(conf_dict['minEta']) / 100
    tool.MaxEtaThreshold = float(conf_dict['maxEta']) / 100

    print "TrigBjetEtHypoToolFromName_j: name = %s, cut_j = %s "%(name,tool.EtThreshold)
    return tool

def TrigBjetEtHypoToolFromName_j( name, conf ):
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName   
    
    decoder = DictFromChainName()        
    decodedDict = decoder.analyseShortName(conf, [], "") # no L1 info        
    decodedDict['chainName'] = name # override
	
    return TrigBjetEtHypoToolFromDict_j( decodedDict )
    

def TrigBjetEtHypoToolFromDict_gsc( chainDict ):
    from AthenaCommon.Constants import DEBUG
    """ set the name of the EtHypoTool (name=chain) and figure out the threshold and selection from conf """

    name    = chainDict['chainName']
    chainPart = chainDict['chainParts'][0]

    
    conf_dict = { 'threshold' : '0',
                  'multiplicity' : '1',
                  'gscThreshold' : '0' if 'gscThreshold' not in chainPart else chainPart['gscThreshold'].replace('gsc',''),
                  'bTag' : 'offperf',
                  'bConfig' : 'EF',
                  'minEta' : chainPart['etaRange'].split('eta')[0],
                  'maxEta' : chainPart['etaRange'].split('eta')[1],
                  'L1' : None }
    
    # chain = conf
    # match = re_Bjet.match( chain )
    # conf_dict = match.groupdict()

    # for k, v in default_conf.items():
    #     if k not in conf_dict: conf_dict[k] = v
    #     if conf_dict[k] == None: conf_dict[k] = v

    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetEtHypoTool        
    tool = TrigBjetEtHypoTool( name )
    tool.OutputLevel = DEBUG
    tool.AcceptAll   = False
    tool.EtThreshold  = float(conf_dict['gscThreshold']) * GeV
    tool.MinEtaThreshold = float(conf_dict['minEta']) / 100
    tool.MaxEtaThreshold = float(conf_dict['maxEta']) / 100

    print "TrigBjetEtHypoToolFromName_gsc: name = %s, cut_j = %s "%(name,tool.EtThreshold)
    return tool

def TrigBjetEtHypoToolFromName_gsc( name, conf ):
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName   
    
    decoder = DictFromChainName()        
    decodedDict = decoder.analyseShortName(conf, [], "") # no L1 info        
    decodedDict['chainName'] = name # override

    return TrigBjetEtHypoToolFromDict_gsc( decodedDict )
    
    
####################################################################################################

if __name__ == "__main__":
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.enableMonitoring=['Validation']

    t = TrigBjetEtHypoToolFromName( "HLT_j35_gsc45_boffperf_split","HLT_j35_gsc45_boffperf_split" )
    assert t, "can't configure gsc boffperf split"

    t = TrigBjetEtHypoToolFromName( "HLT_j35_gsc45_boffperf","HLT_j35_gsc45_boffperf" )
    assert t, "can't configure gsc boffperf"

    t = TrigBjetEtHypoToolFromName( "HLT_j35_boffperf_split","HLT_j35_boffperf_split" )
    assert t, "can't configure boffperf split"

    t = TrigBjetEtHypoToolFromName( "HLT_j35_boffperf","HLT_j35_boffperf" )
    assert t, "can't configure boffperf"

    print ( "\n\n TrigBjetEtHypoToolFromName ALL OK\n\n" )

