# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import re
re_Bjet = re.compile(r'^HLT_(?P<multiplicity>\d+)?j(?P<threshold>\d+)(?:_gsc(?P<gscThreshold>\d+))?(?:_b(?P<bTag>[^_]+)(?:_(?P<bConfig>split))?)?$')

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Constants import VERBOSE,DEBUG

####################################################################################################

def TrigBjetHypoToolFromName( name, conf ):
    from AthenaCommon.Constants import DEBUG
    """ Configure a b-jet hypo tool from chain name. """

    default_conf = { 'threshold' : '0',
                     'multiplicity' : '1',
                     'gscThreshold' : '0',
                     'bTag' : 'offperf',
                     'bConfig' : 'split' }

    chain = conf
    match = re_Bjet.match( chain )
    conf_dict = match.groupdict()

    for k, v in default_conf.items():
        if k not in conf_dict: conf_dict[k] = v
        if conf_dict[k] == None: conf_dict[k] = v

    tool = getBjetHypoConfiguration( name,conf_dict )
    
    print "TrigBjetHypoToolFromName: name = %s, tagger = %s "%(name,tool.MethodTag)
    print "TrigBjetHypoToolFromName: tagger %s and threshold %s "%(tool.MethodTag,tool.BTaggingCut)

    return tool

####################################################################################################  
# Utilities 

def retrieveBtaggingCut( tagger ):
    # MV2c20
    if tagger == "mv2c2040" :
        return 0.75
    if tagger == "mv2c2050" :
        return 0.5
    if tagger == "mv2c2060" :
        return -0.0224729
    if tagger == "mv2c2070" :
        return -0.509032
    if tagger == "mv2c2077" :
        return -0.764668
    if tagger == "mv2c2085" :
        return -0.938441
    # MV2c10
    if tagger == "mv2c1040" :
        return 0.978
    if tagger == "mv2c1050" :
        return 0.948
    if tagger == "mv2c1060" :
        return 0.846
    if tagger == "mv2c1070" :
        return 0.580
    if tagger == "mv2c1077" :
        return 0.162
    if tagger == "mv2c1085" :
        return -0.494
    # MV2c00 (Hybrid MV2c10)
    if tagger == "hmv2c1040" :
        return 0.973
    if tagger == "hmv2c1050" :
        return 0.939
    if tagger == "hmv2c1060" :
        return 0.835
    if tagger == "hmv2c1070" :
        return 0.588
    if tagger == "hmv2c1077" :
        return 0.192
    if tagger == "hmv2c1085" :
        return -0.402
    # If not found
    return -20

def decodeThreshold( threshold_btag ):
    """ decodes the b-tagging thresholds """
    print "TrigBjetHypoToolFromName: decoding threshold b" + threshold_btag

    tagger = "MV2c10"
    if "mv2c20" in threshold_btag :
        tagger = "MV2c20"
    elif "hmv2c10" in threshold_btag : 
        tagger = "MV2c00"

    cut = retrieveBtaggingCut( threshold_btag )
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

