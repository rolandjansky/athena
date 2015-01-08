#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import ToolSvc as toolSvc

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 1


# Detector flags and tags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-R1-2012-02-00-00')
globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-BLKPA-RUN1-06')


from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
#DetFlags.Calo_setOn()
DetFlags.Muon_setOn()

# GeoModel initialisation
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# Needed to call the DB containing the deviations
# example from Monica Verducci
from MuonCondSvc.MuonCondSvcConf import MuonAlignmentErrorDbSvc
MuonAlignmentErrorDbSvc.OutputLevel = INFO
svcMgr += MuonAlignmentErrorDbSvc()

from MuonCondTool.MuonCondToolConf import MuonAlignmentErrorDbTool
MuonAlignmentErrorDbTool = MuonAlignmentErrorDbTool("MuonAlignmentErrorDbTool")
MuonAlignmentErrorDbTool.OutputLevel = INFO

toolSvc += MuonAlignmentErrorDbTool
dbConn="sqlite://;schema=test.sqlite;dbname=COMP200" # LOCAL VERSION
folder="/MUONALIGN/ERRORS"

svcMgr.IOVDbSvc.OutputLevel = INFO
svcMgr.IOVDbSvc.dbConnection=dbConn
svcMgr.IOVDbSvc.Folders+=[folder+" <tag>MuonAlignErrors-TEST-00</tag> <dbConnection>"+dbConn+"</dbConnection>"]

from MuonAlignErrorTool.MuonAlignErrorToolConf import MuonAlign__AlignmentErrorTool
MuonAlignmentErrorTool = MuonAlign__AlignmentErrorTool("MuonAlignmentErrorTool")
MuonAlignmentErrorTool.read_local_file = 0
MuonAlignmentErrorTool.local_input_filename = "./input.txt"
MuonAlignmentErrorTool.OutputLevel = DEBUG
toolSvc += MuonAlignmentErrorTool


# Update alignment corrections using the conditions DB
from MuonRecExample import MuonAlignConfig

# Access the alignment with Oracle
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
DBReplicaSvc = DBReplicaSvc(UseCOOLSQLite=False)
svcMgr += DBReplicaSvc
svcMgr.PoolSvc.SortReplicas=True

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
MessageSvc.setWarning += ["ToolSvc.MuonCalib::IdToFixedIdTool"]
MessageSvc.defaultLimit = 9999999

# Define the input collection
svcMgr.EventSelector.InputCollections = [
    #"/tmp/cmaiani/DESDM_RPVLL.01528435._000622.pool.root.3",
    # DIFFERENT RUN (TO CHECK DB CALL-BACK)
     "/sps/atlas/c/cmaiani/MUONALIGNMENTSYST/data12_8TeV.00202712.physics_Muons.recon.DESDM_RPVLL.r5724_tid01528435_00/DESDM_RPVLL.01528435._000622.pool.root.3",
   # REL 19
    #"/sps/atlas/c/cmaiani/MUONALIGNMENTSYST/data12_8TeV.00202740.physics_Muons.recon.DESDM_RPVLL.r5724_tid01528303_00/DESDM_RPVLL.01528303._000045.pool.root.1",
    #"/sps/atlas/c/cmaiani/MUONALIGNMENTSYST/data12_8TeV.00202740.physics_Muons.recon.DESDM_RPVLL.r5724_tid01528303_00/DESDM_RPVLL.01528303._000073.pool.root.1",
    #"/sps/atlas/c/cmaiani/MUONALIGNMENTSYST/data12_8TeV.00202740.physics_Muons.recon.DESDM_RPVLL.r5724_tid01528303_00/DESDM_RPVLL.01528303._000103.pool.root.1",
    "/sps/atlas/c/cmaiani/MUONALIGNMENTSYST/data12_8TeV.00202740.physics_Muons.recon.DESDM_RPVLL.r5724_tid01528303_00/DESDM_RPVLL.01528303._000205.pool.root.1",
   # REL 17
   # "/sps/atlas/c/cmaiani/MUONALIGNMENTSYST/data12_8TeV.00201113.physics_Muons.merge.DESD_SGLMU.f435_m1117/data12_8TeV.00201113.physics_Muons.merge.DESD_SGLMU.f435_m1117._lb0492-lb0496._0001.1",
    ]

from MuonAlignErrorTool.MuonAlignErrorToolConf import MuonAlign__MuonAlignErrorExampleAlg
topSequence += MuonAlign__MuonAlignErrorExampleAlg("MuonAlignErrorExampleAlg")
topSequence.MuonAlignErrorExampleAlg.alignErrorTool = MuonAlignmentErrorTool


