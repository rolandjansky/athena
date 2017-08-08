include("SimuJobTransforms/CommonSkeletonJobOptions.py")

if hasattr(runArgs, "jobNumber"):
    if runArgs.jobNumber < 1:
        raise ValueError('jobNumber must be a postive integer. %s lies outside this range', str(runArgs.jobNumber))


from G4AtlasApps.SimFlags import simFlags
if hasattr(runArgs, "inputTXT_EVENTIDFile"):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    overlayFlags.EventIDTextFile = runArgs.inputTXT_EVENTIDFile[0]
    simFlags.load_atlas_flags()
    if hasattr(simFlags, 'IsEventOverlayInputSim'):
        simFlags.IsEventOverlayInputSim = True
    if hasattr(runArgs, 'inputVertexPosFile'):
        simFlags.VertexOverrideFile= runArgs.inputVertexPosFile

if hasattr(runArgs, 'truthStrategy'):
    simFlags.TruthService     = 'ISF_'     + runArgs.truthStrategy + 'TruthService'
    simFlags.TruthStrategy    = runArgs.truthStrategy
    try:
        from BarcodeServices.BarcodeServicesConfig import barcodeOffsetForTruthStrategy
        simFlags.SimBarcodeOffset  = barcodeOffsetForTruthStrategy(runArgs.truthStrategy)
    except RuntimeError:
        Offset200kStrings = ['MC12', 'MC15a', 'MC16']
        is200kOffset = any(x in runArgs.truthStrategy for x in Offset200kStrings)
        if is200kOffset:
            simFlags.SimBarcodeOffset  = 200000 #MC12, MC15a, MC16 setting
        else:
            simFlags.SimBarcodeOffset  = 1000000 #MC15 setting
        atlasG4log.warning('Using unknown truth strategy '+str(runArgs.truthStrategy)+' guessing that barcode offset is '+str(simFlags.SimBarcodeOffset))
else:
    simFlags.TruthService     = 'ISF_TruthService'
    simFlags.TruthStrategy    = 'MC12'
    simFlags.SimBarcodeOffset  = 200000 #MC12 setting

