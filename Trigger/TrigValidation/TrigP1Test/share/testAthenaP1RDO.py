# This is just a wrapper job option made for RTT -- 
### usually ATN tests runs with following RDO input:


from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not ('EvtMax' in dir()) and not acf.EvtMax.is_locked():
    acf.EvtMax=5
elif not acf.EvtMax.is_locked():
    acf.EvtMax=EvtMax
    del EvtMax


if not ('doRates' in dir()):
    doRates =True 

#xrdcp root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/
#use this to test on enhanced bias
#PoolRDOInput=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/validation/test_data/EnhBias7TeV.e468s624s633d238/user09.EdwardSarkisyan-Grinbaum.EnhBias7TeV1555OfflS31.digit.RDO.e468s624s633d238.RDO._00001.pool.root"]
#OFLCONDVersion = "OFLCOND-DR-BS7T-ANom-00"
#DetDescrVersion="ATLAS-GEO-08-00-01"

######RTT job
doRTT=False
# in RTT this will be overriden
if not ('PoolRDOInput' in dir()):
    acf.PoolRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/valid3.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s472_RDO.025026._00004.pool.root.1"]

import os,commands
def getEosDirectoryList(path,fileRange):
    cmd = 'eos -b ls %s/ ' % (path)
    files = []
    for i in commands.getoutput(cmd).split('\n'):
        fileNumber = i[i.rfind(".pool.root")-5:i.rfind(".pool.root")]
        print 'fileNumber',fileNumber
        if fileNumber.isdigit():
          if  int(fileNumber) > fileRange[0]-1 and int(fileNumber) <fileRange[1]+1 :
            collection = 'root://eosatlas/'+path+i
            files += ["root://eosatlas/%s/%s" % (path,i) ]
    print files 
    return files 

if ('fileRange' in dir()) and ('dsName' in dir()):
   doRTT=True

# Always defined in RTT    
if not ('DetDescrVersion' in dir()):
    DetDescrVersion="ATLAS-GEO-02-00-00"
    if not('OFLCONDVersion' in dir()):
       OFLCONDVersion = "OFLCOND-SIM-00-00-03"



from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion=DetDescrVersion
if 'OFLCONDVersion' in dir():
  globalflags.ConditionsTag=OFLCONDVersion


###############################
rec.doCBNT=False


doTrigger=True
rec.doESD=False

TriggerModernConfig=True

rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False 
rec.doESD=False 
doTAG=False

rec.doTruth=True

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
#-----------------------------------------------------------


TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

if not doRates:
    TriggerFlags.useL1CaloCalibration = False
    TriggerFlags.triggerMenuSetup = 'MC_pp_v4'
    TriggerFlags.L1PrescaleSet = 'None' 
    TriggerFlags.HLTPrescaleSet = 'None' 

else:
    if os.getenv("AtlasProject") == "AtlasP1HLT" :
        TriggerFlags.triggerMenuSetup = 'MC_pp_v4'
        TriggerFlags.L1PrescaleSet  =   ''
        TriggerFlags.HLTPrescaleSet =   ''

    elif os.getenv("AtlasProject") == "AtlasCAFHLT" :
        TriggerFlags.useL1CaloCalibration = False
        TriggerFlags.triggerMenuSetup = 'MC_pp_v4'
        TriggerFlags.L1PrescaleSet  =   ''
        TriggerFlags.HLTPrescaleSet =   ''

    else:
        TriggerFlags.useL1CaloCalibration = False
        TriggerFlags.triggerMenuSetup = 'MC_pp_v3'
        TriggerFlags.L1PrescaleSet  =   ''
        TriggerFlags.HLTPrescaleSet =   ''


# this will be disabled by the rates tool, but it needs to have them loaded.
    TriggerFlags.enableMonitoring = ['Validation', 'Time', 'Log', 'CostAthena' ]


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

include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

#fix for missing tag info in enhanced bias
if 'OFLCONDVersion' in dir():
  if OFLCONDVersion == "OFLCOND-DR-BS7T-ANom-00":
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.TGCcablingServerSvc.forcedUse=True

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


if doRates:
    include("TriggerRateTools/TriggerRateTools_common.py")
    ServiceMgr.THistSvc.Output += ["TriggerRateTools DATAFILE='rates_Lumi1E31_RDO.root' OPT='RECREATE'"]
    triggerRateTools.doRawTD = True
#   include minbias fraction
#    this cross section and acceptance of enhanced bias at 7 TeV, MC-09
    triggerRateTools.xSection = 0.062*0.036
    triggerRateTools.Luminosity = 10000000.0

# cost tools
    from TrigCostMonitor.TrigCostMonitorConfig import setupCostAlg,setupCostJob
    setupCostAlg()
    setupCostJob()

if doRTT:
    ServiceMgr.EventSelector.InputCollections = getEosDirectoryList(dsName,fileRange)
