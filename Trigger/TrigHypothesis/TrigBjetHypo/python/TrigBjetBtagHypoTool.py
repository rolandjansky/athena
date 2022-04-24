# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import re
from TrigBjetHypo.TrigBjetMonitoringConfig import TrigBjetBtagHypoToolMonitoring

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigBjetBtagHypoTool')

####################################################################################################

# DL1r (Place Holder while we wait for WPs to be defined)
# Values taken from https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarksRelease21#DL1rnn_tagger
bTaggingWP = \
  { 'dl1r60' : 4.31
  , 'dl1r70' : 2.98
  , 'dl1r77' : 2.23
  , 'dl1r85' : 1.32

  , "dl1d40" : 6.957
  , "dl1d50" : 5.730
  , "dl1d60" : 4.512
  , "dl1d70" : 3.251
  , "dl1d75" : 2.489
  , "dl1d77" : 2.157
  , "dl1d80" : 1.626
  , "dl1d85" : 0.634
  , "dl1d90" : -0.465
  , "dl1d95" : -1.616

  , "dl1d85bb77" : 0.806
  , "dl1d85bb70" : 1.380
  , "dl1d85bb65" : 1.634
  , "dl1d85bb60" : 1.862

  , "offperf" : -999
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
    
    log.debug("name = %s, tagger = %s, threshold = %s ", name, tool.Tagger, tool.BTaggingCut)

    nolegname = re.sub("(^leg.*?_)", "", name)
    if 'bJetMon:online' in chainDict['monGroups']:
        tool.MonTool = TrigBjetBtagHypoToolMonitoring(f'TrigBjetOnlineMonitoring/{nolegname}')

    return tool


####################################################################################################  

def decodeThreshold( threshold_btag ):
    """ decodes the b-tagging thresholds """

    log.debug("decoded b-jet threshold: b%s", threshold_btag)

    tagger = "offperf" if threshold_btag == "offperf" else re.findall("(.*)[0-9]{2}",threshold_btag)[0]

    allowedTaggers = ["offperf", "dl1r", "dl1d", "dl1d85bb"]
    if tagger not in allowedTaggers:
        log.debug("tagger = %s not amidst allowed taggers ",threshold_btag)
        assert False, "Can't recognize tagger during TrigBjetHypoTool configuration. Tagger = "+threshold_btag
        return None


    bbcut = None
    bbtagger = None
    btagger = "DL1d20211216"

    if "bb" in threshold_btag:
        bbcut = bTaggingWP[threshold_btag]
        bbtagger = "DL1bb20220331"

        # remove the bb part to get the bcut
        threshold_btag, _ = threshold_btag.split("bb", maxsplit=1)

    if "dl1r" in threshold_btag : 
        btagger = "DL1r"

    bcut = bTaggingWP[threshold_btag]

    return [btagger, bcut] , [bbtagger, bbcut]

####################################################################################################

def getBjetBtagHypoConfiguration( name,conf_dict ):

    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetBtagHypoTool
    tool = TrigBjetBtagHypoTool( name )

    # b-tagging
    [btagger, bcut] , [bbtagger, bbcut] = decodeThreshold( conf_dict['bTag'] )

    if conf_dict['bTag'] == "offperf" :
        tool.AcceptAll = True

    tool.Tagger = btagger
    tool.BBTagger = bbtagger
    tool.BTaggingCut = bcut
    tool.BBTaggingCut = bbcut
    tool.cFraction = 0.018

    return tool
