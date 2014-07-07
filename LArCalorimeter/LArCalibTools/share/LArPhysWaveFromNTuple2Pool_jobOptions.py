# Last update: Caroline Collard 2009-03-31

import commands
###########################################################################
include("LArCalibProcessing/LArCalib_Flags.py")

# output POOL file:
if not 'writePOOLfile' in dir():
    writePOOLfile = True
    
if not 'PhysWavePOOLFileOUT' in dir():
    PhysWavePOOLFileOUT = "/tmp/collard/PhysWaves_88240_EMBC.pool.root"
    
if not 'writeNTuple' in dir():
    writeNTuple  = True

if not 'PhysWaveNTFile' in dir():
    PhysWaveNTFile = "/tmp/collard/PhysWaves_88240_EMBC.root"
    
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

###########################################################################
#                                                                         #
#    Input conditions data (CaliWave) for COOL and/or POOL                #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=COMP200;user=ATLAS_COOL_READER"
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
theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]
theApp.Dlls += ["LArCalibTools"]

theApp.TopAlg += [ "LArPhysWaveFromTuple" ]
LArPhysWaveFromTuple = Algorithm( "LArPhysWaveFromTuple" )
LArPhysWaveFromTuple.NPoints = 768
LArPhysWaveFromTuple.SkipPoints = 4
LArPhysWaveFromTuple.PrefixPoints = 25
LArPhysWaveFromTuple.deltaT  =  (1./(40.08*1.e-3))/24.
LArPhysWaveFromTuple.LArWaveFlag = 20
LArPhysWaveFromTuple.FileNames = [
 "/tmp/collard/fpm_pulse_mini88240.root"
]
LArPhysWaveFromTuple.StoreKey = "FROMTUPLE"


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
    LArPhysWaves2Ntuple.KeyList      = [ "FROMTUPLE" ]
    LArPhysWaves2Ntuple.SaveDerivedInfo = False
    LArPhysWaves2Ntuple.OutputLevel = VERBOSE
    
    topSequence += LArPhysWaves2Ntuple


    theApp.HistogramPersistency = "ROOT"
    from GaudiSvc.GaudiSvcConf import NTupleSvc
    svcMgr += NTupleSvc()
    svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+PhysWaveNTFile+"' OPT='NEW'" ]


###########################################################################
#
# writing to POOL file...
#
###########################################################################

if ( writePOOLfile ):
    
    OutputObjectSpec    = "LArPhysWaveContainer#FROMTUPLE"
    #OutputObjectSpecTag = ???
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    theOutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",PhysWavePOOLFileOUT,
                                                 [OutputObjectSpec],[],False)

##########################################################################

theApp.EvtMax = 1

###########################################################################

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = DEBUG)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################

