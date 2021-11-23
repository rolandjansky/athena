# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV


def _IncTool(name, monGroups, threshold, sel, tool=None):


    if not tool:
        from AthenaConfiguration.ComponentFactory import CompFactory
        tool = CompFactory.TrigEgammaPrecisionCaloHypoTool(name)

    if hasattr(tool, "MonTool"):

        from TrigEgammaMonitoring.TrigEgammaMonitoringMTConfig import doOnlineMonForceCfg
        doOnlineMonAllChains = doOnlineMonForceCfg()
        if (any('egammaMon:online' in group for group in monGroups) or doOnlineMonAllChains):
            from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
            monTool = GenericMonitoringTool("MonTool_"+name)
            monTool.Histograms = [ defineHistogram('dEta', type='TH1F', path='EXPERT', title="PrecisionCalo Hypo #Delta#eta_{L2 L1}; #Delta#eta_{L2 L1}", xbins=80, xmin=-0.01, xmax=0.01),
                                defineHistogram('dPhi', type='TH1F', path='EXPERT', title="PrecisionCalo Hypo #Delta#phi_{L2 L1}; #Delta#phi_{L2 L1}", xbins=80, xmin=-0.01, xmax=0.01),
                                defineHistogram('Et_em', type='TH1F', path='EXPERT', title="PrecisionCalo Hypo cluster E_{T}^{EM};E_{T}^{EM} [MeV]", xbins=50, xmin=-2000, xmax=100000),]

            cuts=['Input','#Delta #eta L2-L1', '#Delta #phi L2-L1','eta','E_{T}^{EM}']

            monTool.Histograms += [ defineHistogram('CutCounter', type='TH1I', path='EXPERT', title="PrecisionCalo Hypo Passed Cuts;Cut",
                                                    xbins=13, xmin=-1.5, xmax=12.5,  opt="kCumulative", xlabels=cuts) ]

            monTool.HistPath = 'PrecisionCaloHypo/'+tool.getName()
            tool.MonTool = monTool


    tool.EtaBins        = [0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47]
    def same( val ):
        return [val]*( len( tool.EtaBins ) - 1 )

    tool.ETthr          = same( float(threshold)*GeV )
    tool.dETACLUSTERthr = 0.1
    tool.dPHICLUSTERthr = 0.1
    tool.ET2thr         = same( 90.0*GeV )

    if sel == 'nocut':
        tool.AcceptAll = True
        tool.ETthr          = same( float( threshold )*GeV ) 
        tool.dETACLUSTERthr = 9999.
        tool.dPHICLUSTERthr = 9999.

    elif sel in ('etcut', 'ion'):
        tool.ETthr          = same( ( float( threshold ) -  3 )*GeV ) 
        # No other cuts applied
        tool.dETACLUSTERthr = 9999.
        tool.dPHICLUSTERthr = 9999.

 
    return tool


def TrigEgammaPrecisionCaloHypoToolFromDict( d, tool=None ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Photon'))]

    def __th(cpart):
        return cpart['threshold']
    
    def __sel(cpart):
        return 'ion' if 'ion' in cpart['extra'] else (cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo'])
    
    name = d['chainName']
    monGroups = d['monGroups']    
    return _IncTool( name, monGroups, __th( cparts[0]),  __sel( cparts[0] ) , tool=tool)
                   
    
