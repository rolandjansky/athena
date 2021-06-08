# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def _IncTool(name):

    tool = CompFactory.Trig.TrigEgammaEmulationPrecisionTrackingHypoTool( name ) 
    return tool


def TrigEgammaPrecisionTrackingHypoToolFromDict( name, d ):
    """ Use menu decoded chain dictionary to configure the tool """
    return _IncTool( name )               
    
