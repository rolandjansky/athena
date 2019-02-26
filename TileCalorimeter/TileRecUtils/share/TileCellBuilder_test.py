#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: TileRecUtils/share/TileCellBuilder_test.py
# Author: sss
# Date: Aug, 2018
# Brief: Test for TileCellBuilder.
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

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

theApp.EvtMax=12



hits_0 = [
    # (sec, side, mod, tow, samp, pmt, adc), [adc, time, qual, ped]
    [0, [
        [(1, 1, 0, 1, 1, 0, 1), [ 0.2,  22.1,  2.4, 42.9]],
        [(1, 1, 0, 1, 1, 1, 1), [-0.4,  75.0,  2.7, 31.4]],
        [(1, 1, 0, 1, 0, 0, 1), [-0.9, -13.3,  1.5, 37.4]],
        [(1, 1, 0, 2, 0, 1, 1), [41.8,   0.1,  2.5, 45.7]],
        [(1, 1, 0, 2, 0, 0, 1), [30.6,  -0.4,  2.0, 33.1]],
        [(1, 1, 0, 2, 1, 1, 1), [ 1.4,  12.9,  4.7, 43.1]],
        [(1, 1, 0, 2, 1, 0, 1), [ 3.4,   3.5,  1.9, 54.2]],
    ]],

    [136, [
        [(3, 1, 8, 13, 3, 0, 1),  [26.9, 4.8, 5.2, 45.4]],
    ]],

    [146, [
        [(3, 1, 18, 13, 3, 0, 1), [ 4.4, -26.5, 23.9, 51.2]],
        [(3, 1, 18, 15, 3, 0, 1), [-0.1, -75.0,  3.7, 51.0]],
        [(3, 1, 18,  8, 2, 0, 1), [-0.1,  75.0,  4.2, 41.7]],
        [(3, 1, 18,  8, 2, 1, 1), [ 3.5, -11.9,  2.6, 44.7]],
        [(3, 1, 18,  9, 1, 0, 1), [ 0.8, -17.6,  1.8, 49.2]],
        [(3, 1, 18,  9, 1, 1, 1), [ 0.1, -75.0,  3.4, 48.4]],
        [(2, 1, 18, 11, 0, 0, 1), [ 8.7,   0.4,  2.7, 50.2]],
        [(2, 1, 18, 11, 0, 1, 1), [ 6.3,   1.9,  1.6, 48.6]],
        [(2, 1, 18, 10, 1, 0, 1), [53.3,   1.3,  3.8, 38.4]],
        [(2, 1, 18, 10, 1, 1, 1), [64.1,   1.0,  3.5, 36.9]],
        [(2, 1, 18, 12, 0, 0, 1), [-0.2, -75.0,  2.8, 38.1]],
        [(2, 1, 18, 12, 0, 1, 1), [ 0.5, -32.7,  1.9, 40.3]],
        [(3, 1, 18, 10, 3, 0, 1), [ 5.4,   7.7,  8.2, 57.0]],
        [(3, 1, 18, 11, 3, 0, 1), [48.9,   0.4,  4.0, 32.3]],
        [(2, 1, 18, 11, 1, 0, 1), [38.5,   0.5,  1.7, 46.8]],
        [(2, 1, 18, 11, 1, 1, 1), [48.1,   0.3,  1.6, 46.3]],
        [(2, 1, 18, 10, 2, 0, 1), [ 0.3,  75.0,  2.0, 34.9]],
        [(2, 1, 18, 10, 2, 1, 1), [0.1,  -75.0,  3.7, 54.7]],
    ]],

    # Some MBTS cells.
    [135, [ [(4,  1, 0, 1, 0, 0, 0), [ 59.1, 2.8, 6.9, 56.2]] ]],
    [181, [ [(4,  1, 6, 1, 0, 0, 0), [ 56.4, 1.2, 2.9, 39.9]] ]],
    [182, [ [(4,  1, 6, 0, 0, 0, 0), [ 44.9, 0.5, 1.4, 42.1]] ]],
    [231, [ [(4, -1, 3, 0, 0, 0, 0), [107.9, 1.7, 7.8, 47.8]] ]],

    # Some E4 cells.
    [220, [ [(4, -1, 0, 2, 0, 0, 0), [  2.3,   3.2, 2.5, 44.4]] ]],
    [223, [ [(4, -1, 1, 2, 0, 0, 0), [  3.1,  -5.3, 2.4, 44.4]] ]],
    [225, [ [(4, -1, 2, 2, 0, 0, 0), [ -0.4,  27.6, 1.3, 32.5]] ]],
    [228, [ [(4, -1, 3, 2, 0, 0, 0), [  0.2, -75.0, 2.4, 35.4]] ]],

    # Some disconnected cells.
    [253, [
        [4706026383833300992, [ 0, 0, 0, 50000]],
        [4706027483344928768, [ 0, 0, 0, 50000]] ], ],
    [255, [
        [4706157225717006336, [ 0, 0, 0, 80000]],
        [4706158325228634112, [ 0, 0, 0, 80000]] ], ],
]


