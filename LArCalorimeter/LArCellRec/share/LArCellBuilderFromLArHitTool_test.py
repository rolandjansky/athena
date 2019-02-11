#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# File: LArCellRec/share/LArCellBuilderFromLArHitTool_test.py
# Author: sss
# Date: Aug, 2018
# Brief: Unit test for LArCellBuilderFromLArHitTool.
#


import ROOT
ROOT.TH1F

from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.LAr_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.isOnline.set_Value(True)

GeV = 1000

import sys
import string
import ROOT
import math
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

from McEventSelector import McEventSelectorConf
svcMgr+=McEventSelectorConf.McEventSelector('EventSelector',
                                             RunNumber=999999,
                                             EventsPerLB=3,
                                             TimeStampInterval=1)

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]
from AtlasGeoModel import LArGM

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc().GlobalTag = 'OFLCOND-RUN12-SDR-35'

include('LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py')

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

theApp.EvtMax=5

import ROOT
from LArCellRec.LArCellDeadOTXCorrToolDefault import LArCellDeadOTXCorrToolDefault

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

###########################################################################


class CaloCell_ID:
    LAREM = 0
    LARHEC = 1
    LARFCAL = 2
    

# barec, samp, reg, ieta, iphi, e, time, exp0
hits = [
    ['LArHitEMB', CaloCell_ID.LAREM,
     [[( 1, 1, 0, 10,  23), 10.0*GeV, 123, ( 10899.3018,  10888.7510,        None,        None)],
      [(-1, 2, 1,  0, 123),  0.1*GeV,  31, (    93.1610,    103.9979,     93.1610,     84.48318)],
      [( 1, 3, 0, 12,  41),   99*GeV,  12, (107903.0859, 107953.2734,        None,        None)],
      [(-1, 0, 0, 54,   9),    2*GeV,  14, (  8000.0,      7972.3979,        None,        None)],
      ]],
    ['LArHitEMEC', CaloCell_ID.LAREM,
     [[( 2, 0, 0,  3,  24),    5*GeV,  16, ( 59998.8047,  59919.6758,  59998.8047,  60117.71875)],
      [(-2, 1, 2, 42,  27),   50*GeV,  24, (105214.4219, 105278.6797, 105214.4219, 105309.0859)],
      [( 2, 2, 1, 17,  37),  100*GeV,  27, (201278.1094, 201371.75,          None,        None)],
      [(-2, 3, 0, 12,  42),   75*GeV,  46, (144681.5156, 144700.2813,        None,        None)],
      [( 3, 1, 0,  3,  21),   25*GeV,  53, ( 60467.5352,  60539.375,         None,        None)],
      [(-3, 2, 0,  5,  13),  125*GeV,  44, (273080.7813, 273192.8125,        None,        None)]
      ]],
    ['LArHitHEC', CaloCell_ID.LARHEC,
     [[( 2, 0, 0,  3,  54),   23*GeV,  54, (102908.2734, 103014.1016,        None,        None)],
      [(-2, 1, 1,  1,  15),  157*GeV,  87, (702460.8125, 702207.5625, 702460.8125, 700990.4375)],
      [( 2, 2, 0,  6,  37),   87*GeV,  65, (778523.5,    778360.3125,        None,        None)],
      [(-2, 3, 1,  2,  14),   97*GeV,  63, (868008.9375, 867810.0625, 868008.9375, 868528.9375)],
      ]],
    ['LArHitFCAL', CaloCell_ID.LARFCAL,
     [[( 2, 1, 0, 33,  13),   34*GeV,  51, (421496.34375, 421274.5938,       None,        None)],
      [(-2, 2, 0, 16,  11),   26*GeV,  45, (439932.34375, 440415.5625,       None,        None)],
      [( 2, 3, 0,  9,   3),   21*GeV,  41, (288026.34375, 286402.875,        None,        None)],
      ]],
    ]


