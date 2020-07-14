# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Logging import logging
from AthenaCommon.Include import Include 
# flake8: noqa 
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool



log = logging.getLogger('TrigEgammaFastElectronHypoTool')

def TrigEgammaFastElectronHypoToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='Electron']
    
    thresholds = sum([ [cpart['threshold']]*int(cpart['multiplicity']) for cpart in cparts], [])

    name = chainDict['chainName']
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.TrigEgammaFastElectronHypoTool(name)

    monTool = GenericMonitoringTool("MonTool"+name)
    monTool.defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="FastElectron Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative")
    monTool.defineHistogram('CaloTrackdEta', type='TH1F', path='EXPERT', title="FastElectron Hypo #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4)
    monTool.defineHistogram('CaloTrackdPhi', type='TH1F', path='EXPERT', title="FastElectron Hypo #Delta #phi between cluster and track;#Delta #phi;Nevents", xbins=80, xmin=-0.4, xmax=0.4)
    monTool.defineHistogram('CaloTrackEoverP', type='TH1F', path='EXPERT', title="FastElectron Hypo E/p;E/p;Nevents", xbins=120, xmin=0, xmax=12)
    monTool.defineHistogram('PtTrack', type='TH1F', path='EXPERT', title="FastElectron Hypo p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000)
    monTool.defineHistogram('PtCalo', type='TH1F', path='EXPERT', title="FastElectron Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000)
    monTool.defineHistogram('CaloEta', type='TH1F', path='EXPERT', title="FastElectron Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5)
    monTool.defineHistogram('CaloPhi', type='TH1F', path='EXPERT', title="FastElectron Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2)

    monTool.HistPath = 'FastElectronHypo/'+tool.getName()
    tool.MonTool = monTool

    nt = len( thresholds )
    tool.TrackPt = [0.0] * nt
    tool.CaloTrackdETA = [ 0.2 ] *nt
    tool.CaloTrackdPHI = [ 990. ] *nt
    tool.CaloTrackdEoverPLow = [ 0.0 ] * nt
    tool.CaloTrackdEoverPHigh = [ 999.0 ] * nt
    tool.TRTRatio = [ -999. ] * nt


    for th, thvalue in enumerate(thresholds):        
        if float(thvalue) < 15:
            tool.TrackPt[ th ] = 1.0 * GeV 
        elif float(thvalue) >= 15 and float(thvalue) < 20:
            tool.TrackPt[ th ] = 2.0 * GeV 
        elif float(thvalue) >= 20 and float(thvalue) < 50:
            tool.TrackPt[ th ] =  3.0 * GeV 
        elif float(thvalue) >= 50:
            tool.TrackPt[ th ] =  5.0 * GeV 
            tool.CaloTrackdETA[ th ] =  999. 
            tool.CaloTrackdPHI[ th ] =  999.
        else:
            raise RuntimeError('No threshold: Default cut configured')
    return tool


def TrigEgammaFastElectronHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName

    decodedDict = dictFromChainName(conf)
        
    return TrigEgammaFastElectronHypoToolFromDict( decodedDict )



if __name__ == "__main__":
    tool = TrigEgammaFastElectronHypoToolFromName("HLT_e3_etcut_L1EM3", "HLT_e3_etcut_L1EM3")
    assert tool, "Not configured simple tool"

    tool = TrigEgammaFastElectronHypoToolFromName("HLT_2e3_etcut_L1E2M3", "HLT_2e3_etcut_L12EM3")    
    assert tool, "Not configured simple tool"
    assert len(tool.TrackPt) == 2, "Multiplicity missonfigured, set "+ str( len( tool.TrackPt ) )

    # Asymmetric chais not working with this. Commenting out for now
    # tool = TrigL2ElectronHypoToolFromName("HLT_e3_e5_etcut_L12EM3", "HLT_e3_e5_etcut_L12EM3")    
    # assert tool, "Not configured simple tool"
    # assert len(tool.TrackPt) == 2, "Multiplicity missonfigured, set "+ str( len( tool.TrackPt ) )

    log.info("ALL OK")
