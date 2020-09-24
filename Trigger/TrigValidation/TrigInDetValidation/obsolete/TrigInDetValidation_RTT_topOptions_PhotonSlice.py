###############################################################
#
# Set Flags for Running the Trigger in TransientBS Mode
# includes Trigger_topOptions.py to do the real work.
# Reads input from ZEBRA.P (readG3=true)
#
# use:
# athena.py -bs testIDtransBS.py
# 
#==============================================================

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#athenaCommonFlags.FilesInput=["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/proj-sit/dferreir/RDOs/valid3.006384.PythiaH120gamgam.digit.RDO.e322_s461/RDO.024054._00001.pool.root.1",]

if not ('EventMax' in dir()):
     EventMax=10
athenaCommonFlags.EvtMax=EventMax

from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
InDetTrigSliceSettings[('pTmin','electron')]= 500.
InDetTrigSliceSettings[('pTmin','photon')]= 500.
InDetTrigSliceSettings[('etaHalfWidth','electron')]=0.2
InDetTrigSliceSettings[('etaHalfWidth','photon')]=0.2
InDetTrigSliceSettings[('phiHalfWidth','electron')]=0.2
InDetTrigSliceSettings[('phiHalfWidth','photon')]=0.2

# Set flags for the ntuple Content
doCBNT=True

#switch off all ESD making algorithms
from RecExConfig.RecFlags import rec
rec.doTruth=True

OutputLevel=INFO

#switch off all ESD making algorithms
from RecExConfig.RecFlags import rec
rec.doESD=True
rec.doAOD=False
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False

#reenable ID algorithms
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.Enabled.set_Value_and_Lock(True)

doAODLVL1=False

doTrigger=True

from TriggerJobOpts.TriggerFlags import TriggerFlags
no_Bphysics=True
include("RecExCond/RecExCommon_flags.py")

TriggerFlags.triggerMenuSetup = 'default'
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False


TriggerFlags.doMuon=False
TriggerFlags.doHypo=False
TriggerFlags.doTruth=True

def resetSigs():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.EgammaSlice.setAll()
    TriggerFlags.EgammaSlice.signatures = ['g20_loose_cnv']

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

print 'igb'
print InDetTrigSliceSettings[('pTmin','electron')]
print InDetTrigSliceSettings[('pTmin','photon')]
print InDetTrigSliceSettings[('etaHalfWidth','electron')]
print InDetTrigSliceSettings[('etaHalfWidth','photon')]
print InDetTrigSliceSettings[('phiHalfWidth','electron')]
print InDetTrigSliceSettings[('phiHalfWidth','photon')]

#theCBNTegamma.usecbnt_convert = TRUE
