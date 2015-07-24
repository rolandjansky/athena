from Prophecy4fControl import ProphecyPowhegMerge

# Use the Powheg parameters for configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
ProphecyPowhegMerger = ProphecyPowhegMerge( runArgs=transform_runArgs, opts=transform_opts )
