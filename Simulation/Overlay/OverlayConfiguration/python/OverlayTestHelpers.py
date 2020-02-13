#!/usr/bin/env python
"""Overlay test helpers

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from argparse import ArgumentParser


def CommonTestArgumentParser(prog):
    """Common overlay test argument parser"""
    parser = ArgumentParser(prog=prog)
    parser.add_argument("-n", "--maxEvents",  default=3, type=int, help="The number of events to run. 0 skips execution")
    parser.add_argument("-t", "--threads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")
    parser.add_argument("-V", "--verboseAccumulators", default=False, action="store_true", help="Print full details of the AlgSequence for each accumulator")
    return parser


def postprocessAndLockFlags(configFlags, args):
    """Postprocess and lock config flags for overlay"""

    # Flags relating to multithreaded execution
    configFlags.Concurrency.NumThreads = args.threads
    if args.threads > 0:
        configFlags.Scheduler.ShowDataDeps = True
        configFlags.Scheduler.ShowDataFlow = True
        configFlags.Scheduler.ShowControlFlow = True
        configFlags.Concurrency.NumConcurrentEvents = args.threads

    configFlags.lock()


def printAndRun(accessor, configFlags, args):
    """Common debugging and execution for overlay tests"""
    # Dump config
    if args.verboseAccumulators:
        accessor.printConfig(withDetails=True)
    accessor.getService("StoreGateSvc").Dump = True
    configFlags.dump()

    # Execute and finish
    sc = accessor.run(maxEvents=args.maxEvents)

    # Dump config summary
    accessor.printConfig(withDetails=False)

    # Success should be 0
    return not sc.isSuccess()
