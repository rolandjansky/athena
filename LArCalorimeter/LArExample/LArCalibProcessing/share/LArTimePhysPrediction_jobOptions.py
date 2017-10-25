###########################################################################
# 01/2009 T. Guillemin
# JO for tphys prediction
# uses the algorithm LArTimePhysPrediction in LArCalibUtils
# 11/09 T. Guillemin
# corrections of the special crates fiber lengths
###########################################################################

from StoreGate.StoreGateConf import StoreGateSvc
#----------------
# Dump StoreGate
#----------------
#sgStore = StoreGateSvc("StoreGateSvc")
#sgStore.Dump = True
#sgStore.OutputLevel = DEBUG

#--------------------
# Dump DetectorStore
#--------------------
#dtStore = StoreGateSvc("DetectorStore")
#dtStore.Dump = True
#dtStore.OutputLevel = DEBUG

#----------------------
# Dump ConditionsStore
#----------------------
#cdStore = StoreGateSvc("ConditionsStore")
#cdStore.Dump = True
#cdStore.OutputLevel = DEBUG 

#Pool file input 
# PoolFileList = [ "/tmp/tguillem/LArCaliWave_89206_EB-EMBC_StripsXtalkCorr.pool.root"]
PoolFileList = [ "/afs/cern.ch/user/l/larcalib/w0/data/WorkingDirectory/00135322_00135323_00135326_Barrel-EMB-EMEC_MEDIUM_26_15.5.1_2/poolFiles/LArCaliWave_00135323_EB-EMBA_StripsXtalkCorr.pool.root"]

###########################################################################
#
#                           Global settings
#
###########################################################################

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

# include modified version of LArCalibProcessing/LArCalib_P3C_MinimalSetup.py
# because of problems with detector flags
# include("LArCalibProcessing/LArCalib_P3C_MinimalSetup.py")

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-04-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()  #Switched on for the prediction
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
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

# end of modified version of LArCalibProcessing/LArCalib_P3C_MinimalSetup.py

