#RunNumber=84404 #EBA
#RunNumber=84436 #LBA
#RunNumber=84449 #LBC
#RunNumber=84412 #EBC
RunNumber=161043
TilePedRun=True
#doPerfMon=True
EvtMin=1 # some number
EvtMax=200 #some events
OutputLevel = 4

#**************************************************************
#
# jopOptions file for TileCal commissioning analysis
#
#==============================================================

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()


if not 'OutputLevel' in dir():
    OutputLevel = 4

if not 'RunFromLocal' in dir():
    RunFromLocal=False

if not 'RunNumber' in dir():
    RunNumber = 0

if RunNumber>40000:
    if not 'TileFrameLength' in dir():
        TileFrameLength = 7; # expected value in all runs starting from M6

if not 'TileUseCOOL' in dir():
    TileUseCOOL=True; # don't use COOL DB by default for commissioning
    
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
    elif RunNumber<99956:
        InputDirectory = "/castor/cern.ch/atlas/testbeam/tilecal/2008/daq"
    elif RunNumber<142681:
        InputDirectory = "/castor/cern.ch/grid/atlas/DAQ/tile/2009/daq"
    else:
        InputDirectory = "/castor/cern.ch/grid/atlas/DAQ/tile/2010/daq"

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
    if not 'doTileOptATLAS' in dir():
        doTileOptATLAS=True
    if not 'doTileOpt2' in dir():
        doTileOpt2=True
    if not 'TileFrameLength' in dir():
        TileFrameLength=7

elif TilePedRun:
    TileRunType = 4
    if not 'doTileOptATLAS' in dir():
        doTileOptATLAS=True
    if not 'doTileOpt2' in dir():
        doTileOpt2=True
    if not 'TileFrameLength' in dir():
        TileFrameLength=7
    
elif TileLasRun:
    TileRunType = 2
    if not 'doTileOptATLAS' in dir():
        doTileOptATLAS=True
    if not 'doTileOpt2' in dir():
        doTileOpt2=True
    if not 'TileFrameLength' in dir():
        TileFrameLength=7
    
elif TileLedRun:
    TileRunType = 2
    if not 'doTileOptATLAS' in dir():
        doTileOptATLAS=True
    if not 'doTileOpt2' in dir():
        doTileOpt2=True
    if not 'TileFrameLength' in dir():
        TileFrameLength=7
    
else:
    if TilePhysRun:
        TileRunType = 1
        if not 'doTileOptATLAS' in dir():
            doTileOptATLAS=True
        if not 'doTileOpt2' in dir():
            doTileOpt2=True

    if not 'TileFrameLength' in dir():
        TileFrameLength = 7; # the value we expect in single gain run

    if not 'doTileOptATLAS' in dir():
        doTileOptATLAS = True

    if not 'doTileOpt2' in dir():
        doTileOpt2 = True

    if not 'doTileFit' in dir():
        doTileFit = True

# special options
# which algorithms to run 
# and output from which algorithm to use as input for TileCellBuilder
# by default we use just fit method, which gives the best results
# but for phys run we use all 3 methods (see above)

if not 'doTileOptATLAS' in dir():
    doTileOptATLAS = False

if not 'doTileOpt2' in dir():
    doTileOpt2 = False

if not 'doTileFit' in dir():
    doTileFit = True

if not 'useRODReco' in dir():
    useRODReco = False

recoMethod="fit"
processAllChannels=True
    
# all other parameters which can be set to True or False
# from the command line 

# use PMT ordering in ntuple (convert channel to PMT number) 
if not 'doTileCable' in dir():
    doTileCable = True

# keep ADC counts by default
if not 'TileCalibrateEnergy' in dir():
    TileCalibrateEnergy = False


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
conddb.setGlobalTag('COMCOND-ES1C-001-00')

#=============================================================
#=== setup all options for optimal filter
#=============================================================

from TileRecUtils.TileRecFlags import jobproperties

jobproperties.TileRecFlags.doTileFit=doTileFit
jobproperties.TileRecFlags.doTileOpt2=doTileOpt2
jobproperties.TileRecFlags.doTileOptATLAS=doTileOptATLAS
jobproperties.TileRecFlags.doTileManyAmps=False
jobproperties.TileRecFlags.doTileFlat=False

jobproperties.TileRecFlags.calibrateEnergy=TileCalibrateEnergy
jobproperties.TileRecFlags.correctTime=False; # do not apply time correction
jobproperties.TileRecFlags.correctAmplitude=False; # don't do parabolic correction in OptATLAS

jobproperties.print_JobProperties('tree&value')

#=============================================================
#=== setup TileConditions
#=============================================================
TileBchlList='TileNoBad.oflBch'; # dummy file with bad channels by default
include( "TileConditions/TileConditions_jobOptions.py" )
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


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
TileDQstatusAlgDefault()


#Add Noise Calib Tool
from TileCalibAlgs.TileCalibAlgsConf import TileDigiNoiseCalibAlg
from TileCalibAlgs.TileCalibAlgsConf import TileRawChNoiseCalibAlg

theTileDigiNoiseCalibAlg = TileDigiNoiseCalibAlg( "theTileDigiNoiseCalibAlg" )
theTileDigiNoiseCalibAlg.DoAvgCorr = True
topSequence += theTileDigiNoiseCalibAlg

theTileRawChNoiseCalibAlg = TileRawChNoiseCalibAlg( "theTileRawChNoiseCalibAlg" )
topSequence += theTileRawChNoiseCalibAlg


#-----------------------
# And some final options
#-----------------------

if 'doPerfMon' in dir():
    if doPerfMon:

        from GaudiSvc.GaudiSvcConf import AuditorSvc
        theAuditorSvc = svcMgr.AuditorSvc
        theApp.AuditAlgorithms=True  
        theApp.AuditServices=True
        theApp.AuditTools=True  

        from AthenaCommon import CfgMgr
        theAuditorSvc += CfgMgr.ChronoAuditor()
        theAuditorSvc += CfgMgr.NameAuditor()
        theAuditorSvc += CfgMgr.MemStatAuditor()
        from PerfMonComps.PerfMonFlags import jobproperties
        jobproperties.PerfMonFlags.OutputFile = "perfmon_ntuple.root"
        jobproperties.PerfMonFlags.doMonitoring = True
        jobproperties.PerfMonFlags.doFastMon = False
        include( "PerfMonComps/PerfMonSvc_jobOptions.py" )

# ByteStreamInputSvc.DumpFlag = TRUE
# from AthenaCommon.AppMgr import ToolSvc
# ToolSvc.TileROD_Decoder.VerboseOutput = TRUE


#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)

svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

print topSequence

EventSelector.SkipEvents = EvtMin
EventSelector.MaxBadEvents = 10000
theApp.EvtMax=EvtMax

