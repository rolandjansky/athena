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

    run = TrigCostConfig.prepareT0Config(options, args)
    key = options.key.split(',')
    run._max_num_files=options.max_num_files
    
    for x in args:
        run.StoreInputDir(x, key)

    if options.test == 't0-config':
        #
        # Erase input files and add them explictly
        #
        files = run.GetStoredFiles(-1) #options.max_num_files)
        run.ClearStoredFiles()

        reg = run.GetRegistryConfig()
        reg.WriteXML(options.runmodxml)
        
        run.Config(reg)
        run.Init()
        
        for file in files:
            run.AddInputFile(file)
            
        run.Exec()
        run.Done()
    else:
        #
        # Add input files through Registry save as an xml file
        #
        reg = run.GetRegistryConfig()
        reg.WriteXML(options.runmodxml)
        run.Execute(options.runmodxml)

    #
    # Execute after job python code
    #
    TrigCostConfig.doAfterExecWork(options, args, run)
