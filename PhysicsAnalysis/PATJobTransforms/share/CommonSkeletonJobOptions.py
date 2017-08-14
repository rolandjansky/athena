######################################################################
#                                                                    #
# Place holder for numerous common job options of skeleton.XXX files #
# Creation: David Cote (September 2009)                              #
#                                                                    #
######################################################################
include.block("PATJobTransforms/CommonSkeletonJobOptions.py")
include("RecExCommon/RecoUsefulFlags.py")

#Disable RecExCommon by default (enable features on demand based on requested outputs)
rec.doAOD=False
rec.doESD=False
rec.readRDO=False

rec.doHist=False
rec.doMonitoring=False
rec.doCBNT=False

rec.doWriteBS=False
rec.doWriteRDO=False
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False	
rec.doWriteTAGCOM=False

rec.AutoConfiguration=['everything']

#production options
rec.Production=True
rec.abortOnErrorMessage=False

## max/skip events
if hasattr(runArgs,"skipEvents"): athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )
if hasattr(runArgs,"maxEvents"): athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )
else: athenaCommonFlags.EvtMax=-1

#RecExCommon configuration
if hasattr(runArgs,"geometryVersion"):
    inputGeometryVersion = runArgs.geometryVersion
    if type(inputGeometryVersion) == str and inputGeometryVersion.endswith("_VALIDATION"):
        inputGeometryVersion = inputGeometryVersion.replace("_VALIDATION", "")
    globalflags.DetDescrVersion.set_Value_and_Lock( inputGeometryVersion )
if hasattr(runArgs,"conditionsTag"): globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag )
if hasattr(runArgs,"beamType"): jobproperties.Beam.beamType.set_Value_and_Lock( runArgs.beamType )
if hasattr(runArgs,"AMITag"): rec.AMITag=runArgs.AMITag
if hasattr(runArgs,"userExec"): rec.UserExecs=runArgs.userExec
if hasattr(runArgs,"RunNumber"): rec.RunNumber=runArgs.RunNumber
if hasattr(runArgs,"projectName"): rec.projectName=runArgs.projectName
if hasattr(runArgs,"trigStream"): rec.triggerStream=runArgs.trigStream
if hasattr(runArgs,"triggerConfig"):
    from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
    tf.triggerConfig=runArgs.triggerConfig

## autoConfiguration keywords triggering pre-defined functions
if hasattr(runArgs,"autoConfiguration"):
    #reset auto-configuration to specified list of values
    rec.AutoConfiguration=[]
    for key in runArgs.autoConfiguration:
        rec.AutoConfiguration.append(key)

