# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def createTrigDisappearingTrackHypoAlg(name):
    # make the Hypo
    from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf import (TrigDisappearingTrackHypoAlg)

    # Setup the hypothesis algorithm
    theDisTrkHypo = TrigDisappearingTrackHypoAlg(name)
    
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    theDisTrkHypo.DisTrkBDTSel = recordable("HLT_DisTrkBDTSel")

    # monioring
    monTool = GenericMonitoringTool("IM_MonTool"+name)
    monTool.defineHistogram('category',            type='TH1F', path='EXPERT', title="DisTrk Category;DisTrk Category;Nevents", xbins=5, xmin=-0.5, xmax=4.5) 
    #
    monTool.defineHistogram('pix4_sct0_pt',        type='TH1F', path='EXPERT', title="Pix4Sct0 p_{T};p_{T} [GeV];Nevents", xbins=50, xmin=0, xmax=100) 
    monTool.defineHistogram('pix4_sct0_z0',        type='TH1F', path='EXPERT', title="Pix4Sct0 z_{0} (wrt pVtx);z_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-50, xmax=50) 
    monTool.defineHistogram('pix4_sct0_d0',        type='TH1F', path='EXPERT', title="Pix4Sct0 d_{0} (wrt pVtx);d_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-10, xmax=10) 
    monTool.defineHistogram('pix4_sct0_iso01',     type='TH1F', path='EXPERT', title="Pix4Sct0 Trk Iso (#DeltaR < 0.1) [GeV];Trk Iso (#DeltaR < 0.1) [GeV];Nevents", xbins=20, xmin=0, xmax=20) 
    monTool.defineHistogram('pix4_sct0_iso0201',   type='TH1F', path='EXPERT', title="Pix4Sct0 Trk Iso (0.1 < #DeltaR < 0.2) [GeV];Trk Iso (0.1 < #DeltaR < 0.2) [GeV];Nevents", xbins=20, xmin=0, xmax=20) 
    monTool.defineHistogram('pix4_sct0_chi2ndof',  type='TH1F', path='EXPERT', title="Pix4Sct0 #chi^{2}/ndof;#chi^{2}/ndof;Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix4_sct0_chi2ndof_pix',  type='TH1F', path='EXPERT', title="Pix4Sct0 #chi^{2}/ndof (Pixel);#chi^{2}/ndof (Pixel);Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix4_sct0_refit_pt',  type='TH1F', path='EXPERT', title="Pix4Sct0 refit p_{T} [GeV];refit p_{T} [GeV];Nevents", xbins=50, xmin=0, xmax=100) 
    monTool.defineHistogram('pix4_sct0_n_pix',     type='TH1I', path='EXPERT', title="Pix4Sct0 N Pixel hits;N Pixel hits;Nevents", xbins=10, xmin=0, xmax=10) 
    monTool.defineHistogram('pix4_sct0_refit_ptratio', type='TH1F', path='EXPERT', title="Pix4Sct0 refit p_{T} / p_{T};refit p_{T} / p_{T};Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix4_sct0_refit_chi2ndof',type='TH1F', path='EXPERT', title="Pix4Sct0 refit #chi^{2}/ndof;refit #chi^{2}/ndof;Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix4_sct0_n_bl',      type='TH1I', path='EXPERT', title="Pix4Sct0 N B-layer hits;N B-layer hits;Nevents", xbins=10, xmin=0, xmax=10) 
    monTool.defineHistogram('pix4_sct0_bdtscore',  type='TH1F', path='EXPERT', title="Pix4Sct0 BDT score;BDT score;Nevents",  xbins=50, xmin=-1, xmax=1) 
    #
    monTool.defineHistogram('pix4_sct1p_pt',        type='TH1F', path='EXPERT', title="Pix4Sct1p p_{T};p_{T} [GeV];Nevents", xbins=50, xmin=0, xmax=100) 
    monTool.defineHistogram('pix4_sct1p_refit_pt',  type='TH1F', path='EXPERT', title="Pix4Sct1p refit p_{T};refit p_{T} [GeV];Nevents", xbins=50, xmin=0, xmax=100) 
    monTool.defineHistogram('pix4_sct1p_refit_z0',  type='TH1F', path='EXPERT', title="Pix4Sct1p refit z_{0} (wrt pVtx);z_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-50, xmax=50) 
    monTool.defineHistogram('pix4_sct1p_refit_d0',  type='TH1F', path='EXPERT', title="Pix4Sct1p refit d_{0} (wrt pVtx);d_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-10, xmax=10) 
    monTool.defineHistogram('pix4_sct1p_n_sct',     type='TH1I', path='EXPERT', title="Pix4Sct1p N SCT hits;N SCT hits;Nevents", xbins=20, xmin=0, xmax=20) 
    monTool.defineHistogram('pix4_sct1p_refit_ptratio', type='TH1F', path='EXPERT', title="Pix4Sct1p refit p_{T} / p_{T};refit p_{T} / p_{T};Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix4_sct1p_refit_chi2ndof_ratio',type='TH1F', path='EXPERT', title="Pix4Sct1p refit #chi^{2}/ndof;refit #chi^{2}/ndof;Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix4_sct1p_iso01',     type='TH1F', path='EXPERT', title="Pix4Sct1p Trk Iso (#DeltaR < 0.1) [GeV];Trk Iso (#DeltaR < 0.1) [GeV];Nevents", xbins=20, xmin=0, xmax=20) 
    monTool.defineHistogram('pix4_sct1p_iso0201',   type='TH1F', path='EXPERT', title="Pix4Sct1p Trk Iso (0.1 < #DeltaR < 0.2) [GeV];Trk Iso (0.1 < #DeltaR < 0.2) [GeV];Nevents", xbins=20, xmin=0, xmax=20) 
    monTool.defineHistogram('pix4_sct1p_chi2ndof_pix',  type='TH1F', path='EXPERT', title="Pix4Sct1p #chi^{2}/ndof (Pixel);#chi^{2}/ndof (Pixel);Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix4_sct1p_n_pix',     type='TH1I', path='EXPERT', title="Pix4Sct1p N Pixel hits;N Pixel hits;Nevents", xbins=10, xmin=0, xmax=10) 
    monTool.defineHistogram('pix4_sct1p_bdtscore', type='TH1F', path='EXPERT', title="Pix4Sct1p BDT score;BDT score;Nevents", xbins=50, xmin=-1, xmax=1) 
    #
    monTool.defineHistogram('pix3_sct0_pt',        type='TH1F', path='EXPERT', title="Pix3Sct0 p_{T} [GeV];p_{T} [GeV];Nevents", xbins=50, xmin=0, xmax=100) 
    monTool.defineHistogram('pix3_sct0_z0',        type='TH1F', path='EXPERT', title="Pix3Sct0 z_{0} (wrt pVtx) [mm];z_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-50, xmax=50) 
    monTool.defineHistogram('pix3_sct0_d0',        type='TH1F', path='EXPERT', title="Pix3Sct0 d_{0} (wrt pVtx) [mm];d_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-10, xmax=10) 
    monTool.defineHistogram('pix3_sct0_chi2ndof',  type='TH1F', path='EXPERT', title="Pix3Sct0 #chi^{2}/ndof;#chi^{2}/ndof;Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix3_sct0_chi2ndof_pix',  type='TH1F', path='EXPERT', title="Pix3Sct0 #chi^{2}/ndof (Pixel);#chi^{2}/ndof (Pixel);Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix3_sct0_iso01',     type='TH1F', path='EXPERT', title="Pix3Sct0 Trk Iso (#DeltaR < 0.1) [GeV];Trk Iso (#DeltaR < 0.1) [GeV];Nevents", xbins=20, xmin=0, xmax=20) 
    monTool.defineHistogram('pix3_sct0_iso0201',   type='TH1F', path='EXPERT', title="Pix3Sct0 Trk Iso (0.1 < #DeltaR < 0.2) [GeV];Trk Iso (0.1 < #DeltaR < 0.2) [GeV];Nevents", xbins=20, xmin=0, xmax=20) 
    monTool.defineHistogram('pix3_sct0_refit_pt',  type='TH1F', path='EXPERT', title="Pix3Sct0 refit p_{T} [GeV];refit p_{T} [GeV];Nevents", xbins=50, xmin=0, xmax=100) 
    monTool.defineHistogram('pix3_sct0_refit_z0',  type='TH1F', path='EXPERT', title="Pix3Sct0 refit z_{0} (wrt pVtx) [mm];refit z_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-50, xmax=50) 
    monTool.defineHistogram('pix3_sct0_refit_d0',  type='TH1F', path='EXPERT', title="Pix3Sct0 refit d_{0} (wrt pVtx) [mm];refit d_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-10, xmax=10) 
    monTool.defineHistogram('pix3_sct0_n_pix',     type='TH1I', path='EXPERT', title="Pix3Sct0 N Pixel hits;N Pixel hits;Nevents", xbins=10, xmin=0, xmax=10) 
    monTool.defineHistogram('pix3_sct0_n_bl',      type='TH1I', path='EXPERT', title="Pix3Sct0 N B-layer hits;N B-layer hits;Nevents", xbins=4, xmin=0, xmax=4) 
    monTool.defineHistogram('pix3_sct0_bdtscore',  type='TH1F', path='EXPERT', title="Pix3lSct0 BDT score;BDT score;Nevents",  xbins=50, xmin=-1, xmax=1) 
    #
    monTool.defineHistogram('pix3_sct1p_pt',        type='TH1F', path='EXPERT', title="Pix3Sct1p p_{T} [GeV];p_{T} [GeV];Nevents", xbins=50, xmin=0, xmax=100) 
    monTool.defineHistogram('pix3_sct1p_z0',        type='TH1F', path='EXPERT', title="Pix3Sct1p z_{0} (wrt pVtx) [mm];z_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-50, xmax=50) 
    monTool.defineHistogram('pix3_sct1p_d0',        type='TH1F', path='EXPERT', title="Pix3Sct1p d_{0} (wrt pVtx) [mm];d_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-10, xmax=10) 
    monTool.defineHistogram('pix3_sct1p_refit_pt',  type='TH1F', path='EXPERT', title="Pix3Sct1p refit p_{T} [GeV];refit p_{T} [GeV];Nevents", xbins=50, xmin=0, xmax=100) 
    monTool.defineHistogram('pix3_sct1p_refit_z0',  type='TH1F', path='EXPERT', title="Pix3Sct1p refit z_{0} (wrt pVtx) [mm];refit z_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-50, xmax=50) 
    monTool.defineHistogram('pix3_sct1p_refit_d0',  type='TH1F', path='EXPERT', title="Pix3Sct1p refit d_{0} (wrt pVtx) [mm];refit d_{0} (wrt pVtx) [mm];Nevents", xbins=25, xmin=-10, xmax=10) 
    monTool.defineHistogram('pix3_sct1p_n_pix',     type='TH1I', path='EXPERT', title="Pix3Sct1p N Pixel hits;N Pixel hits;Nevents", xbins=10, xmin=0, xmax=10) 
    monTool.defineHistogram('pix3_sct1p_n_sct',     type='TH1I', path='EXPERT', title="Pix3Sct1p N SCT hits;N SCT hits;Nevents", xbins=10, xmin=0, xmax=10) 
    monTool.defineHistogram('pix3_sct1p_n_bl',      type='TH1I', path='EXPERT', title="Pix3Sct1p N B-layer hits;N B-layer hits;Nevents", xbins=4, xmin=0, xmax=4) 
    monTool.defineHistogram('pix3_sct1p_chi2ndof',  type='TH1F', path='EXPERT', title="Pix3Sct1p #chi^{2}/ndof;#chi^{2}/ndof;Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix3_sct1p_iso01',     type='TH1F', path='EXPERT', title="Pix3Sct1p Trk Iso (#DeltaR < 0.1) [GeV];Trk Iso (#DeltaR < 0.1) [GeV];Nevents", xbins=20, xmin=0, xmax=20) 
    monTool.defineHistogram('pix3_sct1p_iso0201',   type='TH1F', path='EXPERT', title="Pix3Sct1p Trk Iso (0.1 < #DeltaR < 0.2) [GeV];Trk Iso (0.1 < #DeltaR < 0.2) [GeV];Nevents", xbins=20, xmin=0, xmax=20) 
    monTool.defineHistogram('pix3_sct1p_refit_ptratio', type='TH1F', path='EXPERT', title="Pix3Sct1p refit p_{T} / p_{T};refit p_{T} / p_{T};Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix3_sct1p_refit_chi2ndof',type='TH1F', path='EXPERT', title="Pix3Sct1p refit #chi^{2}/ndof;refit #chi^{2}/ndof;Nevents", xbins=50, xmin=0, xmax=10) 
    monTool.defineHistogram('pix3_sct1p_bdtscore', type='TH1F', path='EXPERT', title="Pix3lSct1p BDT score;BDT score;Nevents", xbins=50, xmin=-1, xmax=1) 
    #
    monTool.HistPath = 'disappearingTrackHypoAlg'
    theDisTrkHypo.MonTool = monTool

    return theDisTrkHypo


def TrigDisappearingTrackHypoToolFromDict( chainDict ):

    log = logging.getLogger('TrigDisappearingTrackHypoTool')

    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='UnconventionalTracking']

    name = chainDict['chainName']
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.TrigDisappearingTrackHypoTool(name)

    # set thresholds
    thresholds = sum([ [cpart['threshold']]*int(cpart['multiplicity']) for cpart in cparts], [])
    thresholds = [ float(THR) for THR in thresholds ]
    strThr = ""
    for THR in thresholds:
        strThr += str(THR)+", "
        
    log.info("Threshold Values are: %s",strThr)

    tool.cutTrackPtGeV = thresholds

    for cpart in cparts:
        if cpart['IDinfo'] =="loose":
            bdt4l0  = [-0.105] * len(thresholds)
            bdt4l1p = [-0.085] * len(thresholds)
            bdt3l0  = [ 0.045] * len(thresholds)
            bdt3l1p = [ 0.145] * len(thresholds)
            log.info("Loose BDT WP is set")
        elif cpart['IDinfo'] =="tight":
            bdt4l0  = [-0.100] * len(thresholds)
            bdt4l1p = [ 0.040] * len(thresholds)
            bdt3l0  = [ 0.040] * len(thresholds)
            bdt3l1p = [ 0.155] * len(thresholds)
            log.info("Tight BDT WP is set")
        else:
            bdt4l0  = [-0.105] * len(thresholds)
            bdt4l1p = [-0.085] * len(thresholds)
            bdt3l0  = [ 0.045] * len(thresholds)
            bdt3l1p = [ 0.145] * len(thresholds)
            if cpart['IDinfo'] =="medium":
                log.info("Medium BDT WP is set")
            else: 
                log.info("IDinfo not provided, setting medium BDT WP")

    tool.cutBDTPix4Sct0  = bdt4l0
    tool.cutBDTPix4Sct1p = bdt4l1p
    tool.cutBDTPix3Sct0  = bdt3l0
    tool.cutBDTPix3Sct1p = bdt3l1p

    return tool


def TrigDisappearingTrackHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool given the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    from TriggerMenuMT.HLT.Menu.DictFromChainName import dictFromChainName
    
    decodedDict = dictFromChainName(conf)
    
    return TrigDisappearingTrackHypoToolFromDict( decodedDict )
