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
  , "dl1d45" : 6.344
  , "dl1d50" : 5.730
  , "dl1d55" : 5.121
  , "dl1d60" : 4.512
  , "dl1d65" : 3.882
  , "dl1d70" : 3.251
  , "dl1d72" : 2.964
  , "dl1d75" : 2.489
  , "dl1d77" : 2.157
  , "dl1d80" : 1.626
  , "dl1d82" : 1.254
  , "dl1d85" : 0.634
  , "dl1d90" : -0.465
  , "dl1d95" : -1.616

  , "offperf" : -999
  }

bbTaggingWP = \
  { "dl1d85bb82" : 0.040
  , "dl1d85bb80" : 0.411
  , "dl1d85bb77" : 0.806
  , "dl1d85bb75" : 1.011
  , "dl1d85bb72" : 1.251
  , "dl1d85bb70" : 1.380
  , "dl1d85bb65" : 1.634
  , "dl1d85bb60" : 1.862
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

    MonTool = None
    nolegname = re.sub("(^leg.*?_)", "", name)
    if 'bJetMon:online' in chainDict['monGroups']:
        MonTool = TrigBjetBtagHypoToolMonitoring(f'TrigBjetOnlineMonitoring/{nolegname}')
    tool = getBjetBtagHypoConfiguration( name,conf_dict, MonTool )

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


    btagger = "DL1d20211216"
    bbtagger = "DL1bb20220331"

    bbcut = bbTaggingWP.get(threshold_btag)

    # remove the bb part to get the b-only cut
    threshold_btag = threshold_btag.split("bb", maxsplit=1)[0]

    # possibly roll back to dl1r for some chains
    if "dl1r" in threshold_btag:
        btagger = "DL1r"

    bcut = bTaggingWP[threshold_btag]

    return [btagger, bcut] , [bbtagger, bbcut]

####################################################################################################

def getBjetBtagHypoConfiguration( name,conf_dict, MonTool ):

    from TrigBjetHypo.TrigBjetHypoConf import TrigBjetBtagHypoTool
    from TrigBjetHypo.TrigBjetHypoConf import BJetThreeValueCheck
    from TrigBjetHypo.TrigBjetHypoConf import BJetTwoValueCheck

    tool = TrigBjetBtagHypoTool( name )
    if MonTool is not None:
        tool.MonTool = MonTool

    [btagger, bcut] , [bbtagger, bbcut] = decodeThreshold( conf_dict['bTag'] )
    tool.monitoredFloats = {f'{btagger}_p{x}':f'btag_p{x}' for x in 'cub'}

    if conf_dict['bTag'] == "offperf":
        # we shoudln't be worried about rates blowing up due to bad
        # b-tagging in the boffperf chains
        tool.vetoBadBeamspot = False
        return tool

    btagTool = BJetThreeValueCheck(
        f'{name}_btag',
        b=f'{btagger}_pb',
        c=f'{btagger}_pc',
        u=f'{btagger}_pu',
        cFraction=0.018,
        threshold=bcut)
    if MonTool is not None:
        btagTool.MonTool = MonTool
    tool.checks.append(btagTool)

    if bbcut is not None:
        bbTool = BJetTwoValueCheck(
            f'{name}_bbtag',
            numerator=f'{bbtagger}_pb',
            denominator=f'{bbtagger}_pbb',
            threshold=bbcut)
        if MonTool is not None:
            bbTool.MonTool = MonTool
        tool.checks.append(bbTool)
        tool.monitoredFloats |= {
            f'{bbtagger}_p{x}':f'bbtag_p{x}' for x in ['b','bb']}

    return tool
