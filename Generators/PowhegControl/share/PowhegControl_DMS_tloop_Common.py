from PowhegControl import PowhegConfig_DMS_tloop

# Use the Powheg_DMS_tloop configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_DMS_tloop( runArgs=transform_runArgs, opts=transform_opts )
