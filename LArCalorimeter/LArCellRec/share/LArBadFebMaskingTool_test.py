#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# File: LArCellRec/share/LArBadFebMaskingTool_test.py
# Author: sss
# Date: Aug, 2018
# Brief: Unit test for LArBadFebMaskingTool.
#


from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.LAr_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.isOnline.set_Value(True)


import sys
import string
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


theApp.EvtMax=6
BADFEB1 = (0, 1, 5, 2)  # barrel_ec, pos_neg, feedthrough, slot
#BADFEB2 = (1, 0, 2, 3)  # barrel_ec, pos_neg, feedthrough, slot
BADFEB3 = (1, 0, 3, 2)  # barrel_ec, pos_neg, feedthrough, slot


import ROOT
from LArCellRec.LArCellRecConf import LArBadFebMaskingTool

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# feb file format:
# barrel_ec pos_neg feedthrough slot flags
#  0       1       12      4  deadAll
#  1       0       16      9 deadReadout deadAll
# badfebs is a list of (barrel_ec, pos_neg, feedthrough, slot) tuples
def make_bad_channel_tool (name, badfebs = []):
    from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
    febfile = ''
    if badfebs:
        febfile = name + '.badfebs'
        f = open (febfile, 'w')
        for (feb, err) in badfebs:
            print >> f, feb[0], feb[1], feb[2], feb[3], err
        f.close()
    return LArBadChanTool (name,
                           ReadFromASCII = True,
                           WriteEmptyFolders = True,
                           CoolFolder = '',
                           ComplementaryCoolFolder = '',
                           CoolMissingFEBsFolder = '',
                           FEBfile = febfile)



###########################################################################


def make_calo_cells (detStore):
    mgr = detStore['CaloMgr']
    ccc = ROOT.CaloCellContainer()
    for i in range (mgr.element_size()):
        elt = mgr.get_element (ROOT.IdentifierHash (i))
        if not elt: break
        cc = ROOT.CaloCell (elt, i, i+0.5, 10, 11, 1)
        ccc.push_back (cc)
        ROOT.SetOwnership (cc, False)
    ccc.order()
    ccc.updateCaloIterators()
    return ccc


###########################################################################


from AthenaPython.PyAthenaComps import Alg, StatusCode
class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return
    def initialize (self):
        ROOT.ICaloCellMakerTool
        self.tool1 = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('LArBadFebMaskingTool/tool1')
        self.tool2 = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('LArBadFebMaskingTool/tool2')
        self.tool3 = ROOT.ToolHandle(ROOT.ICaloCellMakerTool)('LArBadFebMaskingTool/tool3')

        self.ccc = make_calo_cells (self.detStore)
        self.onlineID = self.detStore['LArOnlineID']
        self.offlineID  = self.detStore['CaloCell_ID']
        self.cabling = ROOT.ToolHandle(ROOT.LArCablingService)('LArCablingService')
        if not self.tool1.retrieve():
            return StatusCode.Failure
        if not self.tool2.retrieve():
            return StatusCode.Failure
        if not self.tool3.retrieve():
            return StatusCode.Failure
        if not self.cabling.retrieve():
            return StatusCode.Failure
        return StatusCode.Success

    def execute (self):
        iev = self.getContext().evt()
        tool = self.tool1
        ccc = self.ccc

        if iev == 0:
            # Event 0: no errors.
            sum_errs = []
            exp_ef = 0
            exp_err = 0
            exp_errs = []

        elif iev == 1:
            # Event 1: Bad FEB summary error.
            sum_errs = [(BADFEB1, 1<<ROOT.LArFebErrorSummary.EVTID)]
            exp_ef = 16 # DATACORRUPTED
            exp_err = ROOT.xAOD.EventInfo.Error
            exp_errs = sum_errs

        elif iev == 2:
            # Event 2: Bad chan tool deadAll.
            tool = self.tool2
            sum_errs = []
            exp_ef = 0
            exp_err = 0
            exp_errs = []

        elif iev == 3:
            # Event 3: Bad chan tool inError
            tool = self.tool3
            sum_errs = []
            exp_ef = 0
            exp_err = 0
            exp_errs = [(BADFEB3, 1)]

        elif iev == 4:
            # Event 4: Bad FEB summary error suppressed by deadAll.
            sum_errs = [(BADFEB1, 1<<ROOT.LArFebErrorSummary.EVTID)]
            tool = self.tool2
            exp_ef = 0
            exp_err = 0
            exp_errs = sum_errs

        else:
            # Event 5: empty cell container.
            ccc = ROOT.CaloCellContainer()
            sum_errs = []
            exp_ef = 16 # DATACORRUPTED
            exp_err = ROOT.xAOD.EventInfo.Error
            exp_errs = []

        if not self.rec_feb_error_summary (sum_errs):
            return StatusCode.Failure
        ei = self.evtStore['EventInfo']
        if not tool.process (ccc):
            return StatusCode.Failure

        LAr = ROOT.xAOD.EventInfo.LAr

        assert ei.eventFlags (LAr) == exp_ef
        assert ei.errorState (LAr) == exp_err
        self.check_and_reset_cells (ccc, exp_errs)
        return StatusCode.Success


    def rec_feb_error_summary (self, errs):
        summary = ROOT.LArFebErrorSummary()
        for (idtup, err) in errs:
            febid = self.onlineID.feb_Id (*idtup)
            ifeb = febid.get_identifier32().get_compact()
            summary.set_feb_error (ifeb, err)
        if not self.evtStore.record (summary, 'LArFebErrorSummary'):
            return False
        ROOT.SetOwnership (summary, False)
        return True


    def check_and_reset_cells (self, ccc, errs):
        badcells = self.expand_errors (errs)
        for c in ccc:
            i = c.caloDDE().calo_hash().value()
            err = badcells.get(i)
            if err == None:
                assert c.energy() == i
                assert c.time() == i+0.5
                assert c.quality() == 10
                assert c.provenance() == 11
            else:
                assert c.energy() == 0
                assert c.time() == 0
                assert c.quality() == 0
                assert c.provenance() == 0x800 + 11

                c.setEnergy (i)
                c.setTime (i+0.5)
                c.setQuality (10)
                c.setProvenance (11)
        return


    def expand_errors (self, errs):
        badcells = {}
        for (feb, err) in errs:
            for chan in range(128):
                chanid = self.onlineID.channel_Id (feb[0],
                                                   feb[1],
                                                   feb[2],
                                                   feb[3],
                                                   chan)
                cellid = self.cabling.cnvToIdentifier (chanid)
                cellhash = self.offlineID.calo_cell_hash (cellid)
                badcells[cellhash.value()] = err
        return badcells


    

tool1 = LArBadFebMaskingTool ('tool1')
ToolSvc += tool1

tool2 = LArBadFebMaskingTool ('tool2')
ToolSvc += tool2

tool3 = LArBadFebMaskingTool ('tool3')
ToolSvc += tool3

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg()
topSequence += TestAlg ('testalg1')

