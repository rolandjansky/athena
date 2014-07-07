# Last update: Caroline Collard 2009-03-31

import commands
###########################################################################
include("LArCalibProcessing/LArCalib_Flags.py")

# output POOL file:
if not 'writePOOLfile' in dir():
    writePOOLfile = True
    
if not 'POOLFileOUT' in dir():
    POOLFileOUT = "LArParams_0_HEC.pool.root"

if not 'NTFileOUT' in dir():
    NTFileOUT = "LArParams_0_HEC_new.root"
    
if not 'writeNTuple' in dir():
    writeNTuple  = True

if not 'NTName' in dir():
   NTName = "PARAMS"

if not 'NTFileIN' in dir():
    NTFileIN = "LArParams_0_HEC.root"
    
if not 'OutputKey_Cali' in dir():
   OutputKey_Cali = "LArCaliPulseParams_RTM"

if not 'OutputKey_Det' in dir():
   OutputKey_Det = "LArDetCellParams_RTM"

if not 'OutputFolder_Cali' in dir():
   OutputFolder_Cali="/LAR/ElecCalibOfl/CaliPulseParams/RTM"

if not 'OutputFolder_Det' in dir():
   OutputFolder_Det="/LAR/ElecCalibOfl/DetCellParams/RTM"

if not 'OutputTag' in dir():
   OutputTag="comm-08"

if not 'GroupingType' in dir():
   GroupingType = "ExtendedSubDetector"

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

include( "LArCalibProcessing/LArCalib_MinimalSetup.py")

if not 'OutputDB' in dir():
    OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
    OutputDB = "sqlite://;schema="+OutputSQLiteFile+";dbname=COMP200"

###########################################################################
#                                                                         #
#    Input conditions data (CaliWave) for COOL and/or POOL                #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
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




#########################################################################################################
#
# Reading Ntuple ...
#########################################################################################################

from LArCalibTools.LArCalibToolsConf import LArParamsFromStdNtuple
LArParamsFromTuple = LArParamsFromStdNtuple()
LArParamsFromTuple.NtupleName = NTName
LArParamsFromTuple.FileNames = [ NTFileIN ]
LArParamsFromTuple.StoreKey_Cali = OutputKey_Cali
LArParamsFromTuple.StoreKey_Det = OutputKey_Det
LArParamsFromTuple.GroupingType = GroupingType

topSequence += LArParamsFromTuple

###########################################################################
#
# Writing to Ntuple...
#
###########################################################################

if ( writeNTuple ):

    from LArCalibTools.LArCalibToolsConf import LArParams2Ntuple
    LArParams2Ntuple = LArParams2Ntuple( "LArParams2Ntuple" )
    LArParams2Ntuple.NtupleName   = "PARAMS" 
    LArParams2Ntuple.KeyList      = ["LArCaliPulseParamsComplete", "LArDetCellParamsComplete"]
    LArParams2Ntuple.AllChannels2Ntuple = False
    LArParams2Ntuple.DBKeysList = ["LArCaliPulseParams_RTM","LArDetCellParams_RTM"]

    topSequence += LArParams2Ntuple


    theApp.HistogramPersistency = "ROOT"
    from GaudiSvc.GaudiSvcConf import NTupleSvc
    svcMgr += NTupleSvc()
    svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+NTFileOUT+"' OPT='NEW'" ]


###########################################################################
#
# writing to POOL file...
#
###########################################################################

if ( writePOOLfile ):

    OutputObjectSpecCali   = "LArCaliPulseParamsComplete#"+OutputKey_Cali+"#"+ OutputFolder_Cali
    OutputObjectSpecTagCali    = LArCalibFolderTag(OutputFolder_Cali,OutputTag)
    
    OutputObjectSpecDet   = "LArDetCellParamsComplete#"+OutputKey_Det+"#"+ OutputFolder_Det
    OutputObjectSpecTagDet    = LArCalibFolderTag(OutputFolder_Det,OutputTag)
    
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    theOutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",POOLFileOUT,
                                                 [OutputObjectSpecCali],[OutputObjectSpecTagCali],True)

    theOutputConditionsAlg.Run1 = 0

    theOutputConditionsAlg1 = OutputConditionsAlg("OutputConditionsAlg1",POOLFileOUT,
                                                 [OutputObjectSpecDet],[OutputObjectSpecTagDet],True)

    theOutputConditionsAlg1.Run1 = 0

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

