
# Set up minimal environment, no event loop, no event selector
theApp.EvtSel = "NONE"
theApp.EvtMax=1

# Necessary libraries
#theApp.Dlls += ["CaloRec","CaloClusterCorrection","CaloCondAthenaPoolPoolCnv"]


from JetRec.JetRecFlags import jetFlags
from JetRec.JetGetters import make_StandardJetGetter

make_StandardJetGetter('AntiKt',0.4, 'Topo',outputCollectionName='AntiKt4TopoJetsNew', addJetQualityMoments=True, doCalib=True, calibName='EM:ETAJES')

from JetCalibTools.JetCalibToolsConf import JetCalibCorrDBWriter

#from AthenaCommon.AlgSequence import AlgSequence
#topalg = AlgSequence ("TopAlg")
from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore
oks = ObjKeyStore()
oks.addStreamESD('CaloClusterContainer', ['CaloTopoCluster'] )

corr_output_list=[]
tag_list=[]

# here make a loop over the configured tools (which will be filled from jO
# and create for each of them writer algo + add the proper specification to corr_output_list - key and folder

jetCorrDBWrite = JetCalibCorrDBWriter()
# include the basic setup for the conditions output stream
#pool_file = CALOCORR_POOLFILE
#if pool_file.startswith ('LFN:'):
#    pool_file = pool_file[4:] + '.pool.root'
pool_file="mytest.pool.root"
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg = OutputConditionsAlg (outputFile = pool_file,
                                           ObjectList = corr_output_list,
                                           WriteIOV = False)

#OutputConditionsAlg.OutputLevel=DEBUG
#ToolSvc.ConditionsAlgStream.OutputLevel=DEBUG


from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.DetectorStore.Dump = True

try:
    ll = theApp.OutStream
    ll.remove ('AthenaOutputStream/Stream1')
    theApp.OutStream = ll
except ValueError:
    pass

include ("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

# Now to handle the cool registration.
# Set flag to register and run interval Run1/LB1 to Run2/LB2
#   Usually, only need to set Run1, others go to default
OutputConditionsAlg.WriteIOV   = True
OutputConditionsAlg.Run1       = IOVRunNumberMin 
OutputConditionsAlg.LB1        = IOVLBNumberMin
OutputConditionsAlg.Run2       = IOVRunNumberMax
OutputConditionsAlg.LB2        = IOVLBNumberMax

# Tags need to be globally unique!
OutputConditionsAlg.IOVTagList = tag_list

# Set the connection string
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
IOVDbSvc = Service( "IOVDbSvc" )
#IOVDbSvc.OutputLevel=DEBUG

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
IOVRegistrationSvc().writeKeyInfo = False

#IOVRegSvc.RecreateFolders = True
IOVDbSvc.dbConnection = "sqlite://;schema=swcool.db;dbname=OFLP200"
