# Last update: Caroline Collard 2009-03-31

import commands
###########################################################################
include("LArCalibProcessing/LArCalib_Flags.py")

# output POOL file:
if not 'writePOOLfile' in dir():
    writePOOLfile = True
    
if not 'PhysWavePOOLFileOUT' in dir():
    PhysWavePOOLFileOUT = "LArPhysWave_0_HEC.pool.root"

if not 'PhysWaveNTFileOUT' in dir():
    PhysWaveNTFileOUT = "LArPhysWave_0_HEC_new.root"
    
if not 'writeNTuple' in dir():
    writeNTuple  = True

if not 'PhysWaveNTFileIN' in dir():
    PhysWaveNTFileIN = "LArPhysWave_0_HEC.root"
    
if not 'OutputKey' in dir():
   OutputKey = "LArPhysWaveHECIdeal"

if not 'OutputFolder' in dir():
   OutputFolder="/LAR/ElecCalibOfl/PhysWaves/HECIdeal"

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

from LArCalibTools.LArCalibToolsConf import LArPhysWaveFromStdNtuple
LArPhysWaveFromTuple = LArPhysWaveFromStdNtuple()
LArPhysWaveFromTuple.SkipPoints = 0
LArPhysWaveFromTuple.PrefixPoints = 0
LArPhysWaveFromTuple.FileNames = [ PhysWaveNTFileIN ]
LArPhysWaveFromTuple.StoreKey = OutputKey
LArPhysWaveFromTuple.GroupingType = GroupingType

topSequence += LArPhysWaveFromTuple

###########################################################################
#
# Writing to Ntuple...
#
###########################################################################

if ( writeNTuple ):

    from LArCalibTools.LArCalibToolsConf import LArPhysWaves2Ntuple
    LArPhysWaves2Ntuple = LArPhysWaves2Ntuple( "LArPhysWaves2Ntuple" )
    LArPhysWaves2Ntuple.NtupleName   = "PHYSWAVE" 
    # LArPhysWaves2Ntuple.ReadCaliWave = False
    LArPhysWaves2Ntuple.KeyList      = [ OutputKey ]
    LArPhysWaves2Ntuple.SaveDerivedInfo = True
    
    topSequence += LArPhysWaves2Ntuple


    theApp.HistogramPersistency = "ROOT"
    from GaudiSvc.GaudiSvcConf import NTupleSvc
    svcMgr += NTupleSvc()
    svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+PhysWaveNTFileOUT+"' OPT='NEW'" ]


###########################################################################
#
# writing to POOL file...
#
###########################################################################

if ( writePOOLfile ):

    OutputObjectSpecPhysWave   = "LArPhysWaveContainer#"+OutputKey+"#"+ OutputFolder
    OutputObjectSpecTagPhysWave    = LArCalibFolderTag(OutputFolder,OutputTag)
    
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    theOutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",PhysWavePOOLFileOUT,
                                                 [OutputObjectSpecPhysWave],[OutputObjectSpecTagPhysWave],True)

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

