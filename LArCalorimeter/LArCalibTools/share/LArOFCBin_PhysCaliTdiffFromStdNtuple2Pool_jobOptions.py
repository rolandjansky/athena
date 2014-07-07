# Last update: Caroline Collard 2009-03-31

import commands
###########################################################################
include("LArCalibProcessing/LArCalib_Flags.py")

# output POOL file:
if not 'writePOOLfile' in dir():
    writePOOLfile = True
    
if not 'POOLFileOUT' in dir():
    POOLFileOUT = "LArParams_0.pool.root"

if not 'NTFileOUT' in dir():
    NTFileOUT = "LArParams_0_new.root"
    
if not 'writeNTuple' in dir():
    writeNTuple  = True

if not 'NTName' in dir():
   NTName = "PARAMS"

if not 'NTFileIN' in dir():
    NTFileIN = "LArParams_0.root"
    
if not 'StoreOFC' in dir():
   StoreOFC = True

if StoreOFC:
   if not 'OutputKey_OFCBin' in dir():
      OutputKey_OFCBin = "LArOFCPhase"
   if not 'OutputFolder_OFCBin' in dir():
      OutputFolder_OFCBin="/LAR/ElecCalibOfl/OFCBin/Dummy"


if not 'StoreTdiff' in dir():
   StoreTdiff = True

if StoreTdiff:   
   if not 'OutputKey_Tdiff' in dir():
      OutputKey_Tdiff = "LArPhysCaliTdiff"
   if not 'OutputFolder_Tdiff' in dir():
      OutputFolder_Tdiff="/LAR/ElecCalibOfl/PhysCaliTdiff"


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

from LArCalibTools.LArCalibToolsConf import LArOFCBin_PhysCaliTdiffFromStdNtuple
LArParamsFromTuple = LArOFCBin_PhysCaliTdiffFromStdNtuple()
LArParamsFromTuple.NtupleName = NTName
LArParamsFromTuple.FileNames = [ NTFileIN ]
LArParamsFromTuple.Store_OFC = StoreOFC
LArParamsFromTuple.Store_Tdiff = StoreTdiff
if StoreOFC:
   LArParamsFromTuple.StoreKey_OFC = OutputKey_OFCBin

if StoreTdiff:
   LArParamsFromTuple.StoreKey_Tdiff = OutputKey_Tdiff

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
    LArParams2Ntuple.KeyList      = ["LArOFCBinComplete", "LArPhysCaliTdiffComplete"]
    LArParams2Ntuple.AllChannels2Ntuple = False
    LArParams2Ntuple.DBKeysList = [ "", "", "LArPhysCaliTdiff",
                    "", "", "", "", "", "", "", "", "", "", "LArOFCPhase"]

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

    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

    if StoreOFC:
       OutputObjectSpecOFC    = "LArOFCBinComplete#"+OutputKey_OFCBin+"#"+ OutputFolder_OFCBin
       OutputObjectSpecTagOFC = LArCalibFolderTag(OutputFolder_OFCBin,OutputTag)
       theOutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",POOLFileOUT,
                                                 [OutputObjectSpecOFC],[OutputObjectSpecTagOFC],True)
       theOutputConditionsAlg.Run1 = 0
    
    if StoreTdiff:   
       OutputObjectSpecTdiff    = "LArPhysCaliTdiffComplete#"+OutputKey_Tdiff+"#"+ OutputFolder_Tdiff
       OutputObjectSpecTagTdiff = LArCalibFolderTag(OutputFolder_Tdiff,OutputTag)
       theOutputConditionsAlg1 = OutputConditionsAlg("OutputConditionsAlg1",POOLFileOUT,
                                                 [OutputObjectSpecTdiff],[OutputObjectSpecTagTdiff],True)
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