from AthenaPython.PyAthenaComps import Alg, StatusCode
class Tools:
    def __init__ (self, name):
        self.em = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('LArCellBuilderFromLArHitTool/' + name + 'em')
        self.hec = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('LArCellBuilderFromLArHitTool/' + name + 'hec')
        self.fcal = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('LArCellBuilderFromLArHitTool/' + name + 'fcal')
        return


    def initialize (self):
        if not self.em.retrieve():
            return StatusCode.Failure
        if not self.hec.retrieve():
            return StatusCode.Failure
        if not self.fcal.retrieve():
            return StatusCode.Failure
        return StatusCode.Success


    def process (self, ccc):
        if not self.em.process (ccc):
            return StatusCode.Failure
        if not self.hec.process (ccc):
            return StatusCode.Failure
        if not self.fcal.process (ccc):
            return StatusCode.Failure
        return StatusCode.Success


class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return

    def initialize (self):
        ROOT.ICaloCellMakerTool
        print 'mgr', self.detStore['CaloMgr']
        self.tools1 = Tools ('tool1')
        self.tools2 = Tools ('tool2')
        self.tools3 = Tools ('tool3')
        self.tools4 = Tools ('tool4')
        self.tools5 = Tools ('tool5')

        if not self.tools1.initialize():
            return StatusCode.Failure
        if not self.tools2.initialize():
            return StatusCode.Failure
        if not self.tools3.initialize():
            return StatusCode.Failure
        if not self.tools4.initialize():
            return StatusCode.Failure
        if not self.tools5.initialize():
            return StatusCode.Failure

        self.idhelper  = self.detStore['CaloCell_ID']
        return StatusCode.Success


    def execute (self):
        iev = self.getContext().evt()

        self.record_hits()

        exp_n_noise = 0
        if iev == 0:
            # Event 0: no noise, no windowing, no internal map.
            tools = self.tools1
            def exp_e_fn (exp): return exp[0]

        elif iev == 1:
            # Event 1: no noise, no windowing, internal map (+thresh)
            tools = self.tools2
            def exp_e_fn (exp):
                if exp[0] < 1*GeV: return None
                return exp[0]

        elif iev == 2:
            # Event 2: noise, no windowing, internal map
            tools = self.tools3
            exp_n_noise = 182451
            def exp_e_fn (exp): return exp[1]

        elif iev == 3:
            # Event 3: no noise, windowing
            self.record_mc()
            tools = self.tools4
            def exp_e_fn (exp): return exp[2]

        else:
            # Event 4: noise + windowing
            self.record_mc()
            tools = self.tools5
            exp_n_noise = 2210
            def exp_e_fn (exp): return exp[3]

        exp_cells = {}
        for name, subcal, l in hits:
            c = ROOT.LArHitContainer (name)
            for addr, e, t, exp in l:
                fulladdr = (subcal,) + addr
                exp_e = exp_e_fn(exp)
                if exp_e == None: continue
                exp_cells[fulladdr] = [exp_e, t]
        ccc = ROOT.CaloCellContainer()
        if not tools.process (ccc):
            return StatusCode.Failure

        self.check_ccc (ccc, exp_cells, exp_n_noise)

        #self.print_cells()

        return StatusCode.Success


    def region (self, cid):
        if self.idhelper.sub_calo(cid) == ROOT.CaloCell_ID.LARFCAL:
            return 0
        return self.idhelper.region(cid)


    def check_ccc (self, ccc, exp_cells, exp_n_noise):
        exp_cells = exp_cells.copy()
        n_noise = 0

        for c in ccc:
            cid = c.ID()
            addr = (self.idhelper.sub_calo(cid),
                    self.idhelper.pos_neg(cid),
                    self.idhelper.sampling(cid),
                    self.region(cid),
                    self.idhelper.eta(cid),
                    self.idhelper.phi(cid))
            l = exp_cells.get (addr)
            if l:
                if abs (c.e() - l[0]) > 1e-3:
                    print 'xxx energy mismatch: ', addr, c.e(), l[0]
                    assert 0
                if abs (c.time() - l[1]) > 1e-3:
                    print 'xxx time mismatch', addr, c.time(), l[1]
                    assert 0
                del exp_cells[addr]
            elif c.e() < 2*GeV:
                n_noise += 1
            else:
                print 'xxx unexpected cell', addr, c.e(), c.time()
                assert 0

        if n_noise != exp_n_noise:
            print 'xxx n_noise', n_noise, exp_n_noise
            assert 0
        for extra in exp_cells:
            print 'xxx unfound cell', extra
            assert 0
        return


    def record_hits (self):
        samp = 0.2 # dummy sampling factor
        for name, subcal, l in hits:
            c = ROOT.LArHitContainer (name)
            for addr, e, t, exp in l:
                sys.stdout.flush()
                cellid = self.idhelper.cell_id (subcal,
                                                addr[0],
                                                addr[1],
                                                addr[2],
                                                addr[3],
                                                addr[4])
                h = ROOT.LArHit (cellid, e * samp, t)
                h.finalize()
                c.push_back (h)
            self.evtStore.record (c, name, False)
        return


    def add_particle (self, vtx, pdgid, pt, eta, phi, status=2000):
        e = pt * math.cosh(eta)
        pz= pt * math.sinh(eta)
        v = ROOT.HepMC.FourVector (pt*math.cos(phi), pt*math.sin(phi), pz, e)
        p = ROOT.HepMC.GenParticle (v, pdgid, status)
        vtx.add_particle_out (p)
        return


    def record_mc (self):
        vtx = ROOT.HepMC.GenVertex()
        self.add_particle (vtx,  22, 10*GeV, -1.5, 2.8)
        self.add_particle (vtx,  11, 15*GeV, -2.9, 2.9)
        self.add_particle (vtx, -11, 20*GeV,  1.5, 2.5)

        self.add_particle (vtx,  21, 30*GeV,  2.2, -2.6)
        self.add_particle (vtx,  22,  1*GeV,  0.1,  2.4)
        self.add_particle (vtx,  22, 40*GeV,  3.6,  1.5, 100)

        evt = ROOT.HepMC.GenEvent()
        evt.add_vertex (vtx)
        mccoll = ROOT.McEventCollection()
        mccoll.push_back (evt)
        self.evtStore.record (mccoll, 'GEN_EVENT', False)
        return


    def print_cells (self):
        mgr = self.detStore['CaloMgr']
        for name, subcal, l in hits:
            for addr, e, t, exp in l:
                cellid = self.idhelper.cell_id (subcal,
                                                addr[0],
                                                addr[1],
                                                addr[2],
                                                addr[3],
                                                addr[4])
                dde = mgr.get_element (cellid)
                print 'cell', subcal, addr, dde.eta(), dde.phi()
        return
        


