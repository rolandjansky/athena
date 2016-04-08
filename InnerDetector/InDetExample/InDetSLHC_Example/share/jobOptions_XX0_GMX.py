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

from RadLengthIntTool.RadLengthIntToolConf import XX0_Tool
mXX0_Tool = XX0_Tool()

from AthenaCommon.AppMgr import ServiceMgr

from UserActionSvc.UserActionSvcConf import UserActionSvc;
ServiceMgr += UserActionSvc()
ServiceMgr.UserActionSvc.UserActions += [ mXX0_Tool]
theApp.CreateSvc += [ "UserActionSvc"]

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output  += ["xx0 DATAFILE='xx0.root' OPT='RECREATE'"];

print ServiceMgr

mXX0_Tool.ActionName = "XX0_ActionTool"
mXX0_Tool.volumePartsFile = "VolumeNameParts.txt"
mXX0_Tool.DoIntLength = False
mXX0_Tool.DoEta = True

mXX0_Tool.NBinsEta = 480
mXX0_Tool.EtaMin = -5
mXX0_Tool.EtaMax = 5.

mXX0_Tool.DoPhi = False
mXX0_Tool.NBinsPhi = 50

mXX0_Tool.DoRadial = False
mXX0_Tool.NBinsRadial = 200

mXX0_Tool.RMin =   0.  * mm
mXX0_Tool.RMax =  1150. * mm

mXX0_Tool.DoZScan = False

mXX0_Tool.ZMin = -3490. * mm 
mXX0_Tool.ZMax =  3490. * mm 


print mXX0_Tool
print UserActionSvc

from G4AtlasApps import SimKernel


if 'GeometryTextFile' in dir():
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr.InDetGeometryDBSvc.TextFileName = GeometryTextFile


# enter interactive mode 
theApp.initialize()

from G4AtlasApps import  AtlasG4Eng,PyG4Atlas
XX0_Action = PyG4Atlas.UserAction( 'RadLengthIntTool','XX0_ActionTool',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(XX0_Action)


SimFlags.InitFunctions.add_function('preInitG4', XX0_Action)
#AtlasG4Eng.G4Eng.init_Simulation(3)

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

# start run after the interactive mode 

theApp.nextEvent( theApp.EvtMax )
theApp.finalize()
theApp.exit()

#--- End jobOptions.G4Atlas_Sim.py file  ------------------------------
