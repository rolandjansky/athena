# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV
from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoToolInc
from TrigEgammaHypo.TrigL2CaloHypoCutDefs import L2CaloCutMaps

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigL2CaloHypoTool')

def _IncTool(name, threshold, sel):

    possibleSel = L2CaloCutMaps( threshold ).MapsHADETthr.keys()

    tool = TrigL2CaloHypoToolInc( name ) 
    tool.AcceptAll = False

    if 'Validation' in TriggerFlags.enableMonitoring() or 'Online' in  TriggerFlags.enableMonitoring():
        monTool = GenericMonitoringTool("MonTool_"+name)
        monTool.defineHistogram('dEta', type='TH1F', path='EXPERT', title="L2Calo Hypo #Delta#eta_{L2 L1}; #Delta#eta_{L2 L1}", xbins=80, xmin=-0.01, xmax=0.01)
        monTool.defineHistogram('dPhi', type='TH1F', path='EXPERT', title="L2Calo Hypo #Delta#phi_{L2 L1}; #Delta#phi_{L2 L1}", xbins=80, xmin=-0.01, xmax=0.01)
        monTool.defineHistogram('Et_em', type='TH1F', path='EXPERT', title="L2Calo Hypo cluster E_{T}^{EM};E_{T}^{EM} [MeV]", xbins=50, xmin=-2000, xmax=100000)
        monTool.defineHistogram('Eta', type='TH1F', path='EXPERT', title="L2Calo Hypo entries per Eta;Eta", xbins=100, xmin=-2.5, xmax=2.5)
        monTool.defineHistogram('Phi', type='TH1F', path='EXPERT', title="L2Calo Hypo entries per Phi;Phi", xbins=128, xmin=-3.2, xmax=3.2)

        cuts=['Input','has one TrigEMCluster', '#Delta #eta L2-L1', '#Delta #phi L2-L1','eta','rCore',
              'eRatio','E_{T}^{EM}', 'E_{T}^{Had}','f_{1}','Weta2','Wstot','F3']

        monTool.defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="L2Calo Hypo Passed Cuts;Cut",
                                xbins=13, xmin=-1.5, xmax=12.5,  opt="kCumulative", labels=cuts)

        if 'Validation' in TriggerFlags.enableMonitoring():
            monTool.defineHistogram('Et_had', type='TH1F', path='EXPERT', title="L2Calo Hypo E_{T}^{had} in first layer;E_{T}^{had} [MeV]", xbins=50, xmin=-2000, xmax=100000)
            monTool.defineHistogram('Rcore', type='TH1F', path='EXPERT', title="L2Calo Hypo R_{core};E^{3x3}/E^{3x7} in sampling 2", xbins=48, xmin=-0.1, xmax=1.1)
            monTool.defineHistogram('Eratio', type='TH1F', path='EXPERT', title="L2Calo Hypo E_{ratio};E^{max1}-E^{max2}/E^{max1}+E^{max2} in sampling 1 (excl.crack)", xbins=64, xmin=-0.1, xmax=1.5)
            monTool.defineHistogram('EtaBin', type='TH1I', path='EXPERT', title="L2Calo Hypo entries per Eta bin;Eta bin no.", xbins=11, xmin=-0.5, xmax=10.5)
            monTool.defineHistogram('F1', type='TH1F', path='EXPERT', title="L2Calo Hypo f_{1};f_{1}", xbins=34, xmin=-0.5, xmax=1.2)
            monTool.defineHistogram('Weta2', type='TH1F', path='EXPERT', title="L2Calo Hypo Weta2; E Width in sampling 2", xbins=96, xmin=-0.1, xmax=0.61)
            monTool.defineHistogram('Wstot', type='TH1F', path='EXPERT', title="L2Calo Hypo Wstot; E Width in sampling 1", xbins=48, xmin=-0.1, xmax=11.)
            monTool.defineHistogram('F3', type='TH1F', path='EXPERT', title="L2Calo Hypo F3; E3/(E0+E1+E2+E3)", xbins=96, xmin=-0.1, xmax=1.1)
            
        monTool.HistPath = 'L2CaloHypo/'+tool.name()
        tool.MonTool = monTool


    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    def same( val ):
        return [val]*( len( tool.EtaBins ) - 1 )

    tool.ETthr          = same( float(threshold) )
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.F1thr          = same( 0.005 )
    tool.ET2thr         = same( 90.0*GeV )
    tool.HADET2thr      = same( 999.0 )
    tool.HADETthr       = same( 0.058 ) 
    tool.WETA2thr       = same( 99999. ) 
    tool.WSTOTthr       = same( 99999. )
    tool.F3thr          = same( 99999. )
    tool.CARCOREthr     = same( -9999. ) 
    tool.CAERATIOthr    = same( -9999. )

    if sel == 'nocut' or 'idperf' in name:
        tool.AcceptAll = True
        tool.ETthr          = same( float( threshold )*GeV ) 
        tool.dETACLUSTERthr = 9999.
        tool.dPHICLUSTERthr = 9999.
        tool.F1thr          = same( 0.0 )
        tool.HADETthr       = same( 9999. )
        tool.CARCOREthr     = same( -9999. ) 
        tool.CAERATIOthr    = same( -9999. )

    elif "etcut" in sel: # stcut is part of the name, it can as well be etcut1step (test chains)
        tool.ETthr          = same( ( float( threshold ) -  3 )*GeV ) 
        # No other cuts applied
        tool.dETACLUSTERthr = 9999.
        tool.dPHICLUSTERthr = 9999.
        tool.F1thr          = same( 0.0 )
        tool.HADETthr       = same( 9999. )
        tool.CARCOREthr     = same( -9999. )
        tool.CAERATIOthr    = same( -9999. )

    elif sel in possibleSel: # real selection
        tool.ETthr       = same( ( float( threshold ) - 3 )*GeV )
        tool.HADETthr    = L2CaloCutMaps( threshold ).MapsHADETthr[sel]
        tool.CARCOREthr  = L2CaloCutMaps( threshold ).MapsCARCOREthr[sel]
        tool.CAERATIOthr = L2CaloCutMaps( threshold ).MapsCAERATIOthr[sel]

    
    etaBinsLen = len( tool.EtaBins ) - 1
    for prop in "ETthr HADETthr CARCOREthr CARCOREthr F1thr F3thr WSTOTthr WETA2thr HADETthr HADETthr ET2thr".split():
        propLen = len( getattr( tool, prop ) ) 
        assert propLen == etaBinsLen , "In " + name + " " + prop + " has length " + str( propLen ) + " which is different from EtaBins which has length " + str( etaBinsLen )

        #    assert  _l( EtaBins, tool.ETthr, tool.HADETthr, tool.CARCOREthr, tool.CARCOREthr ) , "All list properties should have equal length ( as EtaBins )"
    #print tool
    return tool


