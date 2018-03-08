# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
from ...utility import FileParser, ProcessManager, SingleProcessThread
from functools import partial
import os

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


def multicore(process, cores):
    """! Run multiple Powheg processes, each in its own thread.

    @param process PowhegBox process.
    @param cores   Number of cores in use.

    @author James Robinson  <james.robinson@cern.ch>
    """
    # Construct random seeds - increment by 1e6 each time
    logger.info("Running in multicore mode with {} subjobs".format(cores))
    with open("pwgseeds.dat", "wb") as random_seed_list:
        for idx in range(cores):
            random_seed_list.write(str(process.random_seed + int(idx * 1e6)) + "\n")

    # Remove iseed when providing seeds from pwgseeds.dat
    FileParser("powheg.input").text_remove("^iseed")
    logger.debug("Disabling iseed variable when multiple seeds are used")

    # Construct generation function
    generation_fn = partial(__run_threaded_multicore, process.executable, cores)

    if process.powheg_version == "V1":
        __multicore_v1(generation_fn)
    elif process.powheg_version == "V2" or process.powheg_version == "RES":
        __multicore_v2(generation_fn, process.itmx1)
    else:
        raise ValueError("Powheg version {} is not supported for multicore running!".format(process.powheg_version))


@timed("multi-core generation (V1)")
def __multicore_v1(generation_fn):
    """! Run Powheg V1 generation in multicore mode."""
    generation_fn()


@timed("multi-core generation (V2)")
def __multicore_v2(generation_fn, n_xgrid_iterations):
    """! Run Powheg V2 generation in multicore mode."""
    __multicore_v2_stage_1(generation_fn, n_xgrid_iterations)
    __multicore_v2_stage_2(generation_fn)
    __multicore_v2_stage_3(generation_fn)
    __multicore_v2_stage_4(generation_fn)


@timed("multi-core generation (V2: stage 1)")
def __multicore_v2_stage_1(generation_fn, n_xgrid_iterations):
    """! Run Powheg V2 (stage 1) generation in multicore mode."""
    FileParser("powheg.input").text_replace("parallelstage.*", "parallelstage 1")
    # For stage 1, we need n_xgrid_iterations iterations
    for xgrid_iteration in range(1, n_xgrid_iterations + 1):
        FileParser("powheg.input").text_replace("xgriditeration.*", "xgriditeration {}".format(xgrid_iteration))
        generation_fn()


@timed("multi-core generation (V2: stage 2)")
def __multicore_v2_stage_2(generation_fn):
    """! Run Powheg V2 (stage 2) generation in multicore mode."""
    FileParser("powheg.input").text_replace("parallelstage.*", "parallelstage 2")
    generation_fn()


@timed("multi-core generation (V2: stage 3)")
def __multicore_v2_stage_3(generation_fn):
    """! Run Powheg V2 (stage 3) generation in multicore mode."""
    FileParser("powheg.input").text_replace("parallelstage.*", "parallelstage 3")
    generation_fn()


@timed("multi-core generation (V2: stage 4)")
def __multicore_v2_stage_4(generation_fn):
    """! Run Powheg V2 (stage 4) generation in multicore mode."""
    FileParser("powheg.input").text_replace("parallelstage.*", "parallelstage 4")
    generation_fn()


def __run_threaded_multicore(powheg_executable, cores):
    """! Run multiple Powheg processes, each in its own thread.

    @author James Robinson  <james.robinson@cern.ch>
    """
    if not os.path.isfile(powheg_executable):
        raise OSError("Powheg executable {} not found!".format(powheg_executable))
    processes = [SingleProcessThread(powheg_executable, seed_index=idx) for idx in range(1, cores + 1)]
    manager = ProcessManager(processes)
    while manager.monitor():
        pass
