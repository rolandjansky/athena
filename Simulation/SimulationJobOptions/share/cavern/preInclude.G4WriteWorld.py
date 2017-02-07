include.block('SimulationJobOptions/preInclude.G4WriteWorld.py')
## Set some sim flags to take care of Neutron BG
from G4AtlasApps.SimFlags import simFlags
simFlags.NeutronTimeCut = int(2**31 - 1) #probably not required.
simFlags.NeutronTimeCut.set_Off()
simFlags.CavernBG = 'WriteWorld'
if 'runArgs' in dir():
    if hasattr(runArgs,'outputEVNT_TRFile'):
        simFlags.WriteTR = runArgs.outputEVNT_TRFile
    elif hasattr(runArgs,'outputTrackRecordFile'):
        simFlags.WriteTR = runArgs.outputTrackRecordFile
    elif hasattr(runArgs,'outputEvgenFile'):
        simFlags.WriteTR = runArgs.outputEvgenFile
else:
    simFlags.WriteTR = 'NeutronBG.pool.root'
