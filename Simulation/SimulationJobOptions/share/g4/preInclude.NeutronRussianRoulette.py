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
if hasattr(simFlags, 'NRRThreshold') and \
   hasattr(simFlags, 'NRRWeight'):
    simFlags.NRRThreshold = threshold
    simFlags.NRRWeight = weight
    print "Turning on NeutronRussianRoulette with threshold %s MeV and weight %s." % (
      simFlags.NRRThreshold.get_Value(),
      simFlags.NRRWeight.get_Value())
else:
  print "WARNING: attribute NRRThreshold or NRRWeight not found in simFlags."
