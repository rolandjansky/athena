# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    GaudiSequencer/python/PyComps.py
# @purpose: a set of python components for event filtering
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'A set of python components for event filtering'
__version__ = '$Revision: 1.4 $'
__author__  = 'Sebastien Binet <binet@cern.ch>'

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class PyEvtFilter (PyAthena.Alg):
    """Filtering algorithm to be used in conjunction with an AthSequencer.
       Different operating modes are possible:
         - give a list of events to accept
         - give a list of events to veto
       For each of these modes, the list of events can be:
         - a list of event numbers: [ 13, 45, 24, 90 ]
         - a list of pairs run/evt  [ (1,13), (1,45), ... ]

       Note that when in 'event list' mode and an event is found in the
       event list, the event is accepted or not according to the
       'filter_policy' property value ('accept' XOR 'reject')
       
       Another way of filtering events is to provide the component with a
       filtering function (via the 'filter_fct' property) which is expected
       to take 2 arguments:
        def my_filter_fct (runnbr, evtnbr):
            'accept all events'
            return True

       examples of usage:
       athena> seq = AthSequencer('seq')
       athena> from GaudiSequencer.PyComps import PyEvtFilter
       athena> seq += PyEvtFilter ('alg')
       athena> seq.alg.filter_fct = lambda run,evt: evt in xrange(100)

       or:
       athena> def my_filter_fct(runnbr, evtnbr):
           ...     if runnbr > 5001 and runnbr < 5100 and
           ...        ( evtnbr in [ 1, 34, 600] or evtnbr > 600 ):
           ...        return True
           ...     return False
       athena> seq.alg.filter_fct = my_filter_fct

       or: (veto events 3,4 and 10)
       athena> seq.alg.filter_fct = lambda r,e : e not in [3,4,10]
       
       or:
       athena> seq.alg.evt_list = [ (5010, 3), (5010, 4) ]
       athena> seq.alg.evt_list = xrange(100)
       athena> seq.alg.evt_list = range(10) + range(15,20)
       athena> seq.alg.evt_list = map (lambda evt: (5100, evt), xrange(100))

       or, to veto events:
       athena> seq.alg.evt_list = [ 10, 3, 40 ]
       athena> seq.alg.filter_policy = 'reject'
    """
    def __init__(self, name='PyEvtFilter', **kw):
        ## init base class
        kw['name'] = name
        super(PyEvtFilter, self).__init__(**kw)

        ## properties and data members
        # list of events to accept
        self.evt_list   = kw.get('evt_list',   None) # default value
        # filtering policy for current event found in event list
        self.filter_policy = kw.get('filter_policy', 'accept')
        # filtering function
        self.filter_fct = kw.get('filter_fct', None) # default value
        
        # location of EventInfo
        self.evt_info = kw.get('evt_info', None)
        
        return

    def initialize(self):
        _info  = self.msg.info
        _error = self.msg.error
        _info('==> initialize...')
        
        # check our properties and which modus operandi is active
        if self.evt_list is None and self.filter_fct is None:
            _error ('invalid properties: evt_list *and* filter_fct are None !')
            _error ('usage:\n%s', self.__doc__)
            return StatusCode.Failure

        if (not (self.evt_list is None)) and \
           (not (self.filter_fct is None)):
            _error ('invalid properties: evt_list *and* filter_fct '
                    'are not None !')
            _error ('usage:\n%s', self.__doc__)
            return StatusCode.Failure

        if not (self.filter_fct is None):
            # checking the filtering function is sound
            import inspect
            args = inspect.getargspec (self.filter_fct)[0]
            # FIXME: class' methods have the hidden 'self' argument...
            if len(args) != 2:
                _error ('filter_fct has NOT the expected signature')
                _error (' nbr of arguments: %i', len(args))
                _error (' expected:         2')
                return StatusCode.Failure
            if not (self.filter_policy in ('reject', 'accept')):
                _error ('invalid value for filter_policy: %r',
                        self.filter_policy)
                _error ("valid values are 'reject' or 'accept'")
                return StatusCode.Failure
        else:
            # transform the list of events into a set of pairs (run,evt)
            evt_list = self.evt_list[:]
            self.evt_list = set()
            for i in evt_list:
                if isinstance(i, tuple): irun, ievt = i
                else:                    irun, ievt = None, i
                self.evt_list.add ((irun, ievt))
                
        _info ('EventInfo name:  %s',
               self.evt_info
               if self.evt_info else '<any>')
        
        if self.evt_list:
            _info("filtering mode:   evt_list")
            _info("filtering policy: %s", self.filter_policy)
            self.filter_policy = False if self.filter_policy == 'reject' else \
                                 True
        else:
            _info ("filtering mode:  filter_fct")

        self.sg = PyAthena.py_svc ('StoreGateSvc')
        if self.sg is None:
            _error ('could not retrieve event store')
            return StatusCode.Failure

        self._evt_cnt = 0     # total number of events processed
        self._evt_acc_cnt = 0 # total number of events accepted by the filter
        self._evt_rej_cnt = 0 # total number of events rejected by the filter
        return StatusCode.Success

    def execute(self):
        self._evt_cnt += 1
        _info = self.msg.info
        _error= self.msg.error

        evtinfo = self.sg.retrieve ('EventInfo', self.evt_info)
        if evtinfo is None:
            _error ('could not retrieve EventInfo at [%s]', self.evt_info)
            return StatusCode.Failure

        filter_passed = None
        
        evtid = evtinfo.event_ID()
        runnbr = evtid.run_number()
        evtnbr = evtid.event_number()
        if self.filter_fct:
            filter_passed = self.filter_fct (runnbr, evtnbr)
        else:
            if (runnbr,evtnbr) in self.evt_list or \
               (None,  evtnbr) in self.evt_list:
                filter_passed = self.filter_policy
            else:
                filter_passed = not self.filter_policy
                
                
        if not isinstance(filter_passed, bool):
            self.msg.error ('invalid filter-passed decision: %r', filter_passed)
            return StatusCode.Failure

        self.setFilterPassed (filter_passed)
        if filter_passed:
            self._evt_acc_cnt += 1
            dec = 'accepted'
        else:
            self._evt_rej_cnt += 1
            dec = 'rejected'
        _info ('[run=%s | evt=%s] ==> [%s]',
               str(runnbr).zfill(5),
               str(evtnbr).zfill(5),
               dec)
        return StatusCode.Success

    def finalize(self):
        _info = self.msg.info
        _info ('==> finalize...')
        _info (' #evts processed: %i', self._evt_cnt)
        _info (' #evts accepted : %i', self._evt_acc_cnt)
        _info (' #evts rejected : %i', self._evt_rej_cnt)
        return StatusCode.Success

    # class PyEvtFilter
