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
    if hasattr(runArgs,"totalExecutorSteps") and runArgs.totalExecutorSteps > 1:
        athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.executorEventSkips[runArgs.executorStep] )
    else:
        athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )
if hasattr(runArgs,"maxEvents"):
    if hasattr(runArgs,"totalExecutorSteps") and runArgs.totalExecutorSteps > 1:
        athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.executorEventCounts[runArgs.executorStep] )
    else:
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

# Avoid command line preInclude for event service
if hasattr(runArgs, "eventService") and runArgs.eventService:
    import AthenaMP.EventService

## autoConfiguration keywords triggering pre-defined functions
## if hasattr(runArgs,"autoConfiguration"):
##     for key in runArgs.autoConfiguration:
##         rec.AutoConfiguration.append(key)

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
from AthenaCommon.ConcurrencyFlags import jobproperties as cf
if cf.ConcurrencyFlags.NumThreads() > 1:
    pmon_properties.PerfMonFlags.doMonitoringMT=True
else:
    pmon_properties.PerfMonFlags.doMonitoring=True
    pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True
