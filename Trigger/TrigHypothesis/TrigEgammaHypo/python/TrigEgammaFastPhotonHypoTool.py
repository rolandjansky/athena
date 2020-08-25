# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Include import Include 
# flake8: noqa 
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
from AthenaCommon.Logging import logging
log = logging.getLogger('TrigEgammaFastPhotonHypoTool')

def TrigEgammaFastPhotonHypoToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """
    thresholds = sum([ [cpart['threshold']]*int(cpart['multiplicity']) for cpart in chainDict['chainParts']], [])

    name = chainDict['chainName']
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.TrigEgammaFastPhotonHypoTool(name)

    monTool = GenericMonitoringTool("MonTool"+name)
    monTool.defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="FastPhoton Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative")
    monTool.defineHistogram('PtCalo', type='TH1F', path='EXPERT', title="FastPhoton Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000)
    monTool.defineHistogram('CaloEta', type='TH1F', path='EXPERT', title="FastPhoton Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5)
    monTool.defineHistogram('CaloPhi', type='TH1F', path='EXPERT', title="FastPhoton Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2)

    monTool.HistPath = 'FastPhotonHypo/'+tool.getName()
    tool.MonTool = monTool

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

    return tool


def TrigEgammaFastPhotonHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
        
    return TrigEgammaFastPhotonHypoToolFromDict( decodedDict )


if __name__ == "__main__":
    tool = TrigEgammaFastPhotonHypoToolFromName("HLT_g5_etcut_L1EM3", "HLT_g5_etcut_L1EM3")   
    assert tool, "Not configured simple tool"

    log.info("ALL OK")
