##########################################################
#
# SimulationJobOptions/preInclude.G4Optimizations.py
# Marilena Bandieramonte
#
# Geant4 Optimization Options for Run3
#  
##########################################################


###########################################################
#
# Activate magnetic field switch off in central LAr calorimeter
#  More info: https://its.cern.ch/jira/browse/ATLPHYSVAL-773
#
###########################################################


simFlags.MuonFieldOnlyInCalo=True
printfunc ("Turning on MuonFieldOnlyInCalo = %s." % (simFlags.MuonFieldOnlyInCalo()) )


###########################################################
#
# Photon Russian Roulette
# "Fast simulation" killing low energy photons with some
#  probability.
#  More info: https://its.cern.ch/jira/browse/ATLASSIM-4096
#
###########################################################

prr_threshold = 0.5 # MeV
prr_weight = 10.

from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'ApplyPRR') and \
   hasattr(simFlags, 'PRRThreshold') and \
   hasattr(simFlags, 'PRRWeight'):
    simFlags.ApplyPRR = True
    simFlags.PRRThreshold = prr_threshold
    simFlags.PRRWeight = prr_weight
    printfunc ("Turning on PhotonRussianRoulette with threshold %s MeV and weight %s." % (
      simFlags.PRRThreshold.get_Value(),
      simFlags.PRRWeight.get_Value()))
else:
  printfunc ("WARNING: attribute PRRThreshold or PRRWeight not found in simFlags.")



#########################################################
#
# Neutron Russian Roulette
# "Fast simulation" killing low energy neutrons with some
#  probability. 
#  More info: its.cern.ch/jira/browse/ATLASSIM-3924
#
#########################################################

nrr_threshold = 2. # MeV
nrr_weight = 10.

from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'ApplyNRR') and \
   hasattr(simFlags, 'NRRThreshold') and \
   hasattr(simFlags, 'NRRWeight'):
    simFlags.CalibrationRun.set_Off()
    simFlags.ApplyNRR = True
    simFlags.NRRThreshold = nrr_threshold
    simFlags.NRRWeight = nrr_weight
    printfunc ("Turning on NeutronRussianRoulette with threshold %s MeV and weight %s." % (
      simFlags.NRRThreshold.get_Value(),
      simFlags.NRRWeight.get_Value()))
else:
  printfunc ("WARNING: attribute NRRThreshold or NRRWeight not found in simFlags.")


############################################################
#
# EM Range Cuts
# Turn on range cuts for gamma processes (conv, phot, compt)
# More info: https://its.cern.ch/jira/browse/ATLASSIM-3956
#
###########################################################

from G4AtlasApps import callbacks
callbacks.turn_on_gamma_range_cuts()


##########################################################
#
# Activate the G4GammaGeneralProcess and the corresponding
# job options
#  
##########################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.G4Commands+=["/process/em/UseGeneralProcess true"];
simFlags.OptionalUserActionList.addAction("G4UA::FixG4CreatorProcessTool")


