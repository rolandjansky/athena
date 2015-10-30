# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class FinalizationSvc(PyAthena.Svc):
    """Service executing calables at finalization
    """
    def __init__(self, name="FinalizationSvc"):
        super(FinalizationSvc,self).__init__(name=name)
        self.__calls = []

    def calls(self):
        return self.__calls

    def append(self, callable):
        self.__calls.append(callable)

    def initialize(self):
        return StatusCode.Success
        
    def finalize(self):
        self.msg.info("executing finalization calls")
        for c in self.__calls:
            self.msg.info("executing finalization call: %s" % c.__name__)
            try:
                c()
            except:
                self.msg.warning("exception thrown while executing finalization call: %s" % c.__name__)                
        return StatusCode.Success

def runAtFinalize(call):
    """ The helper for the above service.
    
    The usual uage should be:
    from TrigValAlgs.FinalizationSvc import runAtFinalize
    runAtFinalize(calable_object_or_function)        
    """

    
    from AthenaCommon.AppMgr import ServiceMgr,theApp
    if not hasattr(ServiceMgr, 'FinalizationSvc'):
        #theApp.CreateSvc.append('FinalizationSvc/FinalizationSvc')
        svc = FinalizationSvc()
        ServiceMgr += svc
        theApp.CreateSvc += [svc.getFullName()]
        print theApp.CreateSvc
    svc = ServiceMgr.FinalizationSvc
    svc.append(call)
    
        
            
def file_deleter(todel):
    """
    The factory of closures which can be used to delete single files or list of files.

    The usage example:
    from TrigValTools.FinalizationSvc import runAtFinalize, file_deleter
    runAtFinalize(file_deleter('Lvl1Results.txt'))
    """
    files_list = todel
    if type(todel) == type(''):
        files_list = [todel]

    def  __file_deleter():
        import os 
        for f in files_list:
            print 'file_deleter, attempting to remove: %s' % f 
            os.remove(f)
    return __file_deleter
        
    
