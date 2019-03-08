#**************************************************************
#
# jopOptions file for reading ByteStream 
#
#==============================================================

from os import system,popen
from subprocess import check_output
from subprocess import CalledProcessError

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()

from AthenaCommon.Logging import logging
tbdump_log = logging.getLogger( 'jobOptions_TileTBDump.py' )

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 3
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False


if not 'RunNumber' in dir():
    RunNumber = 0

if not 'RUN2' in dir(): 
    RUN2 = (RunNumber>232000) or (RunNumber<=0) or (RunNumber==2)

if not 'MC' in dir():
    MC = (RunNumber>0) and (RunNumber<10)

if 'TilePhysRun' in dir():
    if TilePhysRun:
        TileRunType = 1
        if not 'RunStream' in dir():
           RunStream = "physics_L1Calo"
        if not 'DataProject' in dir():
           DataProject = "data15_cos"
        if not 'InputDirectory' in dir():
           if RunNumber<10:
                InputDirectory = "."
                RunFromLocal=True
           elif RunNumber<10800:
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
           elif RunNumber<75354:
                InputDirectory = "/castor/cern.ch/grid/atlas/DAQ/M7"
           elif RunNumber<92227:
                InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/2008/%(run)s/%(stream)s"   % { 'run': RunNumber, 'stream': RunStream })
           elif RunNumber<100000:
                InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/2008/00%(run)s/%(stream)s" % { 'run': RunNumber, 'stream': RunStream })
           elif RunNumber<142407:
                InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/2009/00%(run)s/%(stream)s" % { 'run': RunNumber, 'stream': RunStream })
           elif RunNumber<171194:
                InputDirectory = ( "/castor/cern.ch/grid/atlas/tzero/prod1/perm/%(project)s/%(stream)s/0%(run)s/%(project)s.00%(run)s.%(stream)s.merge.RAW" % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber })
           elif RunNumber < 254945:
                InputDirectory = ( "/castor/cern.ch/grid/atlas/tzero/prod1/perm/%(project)s/%(stream)s/00%(run)s/%(project)s.00%(run)s.%(stream)s.merge.RAW" % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber })
           else:
               if not 'DirectorySuffix' in dir():
                   if RunStream.startswith('calibration'): DirectorySuffix = 'daq.RAW'
                   else: DirectorySuffix = 'merge.RAW'
                   log.warning('DirectorySuffix is not set up and will be used: %(suff)s' % {'suff': DirectorySuffix})

               InputDirectory = ( "/eos/atlas/atlastier0/rucio/%(project)s/%(stream)s/00%(run)s/%(project)s.00%(run)s.%(stream)s.%(suff)s" 
                                  % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber, 'suff': DirectorySuffix })
else:
    TilePhysRun=False

