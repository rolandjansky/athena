from PowhegControl import PowhegConfig_ggH_quark_mass_effects

# Use the Powheg_ggH_quark_mass_effects configuration
transform_runArgs = runArgs if 'runArgs' in dir() else None
transform_opts = opts if 'opts' in dir() else None
PowhegConfig = PowhegConfig_ggH_quark_mass_effects( runArgs=transform_runArgs, opts=transform_opts )

# if 'runArgs' in dir() :
#   PowhegConfig = PowhegConfig_ggH_quark_mass_effects(runArgs)
# else :
#   PowhegConfig = PowhegConfig_ggH_quark_mass_effects()
