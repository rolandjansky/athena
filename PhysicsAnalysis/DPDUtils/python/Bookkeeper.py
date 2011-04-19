# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################
# Bookkeeper tool for skimming #
################################

# Creation: David Cote (DESY), August 2008
#
# WARNING: development version. Please do not use yet! ;-)
#
# Steps are:
#  1) get a connection to all skims via MSMgr (implemented)
#  2) implement a base class to PyAthena.Alg skims, in order to bookkeep the cut-flow of each skims
#  3) persistify the bookkeeping info in metadata during finalize() stage.


from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class Bookkeeper( PyAthena.Alg ):

    def __init__ ( self, name = "Bookkeeper", **kw ) :
        kw['name'] = name
        super(Bookkeeper,self).__init__(**kw)

    def initialize(self):
        self.evtCount=0
        self.skimList=[]
        from AthenaCommon.AlgSequence import AlgSequence
        theJob = AlgSequence()
        for Stream in MSMgr.StreamList:
            print "Stream :%s "%Stream.Name
            print "AcceptedAlgs are:"
            skimNameList=Stream.Stream.AcceptAlgs
            for skimName in skimNameList:
                skim=getattr(theJob,skimName)
                if isinstance(skim,PyAthena.Alg):
                    self.skimList.append(skim)
                    print " -%s (accepted)"%skimName
                else:
                    print "WARNING: %s will be ignored by the bookkeeper... (only PyAthena.Alg skims are supported)"%skimName
        return StatusCode.Success

    def execute(self):
        #in the first event, check that the number of skims is still the same...
        #Or better, go through this stuff every time a skim is added to the MSMgr
            
        for skim in self.skimList:
            print skim.filterPassed()
        self.evtCount+=1
        return StatusCode.Success
            
    def finalize(self):
        print "processed %i events."%self.evtCount
        return StatusCode.Success



