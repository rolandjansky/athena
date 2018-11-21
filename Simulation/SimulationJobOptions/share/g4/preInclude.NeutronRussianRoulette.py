#########################################################
#
# SimulationJobOptions/preInclude.NeutronRussianRoulette.py
# Miha Muskinja
#
# "Fast simulation" killing low energy neutrons with some
#  probability. More info: its.cern.ch/jira/browse/ATLASSIM-3924
#
#########################################################

threshold = 1. # MeV
weight = 10.

from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'RussianRouletteThreshold') and \
   hasattr(simFlags, 'RussianRouletteWeight'):
    simFlags.RussianRouletteThreshold = threshold
    simFlags.RussianRouletteWeight = weight
    print "Turning on RussianRoulette with threshold %s MeV and weight %s." % (
      simFlags.RussianRouletteThreshold.get_Value(),
      simFlags.RussianRouletteWeight.get_Value())
else:
  print "WARNING: attribute RussianRouletteThreshold or RussianRouletteWeight not found in simFlags."
