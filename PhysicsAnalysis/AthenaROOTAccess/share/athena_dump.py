##
# $Id: athena_dumper.py,v 1.33 2009-02-11 02:18:49 ssnyder Exp $
#
# @file AthenaROOTAccess/share/athena_dump.py
# @author sss
# @date Jul 2007
# @brief Test for AthenaROOTAccess.
#
# This test uses Athena to dump AOD-full.pool.root to dump.out/dump-rand.out.
#

# Turn off annoying dict auto-generation --- it doesn't work anyway.
import ROOT
ROOT.gInterpreter.ProcessLine(".autodict")

# Make sure we don't have a stale file catalog.
import os
if os.path.exists ('PoolFileCatalog.xml'):
    os.remove ('PoolFileCatalog.xml')

#theApp.EvtMax = 10


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
theApp.AuditAlgorithms=False
theApp.AuditServices=False
topSequence = AlgSequence()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-R1-2012-03-01-00'
import AtlasGeoModel.GeoModelInit
import AtlasGeoModel.SetGeometryVersion
svcMgr.GeoModelSvc.IgnoreTagDifference = True

# Prevent crashes from partially-initialized tools.
from TrkEventCnvTools import TrkEventCnvToolsConf
EventCnvSuperTool = \
             TrkEventCnvToolsConf.Trk__EventCnvSuperTool('EventCnvSuperTool')
EventCnvSuperTool.DoMuons = False
EventCnvSuperTool.DoID = False
ToolSvc += EventCnvSuperTool

from AthenaServices.AthenaServicesConf import FPEControlSvc
svcMgr += FPEControlSvc()
theApp.CreateSvc += [ svcMgr.FPEControlSvc.getFullJobOptName() ]

# Work around borked PPS --- it can't find its input file with the
# default setting.
from PartPropSvc.PartPropSvcConf import PartPropSvc
svcMgr += PartPropSvc(InputFile='PDGTABLE.MeV=PDG')


import xAODJetCnv.ParticleJetCompatibility

if not globals().get('input_file'):
    input_file = 'AOD.pool.root'

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ input_file ]
#ServiceMgr.EventSelector.InputCollections = [ "AOD-13.0.30.3/AOD-13.0.30.3-full.pool.root" ]
#ServiceMgr.EventSelector.InputCollections = [ "AOD-dev/AOD-dev-full.pool.root" ]

#svcMgr.PoolSvc.AttemptCatalogPatch = True

#include ("PyAnalysisCore/InitPyAnalysisCore.py")

from AthenaROOTAccess.dumpers import *
from PyKernel import PyKernel

if not globals().has_key ('onlykeys'):
    onlykeys = []
if not globals().has_key ('onlytypes'):
    onlytypes = []

from AthenaPython import PyAthena
class Dumper (PyAthena.Alg):
    def __init__ (self, name):
        PyAthena.Alg.__init__ (self, name)
        #self.f = open ('dump.out', 'w')
        #self.f_rand = open ('dump-rand.out', 'w')
        import bz2
        self.f = bz2.BZ2File ('dump.out.bz2', 'w')
        self.f_rand = bz2.BZ2File ('dump-rand.out.bz2', 'w')
        self.evdump = Evdump (self.f, self.f_rand,
                              onlykeys = onlykeys, onlytypes = onlytypes)
        self.retr = PyKernel.retrieve
        return

    def initialize (self):
        self.sg = PyAthena.py_svc('StoreGateSvc')
        self.cs = PyAthena.py_svc('ClassIDSvc')
        return 1

    def get_dhes (self):
        dh = self.sg.retrieve ("DataHeader")
        dhes = []
        for dhe in dh.elements():
            tname = self.cs.typename(dhe.getPrimaryClassID())
            if tname:
                dhes.append ((dhe.getKey(), tname))
        return dhes

   
    def sg_getter (self, d, dhe, keyprint):
        typ = dhe.transtype
        try:
            return self.retr (typ, dhe.key)
        except LookupError:
            if typ == 'TruthParticleContainer':
                return None
            if keyprint != dhe.key:
                return self.retr (typ, keyprint)
            raise
    def execute (self):
        self.evdump.dump_event_common (self.get_dhes(), self.sg_getter)
        return 1

    def finalize (self):
        self.evdump.print_missed (sys.stdout)
        self.f.close()
        self.f_rand.close()
        return 1


dumper = Dumper('dumper')
#topSequence += [ConfigurableUser('dumper')]
topSequence += [dumper]

from RootUtils import PyROOTFixes
#PyROOTFixes.fix_dv_container ("MissingETSigHypoContainer")
#PyROOTFixes.fix_dv_container ("DataVector<Trk::Segment>")
#theApp.getHandle().Dlls += ["PoolSvc"]



#from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#svcMgr.EventSelector.SkipEvents = 495

