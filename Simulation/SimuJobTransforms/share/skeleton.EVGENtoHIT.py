include("SimuJobTransforms/CommonSkeletonJobOptions.py")

if hasattr(runArgs, "jobNumber"):
    if runArgs.jobNumber < 1:
        raise ValueError('jobNumber must be a postive integer. %s lies outside this range', str(runArgs.jobNumber))


if hasattr(runArgs, "inputTXT_EVENTIDFile"):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    overlayFlags.EventIDTextFile = runArgs.inputTXT_EVENTIDFile[0]
    from G4AtlasApps.SimFlags import simFlags
    simFlags.load_atlas_flags()
    if hasattr(simFlags, 'IsEventOverlayInputSim'):
        simFlags.IsEventOverlayInputSim = True
    if hasattr(runArgs, 'inputVertexPosFile'):
        simFlags.VertexOverrideFile= runArgs.inputVertexPosFile