# Add an offset to one pedestal value.
def add_pedestal (offs_addr, offs):
    out = []
    for icoll, colldata in hits_0:
        colldata_out = []
        for addr, data in colldata:
            if addr == offs_addr:
                data = data[:]
                data[3] += offs
            colldata_out.append ([addr, data])
        out.append ([icoll, colldata_out])
    return out


exp_cells_0 = {
    (1, 1, 0, 1, 0) : [-10.4734, -10.4734, -13.3, -13.3, 1, 1, 130, 138, 1, 1],
    (1, 1, 0, 1, 1) : [  2.3274,  -4.6548,  22.1,  75.0, 2, 2, 130, 130, 1, 1],
    (1, 1, 0, 2, 0) : [356.0956, 486.4312,  -0.4,   0.1, 2, 2, 162, 162, 1, 1],
    (1, 1, 0, 2, 1) : [ 39.5662,  16.2920,   3.5,  12.9, 1, 4, 162, 130, 1, 1],

    (2, 1, 18, 10, 1):[620.25793, 745.93872,  1.3,   1.0, 3, 3, 162, 162, 1, 1],
    (2, 1, 18, 10, 2):[  3.49113,   1.16371, 75.0, -75.0, 2, 3, 130, 130, 1, 1],
    (2, 1, 18, 11, 0):[101.24284,  73.31380,  0.4,   1.9, 2, 1, 162, 162, 1, 1],
    (2, 1, 18, 11, 1):[448.02869, 559.74500,  0.5,   0.3, 1, 1, 162, 162, 1, 1],
    (2, 1, 18, 12, 0):[ -2.32742,   5.81855,-75.0, -32.7, 2, 1, 130, 130, 1, 1],
    (3, 1, 18,  8, 2):[ -0.96976,  33.94157, 75.0, -11.9, 4, 2, 130, 130, 1, 1],
    (3, 1, 18,  9, 1):[  7.75807,   0.96976,-17.6, -75.0, 1, 3, 130, 130, 1, 1],
    (3, 1, 18, 10, 3):[ 41.89359,   0.0,      7.7,   7.7, 8, 0, 162,   0, 1,-1],
    (3, 1, 18, 11, 3):[379.36975,   0.0,      0.4,   0.4, 4, 0, 162,   0, 1,-1],
    (3, 1, 18, 13, 3):[ 34.13552,   0.0,    -26.5, -26.5,23, 0, 130,   0, 1,-1],
    (3, 1, 18, 15, 3):[ -0.77581,   0.0,    -75.0, -75.0, 3, 0, 130,   0, 1, -1],

    # coll 136
    (3, 1, 8, 13, 3) : [208.6922, 0.0, 4.8, 4.8, 5, 0, 162, 0, 1, -1],
}


