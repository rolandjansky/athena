from Prophecy4fControl import ProphecyPowhegDefault

# Use the Powheg parameters for configuration
transform_runArgs   = runArgs if 'runArgs' in dir() else None
transform_opts      = opts if 'opts' in dir() else None
# ProphecyConfig      = ProphecyPowhegDefault( runArgs=transform_runArgs, opts=transform_opts )
ProphecyConfig4e    = ProphecyPowhegDefault( runArgs=transform_runArgs, opts=transform_opts )
ProphecyConfig4mu   = ProphecyPowhegDefault( runArgs=transform_runArgs, opts=transform_opts )
ProphecyConfig2e2mu = ProphecyPowhegDefault( runArgs=transform_runArgs, opts=transform_opts )

# runargs.maxEvents number of generated events

# Generate a file for 4e
ProphecyConfig4e.mass_t                  = PowhegConfig.mass_t
ProphecyConfig4e.mass_b                  = PowhegConfig.mass_b
ProphecyConfig4e.mass_c                  = PowhegConfig.mass_c
ProphecyConfig4e.mass_H                  = PowhegConfig.mass_H
ProphecyConfig4e.channel                 = "e anti-e e anti-e"
ProphecyConfig4e.output_events_file_name = "ProphecyOTF4e._1.events"

# for testing, set nEvents_weighted. nEvents is normally set by --maxEvents in launching script
ProphecyConfig4e.nEvents                 = int( 1.3 * runArgs.maxEvents / 4 + 0.5 ) # 1/4 for this channel
ProphecyConfig4e.nEvents_weighted        = 100*runArgs.maxEvents # for prod jobs
ProphecyConfig4e.random_seed             = 123*runArgs.randomSeed # separate random seed for 4e

ProphecyConfig4e.generateRunCard()
ProphecyConfig4e.generateEvents()

# Generate a file for 4mu
ProphecyConfig4mu.mass_t                  = PowhegConfig.mass_t
ProphecyConfig4mu.mass_b                  = PowhegConfig.mass_b
ProphecyConfig4mu.mass_c                  = PowhegConfig.mass_c
ProphecyConfig4mu.mass_H                  = PowhegConfig.mass_H
ProphecyConfig4mu.channel                 = "mu anti-mu mu anti-mu"
ProphecyConfig4mu.output_events_file_name = "ProphecyOTF4mu._1.events"

# for testing, set nEvents_weighted. nEvents is normally set by --maxEvents in launching script
ProphecyConfig4mu.nEvents                 = int( 1.3 * runArgs.maxEvents / 4 + 0.5 ) # 1/4 for this channel
ProphecyConfig4mu.nEvents_weighted        = 100*runArgs.maxEvents # for prod jobs
ProphecyConfig4mu.random_seed             = 368*runArgs.randomSeed # separate random seed for 4mu

ProphecyConfig4mu.generateRunCard()
ProphecyConfig4mu.generateEvents()

# Generate a file for 2e2mu
ProphecyConfig2e2mu.mass_t                  = PowhegConfig.mass_t
ProphecyConfig2e2mu.mass_b                  = PowhegConfig.mass_b
ProphecyConfig2e2mu.mass_c                  = PowhegConfig.mass_c
ProphecyConfig2e2mu.mass_H                  = PowhegConfig.mass_H
ProphecyConfig2e2mu.channel                 = "e anti-e mu anti-mu"
ProphecyConfig2e2mu.output_events_file_name = 'ProphecyOTF2e2mu._1.events'

# for testing, set nEvents_weighted. nEvents is normally set by --maxEvents in launching script
ProphecyConfig2e2mu.nEvents                 = int( 1.3 * runArgs.maxEvents / 2 + 0.5 ) # 1/2 for this channel
ProphecyConfig2e2mu.nEvents_weighted        = 200*runArgs.maxEvents # for prod jobs
ProphecyConfig2e2mu.random_seed             = 278*runArgs.randomSeed # separate random seed for 2e2mu

ProphecyConfig2e2mu.generateRunCard()
ProphecyConfig2e2mu.generateEvents()
