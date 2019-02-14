

def TrigL2PhotonHypoToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """
    thresholds = sum([ [cpart['threshold']]*int(cpart['multiplicity']) for cpart in chainDict['chainParts']], [])

    name = chainDict['chainName']

    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoTool
    tool = TrigL2PhotonHypoTool(name)
    tool.MonTool = ""
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if 'Validation' in TriggerFlags.enableMonitoring() or 'Online' in  TriggerFlags.enableMonitoring():
        from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
        monTool = GenericMonitoringTool("MonTool"+name)
        monTool.Histograms = [         
            defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="L2Photon Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative"),
            defineHistogram('PtCalo', type='TH1F', path='EXPERT', title="L2Photon Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000),
            defineHistogram('CaloEta', type='TH1F', path='EXPERT', title="L2Photon Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5),
            defineHistogram('CaloPhi', type='TH1F', path='EXPERT', title="L2Photon Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2) ]

        monTool.HistPath = 'L2PhotonHypo/'+tool.name()
        tool.MonTool = monTool
        tool += monTool

    from AthenaCommon.SystemOfUnits import GeV    
    nt = len( thresholds )
    tool.ETthr = [ [0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV, 0.*GeV] ] *nt
    tool.CARCOREthr = [ [0., 0., 0., 0., 0., 0., 0., 0., 0.] ] *nt
    tool.CAERATIOthr = [ [0., 0., 0., 0., 0., 0., 0., 0., 0.] ] *nt
    tool.EtaBins = [0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47] *nt
    tool.dETACLUSTERthr = [0.1] * nt
    tool.dPHICLUSTERthr = [0.1] *nt 
    tool.F1thr = [0.005] *nt
    tool.ET2thr = [ [90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV, 90.0*GeV] ] *nt
    tool.HADET2thr = [ [999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0, 999.0] ] *nt
    tool.HADETthr = [ [0.035, 0.035, 0.035, 0.035, 0.035, 0.035, 0.035, 0.035, 0.035] ] *nt

#    for th, thvalue in enumerate(thresholds):
#        print th, thvalue
#        tool.ETthr [ th ] = [(float(thvalue)-1.)*GeV]*9

    return tool


def TrigL2PhotonHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import DictFromChainName
    decoder = DictFromChainName()
    decodedDict = decoder.analyseShortName(conf, [], "") # no L1 info
    decodedDict['chainName'] = name # override
        
    return TrigL2PhotonHypoToolFromDict( decodedDict )


if __name__ == "__main__":
    tool = TrigL2PhotonHypoToolFromName("HLT_g5_etcut", "HLT_g5_etcut")   
    assert tool, "Not configured simple tool"


    print ( "\n\nALL OK\n\n" )    
