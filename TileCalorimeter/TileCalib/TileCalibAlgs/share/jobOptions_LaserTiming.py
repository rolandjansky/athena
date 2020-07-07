# RunNumber=80619
# 76824
RunNumber=76821
# RunNumber=40896 # EBA 2008-05-07
# RunNumber=66263
TileLasRun=True
EvtMin=1 # some number
EvtMax=1000 #9000 #100 events
doTileFit=True
doTileFlat=False
doTileOpt2=False

#**************************************************************
#
# jopOptions file for TileCal commissioning analysis
#
#==============================================================

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 4
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False


if not 'RunFromLocal' in dir():
    RunFromLocal=False

if not 'RunNumber' in dir():
    RunNumber = 0

if RunNumber>40000:
    if not 'TileFrameLength' in dir():
        TileFrameLength = 7; # expected value in all runs starting from M6

if not 'TileUseCOOL' in dir():
    TileUseCOOL=False; # don't use COOL DB by default for commissioning
    
if 'TilePhysRun' in dir():
    if TilePhysRun:
        if not 'InputDirectory' in dir():
           if RunNumber<10800:
                InputDirectory = "/castor/cern.ch/atlas/P1commisioning/phaseII"
           elif RunNumber<20000:
                InputDirectory = "/castor/cern.ch/atlas/P1commisioning/phaseIII"
           elif RunNumber<28000:
                InputDirectory = "/castor/cern.ch/grid/atlas/t0/perm/DAQ"
           elif RunNumber<32053:
                InputDirectory = "/castor/cern.ch/grid/atlas/t0/perm/DAQ/M5"
           elif RunNumber<32946:
                InputDirectory = "/castor/cern.ch/grid/atlas/t0/perm/DAQ/CaloWeekNov07"
           elif RunNumber<39801:
                InputDirectory = "/castor/cern.ch/grid/atlas/t0/perm/DAQ/CaloWeekFeb08"
           elif RunNumber<44520:
                InputDirectory = "/castor/cern.ch/grid/atlas/DAQ/M6"
           elif RunNumber<54063:
                InputDirectory = "/castor/cern.ch/grid/atlas/DAQ/CaloWeekApr08"
           elif RunNumber<71501:
                InputDirectory = "/castor/cern.ch/grid/atlas/DAQ/M7"
           else:
                InputDirectory = "/castor/cern.ch/atlas/testbeam/tilecal/2008/daq"
else:
    TilePhysRun=False

if not 'InputDirectory' in dir():
    if RunNumber<36127:
        InputDirectory = "/castor/cern.ch/atlas/testbeam/tilecal/2007/daq"
    else:
        InputDirectory = "/castor/cern.ch/atlas/testbeam/tilecal/2008/daq"

if not 'FileFilter' in dir():
    FileFilter = ".data"

from os import system,popen

def FindFile(path,runinput,filter):

    run=str(runinput)

    while len(run)<7:
        run='0'+run
        
    files=[]
    fullname=[]

    if RunFromLocal:
        for f in popen('ls %(path)s | grep %(run)s' % {'path': path, 'run':run }):
            files.append(f)

    else:
        if (TilePhysRun and RunNumber>28000 and RunNumber<40000):
            for f in popen('nsls -l %(path)s | grep %(run)s | grep -v "               [ 0-9][ 0-9][0-9] " | grep %(filt)s | grep -e "b0000[01][01][01][01]" -e "physics.cosmics" | grep -v writing | cut -c66- ' % {'path': path, 'run':run, 'filt':filter  }):
                files.append(f)
        elif (TilePhysRun and RunNumber>40000 and RunNumber<45000):
            for f in popen('nsls -l %(path)s | grep %(run)s | grep -v "               [ 0-9][ 0-9][0-9] " | grep %(filt)s | grep -e "NIM0" -e "[pP]hysics" | grep -v writing | cut -c66- ' % {'path': path, 'run':run, 'filt':filter  }):
                files.append(f)
        else:
            for f in popen('nsls -l %(path)s | grep %(run)s | grep -v "               [ 0-9][ 0-9][0-9] " | grep %(filt)s | grep -v writing | cut -c66- ' % {'path': path, 'run':run, 'filt':filter  }):
                files.append(f)

    for nn in range(len(files)):
        temp=files[nn].split('\n')
        fullname.append(path+'/'+temp[0])

    return [fullname,run]