if not 'InputDirectory' in dir():
    if RunNumber < 10:
        InputDirectory = "."
        RunFromLocal = True
    elif RunNumber < 36127:
        InputDirectory = "/castor/cern.ch/atlas/testbeam/tilecal/2007/daq"
    elif RunNumber < 100000:
        InputDirectory = "/castor/cern.ch/atlas/testbeam/tilecal/2008/daq"
    else:
        if RunNumber < 142682:
            Year = 2009
        elif RunNumber < 171194:
            Year = 2010
        elif RunNumber < 194688:
            Year = 2011
        elif RunNumber < 216816:
            Year = 2012            
        elif RunNumber < 224305:
            Year = 2013            
        elif RunNumber < 248584:
            Year = 2014
        elif RunNumber < 287952:
            Year = 2015
        elif RunNumber < 314450:
            Year = 2016
        elif RunNumber < 342540:
            Year = 2017
        elif RunNumber < 367980:
            Year = 2018
        else:
            Year = 2019


        if 'RunStream' in dir():
            if RunStream == 'l1calo' or RunStream == 'L1Calo':
                InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/l1calo/00%(run)s" % { 'run': RunNumber })
            elif RunStream.startswith('calibration_L1Calo'):
                if not 'DataProject' in dir(): 
                    DataProject = 'data15_calib'
                    log.warning('DataProject is not set up and will be used: %(project)s' % {'project': DataProject})

                InputDirectory = ( "/eos/atlas/atlastier0/rucio/%(project)s/%(stream)s/00%(run)s" 
                                   % {'project': DataProject, 'stream': RunStream, 'run': RunNumber })
            else:
                if RunNumber < 254372:
                    InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/%(year)s/00%(run)s/%(stream)s" 
                                       % { 'year': Year, 'run': RunNumber, 'stream': RunStream })
                else:
                    if not 'DataProject' in dir():
                        DataProject = 'data15_cos'
                        log.warning('DataProject is not set up and will be used: %(project)s' % {'project': DataProject})
                    
                    InputDirectory = ( "/eos/atlas/atlastier0/rucio/%(project)s/%(stream)s/00%(run)s/%(project)s.00%(run)s.%(stream)s.daq.RAW" 
                                       % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber })
                    
        else:
            if RunNumber < 270317:
                InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/tile/%(year)s/daq" % { 'year': Year })
            else:
                InputDirectory = ( '/eos/atlas/atlascerngroupdisk/det-tile/online/%(year)s/daq' % { 'year': Year })

if not 'FileFilter' in dir():
    FileFilter = ".data"

if not 'RunFromLocal' in dir():
    if InputDirectory==".":
        RunFromLocal=True
    else:
        RunFromLocal=False



def FindFile(path, runinput, filter):

    run = str(runinput).zfill(7)
    if len(filter)<1: filter='.'

    files = []
    fullname = []

    try:
        if RunFromLocal:
            files = check_output('ls %(path)s | grep %(run)s | grep %(filt)s' % {'path': path, 'run':run, 'filt':filter}, shell = True).splitlines()
        elif (path.startswith('/eos/')):
            files = check_output('xrdfs eosatlas ls -l %(path)s | grep %(run)s | grep -v "#" | grep -v -e "         [ 0-9][ 0-9][0-9] " | grep %(filt)s | sed "s|^.*/||" ' % {'path':path, 'run':run, 'filt':filter}, shell = True).splitlines()
        else:
            files = check_output('nsls %(path)s | grep %(run)s | grep -v -e "               [ 0-9][ 0-9][0-9] " | grep %(filt)s ' % {'path': path, 'run':run, 'filt':filter  }, shell = True).splitlines()

    except CalledProcessError:
        files = []
        log.warn('Seems there are no such directory: ' + path)


    for file_name in (files):
        if (path.startswith('/eos/')):
            fullname.append('root://eosatlas.cern.ch/' + path + '/' + file_name)
        else:
            fullname.append(path + '/' + file_name)

    return [fullname, run]


if not 'FileNameVec' in dir():
    if not 'FileName' in dir() or FileName == "":    
        tmp = FindFile(InputDirectory, RunNumber, FileFilter)
        FileNameVec = tmp[0]
        FormattedRunNumber = tmp[1]
    else:
        FileNameVec = [ FileName ]
        FormattedRunNumber = RunNumber
else:
    FormattedRunNumber = RunNumber

tbdump_log.info("InputDirectory is " + str(InputDirectory))
tbdump_log.info("RunNumber was " + str(RunNumber))
tbdump_log.info("RunNumber is " + str(FormattedRunNumber))
tbdump_log.info("FullFileName is " + str(FileNameVec))

if len(FileNameVec) < 1:
    tbdump_log.fatal("Input file not found")
    sys.exit(1)

if not 'EvtMin' in dir():
    EvtMin = 0
elif (EvtMin < 0):
    EvtMin = 0
    noDump = True

if not 'EvtMax' in dir():
    EvtMax = 3
elif (EvtMax < 1):
    EvtMax = 100000
    dumpOnce = True

# override run type with one of these flags
if not 'TileCisRun' in dir():
    TileCisRun = False
if not 'TileMonoRun' in dir():
    TileMonoRun=False
