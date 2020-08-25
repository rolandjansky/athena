#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
#
# File: LArCellRec/share/LArCellDeadOTXCorr_test.py
# Author: sss
# Date: Aug, 2018
# Brief: Unit test for LArCellDeadOTXCorr.
#

from __future__ import print_function

import ROOT
ROOT.TH1F

from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.LAr_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.isOnline.set_Value(True)


import sys
import string
import ROOT
import math
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]
from AtlasGeoModel import LArGM

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc().GlobalTag = 'OFLCOND-RUN12-SDR-35'

include('LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py')

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

theApp.EvtMax=2


import ROOT
from LArCellRec.LArCellDeadOTXCorrToolDefault import LArCellDeadOTXCorrToolDefault

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


# barrel_ec, pos_neg, feedthrough, slot
BADFEBS2 = [[(0, 1, 12,  3), 'deadReadout'],  # tt  0.2 2.5 (below noise)
            [(1, 0,  3,  2), 'deadReadout'],  # tt -2.8 2.8
            [(0, 0, 10,  1), 'deadReadout'],  # tt -1.0 1.1
            ]


###########################################################################


# feb file format:
# barrel_ec pos_neg feedthrough slot flags
#  0       1       12      4  deadAll
#  1       0       16      9 deadReadout deadAll
# badfebs is a list of (barrel_ec, pos_neg, feedthrough, slot) tuples
def make_bad_channel_condalg (name, badfebs = []):
    from LArBadChannelTool.LArBadChannelToolConf import LArBadFebCondAlg
    febfile = ''
    if badfebs:
        febfile = name + '.badfebs'
        f = open (febfile, 'w')
        for (feb, err) in badfebs:
            print (feb[0], feb[1], feb[2], feb[3], err, file=f)
        f.close()
    alg = LArBadFebCondAlg ('LArBadFebCondAlg' + name,
                            ReadKey = '',
                            WriteKey = name,
                            InputFileName = febfile)

    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    condSeq += alg
    return


###########################################################################




def make_calo_cells (detStore):
    mgr = detStore['CaloMgr']
    ccc = ROOT.CaloCellContainer()
    for i in range (mgr.element_size()):
        elt = mgr.get_element (ROOT.IdentifierHash (i))
        if not elt: break
        cc = ROOT.CaloCell (elt, i%1000, i+0.5, 10, 11, 1)
        ccc.push_back (cc)
        ROOT.SetOwnership (cc, False)
    ccc.order()
    ccc.updateCaloIterators()
    return ccc


