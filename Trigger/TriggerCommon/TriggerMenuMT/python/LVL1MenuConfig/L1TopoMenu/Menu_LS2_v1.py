# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def defineMenu():
    """
    Defines the following LlTopoFlags:
    
    algos   .... list of all algo names in the menu 

    """
    
 
    from ..L1Topo.L1TopoFlags import L1TopoFlags
    from ..L1Topo.TopoOutput import TopoOutput

    
    L1TopoFlags.algos = [
    
        
        # module: 0, FPGA: 0
        TopoOutput( algoname='0DR25-TAU20abi-TAU12abi',                     module=0, fpga=0, clock=0, firstbit=0 ),
        TopoOutput( algoname='0DR28-TAU20abi-TAU12abi',                     module=0, fpga=0, clock=0, firstbit=1 ),
        TopoOutput( algoname='2DISAMB-J25ab-0DR25-TAU20abi-TAU12abi',       module=0, fpga=0, clock=0, firstbit=2 ),
        TopoOutput( algoname='2DISAMB-J25ab-0DR28-TAU20abi-TAU12abi',       module=0, fpga=0, clock=0, firstbit=3 ),
        TopoOutput( algoname='INVM_DPHI_NFF',                               module=0, fpga=0, clock=0, firstbit=4 ), # 4 bits
        TopoOutput( algoname='0INVM10-3MU4ab',                              module=0, fpga=0, clock=0, firstbit=9 ),


    


        # module: 0, FPGA: 1
        TopoOutput( algoname='KF-XE-AJall',                                 module=0, fpga=1, clock=0, firstbit=0 ), # 6 bits
        TopoOutput( algoname='ZAFB_DPHI',                                   module=0, fpga=1, clock=0, firstbit=6 ), # 2 bits
        TopoOutput( algoname='CEP_CJ',                                      module=0, fpga=1, clock=0, firstbit=8 ), # 2 bits
        TopoOutput( algoname='INVM_EMs6',                                   module=0, fpga=1, clock=0, firstbit=10 ), # 3bits to 2 bits
        TopoOutput( algoname='27DPHI32-EMs1-EMs6',                          module=0, fpga=1, clock=0, firstbit=12 ),


        # module: 1, FPGA: 0
        TopoOutput( algoname='HT190-J15s5.ETA21',                           module=1, fpga=0, clock=0, firstbit=0 ),
        TopoOutput( algoname='0DR03-EM7ab-CJ15ab',                          module=1, fpga=0, clock=0, firstbit=1 ),
        TopoOutput( algoname='INVM_NFF',                                    module=1, fpga=0, clock=0, firstbit=2 ), # 4 bits to 1 bit
        TopoOutput( algoname='INVM_AJ_HighMass',                            module=1, fpga=0, clock=0, firstbit=3 ), # 4bits to 1 bit
        TopoOutput( algoname='INVM_AJ_LowMass',                             module=1, fpga=0, clock=0, firstbit=4 ), # 4 bits to 1 bit
        TopoOutput( algoname='HT150-J20s5.ETA31',                           module=1, fpga=0, clock=0, firstbit=5 ),
        TopoOutput( algoname='400INVM9999-AJ30s6.ETA31-AJ20s6.31ETA49',     module=1, fpga=0, clock=0, firstbit=6 ),
        TopoOutput( algoname='10MINDPHI-J20s2-XE30',                        module=1, fpga=0, clock=0, firstbit=7 ),
        TopoOutput( algoname='10MINDPHI-J20s2-XE50',                        module=1, fpga=0, clock=0, firstbit=8 ),
        TopoOutput( algoname='SC111-CJ15ab.ETA26',                          module=1, fpga=0, clock=0, firstbit=9 ),
        TopoOutput( algoname='0DETA20-J50s1-Js2',                           module=1, fpga=0, clock=0, firstbit=10 ),
        TopoOutput( algoname='2DISAMB-J25ab-0DR28-EM15his2-TAU12abi',       module=1, fpga=0, clock=0, firstbit=11 ),
        TopoOutput( algoname='0INVM9-EM7ab-EMab',                           module=1, fpga=0, clock=0, firstbit=12 ),
        TopoOutput( algoname='2DISAMB-TAU12abi-J25ab',                      module=1, fpga=0, clock=0, firstbit=13 ),
        TopoOutput( algoname='0DR04-MU4ab-CJ15ab',                          module=1, fpga=0, clock=0, firstbit=14 ),
        TopoOutput( algoname='0DR04-MU6ab-CJ20ab',                          module=1, fpga=0, clock=0, firstbit=15 ),

        # module: 1, FPGA: 1

        TopoOutput( algoname='DISAMB-30INVM-EM20his2-TAU12ab',              module=1, fpga=1, clock=0, firstbit=0 ),
        TopoOutput( algoname='100RATIO-0MATCH-TAU30si2-EMall',              module=1, fpga=1, clock=0, firstbit=1 ),
        TopoOutput( algoname='NOT-0MATCH-TAU30si1-EMall',                   module=1, fpga=1, clock=0, firstbit=2 ),
        TopoOutput( algoname='LATE-MU10s1',                                 module=1, fpga=1, clock=0, firstbit=3 ),
        TopoOutput( algoname='MULT-CMU4ab',                                 module=1, fpga=1, clock=0, firstbit=4 ),#2bits
        TopoOutput( algoname='MULT-CMU6ab',                                 module=1, fpga=1, clock=0, firstbit=6 ),#2bits        
        TopoOutput( algoname='0DR15-2MU6ab',                                module=1, fpga=1, clock=0, firstbit=8 ),
#        TopoOutput( algoname='8INVM15-0DR22-2MU6ab',                        module=1, fpga=1, clock=0, firstbit=9 ),

#        TopoOutput( algoname='8INVM15-0DR22-MU6ab-MU4ab',                   module=1, fpga=1, clock=1, firstbit=0 ),
#        TopoOutput( algoname='2INVM9-0DR15-MU6ab-MU4ab',                    module=1, fpga=1, clock=1, firstbit=1 ),
#        TopoOutput( algoname='2INVM9-0DR15-2MU4ab',                         module=1, fpga=1, clock=1, firstbit=2 ),
#        TopoOutput( algoname='2INVM9-2DR15-2MU6ab',                         module=1, fpga=1, clock=1, firstbit=3 ),
        TopoOutput( algoname='5DETA99-5DPHI99-2MU4ab',                      module=1, fpga=1, clock=1, firstbit=0 ),
        TopoOutput( algoname='5DETA99-5DPHI99-MU6ab-MU4ab',                 module=1, fpga=1, clock=1, firstbit=1 ), 
        TopoOutput( algoname='5DETA99-5DPHI99-2MU6ab',                      module=1, fpga=1, clock=1, firstbit=2 ),
        TopoOutput( algoname='0DETA04-EM8abi-MU10ab',                       module=1, fpga=1, clock=1, firstbit=3 ),
        TopoOutput( algoname='0DPHI03-EM8abi-MU10ab',                       module=1, fpga=1, clock=1, firstbit=4 ),
        TopoOutput( algoname='0DETA04-EM15abi-MUab',                        module=1, fpga=1, clock=1, firstbit=5 ),
        TopoOutput( algoname='0DPHI03-EM15abi-MUab',                        module=1, fpga=1, clock=1, firstbit=6 ),
        TopoOutput( algoname='0INVM70-27DPHI32-EM10his1-EM10his6',          module=1, fpga=1, clock=1, firstbit=7 ),
        TopoOutput( algoname='0INVM70-27DPHI32-EM12his1-EM12his6',          module=1, fpga=1, clock=1, firstbit=8 ),
#        TopoOutput( algoname='0INVM10-0DR15-EM8abi-MU10ab',                 module=1, fpga=1, clock=1, firstbit=13 ),
#        TopoOutput( algoname='0INVM10-0DR15-EM12abi-MU6ab',                 module=1, fpga=1, clock=1, firstbit=14 ),
        TopoOutput( algoname='8INVM15-2CMU4ab',                             module=1, fpga=1, clock=1, firstbit=9 ),

    ]      
        

