# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
    monTool.defineHistogram('trackPt', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000) 
    monTool.defineHistogram('trackD0', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo d0^{track} ; d0^{track} ;Nevents", xbins=100, xmin=0, xmax=10)
    monTool.defineHistogram('trackNPixHits', type='TH1F', path='EXPERT', title="IsoHPtTrackTrigger Hypo N. Pix Hits^{track} ; N. Pix Hits^{track} ;Nevents", xbins=10, xmin=0, xmax=10) 

    monTool.HistPath = 'IsoHPtTrackTriggerHypoAlgMT/'+tool.getName()
    tool.MonTool = monTool

    strThr = ""

    thresholds = [ float(THR)*GeV for THR in thresholds]
    
    for THR in thresholds:
        strThr += str(THR)+", "
        
    log.info("Threshold Values are: %s",strThr)

    nt = len( thresholds )
    tool.MinTrackPt = thresholds
    tool.MaxTrackd0 = [ 5. ] *nt
    tool.MinTrackNPixHits = [ 2 ] *nt

    
    EnIso=[]
    IsoCum=[]
    IsoCone=[]
    IsoPt=[]
    # Two isolation working points are defined iloose/medium with DR windows of 0.2/0.3 
    for cpart in cparts:
        log.info("Isolation is bound to be set to %s",cpart['isoInfo'])
        if cpart['isoInfo'] =="iloose":
            log.info("Loose isolation is set")
            EnIso.append(True)
            IsoCum.append(False)
            IsoCone.append(0.2)
            IsoPt.append(5*GeV)
        elif cpart['isoInfo'] =="imedium":
            log.info("Medium isolation is set")
            EnIso.append(True)
            IsoCum.append(False)
            IsoCone.append(0.3)
            IsoPt.append(5*GeV)
        elif cpart['isoInfo'] =="icummedium":
            log.info("Medium isolation is set")
            EnIso.append(True)
            IsoCum.append(True)
            IsoCone.append(0.3)
            IsoPt.append(5*GeV)
        else:
            log.info("Isolation is not set")
            EnIso.append(False)
            IsoCum.append(False)
            IsoCone.append(0.3)
            IsoPt.append(5*GeV)
    
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


    log.info("ALL OK")
