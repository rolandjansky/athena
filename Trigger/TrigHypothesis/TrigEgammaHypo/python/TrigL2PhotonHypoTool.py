
def TrigL2PhotonHypoToolFromName( name ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    bname = name.split('_')

    threshold = bname[1]
    from TrigEgammaHypo.TrigL2CaloHypoTool import decodeThreshold
    thresholds = decodeThreshold( threshold )

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoTool
    tool = TrigL2PhotonHypoTool(name)
    tool.MonTool = ""
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if 'Validation' in TriggerFlags.enableMonitoring() or 'Online' in  TriggerFlags.enableMonitoring():
        from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
        monTool = GenericMonitoringTool("MonTool"+name)
        monTool.Histograms = [         
            defineHistogram('CutCounter', type='TH1I', title="L2Photon Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative"),
            defineHistogram('PtCalo', type='TH1F', title="L2Photon Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000),
            defineHistogram('CaloEta', type='TH1F', title="L2Photon Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5),
            defineHistogram('CaloPhi', type='TH1F', title="L2Photon Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2) ]        

        monTool.HistPath = 'L2PhotonHypo/'+tool.name()
        tool.MonTool = monTool
        tool += monTool

    return tool


if __name__ == "__main__":
    tool = TrigL2PhotonHypoToolFromName("HLT_g5_etcut")   
    assert tool, "Not configured simple tool"


    print ( "\n\nALL OK\n\n" )    
