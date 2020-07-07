###########################################################################
#
# LArCalib_Pedestal2Ntuple_jobOptions.py
#
# Last update: 21/11/2007 <Marco.Delmasytro@cern.ch>
#
# DO NOT MODIFY THIS JOBOPTIONS FILE, USE YOUR OWN TOPOPTIONS FILE!
#
###########################################################################

if not 'ReadCOOL' in dir():
    ReadCOOL = True

if not 'RunNumber' in dir():
    RunNumber = 0

if not 'ChannelSelection' in dir():
   ChannelSelection = " "
   # example to read only Barrel C (Strips->Back)
   #ChannelSelection = "<channelSelection>0,3:34</channelSelection>"
   
if not 'PedestalFolder' in dir():
    PedestalFolder = "/LAR/ElecCalib/Pedestals/Pedestal"

if not 'PedestalFolderTag' in dir():
    PedestalFolderTag = "-comm-05"

if not 'DBConnectionCOOL' in dir():
    DBConnectionCOOL = "impl=cool;techno=oracle;schema=ATLAS_COOLONL_LAR;ATLAS_COOLPROD:COMP200:ATLAS_COOL_READER:"
    
if not 'PoolFileDir' in dir():
    PoolFileDir = "/afs/cern.ch/user/l/larp3c/maxidisk/poolFiles/Barrel-22/29142_29143_29146"

if not 'PoolFileName' in dir():
    PoolFileName = "LArPedestal_29142_EB-EMBA.pool.root"
        
if not 'ContainerKey' in dir():
    #ContainerKey = "Pedestal" # deprecated, do not use!
    ContainerKey = "LArPedestal"
    
if not 'NtupleFileName' in dir():
    NtupleFileName = "LArPedestal2Ntuple.root" 

###########################################################################
#
# Global settings
#
###########################################################################

if not "DetDescrVersion" in dir():
    DetDescrVersion = 'ATLAS-Comm-00-00-00'

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

# Set up GeoModel (not really needed but crashes without)
include ("AtlasGeoModel/SetGeometryVersion.py")
include ("AtlasGeoModel/GeoModelInit.py")

# Get identifier mapping
include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")
include("LArIdCnv/LArIdCnv_joboptions.py")

ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print = False

###########################################################################
#
# Algorithms sequencer
#
###########################################################################

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

###########################################################################
#
# Read Pedestals from COOL(Oracle/SQLite) or POOL
#
###########################################################################

from string import *

def FolderTag(folder,tag):
    return join(split(folder, '/'),'') + tag;

def DBConnectionFile(sqlitefile):
    return "impl=cool;techno=sqlite;schema="+sqlitefile+";X:XPTWK130";

if 'SQLiteFile' in dir():
        DBConnection = DBConnectionFile(SQLiteFile)
else:
        DBConnection = DBConnectionCOOL

if (ReadCOOL):

    include ("IOVDbSvc/IOVDbSvc_jobOptions.py")
    
    #if ( ReadCOOL and not 'SQLiteFile' in dir() ):
    #    PoolSvc = Service ( "PoolSvc" )
    #    PoolSvc.ReadCatalog += [ "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond/PoolCat_comcond_castor.xml" ]

    PedestalTagSpec = FolderTag(PedestalFolder,PedestalFolderTag)

    IOVDbSvc.Folders += [ PedestalFolder+"<tag>"+PedestalTagSpec+"</tag>"+"<dbConnection>"+DBConnection+"</dbConnection>"+ChannelSelection ]
    IOVDbSvc.OutputLevel = DEBUG

else:
    include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")

    EventPersistencySvc = Service( "EventPersistencySvc" )
    EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]
    
    ProxyProviderSvc = Service( "ProxyProviderSvc" )
    ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
    CondProxyProvider = Service( "CondProxyProvider" )
    CondProxyProvider.InputCollections += [ PoolFileDir+"/"+PoolFileName ]

###########################################################################
#
# Dump pedestal DB folder(s) to a ROOT ntuple
#
###########################################################################

from LArCalibTools.LArCalibToolsConf import LArPedestals2Ntuple
LArPedestals2Ntuple = LArPedestals2Ntuple("LArPedestals2Ntuple")
LArPedestals2Ntuple.ContainerKey = ContainerKey
topSequence += LArPedestals2Ntuple

###########################################################################
#
# Write out to ROOT file
#
###########################################################################

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='"+NtupleFileName+"' OPT='NEW'" ]

###########################################################################
#
# Use EventSelector to select IOV 
#
###########################################################################     

if (ReadCOOL):

    EventSelector = Service( "EventSelector" )
    EventSelector.RunNumber = RunNumber
    EventSelector.EventsPerRun      = 1
    EventSelector.FirstEvent        = 1
    EventSelector.InitialTimeStamp  = 0
    EventSelector.TimeStampInterval = 1

###########################################################################

theApp.EvtMax = 1

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

###########################################################################

StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True

DetectorStore = Service( "DetectorStore" )
DetectorStore.Dump = True

ConditionStore = Service( "ConditionStore" )
ConditionStore.Dump = True

###########################################################################
