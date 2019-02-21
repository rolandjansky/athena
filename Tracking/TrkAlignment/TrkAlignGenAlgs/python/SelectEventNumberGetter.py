# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   SelectEventNumberGetter.py
## @brief:  to instantiate the SelectEventNumber 
## 	    with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

## SelectEventNumberGetter configures the SelectEventNumber algorithm 
#  and adds to sequencer
class SelectEventNumberGetter ( Configured ) :

    _name = 'SelectEventNumberGetter'

    ## creates and configures SelectEventNumber
    def configure(self):
        mlog = logging.getLogger( self._name+'::configure :')
        
        from TrkAlignGenAlgs.TrkAlignGenAlgsConf import Trk__SelectEventNumber
        selEvtNumber = Trk__SelectEventNumber("SelectEventNumber",
                                              OutputLevel = align.OutputLevel(),
                                              EventList = align.eventList())
        
        self._selectEventNumberHandle = selEvtNumber

        mlog.info("created selectEventNumber:")
        mlog.info(selEvtNumber)

        # add to sequencer
        mlog.info(" now adding to sequencer" )
        from AthenaCommon.AppMgr import athMasterSeq
        sequencer = athMasterSeq
        sequencer += self.SelectEventNumberHandle()
        
        mlog.info(" leaving configure" )

        return True

    ## returns handle to SelectEventNumber
    def SelectEventNumberHandle(self):
        return self._selectEventNumberHandle
