# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        FilterTester
##
## Author:      David Cote (DESY)
## Created:     August 2009
##
## Description: A dummy filter to test CutFlowSvc
##=============================================================================

__doc__ = """This dummy filter provides a simple example.
"""

from AthenaPython.PyAthena import AthFilterAlgorithm, StatusCode

class FilterTester( AthFilterAlgorithm ):

    def __init__ ( self, name = "FilterTester", **kw ):
        kw['name'] = name
        super(FilterTester, self).__init__(**kw)
        return

    def initialize(self):
        self.nProcessed = 0
        return StatusCode.Success

    def execute(self):
        self.nProcessed += 1        

        self.setFilterPassed(False)
        if self.nProcessed % 2 == 0:
            self.setFilterPassed(True)

        return StatusCode.Success

    def finalize(self):
        self.msg.info( ' Number of processed events:  %r' % self.nProcessed )
        return StatusCode.Success
