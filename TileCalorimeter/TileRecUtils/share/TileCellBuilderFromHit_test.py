#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: TileRecUtils/share/TileCellBuilderFromHit_test.py
# Author: scott snyder
# Date: Nov, 2018
# Brief: Test for TileCellBuilderFromHit.
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

theApp.EvtMax=3



hits_0 = [
    # (sec, side, mod, tow, samp, pmt, adc), [energy, time]
    [0, [
        [(1, 1, 0, 1, 1, 0, 1), [ 0.2,  22.1]],
        [(1, 1, 0, 1, 1, 1, 1), [-0.4,  75.0]],
        [(1, 1, 0, 1, 0, 0, 1), [-0.9, -13.3]],
        [(1, 1, 0, 2, 0, 1, 1), [41.8,   0.1]],
        [(1, 1, 0, 2, 0, 0, 1), [30.6,  -0.4]],
        [(1, 1, 0, 2, 1, 1, 1), [ 1.4,  12.9]],
        [(1, 1, 0, 2, 1, 0, 1), [ 3.4,   3.5]],
    ]],

    [136, [
        [(3, 1, 8, 13, 3, 0, 1),  [26.9, 4.8]],
    ]],

    [146, [
        [(3, 1, 18, 13, 3, 0, 1), [ 4.4, -26.5]],
        [(3, 1, 18, 15, 3, 0, 1), [-0.1, -75.0]],
        [(3, 1, 18,  8, 2, 0, 1), [-0.1,  75.0]],
        [(3, 1, 18,  8, 2, 1, 1), [ 3.5, -11.9]],
        [(3, 1, 18,  9, 1, 0, 1), [ 0.8, -17.6]],
        [(3, 1, 18,  9, 1, 1, 1), [ 0.1, -75.0]],
        [(2, 1, 18, 11, 0, 0, 1), [ 8.7,   0.4]],
        [(2, 1, 18, 11, 0, 1, 1), [ 6.3,   1.9]],
        [(2, 1, 18, 10, 1, 0, 1), [53.3,   1.3]],
        [(2, 1, 18, 10, 1, 1, 1), [64.1,   1.0]],
        [(2, 1, 18, 12, 0, 0, 1), [-0.2, -75.0]],
        [(2, 1, 18, 12, 0, 1, 1), [ 0.5, -32.7]],
        [(3, 1, 18, 10, 3, 0, 1), [ 5.4,   7.7]],
        [(3, 1, 18, 11, 3, 0, 1), [48.9,   0.4]],
        [(2, 1, 18, 11, 1, 0, 1), [38.5,   0.5]],
        [(2, 1, 18, 11, 1, 1, 1), [48.1,   0.3]],
        [(2, 1, 18, 10, 2, 0, 1), [ 0.3,  75.0]],
        [(2, 1, 18, 10, 2, 1, 1), [0.1,  -75.0]],
    ]],

    # Some MBTS cells.
    [135, [ [(4,  1, 0, 1, 0, 0, 0), [ 59.1, 2.8]] ]],
    [181, [ [(4,  1, 6, 1, 0, 0, 0), [ 56.4, 1.2]] ]],
    [182, [ [(4,  1, 6, 0, 0, 0, 0), [ 44.9, 0.5]] ]],
    [231, [ [(4, -1, 3, 0, 0, 0, 0), [107.9, 1.7]] ]],

    # Some E4 cells.
    [220, [ [(4, -1, 0, 2, 0, 0, 0), [  2.3,   3.2]] ]],
    [223, [ [(4, -1, 1, 2, 0, 0, 0), [  3.1,  -5.3]] ]],
    [225, [ [(4, -1, 2, 2, 0, 0, 0), [ -0.4,   3.6]] ]],
    [228, [ [(4, -1, 3, 2, 0, 0, 0), [  0.2,  -3.0]] ]],

    ## Some disconnected cells.
    #[253, [
    #    [4706026383833300992, [ 0, 0, 0, 50000]],
    #    [4706027483344928768, [ 0, 0, 0, 50000]] ], ],
    #[255, [
    #    [4706157225717006336, [ 0, 0, 0, 80000]],
    #    [4706158325228634112, [ 0, 0, 0, 80000]] ], ],
]


