# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        EventCounterAlg
##
## Author:      David Cote (CERN)
## Created:     August 2011
##
## Description: Trivial alg to provide the number of executed events to CutFlowSvc
##              Note that this does not support TAG filtering
##              For TAG filtering, there was the EventSelectorCounterTool, but it is no longer supported at the athena core level
##=============================================================================

__doc__ = """Trivial alg to provide the number of executed events to CutFlowSvc
"""

import AthenaCommon.CfgMgr as CfgMgr
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

EventCounterAlg = CfgMgr.EventCounterAlg

class PyEventCounterAlg( PyAthena.AthFilterAlgorithm ):

    def __init__ ( self, name = "EventCounterAlg", **kw ):
        kw['name'] = name
        super(PyEventCounterAlg, self).__init__(**kw)
        return

    def initialize(self):
        self.cutFlowSvc().registerTopFilter( self.name(),
                                             "Number of processed events before any cut",
                                             #xAOD::EventBookkeeper::CutLogic::ALLEVENTSPROCESSED,
                                             1,
                                             "AllStreams" )
        return StatusCode.Success

    def execute(self):
        self.setFilterPassed(True)
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success