exp_mbts_0 = {
    (4,  1, 0, 1, 0) : [0.7221, 0.0, 2.8, 2.8, 6, 0, 162, 0, 1, -1],
    (4,  1, 6, 1, 0) : [0.6891, 0.0, 1.2, 1.2, 2, 0, 162, 0, 1, -1],
    (4,  1, 6, 0, 0) : [0.5486, 0.0, 0.5, 0.5, 1, 0, 162, 0, 1, -1],
    (4, -1, 3, 0, 0) : [1.318,  0.0, 1.7, 1.7, 7, 0, 162, 0, 1, -1],
}


exp_e4_0 = {
    (4, -1, 0, 2, 0) : [17.84357, 0.0,   3.2,   3.2, 2, 0, 130, 0, 1, -1],
    (4, -1, 1, 2, 0) : [24.05003, 0.0,  -5.3,  -5.3, 2, 0, 130, 0, 1, -1],
    (4, -1, 2, 2, 0) : [-3.10323, 0.0,  27.6,  27.6, 1, 0, 130, 0, 1, -1],
    (4, -1, 3, 2, 0) : [1.551615, 0.0, -75.0, -75.0, 2, 0, 130, 0, 1, -1],
}


def exp_merge (base, d):
    new = base.copy()
    new.update (d)
    return new


# Underflow.
hits_1 = add_pedestal ((3, 1, 18, 15, 3, 0, 1), 10000)
exp_cells_1 = exp_merge (exp_cells_0, {
    (3, 1, 18, 15, 3) : [-0.7758, 0.0, -75.0, -75.0, 3, 0, 146, 0, 1, -1],
})

# Overflow.
hits_2 = add_pedestal ((3, 1, 18, 15, 3, 0, 1), 20000)
exp_cells_2 = exp_merge (exp_cells_0, {
    (3, 1, 18, 15, 3) : [-0.7758, 0.0, -75.0, -75.0, 3, 0, 146, 0, 1, -1],
})

# TileBadChan errors.
exp_cells_3 = exp_merge (exp_cells_0, {
    (3, 1, 18,  8, 2) : [ 33.9416, 33.9416, -11.9, -11.9, 2, 2, 138, 130, 1, 1],
    (2, 1, 18, 11, 0) : [101.2428, 73.3138,   1.9,   1.9, 2, 1,  34, 162, 1, 1],
    (2, 1, 18, 12, 0) : [ -2.3274, -2.3274, -75.0, -75.0, 2, 2, 130, 138, 1, 1],
})
exp_mbts_3 = exp_merge (exp_mbts_0, {
    (4, 1,  0,  1, 0) : [  0.0,     0.0,   -100.0,-100.0,255,0,  10,   8, 1,-1],
    (4, 1,  6,  1, 0) : [  0.6891,  0.0,      0.0,   0.0, 2, 0,  34,   0, 1,-1],
})

# TileDQstatus errors
exp_cells_4 = exp_merge (exp_cells_0, {
    (2, 1, 18, 10, 1):[620.2579,620.2579,  1.3,  1.3,   3,   3, 161, 169, 1, 1],
    (2, 1, 18, 12, 0):[  0.5,     0.5,     0.0,  0.0, 255, 255,   9,   9, 1, 1],
    (3, 1, 18,  8, 2):[ 33.9416, 33.9416,-11.9,-11.9,   2,   2, 137, 129, 1, 1],
    (3, 1, 18, 13, 3):[  0.5,     0.0,     0.0,  0.0, 255,   0,   9,   8, 1,-1],
    (3, 1, 18, 15, 3):[  0.5,     0.0,     0.0,  0.0, 255,   0,   9,   8, 1,-1],
})


# fakeCrackCells
exp_cells_5 = exp_merge (exp_cells_0, {
    (3, 1, 8,  8, 2) : [0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0,  0],
    (3, 1, 8,  9, 1) : [0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0,  0],
    (3, 1, 8, 10, 3) : [0.0, 0.0, 0.0, 0.0, 0, 0, 8, 8, 0, -1],
    (3, 1, 8, 11, 3) : [0.0, 0.0, 0.0, 0.0, 0, 0, 8, 8, 0, -1],
    (3, 1, 8, 15, 3) : [0.0, 0.0, 0.0, 0.0, 0, 0, 8, 8, 0, -1],
})


