# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def createTrigdEdxTrackHypoAlg(name):

    # make the Hypo
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import TrigdEdxTrackHypoAlg
    
    # Setup the hypothesis algorithm
    thedEdxTrackHypo = TrigdEdxTrackHypoAlg(name)
    
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    thedEdxTrackHypo.HPtdEdxTrk = recordable("HLT_HPtdEdxTrk")

    # monioring

    monTool = GenericMonitoringTool("IM_MonTool"+name)
    monTool.defineHistogram('trackPtGeV', type='TH1F', path='EXPERT', title="Hypo p_{T}^{track};p_{T}^{track} [GeV];Nevents", xbins=50, xmin=0, xmax=100) 
    monTool.defineHistogram('trackEta',   type='TH1F', path='EXPERT', title="Hypo p_{T}^{track} (after p_{T} cut);eta;Nevents", xbins=60, xmin=-3.0, xmax=3.0) 
    monTool.defineHistogram('tracka0beam',type='TH1F', path='EXPERT', title="Hypo a0beam (after eta cut);a0beam [mm];Nevents", xbins=50, xmin=-5.0, xmax=5.0) 
    monTool.defineHistogram('trackdEdx',  type='TH1F', path='EXPERT', title="Hypo dE/dx (after a0beam cut);dE/dx;Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('trackNhighdEdxHits', type='TH1F', path='EXPERT', title="Hypo Nr high dE/dx hits (after dEdx cut);N high dE/dx hits;Nevents", xbins=10, xmin=0, xmax=10) 

    monTool.HistPath = 'dEdxTrackHypoAlg'
    thedEdxTrackHypo.MonTool = monTool

    return thedEdxTrackHypo


def TrigdEdxTrackHypoToolFromDict( chainDict ):

    log = logging.getLogger('TrigdEdxTrackHypoTool')

    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='UnconventionalTracking']
    thresholds = sum([ [cpart['threshold']]*int(cpart['multiplicity']) for cpart in cparts], [])

    name = chainDict['chainName']
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.TrigdEdxTrackHypoTool(name)

    # set thresholds

    strThr = ""

    thresholds = [ float(THR) for THR in thresholds]
    
    for THR in thresholds:
        strThr += str(THR)+", "
        
    log.info("Threshold Values are: %s",strThr)

    tool.cutTrackPtGeV = thresholds

    trackEta=[]
    trackdEdx=[]
    tracka0beam=[]
    trackNhighdEdxHits=[]
    trackHighdEdxDef=[]

    for cpart in cparts:
        if cpart['IDinfo'] =="loose":
            log.info("UTT: Loose ID working point is set")
            trackEta.append(2.5)
            trackdEdx.append(1.5)
            tracka0beam.append(5.0)
            trackNhighdEdxHits.append(1)
            trackHighdEdxDef.append("1p50")
        elif cpart['IDinfo'] =="tight":
            log.info("UTT: Tight ID working point is set")
            trackEta.append(2.5)
            trackdEdx.append(1.8)
            tracka0beam.append(1.5)
            trackNhighdEdxHits.append(2)
            trackHighdEdxDef.append("1p80")
        else:
            log.info("UTT: Medium ID working point is set")
            trackEta.append(2.5)
            trackdEdx.append(1.7)
            tracka0beam.append(2.5)
            trackNhighdEdxHits.append(2)
            trackHighdEdxDef.append("1p70")

    tool.cutTrackEta    = trackEta
    tool.cutTrackdEdx   = trackdEdx
    tool.cutTracka0beam = tracka0beam
    tool.cutTrackNhighdEdxHits = trackNhighdEdxHits
    tool.cutTrackHighdEdxDef   = trackHighdEdxDef

    return tool


def TrigdEdxTrackHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool given the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    from TriggerMenuMT.HLT.Menu.DictFromChainName import dictFromChainName
    
    decodedDict = dictFromChainName(conf)
    
    return TrigdEdxTrackHypoToolFromDict( decodedDict )
