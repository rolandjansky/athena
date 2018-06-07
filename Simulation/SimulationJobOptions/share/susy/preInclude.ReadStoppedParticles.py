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


def readStoppedParticles():
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed( "COSMICS" ):
        simFlags.RandomSeedList.addSeed( "COSMICS", 2040160768, 443921183 )
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    if not hasattr(topSequence, 'TrackRecordGenerator'):
        from TrackRecordGenerator.TrackRecordGeneratorConf import TrackRecordGenerator
        topSequence += TrackRecordGenerator()
        
    topSequence.TrackRecordGenerator.TRSmearing = -1 #in millimeters, e.g. 10
    topSequence.TrackRecordGenerator.TRPSmearing = -1 #in radians, e.g. 0.01
    topSequence.TrackRecordGenerator.TRCollection = "StoppingPositions"
    topSequence.TrackRecordGenerator.StopParticles = True
    topSequence.TrackRecordGenerator.AtRndmGenSvc = simFlags.RandomSvc.get_Value()
    
simFlags.InitFunctions.add_function("preInit", readStoppedParticles)


#from Pythia_i.Pythia_iConf import Pythia
#topSequence += Pythia()
#topSequence.Pythia.MeanInt=0

#from PythiaRhad_i.PythiaRhad_iConf import PythiaRhad
#topSequence +=PythiaRhad()
#topSequence.PythiaRhad.MeanInt=0

