# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import re
re_Bjet = re.compile(r'^HLT_(?P<multiplicity>\d+)?j(?P<threshold>\d+)(?:_gsc(?P<gscThreshold>\d+))?(?:_b(?P<bTag>[^_]+)(?:_(?P<bConfig>split))?(?:_(?P<minEta>\d+)eta(?P<maxEta>\d+))?)?(?:_L1(?P<L1>.*))?$')

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Constants import VERBOSE,DEBUG

####################################################################################################

bTaggingWP = { 
    # MV2c20
    'mv2c2040' : 0.75,
    'mv2c2050' : 0.5,
    'mv2c2060' : -0.0224729,
    'mv2c2070' : -0.509032,
    'mv2c2077' : -0.764668,
    'mv2c2085' : -0.938441,
    # MV2c10
    'mv2c1040' : 0.978,
    'mv2c1050' : 0.948,
    'mv2c1060' : 0.846,
    'mv2c1070' : 0.580,
    'mv2c1077' : 0.162,
    'mv2c1085' : -0.494,
    # MV2c00 (Hybrid MV2c10)
    'hmv2c1040' : 0.973,
    'hmv2c1050' : 0.939,
    'hmv2c1060' : 0.835,
    'hmv2c1070' : 0.588,
    'hmv2c1077' : 0.192,
    'hmv2c1085' : -0.402,
    }

####################################################################################################  
def TrigBjetHypoToolFromDict( chainDict ):
    print chainDict
    chainPart = chainDict['chainParts'][0]
    conf_dict = { 'threshold'    : chainPart['threshold'],
                  'multiplicity' : '1' if len(chainPart['multiplicity']) == 0 else chainPart['multiplicity'],
                  'gscThreshold' : '0' if 'gscThreshold' not in chainPart else chainPart['gscThreshold'].replace('gsc',''),
                  'bTag' :         chainPart['bTag'][1:],
                  'bConfig' :      'EF' if len(chainPart['bConfig']) == 0 else chainPart['bConfig'][0],
                  'minEta' :       chainPart['etaRange'].split('eta')[0],
                  'maxEta' :       chainPart['etaRange'].split('eta')[1]}
    name = chainDict['chainName']
    # TODO the chain dict can be probably passed over down the line here
    tool = getBjetHypoConfiguration( name,conf_dict )
    
    print "TrigBjetHypoToolFromName: name = %s, tagger = %s "%(name,tool.MethodTag)
    print "TrigBjetHypoToolFromName: tagger %s and threshold %s "%(tool.MethodTag,tool.BTaggingCut)

    return tool

def TrigBjetHypoToolFromName( name, conf ):
    from AthenaCommon.Constants import DEBUG
    """ Configure a b-jet hypo tool from chain name. """
    
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName   
    
    decoder = DictFromChainName()        
    decodedDict = decoder.analyseShortName(conf, [], "") # no L1 info        
    decodedDict['chainName'] = name # override
	
    return TrigBjetHypoToolFromDict( decodedDict )


####################################################################################################  

def decodeThreshold( threshold_btag ):
    """ decodes the b-tagging thresholds """
    print "TrigBjetHypoToolFromName: decoding threshold b" + threshold_btag

    tagger = "MV2c10"
    if "mv2c20" in threshold_btag :
        tagger = "MV2c20"
    elif "hmv2c10" in threshold_btag : 
        tagger = "MV2c00"

    cut = bTaggingWP.get( threshold_btag,-20 )
    return [tagger,cut]

####################################################################################################

def getBjetHypoConfiguration( name,conf_dict ):
    # Common for both split and non-split configurations
    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetHypoTool

    tool = TrigBjetHypoTool( name )
    tool.OutputLevel     = DEBUG
    tool.AcceptAll       = False
    tool.UseBeamSpotFlag = False

    # b-tagging
    [tagger,tb] = decodeThreshold( conf_dict['bTag'] )

    if conf_dict['bTag'] == "offperf" :
        tool.AcceptAll             = True
        tool.OverrideBeamSpotValid = True

    tool.MethodTag = tagger
    tool.BTaggingCut = tb

    # Monitoring
    tool.MonTool = ""
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if 'Validation' in TriggerFlags.enableMonitoring() or 'Online' in  TriggerFlags.enableMonitoring():
        from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
        monTool = GenericMonitoringTool("MonTool"+name)
        monTool.Histograms = []

        monTool.HistPath = 'BjetHypo/'+tool.name()
        tool.MonTool = monTool
        tool += monTool

    return tool

####################################################################################################

if __name__ == "__main__":
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.enableMonitoring=['Validation']

    t = TrigBjetHypoToolFromName( "HLT_j35_gsc45_boffperf_split","HLT_j35_gsc45_boffperf_split" )
    assert t, "can't configure gsc boffperf split"

    t = TrigBjetHypoToolFromName( "HLT_j35_gsc45_boffperf","HLT_j35_gsc45_boffperf" )
    assert t, "can't configure gsc boffperf"

    t = TrigBjetHypoToolFromName( "HLT_j35_boffperf_split","HLT_j35_boffperf_split" )
    assert t, "can't configure boffperf split"

    t = TrigBjetHypoToolFromName( "HLT_j35_boffperf","HLT_j35_boffperf" )
    assert t, "can't configure boffperf"

    print ( "\n\n TrigBjetHypoToolFromName ALL OK\n\n" )

