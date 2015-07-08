include.block('SimulationJobOptions/preInclude.G4ReadCavern.py')
## Configuration to read neutron background track records

from G4AtlasApps.SimFlags import simFlags
simFlags.NeutronTimeCut = int(2**31 - 1) #probably not required.
simFlags.NeutronTimeCut.set_Off()
simFlags.CavernBG = "Read"

import G4AtlasApps.AtlasCosmicTrackRecordJob
from AthenaCommon.AppMgr import ServiceMgr
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from TrackRecordGenerator.TrackRecordGeneratorConf import TrackRecordGenerator
topSequence += TrackRecordGenerator()
topSequence.TrackRecordGenerator.TRSmearing = -1 #in millimeters, e.g. 10
topSequence.TrackRecordGenerator.TRPSmearing = -1 #in radians, e.g. 0.01
topSequence.TrackRecordGenerator.TRCollection = "NeutronBG"
topSequence.TrackRecordGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()
simFlags.RandomSeedList.addSeed( "COSMICS", 2040160768, 80 )
