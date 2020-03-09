# Trigger_topOptions_writeBS.py
# simple jO based on rdotobs.py for doing RDO->BS with just Level-1 trigger

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
from AthenaCommon.GlobalFlags import globalflags
    
if not ('EvtMax' in dir()):
    acf.EvtMax=-1
if (not "doFiltering" in dir()):
    doFiltering=True
if (not "OutputLevel" in dir()):
    rec.OutputLevel=INFO

if not "PhysicsLArFormat" in dir():
    PhysicsLArFormat=False

if (not "setLVL1XML" in dir()):
    setLVL1XML=None 
if (not "myBSRDOOutput" in dir()):
    acf.BSRDOOutput='AppName='+setMenu+', OutputDirectory=./, Run=105200'
else:
    acf.BSRDOOutput=myBSRDOOutput
    
if 'PoolRDOInput' in dir():
    acf.PoolRDOInput = PoolRDOInput
    del PoolRDOInput

if 'setDetDescr' in dir():
    globalflags.DetDescrVersion = setDetDescr
else:
    rec.AutoConfiguration += ['Geo']
    
if "setGlobalTag" in dir():
    globalflags.ConditionsTag = setGlobalTag
else:
    rec.AutoConfiguration += ['ConditionsTag']
   
doTrigger=True
rec.doESD=False
rec.doCBNT=False
rec.doWriteRDO=False
rec.doTruth=False
rec.doESD=False
rec.doWriteESD=False 
rec.doWriteAOD=False 
rec.doAOD=False 
rec.doWriteTAG=False 
rec.doPerfMon=False
rec.doWriteBS=True # enable BS writing

#Set trigger menu
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerMenuSetup = setMenu
if setLVL1XML:
    TriggerFlags.readLVL1configFromXML=True
    TriggerFlags.inputLVL1configFile=setLVL1XML
    TriggerFlags.outputLVL1configFile=None
else:
    TriggerFlags.readLVL1configFromXML=False
TriggerFlags.doLVL2=False
TriggerFlags.doEF=False
TriggerFlags.doHLT=False

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

if PhysicsLArFormat:
    ToolSvc.LArRawDataContByteStreamTool.DSPRunMode=4
    ToolSvc.LArRawDataContByteStreamTool.RodBlockVersion=10
    ToolSvc.LArRawDataContByteStreamTool.IncludeDigits=True

if doFiltering:
    StreamBSFileOutput.AcceptAlgs = ["RoIBuilder"]
