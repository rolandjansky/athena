#################################################################
#  Filename: JetSamplingCalibrator_JetSampling_jobOptions.py
#
#  Description: Calibrates an input JetSampling collection and writes a ROOT
#               ntuple
# 
#  Input file: pool file with JetSampling Collection
#        Set it in:
#        InputPoolFile = "JetSampling.pool.root"
#
#  Output file: ROOT file with JetSampling variables
#        default name:
#        RootNtupleOutput = "ntuple.root"
#
#  Created: Belen.Salvachua@cern.ch Jun 2009
#
###############################################################

# Input File
import glob
if not 'InputPoolFile' in dir():
    InputPoolFile = glob.glob("JetSampling.pool.root")

# Output File    
if not 'RootNtupleOutput' in dir():
    RootNtupleOutput = "ntuple.root"
    
# Athena common flags
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

# Top Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## Set Maximum Number of events (-1 = all)
theApp.EvtMax=-1

ServiceMgr.MessageSvc.defaultLimit = 10000000
ServiceMgr.MessageSvc.OutputLevel  =  WARNING
AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = WARNING


######## Input POOL File ####################
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = InputPoolFile
print ServiceMgr.EventSelector.InputCollections

#############################################
#
# Configure Jet Sampling Calibrator tool
#
####### JetSamplingCalibrator ###############
from JetCalibTools.JetCalibToolsConf import JetSamplingCalibrator
jetcalibrationtool2 = JetSamplingCalibrator()
jetcalibrationtool2.OutputLevel = WARNING
include( "JetSamplingConstants.py" ) # File with JetSampling calibration constants
#############################################

#############################################
# To calibrate JetSamplign objects
# configure Layer Calibrator algorithm and
# add it to general topSequence
####### LayerCalibratorAlg ##################
from JetCalibTools.JetCalibToolsConf import LayerCalibratorAlg
MyAlg = LayerCalibratorAlg()
MyAlg.OutputLevel = WARNING
MyAlg.InputJetSamplingContainer = "JetSamplingCone7Tower"
MyAlg.OutputJetSamplingContainer = "JetSamplingCone7LayerTower"
MyAlg.AlgTool = jetcalibrationtool2 # here we pass the calibrator tool 
topSequence += MyAlg
#############################################

####### Output ROOT ntuple ###################
CBNTAthenaAware=True
include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

# create CBNTAA_AthenaAware module/algorithm
from JetCalib.JetCalibConf import CBNTAA_JetSampling
MyCBNTAA_JetSampling = CBNTAA_JetSampling( "MyCBNTAA_JetSampling")
MyCBNTAA_JetSampling.InputJetSamplingContainer = "JetSamplingCone7LayerTower"
MyCBNTAA_JetSampling.OutputLevel = WARNING
MyCBNTAA_JetSampling.SetFillCalo = False

# include the CBNT algorithm in CBNT_AthenaAware chain
CBNT_AthenaAware += MyCBNTAA_JetSampling

# Enable ATLAS histogram service
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='"+RootNtupleOutput+"' OPT='RECREATE'"]

from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
AANTupleStream = AANTupleStream()
AANTupleStream.OutputName = RootNtupleOutput
AANTupleStream.ExistDataHeader = False
AANTupleStream.ExtraRefNames = [ "StreamESD","StreamRDO" ]
AANTupleStream.OutputLevel = WARNING
topSequence += AANTupleStream
###################################################                         
