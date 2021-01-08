# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file:    PerfMonComps/python/PyComps.py
# @purpose: a set of python components to perform performance monitoring
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'a set of python components to perform performance monitoring'
__version__ = '$Revision: 298807 $'
__author__  = 'Sebastien Binet <binet@cern.ch>'

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class PyStorePayloadMon (PyAthena.Svc):
    'a simple class to monitor the payload size of objects in StoreGate'
    def __init__(self, name='PyStorePayloadMon', **kw):
        ## init base class
        super(PyStorePayloadMon, self).__init__(name, **kw)

        ## properties and data members
        self.sg_name = kw.get('sg_name', 'StoreGateSvc/StoreGateSvc')
        """StoreGateSvc instance to monitor for DataProxy payload sizes"""
        
        self.oname = kw.get('oname', 'pmon_payload.ascii')
        """Name of the output file where the monitoring data will be stored"""

        self.display_mall_infos = kw.get(
            'display_mall_infos', False)
        """display mallinfos stats after each event"""

        import Hephaestus.MemoryTracker as memtrack
        from Hephaestus.MemoryTracker import FreeStatistics as heph_stats
        self.heph_stats = heph_stats

        mask = memtrack.configure()
        memtrack.configure( mask | memtrack.FREESTAT )
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.sg = PyAthena.py_svc(self.sg_name)
        if not self.sg:
            self.msg.error('could not retrieve sg instance [%s]', self.sg_name)
            return StatusCode.Failure

        ns = PyAthena.AthenaInternal
        self._sg_clear = ns.py_sg_clearProxyPayload

        self.clidsvc = PyAthena.py_svc('ClassIDSvc/ClassIDSvc',
                                       iface='IClassIDSvc')
        if not self.clidsvc:
            self.msg.error('could not retrieve clidsvc')
            return StatusCode.Failure
     
        self.heph_stats.start()

        import os
        import os.path
        if os.path.exists(self.oname):
            os.remove(self.oname)
        self.ostream = open(self.oname, 'w')

        # describe data format
        print("#data format: [[(bytes_0, bytes_1, delta, clid, sg_key),]]", file=self.ostream)
        print("data = [", file=self.ostream)
        
        # register incident handles
        incsvc = PyAthena.py_svc('IncidentSvc', iface='IIncidentSvc')
        incsvc.addListener(self, 'BeginEvent')
        incsvc.addListener(self, 'EndEvent')

        return StatusCode.Success

    def handle(self, incident):
        tp = incident.type()
        if tp == 'BeginEvent':
            self.heph_stats.reset()
            #self.heph_stats.start()
            pass
        elif tp == 'EndEvent':
            #self.heph_stats.stop()
            self.execute()
            #self.heph_stats.reset()
        else:
            pass
        return
        
    def execute(self):
        proxies = list(self.sg.proxies())
        data = []
        fs = self.heph_stats
        fd = self.ostream
        
        mem_store_0, ncalls_0 = fs.stat()
        data = map(self.process_proxy, proxies)
        mem_store_1, ncalls_1 = fs.stat()

        clid2name = self.clidsvc.typename
        fmt = "   (%10i, %10i, %10i, \"%s\", \"%s\"),"
        
        print(" [ ## new-event", file=fd)
        for p, mem_0, mem_1 in data:
            tp_name = clid2name(p.clID())
            print(fmt, (mem_0, mem_1, mem_0 - mem_1, tp_name, p.name()), file=fd)
            pass
        mem_store_0 = int(mem_store_0)
        mem_store_1 = int(mem_store_1)
        
        print(fmt, (
            mem_store_0, mem_store_1, mem_store_0 - mem_store_1,
            "StoreGateSvc", self.sg_name
            ), file=fd)
        print(" ],", file=fd)

        self.msg.info('flush-store: %10d -> %10d -- delta= %10d',
                      mem_store_0, mem_store_1, mem_store_0 - mem_store_1)
        return StatusCode.Success

    def process_proxy(self, p):
        fs = self.heph_stats
        mem_0 = -1
        mem_1 = -1
        if p.clID() == 2101: # event-info
            return (p, mem_0, mem_1)
        
        mem_0, ncalls_0 = fs.stat()
        self._sg_clear(self.sg, p)
        mem_1, ncalls_1 = fs.stat()
        
        ## print("::: %f -> %f ==> %f [%i -> %i] (%s/%s)" % (
        ##     mem_0, mem_1, mem_1 - mem_0, ncalls_0, ncalls_1,
        ##     p.clID(), p.name()
        ##     ))
        return (p, int(mem_0), int(mem_1))
    
    def finalize(self):
        self.msg.info('==> finalize...')
        print("] # data", file=self.ostream)
        self.heph_stats.stop()
        
        return StatusCode.Success

    # class PyStorePayloadMon
