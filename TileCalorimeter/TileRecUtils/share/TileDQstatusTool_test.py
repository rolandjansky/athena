#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: TileRecUtils/share/TileDQstatusTool_test.py
# Author: scott snyder
# Date: Nov, 2018
# Brief: Test for TileDQstatusTool.
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

theApp.EvtMax=1


from AthenaPython.PyAthenaComps import Alg, StatusCode


############################################################################


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
    WienerFilterOffline = 9


BEAM_TDC_FRAG = 0x000
BEAM_ADC_FRAG = 0x001
MUON_ADC_FRAG = 0x002
ADDR_ADC_FRAG = 0x003
LASE_PTN_FRAG = 0x004
LASE_ADC_FRAG = 0x005
ADD_FADC_FRAG = 0x006
ECAL_ADC_FRAG = 0x007
DIGI_PAR_FRAG = 0x0ff
LASER_OBJ_FRAG = 0x016


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


############################################################################


def compare_args (args, pat):
    if len(args) != len(pat): return False
    for i in range(len(args)):
        if args[i] != pat[i] and pat[i] != -1:
            return False
    return True
def check_exc (args, exclist):
    for exc in exclist:
        if compare_args (args, exc): return True
    return False
def check_flag (dq, mname, exp, exc, *args):
    ret = getattr(dq, mname)(*args)
    if bool(ret) == bool(exp): return
    if check_exc (args, exc.get(mname, [])): return
    print ('xxx', mname, ret, exp, *args)
    assert 0
    return


