# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaPython import PyAthena
StatusCode = PyAthena.StatusCode

class PyReadD3pdNtuple( PyAthena.Alg ):

    def __init__(self, name='PyReadD3pdNtuple', **kw):
        kw['name'] = name
        super(PyReadD3pdNtuple, self).__init__(**kw)

        self.treename = kw.get('TreeName',
                               '/temp/TTreeStream/egamma')
        return

    def initialize(self):

        hsvc = PyAthena.py_svc('THistSvc', iface='ITHistSvc')
        if not hsvc:
            self.msg.error("Could not retrieve THistSvc !")
            return StatusCode.Failure

        self.tree = hsvc.get(self.treename, klass='TTree')
        if not self.tree:
            self.msg.error('could not retrieve tree from THistSvc')
            return StatusCode.Failure
        return StatusCode.Success

    def execute(self):
        info = self.msg.info
        info('running execute...')
        t = self.tree
        info('run-nbr: %s', t.RunNumber)
        info('evt-nbr: %s', t.EventNumber)
        info('el-nbr:  %s', t.el_n)
        if t.el_n >0:
            info('el.eta[0]: %s', t.el_eta[0])
            nmax = len(t.el_jetcone_dr[0])
            info('el_jetcone_dr[0].size: %s', nmax)
            for i in range(nmax):
                info(' jet-cone-dr[0][%i]: %s', i, t.el_jetcone_dr[0][i])
                
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success

    pass # PyReadD3pdNtuple

class PyReadD3pdEvtStore( PyAthena.Alg ):

    def __init__(self, name='PyReadD3pdEvtStore', **kw):
        kw['name'] = name
        super(PyReadD3pdEvtStore, self).__init__(**kw)

        return

    def initialize(self):

        self.evtstore = PyAthena.py_svc('StoreGateSvc')
        if not self.evtstore:
            self.msg.error('could not retrieve EvtStore')
            return StatusCode.Failure
        return StatusCode.Success

    def execute(self):
        info = self.msg.info
        info('running execute...')
        sg = self.evtstore
        info('run-nbr: %s', sg["RunNumber"])
        info('evt-nbr: %s', sg["EventNumber"])
        info('el-nbr:  %s', sg["el_n"])
        if sg["el_n"] >0:
            el_eta = sg["el_eta"]
            el_jetcone_dr = sg["el_jetcone_dr"]
            info('el.eta[0]: %s', el_eta[0])
            nmax = len(el_jetcone_dr[0])
            info('el_jetcone_dr[0].size: %s', nmax)
            for i in range(nmax):
                info(' jet-cone-dr[0][%i]: %s', i, el_jetcone_dr[0][i])
                
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success

    pass # PyReadD3pdEvtStore


