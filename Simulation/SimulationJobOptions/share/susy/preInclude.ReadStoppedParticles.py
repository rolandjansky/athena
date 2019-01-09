## Configuration to read neutron background track records

from G4AtlasApps import AtlasG4Eng
simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration
if not 'DECAYS' in simdict:
    simdict['DECAYS'] = True
else:
    print 'WARNING: DECAYS key already set to',simdict['DECAYS'],'not sure if I should change that.'

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from G4AtlasApps.SimFlags import simFlags
simFlags.NeutronTimeCut = 175 # 150 + 25
simFlags.ReadTR = athenaCommonFlags.PoolEvgenInput()[0]
if simFlags.EventFilter.statusOn:
    simFlags.EventFilter.switchFilterOff('EtaPhiFilters')
    simFlags.EventFilter.switchFilterOff('VertexPositioner')
    simFlags.VertexFromCondDB.set_Value_and_Lock(False)

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
topSequence.TrackRecordGenerator.TRCollection = "StoppingPositions"
topSequence.TrackRecordGenerator.StopParticles = True
topSequence.TrackRecordGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()
topSequence.TrackRecordGenerator.stopped_tminus = -10
topSequence.TrackRecordGenerator.stopped_tplus = 15

simFlags.RandomSeedList.addSeed( "COSMICS", 2040160768, 80 )

#from Pythia_i.Pythia_iConf import Pythia
#topSequence += Pythia()
#topSequence.Pythia.MeanInt=0

#from PythiaRhad_i.PythiaRhad_iConf import PythiaRhad
#topSequence +=PythiaRhad()
#topSequence.PythiaRhad.MeanInt=0