def filter_e (base, thresh):
    out = {}
    for addr, l in base.items():
        if l[0] >= thresh or l[1] >= thresh:
            out[addr] = l
    return out

# EThreshold
exp_cells_6 = filter_e (exp_cells_0, 300)



# corrections
exp_cells_7 = exp_merge (exp_cells_0, {
    (1, 1,  0,  2, 0) : [356.1491, 486.4361, -0.4, 0.1, 2, 2, 162, 162, 1, 1],
    (2, 1, 18, 10, 1) : [621.3283, 746.7004,  1.3, 1.0, 3, 3, 162, 162, 1, 1],
    (2, 1, 18, 11, 1) : [448.1431, 559.7963,  0.5, 0.3, 1, 1, 162, 162, 1, 1],
    (3, 1,  8, 13, 3) : [213.6019,   0.0,     4.8, 4.8, 5, 0, 162,   0, 1,-1],
    (3, 1, 18, 11, 3) : [379.4317,   0.0,     0.4, 0.4, 4, 0, 162,   0, 1,-1],
})
exp_mbts_7 = exp_merge (exp_mbts_0, {
    (4,  1, 0, 1, 0) : [0.7279, 0.0, 2.8, 2.8, 6, 0, 162, 0, 1, -1],
    (4,  1, 6, 1, 0) : [0.6902, 0.0, 1.2, 1.2, 2, 0, 162, 0, 1, -1],
    (4, -1, 3, 0, 0) : [1.3223, 0.0, 1.7, 1.7, 7, 0, 162, 0, 1, -1],
})


# noisefilter
exp_cells_8 = exp_merge (exp_cells_0, {
    (1, 1, 0, 1, 0) : [-11.3462, -11.3462, -13.3, -13.3, 1, 1, 130, 138, 1, 1],
    (1, 1, 0, 1, 1) : [  1.4546,  -5.5276,  22.1,  75.0, 2, 2, 130, 130, 1, 1],
    (1, 1, 0, 2, 0) : [355.2228, 485.5583,  -0.4,   0.1, 2, 2, 162, 162, 1, 1],
    (1, 1, 0, 2, 1) : [ 39.5662,  15.4192,   3.5,  12.9, 1, 4, 162, 130, 1, 1],
})


# DSP w/corrrections
exp_cells_10 = exp_merge (exp_cells_7, {
    (1, 1, 0, 2, 0) : [356.1227, 486.4159, -0.4, 0.1, 2, 2, 162, 162, 1, 1],
})



# TileFragHash::TYPE
class TileFragHash:
    Beam = 255
    Default = 0
    Digitizer = 0
    OptFilterDsp = 1
    OptFilterOffline = 2
    OptFilterDspCompressed = 3
    ManyAmps = 4
    MF = 5
    FitFilter = 6
    FitFilterCool = 7
    FlatFilter = 8


from AthenaPython.PyAthenaComps import Alg, StatusCode


###########################################################

class PrepareDataAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return
    
    def initialize (self):
        return StatusCode.Success

    def execute (self):
        iev = self.getContext().evt()

        hits = hits_0
        self.exp_cells = exp_cells_0
        self.exp_mbts = exp_mbts_0
        self.extra_ei_flags = 0
        self.tool = 'tool1'
        baddq = {}
        bsflags = 0x32002000
        dspbsflags = 0x32002000
        self.rctype = TileFragHash.OptFilterOffline
        dspcolls = set()
        dqhits = {}

        if iev == 0:
            # Event 0: nominal
            pass

        elif iev == 1:
            # Event 1: Add an underflow.
            hits = hits_1
            self.extra_ei_flags = 0x40
            self.exp_cells = exp_cells_1

        elif iev == 2:
            # Event 2: Add an overflow.
            hits = hits_2
            self.extra_ei_flags = 0x400
            self.exp_cells = exp_cells_2

        elif iev == 3:
            # Event 3: Bad channels
            self.tool = 'tool2'
            self.exp_cells = exp_cells_3
            self.exp_mbts = exp_mbts_3

        elif iev == 4:
            # Event 4: Bad channels from TileDQstatus
            self.rctype = TileFragHash.OptFilterDsp
            baddq = {146 : [2, 10]}
            self.exp_cells = exp_cells_4
            dqhits = hits

        elif iev == 5:
            # Event 5: fakeCrackCells
            self.tool = 'tool5'
            self.exp_cells = exp_cells_5

        elif iev == 6:
            # Event 6: Threshold.
            self.tool = 'tool6'
            self.exp_cells = exp_cells_6

        elif iev == 7:
            # Event 7: Corrections
            self.tool = 'tool7'
            bsflags = 0x32000000
            self.exp_cells = exp_cells_7
            self.exp_mbts = exp_mbts_7

        elif iev == 8:
            # Event 8: noise filter
            self.tool = 'tool8'
            self.exp_cells = exp_cells_8

        elif iev == 9:
            # Event 9: dsp container
            self.tool = 'tool9'
            dspcolls.add (0)

        elif iev == 10:
            # Event 10: dsp container + corrections
            self.tool = 'tool10'
            dspcolls.add (0)
            bsflags = 0x32000000
            dspbsflags = 0x20000000
            self.exp_cells = exp_cells_10
            self.exp_mbts = exp_mbts_7

        else:
            # Event 11: dsp container + noise filter
            self.tool = 'tool11'
            dspcolls.add (0)
            self.exp_cells = exp_cells_8


        self.record_raw_data (hits, self.rctype, baddq, bsflags, dspbsflags, dspcolls)

        self.record_raw_data (dqhits, self.rctype, baddq, bsflags,
                              rawname = 'TRCDQ')

        return StatusCode.Success


    def record_raw_data (self, hits, typ, baddq, bsflags,
                         dspbsflags=0, dspcolls=set(),
                         rawname = 'TileRawChannelCnt'):
        idHelper  = self.detStore['CaloCell_ID'].tile_idHelper()

        unit = 0 # TileRawChannelUnit::ADCcounts
        cont = ROOT.TileRawChannelContainer (False, typ, unit)
        cont.set_bsflags (bsflags)
        hashFunc = cont.hashFunc()

        dspcont = None
        if dspcolls:
            dspcont = ROOT.TileRawChannelContainer (False, typ, unit)
            dspcont.set_bsflags (dspbsflags)

        for icoll, colldata in hits:
            coll = ROOT.TileRawChannelCollection (hashFunc.identifier(icoll))

            mask = 0
            for chan in baddq.get (icoll, []):
                mask |= (1<<(chan/3))
            coll.setFragMemoryPar(mask)

            for addr, data in colldata:
                if type(addr) == type(()):
                    adc_id = idHelper.adc_id (*addr)
                    chan = ROOT.TileRawChannel (adc_id, *data)
                else:
                    hwid = ROOT.HWIdentifier (addr)
                    chan = ROOT.TileRawChannel (hwid, *data)
                coll.push_back (chan)

            if icoll in dspcolls:
                thiscont = dspcont
            else:
                thiscont = cont
            thiscont.addCollection (coll, ROOT.IdentifierHash(icoll))
            ROOT.SetOwnership (coll, False)

        self.evtStore.record (cont, rawname, False)
        if dspcont:
            self.evtStore.record (dspcont, 'TileRawChannelCntDsp', False)
        return


###########################################################


