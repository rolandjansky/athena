#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys

def main():
    import argparse
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    parser = argparse.ArgumentParser()
    parser.add_argument("menu", nargs="?", default='PhysicsP1_pp_run3_v1', #ConfigFlags.Trigger.triggerMenuSetup,
                        help="the menu to generate [%(default)s]")
    parser.add_argument("--bgrp", action="store_true",
                        help="generate default MC bunchgroup")
    parser.add_argument("-v", "--verbose", action="store_true",
                        help="increase output verbosity")

    args = parser.parse_args()

    # set menu
    ConfigFlags.Input.Files = []
    ConfigFlags.Trigger.triggerMenuSetup = args.menu
    ConfigFlags.lock()

    # set verbosity
    if args.verbose:
        from AthenaCommon.Logging import logging
        logging.getLogger("TriggerMenuMT").setLevel(logging.DEBUG)

    # Bunchgroup generation
    if args.bgrp:
        from TriggerMenuMT.L1.Base.Limits import Limits
        from TriggerMenuMT.L1.Base.BunchGroupSet import createDefaultBunchGroupSet
        Limits.setLimits(CTPVersion=4)
        bgs = createDefaultBunchGroupSet()
        bgs.writeJSON(outputFile = "L1BunchGroupSet.json")
    else:
        # L1 menu generation
        from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu
        generateL1Menu(ConfigFlags)

    return 0

if __name__=="__main__":
    sys.exit( main() )
        
        