def _MultTool(name):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoToolMult
    return TrigL2CaloHypoToolMult( name )



def TrigL2CaloHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Photon'))]
    
    def __mult(cpart):
        return int( cpart['multiplicity'] )

    def __th(cpart):
        return cpart['threshold']
    
    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']
    
    name = d['chainName']

    
    # do we need to configure high multiplicity selection, either NeX or ex_ey_ez etc...?
    if len(cparts) > 1 or __mult(cparts[0]) > 1:
        tool = _MultTool(name)
        for cpart in cparts:
            for cutNumber in range( __mult( cpart ) ):
                tool.SubTools += [ _IncTool( cpart['chainPartName']+"_"+str(cutNumber), __th( cpart ), __sel( cpart) ) ]

        return tool
    else:        
        return _IncTool( name, __th( cparts[0]),  __sel( cparts[0] ) )
                    

def TrigL2CaloHypoToolFromName( name, conf ):
    """ To be phased out """
    """ set the name of the HypoTool (name=chain) and figure out the threshold and selection from conf """

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
        
    return TrigL2CaloHypoToolFromDict( decodedDict )
    

if __name__ == "__main__":    
    TriggerFlags.enableMonitoring=['Validation']

    t = TrigL2CaloHypoToolFromName( "HLT_e10_etcut_L1EM3","HLT_e10_etcut_L1EM3" )
    assert t, "cant configure EtCut"


    t = TrigL2CaloHypoToolFromName( "HLT_2e5_etcut_L12EM3", "HLT_2e5_etcut_L12EM3" )
    assert t, "cant configure symmetric selection"
    assert len(t.SubTools) == 2, "Sub-tools not configured"

    t = TrigL2CaloHypoToolFromName( "HLT_3e5_etcut_L13EM3", "HLT_3e5_etcut_L13EM3" )
    assert t, "cant configure symmetric selection"
    assert len(t.SubTools) == 3, "Sub-tools not configured"

    # Asymmetric chais not working with this. Commenting out for now
    # t = TrigL2CaloHypoToolFromName( "HLT_e3_etcut_e5_etcut_L12EM3",  "HLT_e3_etcut_e5_etcut_L12EM3" )
    # assert t, "cant configure asymmetric selection"
    # assert len(t.SubTools) == 2, "Sub-tools not configured"

    t = TrigL2CaloHypoToolFromName( "HLT_e3_etcut_e5_etcut_mu6_L1EM3_MU3",  "HLT_e3_etcut_e5_etcut_L1EM3_MU3" )
    assert t, "cant configure asymmetric selection for combined chains"
    assert len(t.SubTools) == 2, "Sub-tools not configured"

    log.info("TrigL2CaloHypoToolFromName ALL OK" )
