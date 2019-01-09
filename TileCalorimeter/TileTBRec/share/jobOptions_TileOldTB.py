#==============================================================
#
# Job options file to recreate TB 2000-2003 ntuple
#
#==============================================================

#use McEventSelector
## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager, ToolSvc
from AthenaCommon.AppMgr import theApp, ToolSvc

#--- Auditor options -------------------------------------------
# write out a summary of the time spent
#from GaudiAud import *
#svcMgr.AuditorSvc += ChronoAuditor()
#svcMgr.AuditorSvc += MemStatAuditor()

if not 'EvtMax' in dir():
    EvtMax = 5000000

theApp.EvtMax=EvtMax

if not 'RunNumber' in dir():
    RunNumber = 210319

svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.EventsPerRun      = 500000
svcMgr.EventSelector.FirstEvent        = 0

if not 'OutputLevel' in dir():
    OutputLevel = 3

if not hasattr(svcMgr, 'MessageSvc'):
    from GaudiSvc.GaudiSvcConf import MessageSvc
    svcMgr += MessageSvc()
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.OutputLevel  = OutputLevel

if not hasattr(svcMgr, 'HistorySvc'):
    from GaudiSvc.GaudiSvcConf import HistorySvc
    svcMgr += HistorySvc()
svcMgr.HistorySvc.OutputLevel = 4

#--- Detector flags -------------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags    import DetFlags

# - Select detectors
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.em_setOff()
DetFlags.FCal_setOff()
DetFlags.HEC_setOff()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()

# - switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()
DetFlags.simulateLVL1.all_setOff()
# - print flags 
DetFlags.Print()

#--- GeoModel stuff -------------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_geant4()
GlobalFlags.DetGeo.set_ctbh8()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.TileVersionOverride = "TileTB-3B-00"

# Common for the Calo: detector description package
#include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
#include( "CaloIdCnv/CaloIdCnv_joboptions.py" )


if not 'TileBarrelOnly' in dir():
    TileBarrelOnly = False

if not 'TileCisRun' in dir():
    TileCisRun = False
    
if not 'TileAdderFit' in dir():
    TileAdderFit = True

if TileCisRun:
    # set all parameters for standard CIS run
    TileBiGain = True
    TileRunType = 8

    if not 'TileFrameLength' in dir():
        TileFrameLength = 9; # still expect 9 samples even in CIS runs in old testbeams

    TileCalibrateEnergy = False; # keep ADC counts

    if not 'TilePmtOrder' in dir():
        TilePmtOrder = True
else:
    # set all parameters for standard Phys run
    TileBiGain = False
    if not 'TileRunType' in dir():
        TileRunType = 0; # if run type is set to non-zero values, it overrides event trig type

    if not 'TileFrameLength' in dir():
        TileFrameLength = 9; # the value we expect in single gain run

    if not 'TileCalibrateEnergy' in dir():
        TileCalibrateEnergy = True; # convert ADC counts to pCb for normal runs and also apply Cesium

    if not 'TilePmtOrder' in dir():
        TilePmtOrder = True

if not 'InputDirectory' in dir():
    InputDirectory = "."

if not 'OutputDirectory' in dir():
    OutputDirectory = "."

if not 'SkipEvents' in dir():
    SkipEvents=0

if not 'EventShift' in dir():
    EventShift=0


from TileTBRec.TileTBRecConf import *               

theTileTBOldNtupleRead=TileTBOldNtupleRead()
theTileTBOldNtupleRead.NtupleID = "h1000"
theTileTBOldNtupleRead.NtupleDir = InputDirectory
theTileTBOldNtupleRead.RunNumber = RunNumber
theTileTBOldNtupleRead.BiGain = TileBiGain
theTileTBOldNtupleRead.PmtOrder = TilePmtOrder
theTileTBOldNtupleRead.BarrelOnly = TileBarrelOnly
theTileTBOldNtupleRead.AdderFit = TileAdderFit
theTileTBOldNtupleRead.SkipEvents = SkipEvents
theTileTBOldNtupleRead.EventShift = [ EventShift, EventShift, EventShift, EventShift, EventShift, EventShift ]

topSequence += theTileTBOldNtupleRead;


doTileHitToDigit = False
doTileTBHitToBeamElem = False
doTileFlat = False
doTileOpt = False
doTileFit = True
include( "TileTBRec/TileTBDefaults_jobOptions.py" )
include( "TileSimAlgs/TileTBDigitization_jobOptions.py" )
include( "TileTBRec/TileTBRec_jobOptions.py" )


theTileTBOldNtupleWrite=TileTBOldNtupleWrite()
theTileTBOldNtupleWrite.NtupleID = "h1000"
theTileTBOldNtupleWrite.NtupleDir = OutputDirectory
theTileTBOldNtupleWrite.PmtOrder = TilePmtOrder
theTileTBOldNtupleWrite.CalibrateEnergy = TileCalibrateEnergy

topSequence += theTileTBOldNtupleWrite;

print topSequence


#=============================================================
#=== setup TileConditions
#=============================================================
from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()
tileInfoConfigurator.OutputLevel = 3

#=== set different CIS and Cesium ASCII file names for different run periods 

run=str(RunNumber/10000) + "0000"
while len(run)<7:
    run='0'+run
CisLookup  = "Tile2004.lut"
CisLinear  = "Tile" + run + ".cis"
CesiumFile = "Tile" + run + ".ces"

from AthenaCommon.AppMgr import ToolSvc
from TileConditions.TileCondProxyConf import getTileCondProxy
ToolSvc.TileCondToolEmscale.ProxyOflCisLin = getTileCondProxy('FILE','Flt',CisLinear, 'TileCondProxyFile_OflCisLin')
ToolSvc.TileCondToolEmscale.ProxyOflCisNln = getTileCondProxy('FILE','Flt',CisLookup, 'TileCondProxyFile_OflCisNln')
ToolSvc.TileCondToolEmscale.ProxyOflCes    = getTileCondProxy('FILE','Flt',CesiumFile,'TileCondProxyFile_OflCes')

print tileInfoConfigurator

