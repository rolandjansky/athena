# jobOptions_NoiseMC.py
# Configuration file to digitize a HITS file for noise calibration purposes
# and to produce root files with resulting noise 

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 4
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

# number of events to process
if not 'EvtMax' in dir():
    EvtMax = 1000
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=EvtMax

# location of input pool files and output root files
InputDirectory="/afs/cern.ch/user/t/tilecomm/w0/noise/mc15"
OutputDirectory="."

#InputDirectory="."
#OutputDirectory="."

if not 'RunNumber' in dir():
    RunNumber = 0
if not 'Version' in dir():
    Version = 0
Suffix=("%d" % Version )

# name of input and output pool files
InFile=("%s/HITS.pool.root" % InputDirectory)
OutFile=("%s/PedestalSimulation.DIGITS.%s.dummy.root" % (OutputDirectory, Suffix))

# Set input file
jobproperties.AthenaCommonFlags.PoolHitsInput=[]
for i in range (0,100): # one file contains few events only, use it 100 times
    jobproperties.AthenaCommonFlags.PoolHitsInput+=[InFile]

# Set output file
jobproperties.AthenaCommonFlags.PoolRDOOutput=OutFile

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
jobproperties.Digitization.doCaloNoise=True
jobproperties.Digitization.IOVDbGlobalTag = 'OFLCOND-MC12-SDR-06'

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-18-01-00'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.LVL1_setOff()

DetFlags.Truth_setOff()

DetFlags.Tile_setOn()

doTileHitToDigit = False
doTileDigitsFromPulse = True
doTileDigitToRawChannel = True

from TileRecUtils.TileRecFlags import jobproperties
jobproperties.TileRecFlags.calibrateEnergy = False # Set false to use ADC counts, true to use MeV.
jobproperties.TileRecFlags.doTileOptATLAS = True # Non-iterative method DO NOT DISABLE IT !!!
jobproperties.TileRecFlags.doTileOpt2 = True # Iterative method
jobproperties.TileRecFlags.doTileOF1 = True # OF1 method
jobproperties.TileRecFlags.doTileMF = False # COF method

jobproperties.TileRecFlags.correctAmplitude = False
jobproperties.TileRecFlags.correctTime = False
jobproperties.TileRecFlags.correctTimeNI = False
jobproperties.TileRecFlags.OfcFromCOOL = False

# get a handle on topalg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence+=xAODMaker__EventInfoCnvAlg()

include.block( "TileSimAlgs/TileMuonReceiver_jobOptions.py" ) # don't need to simulate muon receiver
include( "Digitization/Digitization.py" )

#Stream1.ItemList+=["TileDigitsContainer#TileDigitsCnt"]
#Stream1.ItemList=[]; # empty list - don't need to create output file
streamRDO.ItemList=[]; # empty list - don't need to create output file

svcMgr.AthenaPoolCnvSvc.MaxFileSizes=["16000000000"]

# don't need any hits at input
ToolSvc.TileHitVecToCntTool.TileHitVectors = []
del topSequence.TileHitVecToCnt

# don't need any L1 or L2 algorithms
del topSequence.TileHitToTTL1
del topSequence.TileRawChannelToL2

# parameters for pulse simulator algorithm
topSequence.TileDigitsFromPulse.OutputContainer='TileDigitsCnt'
#topSequence.TileDigitsFromPulse.OutputLevel=VERBOSE
topSequence.TileDigitsFromPulse.InTimeAmp=100
topSequence.TileDigitsFromPulse.OutOfTimeAmp=50
topSequence.TileDigitsFromPulse.ImperfectionMean=1
topSequence.TileDigitsFromPulse.ImperfectionRms=0
topSequence.TileDigitsFromPulse.UseGaussNoise=False
topSequence.TileDigitsFromPulse.UseInTimeAmpDist=False
topSequence.TileDigitsFromPulse.UseOutOfTimeAmpDist=False
topSequence.TileDigitsFromPulse.PileUpFraction=0
topSequence.TileDigitsFromPulse.GaussianC2CPhaseVariation=0.
topSequence.TileDigitsFromPulse.ChannelSpecificPedestal=True
topSequence.TileDigitsFromPulse.ChannelSpecificNoise=True

#--- Create TileCal h2000 ntuple with RawChannels ---------
if not hasattr(ServiceMgr,"THistSvc"):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr+=THistSvc()
ServiceMgr.THistSvc.Output += ["AANT DATAFILE='%s/tile_h2000.root' OPT='RECREATE'" % OutputDirectory]

doSim = True
ReadRch = True
jobproperties.TileRecFlags.readDigits=True
include( "TileRec/TileNtuple_jobOptions.py" )

TileNtuple.CalibMode = True
TileNtuple.CalibrateEnergy = False
TileNtuple.OfflineUnits = 0
TileNtuple.TileRawChannelContainer    = "TileRawChannelCnt"  if jobproperties.TileRecFlags.doTileOptATLAS() else ""
TileNtuple.TileRawChannelContainerOpt = "TileRawChannelOpt2" if jobproperties.TileRecFlags.doTileOpt2() else ""
TileNtuple.TileRawChannelContainerOF1 = "TileRawChannelOF1"  if jobproperties.TileRecFlags.doTileOF1() else ""
TileNtuple.TileRawChannelContainerMF  = "TileRawChannelMF"   if jobproperties.TileRecFlags.doTileMF() else ""
TileNtuple.TileRawChannelContainerFit = "TrueAmp" 

if not 'OutputLevel' in dir():
    OutputLevel = 4
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
svcMgr += AthenaEventLoopMgr()
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100
