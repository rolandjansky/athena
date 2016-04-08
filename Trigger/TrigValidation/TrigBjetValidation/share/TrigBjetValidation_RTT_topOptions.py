# ********************************************
#
# jobOption for RTT test for HLT b-jet slice
#
# ********************************************

if ( not locals().has_key('DetDescrVersion') ) :
    DetDescrVersion="ATLAS-GEO-06-00-00"

doCBNT=True
CBNTAthenaAware=True
useROOTNtuple=True
doTruth=True

RootNtupleOutput="ntuple.BjetSlice.root"

OutputLevel=INFO
EvtMax=2000


AllAlgs=False

doiPatRec = False
donewTracking=False

doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAODLVL1=False
doAOD=False

doTrigger=True
TriggerModernConfig=True
from TriggerJobOpts.TriggerFlags import TriggerFlags

no_Bphysics=True
include("RecExCond/RecExCommon_flags.py")

TriggerFlags.doMuon=False #mnash: originally False; change doesn't make it fall over but makes horrendously slow
TriggerFlags.doHypo=False
TriggerFlags.doTruth=True

TriggerFlags.BjetSlice.doSiTrack=True
TriggerFlags.BjetSlice.doIDSCAN=True
#TriggerFlags.triggerMenuSetup = 'Physics_lumi1E31_no_prescale'

def resetSigs():
    TriggerFlags.doMuon=False
    TriggerFlags.doHypo=False
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.BjetSlice.setAll();
    TriggerFlags.BjetSlice.signatures = ['b10']

from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(resetSigs)


#-------------------------------------------------------------
# End of setting flags
#-------------------------------------------------------------

include( "RecExCommon/RecExCommon_topOptions.py" )

TriggerFlags.abortOnConfigurationError=True

Service ("StoreGateSvc" ).ActivateHistory=False 
Service( "RegSelSvc" ).enableCalo = False

MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"
Service ("StoreGateSvc" ).ActivateHistory=False
