# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys

def main():
    """
    Main function (c++ style)
    """

    # The name of the application
    import os
    APP_NAME = os.path.basename(__file__).rstrip('.py')
    print APP_NAME
    # Set up a logger object
    import logging
    logger = logging.getLogger(APP_NAME)
    logger.setLevel(logging.INFO)
    hdlr = logging.StreamHandler(sys.stdout)
    frmt = logging.Formatter('%(name)-14s%(levelname)8s %(message)s')
    hdlr.setFormatter(frmt)
    logger.addHandler(hdlr)

    # Setup the environment
    import ROOT
    if ROOT.gROOT.Macro('$ROOTCOREDIR/scripts/load_packages.C'):
        logger.error("Couldn't load the RootCore packages")
        return 1

    if ROOT.xAOD.Init(APP_NAME).isFailure():
        logger.error('Failed to call xAOD::Init(...)')
        return 1


    from ROOT import vector

    #registry
    registry = ROOT.ToolsRegistry('ToolsRegistry')

    # level1 emulation tool
    tool_l1 = ROOT.TrigTauEmul.Level1EmulationTool("Level1EmulationTool")
    chains_to_test = vector('std::string')()
    chains_to_test.push_back("L1_TAU12")
    chains_to_test.push_back("L1_TAU12IL")
    chains_to_test.push_back("L1_TAU12IM")
    chains_to_test.push_back("L1_TAU12IT")
    chains_to_test.push_back("L1_TAU20")
    chains_to_test.push_back("L1_TAU20IL")

    if not tool_l1.setProperty('l1_chains', chains_to_test).isSuccess():
        logger.error('Failed to set the property')
        return 1

    sc = tool_l1.initialize()
    if sc.isFailure():
        logger.error('Failed to initialize the tool')
        return 1

    # TauTriggerEmulation import and setup
    tool = ROOT.TrigTauEmul.HltEmulationTool("TauTriggerEmulationTool")
    chains_to_test = vector('std::string')()
    chains_to_test.push_back("HLT_tau5_perf_ptonly_L1TAU8")
    chains_to_test.push_back("HLT_tau25_perf_ptonly")
    chains_to_test.push_back("HLT_tau25_loose1_ptonly")
    chains_to_test.push_back("HLT_tau25_medium1_ptonly")
    chains_to_test.push_back("HLT_tau25_tight1_ptonly")

    if not tool.setProperty('hlt_chains', chains_to_test).isSuccess():
        logger.error('Failed to set the property')
        return 1

    sc = tool.initialize()
    if sc.isFailure():
        logger.error('Failed to initialize the tool')
        return 1

    # Return gracefully
    return 0


if __name__ == '__main__':
    sys.exit(main())


