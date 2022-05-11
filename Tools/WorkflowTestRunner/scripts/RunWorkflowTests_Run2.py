#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from sys import exit

from WorkflowTestRunner.ScriptUtils import setup_logger, setup_parser, get_test_setup, get_standard_performance_checks, \
    run_tests, run_checks, run_summary
from WorkflowTestRunner.StandardTests import QTest, SimulationTest, OverlayTest, DataOverlayTest, PileUpTest
from WorkflowTestRunner.Test import WorkflowRun, WorkflowType


def main():
    name = "Run2Tests"
    run = WorkflowRun.Run2

    # Setup the environment
    log = setup_logger(name)
    parser = setup_parser()
    options = parser.parse_args()
    setup = get_test_setup(name, options, log)

    # Define which tests to run
    tests_to_run = []
    if options.simulation:
        if not options.workflow or options.workflow is WorkflowType.FullSim:
            tests_to_run.append(SimulationTest("s3759", run, WorkflowType.FullSim, ["EVNTtoHITS"], setup, options.extra_args))
        if not options.workflow or options.workflow is WorkflowType.AF3:
            tests_to_run.append(SimulationTest("s3779", run, WorkflowType.AF3, ["EVNTtoHITS"], setup, options.extra_args))
    elif options.overlay:
        if not options.workflow or options.workflow is WorkflowType.MCOverlay:
            tests_to_run.append(OverlayTest("d1726", run, WorkflowType.MCOverlay, ["Overlay"], setup, options.extra_args))
        if not options.workflow or options.workflow is WorkflowType.DataOverlay:
            tests_to_run.append(DataOverlayTest("d1590", run, WorkflowType.DataOverlay, ["Overlay"], setup, options.extra_args))
    elif options.pileup:
        if setup.parallel_execution:
            log.error("Parallel execution not supported for pile-up workflow")
            exit(1)
        if not options.workflow or options.workflow is WorkflowType.PileUpPresampling:
            tests_to_run.append(PileUpTest("d1730", run, WorkflowType.PileUpPresampling, ["HITtoRDO"], setup, options.extra_args))
        if not options.workflow or options.workflow is WorkflowType.MCPileUpReco:
            tests_to_run.append(QTest("q444", run, WorkflowType.MCPileUpReco, ["RAWtoESD", "ESDtoAOD"], setup, options.extra_args))
    else:
        if not options.workflow or options.workflow is WorkflowType.MCReco:
            if "--CA" in options.extra_args:
                tests_to_run.append(QTest("q443", run, WorkflowType.MCReco, ["HITtoRDO", "RAWtoALL"], setup, options.extra_args + " --steering doRAWtoALL"))
            else:
                tests_to_run.append(QTest("q443", run, WorkflowType.MCReco, ["HITtoRDO", "RDOtoRDOTrigger", "RAWtoESD", "ESDtoAOD"], setup, options.extra_args))
        if not options.workflow or options.workflow is WorkflowType.DataReco:
            tests_to_run.append(QTest("q442", run, WorkflowType.DataReco, ["RAWtoALL"], setup, options.extra_args))

    # Define which perfomance checks to run
    performance_checks = get_standard_performance_checks(setup)

    # Define and run jobs
    run_tests(setup, tests_to_run)

    # Run post-processing checks
    all_passed = run_checks(setup, tests_to_run, performance_checks)

    # final report
    run_summary(setup, tests_to_run, all_passed)


if __name__ == "__main__":
    main()
