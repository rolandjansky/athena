include.block("LArMonTools/LArAllMonitoring_jobOptions.py")

#################################################################
#
# Steers LAr monitoring based on beam type and Commissioning switches.  
# Rob McPherson, first version 30 Sept 2008
#
#  Note: This is the LAr only version, 21 Oct 2008
#
#################################################################

from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
from RecExConfig.RecFlags import rec
from AthenaMonitoring.DQMonFlags import DQMonFlags
from LArROD.LArRODFlags import larRODFlags
from LArMonTools.LArMonFlags import LArMonFlags
    

if DQMonFlags.monManEnvironment() == 'tier0Raw':
    larRawMon=True
    larESDMon=False
elif DQMonFlags.monManEnvironment() == 'tier0ESD':
    larRawMon=False
    larESDMon=True
else: # it should be 'tier0'
    larRawMon=True
    larESDMon=True

from RecExConfig.InputFilePeeker import inputFileSummary
if inputFileSummary['evt_type'][0]=='IS_SIMULATION':
    LArMonFlags.doLArDigitMon = False    
    

# Define ONE LArBadChannelMask for all Monitoring tools
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArBadChannelsMasker=LArBadChannelMasker("LArBadChannelsMasker")
theLArBadChannelsMasker.DoMasking=True
theLArBadChannelsMasker.ProblemsToMask=[
    "deadReadout","deadPhys","short","almostDead",
    "highNoiseHG","highNoiseMG","highNoiseLG","sporadicBurstNoise"
    ]
ToolSvc+=theLArBadChannelsMasker

# Parameter to avoid the "procHist" offline. More efficient
if athenaCommonFlags.isOnline():
    LArMonFlags.EventBlockSize = 20
else:
    LArMonFlags.EventBlockSize = 0

if 'LArDigitKey'  in dir() :
    LArMonFlags.LArDigitKey=LArDigitKey 


#add some trigger configuration to avoid errors from NoiseCorrelationTool (mspalla)
 ####################
from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
tf.readBS.set_Value_and_Lock(True)
tf.doLVL1.set_Value_and_Lock(False)
tf.doLVL2.set_Value_and_Lock(False)
tf.doEF.set_Value_and_Lock(False)
tf.doL1Topo.set_Value_and_Lock(False)
tf.readLVL1Calo.set_Value_and_Lock(True)
tf.enableMonitoring.set_Value_and_Lock(False)
tf.doID.set_Value_and_Lock(False)
tf.doMuon.set_Value_and_Lock(False)
tf.doBcm.set_Value_and_Lock(False)
tf.doTrt.set_Value_and_Lock(False)
tf.configForStartup="HLTonline"
tf.configForStartup.lock()
include("TrigTier0/BStoESD_Tier0_HLTConfig_jobOptions.py")


# This is the main block choosing what monitoring to run.
# There is some repetition, but I think maintenance is easier (Rob).

if jobproperties.Beam.beamType() == 'cosmics':
    if larESDMon and not athenaCommonFlags.isOnline():
        if LArMonFlags.doLArAffectedRegions():
            include ("LArAffectedRegion/LArAffectedRegion_read.py")
            include ("LArMonTools/LArAffectedRegions_jobOptions.py")
    if larRawMon:
            if jobproperties.Global.DataSource.get_Value() == 'data':
                if LArMonFlags.doLArFEBMon():
                    include ("LArMonTools/LArFEBMon_jobOptions.py" )
                if LArMonFlags.doLArRODMonTool():
                    include ("LArMonTools/LArRODMonTool_jobOptions.py")
                if not athenaCommonFlags.isOnline():
                    if LArMonFlags.doLArHVCorrectionMonTool():
                        include ("LArMonTools/LArHVCorrectionMonTool_jobOptions.py" )
            if LArMonFlags.doLArCosmicsMonTool():
                include ("LArMonTools/LArCosmicsMonTool_jobOptions.py")
            if LArMonFlags.doLArCoverage():
                include ("LArMonTools/LArCoverage_jobOptions.py")
            if LArMonFlags.doLArDigitMon():
                include ("LArMonTools/LArDigitMon_jobOptions.py")
            if LArMonFlags.doLArNoiseCorrelationMon():
                include ("LArMonTools/LArNoiseCorrelationMon_jobOptions.py")
            if LArMonFlags.doLArNoisyROMon():
                include ("LArMonTools/LArNoisyROMon_jobOptions.py")
            if LArMonFlags.doLArRawChannelMon():
                include ("LArMonTools/LArRawChannelMonTool_jobOptions.py")
            if LArMonFlags.doLArCollisionTimeMon():
                include("LArMonTools/LArCollisionTimeMonTool_jobOptions.py")