class TestAlg (Alg):
    def __init__ (self, name, prepAlg):
        Alg.__init__ (self, name)
        self.prepAlg = prepAlg
        return

    def initialize (self):
        ROOT.ICaloCellMakerTool

        def gettool (name):
            tool = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('TileCellBuilder/' + name)
            if not tool.retrieve():
                assert 0
            return tool

        self.tool1 = gettool ('tool1')
        self.tool2 = gettool ('tool2')
        self.tool5 = gettool ('tool5')
        self.tool6 = gettool ('tool6')
        self.tool7 = gettool ('tool7')
        self.tool8 = gettool ('tool8')
        self.tool9 = gettool ('tool9')
        self.tool10 = gettool ('tool10')
        self.tool11 = gettool ('tool11')

        return StatusCode.Success


    def finalize (self):
        self.check_bad_chan_lines()
        return StatusCode.Success


    def execute (self):
        tool = getattr (self, self.prepAlg.tool)

        ccc = ROOT.CaloCellContainer()
        if not tool.process (ccc, self.getContext()):
            return StatusCode.Failure

        rctype = self.prepAlg.rctype
        self.compare_cells (ccc, self.prepAlg.exp_cells, rctype)
        self.compare_cells (self.evtStore['MBTSContainer'], self.prepAlg.exp_mbts, rctype)
        self.compare_cells (self.evtStore['E4prContainer'], exp_e4_0, rctype)

        self.check_ei (self.prepAlg.extra_ei_flags)
        return StatusCode.Success


    @staticmethod
    def update_qbit (qb, rctype):
        if qb == 0: return 0
        return qb&(~7) | rctype


    def compare_cells (self, ccc, exp_cells, rctype):
        exp_cells = exp_cells.copy()
        idHelper  = self.detStore['CaloCell_ID'].tile_idHelper()

        for c in ccc:
            lcell = [c.ene1(), c.ene2(), c.time1(), c.time2(),
                     ord(c.qual1()), ord(c.qual2()),
                     ord(c.qbit1()), ord(c.qbit2()),
                     c.gain1(), c.gain2()]

            cid = c.ID()
            addr = (idHelper.section(cid),
                    idHelper.side(cid),
                    idHelper.module(cid),
                    idHelper.tower(cid),
                    idHelper.sampling(cid))
            l = exp_cells.get (addr)
            if not l:
                print ('xxx unexpected cell', addr, lcell)
                assert 0
                continue

            l = l[:]

            # Low 3 bits of qbit correspond to rctype, unless cell was masked.
            if l[0] != 0: l[6] = TestAlg.update_qbit (l[6], rctype)
            if l[1] != 0: l[7] = TestAlg.update_qbit (l[7], rctype)

            if (abs (lcell[0] - l[0]) > 1e-3 or
                abs (lcell[1] - l[1]) > 1e-3 or
                abs (lcell[2] - l[2]) > 1e-3 or
                abs (lcell[3] - l[3]) > 1e-3 or
                abs (lcell[4] != l[4]) or
                abs (lcell[5] != l[5]) or
                abs (lcell[6] != l[6]) or
                abs (lcell[7] != l[7]) or
                abs (lcell[8] != l[8]) or
                abs (lcell[9] != l[9])):
                print ('xxx cell mismatch: ', addr, lcell, l)
                assert 0
            del exp_cells[addr]

        for extra in exp_cells:
            print ('xxx unfound cell', extra)
            assert 0
        return


    def check_ei (self, extra_flags):
        ei = self.evtStore['EventInfo']
        assert ei.eventFlags (ROOT.xAOD.EventInfo.Tile) == (0xf0f0005 | extra_flags)
        assert ei.errorState (ROOT.xAOD.EventInfo.Tile) == ROOT.xAOD.EventInfo.Error
        return


    def make_bad_chan_lines (self, chans):
        idHelper  = self.detStore['CaloCell_ID'].tile_idHelper()
        hwidHelper = self.detStore['TileHWID']
        cabling = ROOT.TileCablingService.getInstance()

        lines = []
        for addr, data in chans:
            pmt_id = idHelper.pmt_id (*addr)
            channel_id = cabling.s2h_channel_id (pmt_id)
            frag = hwidHelper.frag (channel_id)
            channel = hwidHelper.channel (channel_id)
            line = '0x%03x %d 0 %d %d %d' % (frag, channel, data[0], data[1], data[2])
            lines.append (line)

        return '\n'.join (lines)


    def check_bad_chan_lines (self):
        for name, (chans, lines) in baddefs.items():
            xlines = self.make_bad_chan_lines (chans)
            if xlines.strip() != lines.strip():
                print ('xxx Bad chan lines need updating: ', name)
                print (xlines)
        return


