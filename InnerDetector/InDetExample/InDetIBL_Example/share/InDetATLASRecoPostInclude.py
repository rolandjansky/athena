#
# --- Only do for RAW-ESD stage
#
from RecExConfig.RecFlags import rec
if rec.doWriteESD() or ('doWriteESD' in dir() and doWriteESD):

    # --- fix primary vertex finder bug
    print '===> OVERWRITTEN TOOL SETTINGS ................'
    InDetPriVxFinderTool.maxVertices = 200
    print InDetPriVxFinderTool
    print '===> OVERWRITTEN TOOL SETTINGS ................'

    # --- fix vertexing setup for selection
    print '===> OVERWRITTEN TOOL SETTINGS ................'
    from TrkVertexWeightCalculators.TrkVertexWeightCalculatorsConf import Trk__TrueVertexDistanceWeightCalculator
    VertexWeightCalculator = Trk__TrueVertexDistanceWeightCalculator(name  = "InDetTrueVertexDistanceWeightCalculator",
                                                                     McTruthCollection = InDetKeys.McEventCollection() )
    ToolSvc += VertexWeightCalculator
    print VertexWeightCalculator
    print '===> OVERWRITTEN TOOL SETTINGS ................'
    
    print '===> OVERWRITTEN TOOL SETTINGS ................'
    VertexCollectionSortingTool.VertexWeightCalculator = VertexWeightCalculator
    print VertexCollectionSortingTool
    print '===> OVERWRITTEN TOOL SETTINGS ................'
    
    # --- fix vertexing smoother setup
    print '===> OVERWRITTEN TOOL SETTINGS ................'
    from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
    InDetVertexSmoother = Trk__SequentialVertexSmoother()
    ToolSvc += InDetVertexSmoother
    print InDetVertexSmoother
    print '===> OVERWRITTEN TOOL SETTINGS ................'
                                                                
    print '===> OVERWRITTEN TOOL SETTINGS ................'
    InDetVxFitterTool.VertexSmoother = InDetVertexSmoother
    print InDetVxFitterTool
    print '===> OVERWRITTEN TOOL SETTINGS ................'
                                                                            

