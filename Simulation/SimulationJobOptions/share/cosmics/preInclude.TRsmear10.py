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

from TrackRecordGenerator.TrackRecordGeneratorConf import TrackRecordGenerator
cg = TrackRecordGenerator()
cg.TRSmearing = 10 #in millimeters
cg.TRPSmearing = 0.001 #in radians

