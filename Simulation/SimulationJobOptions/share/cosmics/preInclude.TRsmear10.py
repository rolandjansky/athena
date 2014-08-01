#########################################################
#
# SimulationJobOptions/preInclude.TRsmear10.py
# Zach Marshall
#
# Smear the track records input into cosmic sim by 10mm
# and 1 mrad
#
# Inserted into the alg seq by preInclude.Cosmics.py
#
#########################################################

from CosmicGenerator.CosmicGeneratorConf import CosmicGenerator
cg = CosmicGenerator()
cg.TRSmearing = 10 #in millimeters
cg.TRPSmearing = 0.001 #in radians

