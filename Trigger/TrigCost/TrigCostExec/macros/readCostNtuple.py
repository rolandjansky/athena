#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
    
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

    run = TrigCostConfig.prepareNtupleConfig(options, args)
    key = options.key.split(',')
    
    for x in args:
        run.StoreInputDir(x, key)

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
