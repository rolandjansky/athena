# #############################################################################
#
# jopOptions file for simple calorimeter reconstruction 
# ( without "RecExCommon/RecExCommon_topOptions.py" and "RecExCommon/RecExCommon_flags.py" )
#
# Only CaloCellGetter + CaloTopoCluster + CBNTAA_CaloCluster ++CBNTAA_CalibrationInfoDM2
# 
#
# See also following examples
#  1. TileCalorimeter/TileExample/TileRecEx/share/jobOptions_TileTB_SimRec.py
#  2. Reconstruction/MuonIdentification/MuidExample/share/jobOptions.py
# #############################################################################
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()
GlobalFlags.InputFormat.set_pool()

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.makeRIO.all_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Calo_setOn()
DetFlags.simulate.Truth_setOn()

# setup POOL access in ATHENA
import AthenaPoolCnvSvc.ReadAthenaPool

DetFlags.Print()
GlobalFlags.Print()

theApp.EvtMax=500
DetDescrVersion='ATLAS-CSC-05-00-00'
#LArIOVDbTag = "CSC02-E-QGSP_EMV"

# geomodel
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# particles info from RecExCommon_topOptions.py
include( "PartPropSvc/PartPropSvc.py" )

# calorimetry
from RecExConfig.ObjKeyStore import objKeyStore
# 14.2.20
#objKeyStore.readInputFile('RecExCond/OKS_streamRDO.py')
# 14.2.21
objKeyStore.readInputFile('RecExPers/OKS_streamRDO.py')

include( "TileConditions/TileConditions_jobOptions.py" )
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )

# data from pool
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )
# extra dictionary for calibration hits
include( "CaloSimEvent/CaloSimEventDict_joboptions.py" )



################################################################################
## CALORIMETER RECONSTRUCTION from include ("CaloRec/CaloRec_jobOptions.py")
################################################################################
include( "CaloRec/CaloCellMaker_config.py" )
from CaloRec.CaloCellGetter import CaloCellGetter
CaloCellGetter()

include( "CaloRec/CaloTopoCluster_jobOptions.py" )
CaloTopoCluster.LocalCalib.EMFrac.UseNormalizedEnergyDensity = True
CaloTopoCluster.LocalCalib.EMFrac.MaxEMFraction = 0.5
CaloTopoCluster.KeepCorrectionToolAndContainerNames += [OOCCalib.getFullName(),"CaloWeightTopoCluster"]
CaloTopoCluster.KeepCorrectionToolAndContainerNames += [DMCalib.getFullName(),"CaloOOCTopoCluster"]

# input data
#ServiceMgr.EventSelector.InputCollections = [ "simul.single.pool.root" ]
ServiceMgr.EventSelector.InputCollections = [ "DigitizationOutput.pool.root" ]

#svcMgr.EventSelector.SkipEvents = 200;

# CBNT staff
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()
#ServiceMgr.THistSvc.Output += ["AANT DATAFILE='$RecoFile' OPT='RECREATE'" ]
ServiceMgr.THistSvc.Output += ["AANT DATAFILE='cbntaa.root' OPT='RECREATE'" ]

from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream(
    ExistDataHeader = False,
    WriteInputDataHeader = False,
    OutputName = 'ntuple.root',
    OutputLevel = WARNING
    )



################################################################################
## propriet condition data
################################################################################
#theApp.Dlls+=["CaloCondAthenaPoolPoolCnv"]
## add necessary services if not done yet
#if not hasattr(ServiceMgr, 'ProxyProviderSvc'):
    #from StoreGate.StoreGateConf import ProxyProviderSvc
    #ServiceMgr += ProxyProviderSvc()
#if not 'CondProxyProvider' in ServiceMgr.ProxyProviderSvc.ProviderNames:
    #ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
#if not hasattr(ServiceMgr, "CondProxyProvider"):
    #from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
    #ServiceMgr += CondProxyProvider()

## set this to the file containing your calibration objects
##ServiceMgr.CondProxyProvider.InputCollections += ["/home/pospelov/testarea/test_14.2.0/writecond/HadDMCoeff2_v21_1a.pool.root"]
#ServiceMgr.CondProxyProvider.InputCollections += ["/afs/ipp-garching.mpg.de/home/g/gdp/public/HadDMCoeff2/HadDMCoeff2_v21_1a.pool.root"]

#if not hasattr(ServiceMgr, "IOVSvc"):
    #from IOVSvc.IOVSvcConf import IOVSvc
    #ServiceMgr += IOVSvc()

#myIOVDbFolders = []
#for x in ServiceMgr.IOVDbSvc.Folders:
            ##if not ( x.startswith("/CALO/HadCalibration/CaloDMCorr2") ):
            #if ( x.find("/CALO/HadCalibration/CaloDMCorr2") == -1 ):
                #print "adding ",x," to IOVDbSvc.Folders"
                #myIOVDbFolders += [x]

