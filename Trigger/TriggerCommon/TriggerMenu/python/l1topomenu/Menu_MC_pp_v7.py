# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def defineMenu():
    """
    Defines the following LlTopoFlags:
    
    algos   .... list of all algo names in the menu 

    """
    
    from TriggerMenu.l1topo.L1TopoFlags import L1TopoFlags
    from TriggerMenu.l1topo.TopoOutput import TopoOutput
    from TriggerJobOpts.TriggerFlags import TriggerFlags

    # if this flag is True for mc v7: produce xml with full v7 menu
    # if this flag is False for physics v7: produce xml with v7 new items removed, so to match with FW 
    mcv7_flag = True

    if 'Physics_pp_v7' in TriggerFlags.triggerMenuSetup() or 'Physics_pp_v6' in TriggerFlags.triggerMenuSetup() or 'HI' in TriggerFlags.triggerMenuSetup():
        mcv7_flag = False

    newalgos_onlyinv6 = [
#          TopoOutput( algoname='15MINDPHI-EM12s6-XE0',                        module=0, fpga=0, clock=1, firstbit=14 ),#v6n        
#          TopoOutput( algoname='15MINDPHI-EM15s6-XE0',                        module=0, fpga=0, clock=1, firstbit=15 ),#v6n
#          TopoOutput( algoname='35MT-EM12s6-XE0',                             module=0, fpga=0, clock=1, firstbit=13 ),#v6n
#          TopoOutput( algoname='DISAMB-0DR28-EM15his2-TAU12abi',              module=0, fpga=1, clock=0, firstbit=15 ),#v6n
#          TopoOutput( algoname='LATE-MU10s1',                                  module=1, fpga=0, clock=1, firstbit=10 ),#v6n
#          TopoOutput( algoname='SC111-CJ15ab.ETA26',                          module=1, fpga=1, clock=0, firstbit=11 ),#v6n
#          TopoOutput( algoname='SC85-CJ15ab.ETA26',                           module=1, fpga=1, clock=0, firstbit=12 ),#v6n
#          TopoOutput( algoname='10MINDPHI-AJ20s2-XE50',                       module=1, fpga=0, clock=1, firstbit=9 ),#v6n

#          TopoOutput( algoname='0DR22-2MU6ab',                                module=1, fpga=0, clock=0, firstbit=10 ),#v6diff
#          TopoOutput( algoname='7INVM15-2MU4ab',                              module=0, fpga=1, clock=0, firstbit=5 ),#v6diff 
#          TopoOutput( algoname='0DR22-MU6ab-MU4ab',                           module=1, fpga=1, clock=1, firstbit=10 ), #v6diff
 #         TopoOutput( algoname='0DR15-2MU4ab',                                module=1, fpga=0, clock=1, firstbit=0 ), #v6diff
  #        TopoOutput( algoname='0DR24-2MU4ab',                                module=1, fpga=0, clock=1, firstbit=2 ),#v6diff
#          TopoOutput( algoname='INVM_EMs6',                                   module=0, fpga=0, clock=0, firstbit=11 ), # 3bits
#          TopoOutput( algoname='100RATIO-0MATCH-TAU30si2-EMall',              module=0, fpga=0, clock=1, firstbit=7 ),

#          TopoOutput( algoname='2INVM9-2MU4ab',                               module=0, fpga=1, clock=1, firstbit=2 ),  #v6 new
#          TopoOutput( algoname='2INVM9-MU6ab-MU4ab',                          module=0, fpga=1, clock=1, firstbit=3 ),  #v6 new
#          TopoOutput( algoname='0DR25-TAU20abi-TAU12abi',                     module=0, fpga=1, clock=0, firstbit=14 ), #v6 new
#          TopoOutput( algoname='1DISAMB-J25ab-0DR25-TAU20abi-TAU12abi',       module=1, fpga=0, clock=1, firstbit=8 ), #v6 new 
#          TopoOutput( algoname='0DR15-2MU6ab',                                module=1, fpga=0, clock=1, firstbit=11 ), #v6 new
#          TopoOutput( algoname='10MINDPHI-J20s2-XE30',                        module=1, fpga=0, clock=1, firstbit=12 ),  #v6 new
#          TopoOutput( algoname='DISAMB-30INVM-EM20his2-TAU12ab',              module=0, fpga=1, clock=1, firstbit=0 ),  #v6 new
#          TopoOutput( algoname='400INVM9999-AJ30s6.ETA31-AJ20s6.31ETA49',     module=0, fpga=1, clock=1, firstbit=1 ),  #v6 new
        ]

    newalgos_onlyinv5 = [
#        TopoOutput( algoname='2DR15-CMU6ab-CMU4ab',                         module=1, fpga=0, clock=0, firstbit=10 ),#v6diff
#        TopoOutput( algoname='4INVM8-ONEBARREL-MU6ab-MU4ab',                module=0, fpga=1, clock=0, firstbit=5 ),#v6diff 
#        TopoOutput( algoname='2DR15-ONEBARREL-MU6ab-MU4ab',                 module=1, fpga=1, clock=1, firstbit=10 ), #v6diff
#        TopoOutput( algoname='2INVM999-CMU6ab-CMU4ab',                      module=1, fpga=0, clock=1, firstbit=0 ), #v6diff
#        TopoOutput( algoname='4INVM8-CMU6ab-CMU4ab',                        module=1, fpga=0, clock=1, firstbit=2 ),#v6diff
#        TopoOutput( algoname='INVM_EMall',                                  module=0, fpga=0, clock=0, firstbit=11 ), # 3bits
#        TopoOutput( algoname='210RATIO-0MATCH-TAU30si2-EMall',              module=0, fpga=0, clock=1, firstbit=7 ),
    ]
    
    L1TopoFlags.algos = [
        
        # module: 0, FPGA: 0
        TopoOutput( algoname='INVM_AJ_HighMass',                            module=0, fpga=0, clock=0, firstbit=0 ), # need 4bits 
        TopoOutput( algoname='INVM_AJ_LowMass',                             module=0, fpga=0, clock=0, firstbit=4 ), # 4 bits 
        TopoOutput( algoname='INVM_NFF',                                    module=0, fpga=1, clock=1, firstbit=12 ), # 4 bits 
        TopoOutput( algoname='63DETA127-FJ20s1-FJ20s2',                     module=1, fpga=1, clock=0, firstbit=14 ), # 4 bits 
        TopoOutput( algoname='0DETA20-J50s1-Js2',                           module=1, fpga=1, clock=0, firstbit=12 ), # 4 bits 
        TopoOutput( algoname='27DPHI31-EMs1-EMs6',                          module=0, fpga=0, clock=1, firstbit=12 ), # 4 bits 
        TopoOutput( algoname='HT150-J20s5.ETA31',                           module=0, fpga=0, clock=0, firstbit=9 ),
        TopoOutput( algoname='HT190-J15s5.ETA21',                           module=0, fpga=0, clock=0, firstbit=10 ),
        TopoOutput( algoname='05MINDPHI-EM12s6-XE0',                        module=0, fpga=0, clock=0, firstbit=14 ),#v6
#        TopoOutput( algoname='10MINDPHI-EM12s6-XE0',                        module=0, fpga=0, clock=0, firstbit=15 ),#v6
        TopoOutput( algoname='05MINDPHI-EM15s6-XE0',                        module=0, fpga=0, clock=1, firstbit=0 ),
        TopoOutput( algoname='25MT-EM12s6-XE0',                             module=0, fpga=0, clock=1, firstbit=1 ),#v6
#        TopoOutput( algoname='30MT-EM12s6-XE0',                             module=0, fpga=0, clock=1, firstbit=2 ),#v6
        TopoOutput( algoname='35MT-EM15s6-XE0',                             module=0, fpga=0, clock=1, firstbit=3 ),
#        TopoOutput( algoname='0DETA10-Js1-Js2',                             module=0, fpga=0, clock=1, firstbit=4 ),
#        TopoOutput( algoname='10MINDPHI-Js2-XE50',                          module=0, fpga=0, clock=1, firstbit=5 ),
        TopoOutput( algoname='10MINDPHI-J20s2-XE50',                        module=0, fpga=0, clock=1, firstbit=6 ),
        TopoOutput( algoname='100RATIO-0MATCH-TAU30si2-EMall',              module=0, fpga=0, clock=1, firstbit=7 ),
        TopoOutput( algoname='NOT-0MATCH-TAU30si1-EMall',                   module=0, fpga=0, clock=1, firstbit=8 ), 
        TopoOutput( algoname='LAR-EM20shi1',                                module=0, fpga=0, clock=1, firstbit=9 ), 
        TopoOutput( algoname='LAR-J100s1',                                  module=0, fpga=0, clock=1, firstbit=10 ),
        TopoOutput( algoname='ZEE-EM20shi2',                                module=0, fpga=0, clock=1, firstbit=2 ),
        TopoOutput( algoname='FTK-EM20s1',                                  module=1, fpga=0, clock=1, firstbit=13 ),  
        TopoOutput( algoname='FTK-J100s1',                                  module=1, fpga=0, clock=0, firstbit=10 ),  
        TopoOutput( algoname='FTK-MU20s1',                                  module=1, fpga=0, clock=0, firstbit=15 ),  
        TopoOutput( algoname='NOT-02MATCH-EM10s1-AJj15all.ETA49',           module=0, fpga=0, clock=1, firstbit=11 ), 
#        TopoOutput( algoname='05RATIO-XE0-SUM0-EM10s1-HT0-AJj15all.ETA49',  module=0, fpga=0, clock=1, firstbit=12 ), 
        TopoOutput( algoname='15MINDPHI-EM12s6-XE0',                        module=0, fpga=0, clock=1, firstbit=14 ),#v6n        
        TopoOutput( algoname='15MINDPHI-EM15s6-XE0',                        module=0, fpga=0, clock=1, firstbit=15 ),#v6n
        TopoOutput( algoname='35MT-EM12s6-XE0',                             module=0, fpga=0, clock=1, firstbit=13 ),#v6n
        
        # module: 0, FPGA: 1
#        TopoOutput( algoname='1DISAMB-TAU20ab-J20ab',                       module=0, fpga=1, clock=0, firstbit=0 ), 
#        TopoOutput( algoname='DISAMB-EM15abhi-TAU12abi',                    module=0, fpga=1, clock=0, firstbit=1 ),
        TopoOutput( algoname='1DISAMB-EM15his2-TAU12abi-J25ab',             module=0, fpga=1, clock=0, firstbit=2 ), 
        TopoOutput( algoname='1DISAMB-J25ab-0DR28-EM15his2-TAU12abi',       module=0, fpga=1, clock=0, firstbit=3 ),
#        TopoOutput( algoname='4INVM8-2MU6ab',                               module=0, fpga=1, clock=0, firstbit=4 ), 
        TopoOutput( algoname='2INVM9-2MU6ab',                               module=0, fpga=1, clock=0, firstbit=4 ),#v6
#        TopoOutput( algoname='2INVM999-ONEBARREL-MU6ab-MU4ab',              module=0, fpga=1, clock=0, firstbit=5 ), 
#        TopoOutput( algoname='4INVM8-ONEBARREL-MU6ab-MU4ab',                module=0, fpga=1, clock=0, firstbit=6 ),
        TopoOutput( algoname='2INVM8-ONEBARREL-MU6ab-MU4ab',                module=0, fpga=1, clock=0, firstbit=6 ),#v6
#        TopoOutput( algoname='5DETA99-5DPHI99-2MU4ab',                      module=0, fpga=1, clock=0, firstbit=7 ), 
        TopoOutput( algoname='5DETA99-5DPHI99-MU6ab-MU4ab',                 module=0, fpga=1, clock=0, firstbit=8 ), 
        TopoOutput( algoname='5DETA99-5DPHI99-2MU6ab',                      module=0, fpga=1, clock=0, firstbit=9 ), 
        TopoOutput( algoname='1DISAMB-TAU20abi-TAU12abi-J25ab',             module=0, fpga=1, clock=0, firstbit=10 ),
        TopoOutput( algoname='0DR28-MU10ab-TAU12abi',                       module=0, fpga=1, clock=0, firstbit=11 ),
        TopoOutput( algoname='0DETA20-0DPHI20-TAU20abi-TAU12abi',           module=0, fpga=1, clock=0, firstbit=12 ),
#        TopoOutput( algoname='DISAMB-EM15abhi-TAU40ab',                     module=0, fpga=1, clock=0, firstbit=13 ), 
        TopoOutput( algoname='DISAMB-0DR28-EM15his2-TAU12abi',              module=0, fpga=1, clock=0, firstbit=15 ),#v6n

        TopoOutput( algoname='DISAMB-30INVM-EM20his2-TAU12ab',              module=0, fpga=1, clock=1, firstbit=0 ),  #v6 new
        TopoOutput( algoname='400INVM9999-AJ30s6.ETA31-AJ20s6.31ETA49',     module=0, fpga=0, clock=0, firstbit=15 ),  #v6 new

        TopoOutput( algoname='0DR22-2MU6ab',                                module=0, fpga=1, clock=1, firstbit=4 ),#v6
        TopoOutput( algoname='7INVM15-2MU4ab',                              module=0, fpga=1, clock=1, firstbit=5 ),#v6
        TopoOutput( algoname='0DR22-MU6ab-MU4ab',                           module=0, fpga=1, clock=1, firstbit=6 ),#v6
        TopoOutput( algoname='0DR15-2MU4ab',                                module=0, fpga=1, clock=1, firstbit=7 ),#v6
        TopoOutput( algoname='0DR24-2MU4ab',                                module=0, fpga=1, clock=1, firstbit=8 ),#v6
        TopoOutput( algoname='0DR15-2MU6ab',                                module=0, fpga=1, clock=1, firstbit=9 ), #v6 new
        TopoOutput( algoname='2INVM9-2MU4ab',                               module=0, fpga=1, clock=1, firstbit=10 ),  #v6 new
        TopoOutput( algoname='2INVM9-MU6ab-MU4ab',                          module=0, fpga=1, clock=1, firstbit=11 ),  #v6 new       
        TopoOutput( algoname='INVM_EMs6',                                   module=0, fpga=0, clock=0, firstbit=11 ), # 3bits
 
        # module: 1, FPGA: 0        
        TopoOutput( algoname='05MINDPHI-AJj10s6-XE0',                       module=1, fpga=0, clock=0, firstbit=0 ),#v6
        TopoOutput( algoname='10MINDPHI-AJj10s6-XE0',                       module=1, fpga=0, clock=0, firstbit=1 ),#v6
        TopoOutput( algoname='15MINDPHI-AJj10s6-XE0',                       module=1, fpga=0, clock=0, firstbit=2 ),#v6
        TopoOutput( algoname='0DR04-MU4ab-CJ15ab',                          module=1, fpga=0, clock=0, firstbit=3 ), 
        TopoOutput( algoname='0DR04-MU4ab-CJ20ab',                          module=1, fpga=0, clock=0, firstbit=4 ),
        TopoOutput( algoname='0DR04-MU4ab-CJ30ab',                          module=1, fpga=0, clock=0, firstbit=5 ),
        TopoOutput( algoname='0DR04-MU6ab-CJ20ab',                          module=1, fpga=0, clock=0, firstbit=6 ),
        TopoOutput( algoname='0DR04-MU6ab-CJ25ab',                          module=1, fpga=0, clock=0, firstbit=7 ),
        TopoOutput( algoname='10MINDPHI-CJ20ab-XE50',                       module=1, fpga=0, clock=0, firstbit=8 ),
#        TopoOutput( algoname='2DR15-2CMU4ab',                               module=1, fpga=0, clock=0, firstbit=9 ),
        TopoOutput( algoname='0DR24-2CMU4ab',                               module=1, fpga=0, clock=0, firstbit=9 ),#v6
#        TopoOutput( algoname='2DR15-CMU6ab-CMU4ab',                         module=1, fpga=0, clock=0, firstbit=10 ),
        TopoOutput( algoname='MULT-CMU4ab',                                 module=1, fpga=0, clock=0, firstbit=11 ), # 2bits
        TopoOutput( algoname='MULT-CMU6ab',                                 module=1, fpga=0, clock=0, firstbit=13 ), # 2bits
#        TopoOutput( algoname='2INVM999-2CMU4ab',                            module=1, fpga=0, clock=0, firstbit=15 ), 
#        TopoOutput( algoname='1INVM19-2CMU4ab',                             module=1, fpga=0, clock=0, firstbit=15 ),#v6
#        TopoOutput( algoname='2INVM999-CMU6ab-CMU4ab',                      module=1, fpga=0, clock=1, firstbit=0 ), 
#        TopoOutput( algoname='4INVM8-2CMU4ab',                              module=1, fpga=0, clock=1, firstbit=1 ),
#        TopoOutput( algoname='2INVM8-2CMU4ab',                              module=1, fpga=0, clock=1, firstbit=1 ),#v6
#        TopoOutput( algoname='4INVM8-CMU6ab-CMU4ab',                        module=1, fpga=0, clock=1, firstbit=2 ),
#        TopoOutput( algoname='0DETA04-0DPHI03-EM8abi-MU10ab',               module=1, fpga=0, clock=1, firstbit=3 ),
         TopoOutput( algoname='0DETA04-EM8abi-MU10ab',                       module=1, fpga=0, clock=1, firstbit=3 ), #ATR-14282
         TopoOutput( algoname='0DPHI03-EM8abi-MU10ab',                       module=1, fpga=0, clock=1, firstbit=6 ),

#        TopoOutput( algoname='0DETA04-0DPHI03-EM15abi-MUab',                module=1, fpga=0, clock=1, firstbit=4 ),
         TopoOutput( algoname='0DETA04-EM15abi-MUab',                       module=1, fpga=0, clock=1, firstbit=4 ), #ATR-14282
         TopoOutput( algoname='0DPHI03-EM15abi-MUab',                       module=1, fpga=0, clock=1, firstbit=8 ),

#        TopoOutput( algoname='2DR15-CMU4ab-MU4ab',                          module=1, fpga=0, clock=1, firstbit=5 ),
        TopoOutput( algoname='0DR24-CMU4ab-MU4ab',                          module=1, fpga=0, clock=1, firstbit=5 ),#v6
#        TopoOutput( algoname='2INVM999-CMU4ab-MU4ab',                       module=1, fpga=0, clock=1, firstbit=6 ), 
#        TopoOutput( algoname='1INVM19-CMU4ab-MU4ab',                        module=1, fpga=0, clock=1, firstbit=6 ),#v6
#        TopoOutput( algoname='4INVM8-CMU4ab-MU4ab',                         module=1, fpga=0, clock=1, firstbit=7 ),         
        TopoOutput( algoname='2INVM8-CMU4ab-MU4ab',                         module=1, fpga=0, clock=1, firstbit=7 ),#v6

        TopoOutput( algoname='0DR25-TAU20abi-TAU12abi',                     module=1, fpga=1, clock=1, firstbit=10 ), #v6 new
        TopoOutput( algoname='10MINDPHI-AJ20s2-XE50',                       module=1, fpga=0, clock=1, firstbit=9 ),#v6n
        TopoOutput( algoname='LATE-MU10s1',                                  module=1, fpga=0, clock=1, firstbit=10 ),#v6n
        TopoOutput( algoname='1DISAMB-J25ab-0DR25-TAU20abi-TAU12abi',       module=1, fpga=1, clock=1, firstbit=11 ), #v6 new 
#          TopoOutput( algoname='0DR15-2MU6ab',                                module=1, fpga=0, clock=1, firstbit=11 ), #v6 new
        TopoOutput( algoname='10MINDPHI-J20s2-XE30',                        module=0, fpga=0, clock=1, firstbit=5 ),  #v6 new


        # module: 1, FPGA: 1        
        TopoOutput( algoname='KF-XE-AJall',                                 module=1, fpga=1, clock=0, firstbit=0 ), # 6 bits        
        TopoOutput( algoname='0MATCH-4AJ20.ETA31-4AJj15.ETA31',             module=1, fpga=1, clock=0, firstbit=6 ),
        TopoOutput( algoname='HT190-AJ15all.ETA21',                         module=1, fpga=1, clock=0, firstbit=7 ),
        TopoOutput( algoname='HT150-AJ20all.ETA31',                         module=1, fpga=1, clock=0, firstbit=8 ),
        TopoOutput( algoname='HT150-AJj15all.ETA49',                        module=1, fpga=1, clock=0, firstbit=9 ),
        TopoOutput( algoname='HT20-AJj15all.ETA49',                         module=1, fpga=1, clock=0, firstbit=10 ),
        TopoOutput( algoname='SC111-CJ15ab.ETA26',                          module=1, fpga=0, clock=1, firstbit=11 ),#v6n
        TopoOutput( algoname='SC85-CJ15ab.ETA26',                           module=1, fpga=0, clock=1, firstbit=12 ),#v6n
        TopoOutput( algoname='05RATIO-XE0-HT0-AJj15all.ETA49',              module=1, fpga=1, clock=0, firstbit=13 ), 
#        TopoOutput( algoname='08RATIO-XE0-HT0-AJj15all.ETA49',              module=1, fpga=1, clock=0, firstbit=14 ), 
        TopoOutput( algoname='90RATIO2-XE0-HT0-AJj15all.ETA49',             module=1, fpga=1, clock=0, firstbit=15 ),
        TopoOutput( algoname='250RATIO2-XE0-HT0-AJj15all.ETA49',            module=1, fpga=1, clock=1, firstbit=0 ),
        TopoOutput( algoname='10MINDPHI-J20ab-XE50',                        module=1, fpga=1, clock=1, firstbit=1 ), 
        TopoOutput( algoname='0DR28-TAU20abi-TAU12abi',                     module=1, fpga=1, clock=1, firstbit=2 ),
        TopoOutput( algoname='1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi',       module=1, fpga=1, clock=1, firstbit=3 ), 
        TopoOutput( algoname='1DISAMB-TAU12abi-J25ab',                      module=1, fpga=1, clock=1, firstbit=4 ), 
        TopoOutput( algoname='0DR10-MU10ab-MU6ab',                          module=1, fpga=1, clock=1, firstbit=5 ),  
        TopoOutput( algoname='2DR99-2MU4ab',                                module=1, fpga=1, clock=1, firstbit=6 ), 
#        TopoOutput( algoname='2DR15-2MU4ab',                                module=1, fpga=1, clock=1, firstbit=7 ),
        TopoOutput( algoname='0DR34-2MU4ab',                                module=1, fpga=1, clock=1, firstbit=7 ), #v6
        TopoOutput( algoname='2DR15-2MU6ab',                                module=1, fpga=1, clock=1, firstbit=8 ), 
#        TopoOutput( algoname='2DR15-MU6ab-MU4ab',                           module=1, fpga=1, clock=1, firstbit=9 ), 
        TopoOutput( algoname='0DR15-MU6ab-MU4ab',                           module=1, fpga=1, clock=1, firstbit=9 ), #v6
#        TopoOutput( algoname='2DR15-ONEBARREL-MU6ab-MU4ab',                 module=1, fpga=1, clock=1, firstbit=10 ),
#        TopoOutput( algoname='2INVM999-2MU4ab',                             module=1, fpga=1, clock=1, firstbit=11 ),
#        TopoOutput( algoname='1INVM19-2MU4ab',                              module=1, fpga=1, clock=1, firstbit=11 ), #v6
#        TopoOutput( algoname='2INVM999-MU6ab-MU4ab',                        module=1, fpga=1, clock=1, firstbit=12 ),
        TopoOutput( algoname='8INVM15-MU6ab-MU4ab',                         module=1, fpga=1, clock=1, firstbit=12 ), #v6
#        TopoOutput( algoname='2INVM999-2MU6ab',                             module=1, fpga=1, clock=1, firstbit=13 ),
        TopoOutput( algoname='8INVM15-2MU6ab',                              module=1, fpga=1, clock=1, firstbit=13 ), #v6
        TopoOutput( algoname='0INVM9-EM7ab-EMab',                           module=0, fpga=0, clock=0, firstbit=8 ), #v6
        TopoOutput( algoname='0DR04-EM7ab-CJ15ab',                          module=0, fpga=0, clock=1, firstbit=4 ), #v6
#        TopoOutput( algoname='4INVM8-2MU4ab',                               module=1, fpga=1, clock=1, firstbit=14 ), 
        TopoOutput( algoname='2INVM8-2MU4ab',                               module=1, fpga=1, clock=1, firstbit=14 ), #v6
# ATR-15197        TopoOutput( algoname='2INVM9-2MU4ab',                               module=1, fpga=1, clock=1, firstbit=14 ), #v6
#        TopoOutput( algoname='4INVM8-MU6ab-MU4ab',                          module=1, fpga=1, clock=1, firstbit=15 ), 
        TopoOutput( algoname='2INVM8-MU6ab-MU4ab',                          module=1, fpga=1, clock=1, firstbit=15 ), #v6
# ATR-15197        TopoOutput( algoname='2INVM9-MU6ab-MU4ab',                          module=1, fpga=1, clock=1, firstbit=15 ), #v6

        ]


    if mcv7_flag:
        L1TopoFlags.algos +=  newalgos_onlyinv6
    else:
        L1TopoFlags.algos +=  newalgos_onlyinv5 
