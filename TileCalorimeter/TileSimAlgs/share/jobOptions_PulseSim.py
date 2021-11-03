# jobOptions_NoiseMC.py
# Configuration file to digitize a HITS file for noise calibration purposes
# and to produce root files with resulting noise 

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 4
svcMgr.MessageSvc.OutputLevel = OutputLevel
#svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.defaultLimit = 10
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

# number of events to process
if not 'EvtMax' in dir():
    EvtMax = 1000
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=EvtMax

from AthenaCommon import CfgMgr

# location of input pool files and output root files
#InputDirectory="/afs/cern.ch/user/t/tilecomm/w0/noise/run1"
InputDirectory="."
OutputDirectory="."

if not 'RunNumber' in dir():
    RunNumber = 0
if not 'Version' in dir():
    Version = 0
Suffix=("%d" % Version )

# name of input and output pool files
InFile=("%s/PedestalSimulation.HITS.GEO-18.root" % InputDirectory)
OutFile=("%s/PedestalSimulation.DIGITS.%s.dummy.root" % (OutputDirectory, Suffix))

# Set input file
jobproperties.AthenaCommonFlags.PoolHitsInput=[]
#for i in range (0,100): # one file contains few events only, use it 100 times
for i in range (0,100): # one file contains few events only, use it 100 times
    jobproperties.AthenaCommonFlags.PoolHitsInput+=[InFile]

# Set output file
jobproperties.AthenaCommonFlags.PoolRDOOutput=OutFile

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
jobproperties.Digitization.doCaloNoise=True
jobproperties.Digitization.IOVDbGlobalTag = 'OFLCOND-MC16-SDR-28'

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

PhaseII=False
TileFrameLength=7
TriggerSample= (TileFrameLength-1)//2# Floor division
ForceOFCOnFly = False

from TileRecUtils.TileRecFlags import jobproperties
jobproperties.TileRecFlags.calibrateEnergy = False #Set false to use ADC counts, true to use MeV.
jobproperties.TileRecFlags.doTileOptATLAS = True #Non-iterative method
jobproperties.TileRecFlags.doTileOpt2 = True #Iterative method

if(TileFrameLength != 7 or ForceOFCOnFly is True):
    jobproperties.TileRecFlags.OfcFromCOOL = False

include( "Digitization/Digitization.py" )

from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()
tileInfoConfigurator.NSamples = TileFrameLength
tileInfoConfigurator.TrigSample = TriggerSample
tileInfoConfigurator.setupAdcRange(12) if(PhaseII) else tileInfoConfigurator.setupAdcRange(10) # 12 if Phase II

#Stream1.ItemList+=["TileDigitsContainer#TileDigitsCnt"]
#Stream1.ItemList=[]; # empty list - don't need to create output file
streamRDO.ItemList=[]; # empty list - don't need to create output file

svcMgr.AthenaPoolCnvSvc.MaxFileSizes=["16000000000"]

# get a handle on topalg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# don't need any hits at input
del topSequence.TileHitVecToCnt 

# don't need any L1 or L2 algorithms
del topSequence.TileHitToTTL1
del topSequence.TileRawChannelToL2

# don't need TileMuonReceiver
del topSequence.TilePulseForTileMuonReceiver
del topSequence.TileMuonReceiverDecision

# parameters for pulse simulation

#General configuration
topSequence.TileDigitsFromPulse.OutputLevel=DEBUG
topSequence.TileDigitsFromPulse.NSamples=TileFrameLength #Select number of samples to be saved in each pulse
topSequence.TileDigitsFromPulse.Bigain=False #Save the two gain modes
topSequence.TileDigitsFromPulse.InTimeAmp=1000
topSequence.TileDigitsFromPulse.ImperfectionMean=1
topSequence.TileDigitsFromPulse.ImperfectionRms=0
topSequence.TileDigitsFromPulse.UseInTimeAmpDist=False
topSequence.TileDigitsFromPulse.GaussianC2CPhaseVariation=0.

#PhaseII parameters
if(PhaseII):
    topSequence.TileDigitsFromPulse.TilePhaseII=True

#Pedestal
if(PhaseII):
    topSequence.TileDigitsFromPulse.ChannelSpecificPedestal=False
    topSequence.TileDigitsFromPulse.PedestalValueHG=100
    topSequence.TileDigitsFromPulse.PedestalValueLG=100
else:
    topSequence.TileDigitsFromPulse.ChannelSpecificPedestal=True

#Noise
if(PhaseII):
    topSequence.TileDigitsFromPulse.UseGaussNoise=True
else:
    topSequence.TileDigitsFromPulse.ChannelSpecificNoise=True

#Pile up
topSequence.TileDigitsFromPulse.UseOutOfTimeAmpDist=False
topSequence.TileDigitsFromPulse.PileUpFraction=0
topSequence.TileDigitsFromPulse.nPulses=21
topSequence.TileDigitsFromPulse.AmpDistLowerLimit=0
topSequence.TileDigitsFromPulse.SimulatePileUpWithPoiss=False
topSequence.TileDigitsFromPulse.AvgMuForPileUpSimulation=80

print(topSequence.TileDigitsFromPulse)

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
TileNtuple.TileRawChannelContainer  = "TileRawChannelCnt"
TileNtuple.TileRawChannelContainerOpt = "TileRawChannelOpt2"
#TileNtuple.TileRawChannelContainerFit  = "TrueAmp" 
TileNtuple.NSamples = TileFrameLength

if not 'OutputLevel' in dir():
    OutputLevel = 4
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
svcMgr += AthenaEventLoopMgr()
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100


