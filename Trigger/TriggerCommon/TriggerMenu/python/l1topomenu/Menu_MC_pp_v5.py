# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def defineMenu():
    """
    Defines the following LlTopoFlags:
    
    algos   .... list of all algo names in the menu 

    """
    
    from TriggerMenu.l1topo.L1TopoFlags import L1TopoFlags
    from TriggerMenu.l1topo.TopoOutput import TopoOutput

    L1TopoFlags.algos = [
        
        # module: 0, FPGA: 0
        TopoOutput( algoname='INVM_AJ_HighMass',                            module=0, fpga=0, clock=0, firstbit=0 ), # need 4bits 
        TopoOutput( algoname='INVM_AJ_LowMass',                             module=0, fpga=0, clock=0, firstbit=4 ), # 4 bits 
        TopoOutput( algoname='HT150-J20s5.ETA31',                           module=0, fpga=0, clock=0, firstbit=9 ),
        TopoOutput( algoname='HT190-J15s5.ETA21',                           module=0, fpga=0, clock=0, firstbit=10 ),
        TopoOutput( algoname='INVM_EMall',                                  module=0, fpga=0, clock=0, firstbit=11 ), # 3bits
        TopoOutput( algoname='10MINDPHI-EM10s6-XE0',                        module=0, fpga=0, clock=0, firstbit=14 ),
        TopoOutput( algoname='15MINDPHI-EM10s6-XE0',                        module=0, fpga=0, clock=0, firstbit=15 ),
        TopoOutput( algoname='05MINDPHI-EM15s6-XE0',                        module=0, fpga=0, clock=1, firstbit=0 ),
        TopoOutput( algoname='25MT-EM10s6-XE0',                             module=0, fpga=0, clock=1, firstbit=1 ),
        TopoOutput( algoname='30MT-EM10s6-XE0',                             module=0, fpga=0, clock=1, firstbit=2 ),
        TopoOutput( algoname='35MT-EM15s6-XE0',                             module=0, fpga=0, clock=1, firstbit=3 ),
        TopoOutput( algoname='0DETA10-Js1-Js2',                             module=0, fpga=0, clock=1, firstbit=4 ),
        TopoOutput( algoname='10MINDPHI-Js2-XE50',                          module=0, fpga=0, clock=1, firstbit=5 ),
        TopoOutput( algoname='10MINDPHI-J20s2-XE50',                        module=0, fpga=0, clock=1, firstbit=6 ),
        TopoOutput( algoname='210RATIO-0MATCH-TAU30si2-EMall',              module=0, fpga=0, clock=1, firstbit=7 ), ### WRONG FPGA 1/0
        TopoOutput( algoname='NOT-0MATCH-TAU30si2-EMall',                   module=0, fpga=0, clock=1, firstbit=8 ), 
        TopoOutput( algoname='LAR-EM50s1',                                  module=0, fpga=0, clock=1, firstbit=9 ), # -> former FPGA 4 ### WRONG FPGA
        TopoOutput( algoname='LAR-J100s1',                                  module=0, fpga=0, clock=1, firstbit=10 ), # -> former FPGA 4 ### WRONG FPGA
        TopoOutput( algoname='NOT-02MATCH-EM10s1-AJj15all.ETA49',           module=0, fpga=0, clock=1, firstbit=11 ), # -> former FPGA 4
        TopoOutput( algoname='05RATIO-XE0-SUM0-EM10s1-HT0-AJj15all.ETA49',  module=0, fpga=0, clock=1, firstbit=12 ), # -> former FPGA 4        
        
        # module: 0, FPGA: 1
        TopoOutput( algoname='1DISAMB-TAU20ab-J20ab',                       module=0, fpga=1, clock=0, firstbit=0 ), ### WRONG FPGA
        TopoOutput( algoname='DISAMB-EM15abhi-TAU12abi',                    module=0, fpga=1, clock=0, firstbit=1 ),
        TopoOutput( algoname='1DISAMB-EM15his2-TAU12abi-J25ab',             module=0, fpga=1, clock=0, firstbit=2 ), 
        TopoOutput( algoname='1DISAMB-J25ab-0DR28-EM15his2-TAU12abi',       module=0, fpga=1, clock=0, firstbit=3 ),
        TopoOutput( algoname='4INVM8-2MU6ab',                               module=0, fpga=1, clock=0, firstbit=4 ), # M9
        TopoOutput( algoname='2INVM999-ONEBARREL-MU6ab-MU4ab',              module=0, fpga=1, clock=0, firstbit=5 ), 
        TopoOutput( algoname='4INVM8-ONEBARREL-MU6ab-MU4ab',                module=0, fpga=1, clock=0, firstbit=6 ),
        TopoOutput( algoname='5DETA99-5DPHI99-2MU4ab',                      module=0, fpga=1, clock=0, firstbit=7 ), # M9
        TopoOutput( algoname='5DETA99-5DPHI99-MU6ab-MU4ab',                 module=0, fpga=1, clock=0, firstbit=8 ), # M9
        TopoOutput( algoname='5DETA99-5DPHI99-2MU6ab',                      module=0, fpga=1, clock=0, firstbit=9 ), # M9
        TopoOutput( algoname='1DISAMB-TAU20abi-TAU12abi-J25ab',             module=0, fpga=1, clock=0, firstbit=10 ),
        TopoOutput( algoname='0DR28-MU10ab-TAU12abi',                       module=0, fpga=1, clock=0, firstbit=11 ), # M9
        TopoOutput( algoname='0DETA20-0DPHI20-TAU20abi-TAU12abi',           module=0, fpga=1, clock=0, firstbit=12 ), # -> former FPGA 4
        TopoOutput( algoname='DISAMB-EM15abhi-TAU40ab',                     module=0, fpga=1, clock=0, firstbit=13 ), # -> added back in
        
        # module: 1, FPGA: 0        
        TopoOutput( algoname='05MINDPHI-AJj20s6-XE0',                       module=1, fpga=0, clock=0, firstbit=0 ),
        TopoOutput( algoname='10MINDPHI-AJj20s6-XE0',                       module=1, fpga=0, clock=0, firstbit=1 ),
        TopoOutput( algoname='15MINDPHI-AJj20s6-XE0',                       module=1, fpga=0, clock=0, firstbit=2 ),
        TopoOutput( algoname='0DR04-MU4ab-CJ15ab',                          module=1, fpga=0, clock=0, firstbit=3 ), # fpga 1 clock 0
        TopoOutput( algoname='0DR04-MU4ab-CJ20ab',                          module=1, fpga=0, clock=0, firstbit=4 ),
        TopoOutput( algoname='0DR04-MU4ab-CJ30ab',                          module=1, fpga=0, clock=0, firstbit=5 ),
        TopoOutput( algoname='0DR04-MU6ab-CJ20ab',                          module=1, fpga=0, clock=0, firstbit=6 ),
        TopoOutput( algoname='0DR04-MU6ab-CJ25ab',                          module=1, fpga=0, clock=0, firstbit=7 ),
        TopoOutput( algoname='10MINDPHI-CJ20ab-XE50',                       module=1, fpga=0, clock=0, firstbit=8 ),
        TopoOutput( algoname='2DR15-2CMU4ab',                               module=1, fpga=0, clock=0, firstbit=9 ),
        TopoOutput( algoname='2DR15-CMU6ab-CMU4ab',                         module=1, fpga=0, clock=0, firstbit=10 ),
        TopoOutput( algoname='MULT-CMU4ab',                                 module=1, fpga=0, clock=0, firstbit=11 ), # 2bits
        TopoOutput( algoname='MULT-CMU6ab',                                 module=1, fpga=0, clock=0, firstbit=13 ), # 2bits
        TopoOutput( algoname='2INVM999-2CMU4ab',                            module=1, fpga=0, clock=0, firstbit=15 ), 
        TopoOutput( algoname='2INVM999-CMU6ab-CMU4ab',                      module=1, fpga=0, clock=1, firstbit=0 ), 
        TopoOutput( algoname='4INVM8-2CMU4ab',                              module=1, fpga=0, clock=1, firstbit=1 ),
        TopoOutput( algoname='4INVM8-CMU6ab-CMU4ab',                        module=1, fpga=0, clock=1, firstbit=2 ),
        TopoOutput( algoname='0DETA04-0DPHI03-EM8abi-MU10ab',               module=1, fpga=0, clock=1, firstbit=3 ),
        TopoOutput( algoname='0DETA04-0DPHI03-EM15abi-MUab',                module=1, fpga=0, clock=1, firstbit=4 ),
        TopoOutput( algoname='2DR15-CMU4ab-MU4ab',                          module=1, fpga=0, clock=1, firstbit=5 ),
        TopoOutput( algoname='2INVM999-CMU4ab-MU4ab',                       module=1, fpga=0, clock=1, firstbit=6 ), 
        TopoOutput( algoname='4INVM8-CMU4ab-MU4ab',                         module=1, fpga=0, clock=1, firstbit=7 ),         
        # 0DPHI10-XE40-LATEs1-MU10all # LATEMU 10s1
        
        # module: 1, FPGA: 1        
        TopoOutput( algoname='KF-XE-AJall',                                 module=1, fpga=1, clock=0, firstbit=0 ), # 6 bits        
        TopoOutput( algoname='0MATCH-4AJ20.ETA31-4AJj15.ETA31',             module=1, fpga=1, clock=0, firstbit=6 ),
        TopoOutput( algoname='HT190-AJ15all.ETA21',                         module=1, fpga=1, clock=0, firstbit=7 ),
        TopoOutput( algoname='HT150-AJ20all.ETA31',                         module=1, fpga=1, clock=0, firstbit=8 ),
        TopoOutput( algoname='HT150-AJj15all.ETA49',                        module=1, fpga=1, clock=0, firstbit=9 ),
        TopoOutput( algoname='HT20-AJj15all.ETA49',                         module=1, fpga=1, clock=0, firstbit=10 ),
        TopoOutput( algoname='05RATIO-XE0-HT0-AJj15all.ETA49',              module=1, fpga=1, clock=0, firstbit=13 ), 
        TopoOutput( algoname='08RATIO-XE0-HT0-AJj15all.ETA49',              module=1, fpga=1, clock=0, firstbit=14 ), 
        TopoOutput( algoname='90RATIO2-XE0-HT0-AJj15all.ETA49',             module=1, fpga=1, clock=0, firstbit=15 ),
        TopoOutput( algoname='250RATIO2-XE0-HT0-AJj15all.ETA49',            module=1, fpga=1, clock=1, firstbit=0 ),
        TopoOutput( algoname='10MINDPHI-J20ab-XE50',                        module=1, fpga=1, clock=1, firstbit=1 ), # -> FPGA 4      
        TopoOutput( algoname='0DR28-TAU20abi-TAU12abi',                     module=1, fpga=1, clock=1, firstbit=2 ), # M9 # -> former FPGA 2        
        TopoOutput( algoname='1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi',       module=1, fpga=1, clock=1, firstbit=3 ),  # -> former FPGA 2
        TopoOutput( algoname='1DISAMB-TAU12abi-J25ab',                      module=1, fpga=1, clock=1, firstbit=4 ), #  -> former FPGA 2
        TopoOutput( algoname='0DR10-MU10ab-MU6ab',                          module=1, fpga=1, clock=1, firstbit=5 ), # M9 
        TopoOutput( algoname='2DR99-2MU4ab',                                module=1, fpga=1, clock=1, firstbit=6 ), # M9 -> now FPGA 4
        TopoOutput( algoname='2DR15-2MU4ab',                                module=1, fpga=1, clock=1, firstbit=7 ), # M9 -> now FPGA 4 
        TopoOutput( algoname='2DR15-2MU6ab',                                module=1, fpga=1, clock=1, firstbit=8 ), # M9 -> now FPGA 4
        TopoOutput( algoname='2DR15-MU6ab-MU4ab',                           module=1, fpga=1, clock=1, firstbit=9 ), # M9 -> now FPGA 4
        TopoOutput( algoname='2DR15-ONEBARREL-MU6ab-MU4ab',                 module=1, fpga=1, clock=1, firstbit=10 ), #    -> now FPGA 4
        TopoOutput( algoname='2INVM999-2MU4ab',                             module=1, fpga=1, clock=1, firstbit=11 ), # M9 -> now FPGA 4
        TopoOutput( algoname='2INVM999-MU6ab-MU4ab',                        module=1, fpga=1, clock=1, firstbit=12 ), # M9 -> now FPGA 4
        TopoOutput( algoname='2INVM999-2MU6ab',                             module=1, fpga=1, clock=1, firstbit=13 ), # M9 -> now FPGA 4
        TopoOutput( algoname='4INVM8-2MU4ab',                               module=1, fpga=1, clock=1, firstbit=14 ), # M9 -> now FPGA 4
        TopoOutput( algoname='4INVM8-MU6ab-MU4ab',                          module=1, fpga=1, clock=1, firstbit=15 ), # M9 -> now FPGA 4

        ]

