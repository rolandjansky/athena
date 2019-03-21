#########################################################
#
# SimulationJobOptions/preInclude.PhotonRussianRoulette.py
# Miha Muskinja
#
# "Fast simulation" killing low energy photons with some
#  probability.
#
#########################################################

threshold = 1. # MeV
weight = 10.

from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'PRRThreshold') and \
   hasattr(simFlags, 'PRRWeight'):
    simFlags.PRRThreshold = threshold
    simFlags.PRRWeight = weight
    print "Turning on PhotonRussianRoulette with threshold %s MeV and weight %s." % (
      simFlags.PRRThreshold.get_Value(),
      simFlags.PRRWeight.get_Value())
else:
  print "WARNING: attribute PRRThreshold or PRRWeight not found in simFlags."
