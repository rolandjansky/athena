#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# File: TileRecUtils/share/TileDQstatusAlg_test.py
# Author: sss
# Date: Sep, 2018
# Brief: Test for TileDQstatusAlg.
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

theApp.EvtMax=2


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


# Record test data to SG.
class RecordAlg (Alg):
    def __init__ (self, name,
                  rc_key, digits_key, be_key=''):
        Alg.__init__ (self, name)
        self.rc_key = rc_key
        self.rc_hits = hits_0
        self.rc_typ = TileFragHash.OptFilterOffline
        #self.rc_typ = TileFragHash.OptFilterDsp
        self.rc_baddq = {}
        #self.rc_baddq = {146 : [2, 10]}

        self.digits_key = digits_key
        self.digits_incomplete = False
        self.digits_isCalib = False

        self.be_key = be_key
        self.be_frag = LASE_PTN_FRAG
        return


    def initialize (self):
        return StatusCode.Success


    def execute (self):
        if not self.record_raw_data (self.rc_key,
                                     self.rc_hits,
                                     self.rc_typ,
                                     self.rc_baddq):
            return StatusCode.Failure

        if not self.record_digits (self.digits_key,
                                   self.digits_incomplete,
                                   self.digits_isCalib):
            return StatusCode.Failure

        if self.be_key != '':
            if not self.record_beamelem (self.be_key, self.be_frag):
                return StatusCode.Failure
        return StatusCode.Success


    def record_raw_data (self, key, hits, typ, baddq):
        idHelper  = self.detStore['CaloCell_ID'].tile_idHelper()

        unit = 0 # TileRawChannelUnit::ADCcounts
        cont = ROOT.TileRawChannelContainer (False, typ, unit)
        cont.set_bsflags (0x32002000)
        hashFunc = cont.hashFunc()

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

            cont.addCollection (coll, ROOT.IdentifierHash(icoll))
            ROOT.SetOwnership (coll, False)

        return self.evtStore.record (cont, key, False)


    def record_digits (self, key, incomplete, isCalib):
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
        return self.evtStore.record (cont, key, False)


    def record_beamelem (self, key, frag):
        cont = ROOT.TileBeamElemContainer()
        hashFunc = cont.hashFunc()

        coll = ROOT.TileBeamElemCollection (frag)
        coll.setLvl1Type (123)

        if frag == LASE_PTN_FRAG:
            v = getattr(ROOT,'vector<unsigned>')()
            v.resize(16)
            v[0] = (23 << 8)
            elem = ROOT.TileBeamElem (ROOT.Identifier(), v)
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
            coll.setRODBCID (11)
            

        cont.addCollection (coll, ROOT.IdentifierHash (frag))
        ROOT.SetOwnership (coll, False)

        return self.evtStore.record (cont, key, False)


############################################################################


# Check results.
class CheckAlg (Alg):
    def __init__ (self, name, dq_key):
        Alg.__init__ (self, name)
        self.dq_key = dq_key
        return


    def initialize (self):
        return StatusCode.Success


    def execute (self):
        dq = self.evtStore[self.dq_key]

        assert dq.isFilled() == True
        assert dq.isBiGain() == False
        assert dq.nonZeroCounter() == True
        assert dq.incompleteDigits() == False
        assert dq.calibMode() == 0
        assert dq.trigType() == 0

        self.check_empty (dq)
        
        #cispar
        #isAdcDQgood
        #isChanDQgood
        #checkGlobalErr
        #checkGlobalCRCErr
        #checkROD_CRCErr
        #checkFE_CRCErr
        #checkBCIDErr
        #checkBCIDErrDetail
        #checkHeaderFormatErr
        #checkHeaderParityErr
        #checkSampleFormatErr
        #checkSampleParityErr
        #checkMemoryParityErr
        #checkSingleStrobeErr
        #checkDoubleStrobeErr
        return StatusCode.Success


    def check_empty (self, dq):
        for part in range(5):
            for drawer in range(64):
                for dmu in range (16):
                    is_empty = (part==3 and drawer==13 and
                                dmu in [3, 9, 12])
                    assert dq.checkEmptyEvent(part, drawer, dmu) == is_empty
        return

        
############################################################################

from TileRecUtils.TileRecUtilsConf import TileDQstatusAlg

record1 = RecordAlg ('record1',
                     'RC1', 'Dig1')
topSequence += record1
alg1 = TileDQstatusAlg ('alg1',
                        TileRawChannelContainer = 'RC1',
                        TileDigitsContainer = 'Dig1',
                        TileDQstatus = 'DQ1')
topSequence += alg1
check1 = CheckAlg ('check1', 'DQ1')
topSequence += check1
