# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

log = logging.getLogger('TrigBjetBtagHypoTool')

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
    # DL1r (Place Holder while we wait for WPs to be defined)
    # Values taken from https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarksRelease21#DL1rnn_tagger
    'dl1r60' : 4.31,
    'dl1r70' : 2.98,
    'dl1r77' : 2.23,
    'dl1r85' : 1.32,
    }

####################################################################################################  
def TrigBjetBtagHypoToolFromDict( chainDict ):

    chainPart = chainDict['chainParts'][0]
    conf_dict = { 'threshold'    : chainPart['threshold'],
                  'multiplicity' : '1' if len(chainPart['multiplicity']) == 0 else chainPart['multiplicity'],
                  'bTag' :         chainPart['bTag'][1:],
                  'bConfig' :      'EF' if len(chainPart['bConfig']) == 0 else chainPart['bConfig'][0],
                  'minEta' :       chainPart['etaRange'].split('eta')[0],
                  'maxEta' :       chainPart['etaRange'].split('eta')[1]}
    name = chainDict['chainName']
    # TODO the chain dict can be probably passed over down the line here
    tool = getBjetBtagHypoConfiguration( name,conf_dict )
    
    log.debug("name = %s, tagger = %s, threshold = %s ", name, tool.MethodTag, tool.BTaggingCut)

    return tool

def TrigBjetBtagHypoToolFromName( name, conf ):
    """ Configure a b-jet hypo tool from chain name. """
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName( conf )
    decodedDict['chainName'] = name # override
    
    return TrigBjetBtagHypoToolFromDict( decodedDict )


####################################################################################################  

def decodeThreshold( threshold_btag ):
    """ decodes the b-tagging thresholds """

    log.debug("TrigBjetBtagHypoToolFromName: decoding threshold b%s", threshold_btag)

    import re
    tagger = "offperf" if threshold_btag == "offperf" else re.findall("(.*)[0-9]{2}",threshold_btag)[0]

    allowedTaggers = ["offperf","hmv2c10","mv2c10","mv2c20","dl1r"]
    if tagger not in allowedTaggers:
        log.debug("tagger = %s not amidst allowed taggers ",threshold_btag)
        assert False, "Can't recognize tagger during TrigBjetHypoTool configuration. Tagger = "+threshold_btag
        return None

    tagger = "MV2c10"
    if"mv2c20" in threshold_btag :
        tagger = "MV2c20"
    elif "hmv2c10" in threshold_btag : 
        tagger = "MV2c00"
    elif "dl1r" in threshold_btag : 
        tagger = "DL1r"

    cut = bTaggingWP.get( threshold_btag,-20 )
    return [tagger,cut]

####################################################################################################

def getBjetBtagHypoConfiguration( name,conf_dict ):

    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetBtagHypoTool
    tool = TrigBjetBtagHypoTool( name )

    # b-tagging
    [tagger,tb] = decodeThreshold( conf_dict['bTag'] )

    if conf_dict['bTag'] == "offperf" :
        tool.AcceptAll = True

    tool.MethodTag = tagger
    tool.BTaggingCut = tb
    tool.cFraction = 0.03

    return tool

####################################################################################################

if __name__ == "__main__":
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.enableMonitoring=['Validation']

    t = TrigBjetBtagHypoToolFromName( "HLT_j45_ftf_subjesgscIS_boffperf_split_L1J15","HLT_j45_ftf_subjesgscIS_boffperf_split_L1J15" )
    assert t, "can't configure boffperf chain"

    t = TrigBjetBtagHypoToolFromName( "HLT_j45_ftf_subjesgscIS_bmv2c2070_split_L1J15","HLT_j45_ftf_subjesgscIS_bmv2c2070_split_L1J15" )
    assert t, "can't configure bmv2c20 chain"

    t = TrigBjetBtagHypoToolFromName( "HLT_j45_ftf_subjesgscIS_bmv2c1070_split_L1J15","HLT_j45_ftf_subjesgscIS_bmv2c1070_split_L1J15" )
    assert t, "can't configure bmv2c10 chain"

    t = TrigBjetBtagHypoToolFromName( "HLT_j45_ftf_subjesgscIS_bhmv2c1070_split_L1J15","HLT_j45_ftf_subjesgscIS_bhmv2c1070_split_L1J15" )
    assert t, "can't configure bhmv2c10 chain"


    log.info( "\n\n TrigBjetBtagHypoToolFromName ALL OK\n\n" )
