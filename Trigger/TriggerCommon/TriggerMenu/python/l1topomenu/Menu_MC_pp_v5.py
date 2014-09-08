# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



def defineMenu():
    """
    Defines the following LlTopoFlags:
    
    algos   .... list of all algo names in the menu 

    """
    
    from TriggerMenu.l1topo.L1TopoFlags import L1TopoFlags
    from TriggerMenu.l1topo.TopoOutput import TopoOutput


    L1TopoFlags.algos = [
        TopoOutput( algoname='J25',            module=0, fpga=0, clock=0, firstbit=0 ),
        TopoOutput( algoname='DPhi_JsJs',      module=0, fpga=0, clock=0, firstbit=1 ),
        TopoOutput( algoname='HT200',          module=0, fpga=0, clock=0, firstbit=2 ),
        TopoOutput( algoname='EM25',           module=0, fpga=0, clock=0, firstbit=3 ),
        TopoOutput( algoname='DPhi_EMsEMs',    module=0, fpga=0, clock=0, firstbit=4 ),
        TopoOutput( algoname='DEta_EMsEMs',    module=0, fpga=0, clock=0, firstbit=5 ),
        TopoOutput( algoname='TAU25',          module=0, fpga=0, clock=0, firstbit=6 ),
        TopoOutput( algoname='DPhi_TAUsTAUs',  module=0, fpga=0, clock=0, firstbit=7 ),
        TopoOutput( algoname='DEta_TAUTAU_10', module=0, fpga=0, clock=0, firstbit=8 ),
        TopoOutput( algoname='XE',             module=0, fpga=0, clock=0, firstbit=9 ),
        ]

    
    
