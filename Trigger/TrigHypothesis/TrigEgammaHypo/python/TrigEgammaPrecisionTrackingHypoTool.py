# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def _IncTool(name, tool=None):


    if not tool:
        from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionTrackingHypoTool    
        tool = TrigEgammaPrecisionTrackingHypoTool( name ) 
    tool.AcceptAll = True
    return tool


def TrigEgammaPrecisionTrackingHypoToolFromDict( d , tool=None):
    """ Use menu decoded chain dictionary to configure the tool """
    name = d['chainName'] 
    #return _IncTool( name, __th( cparts[0]),  __sel( cparts[0] ) )
    return _IncTool( name , tool=tool)               
    
