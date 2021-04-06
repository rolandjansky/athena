from Prophecy4fControl import ProphecyPowhegMerge

# Use the Powheg parameters for configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
ProphecyPowhegMerger = ProphecyPowhegMerge( runArgs=transform_runArgs, opts=transform_opts )

# Use when using OTF powheg: ProphecyPowhegMerger.input_powhegLHE = PowhegConfig.output_events_file_name
ProphecyPowhegMerger.input_powheg_file_name         = powhegInputFile
# ProphecyPowhegMerger.input_prophecy4e_file_name     = ProphecyConfig4e.output_events_file_name
# ProphecyPowhegMerger.input_prophecy_2e2mu_file_name = ProphecyConfig2e2mu.output_events_file_name
ProphecyPowhegMerger.random_seed                    = 431*runArgs.randomSeed # separate random seed four decay modes

ProphecyPowhegMerger.merge()

