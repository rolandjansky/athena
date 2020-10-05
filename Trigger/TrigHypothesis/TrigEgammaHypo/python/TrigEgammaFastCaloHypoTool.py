# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Include import Include 
# flake8: noqa 
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags 
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigEgammaHypo.TrigL2CaloHypoCutDefs import L2CaloCutMaps
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool,defineHistogram

_possibleSel  = { 'tight':'Tight', 'medium':'Medium', 'loose':'Loose', 'vloose':'VeryLoose',
                  'lhtight':'Tight', 'lhmedium':'Medium', 'lhloose':'Loose', 'lhvloose':'VeryLoose'}


from AthenaCommon.Logging import logging
log = logging.getLogger('TrigEgammaFastCaloHypoTool')

def _GetPath( cand, sel, basepath = 'RingerSelectorTools/TrigL2_20180903_v9' ):
    
    logger = logging.getLogger("TrigMultiVarHypo.GetPath")
    from TriggerMenu.egamma.EgammaSliceFlags import EgammaSliceFlags
    if EgammaSliceFlags.ringerVersion():
       basepath = EgammaSliceFlags.ringerVersion()
    logger.info('TrigMultiVarHypo version: %s', basepath)
    if not sel in _possibleSel.keys():
       raise RuntimeError( "Bad selection name: %s" % sel )
    if 'e' in cand:
       constant = basepath+'/'+ 'TrigL2CaloRingerElectron{SEL}Constants.root'.format(SEL=_possibleSel[sel])
       threshold = basepath+'/'+ 'TrigL2CaloRingerElectron{SEL}Thresholds.root'.format(SEL=_possibleSel[sel])
    elif 'g' in cand:
       constant = basepath+'/'+ 'TrigL2CaloRingerPhoton{SEL}Constants.root'.format(SEL=_possibleSel[sel])
       threshold = basepath+'/'+ 'TrigL2CaloRingerPhoton{SEL}Thresholds.root'.format(SEL=_possibleSel[sel])
    else:
       raise RuntimeError( "Bad signature %s" % cand )
    return constant, threshold


