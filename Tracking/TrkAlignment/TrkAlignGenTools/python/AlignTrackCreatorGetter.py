# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   AlignTrackCreatorGetter.py
## @brief:  to instantiate the AlignTrackCreator 
## 	    with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

## AlignTrackCreatorGetter creates AlignTrackCreator but 
#  but does not add to ToolSvc, so it must be used as a 
#  private tool or added to the ToolSvc by the user
class AlignTrackCreatorGetter ( Configured ) :
  
  _name = 'AlignTrackCreatorGetter'

  def configure(self):
    mlog = logging.getLogger( self._name+'::configure :' )
            
    # create tool instance
    from TrkAlignGenTools.AlignResidualCalculatorGetter import AlignResidualCalculatorGetter
    resCalc = AlignResidualCalculatorGetter().AlignResidualCalculatorHandle()

    from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__AlignTrackCreator
    alignTrackCreator=Trk__AlignTrackCreator("AlignTrackCreator",
                                             OutputLevel = align.OutputLevel(),
                                             ResidualCalculator = resCalc,
                                             IncludeScatterers = align.includeScatterers(),
                                             RemoveATSOSNotInAlignModule = align.removeATSOSNotInAlignModule(),
                                             RequireOverlap = align.requireOverlap())                

    # options for good event list
    if align.writeEventList() : 
        alignTrackCreator.EventList = "goodEvts.txt"
        alignTrackCreator.WriteEventList = True

    self._AlignTrackCreatorHandle = alignTrackCreator

    #mlog.info(" now adding to ToolSvc")
    #from __main__ import ToolSvc 
    #ToolSvc += self.AlignTrackCreatorHandle()
    
    mlog.info(" leaving configure")
    
    return True
  
  ## returns AlignTrackCreator tool 
  def AlignTrackCreatorHandle(self):
    return self._AlignTrackCreatorHandle
