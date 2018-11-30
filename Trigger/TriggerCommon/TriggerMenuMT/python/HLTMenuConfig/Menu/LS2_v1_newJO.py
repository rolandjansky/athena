# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

def get_flag_item(chainName, L1itemforchain, groups):
    PhysicsStream = 'Main'
    return [chainName, L1itemforchain, [], [PhysicsStream], groups, -1]

def setupMenu(flags):
    """ 
    Assign chains for LS2_v1 to menu flags
    """

    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1_newJO.py' )

    #---------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep],
    #   OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    #---------------------------------------------------------------------

    #---------------------------------------------------------------------
    # if it's needed to temporary remove almost all the chains from the menu
    # be aware that it is necessary to leave at least one chain in the muon slice
    # otherwise athenaHLT will seg-fault 
    #---------------------------------------------------------------------


    flags.Trigger.menu.muons = [
        get_flag_item('mu20', 'L1_MU10', ['RATE:SingleMuon', 'BW:Muon']),
        get_flag_item('mu8', 'L1_MU6', ['RATE:SingleMuon', 'BW:Muon'])
    ]

    flags.Trigger.menu.egamma = [
        get_flag_item('HLT_e20_loose', 'L1_EM10', ['RATE:SingleElectron', 'BW:Electron'])
    ]

    flags.Trigger.menu.combined = [
        get_flag_item('e8_mu8', 'L1_EM6_MU6', ['RATE:SingleMuon', 'BW:Muon'])
    ]



if __name__ == "__main__":
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    acc = ComponentAccumulator()

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    setupMenu(ConfigFlags)

    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]

    ConfigFlags.lock()
    ConfigFlags.dump()
    
    from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT_newJO import generateMenu    
    menu = generateMenu( ConfigFlags )

    acc.merge(menu)

    f = open('newJOMenu.pkl', 'wb')
    acc.store(f)
    f.close()