elif jobproperties.Beam.beamType() == 'singlebeam':
    if larESDMon and not athenaCommonFlags.isOnline():
        if LArMonFlags.doLArAffectedRegions():
            include ("LArAffectedRegion/LArAffectedRegion_read.py")
            include ("LArMonTools/LArAffectedRegions_jobOptions.py")
    if larRawMon:
            if jobproperties.Global.DataSource.get_Value() == 'data':
                if LArMonFlags.doLArFEBMon():
                    include ("LArMonTools/LArFEBMon_jobOptions.py" )
                if LArMonFlags.doLArRODMonTool():
                    include ("LArMonTools/LArRODMonTool_jobOptions.py")
                if not athenaCommonFlags.isOnline():
                    if LArMonFlags.doLArHVCorrectionMonTool():
                        include ("LArMonTools/LArHVCorrectionMonTool_jobOptions.py" )
            if LArMonFlags.doLArCoverage():
                include ("LArMonTools/LArCoverage_jobOptions.py")
            if LArMonFlags.doLArDigitMon():
                include ("LArMonTools/LArDigitMon_jobOptions.py")
            if LArMonFlags.doLArNoiseCorrelationMon():
                include ("LArMonTools/LArNoiseCorrelationMon_jobOptions.py")
            if LArMonFlags.doLArNoisyROMon():
                include ("LArMonTools/LArNoisyROMon_jobOptions.py")
            if LArMonFlags.doLArRawChannelMon():
                include("LArMonTools/LArRawChannelMonTool_jobOptions.py")
            if LArMonFlags.doLArCollisionTimeMon():
                include("LArMonTools/LArCollisionTimeMonTool_jobOptions.py")
else:  # should be jobproperties.Beam.beamType() =='collisions'
    if larESDMon and not athenaCommonFlags.isOnline():
        if LArMonFlags.doLArAffectedRegions():
            include ("LArAffectedRegion/LArAffectedRegion_read.py")
            include ("LArMonTools/LArAffectedRegions_jobOptions.py")
    if larRawMon:
            if jobproperties.Global.DataSource.get_Value() == 'data':
                if LArMonFlags.doLArFEBMon():
                    include ("LArMonTools/LArFEBMon_jobOptions.py" )
                if LArMonFlags.doLArRODMonTool():
                    include ("LArMonTools/LArRODMonTool_jobOptions.py")
                if not athenaCommonFlags.isOnline():
                    if LArMonFlags.doLArHVCorrectionMonTool():
                        include ("LArMonTools/LArHVCorrectionMonTool_jobOptions.py" )
            if LArMonFlags.doLArCoverage():
                include ("LArMonTools/LArCoverage_jobOptions.py")
            if LArMonFlags.doLArDigitMon():
                include ("LArMonTools/LArDigitMon_jobOptions.py")
            if LArMonFlags.doLArNoiseCorrelationMon():
                include ("LArMonTools/LArNoiseCorrelationMon_jobOptions.py")
            if LArMonFlags.doLArNoisyROMon():
                include ("LArMonTools/LArNoisyROMon_jobOptions.py")
            if LArMonFlags.doLArRawChannelMon():
                include("LArMonTools/LArRawChannelMonTool_jobOptions.py")
            if LArMonFlags.doLArCollisionTimeMon():
                include("LArMonTools/LArCollisionTimeMonTool_jobOptions.py")


