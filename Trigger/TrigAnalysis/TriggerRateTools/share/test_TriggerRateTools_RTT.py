
### usually ATN tests runs with following RDO input:

if not ('doFastCaloAccess' in dir()):
    doFastCaloAccess=True

if not ('EvtMax' in dir()):
    EvtMax=3
if not ('OutputLevel' in dir()):
    OutputLevel=WARNING

if not ('doRates' in dir()):
    doRates =True 

# in RTT this will be overriden
if not ('PoolRDOInput' in dir()):
    PoolRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/valid3.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s472_RDO.025026._00004.pool.root.1"]
    


# old default was DetDescrVersion="ATLAS-CSC-01-02-00" and no conditions specified 
if not ('DetDescrVersion' in dir()):
    DetDescrVersion="ATLAS-CSC-01-02-00"
    #DetDescrVersion="ATLAS-GEO-02-00-00"
    #if not('OFLCONDVersion' in dir()):
    #   OFLCONDVersion = "OFLCOND-SIM-00-00-03"
        

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion=DetDescrVersion
if 'OFLCONDVersion' in dir():
  globalflags.ConditionsTag=OFLCONDVersion


###############################
doCBNT=False


doTrigger=True
doESD=False

TriggerModernConfig=True

doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAOD=False 
doESD=False 
doTAG=False

doTruth=True

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
#-----------------------------------------------------------

#TriggerFlags.readHLTconfigFromXML=False
#TriggerFlags.readLVL1configFromXML=False

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

if not doRates:
    TriggerFlags.triggerMenuSetup = 'MC_lumi1E31'
    TriggerFlags.L1PrescaleSet = 'None' 
    TriggerFlags.HLTPrescaleSet = 'None' 
# this will be disabled by the rates tool, but it needs to have them loaded.
else:
    if os.getenv("AtlasProject") == "AtlasP1HLT" :
        TriggerFlags.triggerMenuSetup = 'Physics_lumi1E31'
    else:
        TriggerFlags.triggerMenuSetup = 'MC_lumi1E31'
    TriggerFlags.L1PrescaleSet = 'L1Prescales100_Physics_lumi1E31'
    TriggerFlags.HLTPrescaleSet = 'HLTPrescales100_Physics_lumi1E31'

if doRates:
    def addAlwaysAcceptChain():
        TriggerFlags.MinBiasSlice.enableSignature("always")
    from TriggerMenuPython.GenerateMenu import GenerateMenu
    GenerateMenu.overwriteSignaturesWith(addAlwaysAcceptChain)

#------------ This is a temporary fix ---------------
from RecExConfig.RecConfFlags import recConfFlags
recConfFlags.AllowIgnoreConfigError=False
athenaCommonFlags.AllowIgnoreConfigError=False
#-------------end of temporary fix-------------------

# make sure the names are right
TriggerFlags.outputLVL1configFile="outputLVL1config.xml"
TriggerFlags.outputHLTconfigFile="outputHLTconfig.xml"

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

# abort when there is an unchecked status code
StatusCodeSvc.AbortOnError=False

jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "ntuple.root"

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

#from AthenaCommon.AlgSequence import AlgSequence
print AlgSequence
print ServiceMgr



# disable printing of histograms in finalize
if hasattr(svcMgr,'THistSvc'):
    svcMgr.THistSvc.OutputLevel = WARNING

# THistSvc = Service ( "THistSvc" )
# THistSvc.OutputLevel=WARNING

##### MET slice configuration: ######
EFMissingET_Fex= Configurable.allConfigurables.get("EFMissingET_Fex")
##- cell tool
from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromCells
EFMissingETFromCells = EFMissingETFromCells()
##- FEB tool
from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromFEBHeader
EFMissingETFromFEBHeader = EFMissingETFromFEBHeader()

##- uncomment the following line to us FEB tool instead of cell tool
#EFMissingET_Fex.Tools = [EFMissingETFromFEBHeader]

if doFastCaloAccess:
    # on MET side:
    EFMissingETFromCells.useFullCollection = True
    EFMissingETFromFEBHeader.useFullCollection = True
else:
    EFMissingETFromCells.useFullCollection = False
    EFMissingETFromFEBHeader.useFullCollection = False

if doRates:
    include("TriggerRateTools/TriggerRateTools_common.py")
    ServiceMgr.THistSvc.Output += ["TriggerRateTools DATAFILE='TriggerRates.root' OPT='RECREATE'"]
    triggerRateTools.doRawTD = True
#   include minbias fraction
    triggerRateTools.xSection = 0.070*0.0798
    triggerRateTools.Luminosity = 10000000.0

ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/atlasgroupdisk/trig-daq/user/SethCaughron/user.SethCaughron.misal1_mc12.EnhancedBias_1031.digit.RDO.v13004004_1to5000/user.SethCaughron.misal1_mc12.EnhancedBias_1031.digit.RDO.v13004004.RDO._00001.pool.root"]

triggerRateTools.doRetrieveItems = True

