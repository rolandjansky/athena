###########################################################################
#    Copyright (C) 2009 by Miroslav Nozicka
#    <nozicka@mail.desy.de>
#
#   DESCRIPTION: TrigHistorySvc - derived configurable of the HistorySvc
#                - extends the functionality of the HistorySvc obtaining 
#                  the python class (py_name) and python module (py_package)
#                  of the algorithm
#
###########################################################################


from AthenaPython.PyAthena import StatusCode

from AthenaPython.Configurables import CfgPyAlgorithm, _get_prop_value

class TrigHistoryAlg(CfgPyAlgorithm) :
    def __init__(self, name='TrigHistoryAlg', **kw):
        super(TrigHistoryAlg, self).__init__(name, **kw)
        from AthenaCommon.Logging import logging
        self.__dict__['msg'] = logging.getLogger( 'TrigHistoryAlg.py' )
        return
    
    def sysInitialize(self):
        self.msg.setLevel(_get_prop_value(self,'OutputLevel'))
        self.msg.info("sysInitialize")
        return self.initialize()
  
    def initialize(self):
        self.msg.info("initialize")
        from TrigConfOffline.TrigHistoryDumper import dump
        dump(self.OutputFile)
        return StatusCode.Success
  
    def sysReinitialize(self):
        self.msg.setLevel(_get_prop_value(self,'OutputLevel'))
        return self.reinitialize()
    
    def reinitialize(self):
        self.msg.info( "==> re-initialize..." )
        return StatusCode.Success
  
    def sysExecute(self):
        #self.msg.info("Mirek: TestAlg - sysExecute")
        return StatusCode.Success
    
    def execute(self):
        #self.msg.info("Mirek: TestAlg - execute")
        return StatusCode.Success
  
    def sysFinalize(self):
        self.msg.info("sysFinalize")
        return self.finalize()
    
    def finalize(self):
        self.msg.info("finalize")
        return StatusCode.Success
  
    def sysBeginRun(self):
        return self.beginRun()
    
    def beginRun(self):
        self.msg.debug( "==> beginRun..." )
        return StatusCode.Success
  
    def sysEndRun(self):
        return self.endRun()
    
    def endRun(self):
        self.msg.debug( "==> endRun..." )
        return StatusCode.Success
  
    def filterPassed(self):
        """Did this algorithm pass or fail its filter criterion for the last event?"""
        return self._cppHandle.filterPassed()
  
    def setFilterPassed(self, state):
        """Set the filter passed flag to the specified state"""
        return self._cppHandle.setFilterPassed(state)