if not 'TilePedRun' in dir():
    TilePedRun=False
if not 'TileLasRun' in dir():
    TileLasRun=False
if not 'TilePhysRun' in dir():
    TilePhysRun=False
if not 'TileRampRun' in dir():
    TileRampRun=False
if not 'TileL1CaloRun' in dir():
    TileL1CaloRun=False

TileLasPulse = TileLasRun
TileCisPulse = (TileCisRun or TileMonoRun or TileRampRun or TileL1CaloRun)

#=============================================================
#=== init Det Descr
#=============================================================

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
if MC:
    globalflags.DataSource.set_Value_and_Lock('geant4')
else:
     globalflags.DataSource.set_Value_and_Lock('data')
if RUN2: globalflags.DatabaseInstance="CONDBR2"
else:    globalflags.DatabaseInstance="COMP200"

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.beamType.set_Value_and_Lock('cosmics')

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
if RUN2: jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-04-00-00"
else:    jobproperties.Global.DetDescrVersion = "ATLAS-R1-2012-02-00-00"
tbdump_log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion()) )

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True
tbdump_log.info( "GeoModelSvc.AtlasVersion = %s" % (GeoModelSvc.AtlasVersion) )

from RecExConfig.RecFlags import rec
if RUN2: rec.projectName = "data15_tilecomm"
else:    rec.projectName = "data12_tilecomm"

from IOVDbSvc.CondDB import conddb
if MC:     conddb.setGlobalTag("OFLCOND-RUN12-SDR-25")
elif RUN2: conddb.setGlobalTag("CONDBR2-BLKPA-2018-03")
else:      conddb.setGlobalTag("COMCOND-BLKPA-RUN1-06")

#=============================================================
#=== setup TileConditions
#=============================================================
TileUseCOOL=True
include( "TileConditions/TileConditions_jobOptions.py" )
tileInfoConfigurator.OutputLevel = OutputLevel

from AthenaCommon.AppMgr import ToolSvc
from TileConditions.TileCondToolConf import *

print tileInfoConfigurator

#=============================================================
#=== ByteStream Input
#=============================================================
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
theApp.ExtSvc += [ "ByteStreamCnvSvc" ] 
svcMgr.ByteStreamInputSvc.FullFileName += FileNameVec
svcMgr.ByteStreamCnvSvc.ROD2ROBmap = [ "-1" ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["TileBeamElemContainer/TileBeamElemCnt",
                                                  "TileRawChannelContainer/TileRawChannelCnt",
                                                  "TileDigitsContainer/TileDigitsCnt",
                                                  "TileL2Container/TileL2Cnt",
                                                  "TileLaserObject/TileLaserObj" ]


#----------------
# Add Algorithms
#----------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if not 'noDump' in dir():
    from TileTBRec.TileTBRecConf import TileTBDump
    theTileTBDump = TileTBDump()
    topSequence += theTileTBDump
    theTileTBDump.OutputLevel = 2
    if not 'dumpOnce' in dir():
        theTileTBDump.dumpOnce = False
    else:
        theTileTBDump.dumpOnce = True
    theTileTBDump.dumpUnknown = False

svcMgr.EventSelector.OutputLevel = 2
svcMgr.ByteStreamInputSvc.OutputLevel = 3

if OutputLevel < 2:
    from TileByteStream.TileByteStreamConf import TileROD_Decoder
    ToolSvc += TileROD_Decoder()
    ToolSvc.TileROD_Decoder.VerboseOutput = True
    #svcMgr.ByteStreamInputSvc.DumpFlag = True
else:
    svcMgr.ByteStreamInputSvc.DumpFlag = False

svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.EventSelector.MaxBadEvents = 10000
svcMgr.EventSelector.ProcessBadEvent = True
svcMgr.EventSelector.SkipEvents = EvtMin
theApp.EvtMax = EvtMax

if not 'db' in dir():
    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    svcMgr += DBReplicaSvc(UseCOOLSQLite=False)
