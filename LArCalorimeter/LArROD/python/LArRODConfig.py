# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getLArRawChannelBuilder(name="LArRawChannelBuilder" , **kwargs):
    # Currently just for the MC case, but could easily be extended to
    # cover the data case.

    from AthenaCommon.AppMgr import ToolSvc

    kwargs.setdefault('LArRawChannelContainerName', "LArRawChannels")
    kwargs.setdefault('UseOFCTool', True)

    # Turn off printing for LArRoI_Map
    from LArRawUtils.LArRawUtilsConf import LArRoI_Map
    LArRoI_Map = LArRoI_Map("LArRoI_Map")
    ToolSvc += LArRoI_Map
    LArRoI_Map.Print=False
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.digitize.LAr_on() :
        kwargs.setdefault('DataLocation', 'LArDigitContainer_MC')

    kwargs.setdefault('ADC2MeVTool', 'LArADC2MeVToolDefault')

    kwargs.setdefault('OFCTool', 'LArOFCToolDefault')

    from LArROD.LArRODFlags import larRODFlags
    kwargs.setdefault('firstSample',larRODFlags.firstSample())
    
    return CfgMgr.LArRawChannelBuilder(name, **kwargs)

def getLArRawChannelBuilder_DigiHSTruth(name="LArRawChannelBuilder_DigiHSTruth" , **kwargs):
    # Currently just for the MC case, but could easily be extended to
    # cover the data case.

    from AthenaCommon.AppMgr import ToolSvc

    kwargs.setdefault('LArRawChannelContainerName', "LArRawChannels_DigiHSTruth")
    kwargs.setdefault('UseOFCTool', True)

    # Turn off printing for LArRoI_Map
    from LArRawUtils.LArRawUtilsConf import LArRoI_Map
    LArRoI_Map = LArRoI_Map("LArRoI_Map")
    ToolSvc += LArRoI_Map
    LArRoI_Map.Print=False
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.digitize.LAr_on() :
        kwargs.setdefault('DataLocation', 'LArDigitContainer_DigiHSTruth')

    kwargs.setdefault('ADC2MeVTool', 'LArADC2MeVToolDefault')

    kwargs.setdefault('OFCTool', 'LArOFCToolDefault')

    from LArROD.LArRODFlags import larRODFlags
    kwargs.setdefault('firstSample',larRODFlags.firstSample())

    return CfgMgr.LArRawChannelBuilder(name, **kwargs)
