# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
import ROOT

class LumiCalcPyAthena( PyAthena.Alg ):

    def __init__ ( self, name = "LumiCalcPyAthena", **kw ):
        ## initialize base class
        kw['name'] = name
        super(LumiCalcPyAthena, self).__init__(**kw)

        #handle to lumicalcSvc
        self.lcSvc = kw.get('lcSvc', 'LumiCalcSvc/LumiCalcSvc')

        #handle to storegate
        self.sgSvc = None

        return

    def initialize(self):

        self.sgSvc = PyAthena.py_svc('StoreGateSvc')
        if not self.sgSvc :
            self.msg.error("could not retrieve a handle to the event store !")
            return StatusCode.Failure


        self.lcSvc = PyAthena.py_svc('LumiCalcSvc/LumiCalcSvc', iface='ILumiCalcSvc')
        if not self.lcSvc :
            self.msg.error("could not retrieve a handle to the lumicalc Service !")
            return StatusCode.Failure


        return StatusCode.Success

    def execute(self):

        return StatusCode.Success


    def finalize(self):

        print "Getting handle on LumiTree..."
        tree = self.lcSvc.getLumiTree()
        #print "Now printing out the whole tree..."
        #tree.Scan("*")



        return StatusCode.Success
        
