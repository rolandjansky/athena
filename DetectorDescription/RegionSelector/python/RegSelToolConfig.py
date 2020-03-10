#
#   @file    RegSelToolConfig.py
#
#            configuration functions for the new RegSelTools 
#
#   @author  sutt 
#
#   @date    Sun  8 Mar 2020 03:27:57 GMT
#                 
#   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration#                 
#



def _makeSiRegSelTool(detector, enable) :
                
    from RegionSelector.RegionSelectorConf import RegSelTool
    tool = RegSelTool(name="RegSelTool_"+detector)
    
    # should we enable the look up table access ?

    if ( enable ) :

        # add the lookup table to retrieve

        tool.RegSelLUT = "RegSelLUTCondData_"+detector


        # add the conditions algorithm to create the lookup table

        from AthenaCommon.AlgSequence import AthSequencer
        condseq = AthSequencer('AthCondSeq')
        
        if not hasattr(condseq, 'RegSelCondAlg_'+detector):
            from InDetRegionSelector.InDetRegionSelectorConf import SiRegSelCondAlg
            CondAlg = SiRegSelCondAlg( name = ("RegSelCondAlg_"+detector),
                                       ManagerName = detector,
                                       PrintTable  = False)    
            CondAlg.RegSelLUT   = "RegSelLUTCondData_"+detector
            condseq += CondAlg
    else:
        # detector not configured so don't enable 
        # lookup table access

        tool.Initialised = False
    
    return tool



def makeRegSelTool_Pixel() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.pixel_on()
    return _makeSiRegSelTool( 'Pixel', enabled )


def makeRegSelTool_SCT() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.SCT_on()
    return _makeSiRegSelTool( 'SCT', enabled )


def makeRegSelTool_TRT() :

    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.TRT_on()
                
    from RegionSelector.RegionSelectorConf import RegSelTool
    tool = RegSelTool(name="RegSelTool_TRT")
    
    # should we enable the look up table access ?

    if ( enabled ) :

        # add the lookup table to retrieve

        tool.RegSelLUT = "RegSelLUTCondData_TRT"

        # add the conditions algorithm to create the lookup table

        from AthenaCommon.AlgSequence import AthSequencer
        condseq = AthSequencer('AthCondSeq')
        
        if not hasattr(condseq, 'RegSelCondAlg_TRT'):
            from InDetRegionSelector.InDetRegionSelectorConf import TRT_RegSelCondAlg
            CondAlg = TRT_RegSelCondAlg( name = "RegSelCondAlg_TRT",
                                         ManagerName = "TRT",
                                         PrintTable  = False)    
            CondAlg.RegSelLUT   = "RegSelLUTCondData_TRT"
            condseq += CondAlg
    else:
        # detector subsystem not configured so don't enable 
        # lookup table access

        tool.Initialised = False
    
    return tool
