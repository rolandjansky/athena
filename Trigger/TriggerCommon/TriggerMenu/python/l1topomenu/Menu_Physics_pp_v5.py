# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def defineMenu():
    """
    Defines the following LlTopoFlags:
    
    algos   .... list of all algo names in the menu 

    """
    
    from TriggerMenu.l1topo.L1TopoFlags import L1TopoFlags
    from TriggerMenu.l1topo.TopoOutput import TopoOutput

    
    L1TopoFlags.algos = [
        TopoOutput( algoname='JetDefaultDeltaPhiIncl_12', module=0, fpga=0, clock=0, firstbit=0 )
        ]

    
    