def _IncTool(name, cand, threshold, sel):

    possibleSel = L2CaloCutMaps( threshold ).MapsHADETthr.keys()
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.TrigEgammaFastCaloHypoToolInc( name )
    tool.AcceptAll = False
    tool.UseRinger = False
    

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
                                xbins=13, xmin=-1.5, xmax=12.5,  opt="kCumulative", xlabels=cuts)

        if 'Validation' in TriggerFlags.enableMonitoring():
            monTool.defineHistogram('Et_had', type='TH1F', path='EXPERT', title="L2Calo Hypo E_{T}^{had} in first layer;E_{T}^{had} [MeV]", xbins=50, xmin=-2000, xmax=100000)
            monTool.defineHistogram('Rcore', type='TH1F', path='EXPERT', title="L2Calo Hypo R_{core};E^{3x3}/E^{3x7} in sampling 2", xbins=48, xmin=-0.1, xmax=1.1)
            monTool.defineHistogram('Eratio', type='TH1F', path='EXPERT', title="L2Calo Hypo E_{ratio};E^{max1}-E^{max2}/E^{max1}+E^{max2} in sampling 1 (excl.crack)", xbins=64, xmin=-0.1, xmax=1.5)
            monTool.defineHistogram('EtaBin', type='TH1I', path='EXPERT', title="L2Calo Hypo entries per Eta bin;Eta bin no.", xbins=11, xmin=-0.5, xmax=10.5)
            monTool.defineHistogram('F1', type='TH1F', path='EXPERT', title="L2Calo Hypo f_{1};f_{1}", xbins=34, xmin=-0.5, xmax=1.2)
            monTool.defineHistogram('Weta2', type='TH1F', path='EXPERT', title="L2Calo Hypo Weta2; E Width in sampling 2", xbins=96, xmin=-0.1, xmax=0.61)
            monTool.defineHistogram('Wstot', type='TH1F', path='EXPERT', title="L2Calo Hypo Wstot; E Width in sampling 1", xbins=48, xmin=-0.1, xmax=11.)
            monTool.defineHistogram('F3', type='TH1F', path='EXPERT', title="L2Calo Hypo F3; E3/(E0+E1+E2+E3)", xbins=96, xmin=-0.1, xmax=1.1)

        monTool.HistPath = 'L2CaloHypo/'+tool.getName()
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
        tool.UseRinger = False
        tool.ETthr          = same( float( threshold )*GeV )
        tool.dETACLUSTERthr = 9999.
        tool.dPHICLUSTERthr = 9999.
        tool.F1thr          = same( 0.0 )
        tool.HADETthr       = same( 9999. )
        tool.CARCOREthr     = same( -9999. )
        tool.CAERATIOthr    = same( -9999. )

    elif sel == "etcut": # stcut is part of the name, it can as well be etcut1step (test chains)
        tool.UseRinger = False
        tool.ETthr          = same( ( float( threshold ) -  3 )*GeV )
        # No other cuts applied
        tool.dETACLUSTERthr = 9999.
        tool.dPHICLUSTERthr = 9999.
        tool.F1thr          = same( 0.0 )
        tool.HADETthr       = same( 9999. )
        tool.CARCOREthr     = same( -9999. )
        tool.CAERATIOthr    = same( -9999. )

    elif sel in possibleSel and "noringer" in name: # real selection
        tool.UseRinger = False
        tool.ETthr       = same( ( float( threshold ) - 3 )*GeV )
        tool.HADETthr    = L2CaloCutMaps( threshold ).MapsHADETthr[sel]
        tool.CARCOREthr  = L2CaloCutMaps( threshold ).MapsCARCOREthr[sel]
        tool.CAERATIOthr = L2CaloCutMaps( threshold ).MapsCAERATIOthr[sel]

    elif sel in possibleSel and cand=="g": # real selection
        tool.UseRinger = False
        tool.ETthr	  = same( ( float( threshold ) - 3 )*GeV )
        tool.HADETthr    = L2CaloCutMaps( threshold ).MapsHADETthr[sel]
        tool.CARCOREthr  = L2CaloCutMaps( threshold ).MapsCARCOREthr[sel]
        tool.CAERATIOthr = L2CaloCutMaps( threshold ).MapsCAERATIOthr[sel]    

    elif sel in _possibleSel.keys() and not "noringer" in name and cand=="e":
        tool.UseRinger = True
        pconstants, pthresholds = _GetPath( cand, sel )
        tool.ConstantsCalibPath = pconstants
        tool.ThresholdsCalibPath = pthresholds
        tool.MonTool = ""
        tool.EtCut = (float(threshold)-3.)*GeV        

        # monitoring part  
    
        if (('Validation' in TriggerFlags.enableMonitoring()) or
        ('Online' in  TriggerFlags.enableMonitoring())    or 
        (athenaCommonFlags.isOnline)):
            monTool = GenericMonitoringTool('MonTool'+name)
            monTool.Histograms = [
                defineHistogram( "TIME_total", path='EXPERT',title="Total Time;time[ms]",xbins=50, xmin=0,xmax=5,type='TH1F'),
                defineHistogram( "TIME_preproc", path='EXPERT',title="Total Time;time[ms]",xbins=50, xmin=0,xmax=5,type='TH1F'),
                defineHistogram( "TIME_propagate",path='EXPERT', title="Total Time;time[ms]",xbins=50, xmin=0,xmax=5,type='TH1F'),
                defineHistogram('Eta', type='TH1F', path='EXPERT',title="#eta of Clusters; #eta; number of RoIs", xbins=50,xmin=-2.5,xmax=2.5),
                defineHistogram('Phi',type='TH1F', path='EXPERT',title="#phi of Clusters; #phi; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2),
                defineHistogram('Et',type='TH1F', path='EXPERT',title="E_{T} of Clusters; E_{T} [MeV]; number of RoIs", xbins=60,xmin=0,xmax=5e4),
                defineHistogram('RnnOut',type='TH1F', path='EXPERT',title="Neural Network output; NN output; number of RoIs", xbins=100,xmin=-10,xmax=10),
            ]
    
            monTool.HistPath='L2CaloHypo_Ringer/'+monTool.name()
            tool.MonTool=monTool


    
    etaBinsLen = len( tool.EtaBins ) - 1
    for prop in "ETthr HADETthr CARCOREthr CARCOREthr F1thr F3thr WSTOTthr WETA2thr HADETthr HADETthr ET2thr".split():
        propLen = len( getattr( tool, prop ) )
        assert propLen == etaBinsLen , "In " + name + " " + prop + " has length " + str( propLen ) + " which is different from EtaBins which has length " + str( etaBinsLen )

    return tool


def _MultTool(name):
    from AthenaConfiguration.ComponentFactory import CompFactory
    return CompFactory.TrigEgammaFastCaloHypoToolMult( name )



def TrigEgammaFastCaloHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Photon'))]

    from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgOnlineDefault
    LuminosityCondAlgOnlineDefault()    

    #def __mult(cpart):
     #   return int( cpart['multiplicity'] )

    def __th(cpart):
        return cpart['threshold']

    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']

    def __cand(cpart):
        return cpart['trigType']

    name = d['chainName']


    # do we need to configure high multiplicity selection, either NeX or ex_ey_ez etc...?
    #if len(cparts) > 1 or __mult(cparts[0]) > 1:
     #   tool = _MultTool(name)
     #   for cpart in cparts:
      #      for cutNumber in range( __mult( cpart ) ):
       #         tool.SubTools += [ _IncTool( cpart['chainPartName']+"_"+str(cutNumber), __cand(cpart), __th( cpart ), __sel( cpart) ) ]

       # return tool
    # else:
    return _IncTool( name, __cand( cparts[0]), __th( cparts[0]),  __sel( cparts[0]))


def TrigEgammaFastCaloHypoToolFromName( name, conf ):
    """ To be phased out """
    """ set the name of the HypoTool (name=chain) and figure out the threshold and selection from conf """

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)

    return TrigEgammaFastCaloHypoToolFromDict( decodedDict )


