# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrkDistributedKalmanFilter.TrkDistributedKalmanFilterConf import Trk__DistributedKalmanFilter

class ConfiguredDistributedKalmanFilter(Trk__DistributedKalmanFilter):
    __slots__ = []
    def __init__(self, name = "ConfiguredDistributedKalmanFilter", MagTool = None, ExtrapTool = None, ROTCreator = None):
        Trk__DistributedKalmanFilter.__init__(self, name)
        from __main__ import ToolSvc
        if ExtrapTool is None :
            from TrkExTools.TrkExToolsConf import Trk__Extrapolator
            myExtrapolator = Trk__Extrapolator()
            ToolSvc += myExtrapolator
            self.ExtrapolatorTool = myExtrapolator

        if MagTool is None :
            from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
            myMagneticFieldTool = Trk__MagneticFieldTool()
            ToolSvc += myMagneticFieldTool
            self.MagFieldTool = myMagneticFieldTool
        
        if ROTCreator is None :
            from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
            myRIO_OnTrackCreator = Trk__RIO_OnTrackCreator(name = 'RotCreator', Mode='indet')
            ToolSvc += myRIO_OnTrackCreator
            self.ROTcreator = myRIO_OnTrackCreator

    def msgStreamLevel(self, level):
        self.OutputLevel = level
    
    def printInfo(self):
        print '***** ConfiguredDistributedKalmanFilter ***************************************'
        print '* - ToolName:                    ',self.name
        print '* - Magnetic Field Tool Name     ',self.MagFieldTool.getName()
        print '* - ROT Creator Tool Name        ',self.ROTcreator.getName()
        print '* - Extrapolation Tool Name      ',self.ExtrapolatorTool.getName()
        print '*******************************************************************************'