# Data from xAODTriggerTowers dumped from an example MC event.
tt_table = [
  [ -0.1 , 1.4 , [32, 30, 35, 36, 33, 32, 32] ],
  [ -0.2 , 3.9 , [35, 31, 34, 38, 38, 35, 36] ],
  [ -0.2 , 5.0 , [31, 31, 32, 36, 38, 31, 33] ],
  [ -0.2 , 5.3 , [32, 31, 31, 35, 33, 30, 30] ],
  [ -0.2 , 6.2 , [35, 30, 33, 38, 30, 32, 32] ],
  [ -0.2 , 2.9 , [29, 34, 46, 59, 44, 33, 29] ],
  [ -0.2 , 4.8 , [31, 30, 34, 36, 31, 29, 29] ],
  [ -0.2 , 6.1 , [36, 35, 42, 53, 43, 37, 29] ],
  [ -0.3 , 3.3 , [32, 33, 35, 38, 37, 30, 30] ],
  [ -0.3 , 5.9 , [31, 32, 35, 41, 37, 31, 30] ],
  [ -0.3 , 6.0 , [32, 35, 44, 54, 44, 33, 27] ],
  [ -0.3 , 6.1 , [39, 45, 82, 131, 91, 45, 22] ],
  [ -0.3 , 6.2 , [33, 35, 43, 59, 47, 36, 27] ],
  [ -0.6 , 0.9 , [31, 26, 32, 36, 32, 32, 33] ],
  [ -0.6 , 2.9 , [32, 32, 32, 36, 32, 29, 33] ],
  [ -0.9 , 1.2 , [34, 33, 35, 41, 32, 31, 31] ],
  [ -0.9 , 1.0 , [34, 34, 37, 44, 40, 33, 32] ],
  [ -0.9 , 1.1 , [32, 32, 33, 36, 36, 33, 31] ],
  [ -0.9 , 1.6 , [35, 32, 34, 38, 36, 33, 33] ],
  [ -0.9 , 4.4 , [29, 28, 31, 35, 32, 27, 28] ],
  [ -1.0 , 0.6 , [31, 32, 34, 37, 36, 31, 28] ],
  [ -1.0 , 0.8 , [31, 33, 35, 37, 35, 33, 28] ],
  [ -1.0 , 0.9 , [34, 37, 38, 43, 37, 31, 29] ],
  [ -1.0 , 1.0 , [39, 44, 76, 123, 88, 42, 22] ],
  [ -1.0 , 1.1 , [44, 52, 129, 239, 157, 57, 7] ],  # USED
  [ -1.0 , 1.2 , [33, 30, 32, 38, 39, 33, 36] ],
  [ -1.1 , 1.0 , [31, 33, 36, 44, 39, 35, 31] ],
  [ -1.1 , 1.1 , [31, 28, 33, 41, 37, 34, 30] ],
  [ -1.1 , 1.8 , [32, 33, 34, 37, 35, 32, 31] ],
  [ -1.2 , 1.1 , [36, 33, 35, 37, 35, 33, 33] ],
  [ -1.4 , 0.8 , [34, 31, 32, 35, 30, 30, 32] ],
  [ -1.5 , 0.5 , [32, 29, 30, 36, 33, 30, 29] ],
  [ -1.5 , 4.5 , [33, 34, 36, 44, 40, 34, 30] ],
  [ -1.5 , 3.2 , [30, 33, 34, 36, 34, 31, 32] ],
  [ -1.5 , 4.3 , [35, 35, 36, 41, 37, 33, 29] ],
  [ -1.6 , 4.3 , [33, 33, 34, 36, 35, 31, 33] ],
  [ -1.8 , 0.0 , [31, 34, 36, 39, 35, 32, 30] ],
  [ -1.8 , 4.3 , [31, 31, 35, 37, 35, 31, 31] ],
  [ -1.8 , 5.1 , [32, 33, 35, 40, 37, 33, 33] ],
  [ -1.9 , 0.0 , [33, 33, 36, 37, 35, 32, 31] ],
  [ -1.9 , 4.4 , [33, 34, 36, 37, 36, 32, 30] ],
  [ -1.9 , 5.5 , [33, 35, 40, 51, 44, 34, 29] ],
  [ -1.9 , 6.2 , [32, 34, 37, 41, 37, 31, 32] ],
  [ -2.2 , 6.0 , [32, 31, 33, 37, 32, 30, 30] ],
  [ -2.3 , 2.2 , [35, 37, 59, 88, 66, 38, 25] ],
  [ -2.3 , 2.3 , [32, 36, 47, 62, 51, 34, 28] ],
  [ -2.5 , 2.1 , [33, 33, 34, 36, 33, 33, 30] ],
  [ -2.5 , 2.9 , [34, 35, 36, 41, 37, 35, 32] ],
  [ -2.5 , 3.2 , [30, 34, 39, 50, 43, 35, 29] ],
  [ -2.6 , 2.3 , [36, 37, 36, 37, 33, 31, 29] ],
  [ -2.6 , 2.5 , [32, 35, 38, 40, 36, 31, 29] ],
  [ -2.6 , 2.8 , [33, 37, 39, 49, 40, 33, 29] ],
  [ -2.6 , 3.0 , [37, 37, 40, 47, 44, 36, 32] ],
  [ -2.6 , 3.2 , [35, 34, 35, 37, 34, 32, 32] ],
  [ -2.6 , 3.4 , [33, 36, 34, 37, 34, 32, 30] ],
  [ -2.8 , 2.8 , [40, 44, 64, 95, 71, 39, 25] ],  # USED
  [ -2.8 , 3.0 , [35, 39, 52, 69, 53, 34, 27] ],
  [ -3.8 , 0.2 , [33, 37, 32, 31, 31, 33, 35] ],
  [ -3.8 , 1.8 , [33, 38, 35, 35, 29, 24, 30] ],
  [ -3.8 , 3.3 , [30, 35, 33, 32, 31, 31, 33] ],
  [ 0.1 , 0.9 , [33, 33, 33, 38, 39, 32, 32] ],
  [ 0.1 , 4.9 , [32, 33, 34, 37, 34, 31, 27] ],
  [ 0.1 , 5.6 , [31, 33, 35, 41, 37, 32, 33] ],
  [ 0.2 , 0.9 , [30, 30, 31, 37, 32, 29, 29] ],
  [ 0.2 , 2.5 , [35, 31, 31, 36, 31, 29, 30] ], # USED
  [ 0.2 , 3.3 , [32, 28, 34, 36, 31, 30, 32] ],
  [ 0.3 , 1.7 , [35, 32, 35, 37, 34, 32, 33] ],
  [ 0.3 , 5.6 , [34, 29, 31, 35, 34, 30, 32] ],
  [ 0.4 , 0.8 , [34, 32, 33, 37, 32, 29, 27] ],
  [ 0.4 , 1.2 , [30, 29, 29, 35, 30, 28, 30] ],
  [ 0.6 , 1.8 , [31, 33, 34, 38, 35, 33, 31] ],
  [ 0.6 , 3.6 , [31, 30, 33, 35, 31, 30, 32] ],
  [ 0.9 , 1.6 , [33, 33, 34, 37, 36, 31, 30] ],
  [ 0.9 , 5.4 , [31, 31, 36, 36, 34, 31, 31] ],
  [ 0.9 , 1.8 , [32, 34, 31, 35, 33, 29, 31] ],
  [ 0.9 , 3.0 , [32, 35, 32, 38, 34, 31, 29] ],
  [ 0.9 , 5.0 , [32, 33, 35, 37, 38, 33, 32] ],
  [ 1.0 , 5.6 , [34, 33, 37, 38, 33, 32, 30] ],
  [ 1.1 , 4.5 , [33, 31, 33, 37, 34, 31, 31] ],
  [ 1.4 , 1.4 , [33, 33, 36, 39, 37, 34, 31] ],
  [ 1.4 , 1.5 , [31, 32, 34, 38, 36, 33, 32] ],
  [ 1.5 , 1.8 , [33, 36, 38, 39, 38, 32, 31] ],
  [ 1.5 , 4.8 , [31, 30, 34, 37, 36, 33, 32] ],
  [ 2.0 , 1.6 , [31, 33, 37, 40, 37, 32, 30] ],
  [ 2.6 , 2.8 , [36, 35, 35, 38, 38, 33, 33] ],
  [ 2.6 , 5.2 , [31, 34, 36, 36, 31, 29, 31] ],
  [ 3.8 , 4.1 , [33, 36, 35, 34, 29, 25, 26] ],
  [ 4.3 , 5.3 , [34, 44, 44, 51, 27, 5, 10] ],
  [ -1.6 , 1.6 , [30, 30, 32, 33, 33, 30, 31] ],
  [ -1.8 , 4.2 , [33, 31, 36, 41, 39, 36, 34] ],
  [ -1.8 , 4.3 , [32, 33, 38, 40, 37, 35, 33] ],
  [ -1.9 , 4.4 , [31, 33, 46, 53, 49, 42, 35] ],
  [ -1.9 , 4.5 , [31, 32, 34, 36, 34, 33, 32] ],
  [ -2.0 , 4.1 , [32, 32, 34, 35, 34, 33, 32] ],
  [ -2.0 , 6.2 , [32, 31, 47, 56, 51, 42, 35] ],
  [ -2.2 , 3.2 , [30, 30, 32, 33, 33, 31, 31] ],
  [ -2.2 , 3.5 , [33, 31, 33, 35, 32, 32, 32] ],
  [ -2.3 , 2.7 , [32, 31, 32, 34, 32, 32, 33] ],
  [ -2.5 , 2.9 , [33, 31, 33, 34, 34, 33, 31] ],
  [ -2.5 , 3.0 , [33, 33, 33, 36, 36, 33, 34] ],
  [ -2.6 , 2.8 , [32, 31, 38, 45, 43, 39, 35] ],
  [ -2.6 , 3.0 , [31, 34, 41, 45, 42, 38, 33] ],
  [ -2.6 , 3.2 , [35, 35, 38, 41, 38, 37, 36] ],
  [ -2.8 , 2.8 , [36, 35, 42, 48, 43, 38, 33] ],
  [ -2.8 , 3.0 , [35, 35, 47, 55, 48, 41, 36] ],
  [ -3.0 , 2.8 , [32, 35, 37, 40, 38, 37, 37] ],
  [ -3.0 , 3.0 , [33, 34, 38, 40, 38, 37, 35] ],
  [ -4.7 , 2.9 , [34, 38, 36, 37, 32, 32, 31] ],
  [ 1.8 , 4.3 , [31, 30, 34, 34, 33, 31, 31] ],
  [ 0.8 , 4.9 , [32, 31, 34, 37, 35, 34, 34] ],
  [ -0.6 , 2.8 , [33, 31, 35, 38, 33, 34, 31] ],
  [ -0.9 , 2.9 , [32, 31, 36, 40, 39, 34, 33] ],
  [ -0.3 , 6.1 , [30, 29, 33, 36, 34, 34, 28] ],
  [ -0.3 , 6.2 , [33, 31, 36, 37, 38, 32, 34] ],
  [ -0.4 , 6.2 , [30, 33, 35, 38, 39, 34, 33] ],
  [ 1.4 , 4.7 , [31, 33, 32, 37, 33, 32, 31] ],
  [ -1.0 , 1.1 , [34, 34, 38, 39, 36, 34, 33] ],
  [ -1.2 , 4.1 , [33, 32, 34, 35, 35, 34, 32] ],
  [ -0.1 , 0.0 , [28, 31, 35, 38, 34, 32, 34] ],
]

