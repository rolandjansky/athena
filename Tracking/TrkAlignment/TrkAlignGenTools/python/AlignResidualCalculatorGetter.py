# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   AlignResidualCalculatorGetter.py
## @brief:  to instantiate the AlignResidualCalculatorTool 
## 	      with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009
  

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags


## AlignResidualCalculatorGetter creates a public 
#  AlignResidualCalculator with default options
#  and adds tool to ToolSvc

class AlignResidualCalculatorGetter ( Configured ) :

    _name = 'AlignResidualCalculatorGetter'

    def configure(self):
        mlog = logging.getLogger( self._name+'::configure :' )
        outputLevel = AlignmentFlags.OutputLevel()

        # create tool instance
        from AthenaCommon.AppMgr import ToolSvc
        
        from MuonResidualPullCalculators.MuonResidualPullCalculatorsConf import Muon__TGC_ResidualPullCalculator
        tgc_respullcalc=Muon__TGC_ResidualPullCalculator("TGC_ResidualPullCalculator")
                                                         #OutputLevel=outputLevel)
        ToolSvc += tgc_respullcalc
        
        from InDetTrackValidation.InDetTrackValidationConf import InDet__SCT_ResidualPullCalculator
        sct_respullcalc=InDet__SCT_ResidualPullCalculator()
        ToolSvc+=sct_respullcalc
        
        #from TrkValTools.TrkValToolsConf import Trk__ResidualPullCalculator
        from TrkResidualPullCalculator.TrkResidualPullCalculatorConf import \
            Trk__ResidualPullCalculator 
        resPullCalc = Trk__ResidualPullCalculator("ResidualPullCalculator",
                                                  #OutputLevel=outputLevel,
                                                  ResidualPullCalculatorForTGC=tgc_respullcalc,
                                                  ResidualPullCalculatorForSCT=sct_respullcalc)
        ToolSvc += resPullCalc

        self._ResidualPullCalculatorHandle = ToolSvc.ResidualPullCalculator

        from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__AlignResidualCalculator
        resCalc = Trk__AlignResidualCalculator("AlignResidualCalculator",
                                               OutputLevel = outputLevel,
                                               ResidualType=0,
                                               ResidualPullCalculator = resPullCalc,
                                               IncludeScatterers = AlignmentFlags.includeScatteringResiduals())

        self._AlignResidualCalculatorHandle = resCalc

        mlog.info(" now adding to ToolSvc")
        ToolSvc += self.AlignResidualCalculatorHandle()

        mlog.info(" leaving configure")

        return True

    ## returns AlignResidualCalculator tool
    def AlignResidualCalculatorHandle(self):
        return self._AlignResidualCalculatorHandle

    ## returns ResidualPullCalculator
    def ResidualPullCalculatorHandle(self):
        return self._ResidualPullCalculatorHandle