#########################################################################


baddefs = {}
BAD_LOW  = [1<<1,    0,    0] # AdcDead
BAD_HIGH = [   0, 1<<1,    0] # AdcDead
BAD_BOTH = [   0,    0, 1<<1] # NoHV
BADTIMING = [  0,    0, 1<<9] # BadTiming

# chans: [(sec, side, mod, tow, samp, pmt), [DATA0, DATA1, DATA2]]
# We'd like to specify cells using offline addressing, but the bad cell
# file uses online addressing.  We can convert, but that requires
# idhelpers, etc, that aren't available until after initialization.
# So we pass in both representations; in finalize(), we'll check
# that they match and print out the correct bad cell lines
# if they do not.
def make_tileBadChanTool (name, chans = [], lines = ''):
    global ToolSvc
    from TileConditions.TileCondProxyConf import getTileCondProxy
    from TileConditions.TileConditionsConf import TileBadChanTool
    from TileConditions.TileConditionsConf import TileBadChannelsCondAlg

    # File data line format:
    #  frag channel dummy adcStatus0 adcStatus1 chnStatus
    #  chnStatus + adcStatusN are given to TileBchDecoder.
    #  Offline problem masks:
    #         chn                         adc
    #   0     GeneralMaskChannel          GeneralMaskAdc
    #   1     NoPmt                       AdcDead
    #   2     NoHV                        StuckBit
    #   3     WrongHV                     DataCorruption
    #   4     NoLaser                     VeryLargeHfNoise
    #   5     BadLaser                    NoData
    #   6     NoCesium                    WrongDspConfig
    #   7     BadCesium                   LargeHfNoise
    #   8     NoTiming                    CorrelatedNoise
    #   9     BadTiming                   LargeLfNoise
    #  10     TrigGeneralMask             NoCis
    #  11     TrigNoGain                  BadCis
    #  12     TrigHalfGain                SevereStuckBit
    #  13     TrigNoisy                   SevereDataCorruption
    #  14     Emergency                   IgnoredByDQV
    #  15     HVReadoutPb
    #  16     BrokenClearFibre
    #  17     IgnoreCs
    #  18     UnstableCs
    #
    # 
    #  Online problem masks:
    #         chn                         adc
    #   0     IgnoredInDsp                OnlineGeneralMaskAdc
    #   1     IgnoredInHlt
    #   2     DisableForL1
    #   3     TrigGeneralMask
    #   4     TrigNoGain
    #   5     TrigHalfGain
    #   6     TrigNoisy
    #   7     OnlineBadTiming

    if not chans:
        TileBchList = 'TileNoBad.oflBch'
    else:
        TileBchList = name + '.bch'
        f = open (TileBchList, 'w')
        print ('OBJVERSION 0', file=f)
        print ('0x000	0	0	0 0 0', file=f)
        f.write (lines)
        f.close()
        baddefs[name] = (chans, lines)

    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    condAlg = TileBadChannelsCondAlg (name + 'Cond',
                                      TileBadChannels = name + 'CondData',
                                      OflBchProxy = getTileCondProxy ('FILE', 'Bch', TileBchList, name + '_ofl'),
                                      OnlBchProxy = getTileCondProxy ('FILE', 'Bch', 'TileNoBad.oflBch', name + '_onl'),
                                      )
    condSeq += condAlg

    bct = TileBadChanTool (name,
                           TileBadChannels = name + 'CondData')
    return bct


bct1 = make_tileBadChanTool ('tilecellbuilder_bct1')

