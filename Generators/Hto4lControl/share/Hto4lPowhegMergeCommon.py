from Hto4lControl import Hto4lPowhegMerge

# Use the Powheg parameters for configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
hto4lPowhegMerger = Hto4lPowhegMerge( runArgs=transform_runArgs, opts=transform_opts )
