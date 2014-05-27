# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   AlignTrackDresserGetter.py
## @brief:  to instantiate the AlignTrackDresser
## 	    with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags

## AlignTrackDresserGetter creates an AlignTrackDresser
#  but does not add to ToolSvc, so user has to use as a
#  private or add to ToolSvc as public tool
class AlignTrackDresserGetter ( Configured ) :
  
  _name = 'AlignTrackDresserGetter'

  def configure(self):
    mlog = logging.getLogger( self._name+'::configure :' )
            
    # create tool instance
    from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__AlignTrackDresser
    alignTrackDresser=Trk__AlignTrackDresser("AlignTrackDresser",
                                             OutputLevel = AlignmentFlags.OutputLevel(),
                                             DerivCalcTool = self.derivativeCalcTool())

    self._AlignTrackDresserHandle = alignTrackDresser
    
    #mlog.info(" now adding to ToolSvc")
    #from __main__ import ToolSvc 
    #ToolSvc += self.AlignTrackDresserHandle()
    
    mlog.info(" leaving configure")
    
    return True
  
  ## returns AlignTrackDresser
  def AlignTrackDresserHandle(self):
    return self._AlignTrackDresserHandle

  ## returns derivative calculator owned by AlignTrackDresser.  If it does
  #  exist, creates new one based on AlignmentFlags.
  def derivativeCalcTool(self):
    try: 
      self._derivCalcTool
    except:
      derivativeType = AlignmentFlags.derivativeType()
      if derivativeType == 'Analytical' :      
        from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__AnalyticalDerivCalcTool
        self._derivCalcTool = Trk__AnalyticalDerivCalcTool("AnalyticalDerivCalcTool",
                                                           OutputLevel=AlignmentFlags.OutputLevel())
      elif derivativeType == 'Numerical' :
        from TrkAlignGenTools.ShiftingDerivCalcToolGetter import ShiftingDerivCalcToolGetter
        self._derivCalcTool = ShiftingDerivCalcToolGetter().ShiftingDerivCalcToolHandle()
      else:
        print "bad option!"
    print "have derivCalcTool"
    return self._derivCalcTool



