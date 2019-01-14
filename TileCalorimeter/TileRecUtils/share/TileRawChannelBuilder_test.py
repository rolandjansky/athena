#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# File: TileRecUtils/share/TileRawChannelBuilder_test.py
# Author: sss
# Date: Aug, 2018
# Brief: Test for TileRawChannelBuilder.
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

theApp.EvtMax=4



def reldiff (a, b):
    den = abs(a)+abs(b)
    if den == 0: den = 1
    return abs(a-b)/den



digits_0 = [
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
]


exp_chans_0 = {
    (1, 1,  0,  1, 1, 0) : [  0.2,  22.1,  2.4,     42.9],
    (1, 1,  0,  1, 1, 1) : [ -0.4,  75.0,  2.7, 120031.4],
    (1, 1,  0,  1, 0, 0) : [ -0.9, -13.3,  1.5, 120037.4],
    (1, 1,  0,  2, 0, 1) : [ 41.8,   0.1,  2.5,     45.7],
    (1, 1,  0,  2, 0, 0) : [ 30.6,  -0.4,  2.0, 120033.1],
    (1, 1,  0,  2, 1, 1) : [  1.4,  12.9,  4.7,     43.1],
    (1, 1,  0,  2, 1, 0) : [  3.4,   3.5,  1.9,     54.2],
    (4, 1,  0,  1, 0, 0) : [ 59.1,   2.8,  6.9,     56.2],
    (3, 1,  8, 13, 3, 0) : [ 26.9,   4.8,  5.2,     45.4],
    (3, 1, 18, 13, 3, 0) : [  4.4, -26.5, 23.9,  60051.2],
    (3, 1, 18, 15, 3, 0) : [ -0.1, -75.0,  3.7, 120051.0],
    (3, 1, 18,  8, 2, 0) : [ -0.1,  75.0,  4.2, 120041.7],
    (3, 1, 18,  8, 2, 1) : [  3.5, -11.9,  2.6, 120044.7],
    (3, 1, 18,  9, 1, 0) : [  0.8, -17.6,  1.8, 120049.2],
    (3, 1, 18,  9, 1, 1) : [  0.1, -75.0,  3.4, 120048.4],
    (2, 1, 18, 11, 0, 0) : [  8.7,   0.4,  2.7,  60050.2],
    (2, 1, 18, 11, 0, 1) : [  6.3,   1.9,  1.6,  60048.6],
    (2, 1, 18, 10, 1, 0) : [ 53.3,   1.3,  3.8,  60038.4],
    (2, 1, 18, 10, 1, 1) : [ 64.1,   1.0,  3.5,  60036.9],
    (2, 1, 18, 12, 0, 0) : [ -0.2, -75.0,  2.8, 120038.1],
    (2, 1, 18, 12, 0, 1) : [  0.5, -32.7,  1.9, 120040.3],
    (3, 1, 18, 10, 3, 0) : [  5.4,   7.7,  8.2,  60057.0],
    (3, 1, 18, 11, 3, 0) : [ 48.9,   0.4,  4.0,  60032.3],
    (2, 1, 18, 11, 1, 0) : [ 38.5,   0.5,  1.7,  60046.8],
    (2, 1, 18, 11, 1, 1) : [ 48.1,   0.3,  1.6,  60046.3],
    (2, 1, 18, 10, 2, 0) : [  0.3,  75.0,  2.0, 170034.9],
    (2, 1, 18, 10, 2, 1) : [  0.1, -75.0,  3.7, 120054.7],
    (4, 1,  6,  1, 0, 0) : [ 56.4,   1.2,  2.9,     39.9],
    (4, 1,  6,  0, 0, 0) : [ 44.9,   0.5,  1.4,     42.1],
    (4, -1, 0,  2, 0, 0) : [  2.3,   3.2,  2.5,     44.4],
    (4, -1, 1,  2, 0, 0) : [  3.1,  -5.3,  2.4,  10044.4],
    (4, -1, 2,  2, 0, 0) : [ -0.4,  27.6,  1.3,  10032.5],
    (4, -1, 3,  2, 0, 0) : [  0.2, -75.0,  2.4,  10035.4],
    (4, -1, 3,  0, 0, 0) : [107.9,   1.7,  7.8,     47.8],
}


def exp_merge (base, d):
    new = base.copy()
    new.update (d)
    return new