#to read the Calo descriptor
include ("CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

###############################################################################
#
# Read POOL file(s)
#
###############################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('COMCOND-006-00')

if ( len(PoolFileList)>0 ):

   from AthenaCommon.ConfigurableDb import getConfigurable
   svcMgr += getConfigurable( "ProxyProviderSvc" )()
   svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
   
   svcMgr += getConfigurable( "CondProxyProvider" )()
   svcMgr.CondProxyProvider.InputCollections += PoolFileList

###############################################################################
#
# Cables/fibers lengths required for the prediction of the physics time
#
###############################################################################
# speed of the signal in the cables
# approximation: same speed in all the cables (neglect the cables resistance influence)
sCalib = 5.5
sSignal = 5.5
sPig = 5.5
sLTP = 5.5

########### calibration cables
# all the vacuum cables and the warm cables have ~the same length for all the FTs
# ==> they do not introduce any relative delays (it may have to be taken into account for the FCAL)

# barrel: divide in 16 bins of 0.1
LCalib_EMB = [[4.58,4.58,4.62,4.62,4.7,4.7,4.81,4.81,4.94,4.94,5.19,5.19,5.46,5.46,5.62,5.62],[4.38,4.38,4.38,4.38,4.38,4.38,4.38,4.38,4.63,4.63,4.83,4.83,5.13,5.13,5.13,0],[3.03,3.03,3.03,3.03,3.03,3.03,3.03,3.03,3.28,3.28,3.48,3.48,3.78,3.78,0],[5.13,3.03,3.03,3.03,3.03,3.03,3.03,3.03,3.03,3.28,3.28,3.48,3.48,3.78,3.78,0,0]]
EtaMin_EMB = [0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5]
EtaMax_EMB = [0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6]
LPigCalib_EMB = 0.7

# EMEC
# only one length of calibration cables is used on the detector
LCalib_EMEC = 4.90
LPigCalib_EMEC = 0.9
# HEC
LCalib_HEC = 13.0
# FCAL: signal injected at the FEB level => no calibration cables

########### signal cables (only important for the FCAL)
#EMB
LSignal_EMB = [[3.35,3.35,3.06,3.06,2.75,2.75,2.41,2.41,2.03,2.03,1.58,1.58,1.06,1.06,0.58,0.58],[3.56,3.56,3.31,3.31,2.96,2.96,2.82,2.82,2.45,2.45,2.01,2.01,1.53,1.53,1.37],[3.03,3.03,3.03,3.03,2.31,2.31,2.31,2.31,1.37,1.37,1.37,1.37,0.45,0.45,1.37],[3.68,3.6,3.6,3.6,3.37,2.93,2.43,1.87,2.55,1.88,1.32,0.81,0.93,1.88]]
#EMEC (approximation for first iteration: lengths have to be coded for a dedicated study)
LSignal_EMEC = 4.0
#HEC (takes pigtail into account)
LSignal_HEC = 13.0
#FCAL (takes pigtail into account)
LSignal_FCAL = [8.4,7.55,7.4]

########### TTC fibers
# The values can be found in the note ATL-AL-EN-0077
# Delays in ns are given with respect to the longest fiber
# barrel: 2 * 32 FTs (0->C,1->A)
DeltaTTC_EMB = [[93.02,92.49,94.1,92.02,93.04,91.84,92.81,91.87,92.98,92.36,93.27,91.96,93.24,92.28,93.56,92.73,94.03,91.01,92.9,92.2,92.89,90.45,91.34,92.37,93.27,91.31,92.5,91.09,92.3,92.25,92.88,91.97],[93.49,91.02,92.6,92.99,94.21,92.36,93.55,91.93,93.24,92.21,92.98,92.95,94.04,92.5,93.44,92.1,93,92.74,93.64,97.16,98.24,90.42,91.96,91.05,92.75,91.89,93.1,92.4,93.09,93.38,94.36,91.97]]

# end-caps: 2 * 25 FTs (0->C,1->A)
DeltaTTC_EC = [[14.4,14.5,0,0,13.6,15.1,15.4,12.7,14.3,0,0,1.7,2.5,12.7,14.1,0,0,13.4,14.5,11.7,13.1,0,0,12.7,14.3],[13.7,15.1,0,0,13.8,14.7,15.8,13.9,14.9,0,0,4.7,5.7,13.6,14.5,0,0,13.1,13.9,13.9,14.7,0,0,0,1]]
# special crates: 3 fibers per crate
DeltaTTC_ECC_SPEC = [[14.7,15.6,16.7],[13.6,14.9,17.3],[15.9,17.6,18.3],[13.5,14.5,14.5]]
DeltaTTC_ECA_SPEC = [[13.5,14.6,15.9],[13.8,15,18],[13.9,15,15.9],[12.5,14,15]]

###############################################################################
#
# Algorithms
#
############################################################################### 
# make sure that other subdetectors are not built
#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_geant4()
#GlobalFlags.Luminosity.set_zero()

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )

from LArCalibUtils.LArCalibUtilsConf import LArTimePhysPrediction
LArTimePhysPrediction = LArTimePhysPrediction()
LArTimePhysPrediction.KeyInput = "LArCaliWave"
LArTimePhysPrediction.nchannels_max = 1000000
topSequence += LArTimePhysPrediction
LArTimePhysPrediction.sCalib = sCalib
LArTimePhysPrediction.sSignal = sSignal
LArTimePhysPrediction.sPig = sPig
LArTimePhysPrediction.sLTP = sLTP
LArTimePhysPrediction.vLCalib_EMB = LCalib_EMB
LArTimePhysPrediction.vEtaMin_EMB = EtaMin_EMB
LArTimePhysPrediction.vEtaMax_EMB = EtaMax_EMB
LArTimePhysPrediction.vLCalib_EMEC = LCalib_EMEC
LArTimePhysPrediction.vLCalib_HEC = LCalib_HEC
LArTimePhysPrediction.vLSignal_EMB = LSignal_EMB
LArTimePhysPrediction.vLSignal_EMEC = LSignal_EMEC
LArTimePhysPrediction.vLSignal_HEC = LSignal_HEC
LArTimePhysPrediction.vLSignal_FCAL = LSignal_FCAL
LArTimePhysPrediction.vDeltaTTC_EMB = DeltaTTC_EMB
LArTimePhysPrediction.vDeltaTTC_EC = DeltaTTC_EC
LArTimePhysPrediction.vDeltaTTC_ECC_SPEC = DeltaTTC_ECC_SPEC
LArTimePhysPrediction.vDeltaTTC_ECA_SPEC = DeltaTTC_ECA_SPEC

###########################################################################
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
#MessageSvc.defaultLimit = 1000;
###########################################################################
#save the ntuple
theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='Test.ntuple.root' OPT='NEW'" ]
###########################################################################
theApp.EvtMax = 1 # Leave this unchanged!
###########################################################################