if __name__ == "__main__":
    TriggerFlags.enableMonitoring=['Validation']

    t = TrigEgammaFastCaloHypoToolFromName( "HLT_e10_etcut_L1EM3","HLT_e10_etcut_L1EM3" )
    assert t, "cant configure EtCut"


    #t = TrigEgammaFastCaloHypoToolFromName( "HLT_2e5_etcut_L12EM3", "HLT_2e5_etcut_L12EM3" )
    #assert t, "cant configure symmetric selection"
    #assert len(t.SubTools) == 2, "Sub-tools not configured"

    #t = TrigEgammaFastCaloHypoToolFromName( "HLT_3e5_etcut_L13EM3", "HLT_3e5_etcut_L13EM3" )
    #assert t, "cant configure symmetric selection"
    #assert len(t.SubTools) == 3, "Sub-tools not configured"

    # Asymmetric chais not working with this. Commenting out for now
    # t = TrigEgammaFastCaloHypoToolFromName( "HLT_e3_etcut_e5_etcut_L12EM3",  "HLT_e3_etcut_e5_etcut_L12EM3" )
    # assert t, "cant configure asymmetric selection"
    # assert len(t.SubTools) == 2, "Sub-tools not configured"

    #t = TrigEgammaFastCaloHypoToolFromName( "HLT_e3_etcut_e5_etcut_mu6_L1EM3_MU3",  "HLT_e3_etcut_e5_etcut_L1EM3_MU3" )
    #assert t, "cant configure asymmetric selection for combined chains"
    #assert len(t.SubTools) == 2, "Sub-tools not configured"

    log.info("TrigEgammaFastCaloHypoToolFromName ALL OK" )
