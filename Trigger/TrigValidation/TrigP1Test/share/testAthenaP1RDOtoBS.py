# Trigger_topOptions_writeBS.py
# simple jO based on rdotobs.py for doing RDO->BS with just Level-1 trigger

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not ('EvtMax' in dir()):
    acf.EvtMax=25
if (not "doFiltering" in dir()):
    doFiltering=True
if (not "OutputLevel" in dir()):
    rec.OutputLevel=INFO

if (not "setMenu" in dir()):
    setMenu='lumi1E31'
if (not "setLv1XML" in dir()):
    setLv1XML=""
if (not "setPrescale" in dir()):
    setPrescale=""
if (not "PoolRDOInput" in dir()):
    acf.PoolRDOInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/valid3.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s472_RDO.025026._00004.pool.root.1"]
if (not "BSRDOOutput" in dir()):
    acf.BSRDOOutput='AppName=Athena, OutputDirectory=./, FileTag='+setMenu+'.Release14.2.24, Run=5200'


   
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


#set Geometry/conditions to use - could pick it up from RDO file automatically
DetDescrVersion="ATLAS-GEO-02-00-00"
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion=DetDescrVersion
globalflags.ConditionsTag="OFLCOND-SIM-00-00-03"

#Set trigger menu
from TriggerJobOpts.TriggerFlags  import TriggerFlags
TriggerFlags.triggerMenuSetup = setMenu
TriggerFlags.L1PrescaleSet = setPrescale
TriggerFlags.HLTPrescaleSet = "None"
if setMenu=='SingleBeam_v1' or setMenu=='Collision_v1' or setMenu=='Cosmic_v1':
    #For these menus, the L1 menu are not generated to be the same as online
    #so we just read in the existing file
    TriggerFlags.readLVL1configFromXML=True
    TriggerFlags.inputLVL1configFile="TriggerMenuXML/LVL1config_SingleBeam_v1_7-bit_trigger_types.xml"
    TriggerFlags.outputLVL1configFile=None
elif setLv1XML == 'TrigP1Test':
    TriggerFlags.readLVL1configFromXML=True
    TriggerFlags.inputLVL1configFile="../AthenaP1RDO/outputLVL1config.xml"
    TriggerFlags.outputLVL1configFile=None
else:
    TriggerFlags.readLVL1configFromXML=False
TriggerFlags.doLVL2=False
TriggerFlags.doEF=False

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

if doFiltering:
    StreamBSFileOutput.AcceptAlgs = ["RoIBuilder"]
