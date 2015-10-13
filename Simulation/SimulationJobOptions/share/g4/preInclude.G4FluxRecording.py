include.block('SimulationJobOptions/preInclude.G4FluxRecording.py')
## Set some sim flags to take care of Neutron BG
from G4AtlasApps.SimFlags import simFlags
simFlags.CalibrationRun.set_Off() # Turn off calibration hits (just a waste)
simFlags.RecordFlux=True
simFlags.NeutronTimeCut = int(2**31 - 1) #probably not required.
simFlags.NeutronTimeCut.set_Off()
simFlags.CavernBG = 'Write'