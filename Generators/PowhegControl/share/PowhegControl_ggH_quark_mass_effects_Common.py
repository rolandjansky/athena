from PowhegControl.PowhegUtils import PowhegConfig_ggH_quark_mass_effects

# Use the Powheg_ggH_quark_mass_effects configuration
if 'runArgs' in dir() :
  PowhegConfig = PowhegConfig_ggH_quark_mass_effects(runArgs)
else :
  PowhegConfig = PowhegConfig_ggH_quark_mass_effects()
