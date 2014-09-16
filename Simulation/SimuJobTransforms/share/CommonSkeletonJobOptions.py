######################################################################
#                                                                    #
# Place holder for numerous common job options of skeleton.XXX files #
# Creation: David Cote (September 2009)                              #
#                                                                    #
######################################################################
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#from AthenaCommon.BFieldFlags import jobproperties ##Not sure if this is appropriate for G4 sim

## Max/skip events
if hasattr(runArgs,"skipEvents"):
    athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )
if hasattr(runArgs,"maxEvents"):
    athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )
else:
    athenaCommonFlags.EvtMax=-1

if hasattr(runArgs,"conditionsTag"):
    if runArgs.conditionsTag != 'NONE':
        globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag ) #make this one compulsory?
if hasattr(runArgs,"beamType"):
    if runArgs.beamType != 'NONE':
        # Setting beamType='cosmics' keeps cavern in world volume for g4sim also with non-commissioning geometries
        jobproperties.Beam.beamType.set_Value_and_Lock( runArgs.beamType )
## if hasattr(runArgs,"AMITag"): rec.AMITag=runArgs.AMITag
## if hasattr(runArgs,"userExec"): rec.UserExecs=runArgs.userExec
## if hasattr(runArgs,"RunNumber"): rec.RunNumber=runArgs.RunNumber
## if hasattr(runArgs,"projectName"): rec.projectName=runArgs.projectName
## if hasattr(runArgs,"trigStream"): rec.triggerStream=runArgs.trigStream
## if hasattr(runArgs,"triggerConfig"):
##     from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
##     tf.triggerConfig=runArgs.triggerConfig

## autoConfiguration keywords triggering pre-defined functions
## if hasattr(runArgs,"autoConfiguration"):
##     for key in runArgs.autoConfiguration:
##         rec.AutoConfiguration.append(key)

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doMonitoring=True
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True
