# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def TrigL2ElectronHypoToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in chainDict['chainParts'] if i['signature'] is 'Electron']
    thresholds = sum([ [cpart['threshold']]*int(cpart['multiplicity']) for cpart in cparts], [])


    name = chainDict['chainName']
    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2ElectronHypoTool
    tool = TrigL2ElectronHypoTool(name)
    tool.RespectPreviousDecision = True
    tool.MonTool = ""
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if 'Validation' in TriggerFlags.enableMonitoring() or 'Online' in  TriggerFlags.enableMonitoring():
        from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
        monTool = GenericMonitoringTool("MonTool"+name)
        monTool.Histograms = [         
            defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="L2Electron Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative"),
            defineHistogram('CaloTrackdEta', type='TH1F', path='EXPERT', title="L2Electron Hypo #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4),
            defineHistogram('CaloTrackdPhi', type='TH1F', path='EXPERT', title="L2Electron Hypo #Delta #phi between cluster and track;#Delta #phi;Nevents", xbins=80, xmin=-0.4, xmax=0.4),
            defineHistogram('CaloTrackEoverP', type='TH1F', path='EXPERT', title="L2Electron Hypo E/p;E/p;Nevents", xbins=120, xmin=0, xmax=12),
            defineHistogram('PtTrack', type='TH1F', path='EXPERT', title="L2Electron Hypo p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000),
            defineHistogram('PtCalo', type='TH1F', path='EXPERT', title="L2Electron Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000),
            defineHistogram('CaloEta', type='TH1F', path='EXPERT', title="L2Electron Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5),
            defineHistogram('CaloPhi', type='TH1F', path='EXPERT', title="L2Electron Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2) ]

        monTool.HistPath = 'L2ElectronHypo/'+tool.name()
        tool.MonTool = monTool
        tool += monTool

    from AthenaCommon.SystemOfUnits import GeV    
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


def TrigL2ElectronHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName
    decoder = DictFromChainName()
    decodedDict = decoder.analyseShortName(conf, [], "") # no L1 info
    decodedDict['chainName'] = name # override
        
    return TrigL2ElectronHypoToolFromDict( decodedDict )



if __name__ == "__main__":
    tool = TrigL2ElectronHypoToolFromName("HLT_e3_etcut", "HLT_e3_etcut")
    assert tool, "Not configured simple tool"

    tool = TrigL2ElectronHypoToolFromName("HLT_2e3_etcut", "HLT_2e3_etcut")    
    assert tool, "Not configured simple tool"
    assert len(tool.TrackPt) == 2, "Multiplicity missonfigured, set "+ str( len( tool.TrackPt ) )

    tool = TrigL2ElectronHypoToolFromName("HLT_e3_e5_etcut", "HLT_e3_e5_etcut")    
    assert tool, "Not configured simple tool"
    assert len(tool.TrackPt) == 2, "Multiplicity missonfigured, set "+ str( len( tool.TrackPt ) )

    print ( "\n\nALL OK\n\n" )    