# TileDQstatus errors
exp_chans_1 = exp_merge (exp_chans_0, {
    (3, 1, 18, 13, 3, 0) : [  4.4, -26.5, 23.9,  70051.2],
    (3, 1, 18, 15, 3, 0) : [ -0.1, -75.0,  3.7,  70051.0],
    (3, 1, 18,  8, 2, 0) : [ -0.1,  75.0,  4.2,  70041.7],
    (2, 1, 18, 10, 1, 1) : [ 64.1,   1.0,  3.5,  70036.9],
    (2, 1, 18, 12, 0, 0) : [ -0.2, -75.0,  2.8,  70038.1],
    (2, 1, 18, 12, 0, 1) : [  0.5, -32.7,  1.9,  70040.3],
    })


# Noise filter
exp_chans_2 = exp_merge (exp_chans_0, {
    (1, 1, 0, 1, 1, 0) : [ -0.6, 22.1, 2.4, 43.7],
    (1, 1, 0, 2, 0, 1) : [ 41.0,  0.1, 2.5, 46.5],
    (1, 1, 0, 2, 1, 1) : [  0.6, 12.9, 4.7, 43.9],
})
    

# Noise filter+dsp
exp_chans_3 = exp_merge (exp_chans_0, {
    (1, 1, 0, 1, 1, 0) : [  0.125,  22.1,  2.4,     42.975],
    (1, 1, 0, 1, 1, 1) : [ -0.475,  75.0,  2.7, 120031.477],
    (1, 1, 0, 1, 0, 0) : [ -0.975, -13.3,  1.5, 120037.477],
    (1, 1, 0, 2, 0, 1) : [ 41.725,   0.1,  2.5,     45.775],
    (1, 1, 0, 2, 0, 0) : [ 30.525,  -0.4,  2.0, 120033.180],
    (1, 1, 0, 2, 1, 1) : [  1.325,  12.9,  4.7,     43.175],
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
#########################################################################


class PrepareDataAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return


    def initialize (self):
        return StatusCode.Success


    def execute (self):
        iev = self.getContext().evt()

        if iev == 1:
            baddq = {146: (2, 10)}
            rc = self.make_bad_rc (baddq)
        elif iev == 3:
            rc = self.make_rc (digits_0)
        else:
            rc = self.make_rc ({})

        self.evtStore.record (rc, 'TRCDQ', False)

        return StatusCode.Success

            
    def make_rc (self, digits):
        idHelper  = self.detStore['CaloCell_ID'].tile_idHelper()

        unit = 0 # TileRawChannelUnit::ADCcounts
        typ = TileFragHash.Default
        cont = ROOT.TileRawChannelContainer (False, typ, unit)
        hashFunc = cont.hashFunc()

        digits = dict (digits)
        for icoll in range(256):
            colldata = digits.get (icoll, [])

            coll = self.make_rc_coll (idHelper, hashFunc, icoll, colldata)
            cont.addCollection (coll, ROOT.IdentifierHash(icoll))
            ROOT.SetOwnership (coll, False)

        return cont


    def make_rc_coll (self, idHelper, hashFunc, icoll, colldata):
        coll = ROOT.TileRawChannelCollection (hashFunc.identifier(icoll))

        for addr, data in colldata:
            if type(addr) == type(()):
                adc_id = idHelper.adc_id (*addr)
                chan = ROOT.TileRawChannel (adc_id, *data)
            else:
                hwid = ROOT.HWIdentifier (addr)
                chan = ROOT.TileRawChannel (hwid, *data)
            chan.setPedestal (0)
            coll.push_back (chan)

        return coll


    def make_bad_rc (self, baddq):
        cont = ROOT.TileRawChannelContainer (False, TileFragHash.OptFilterDsp, 0)
        hashFunc = cont.hashFunc()
        for icoll, chans in baddq.items():
            coll = ROOT.TileRawChannelCollection (hashFunc.identifier (icoll))
            mask = 0
            for chan in chans:
                mask |= (1<<(chan/3))
            coll.setFragMemoryPar(mask)
            cont.addCollection (coll, ROOT.IdentifierHash(icoll))
            ROOT.SetOwnership (coll, False)
        return cont



#########################################################################


class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return

    def initialize (self):
        ROOT.ICaloCellMakerTool

        def gettool (name):
            tool = ROOT.ToolHandle(ROOT.TileRawChannelBuilder)('TileRawChannelBuilderTest/' + name)
            if not tool.retrieve():
                assert 0
            return tool

        self.tool1 = gettool ('tool1')
        self.tool2 = gettool ('tool2')

        return StatusCode.Success


    def finalize (self):
        # self.check_bad_chan_lines()
        return StatusCode.Success


    def execute (self):
        iev = self.getContext().evt()

        if iev == 3:
            return StatusCode.Success

        dspcolls = set()

        tool = self.tool1
        exp_chans = exp_chans_0

        if iev == 0:
            # Event 0: nominal
            pass

        elif iev == 1:
            # Event 1: DQstatus bad channels
            exp_chans = exp_chans_1

        elif iev == 2:
            # Event 2: noise filter
            tool = self.tool2
            exp_chans = exp_chans_2

        else:
            # Event 3: noise filter + dsp
            exp_chans = exp_chans_3

        digits = self.make_digits (digits_0)

        if not tool.createContainer():
            return StatusCode.Failure

        for coll in digits:
            tool.build (coll)

        if not tool.commitContainer():
            return StatusCode.Failure

        chans = self.evtStore['TileRawChannelFiltered']
        self.compare_chans (chans, exp_chans)

        return StatusCode.Success


    @staticmethod
    def make_vec (data):
        v = getattr(ROOT, 'std::vector<float>')()
        v.reserve (len (data))
        for x in data:
            v.push_back (x)
        return v


    def make_digits (self, digits):
        idHelper  = self.detStore['CaloCell_ID'].tile_idHelper()

        unit = 0 # TileRawChannelUnit::ADCcounts
        typ = TileFragHash.Default
        cont = ROOT.TileDigitsContainer (False, typ, unit)
        hashFunc = cont.hashFunc()

        for icoll, colldata in digits:

            coll = self.make_digit_coll (idHelper, hashFunc, icoll, colldata)
            cont.addCollection (coll, ROOT.IdentifierHash(icoll))
            ROOT.SetOwnership (coll, False)

        return cont


    def make_digit_coll (self, idHelper, hashFunc, icoll, colldata):
        coll = ROOT.TileDigitsCollection (hashFunc.identifier(icoll))

        for addr, data in colldata:
            if type(addr) == type(()):
                adc_id = idHelper.adc_id (*addr)
                chan = ROOT.TileDigits (adc_id, TestAlg.make_vec (data))
            else:
                hwid = ROOT.HWIdentifier (addr)
                chan = ROOT.TileDigits (hwid, TestAlg.make_vec (data))
            coll.push_back (chan)

        return coll


    def compare_chans (self, chans, exp_chans):
        assert chans.get_type() == 0
        assert chans.get_unit() == 0
        assert chans.get_bsflags() == (3<<28)

        exp_chans = exp_chans.copy()
        idHelper  = self.detStore['CaloCell_ID'].tile_idHelper()

        for coll in chans:
            for c in coll:
                lchan = [c.amplitude(),
                         c.time(),
                         c.quality(),
                         c.pedestal()]

                cid = c.identify()
                addr = (idHelper.section(cid),
                        idHelper.side(cid),
                        idHelper.module(cid),
                        idHelper.tower(cid),
                        idHelper.sampling(cid),
                        idHelper.pmt(cid))

                l = exp_chans.get (addr)
                if not l:
                    print ('xxx unexpected chan', addr, lchan)
                    assert 0
                    continue

                l = l[:]

                if (reldiff (lchan[0], l[0]) > 1e-3 or
                    reldiff (lchan[1], l[1]) > 1e-3 or
                    reldiff (lchan[2], l[2]) > 1e-3 or
                    reldiff (lchan[3], l[3]) > 1e-3):
                    print ('xxx chan mismatch: ', addr, lchan, l)
                    assert 0
                del exp_chans[addr]

        for extra in exp_chans:
            print ('xxx unfound chan', extra)
            assert 0
        return



#########################################################################


from TileRecUtils.TileRecUtilsConf import \
    TileRawChannelBuilderTest, TileRawChannelNoiseFilter, \
    TileDQstatusAlg
noisefilter = TileRawChannelNoiseFilter ('noisefilter')

ToolSvc += TileRawChannelBuilderTest ('tool1')
ToolSvc += TileRawChannelBuilderTest ('tool2', NoiseFilterTools = [noisefilter])

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg (DoBeginRun = False)

prepalg1 = PrepareDataAlg ('prepalg1')
topSequence += prepalg1

dqstat1 = TileDQstatusAlg ('dqstat1', TileRawChannelContainer = 'TRCDQ')
topSequence += dqstat1

testalg1 = TestAlg ('testalg1')
topSequence += testalg1


