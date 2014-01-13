#!/usr/bin/env python

#-----------------------------------------------------------------------------
# Configure python and C++ objects, add input files and execute C++ code
#    
if __name__ == "__main__":

    #
    # Parse command line options
    #    
    import TrigCostRate.TrigCostRateConfig as TrigCostConfig
    
    p = TrigCostConfig.prepareRatesParser()
    (options, args) = p.parse_args()
    
    #
    # Configure python RunConfig wrapper around C++ RunModule
    #
    if len(args) < 1:
        p.error("Error - missing path to input ntuple files")

    run = TrigCostConfig.prepareRunConfig(options, args)
    
    if options.do_recursive:
        for x in args:
            run.StoreRecursiveDir(x, options.key, max_num_files=options.max_num_files)
    else:
        for x in args:
            run.StoreInputDir(x, options.key, max_num_files=options.max_num_files)

    reg = run.GetRegistryConfig()
    reg.WriteXML(options.runmodxml)

    run.Execute(options.runmodxml)

    #
    # Execute after job python code
    #
    TrigCostConfig.doAfterExecWork(options, args, run)
