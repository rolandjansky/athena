#**************************************************************
#
# jopOptions file for reading ByteStream 
#
#==============================================================

from os import system,popen

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()

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
             
if 'TilePhysRun' in dir():
    if TilePhysRun:
        TileRunType = 1
        if not 'RunStream' in dir():
           RunStream = "physics_L1Calo"
        if not 'DataProject' in dir():
           DataProject = "data11_7TeV"
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
           else:
                InputDirectory = ( "/castor/cern.ch/grid/atlas/tzero/prod1/perm/%(project)s/%(stream)s/00%(run)s/%(project)s.00%(run)s.%(stream)s.merge.RAW" % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber })
else:
    TilePhysRun=False

if not 'InputDirectory' in dir():
    if RunNumber<10:
        InputDirectory = "."
        RunFromLocal=True
    elif RunNumber<36127:
        InputDirectory = "/castor/cern.ch/atlas/testbeam/tilecal/2007/daq"
    elif RunNumber<100000:
        InputDirectory = "/castor/cern.ch/atlas/testbeam/tilecal/2008/daq"
    else:
        if RunNumber<142682:
            Year=2009
        elif RunNumber<171194:
            Year=2010
        elif RunNumber<194688:
            Year=2011
        else:
            Year=2012
        if 'RunStream' in dir():
            if RunStream == 'l1calo' or RunStream == 'L1Calo':
                InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/l1calo/00%(run)s" % { 'run': RunNumber })
            else:
                InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/%(year)s/00%(run)s/%(stream)s" % { 'year': Year, 'run': RunNumber, 'stream': RunStream })
        else:
            InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/tile/%(year)s/daq" % { 'year': Year })

if not 'FileFilter' in dir():
    FileFilter = ".data"

if not 'RunFromLocal' in dir():
    if InputDirectory==".":
        RunFromLocal=True
    else:
        RunFromLocal=False

def FindFile(path,runinput,filter):

    run=str(runinput)

    while len(run)<7:
        run='0'+run
        
    files=[]
    fullname=[]

    if RunFromLocal:
        for f in popen('ls %(path)s | grep %(run)s | grep %(filt)s' % {'path': path, 'run':run, 'filt':filter }):
            files.append(f)

    else:
        for f in popen('nsls -l %(path)s | grep %(run)s | grep -v "               [ 0-9][ 0-9][0-9] " | grep %(filt)s | cut -c66- | sort -t_ -k4 ' % {'path': path, 'run':run, 'filt':filter }):
            files.append(f)

    for nn in range(len(files)):
        temp=files[nn].split('\n')
        fullname.append(path+'/'+temp[0])

    return [fullname,run]


if not 'FileNameVec' in dir():
    if not 'FileName' in dir() or FileName=="":    
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
globalflags.DetGeo.set_Value_and_Lock('commis')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')

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
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-18-01-00'
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion()) )

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True
log.info( "GeoModelSvc.AtlasVersion = %s" % (GeoModelSvc.AtlasVersion) )
#GeoModelSvc.TileVersionOverride = "TileCal-GEO-05"
#log.info( "GeoModelSvc.TileVersionOverride = %s" % (GeoModelSvc.TileVersionOverride) )

# Set Global tag for IOVDbSvc for commissioning
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('COMCOND-ES1PA-006-01')

#=============================================================
#=== setup TileConditions
#=============================================================
TileUseCOOL=True
include( "TileConditions/TileConditions_jobOptions.py" )
tileInfoConfigurator.OutputLevel = OutputLevel

from AthenaCommon.AppMgr import ToolSvc
from TileConditions.TileCondToolConf import *
if TileLasPulse:
    tileInfoConfigurator.TileCondToolTiming = getTileCondToolTiming( 'COOL','LAS')
    ToolSvc += getTileCondToolOfcCool('COOL', 'LAS')
elif TileCisPulse:
    tileInfoConfigurator.TileCondToolTiming = getTileCondToolTiming( 'COOL','CIS')
    ToolSvc += getTileCondToolOfcCool('COOL', 'CISPULSE100')
else:
    tileInfoConfigurator.TileCondToolTiming = getTileCondToolTiming( 'COOL','PHY')
    ToolSvc += getTileCondToolOfcCool('COOL', 'PHY')

print tileInfoConfigurator

#=============================================================
#=== ByteStream Input
#=============================================================
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
theApp.ExtSvc += [ "ByteStreamCnvSvc" ] 
ByteStreamInputSvc = svcMgr.ByteStreamInputSvc
ByteStreamInputSvc.FullFileName += FileNameVec
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

ByteStreamInputSvc.MaxBadEvents = 10000
ByteStreamInputSvc.DumpFlag = False
theApp.EvtMax = EvtMax

svcMgr.EventSelector.SkipEvents = EvtMin
svcMgr.EventSelector.OutputLevel = 2
ByteStreamInputSvc.OutputLevel = 3
#AthenaEventLoopMgr.OutputLevel = 3
