# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
from ...utility import FileParser, ProcessManager, SingleProcessThread
from functools import partial
import os
import shutil

# Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


def multicore(process):
    """! Run multiple Powheg processes, each in its own thread.

    @param process  PowhegBox process.

    @author James Robinson  <james.robinson@cern.ch>
    """
    # Construct random seeds - increment by 1e6 each time
    logger.info("Running in multicore mode with {} subjobs".format(process.cores))
    with open("pwgseeds.dat", "wb") as random_seed_list:
        for idx in range(process.cores):
            random_seed_list.write(str(process.random_seed + int(idx * 1e6)) + "\n")

    # Remove iseed when providing seeds from pwgseeds.dat
    FileParser("powheg.input").text_remove("^iseed")
    logger.debug("Disabling iseed variable when multiple seeds are used")

    # Construct generation function
    generation_fn = partial(multicore_untimed, process)

    if process.powheg_version == "V1":
        __multicore_v1(generation_fn)
    elif process.powheg_version == "V2" or process.powheg_version == "RES":
        __multicore_multistage(process, generation_fn)
    else:
        raise ValueError("Powheg version {} is not supported for multicore running!".format(process.powheg_version))

@timed("multi-core generation (V1)")
def __multicore_v1(generation_fn):
    """! Run Powheg V1 generation in multi-core mode.

    @param generation_fn  Function that can be called without argument to generate events.

    @author James Robinson  <james.robinson@cern.ch>
    """
    generation_fn()

@timed("multi-core generation (V2/RES)")
def __multicore_multistage(process, generation_fn):
    """! Run Powheg V2/RES generation in multi-core mode.

    @param process        PowhegBox process.
    @param generation_fn  Function that can be called without argument to generate events.

    @author James Robinson  <james.robinson@cern.ch>
    """
    shutil.rmtree("multistage_inputs", ignore_errors=True)
    os.mkdir("multistage_inputs")
    shutil.copy("pwgseeds.dat", "multistage_inputs/pwgseeds.dat")

    if process.stage_is_completed(1):
        logger.info("=> Skipping multi-core generation (V2/RES): stage 1 <=")
    else:
        __multicore_multistage_stage_1(generation_fn, process.itmx1)
    if process.stage_is_completed(2):
        logger.info("=> Skipping multi-core generation (V2/RES): stage 2 <=")
    else:
        __multicore_multistage_stage_2(generation_fn)
    if process.stage_is_completed(3):
        logger.info("=> Skipping multi-core generation (V2/RES): stage 3 <=")
    else:
        __multicore_multistage_stage_3(generation_fn)
    __multicore_multistage_stage_4(generation_fn)

@timed("multi-core generation (V2/RES): stage 1)")
def __multicore_multistage_stage_1(generation_fn, n_xgrid_iterations):
    """! Run Powheg V2/RES (stage 1) generation in multi-core mode.

    @param generation_fn       Function that can be called without argument to generate events.
    @param n_xgrid_iterations  Number of xgrid iterations to perform.

    @author James Robinson  <james.robinson@cern.ch>
    """
    FileParser("powheg.input").text_replace("parallelstage.*", "parallelstage 1")
    # For stage 1, we need n_xgrid_iterations iterations
    for xgrid_iteration in range(1, n_xgrid_iterations + 1):
        FileParser("powheg.input").text_replace("xgriditeration.*", "xgriditeration {}".format(xgrid_iteration))
        shutil.copy("powheg.input", "multistage_inputs/powheg.input.parallelstage{ps}.xgriditeration{xgi}".format(ps=1, xgi=xgrid_iteration))
        generation_fn()

@timed("multi-core generation (V2/RES): stage 2)")
def __multicore_multistage_stage_2(generation_fn):
    """! Run Powheg V2/RES (stage 2) generation in multi-core mode.

    @param generation_fn  Function that can be called without argument to generate events.

    @author James Robinson  <james.robinson@cern.ch>
    """
    FileParser("powheg.input").text_replace("parallelstage.*", "parallelstage 2")
    shutil.copy("powheg.input", "multistage_inputs/powheg.input.parallelstage{ps}".format(ps=2))
    generation_fn()

@timed("multi-core generation (V2/RES): stage 3)")
def __multicore_multistage_stage_3(generation_fn):
    """! Run Powheg V2/RES (stage 3) generation in multi-core mode.

    @param generation_fn  Function that can be called without argument to generate events.

    @author James Robinson  <james.robinson@cern.ch>
    """
    FileParser("powheg.input").text_replace("parallelstage.*", "parallelstage 3")
    shutil.copy("powheg.input", "multistage_inputs/powheg.input.parallelstage{ps}".format(ps=3))
    generation_fn()

@timed("multi-core generation (V2/RES): stage 4)")
def __multicore_multistage_stage_4(generation_fn):
    """! Run Powheg V2/RES (stage 4) generation in multi-core mode.

    @param generation_fn  Function that can be called without argument to generate events.

    @author James Robinson  <james.robinson@cern.ch>
    """
    FileParser("powheg.input").text_replace("parallelstage.*", "parallelstage 4")
    shutil.copy("powheg.input", "multistage_inputs/powheg.input.parallelstage{ps}".format(ps=4))
    generation_fn()

def multicore_untimed(process):
    """! Run multiple Powheg processes, each in its own thread.

    @param process  PowhegBox process.

    @author James Robinson  <james.robinson@cern.ch>
    """
    if not os.path.isfile(process.executable):
        raise OSError("Powheg executable {} not found!".format(process.executable))
    threads = [SingleProcessThread(process.executable, seed_index=idx) for idx in range(1, process.cores + 1)]
    manager = ProcessManager(threads)
    while manager.monitor():
        pass
