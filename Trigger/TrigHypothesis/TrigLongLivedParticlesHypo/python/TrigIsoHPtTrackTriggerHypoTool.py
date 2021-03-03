# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Logging import logging
# from AthenaCommon.Include import Include 
log = logging.getLogger("TrigLongLivedParticlesHypo.TrigIsoHPtTrackTriggerHypoTool")
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool



log = logging.getLogger('TrigIsoHPtTrackTriggerHypoTool')

def TrigIsoHPtTrackTriggerHypoToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='UnconventionalTracking']
    
    thresholds = sum([ [cpart['threshold']]*int(cpart['multiplicity']) for cpart in cparts], [])

    name = chainDict['chainName']
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.TrigIsoHPtTrackTriggerHypoTool(name)

    monTool = GenericMonitoringTool("IM_MonTool"+name)
    monTool.defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="IsoHPtTrackTrigger Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative") 
    monTool.defineHistogram('trackPt', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=30, xmin=0, xmax=300000) 
    monTool.defineHistogram('trackd0', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo d0^{track} ; d0^{track} ;Nevents", xbins=100, xmin=0, xmax=10)
    monTool.defineHistogram('trackNPixHits', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo N. Pix Hits^{track} ; N. Pix Hits^{track} ;Nevents", xbins=10, xmin=0, xmax=10) 

    monTool.defineHistogram('trackNSCTHits', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo N. SCT Hits^{track} ; N. SCT Hits^{track} ;Nevents", xbins=20, xmin=0, xmax=20) 
    monTool.defineHistogram('trackd0Sig', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo d0 Sig^{track} ; d0 Sig^{track} ;Nevents", xbins=60, xmin=0, xmax=6)
    monTool.defineHistogram('trackEta', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo Eta^{track} ; Eta^{track} ;Nevents", xbins=30, xmin=-3, xmax=3) 
    monTool.defineHistogram('trackIsoPt', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo Iso p_{T}^{track} [MeV]; Isolation Cone p_{T}^{track} [MeV];Nevents", xbins=100, xmin=0, xmax=100000) 
    monTool.defineHistogram('trackAggrIsoPt', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo Iso p_{T}^{track} [MeV]; Isolation Cumalitive Cone p_{T}^{track} [MeV];Nevents", xbins=100, xmin=0, xmax=100000) 



    monTool.HistPath = 'IsoHPtTrackTriggerHypoAlgMT/'+tool.getName()
    tool.MonTool = monTool

    strThr = ""

    thresholds = [ float(THR)*GeV for THR in thresholds]
    
    for THR in thresholds:
        strThr += str(THR)+", "
        
    log.debug("Threshold Values are: %s",strThr)

    nt = len( thresholds )
    tool.MinTrackPt = thresholds
    tool.MinTrackNPixHits = [ 2 ] *nt
    tool.MinTrackNSCTHits = [ 5 ] *nt

    EnIso=[]
    IsoCum=[]
    IsoCone=[]
    IsoPt=[]

    TrackEta=[]
    Trackd0=[]
    Trackd0Sig=[]


    for cpart in cparts:
        # For isolation working points are defined iloose/medium/aggrmedium/aggrloose  
        log.debug("Isolation is bound to be set to %s and event working point %s",cpart['isoInfo'],cpart['IDinfo'])
        if cpart['isoInfo'] =="iloose":
            log.debug("Loose isolation is set")
            EnIso.append(True)
            IsoCum.append(False)
            IsoCone.append(0.3)
            IsoPt.append(5*GeV)
        elif cpart['isoInfo'] =="imedium":
            log.debug("Medium isolation is set")
            EnIso.append(True)
            IsoCum.append(False)
            IsoCone.append(0.3)
            IsoPt.append(3*GeV)
        elif cpart['isoInfo'] =="iaggrloose":
            log.debug("Loose Cummulative(Aggregate) isolation is set")
            EnIso.append(True)
            IsoCum.append(True)
            IsoCone.append(0.3)
            IsoPt.append(20*GeV)
        elif cpart['isoInfo'] =="iaggrmedium":
            log.debug("Medium Cummulative(Aggregate) isolation is set")
            EnIso.append(True)
            IsoCum.append(True)
            IsoCone.append(0.3)
            IsoPt.append(10*GeV)
        else:
            log.debug("Isolation is not set")
            EnIso.append(False)
            IsoCum.append(False)
            IsoCone.append(0.3)
            IsoPt.append(5*GeV)

        #Working point defenitions
        if cpart['IDinfo'] =="loose":
            log.debug("Loose ID working point is set")
            TrackEta.append(2.5)
            Trackd0.append(1000.)
            Trackd0Sig.append(10.)
            
        elif cpart['IDinfo'] =="tight":
            log.debug("Tight ID working point is set")
            TrackEta.append(2.0)
            Trackd0.append(1000.)
            Trackd0Sig.append(5.)
        else:
            log.debug("Medium ID working point is set")
            TrackEta.append(2.5)
            Trackd0.append(1000.)
            Trackd0Sig.append(5.)



    tool.MinTrackEta = TrackEta
    tool.MaxTrackd0 = Trackd0
    tool.MaxTrackd0Sig = Trackd0Sig
    
    tool.EnableTrackIsolation = EnIso
    tool.EnableCumalitiveIsolation = IsoCum
    tool.TrackIsoCone = IsoCone
    tool.MinIsoTrackPt = IsoPt

    return tool


def TrigIsoHPtTrackTriggerHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName

    decodedDict = dictFromChainName(conf)
        
    return TrigIsoHPtTrackTriggerHypoToolFromDict( decodedDict )



if __name__ == "__main__":
    tool = TrigIsoHPtTrackTriggerHypoToolFromName("HLT_unconvtrk50_isohpttrack_imedium_L1XE50", "HLT_unconvtrk50_isohpttrack_imedium_L1XE50")
    assert tool, "Not configured simple tool"


    log.debug("ALL OK")
