from PowhegControl.PowhegUtils import PowhegConfig_bb

# Use the Powheg_bb configuration
PowhegConfig = PowhegConfig_bb()
PowhegConfig.energy = 4000
PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()