if not 'FileNameVec' in dir():
    if not 'FileName' in dir():    
        tmp= FindFile(InputDirectory,RunNumber,FileFilter)
        FileNameVec = tmp[0]
        FormattedRunNumber=tmp[1]
    else:
        FileNameVec = [ FileName ]
        FormattedRunNumber=RunNumber
else:
    FormattedRunNumber=RunNumber
  	 
print "InputDirectory is "+str(InputDirectory)
print "RunNumber was "+str(RunNumber)
print "RunNumber is "+str(FormattedRunNumber)
print "FullFileName is "+str(FileNameVec)


if not 'EvtMin' in dir():
        EvtMin=1
        EvtMinNotSet=True

if not 'EvtMax' in dir():
    EvtMax = 5000000

if not 'EventsPerFile' in dir():
    EventsPerFile = 0

if not 'DrawerList' in dir():
    DrawerList="-1"

#-----------------------------------
# TileCal reconstruction for commissioning
#-----------------------------------

# if run type is set to non-zero value, it overrides event trig type
if not 'TileRunType' in dir():
    TileRunType = 0; # 1 - physics, 2 - laser, 4 - pedestal, 8 - CIS run 

# override run type with one of these flags
if not 'TileCisRun' in dir():
    TileCisRun = False
if not 'TileMonoRun' in dir():
    TileMonoRun=False
if not 'TilePedRun' in dir():
    TilePedRun=False
if not 'TileLasRun' in dir():
    TileLasRun=False
if not 'TileLedRun' in dir():
    TileLedRun=False
if not 'TilePhysRun' in dir():
    TilePhysRun=False

if TileCisRun:
    TileRunType = 8
    # set all parameters for standard CIS run
    if not 'TileFrameLength' in dir():
        TileFrameLength = 7

    # do not convert ADC counts to pCb
    if not 'TileCalibrateEnergy' in dir():
        TileCalibrateEnergy = False

    # skip all events when just one channel is fired (4*48)
    if 'EvtMinNotSet' in dir():
        EvtMin=190

elif TileMonoRun:
    TileRunType = 8
    if not 'doTileFlat' in dir():
        doTileFlat=True
    if not 'doTileOpt2' in dir():
        doTileOpt2=True
    if not 'TileFrameLength' in dir():
        TileFrameLength=9

elif TilePedRun:
    TileRunType = 4
    if not 'TileFrameLength' in dir():
        TileFrameLength=7
    
elif TileLasRun:
    TileRunType = 2
    if not 'doTileFlat' in dir():
        doTileFlat=True
    if not 'doTileOpt2' in dir():
        doTileOpt2=True
    if not 'TileFrameLength' in dir():
        TileFrameLength=9
    
elif TileLedRun:
    TileRunType = 2
    if not 'doTileFlat' in dir():
        doTileFlat=True
    if not 'doTileOpt2' in dir():
        doTileOpt2=True
    if not 'TileFrameLength' in dir():
        TileFrameLength=9
    
else:
    if TilePhysRun:
        TileRunType = 1
        if not 'doTileFlat' in dir():
            doTileFlat=True
        if not 'doTileOpt2' in dir():
            doTileOpt2=True

    if not 'TileFrameLength' in dir():
        TileFrameLength = 9; # the value we expect in single gain run

    if not 'doTileFlat' in dir():
        doTileFlat = True

    if not 'doTileOpt2' in dir():
        doTileOpt2 = True

    if not 'doTileFit' in dir():
        doTileFit = True

# special options
# which algorithms to run 
# and output from which algorithm to use as input for TileCellBuilder
# by default we use just fit method, which gives the best results
# but for phys run we use all 3 methods (see above)

if not 'doTileFlat' in dir():
    doTileFlat = False

if not 'doTileOpt2' in dir():
    doTileOpt2 = False