#ServiceMgr.IOVDbSvc.Folders = myIOVDbFolders
##ServiceMgr.IOVDbSvc.Folders+=["/CALO/HadCalibration/CaloDMCorr2"+"<dbConnection>impl=cool;techno=sqlite;schema=myDBHadDMCoeff200.db;X:OFLP200</dbConnection>"+"<tag>CaloHadDMCoeff-002-01</tag>"]
#ServiceMgr.IOVDbSvc.Folders+=["/CALO/HadCalibration/CaloDMCorr2"+"<dbConnection>sqlite://X;schema=/afs/ipp-garching.mpg.de/home/g/gdp/public/HadDMCoeff2/myDBHadDMCoeff200.db;dbname=OFLP200</dbConnection>"+"<tag>CaloHadDMCorr-002-01</tag>"]
#ServiceMgr.IOVSvc.updateInterval = "JOB"
##ServiceMgr.IOVSvc.PreLoadData=True

#print "XXX", ServiceMgr.IOVDbSvc.Folders


########################################################
# user CBNT's
########################################################
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )
# truth information  from include( "RecExCommon/CBNT_Truth_jobOptions.py" )
from CBNT_Truth.CBNTAA_TruthCfg import CBNTAA_Truth
theCBNTAA_Truth=CBNTAA_Truth()
from CBNT_Truth.CBNT_TruthConf import CBNT_TruthSelector
theCBNTAA_Truth += CBNT_TruthSelector("All", PtMin = -1. * GeV, EtaMax = -1, OptionAll = True, Enable = True) 
CBNT_AthenaAware+=theCBNTAA_Truth

include( "CaloRec/CaloTopoCluster_CBNT_jobOptions.py" )
theCBNT_CaloClusterTopo.UseLink=True
theCBNT_CaloClusterTopo.AddCellDetails = False
theCBNT_CaloClusterTopo.MaxCaloCluster = 600
theCBNT_CaloClusterTopo.MaxCell = 50000

CBNT_AthenaAware += CBNTAA_CaloCluster("CBNT_CaloClusterTopoEM")
theCBNT_CaloClusterTopoEM=CBNTAA_CaloCluster("CBNT_CaloClusterTopoEM")
CBNT_AthenaAware += CBNTAA_CaloCluster("CBNT_CaloClusterTopoW" )
theCBNT_CaloClusterTopoW=CBNTAA_CaloCluster("CBNT_CaloClusterTopoW")
CBNT_AthenaAware += CBNTAA_CaloCluster("CBNT_CaloClusterTopoOOC")
theCBNT_CaloClusterTopoOOC=CBNTAA_CaloCluster("CBNT_CaloClusterTopoOOC")

theCBNT_CaloClusterTopoEM.ClusterColl = "CaloTopoClusters"
theCBNT_CaloClusterTopoEM.Suffix = "_topo_em"
theCBNT_CaloClusterTopoEM.EMOnly = FALSE
theCBNT_CaloClusterTopoEM.MaxCaloCluster = 600
theCBNT_CaloClusterTopoEM.UseLink=True
#theCBNT_CaloClusterTopoEM.AddCellDetails = True
#theCBNT_CaloClusterTopoEM.MaxCell = 50000

theCBNT_CaloClusterTopoW.ClusterColl = "CaloWeightTopoCluster"
theCBNT_CaloClusterTopoW.Suffix = "_topo_w"
theCBNT_CaloClusterTopoW.EMOnly = FALSE
theCBNT_CaloClusterTopoW.MaxCaloCluster = 600
theCBNT_CaloClusterTopoW.UseLink=True

theCBNT_CaloClusterTopoOOC.ClusterColl = "CaloOOCTopoCluster"
theCBNT_CaloClusterTopoOOC.Suffix = "_topo_ooc"
theCBNT_CaloClusterTopoOOC.EMOnly = FALSE
theCBNT_CaloClusterTopoOOC.MaxCaloCluster = 600
theCBNT_CaloClusterTopoOOC.UseLink=True

include ("CaloCalibHitRec/CalibrationInfo_CBNT_jobOptions.py")
CBNT_CalibrationInfo.AddCellDetails = False

include ("CaloCalibHitRec/CalibrationInfoDM2_CBNT_jobOptions.py")
CBNT_CalibrationInfoDM2.AddHitsDetailsDM  = True
CBNT_CalibrationInfoDM2.AddHitsDetailsAI  = True
CBNT_CalibrationInfoDM2.AddHitsDetailsCLS = False
CBNT_CalibrationInfoDM2.AddHitsDetailsReg = False
CBNT_CalibrationInfoDM2.AssignDM2Cluster = False

include( "CaloCalibHitRec/CalibClusterMoments_CBNT_jobOptions.py" )
