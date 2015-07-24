from Prophecy4fControl import ProphecyPowhegDefault

# Use the Powheg parameters for configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
ProphecyConfig = ProphecyPowhegDefault( runArgs=transform_runArgs, opts=transform_opts )
