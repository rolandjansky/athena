#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# File: FastCaloSim/share/FastShowerCellBuilderTool_test.py
# Author: scott snyder
# Date: Nov, 2018
# Brief: Test for FastShowerCellBuilderTool.
#

from __future__ import print_function


import ROOT
ROOT.TH1F

from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.Tile_setOn()
DetFlags.detdescr.LAr_setOn()

RunNumber = 284500

import sys
import string
import ROOT
import math
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry
include('TileConditions/TileConditions_jobOptions.py')

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]
from AtlasGeoModel import TileGM
from AtlasGeoModel import LArGM   #LAr needed to get MBTS DD.

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc().GlobalTag = 'OFLCOND-RUN12-SDR-35'

import MagFieldServices.SetupField

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

theApp.EvtMax=1

exp_cells = {
    (0, -3,  1,  0,  2,  29) :  1082.3,
    (0, -3,  1,  0,  2,  30) :   593.1,
    (0, -3,  1,  0,  3,  28) :  2893.1,
    (0, -3,  1,  0,  3,  29) : 85573.6,
    (0, -3,  1,  0,  3,  30) : 19897.8,
    (0, -3,  1,  0,  4,  28) :  1775.3,
    (0, -3,  1,  0,  4,  29) : 13901.8,
    (0, -3,  1,  0,  4,  30) :  4405.1,
    (0, -3,  1,  0,  5,  29) :   650.2,
    (0, -3,  1,  0,  5,  30) :   508.4,
    (0, -3,  2,  0,  3,  29) :   557.1,
    (0, -2,  0,  0,  0,  28) :   765.0,
    (0, -2,  1,  1,  2,  28) :  5965.7,
    (0, -2,  2,  1,  2, 113) :  1617.0,
    (0, -2,  2,  1,  2, 114) :  2100.8,
    (0, -1,  0,  0, 59,  28) :   871.8,
    (0, -1,  0,  0, 60,  28) :  1184.4,
    (0,  1,  0,  0, 59,  25) :   282.9,
    (0,  1,  0,  0, 60,  25) :   208.4,
    (0,  2,  0,  0,  0,  24) :   567.1,
    (0,  2,  0,  0,  0,  25) :  4319.5,
    (0,  2,  0,  0,  1,  25) :   810.2,
    (0,  2,  1,  1,  1,  25) :   745.2,
    (0,  2,  1,  1,  2,  25) : 13066.6,
    (0,  2,  1,  4, 28,  37) :   540.0,
    (0,  2,  1,  4, 29,  37) :   910.3,
    (0,  2,  1,  4, 30,  37) :   788.2,
    (0,  2,  1,  4, 31,  37) :   610.1,
    (0,  2,  1,  4, 32,  37) :   266.0,
    (0,  2,  2,  1,  1, 101) :   324.4,
    (0,  2,  2,  1,  2, 100) :  1023.7,
    (0,  2,  2,  1,  2, 101) :  6262.3,
    (0,  2,  2,  1,  2, 102) :   636.3,
    (0,  2,  2,  1,  3, 101) :   428.5,
    (0,  2,  2,  1, 29, 149) :   683.1,
    (0,  2,  2,  1, 29, 150) :  1136.9,
    (0,  2,  2,  1, 29, 151) :   265.4,
    (0,  2,  2,  1, 30, 148) :   410.7,
    (0,  2,  2,  1, 30, 149) :  6159.1,
    (0,  2,  2,  1, 30, 150) :  9094.0,
    (0,  2,  2,  1, 30, 151) :   757.1,
    (0,  2,  2,  1, 31, 148) :   248.7,
    (0,  2,  2,  1, 31, 149) :  1078.7,
    (0,  2,  2,  1, 31, 150) :  1348.1,
    (0,  2,  2,  1, 31, 151) :   396.7,
    (0,  2,  2,  1, 32, 150) :   165.0,
    (0,  2,  3,  0, 13, 149) :  6118.8,
    (0,  2,  3,  0, 13, 150) : 12741.9,
    (0,  2,  3,  0, 13, 151) :  1333.6,
    (0,  2,  3,  0, 14, 149) :   853.6,
    (0,  2,  3,  0, 14, 150) :  1421.6,
    (0,  2,  3,  0, 14, 151) :   617.1,
    (1,  2,  0,  0,  6,  36) :   191.8,
    (1,  2,  0,  0,  6,  37) :  2026.1,
    (1,  2,  0,  0,  6,  38) :   103.9,
    (1,  2,  0,  0,  7,  37) :   432.4,
    (1,  2,  1,  0,  6,  36) :  1347.2,
    (1,  2,  1,  0,  6,  37) : 13680.3,
    (1,  2,  1,  0,  6,  38) :  1055.9,
    (1,  2,  1,  0,  7,  36) :   568.6,
    (1,  2,  1,  0,  7,  37) :  9250.3,
    (1,  2,  1,  0,  7,  38) :   589.0,
    (1,  2,  2,  0,  6,  37) :  1702.3,
    (1,  2,  2,  0,  7,  37) :  1321.2,
    (3,  3, -1, 28, 15,   3) :  2566.1,
    (3,  3,  1, 24, 15,   3) :   690.3,
    (3,  3,  1, 25, 15,   3) :  9414.4,
    }


