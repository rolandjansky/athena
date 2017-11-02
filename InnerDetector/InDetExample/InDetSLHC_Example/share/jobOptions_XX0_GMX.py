#
# Job options file for Geantino mapping of radiation lengths
#

execfile("ID_only.py")
execfile("geometry.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = 'HITS.pool.root'
athenaCommonFlags.EvtMax = 100000

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

SimFlags.EventFilter.set_Off()
SimFlags.MagneticField.set_Off()

athenaCommonFlags.PoolEvgenInput.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=SimFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 999
pg.sampler.pos = PG.PosSampler(x=0.0, y=0.0, z=[0.0,0.0], t=0.0)
import math
pg.sampler.mom = PG.EEtaMPhiSampler(energy=50000, eta=[-5.,5.], phi=[0., 2 * math.pi])
topSeq += pg

MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 500

from AthenaCommon.AppMgr import ServiceMgr

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output  += ["xx0 DATAFILE='xx0.root' OPT='RECREATE'"];

print ServiceMgr

from G4AtlasApps import SimKernel


if 'GeometryTextFile' in dir():
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr.InDetGeometryDBSvc.TextFileName = GeometryTextFile


# enter interactive mode 
theApp.initialize()

## TODO Something like this should work with the appropriate CfgGetter method:
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('XX0_Tool',['Event','Run','Step'])
simFlags.UserActionConfig.addConfig('XX0_Tool','volumePartsFile', "VolumeNameParts.txt")
simFlags.UserActionConfig.addConfig('XX0_Tool','DoIntLength', False)
simFlags.UserActionConfig.addConfig('XX0_Tool','DoEta', True)
simFlags.UserActionConfig.addConfig('XX0_Tool','NBinsEta', 480)
simFlags.UserActionConfig.addConfig('XX0_Tool','EtaMin', -5)
simFlags.UserActionConfig.addConfig('XX0_Tool','EtaMax', 5.)
simFlags.UserActionConfig.addConfig('XX0_Tool','DoPhi', False)
simFlags.UserActionConfig.addConfig('XX0_Tool','NBinsPhi', 50)
simFlags.UserActionConfig.addConfig('XX0_Tool','DoRadial', False)
simFlags.UserActionConfig.addConfig('XX0_Tool','NBinsRadial', 200)
simFlags.UserActionConfig.addConfig('XX0_Tool','RMin',   0.  * mm)
simFlags.UserActionConfig.addConfig('XX0_Tool','RMax',  1150. * mm)
simFlags.UserActionConfig.addConfig('XX0_Tool','DoZScan', False)
simFlags.UserActionConfig.addConfig('XX0_Tool','ZMin', -3490. * mm)
simFlags.UserActionConfig.addConfig('XX0_Tool','ZMax',  3490. * mm)


#AtlasG4Eng.G4Eng.init_Simulation(3)

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

# start run after the interactive mode 

theApp.nextEvent( theApp.EvtMax )
theApp.finalize()
theApp.exit()

#--- End jobOptions.G4Atlas_Sim.py file  ------------------------------
