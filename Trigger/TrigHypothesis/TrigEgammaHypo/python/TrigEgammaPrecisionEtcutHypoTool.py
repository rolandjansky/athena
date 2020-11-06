# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def _IncTool(name, threshold, sel):

    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionEtcutHypoTool    

    tool = TrigEgammaPrecisionEtcutHypoTool( name ) 

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
                   
    