###########################################################

from AthenaPython.PyAthenaComps import Alg, StatusCode


def make_calo_cells (detStore):
    mgr = detStore['CaloMgr']
    ccc = ROOT.CaloCellContainer()
    for i in range (mgr.element_size()):
        elt = mgr.get_element (ROOT.IdentifierHash (i))
        if not elt: break
        cc = ROOT.CaloCell (elt, 0, 0, 0, 0)
        ccc.push_back (cc)
        ROOT.SetOwnership (cc, False)
    ccc.order()
    ccc.updateCaloIterators()
    return ccc


class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return

    def initialize (self):
        ROOT.ICaloCellMakerTool

        def gettool (name):
            tool = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('FastShowerCellBuilderTool/' + name)
            if not tool.retrieve():
                assert 0
            return tool

        self.tool1 = gettool ('tool1')

        self.idhelper = self.detStore['CaloCell_ID']

        return StatusCode.Success


    def execute (self):
        self.record_mc()

        ccc = make_calo_cells (self.detStore)
        if not self.tool1.process (ccc):
            return StatusCode.Failure

        self.compare_cells (ccc, exp_cells)

        return StatusCode.Success


    def add_particle (self, vtx, pdgid, pt, eta, phi, status=2002):
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

        self.add_particle (vtx, 211, 30*GeV,  2.2, -2.6)
        self.add_particle (vtx,  22,  1*GeV,  0.1,  2.4)
        self.add_particle (vtx,  22, 40*GeV,  3.6,  1.5, 100)

        evt = ROOT.HepMC.GenEvent()
        evt.add_vertex (vtx)
        mccoll = ROOT.McEventCollection()
        mccoll.push_back (evt)
        self.evtStore.record (mccoll, 'TruthEvent', False)
        return


    def region (self, cid):
        if self.idhelper.sub_calo(cid) == ROOT.CaloCell_ID.LARFCAL:
            return 0
        return self.idhelper.region(cid)


    def compare_cells (self, ccc, exp_cells):
        exp_cells = exp_cells.copy()
        idhelper  = self.idhelper
        tilehelper = idhelper.tile_idHelper()

        for c in ccc:
            lcell = [c.energy(), c.time(),
                     c.quality(), c.provenance(),
                     c.gain()]

            cid = c.ID()
            sub_calo = idhelper.sub_calo(cid)
            if sub_calo == 3:
                addr = (sub_calo,
                        tilehelper.section(cid),
                        tilehelper.side(cid),
                        tilehelper.module(cid),
                        tilehelper.tower(cid),
                        tilehelper.sampling(cid))
            else:
                addr = (sub_calo,
                        idhelper.pos_neg(cid),
                        idhelper.sampling(cid),
                        self.region(cid),
                        idhelper.eta(cid),
                        idhelper.phi(cid))

            l = exp_cells.get (addr)
            if not l:
                if abs(lcell[0]) > 500:
                    print ('xxx unexpected cell', addr, lcell)
                    assert 0
                continue

            if (abs ((lcell[0] - l)/l) > 1e-3 or
                abs (lcell[1]) > 1e-3 or
                lcell[2] != 0 or
                lcell[3] != 0 or
                lcell[4] != 0):
                print ('xxx cell mismatch: ', addr, lcell, l)
                assert 0
            del exp_cells[addr]

        for extra in exp_cells:
            print ('xxx unfound cell', extra)
            assert 0
        return


#########################################################################


from FastCaloSim.FastCaloSimFactory import FastCaloSimFactory

def maketool (name, **kw):
    return FastCaloSimFactory (name, **kw)
ToolSvc += maketool ('tool1')

testalg1 = TestAlg ('testalg1')
topSequence += testalg1


# Suppress useless GeoModelSvc messages.
from AthenaCommon import Constants
GeoModelSvc().OutputLevel=Constants.WARNING
