# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import os
from AthenaCommon.Utils import unixtools

if not 'FNAMES' in dir():
    paths = os.getenv('DATAPATH').split(os.pathsep) + [os.getenv('ATLAS_REFERENCE_DATA','')]
    testdir = unixtools.find_datafile(os.getenv('ATLAS_REFERENCE_TAG'), paths)
    FNAMES = [os.path.join(testdir, f) for f in ('ntuple.0.root', 'ntuple.1.root')]

if not isinstance(FNAMES, (list,tuple)):
    FNAMES = [FNAMES]
    pass

import AthenaRootComps.ReadAthenaRoot
svcMgr.EventSelector.InputCollections = FNAMES
svcMgr.EventSelector.TupleName = "egamma"

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaPython import PyAthena
StatusCode = PyAthena.StatusCode

class MyAlg( PyAthena.Alg ):

    def __init__(self, name='MyAlg', **kw):
        kw['name'] = name
        self.activeBranches = kw.get(
            'activeBranches',
            ['RunNumber',
             'EventNumber',
             'el_n',
             'el_eta',
             'el_jetcone_dr',
             ])
        self.fname = kw.get('fname', 'data.var.txt')
        super(MyAlg, self).__init__(**kw)
        return

    def initialize(self):
        self.evtstore = PyAthena.py_svc('StoreGateSvc')
        self.fd = open(self.fname, 'w')
        return StatusCode.Success

    def execute(self):
        self.msg.info('running execute...')
        keys = []
        for p in self.evtstore.proxies():
            if not p.isValid():
                continue
            keys.append(p.name())
        for br in self.activeBranches:
            try:
                if not br in keys:
                    raise KeyError("no such object [%s] in store" % br)
                o = self.evtstore[br]
                if hasattr(o, 'at'):
                    o = list(o)
                    for i,v in enumerate(o):
                        if hasattr(v, 'at') and not isinstance(v, (basestring,)):
                            o[i] = list(v)
                self.msg.info('%s: %r', br, o)
                printfunc ("%s: %r" % (br, o), file=self.fd)
            except Exception as err:
                self.msg.info(' --> err for [%s]: %s' % (br, err))
                pass
        return StatusCode.Success

    def finalize(self):
        if hasattr(self, 'fd') and self.fd:
            self.fd.flush()
            self.fd.close()
        return StatusCode.Success

    pass # MyAlg

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += MyAlg('py_alg')

if 'BRANCHES' in dir():
    job.py_alg.activeBranches = BRANCHES
    pass

if not 'EVTMAX' in dir():
    EVTMAX=-1
theApp.EvtMax = EVTMAX

if not 'DOWRITE' in dir():
    DOWRITE=1
if DOWRITE:
    svcMgr += CfgMgr.DecisionSvc()
    import AthenaRootComps.WriteAthenaRoot as arcw
    out = arcw.createNtupleOutputStream("StreamD3PD", "d3pd.root", "egamma")
    if not 'OUTBRANCHES' in dir():
        OUTBRANCHES=[
            "el_n",
            "el_eta",
            "el_jetcone_dr",
            ]
    out.ItemList += OUTBRANCHES

