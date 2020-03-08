



def makeRegSelTool(detector='Pixel') :
    
    from AthenaCommon.AlgSequence import AthSequencer
    condseq = AthSequencer('AthCondSeq')

    if not hasattr(condseq, 'RegSelCondAlg_'+detector):
        from InDetRegionSelector.InDetRegionSelectorConf import SiRegSelCondAlg
        CondAlg = SiRegSelCondAlg( name = ("RegSelCondAlg_"+detector),
                                        ManagerName = detector,
                                        PrintTable  = False)    
        CondAlg.RegSelLUT   = "RegSelLUTCondData_"+detector
        condseq += CondAlg

    from AthenaCommon.AppMgr import ToolSvc
    from RegionSelector.RegionSelectorConf import RegSelTool

    tool = RegSelTool(name="RegSelTool_"+detector)
    tool.RegSelLUT = "RegSelLUTCondData_"+detector

    ToolSvc += tool

    return tool



def makeResgSelTool_Pixel() :
    return makeRegSelTool('Pixel')

def makeResgSelTool_SCT() :
    return makeRegSelTool('SCT')
