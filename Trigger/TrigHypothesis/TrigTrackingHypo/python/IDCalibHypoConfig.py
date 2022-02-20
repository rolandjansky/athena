# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def createIDCalibHypoAlg(name):
    # make the Hypo
    from TrigTrackingHypo.TrigTrackingHypoConf import (IDCalibHypoAlg)

    # Setup the hypothesis algorithm
    theHypo = IDCalibHypoAlg(name)
    
    # monioring
    monTool = GenericMonitoringTool("IM_MonTool"+name)
    monTool.defineHistogram('pt', type='TH1F', path='EXPERT', title="p_{T};p_{T} [GeV];Nevents", xbins=50, xmin=0, xmax=100) 
    #
    monTool.HistPath = 'IDCalibHypoAlg'
    theHypo.MonTool = monTool

    return theHypo


def IDCalibHypoToolFromDict( chainDict ):

    log = logging.getLogger('IDCalibHypoTool')

    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='Calib']

    name = "default_chain_name"
    if 'chainName' in chainDict:
        name = chainDict['chainName']
    else:
        log.error("chainName not in chain dictionary")

    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.IDCalibHypoTool(name)

    # set thresholds
    import re
    m = re.search(r'trk(\d+)',[ cpart['hypo'] for cpart in cparts ][0])
    threshold = m.group(1)

    mult = [ cpart['multiplicity'] for cpart in cparts ][0]
    thresholds = [ float(threshold) for x in range(0,int(mult)) ]
    strThr = ""
    for THR in thresholds:
        strThr += str(THR)+", "
        
    log.info("Threshold Values are: %s",strThr)

    tool.cutTrackPtGeV = thresholds

    return tool


def IDCalibHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool given the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    from TriggerMenuMT.HLT.Config.Utility.DictFromChainName import dictFromChainName
    
    decodedDict = dictFromChainName(conf)
    
    return IDCalibHypoToolFromDict( decodedDict )
