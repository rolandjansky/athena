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

from AthenaCommon import CfgMgr

## Message log
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigHistorySvc.py' )

## Status code
from AthenaPython.PyAthena import StatusCode

class TrigHistorySvc(CfgMgr.HistorySvc) :
    def __init__(self, name="HistorySvc", **kwargs) :
        super(TrigHistorySvc, self).__init__(name)
        for n,v in kwargs.items():
            setattr(self, n, v)
  
    def setup(self) :
        # This method is called during the ApplicationMgr.setup() -> ServiceMgr.setup()
        # Here is the last chance to obtain the py_package and py_name of every algorithm
        log.info("TrigHistorySvc.setup()")
        selfproperties = self.getProperties()
        super(TrigHistorySvc, self).setup()

        if 'Dump' in selfproperties and selfproperties['Dump'] not in ('<no value>', False,'',None) :
            log.info("Adding TrigHistoryAlg to the end of the athena master sequence, which will dump the configuration in initialize")
            from AthenaCommon.AppMgr import athMasterSeq
            from TrigConfOffline.TrigHistoryAlg import TrigHistoryAlg
            try:
                outfile = self.getProperties()['OutputFile']
            except KeyError:
                outfile = '<no value>'
            athMasterSeq += TrigHistoryAlg(OutputFile=outfile)
            
        log.info("TrigHistorySvc finished the setup")