from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
noisetool = CaloNoiseToolDefault('tool')
noisetool.UseTile = False
ToolSvc += noisetool

from LArCellRec.LArCellRecConf import LArCellBuilderFromLArHitTool

def make_tools (name, WithNoise = False, **kw):
    noisetool = 'CaloNoiseTool/CaloNoiseToolDefault'
    tool_em = LArCellBuilderFromLArHitTool (name + 'em',
                                            LArRegion = 'LAr_EM',
                                            NoiseTool = noisetool,
                                            BeginRunPriority = 100,
                                            WithNoise = WithNoise,
                                            **kw)


    tool_hec = LArCellBuilderFromLArHitTool (name + 'hec',
                                             LArRegion = 'LAr_HEC',
                                             NoiseTool = noisetool,
                                             BeginRunPriority = 100,
                                             WithNoise = WithNoise,
                                             **kw)

    tool_fcal = LArCellBuilderFromLArHitTool (name + 'fcal',
                                              LArRegion = 'LAr_FCal',
                                              NoiseTool = noisetool,
                                              BeginRunPriority = 100,
                                              WithNoise = WithNoise,
                                              **kw)
    global ToolSvc
    ToolSvc += tool_em
    ToolSvc += tool_hec
    ToolSvc += tool_fcal
    return


make_tools ('tool1')
make_tools ('tool2', ThresholdOnE = 1*GeV)
make_tools ('tool3', WithNoise = True)
make_tools ('tool4', Windows = True, ThresholdOnE = 0*GeV)
make_tools ('tool5', Windows = True, WithNoise = True)

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg()

testalg1 = TestAlg ('testalg1')
topSequence += testalg1
