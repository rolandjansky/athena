# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#Translate the commonly used runArgs into configFlags 
def commonRunArgsToFlags(runArgs,configFlags):
    ## Max/skip events
    if hasattr(runArgs,"skipEvents"):
        configFlags.Exec.SkipEvents=runArgs.skipEvents

    if hasattr(runArgs,"maxEvents"):
        configFlags.Exec.MaxEvents=runArgs.maxEvents

    if hasattr(runArgs,"conditionsTag"):
        configFlags.IOVDb.GlobalTag=runArgs.conditionsTag

    if hasattr(runArgs,"geometryVersion"): 
        configFlags.GeoModel.AtlasVersion=runArgs.geometryVersion

    if hasattr(runArgs,"beamType"):
        from AthenaConfiguration.Enums import BeamType
        configFlags.Beam.Type=BeamType(runArgs.beamType)

    ## Threading arguments
    if hasattr(runArgs,"threads"):
        configFlags.Concurrency.NumThreads = runArgs.threads

    if hasattr(runArgs,"concurrentEvents"):
        configFlags.Concurrency.NumConcurrentEvents = runArgs.concurrentEvents

    ## Executor steps
    if hasattr(runArgs,"totalExecutorSteps"):
        configFlags.ExecutorSplitting.TotalSteps = runArgs.totalExecutorSteps

    if hasattr(runArgs,"executorStep"):
        configFlags.ExecutorSplitting.Step = runArgs.executorStep

    if hasattr(runArgs,"executorEventCounts"):
        configFlags.ExecutorSplitting.TotalEvents = configFlags.Exec.MaxEvents
        configFlags.Exec.MaxEvents = runArgs.executorEventCounts[configFlags.ExecutorSplitting.Step]

    if hasattr(runArgs,"executorEventSkips"):
        configFlags.Exec.SkipEvents = runArgs.executorEventSkips[configFlags.ExecutorSplitting.Step]
