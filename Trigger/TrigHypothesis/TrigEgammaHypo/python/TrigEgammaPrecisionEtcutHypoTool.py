# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV

def _IncTool(name, threshold, sel):

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionEtcutHypoTool    

    tool = TrigEgammaPrecisionEtcutHypoTool( name ) 

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
    monTool = GenericMonitoringTool("MonTool_"+name)

    tool.AcceptAll = True
    return tool


def TrigEgammaPrecisionEtcutHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Photon'))]

    def __th(cpart):
        return cpart['threshold']
    
    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']
    
    name = d['chainName']
        
    return _IncTool( name, __th( cparts[0]),  __sel( cparts[0] ) )
                   
    