bct2 = make_tileBadChanTool ('tilecellbuilder_bct2',
                             [[(3, 1, 18,  8, 2, 0), BAD_HIGH],
                              [(2, 1, 18,  9, 1, 0), BAD_LOW],
                              [(2, 1, 18, 12, 0, 1), BAD_BOTH],
                              [(2, 1, 18, 11, 0, 0), BADTIMING],
                              [(4, 1,  0,  1, 0, 0), BAD_BOTH],
                              [(4, 1,  6,  1, 0, 0), BADTIMING],
                              ],
                             """
0x312 2 0 0 2 0
0x312 4 0 2 0 0
0x312 11 0 0 0 2
0x312 6 0 0 0 512
0x307 12 0 0 0 2
0x335 12 0 0 0 512
""")



from TileRecUtils.TileRecUtilsConf import TileCellBuilder, \
    TileRawChannelNoiseFilter, TileDQstatusAlg
noisefilter = TileRawChannelNoiseFilter ('noisefilter')

def maketool (name, bct, **kw):
    return TileCellBuilder (name, TileBadChanTool = bct, **kw)
ToolSvc += maketool ('tool1', bct1)
ToolSvc += maketool ('tool2', bct2)
ToolSvc += maketool ('tool5', bct1, fakeCrackCells = True)
ToolSvc += maketool ('tool6', bct1, EThreshold = 300)
ToolSvc += maketool ('tool7', bct1, correctAmplitude = True, correctTime = True)
ToolSvc += maketool ('tool8', bct1, NoiseFilterTools = [noisefilter])
ToolSvc += maketool ('tool9', bct1, TileDSPRawChannelContainer = 'TileRawChannelCntDsp')
ToolSvc += maketool ('tool10', bct1, TileDSPRawChannelContainer = 'TileRawChannelCntDsp',
                     correctTime = True, correctAmplitude = True)
ToolSvc += maketool ('tool11', bct1, TileDSPRawChannelContainer = 'TileRawChannelCntDsp',
                     NoiseFilterTools = [noisefilter])

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg (DoBeginRun = False)

prepalg1 = PrepareDataAlg ('prepalg1')
topSequence += prepalg1

dqstat1 = TileDQstatusAlg ('dqstat1', TileRawChannelContainer = 'TRCDQ')
topSequence += dqstat1

testalg1 = TestAlg ('testalg1', prepalg1)
topSequence += testalg1



def addr(x):
    cabling=ROOT.TileCablingService.getInstance()
    hwid = ROOT.HWIdentifier(x)
    adcid=cabling.h2s_adc_id(hwid)
    idHelper  = testalg1.detStore['CaloCell_ID'].tile_idHelper()
    return (idHelper.section(adcid),
            idHelper.side(adcid),
            idHelper.module(adcid),
            idHelper.tower(adcid),
            idHelper.sampling(adcid),
            idHelper.pmt(adcid),
            idHelper.adc(adcid))

def selindex (fname):
    from array import array
    cabling=ROOT.TileCablingService.getInstance()
    lcoll = ''
    for l in open(fname).readlines():
        l = l.strip()
        if l.find('Coll') == 0:
            lcoll = l
            continue
        if l.find('[') < 0: continue
        iid = int(l.split()[0])
        hwid = ROOT.HWIdentifier(iid)
        index=array('i',[0]) 
        pmt=array('i',[0])
        cabling.h2s_cell_id_index (hwid, index, pmt)
        #if index[0] == -3:
        #    print (iid, addr(iid), lcoll, l)
        hwidHelper = testalg1.detStore['TileHWID']
        ros = hwidHelper.ros (hwid)
        chan = hwidHelper.channel (hwid)
        if chan == 0 and ros > 2:
            drawer = hwidHelper.drawer (hwid)
            idx = (ros - 3) * 64 + drawer
            print (iid, idx, addr(iid), lcoll, l)
    return


# Suppress useless GeoModelSvc messages.
from AthenaCommon import Constants
GeoModelSvc().OutputLevel=Constants.WARNING
