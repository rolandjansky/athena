###############################################################
#
# Skeleton top job options for D3PD validation
#
#==============================================================


# ==============================================================================
# Load your input file that you want to process
# ==============================================================================
include("PATJobTransforms/CommonSkeletonJobOptions.py")

from AthenaCommon.Logging import logging
skelLog = logging.getLogger('ValidateD3PD')
skelLog.info( '****************** Starting D3PD Validation *****************' )

if hasattr(runArgs,"inputESDFile"):
    rec.readESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock( runArgs.inputESDFile )
elif hasattr(runArgs,"inputAODFile"):
    rec.readAOD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
else:
    raise RuntimeError('No input file argument given (ESD or AOD input required)')

#Default 'do' list:
doPhysInDetPerf     = True #!!
doPhysBackTrack     = True
doPhysMet           = True  
doPhysJets          = False ## Tag update needed
doPhysTau           = True
doPhysElectrons     = False ## Not tagged
doPhysMuons         = False ## Need to be implemented 
doPhysBtag          = True
doPhysSUSY          = True
doPhysMonTop        = True
doPhysPhotons       = False
doZee               = False ## Tag update needed
doExotics           = True
doHSG6              = True

Routines = ['PhysInDetPerf','PhysBackTrack','PhysMet','PhysJets','PhysTau','PhysElectrons','PhysMuons','PhysBtag','PhysSUSY','PhysMonTop','Zee','Exotics','HSG6','PhysPhotons']

#Switch on/off various validation routines:
if hasattr(runArgs,"d3pdVal"):
    for val in Routines:
        dostr = val
        dontstr = 'no'+val
        if dostr in runArgs.d3pdVal:
            vars()['do'+dostr] = True
        if dontstr in runArgs.d3pdVal:
            vars()['do'+dostr] = False


# ==============================================================================
# Configure RecExCommon (the mother of all job options in Athena) 
# and schedule your DPD making.
# Unfortunately, for now, you still have to turn OFF some things by hand
# ==============================================================================

from InDetRecExample.InDetKeys import InDetKeys
InDetKeys.UnslimmedTracks.set_Value_and_Lock('Tracks')
InDetKeys.UnslimmedTracksTruth.set_Value_and_Lock('TrackTruthCollection')

rec.doHist.set_Value_and_Lock(True)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.ParticleJet = False

# ----------------------------------------------------------------------------------------------------
# If you have your own DPD Maker scripts
# (for examples, see in svn: PhysicsAnalysis/D2PDMaker/share/D2PD_ExampleSimple*.py ),
# then just append your script (wherever it is) to this list:
#       rec.DPDMakerScripts.append("MyPackage/MyScript")
# An example scripts is appended below, so you can see how it works!
# ----------------------------------------------------------------------------------------------------


# Set up trigger for All tools
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
    
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']

# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter("ReadPool")
    
# ----------------------------------------------------------------------------------------------------
## primary tracking jobOptions
### Removed General definitions and filter
# ----------------------------------------------------------------------------------------------------
if doPhysInDetPerf:
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doStandardPlots.set_Value_and_Lock(True)
    rec.DPDMakerScripts.append("RunPhysVal/PhysValInDetPerf_jobOptions.py")

# ----------------------------------------------------------------------------------------------------
## secondary tracking jobOptions
# ----------------------------------------------------------------------------------------------------
if doPhysBackTrack:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValBackTrack_jobOptions.py")


# ----------------------------------------------------------------------------------------------------
## Tau jobOptions
# ----------------------------------------------------------------------------------------------------
if doPhysTau:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValTau_jobOptions.py")


# ----------------------------------------------------------------------------------------------------
## b-tagging jobOptions
# ----------------------------------------------------------------------------------------------------
if doPhysBtag:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValBtag_jobOptions.py")


# ----------------------------------------------------------------------------------------------------
## MET jobOptions (ESD)
# ----------------------------------------------------------------------------------------------------
if doPhysMet:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValMET_jobOptions.py")


# ----------------------------------------------------------------------------------------------------
## Jets jobOptions 
# ----------------------------------------------------------------------------------------------------
if doPhysJets:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValJets_jobOptions.py")


# ----------------------------------------------------------------------------------------------------
## electrons jobOptions
# ----------------------------------------------------------------------------------------------------
if doPhysElectrons:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValElectrons_jobOptions.py")


# ----------------------------------------------------------------------------------------------------
## muons jobOptions
# ----------------------------------------------------------------------------------------------------
if doPhysMuons:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValMuons_jobOptions.py")

# ----------------------------------------------------------------------------------------------------
## SUSY jobOptions
# ----------------------------------------------------------------------------------------------------
if doPhysSUSY:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValSUSY_jobOptions.py")

# ----------------------------------------------------------------------------------------------------
## PhysValMon + Top jobOptions
# ----------------------------------------------------------------------------------------------------
if doPhysMonTop:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValMonTop_jobOptions.py")

# ----------------------------------------------------------------------------------------------------
## Zee jobOptions
# ----------------------------------------------------------------------------------------------------
if doZee:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValZee_jobOptions.py")

# ----------------------------------------------------------------------------------------------------
## Exotics jobOptions
# ----------------------------------------------------------------------------------------------------
if doExotics:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValExotics_jobOptions.py")

# ----------------------------------------------------------------------------------------------------
## HSG6 jobOptions
# ----------------------------------------------------------------------------------------------------
if doHSG6:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValHSG6_jobOptions.py")

# ----------------------------------------------------------------------------------------------------
## photons jobOptions
# ----------------------------------------------------------------------------------------------------
if doPhysPhotons:
    rec.DPDMakerScripts.append("RunPhysVal/PhysValPhotons_jobOptions.py")


## Pre-exec
if hasattr(runArgs,"preExec"):
    skelLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        skelLog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)
    
# ==============================================================================
# Now, include the master top options from RecExCommon.
# This automatically ensures that your Athena job will be set up correctly,
# i.e., if RecExCommon doesn't work, the release is broken!
# ==============================================================================
include ("RecExCommon/RecExCommon_topOptions.py")


## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    skelLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        skelLog.info(cmd)
        exec(cmd)


