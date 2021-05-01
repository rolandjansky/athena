import commands
###########################################################################
include("LArCalibProcessing/LArCalib_Flags.py")

# output POOL file:
if not 'writePOOLfile' in dir():
    writePOOLfile = True
    
if not 'AutoCorrPOOLFileOUT' in dir():
    AutoCorrPOOLFileOUT = "LArAutoCorr_new.pool.root"

if not 'AutoCorrNTFileOUT' in dir():
    AutoCorrNTFileOUT = "LArAutoCorr_new.root"
    
if not 'writeNTuple' in dir():
    writeNTuple  = True

if not 'AutoCorrNTFileIN' in dir():
    AutoCorrNTFileIN = "LArAutoCorr.root"
    
if not 'OutputKey' in dir():
   OutputKey = "LArAutoCorr"

if not 'OutputFolder' in dir():
   OutputFolder="/LAR/ElecCalibMC/AutoCorr"

if not 'OutputTag' in dir():
   OutputTag="-test"

if not 'GroupingType' in dir():
   GroupingType = "SingleGroup"

if not 'IsComplete' in dir():
   IsComplete = False

if not 'DoSFcal' in dir():
   DoSFcal = True

if not 'DBTag' in dir():   
   DBTag   = LArCalib_Flags.globalFlagDB

if not 'GeomTag' in dir():
   GeomTag = "ATLAS-GEO-21-00-00"

if not 'OutputDB' in dir():
    OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
    if IsComplete:
       OutputDB = "sqlite://;schema="+OutputSQLiteFile+";dbname=COMP200"
    else:
       OutputDB = "sqlite://;schema="+OutputSQLiteFile+";dbname=OFLP200"

###########################################################################
#
# Global settings
#
###########################################################################

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

# this include doesn't work for MC case
## include( "LArCalibProcessing/LArCalib_MinimalSetup.py")
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
if IsComplete:
   globalflags.DataSource.set_Value_and_Lock('data')
   globalflags.InputFormat.set_Value_and_Lock('bytestream')
   globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')
else:
   globalflags.DataSource.set_Value_and_Lock('geant4')
   globalflags.InputFormat.set_Value_and_Lock('pool')


from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = GeomTag

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
#DetFlags.Print()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#Get identifier mapping
if IsComplete:
   include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
else:
   include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

###########################################################################
#                                                                         #
#    Input conditions data (CaliWave) for COOL and/or POOL                #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = DBTag
svcMgr.IOVDbSvc.DBInstance=""

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=COMP200"
conddb.addFolder("","/LAR/BadChannels/BadChannels<dbConnection>"+DBConnectionCOOL+"</dbConnection>")
conddb.addFolder("","/LAR/BadChannels/MissingFEBs<dbConnection>"+DBConnectionCOOL+"</dbConnection>")

svcMgr.PoolSvc.ReadCatalog += ["xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
                               "prfile:poolcond/PoolCat_oflcond.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_diskbuffer_afs.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_cond08_data.000001.lar.COND_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000003.lar_conditions.recon.pool.v0000_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000004.lar_conditions.recon.pool.v0000_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000005.lar_conditions.recon.pool.v0000_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000006.lar_conditions.recon.pool.v0000_castor.xml"]


from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
LArMCSymCondAlgDefault()




#########################################################################################################
#
# Reading Ntuple ...
#########################################################################################################

from LArCalibTools.LArCalibToolsConf import LArAutoCorrFromStdNtuple
LArAutoCorrFromTuple = LArAutoCorrFromStdNtuple()
LArAutoCorrFromTuple.FileNames = [ AutoCorrNTFileIN ]
LArAutoCorrFromTuple.StoreKey = OutputKey
LArAutoCorrFromTuple.GroupingType = GroupingType
LArAutoCorrFromTuple.isComplete = IsComplete
LArAutoCorrFromTuple.doSFcal = DoSFcal

topSequence += LArAutoCorrFromTuple

###########################################################################
#
# Writing to Ntuple...
#
###########################################################################

if ( writeNTuple ):

    from LArCalibTools.LArCalibToolsConf import LArAutoCorr2Ntuple
    LArAutoCorr2Ntuple = LArAutoCorr2Ntuple( "LArAutoCorr2Ntuple" )
    LArAutoCorr2Ntuple.ContainerKey    = OutputKey
    LArAutoCorr2Ntuple.AddFEBTempInfo = False
    LArAutoCorr2Ntuple.AddBadChannelInfo = False
    
    topSequence += LArAutoCorr2Ntuple


    theApp.HistogramPersistency = "ROOT"
    from GaudiSvc.GaudiSvcConf import NTupleSvc
    svcMgr += NTupleSvc()
    svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+AutoCorrNTFileOUT+"' OPT='NEW'" ]


###########################################################################
#
# writing to POOL file...
#
###########################################################################

if ( writePOOLfile ):

    if ( IsComplete ):
       OutputObjectSpecAutoCorr   = "LArAutoCorrComplete#"+OutputKey+"#"+ OutputFolder
    else:   
       OutputObjectSpecAutoCorr   = "LArAutoCorrMC#"+OutputKey+"#"+ OutputFolder
    OutputObjectSpecTagAutoCorr    = LArCalibFolderTag(OutputFolder,OutputTag)
    
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    theOutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",AutoCorrPOOLFileOUT,
                                                 [OutputObjectSpecAutoCorr],[OutputObjectSpecTagAutoCorr],True)

    theOutputConditionsAlg.Run1 = 0

    svcMgr.IOVDbSvc.dbConnection  = OutputDB

    from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
    svcMgr += IOVRegistrationSvc()
    svcMgr.IOVRegistrationSvc.RecreateFolders = True

##########################################################################

theApp.EvtMax = 1

###########################################################################

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

###########################################################################

