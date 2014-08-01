
from G4AtlasApps import AtlasG4Eng
simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration
if not 'DECAYS' in simdict:
    simdict['DECAYS'] = True
    print "ACH12345: ",simdict
    pass
else:
    print 'WARNING: DECAYS key already set to',simdict['DECAYS'],'not sure if I should change that.'


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from G4AtlasApps.SimFlags import simFlags
#simFlags.NeutronTimeCut = 175 # 150 + 25
#simFlags.ReadTR = athenaCommonFlags.PoolEvgenInput()[0]
if simFlags.EventFilter.statusOn:
    simFlags.EventFilter.get_Value()['EtaPhiFilters'] = False

#import G4AtlasApps.AtlasCosmicTrackRecordJob
from AthenaCommon.AppMgr import ServiceMgr
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()

