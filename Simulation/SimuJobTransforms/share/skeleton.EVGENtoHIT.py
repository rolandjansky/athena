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
    simFlags.TruthStrategy = runArgs.truthStrategy

try:
    from BarcodeServices.BarcodeServicesConfig import barcodeOffsetForTruthStrategy
    simFlags.SimBarcodeOffset  = barcodeOffsetForTruthStrategy(str(simFlags.TruthStrategy.get_Value()))
except RuntimeError:
    Offset200kStrings = ['MC12', 'MC15a', 'MC16']
    is200kOffset = any(x in str(simFlags.TruthStrategy.get_Value()) for x in Offset200kStrings)
    if is200kOffset:
        simFlags.SimBarcodeOffset  = 200000 #MC12, MC15a, MC16 setting
    else:
        simFlags.SimBarcodeOffset  = 1000000 #MC15 setting
    atlasG4log.warning('Using unknown truth strategy '+str(simFlags.TruthStrategy.get_Value())+' guessing that barcode offset is '+str(simFlags.SimBarcodeOffset.get_Value()))