def make_TriggerTowerContainer():
    ttc = ROOT.xAOD.TriggerTowerContainer_v2()
    aux = ROOT.xAOD.TriggerTowerAuxContainer_v2()
    ttc.setNonConstStore (aux)

    veccls = getattr (ROOT, 'std::vector<unsigned short>')
    for tt_data in tt_table:
        tt = ROOT.xAOD.TriggerTower_v2()
        ttc.push_back (tt)
        ROOT.SetOwnership (tt, False)
        tt.setEta (tt_data[0])
        tt.setPhi (tt_data[1])
        v = veccls()
        v.reserve (len(tt_data[2]))
        for adc in tt_data[2]:
            v.push_back (adc)
        tt.setAdc (v)
    return (ttc, aux)


###########################################################################

from AthenaPython.PyAthenaComps import Alg, StatusCode
class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return

    def initialize (self):
        # Work around issue with cling in root 6.20.06 getting confused
        # by forward declarations.
        ROOT.xAOD.TriggerTowerContainer_v2

        ROOT.ICaloCellMakerTool
        self.tool1 = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('LArCellDeadOTXCorr/tool1')
        self.tool2 = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('LArCellDeadOTXCorr/tool2')

        self.idmgr = self.detStore['CaloIdManager']
        self.onlineID = self.detStore['LArOnlineID']
        self.offlineID  = self.detStore['CaloCell_ID']
        self.ccc = None
        if not self.tool1.retrieve():
            return StatusCode.Failure
        if not self.tool2.retrieve():
            return StatusCode.Failure

        self.ttsvc = ROOT.ToolHandle(ROOT.CaloTriggerTowerService)('CaloTriggerTowerService')
        if not self.ttsvc.retrieve():
            return StatusCode.Failure

        return StatusCode.Success

    def execute (self):
        if not self.ccc:
            self.ccc = make_calo_cells (self.detStore)

        ctx = self.getContext()
        iev = ctx.evt()

        (t, a) = make_TriggerTowerContainer()
        if not self.evtStore.record (t, 'xAODTriggerTowers', False):
            return StatusCode.Failure
        if not self.evtStore.record (a, 'xAODTriggerTowersAux.', False):
            return StatusCode.Failure

        if iev == 0:
            # Event 0: no errors
            tool = self.tool1
            exp_diffs = []

        else:
            # Event 1: Some dead FEBs
            tool = self.tool2
            exp_diffs = [
                # -2.8, 2.8
                ([604, 605, 668, 669], 31469.2148438),
                ([472, 473, 536, 537], 3315.55249023),
                ([476, 477, 540, 541], 3067.95507812),
                ([478, 479, 542, 543], 6195.74316406),
                ([606, 607, 670, 671], 18408.6542969),
                ([732, 733], 4353.24609375),
                ([734, 735, 798, 799], 4605.72900391),
                ([796, 797], 4353.24609375),
                # -1.0, 1.1
                ([34443, 34507, 34571, 34635], 12576.5966797),
                ([34442, 34506, 34570, 34634], 1314.39941406),
                ]

        if not tool.process (self.ccc, ctx):
            return StatusCode.Failure
 
        self.check_and_reset_cells (exp_diffs)

        #self.print_tt_febs()

        return StatusCode.Success
    

    def check_and_reset_cells (self, exp_diffs):
        cell_diffs = {}
        for ids, e in exp_diffs:
            for xid in ids:
                cell_diffs[xid] = e

        for c in self.ccc:
            i = c.caloDDE().calo_hash().value()
            exp_e = cell_diffs.get (i)
            if exp_e != None:
                assert abs (c.energy() - exp_e) < 1e-6
            else:
                assert c.energy() == i%1000

            assert c.time() == i+0.5
            assert c.quality() == 10
            assert c.provenance() == 11
        return


    #####


    # Print list of FEBS corresponding to each entry in tt_table.
    def print_tt_febs (self):
        cabling = self.condStore()['LArOnOffIdMap'].find (ctx.eventID())
        for tt in tt_table:
            emid, hadid, febs = self.tt_to_febs (cabling, tt[0], tt[1])
            print ('tt ', tt[0], tt[1], hex(emid), hex(hadid), end='')
            for feb, idv in febs.items():
                print (feb, hex(idv), end='')
            print()
        return
        
    def tt_to_febs (self, cabling, eta, phi):
        lvl1Helper = self.idmgr.getLVL1_ID()
        pos_neg_z = self.pos_neg_z (eta)
        region = self.region_index (eta)
        ieta = self.eta_index (eta)
        iphi = self.phi_index (eta, phi)
        emid = lvl1Helper.tower_id (pos_neg_z, 0, region, ieta, iphi)
        hadid = lvl1Helper.tower_id (pos_neg_z, 1, region, ieta, iphi)
        febs = {}
        for ttid in (emid, hadid):
            for cellid in self.ttsvc.createCellIDvecTT (ttid):
                febid = cabling.createSignalChannelID (cellid)
                barrel_ec = self.onlineID.barrel_ec (febid)
                pos_neg = self.onlineID.pos_neg (febid)
                feedthrough = self.onlineID.feedthrough (febid)
                slot = self.onlineID.slot (febid)
                febs[(barrel_ec, pos_neg, feedthrough, slot)] = febid.get_compact()
        return (emid.get_compact(), hadid.get_compact(), febs)


    def pos_neg_z (self, eta):
        return 1 if eta >=0 else -1


    # min, max, etawidth, phiwidth
    ETA_REGIONS = [[0.0, 2.5,  0.1,    math.pi/32],
                   [2.5, 3.1,  0.2,    math.pi/32*2],
                   [3.1, 3.2,  0.1,    math.pi/32*2],
                   [3.2, 4.9,  0.425,  math.pi/32*4],
                    ]

    def find_region (self, eta):
        aeta = abs(eta)
        for i, r in enumerate (TestAlg.ETA_REGIONS):
            if aeta < r[1]: return (i, r)
        return (-1, None)
        
    def region_index (self, eta):
        return self.find_region (eta)[0]

    def eta_index (self, eta):
        r = self.find_region (eta)[1]
        if r:
            return int((abs(eta) - r[0]) / r[2])
        return -1

    def phi_index (self, eta, phi):
        r = self.find_region (eta)[1]
        if r:
            return int (phi / r[3])
        return -1


from LArCabling.LArCablingAccess import LArOnOffIdMapping
LArOnOffIdMapping()

make_bad_channel_condalg ('GoodFebs')
make_bad_channel_condalg ('BadFebs', BADFEBS2)

tool1 = LArCellDeadOTXCorrToolDefault ('tool1')
tool1.BadFebKey = 'GoodFebs'
ToolSvc += tool1

tool2 = LArCellDeadOTXCorrToolDefault ('tool2')
tool2.BadFebKey = 'BadFebs'
ToolSvc += tool2
#tool2.useL1CaloDB = True

#############################
# L1CaloDbTag = "<tag>HEAD</tag>"
# l1calofolder = "/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib"

# from IOVDbSvc.CondDB import conddb
# conddb.dbname= 'CONDBR2'
# if not conddb.folderRequested(l1calofolder):
#     conddb.addFolder('TRIGGER_ONL', l1calofolder + L1CaloDbTag)

#############################


testalg1 = TestAlg ('testalg1')
topSequence += testalg1
