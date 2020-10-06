#
# --- Only do for RAW-ESD stage
#
from RecExConfig.RecFlags import rec
if rec.doWriteESD() or rec.doWriteAOD() or ('doWriteESD' in dir() and doWriteESD) or ('doWriteAOD' in dir() and doWriteAOD) or InDetFlags.doTrkNtuple() or InDetFlags.doTrkD3PD():

    # --- fix primary vertex finder bug
    printfunc ('===> OVERWRITTEN TOOL SETTINGS ................')
    InDetPriVxFinderTool.maxVertices = 200
    printfunc (InDetPriVxFinderTool)
    printfunc ('===> OVERWRITTEN TOOL SETTINGS ................')

    # --- fix vertexing setup for selection
    printfunc ('===> OVERWRITTEN TOOL SETTINGS ................')
    from TrkVertexWeightCalculators.TrkVertexWeightCalculatorsConf import Trk__TrueVertexDistanceWeightCalculator
    VertexWeightCalculator = Trk__TrueVertexDistanceWeightCalculator(name  = "InDetTrueVertexDistanceWeightCalculator",
                                                                     McTruthCollection = InDetKeys.McEventCollection() )
    ToolSvc += VertexWeightCalculator
    printfunc (VertexWeightCalculator)
    printfunc ('===> OVERWRITTEN TOOL SETTINGS ................')
    
    printfunc ('===> OVERWRITTEN TOOL SETTINGS ................')
    VertexCollectionSortingTool.VertexWeightCalculator = VertexWeightCalculator
    printfunc (VertexCollectionSortingTool)
    printfunc ('===> OVERWRITTEN TOOL SETTINGS ................')
    
    # --- fix vertexing smoother setup
    printfunc ('===> OVERWRITTEN TOOL SETTINGS ................')
    from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
    InDetVertexSmoother = Trk__SequentialVertexSmoother()
    ToolSvc += InDetVertexSmoother
    printfunc (InDetVertexSmoother)
    printfunc ('===> OVERWRITTEN TOOL SETTINGS ................')
                                                                
    printfunc ('===> OVERWRITTEN TOOL SETTINGS ................')
    InDetVxFitterTool.VertexSmoother = InDetVertexSmoother
    printfunc (InDetVxFitterTool)
    printfunc ('===> OVERWRITTEN TOOL SETTINGS ................')
                                                                            
    # --- turn off cluster calibration from DB, does not exist for IBL
    useNewChargeFormat  = False
    if not useNewChargeFormat:
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeCalibCondAlg
        PixelChargeCalibCondAlg.ReadKey=""
    else:
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeLUTCalibCondAlg
        PixelChargeLUTCalibCondAlg.ReadKey=""

    useNewDeadmapFormat = False
    if not useNewDeadmapFormat:
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
        PixelConfigCondAlg.ReadDeadMapKey = ""
    else:
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDeadMapCondAlg
        PixelDeadMapCondAlg.ReadKey = ""

# --- switch alignment for IBL geometry off (gives WARNING)
from PixelGeoModel.PixelGeoModelConf import PixelDetectorTool
pixelTool           = PixelDetectorTool()
pixelTool.Alignable = False
