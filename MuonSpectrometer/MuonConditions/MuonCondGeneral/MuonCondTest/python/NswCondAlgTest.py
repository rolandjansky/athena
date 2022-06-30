# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def NSWCondAlgTest(flags,alg_name="NSWCondTestAlg", **kwargs):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()
    from AthenaConfiguration.ComponentFactory import CompFactory
    from MuonConfig.MuonCondAlgConfig import NswCalibDbAlgCfg
    result.merge(NswCalibDbAlgCfg(flags))
    the_alg = CompFactory.NswCondTestAlg(alg_name, **kwargs)
    result.addEventAlgo(the_alg, primary=True)
    return result
    

if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from .MdtCablingTester import SetupArgParser, setupServicesCfg
    
    parser = SetupArgParser()
    parser.add_argument("--LogName", default="LogFile", 
                        help="If the test is run multiple times to ensure reproducibility, then the dump of the test can be resteered")
    args = parser.parse_args()
    ConfigFlags.Concurrency.NumThreads = args.threads
    ConfigFlags.Concurrency.NumConcurrentEvents = args.threads  # Might change this later, but good enough for the moment.
    ConfigFlags.Output.ESDFileName = args.output
    ConfigFlags.Input.Files = args.inputFile
    ConfigFlags.lock()

   
    cfg = setupServicesCfg(ConfigFlags)
    msgService = cfg.getService('MessageSvc')
    msgService.Format = "S:%s E:%e % F%128W%S%7W%R%T  %0W%M"

    cfg.merge(NSWCondAlgTest(ConfigFlags, LogName = args.LogName))
    cfg.printConfig(withDetails=True, summariseProps=True)

    ConfigFlags.dump()

    with open("NSWCondAlgTest.pkl", "wb") as f:
        cfg.store(f)
        f.close()

    sc = cfg.run(1)
    if not sc.isSuccess():
        import sys
        sys.exit("Execution failed")

