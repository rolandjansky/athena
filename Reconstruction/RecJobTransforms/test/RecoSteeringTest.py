#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.Logging import logging
log = logging.getLogger("RecoSteering")

import sys
def _run(input):
    import datetime
    startStamp = datetime.datetime.now()
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    flags.Exec.MaxEvents = 5
    flags.Concurrency.NumThreads = 1
    flags.Input.Files = []
    #TODO these flags should be defaulted in the divier function above, 
    #TODO    but then we ought to have option to set them from command line should the parser be passed there too?

    flags.Output.ESDFileName="myESD.pool.root"
    flags.Output.AODFileName="myAOD.pool.root"
    parser = flags.getArgumentParser()
    args = flags.fillFromArgs(parser=parser)

    # test inputs
    if not flags.Input.Files:
        from AthenaConfiguration.TestDefaults import defaultTestFiles
        if input == "RAW":
            flags.Input.Files = defaultTestFiles.RAW
        if input == "ESD":
            flags.Input.Files = defaultTestFiles.ESD
        if input == "RDO":
            flags.Input.Files = defaultTestFiles.RDO

    # Enable PerfMon
    flags.PerfMon.doFullMonMT = True
    flags.PerfMon.OutputJSON = f"perfmonmt_{input}.json"

    flags.lock()
    log.info("Configuring according to flag values listed below")
    flags.dump()

    from RecJobTransforms.RecoSteering import RecoSteering
    acc = RecoSteering(flags)
    confStamp = datetime.datetime.now()
    log.info("configured in %d seconds", (confStamp-startStamp).seconds )
    acc.printConfig(withDetails=True)

    confFileName=f"recoConfig{input}.pkl"
    if args.configOnly:
        confFileName=args.configOnly

    with open(confFileName, "wb") as confFile:
        acc.store(confFile)
        log.info("configOnly option specified. Saved in: %s ... exiting now.", args.configOnly )
    if args.configOnly:
        sys.exit(0)

    # running        
    statusCode = acc.run()
    endStamp = datetime.datetime.now()
    log.info("total time spent %d seconds (running %s seconds) ", (endStamp-startStamp).seconds, (endStamp-confStamp).seconds )
    return statusCode


if __name__ == "__main__":
    statusCode = None
    for test in ["RAW", "ESD", "RDO"]:
        if f"--{test}" in sys.argv:
            del sys.argv[sys.argv.index(f"--{test}")]
            statusCode = _run(input=test)
    assert statusCode is not None, "No test was run"
    sys.exit(not statusCode.isSuccess())
