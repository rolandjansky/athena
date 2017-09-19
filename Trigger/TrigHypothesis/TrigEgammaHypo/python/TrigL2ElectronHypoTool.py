


def TrigL2ElectronHypoToolFromName( name ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    assert name.startswith("HLT_e"), "The chain name does not start from HLT_e"

    bname = name.split('_')
    threshold = int(bname[1][1:])
    assert str(threshold) == bname[1][1:], "Threshold definition is not a simple int"
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2ElectronHypoTool

    tool = TrigL2ElectronHypoTool(name)
    tool.MonTool = ""
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if 'Validation' in TriggerFlags.enableMonitoring() or 'Online' in  TriggerFlags.enableMonitoring():
        from AthenaMonitoring.AthenaMonitoringConf import GenericMonitoringTool
        from AthenaMonitoring.DefineHistogram import defineHistogram
        monTool = GenericMonitoringTool("MonTool"+name)
        monTool.Histograms = [         
            defineHistogram('CutCounter', type='TH1I', title="L2Electron Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative", labels=labelsDescription),
            defineHistogram('CaloTrackdEta', type='TH1F', title="L2Electron Hypo #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4),
            defineHistogram('CaloTrackdPhi', type='TH1F', title="L2Electron Hypo #Delta #phi between cluster and track;#Delta #phi;Nevents", xbins=80, xmin=-0.4, xmax=0.4),
            defineHistogram('CaloTrackEoverP', type='TH1F', title="L2Electron Hypo E/p;E/p;Nevents", xbins=120, xmin=0, xmax=12),
            defineHistogram('PtTrack', type='TH1F', title="L2Electron Hypo p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000),
            defineHistogram('PtCalo', type='TH1F', title="L2Electron Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000),
            defineHistogram('CaloEta', type='TH1F', title="L2Electron Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5),
            defineHistogram('CaloPhi', type='TH1F', title="L2Electron Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2) ]        

        monTool.HistPath = 'L2ElectronHypo/'+tool.name()
        tool.MonTool = monTool
        tool += monTool

    from AthenaCommon.SystemOfUnits import GeV    
    tool.TrackPt = [ 1.0 * GeV ] 
    tool.CaloTrackdETA = [ 0.2 ]
    tool.CaloTrackdPHI = [ 999. ]
    tool.CaloTrackdEoverPLow = [ 0.0 ]
    tool.CaloTrackdEoverPHigh = [ 999.0 ]
    tool.TRTRatio = [ -999. ]

    if float(threshold) < 15:
        tool.TrackPt = [ 1.0 * GeV ]
    elif float(threshold) >= 15 and float(threshold) < 20:
        tool.TrackPt = [ 2.0 * GeV ]
    elif float(threshold) >= 20 and float(threshold) < 50:
        tool.TrackPt = [ 3.0 * GeV ]
    elif float(threshold) >= 50:
        tool.TrackPt = [ 5.0 * GeV ]
        tool.CaloTrackdETA = [ 999. ]
        tool.CaloTrackdPHI = [ 999. ]
    else:
        raise RuntimeError('No threshold: Default cut configured')
    return tool

if __name__ == "__main__":
    tool = TrigL2ElectronHypoToolFromName("HLT_e4")    
