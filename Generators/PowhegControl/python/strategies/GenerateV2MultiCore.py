# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl GenerateV2MultiCore
#  Run multiple Powheg v2 processes using a thread for each one
#  - needs multiple stages
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import time
from AthenaCommon.Logging import logging
from ..utility import FileParser, ProcessHandling, RepeatingTimer

# Initialise logging handler
logger = logging.getLogger("PowhegControl")


# Run multiple Powheg processes
def generate_v2_multi_core(configurator):
    # Construct random seeds - increment by 1e6 each time
    logger.info("Running in multicore mode with {} subjobs".format(configurator.cores))
    with open("pwgseeds.dat", "wb") as random_seed_list:
        for idx in range(configurator.cores):
            random_seed_list.write(str(configurator.random_seed + int(idx * 1e6)) + "\n")

    # Remove iseed if providing seeds from pwgseeds.dat
    FileParser("powheg*.input").text_remove("^iseed")

    logger.debug("Disabling iseed variable when multiple seeds are used")

    # Set up four passes to run each parallelstage
    for idx_stage in range(1, 5):
        logger.info("Now running stage {}/4".format(idx_stage))

        # Set number of xgriditerations to itmx1
        n_xgriditerations = configurator.itmx1
        current_xgriditeration = 0

        # Inner loop to allow multiple xgrid iterations
        while True:
            # For stage 1, we need itmx1 iterations
            if idx_stage == 1:
                current_xgriditeration += 1
                FileParser("powheg.input").text_replace("xgriditeration.*", "xgriditeration {}".format(current_xgriditeration))
                logger.info("Now running xgriditeration {}/{}".format(current_xgriditeration, n_xgriditerations))

            # Run the multiprocess step and wait for output collector to terminate
            time_start = time.time()
            processes = [ProcessHandling.SingleProcessThread(configurator._powheg_executable, seed_index=idx) for idx in range(1, configurator.cores + 1)]
            manager = ProcessHandling.ProcessManager(processes)
            while manager.monitor():
                pass

            # Exit inner loop
            if idx_stage != 1 or current_xgriditeration >= n_xgriditerations:
                break

        # Increment parallelstage and repeat
        FileParser("powheg.input").text_replace("parallelstage {}".format(idx_stage), "parallelstage {}".format(idx_stage + 1))
        logger.info("Finished stage {}/4 in {}".format(idx_stage, RepeatingTimer.human_readable_time_interval(time.time() - time_start)))
