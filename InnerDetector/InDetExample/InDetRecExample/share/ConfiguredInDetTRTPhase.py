# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredInDetTRTPhase.py')

# --------------------------------------------------------------------------------
#
# --- TRT phase calculation
#
# --------------------------------------------------------------------------------

class ConfiguredInDetTRTPhase:

    def __init__(self, InputTrackCollections = [], TRT_Segments_EC = None):
        
        from InDetRecExample.InDetJobProperties import InDetFlags
        from AthenaCommon.DetFlags import DetFlags
        from InDetRecExample.InDetKeys import InDetKeys
        #
        # get ToolSvc and topSequence
        #
        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        
        if InDetFlags.doPRDFormation() and DetFlags.makeRIO.TRT_on():

            #    
            # --- calculation of the event phase from all 3 input collections
            #

            useNewEP = True
            if globalflags.DataSource == 'data':
                if InDetFlags.doCosmics():
                    globalOffset = 8
                else:
                    globalOffset = 0
            else:
                globalOffset = -3.125
            cutWindowCenter  = -8.5
            numberIterations = 5
            cutWindowSize    = 7
                                                    
            #    
            # --- load tool
            #
            from InDetCosmicsEventPhase.InDetCosmicsEventPhaseConf import InDet__InDetCosmicsEventPhaseTool
            InDetCosmicsEventPhaseTool = InDet__InDetCosmicsEventPhaseTool(name              = "InDetCosmicsEventPhaseTool",
                                                                           UseTRTCalibration = True,
                                                                           UseNewEP          = useNewEP,
                                                                           GlobalOffset      = globalOffset
                                                                           )
                                                                           
            ToolSvc += InDetCosmicsEventPhaseTool
            if(InDetFlags.doPrintConfigurables()):
                print InDetCosmicsEventPhaseTool
            
            from InDetCosmicsEventPhase.InDetCosmicsEventPhaseConf import InDet__InDetFixedWindowTrackTimeTool
            InDetFixedWindowTrackTimeTool = InDet__InDetFixedWindowTrackTimeTool(name              = "InDetFixedWindowTrackTimeTool",
                                                                                 UseTRTCalibration = True,
                                                                                 UseNewEP          = useNewEP,
                                                                                 GlobalOffset      = globalOffset,
                                                                                 WindowCenter      = cutWindowCenter,
                                                                                 WindowSize        = cutWindowSize
                                                                                 )
                                                                           
            ToolSvc += InDetFixedWindowTrackTimeTool
            if(InDetFlags.doPrintConfigurables()):
                print InDetFixedWindowTrackTimeTool
            
            
            from InDetCosmicsEventPhase.InDetCosmicsEventPhaseConf import InDet__InDetSlidingWindowTrackTimeTool
            InDetSlidingWindowTrackTimeTool = InDet__InDetSlidingWindowTrackTimeTool(name              = "InDetSlidingWindowTrackTimeTool",
                                                                                     UseTRTCalibration = True,
                                                                                     UseNewEP          = useNewEP,
                                                                                     GlobalOffset      = globalOffset,
                                                                                     NumberIterations  = numberIterations,
                                                                                     WindowSize        = cutWindowSize
                                                                                     )
                                                                           
            ToolSvc += InDetSlidingWindowTrackTimeTool
            if(InDetFlags.doPrintConfigurables()):
                print InDetSlidingWindowTrackTimeTool

            #
            # --- load algorithm
            #
            from InDetCosmicsEventPhase.InDetCosmicsEventPhaseConf import InDet__InDetCosmicsEventPhase
            InDetCosmicsEventPhase = InDet__InDetCosmicsEventPhase(name              = "InDetCosmicsEventPhase",
                                                                   InputTracksNames  = InputTrackCollections,
                                                                   TrackSummaryTool  = InDetTrackSummaryTool,
                                                                   #EventPhaseTool    = InDetCosmicsEventPhaseTool)
                                                                   #EventPhaseTool    = InDetFixedWindowTrackTimeTool)
                                                                   EventPhaseTool    = InDetSlidingWindowTrackTimeTool)
            if InDetFlags.doCosmics():
                InDetCosmicsEventPhase.EventPhaseTool=InDetCosmicsEventPhaseTool
                topSequence += InDetCosmicsEventPhase  # indented here: do not calculate the phase in collisions 
            if (InDetFlags.doPrintConfigurables()):
                print InDetCosmicsEventPhase
    