class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return


    def initialize (self):
        ROOT.ICaloCellMakerTool

        def gettool (name):
            tool = ROOT.ToolHandle(ROOT.ITileDQstatusTool)('TileDQstatusTool/' + name)
            if not tool.retrieve():
                assert 0
            return tool

        self.tool1 = gettool ('tool1')
        self.tool2 = gettool ('tool2')

        return StatusCode.Success


    def execute (self):
        self.test1()
        self.test2()
        self.test3()
        self.test4()
        self.test5()
        self.test6()
        self.test7()
        self.test8()
        return StatusCode.Success


    def test1 (self):
        print ('*** Starting test1')
        sys.stdout.flush()

        flag_exc = {'checkEmptyEvent' :
                    [(3,13,3), (3,13,9), (3,13,12)],
                    }

        rctype = TileFragHash.OptFilterOffline
        self.do_test (rctype, flag_exc)
        return


    def test2 (self):
        print ('*** Starting test2')
        sys.stdout.flush()

        flag_exc = {'checkEmptyEvent' :
                    [(3,13,3), (3,13,9), (3,13,12)],
                    'checkMemoryParityErr' :
                    [(3,18,0,-1), (3,18,3,-1)],
                    'isAdcDQgood' :
                    [(3,18,0,-1), (3,18,1,-1), (3,18,2,-1),
                     (3,18,9,-1), (3,18,10,-1), (3,18,11,-1),],
                    'isChanDQgood' :
                    [(3,18,0), (3,18,1), (3,18,2),
                     (3,18,9), (3,18,10), (3,18,11),],
                    }

        rctype = TileFragHash.OptFilterDsp
        rc_baddq = {146 : [('setFragMemoryPar', 0x09)]}
        self.do_test (rctype, flag_exc, rc_baddq = rc_baddq)
        return


    def test3 (self):
        print ('*** Starting test3')
        sys.stdout.flush()

        flag_exc = {'checkEmptyEvent' :
                    [(3,13,3), (3,13,9), (3,13,12)],
                    'isAdcDQgood'  : [(3,18,-1,-1)],
                    'isChanDQgood' : [(3,18,-1)],
                    'checkBCIDErr' : [(3,18,-1,-1)],
                    'checkBCIDErrDetail' : [(3,18,-1,-1)],
                    }

        rctype = TileFragHash.OptFilterDsp
        rc_baddq = {146 : [('setFragBCID', 0x6565)]}
        self.do_test (rctype, flag_exc, rc_baddq = rc_baddq)
        return


    def test4 (self):
        print ('*** Starting test4')
        sys.stdout.flush()

        flag_exc = {'checkEmptyEvent' :
                    [(3,13,3), (3,13,9), (3,13,12)],
                    }

        rctype = TileFragHash.OptFilterOffline
        self.do_test (rctype, flag_exc, isCalib = True)
        return


    def test5 (self):
        print ('*** Starting test5')
        sys.stdout.flush()

        flag_exc = {'checkEmptyEvent' :
                    [(3,13,3), (3,13,9), (3,13,12)],
                    }

        rctype = TileFragHash.OptFilterOffline
        self.do_test (rctype, flag_exc, incomplete = True)
        return


    def test6 (self):
        print ('*** Starting test6')
        sys.stdout.flush()

        flag_exc = {'checkEmptyEvent' :
                    [(3,13,3), (3,13,9), (3,13,12)],
                    }

        rctype = TileFragHash.OptFilterOffline
        self.do_test (rctype, flag_exc, beamfrag = DIGI_PAR_FRAG)
        return


    def test7 (self):
        print ('*** Starting test7')
        sys.stdout.flush()

        flag_exc = {'checkEmptyEvent' :
                    [(3,13,3), (3,13,9), (3,13,12)],
                    }

        rctype = TileFragHash.OptFilterOffline
        self.do_test (rctype, flag_exc, beamfrag = LASER_OBJ_FRAG)
        return


    def test8 (self):
        print ('*** Starting test8')
        sys.stdout.flush()

        flag_exc = {'checkEmptyEvent' :
                    [(3,13,3), (3,13,9), (3,13,12)],
                    }

        rctype = TileFragHash.OptFilterOffline
        self.do_test (rctype, flag_exc, tool = self.tool2)
        return


    def do_test (self, rctype, flag_exc, rc_baddq={},
                 isCalib = False,
                 incomplete = False,
                 beamfrag = LASE_PTN_FRAG,
                 tool = None):
        rc = self.make_raw_data (hits_0, rctype, rc_baddq)
        digits = self.make_digits (incomplete, isCalib)
        be = self.make_beamelem (beamfrag)

        dq = ROOT.TileDQstatus()

        eid = ROOT.EventIDBase()
        eid.set_bunch_crossing_id (1234)
        ctx = ROOT.EventContext()
        ctx.setEventID (eid)
        ctx.setSlot (0)

        trigType = -123
        cispar = 110*[0]
        bcid = 1234
        if beamfrag == LASE_PTN_FRAG:
            trigType = 23

        elif beamfrag == DIGI_PAR_FRAG:
            cispar = [100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                      110, 111, 112, 113, 114, 111,   2,   5,   6,   7,
                        8, 117, 118, 119, 120] + 85*[0]
            bcid = 11

        elif beamfrag == LASER_OBJ_FRAG:
            bcid = 22

        if not tool:
            tool = self.tool1
        assert tool.makeStatus (ctx, rc, digits, be, dq).isSuccess()

        assert dq.isFilled() == True
        assert dq.isBiGain() == isCalib
        assert dq.nonZeroCounter() == True
        assert dq.incompleteDigits() == incomplete
        assert dq.calibMode() == isCalib
        assert dq.trigType() == trigType
        assert dq.RODBCID() == bcid

        dq_cispar = [dq.cispar()[i] for i in range(110)]
        assert dq_cispar == cispar


        for partition in range(5):
            for drawer in range(64):
                for gain in range(2):
                    check_flag (dq, 'checkGlobalErr', False, flag_exc,
                                partition, drawer, gain)
                    check_flag (dq, 'checkGlobalCRCErr', False, flag_exc,
                                partition, drawer, gain)
                for ch in range(48):
                    for gain in range(2):
                        check_flag (dq, 'isAdcDQgood', True, flag_exc,
                                    partition, drawer, ch, gain)
                    check_flag (dq, 'isChanDQgood', True, flag_exc,
                                partition, drawer, ch)
                for dmu in range(16):
                    check_flag (dq, 'checkEmptyEvent', False, flag_exc,
                                partition, drawer, dmu)
                    for gain in range(2):
                        check_flag (dq, 'checkROD_CRCErr', False, flag_exc,
                                    partition, drawer, dmu, gain)
                        check_flag (dq, 'checkFE_CRCErr', False, flag_exc,
                                    partition, drawer, dmu, gain)
                        check_flag (dq, 'checkBCIDErr', False, flag_exc,
                                    partition, drawer, dmu, gain)
                        check_flag (dq, 'checkBCIDErrDetail', False, flag_exc,
                                    partition, drawer, dmu, gain)
                        check_flag (dq, 'checkHeaderFormatErr', False, flag_exc,
                                    partition, drawer, dmu, gain)
                        check_flag (dq, 'checkHeaderParityErr', False, flag_exc,
                                    partition, drawer, dmu, gain)
                        check_flag (dq, 'checkSampleFormatErr', False, flag_exc,
                                    partition, drawer, dmu, gain)
                        check_flag (dq, 'checkSampleParityErr', False, flag_exc,
                                    partition, drawer, dmu, gain)
                        check_flag (dq, 'checkMemoryParityErr', False, flag_exc,
                                    partition, drawer, dmu, gain)
                        check_flag (dq, 'checkSingleStrobeErr', False, flag_exc,
                                    partition, drawer, dmu, gain)
                        check_flag (dq, 'checkDoubleStrobeErr', False, flag_exc,
                                    partition, drawer, dmu, gain)

        return StatusCode.Success


    def make_raw_data (self, hits, typ, baddq):
        idHelper  = self.detStore['CaloCell_ID'].tile_idHelper()

        unit = 0 # TileRawChannelUnit::ADCcounts
        cont = ROOT.TileRawChannelContainer (False, typ, unit)
        cont.set_bsflags (0x32002000)
        hashFunc = cont.hashFunc()

        for icoll, colldata in hits:
            coll = ROOT.TileRawChannelCollection (hashFunc.identifier(icoll))
            coll.setFragFEChipMask (0)

            for func, mask in baddq.get (icoll, []):
                getattr(coll, func)(mask)

            for addr, data in colldata:
                if type(addr) == type(()):
                    adc_id = idHelper.adc_id (*addr)
                    chan = ROOT.TileRawChannel (adc_id, *data)
                else:
                    hwid = ROOT.HWIdentifier (addr)
                    chan = ROOT.TileRawChannel (hwid, *data)
                coll.push_back (chan)

            cont.addCollection (coll, ROOT.IdentifierHash(icoll))
            ROOT.SetOwnership (coll, False)

        return cont
        

    def make_digits (self, incomplete, isCalib):
        cont = ROOT.TileDigitsContainer()
        hashFunc = cont.hashFunc()
        hwidHelper = self.detStore['TileHWID']

        emptyfrag = (3, 13) # ros, drawer
        for frag in [(1, 3), emptyfrag]:
            id = hwidHelper.frag (*frag)
            coll = ROOT.TileDigitsCollection (id)

            if frag == emptyfrag:
                v = getattr(ROOT,'vector<unsigned>')()
                v.resize(16)
                v[3]  = 0xffffffff
                v[9]  = 0xffffffff
                v[12] = 0xffffffff
                coll.setFragChipHeaderWords (v)


            if frag == emptyfrag and incomplete:
                ndig = 10
            elif frag == emptyfrag and isCalib:
                ndig = 96
            else:
                ndig = 48

            digits = getattr(ROOT,'vector<float>')()
            digits.resize (12);
            for idig in range(ndig):
                dig = ROOT.TileDigits(ROOT.HWIdentifier(), digits)
                coll.push_back (dig)

            cont.addCollection (coll, ROOT.IdentifierHash (hashFunc(id)))
            ROOT.SetOwnership (coll, False)

        return cont


    def make_beamelem (self, frag):
        cont = ROOT.TileBeamElemContainer()
        hashFunc = cont.hashFunc()

        coll = ROOT.TileBeamElemCollection (frag)
        coll.setLvl1Type (123)

        if frag == LASE_PTN_FRAG:
            v = getattr(ROOT,'vector<unsigned>')()
            v.resize(16)
            v[0] = (23 << 8)
            elem = ROOT.TileBeamElem (ROOT.HWIdentifier(), v)
            coll.push_back (elem)

        elif frag == DIGI_PAR_FRAG:
            coll.setRODBCID (11)
            hwidHelper = self.detStore['TileHWID']

            def id (cha):
                ros = 1
                drawer = 1
                adc = 0
                return hwidHelper.adc_id (ros, drawer, cha, adc)

            for cha in range(15):
                hwid = id(cha)
                elem = ROOT.TileBeamElem (hwid, cha + 100)
                coll.push_back (elem)

            hwid = id(15)
            v = getattr(ROOT,'vector<unsigned>')()
            v.resize(10)
            for i in range(10):
                v[i] = 111 + i
            v[1] = 2
            v[2] = 0x08070605
            elem = ROOT.TileBeamElem (hwid, v)
            coll.push_back (elem)

        elif frag == LASER_OBJ_FRAG:
            coll.setRODBCID (22)
            

        cont.addCollection (coll, ROOT.IdentifierHash (frag))
        ROOT.SetOwnership (coll, False)

        return cont



############################################################################

from TileRecUtils.TileRecUtilsConf import TileDQstatusTool

tool1 = TileDQstatusTool ('tool1')
ToolSvc += tool1
tool2 = TileDQstatusTool ('tool2', SimulateTrips = True)
ToolSvc += tool2

test1 = TestAlg ('test1')
topSequence += test1
