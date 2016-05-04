# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class PassedTriggerFilter (PyAthena.Alg):
    def __init__(self, name='PassedTriggerFilter', **kw):
        ## init base class
        kw['name'] = name
        super(PassedTriggerFilter, self).__init__(**kw)
        self.__all = 0
        self.__pass = 0
        self.__stat_per_stream = {}
        
    def initialize(self):
        self.sg = PyAthena.py_svc ('StoreGateSvc')
        if self.sg is None:
            return StatusCode.Failure
        return StatusCode.Success


    def execute(self):        
        self.__all += 1
        _error = self.msg.error
        _info = self.msg.info

        evtinfo = self.sg.retrieve ('EventInfo', '')
        if evtinfo is None:

            return StatusCode.Failure

        trigger = evtinfo.trigger_info()
        if trigger is None:
            _error ('could not retrieve TriggerInfo' )
            return StatusCode.Failure

        stags   =  trigger.streamTags()        
        streams = [s.name() for s in list(stags) if s.type() == 'physics']
        _info(str(streams))
        
        for s in streams:
            self.__stat_per_stream[s] = self.__stat_per_stream.get(s, 0) + 1


        if streams: # empty list == False, non-empty == True
            self.__pass += 1
            self.setFilterPassed(True)
        else:
            self.setFilterPassed(False)

        return StatusCode.Success

    def finalize(self):
        _info = self.msg.info
        _info('Events Processed : %d' % self.__all)
        _info('Events Passed    : %d' % self.__pass)
        events_duplicated = reduce(lambda x,y: x+y, self.__stat_per_stream.itervalues(), 0)
        _info('Events with dup. : %d' % events_duplicated)
            
        
        for name, count in self.__stat_per_stream.iteritems():
            _info('Events in stream %s: %d' % (name, count))
        
        return StatusCode.Success

