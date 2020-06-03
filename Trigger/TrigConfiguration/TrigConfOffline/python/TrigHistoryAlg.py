###########################################################################
#    
#    <nozicka@mail.desy.de>
#
#   DESCRIPTION: TrigHistorySvc - derived configurable of the HistorySvc
#                - extends the functionality of the HistorySvc obtaining 
#                  the python class (py_name) and python module (py_package)
#                  of the algorithm
#
###########################################################################


from AthenaPython.PyAthena import Alg, StatusCode

class TrigHistoryAlg(Alg) :
    def __init__(self, name='TrigHistoryAlg', **kw):
        super(TrigHistoryAlg, self).__init__(name, **kw)
        from AthenaCommon.Logging import logging
        self.__dict__['msg'] = logging.getLogger( 'TrigHistoryAlg.py' )
        return
    
    def initialize(self):
        self.msg.info("initialize - dumping configuration")
        from TrigConfOffline.TrigHistoryDumper import dump
        dump(self.OutputFile)
        return StatusCode.Success