exp_cells_0 = {
    (1, 1, 0, 1, 0) : [ -30.6,    0.0, -13.3, -13.3, 0, 0, 128,   0, 1, -1],
    (1, 1, 0, 1, 1) : [   6.8,    0.0,  22.1,   0.0, 0, 0, 128,   0, 1,  1],
    (1, 1, 0, 2, 0) : [1040.4, 1421.2,  -0.4,   0.1, 0, 0, 160, 160, 1,  1],
    (1, 1, 0, 2, 1) : [ 115.6,   47.6,   3.5,  12.9, 0, 0, 160, 160, 1,  1],

    (2, 1, 18, 10, 1):[1812.2, 2179.4,   1.3,   1.0, 0, 0, 160, 160, 1,  1],
    (2, 1, 18, 10, 2):[   0.0,    0.0,   0.0,   0.0, 0, 0,   0,   0, 1,  1],
    (2, 1, 18, 11, 0):[ 295.8,  214.2,   0.4,   1.9, 0, 0, 160, 160, 1,  1],
    (2, 1, 18, 11, 1):[1309.0, 1635.4,   0.5,   0.3, 0, 0, 160, 160, 1,  1],
    (2, 1, 18, 12, 0):[   0.0,    0.0,   0.0,   0.0, 0, 0,   0,   0, 1,  1],
    (3, 1, 18,  8, 2):[   0.0,  119.0,   0.0, -11.9, 0, 0,   0, 160, 1,  1],
    (3, 1, 18,  9, 1):[  27.2,    0.0, -17.6,   0.0, 0, 0, 128,   0, 1,  1],
    (3, 1, 18, 10, 3):[ 675.0,    0.0,   7.7,   7.7, 0, 0, 160,   0, 1, -1],
    (3, 1, 18, 11, 3):[5232.3,    0.0,   0.4,   0.4, 0, 0, 160,   0, 1, -1],
    (3, 1, 18, 13, 3):[   0.0,    0.0,   0.0,   0.0, 0, 0,   0,   0, 1, -1],
    (3, 1, 18, 15, 3):[   0.0,    0.0,   0.0,   0.0, 0, 0,   0,   0, 1, -1],

    # coll 136
    (3, 1, 8, 13, 3) :[2609.3,    0.0,   4.8,   4.8, 0, 0, 160,   0, 1, -1],
}


exp_mbts_0 = {
    (4,  1, 0, 1, 0) : [0.0931, 0.0, 2.8, 2.8, 0, 0, 160, 0, 1, -1],
    (4,  1, 6, 1, 0) : [0.0888, 0.0, 1.2, 1.2, 0, 0, 160, 0, 1, -1],
    (4,  1, 6, 0, 0) : [0.0566, 0.0, 0.5, 0.5, 0, 0, 160, 0, 1, -1],
    (4, -1, 3, 0, 0) : [0.1360, 0.0, 1.7, 1.7, 0, 0, 160, 0, 1, -1],
}


exp_e4_0 = {
    (4, -1, 0, 2, 0) : [172.5, 0.0,  3.2,  3.2, 0, 0, 160, 0, 1, -1],
    (4, -1, 1, 2, 0) : [232.5, 0.0, -5.3, -5.3, 0, 0, 160, 0, 1, -1],
    (4, -1, 2, 2, 0) : [-30.0, 0.0,  3.6,  3.6, 0, 0, 128, 0, 1, -1],
    (4, -1, 3, 2, 0) : [ 15.0, 0.0, -3.0, -3.0, 0, 0, 128, 0, 1, -1],
}


def exp_merge (base, d):
    new = base.copy()
    new.update (d)
    return new