if not 'doTileFit' in dir():
    doTileFit = True

from TileRecUtils.TileRecFlags import jobproperties

jobproperties.TileRecFlags.doTileFit=doTileFit
jobproperties.TileRecFlags.doTileOpt2=doTileOpt2
jobproperties.TileRecFlags.doTileFlat=doTileFlat
jobproperties.TileRecFlags.doTileManyAmps=False

jobproperties.print_JobProperties('tree&value')

if not 'useRODReco' in dir():
    useRODReco = False


recoMethod="fit"
processAllChannels=True
    
# all other parameters which can be set to True or False
# from the command line 

# use PMT ordering in ntuple (convert channel to PMT number) 
if not 'doTileCable' in dir():
    doTileCable = True

# convert ADC counts to pCb by default
if not 'TileCalibrateEnergy' in dir():
    TileCalibrateEnergy = True



#---------------
# end of options
#---------------

#=============================================================
#=== init Det Descr
#=============================================================

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOn()
DetFlags.readRDOBS.Tile_setOn()
DetFlags.Print()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-CommNF-07-00-00'
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion()) )

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
log.info( "GeoModelSvc.AtlasVersion = %s" % (GeoModelSvc.AtlasVersion) )
#GeoModelSvc.TileVersionOverride = "TileCal-Comm-01"
#log.info( "GeoModelSvc.TileVersionOverride = %s" % (GeoModelSvc.TileVersionOverride) )

include( "TileTBRec/TileTBDefaults_jobOptions.py" )

# Set Global tag for IOVDbSvc for commissioning
from IOVDbSvc.CondDB import conddb
# conddb.setGlobalTag('COMCOND-005-01')
conddb.setGlobalTag('COMCOND-ES1C-000-00')

#=============================================================
#=== setup TileConditions
#=============================================================
TileBchlList='TileNoBad.oflBch'; # dummy file with bad channels by default
include( "TileConditions/TileConditionsCosmics_jobOptions.py" )
tileInfoConfigurator.OutputLevel = OutputLevel

print tileInfoConfigurator

#=============================================================
#=== ByteStream Input
#=============================================================
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
theApp.ExtSvc += [ "ByteStreamCnvSvc" ] 
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc
EventSelector.Input += FileNameVec

# read ByteStream and reconstruct data
include( "TileTBRec/TileTBRec_jobOptions.py" )

# frag to ROD mapping
if not 'TileMap' in dir():
    ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc
    ByteStreamCnvSvc.ROD2ROBmap = [ "-1" ]
else:
    include( TileMap )

#=============================================================
#=== Calib alg setup
#=============================================================

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TileCalibAlgs.TileCalibAlgsConf import TileTopCalibAlg
from TileCalibAlgs.TileCalibAlgsConf import TileLaserTimingTool

topCalibAlg = TileTopCalibAlg( "TileTopCalibAlg" )
topCalibAlg.FileName = "/tmp/TileLaserTimingNtuple.root"
topSequence += topCalibAlg

oTileLaserTimingTool = TileLaserTimingTool("oTileLaserTimingTool")
oTileLaserTimingTool.OutputLevel = DEBUG#VERBOSE
oTileLaserTimingTool.RawChannelContainer ="TileRawChannelFit"
oTileLaserTimingTool.DigitsContainer = "TileDigitsCnt"
oTileLaserTimingTool.NtupleID = "h3000"
oTileLaserTimingTool.FiberLightSpeed=22.5
oTileLaserTimingTool.NSamples = 7
oTileLaserTimingTool.EneLowLimitPulseShape = 0.000001

ToolSvc += oTileLaserTimingTool;

topCalibAlg.TileCalibTools += [ToolSvc.oTileLaserTimingTool];

#-----------------------
# And some final options
#-----------------------

theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 


# ByteStreamInputSvc.DumpFlag = TRUE
# from AthenaCommon.AppMgr import ToolSvc
# ToolSvc.TileROD_Decoder.VerboseOutput = TRUE

print topSequence

EventSelector.SkipEvents = EvtMin
EventSelector.MaxBadEvents = 10000
theApp.EvtMax=EvtMax

