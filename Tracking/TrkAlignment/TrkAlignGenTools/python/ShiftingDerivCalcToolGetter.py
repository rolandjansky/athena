# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   ShiftingDerivCalcToolGetter.py
## @brief:  to instantiate the ShiftingDerivCalcToolDresser
## 	    with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

## ShiftingDerivCalcToolGetter creates and configures the 
#  ShiftingDerivCalcTool with default parameters.  The tool 
#  is not added to ToolSvc so user must use it as a private 
#  tool or add to ToolSvc as a public tool.
class ShiftingDerivCalcToolGetter ( Configured ) :

    _name = 'ShiftingDerivCalcToolGetter'

    ## creates and configures tool
    def configure(self):
        mlog = logging.getLogger( self._name+'::configure :' )
    
        # create tool instance
        from TrkAlignGenTools.AlignResidualCalculatorGetter import AlignResidualCalculatorGetter
        resCalc = AlignResidualCalculatorGetter().AlignResidualCalculatorHandle()

        from TrkAlignGenTools.AlignmentTrackFitter import AlignmentTrackFitter
        from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__ShiftingDerivCalcTool
        shiftingDerivCalcTool=Trk__ShiftingDerivCalcTool("ShiftingDerivCalcTool",
                                                         OutputLevel=AlignmentFlags.OutputLevel(),
                                                         TrackFitterTool = AlignmentTrackFitter().fitter(),
                                                         ResidualCalculator = resCalc,
                                                         TranslationSize = mualign.shiftSizeTranslations(),
                                                         RotationSize = mualign.shiftSizeRotations(),
                                                         NumberOfShifts = mualign.numberOfShifts(),
                                                         doChi2VChamberShiftsMeasType = mualign.doChi2VChamberShiftsMeasType())
        if AlignmentFlags.useSLFitter() :
            shiftingDerivCalcTool.SLTrackFitterTool = AlignmentTrackFitter().slfitter()

        # configure other jobOptions
        shiftingDerivCalcTool.RunOutlierRemoval    = AlignmentFlags.runOutlierRemoval()
        shiftingDerivCalcTool.ParticleNumber       = AlignmentFlags.particleNumber()
        shiftingDerivCalcTool.TrackAlignParamCut   = AlignmentFlags.trackAlignParamCut()
        shiftingDerivCalcTool.SetMinIterations     = AlignmentFlags.setMinIterations()

        self._ShiftingDerivCalcToolHandle = shiftingDerivCalcTool
        
        mlog.info(" now adding to ToolSvc")
        from __main__ import ToolSvc 
        ToolSvc += self.ShiftingDerivCalcToolHandle()
        
        mlog.info(" leaving configure")

        return True

    ## returns handle to ShiftingDerivCalcTool
    def ShiftingDerivCalcToolHandle(self):
        return self._ShiftingDerivCalcToolHandle