# TileBadChan errors.
exp_cells_1 = exp_merge (exp_cells_0, {
    (1, 1,  0,  1, 0) : [-30.6, -30.6, -13.3, -13.3, 0, 0, 128, 136, 1, 1],
    (2, 1, 18, 11, 0) : [295.8, 214.2,   1.9,   1.9, 0, 0,  32, 160, 1, 1],
    (2, 1, 18, 12, 0) : [  0.0,   0.0,   0.0,   0.0, 0, 0,   0,   8, 1, 1],
    (3, 1, 18,  8, 2) : [119.0, 119.0, -11.9, -11.9, 0, 0, 168, 160, 1, 1],
})
exp_mbts_1 = exp_merge (exp_mbts_0, {
    (4, 1, 0, 1, 0) : [0.0,  0.0, -100.0, -100.0, 255, 0,  8, 8, 1, -1],
    (4, 1, 6, 1, 0) : [0.0888, 0.0,    0.0,    0.0,   0, 0, 32, 0, 1, -1],
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


class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return

    def initialize (self):
        ROOT.ICaloCellMakerTool

        def gettool (name):
            tool = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('TileCellBuilderFromHit/' + name)
            if not tool.retrieve():
                assert 0
            return tool

        self.tool1 = gettool ('tool1')
        self.tool2 = gettool ('tool2')
        self.tool3 = gettool ('tool3')

        return StatusCode.Success


    def execute (self):
        iev = self.getContext().evt()

        rctype = TileFragHash.OptFilterOffline
        bsflags = 0x32002000
        hits = hits_0
        exp_cells = exp_cells_0
        exp_mbts = exp_mbts_0
        noise_thresh = 0

        if iev == 0:
            tool = self.tool1

        elif iev == 1:
            tool = self.tool2
            exp_cells = exp_cells_1
            exp_mbts = exp_mbts_1

        elif iev == 2:
            tool = self.tool3
            noise_thresh = 0.4

        else:
            assert 0
            
        self.record_hits (hits, rctype, bsflags, 'TileHitCnt')

        ccc = ROOT.CaloCellContainer()
        if not tool.process (ccc, self.getContext()):
            return StatusCode.Failure

        self.compare_cells (ccc, exp_cells, noise_thresh)
        self.compare_cells (self.evtStore['MBTSContainer'], exp_mbts)
        self.compare_cells (self.evtStore['E4prContainer'], exp_e4_0)

        return StatusCode.Success


    def record_hits (self, hits, typ, bsflags, key):
        idHelper  = self.detStore['CaloCell_ID'].tile_idHelper()

        unit = 0 # TileRawChannelUnit::ADCcounts
        cont = ROOT.TileHitContainer (False, typ, unit)
        cont.set_bsflags (bsflags)
        hashFunc = cont.hashFunc()

        for icoll, colldata in hits:
            coll = ROOT.TileHitCollection (hashFunc.identifier(icoll))

            for addr, data in colldata:
                adc_id = idHelper.adc_id (*addr)
                hit = ROOT.TileHit (adc_id, *data)
                coll.push_back (hit)

            cont.addCollection (coll, ROOT.IdentifierHash(icoll))
            ROOT.SetOwnership (coll, False)

        self.evtStore.record (cont, key, False)
        return


    def compare_cells (self, ccc, exp_cells, noise_thresh=0):
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
                if abs(lcell[0]) > noise_thresh:
                    print ('xxx unexpected cell', addr, lcell)
                    assert 0
                continue

            l = l[:]

            thr = max (1e-3, noise_thresh)
            if (abs (lcell[0] - l[0]) > thr or
                abs (lcell[1] - l[1]) > thr or
                abs (lcell[2] - l[2]) > thr or
                abs (lcell[3] - l[3]) > thr or
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



from TileRecUtils.TileRecUtilsConf import TileCellBuilderFromHit

def maketool (name, bct, noise=0, **kw):
    return TileCellBuilderFromHit (name,
                                   TileBadChanTool = bct,
                                   UseCaloNoiseTool = False,
                                   TileHitContainer = 'TileHitCnt',
                                   NoiseSigma = noise,
                                   **kw)
ToolSvc += maketool ('tool1', bct1)
ToolSvc += maketool ('tool2', bct2, maskBadChannels = True)
ToolSvc += maketool ('tool3', bct1, noise = 0.1)

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg (DoBeginRun = False)

testalg1 = TestAlg ('testalg1')
topSequence += testalg1



# Suppress useless GeoModelSvc messages.
from AthenaCommon import Constants
GeoModelSvc().OutputLevel=Constants.WARNING
