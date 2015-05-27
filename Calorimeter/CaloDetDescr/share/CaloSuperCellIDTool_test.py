# $Id$
#
# @file CaloDetDescr/share/CaloSuperCellIDTool_test.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2012
# @brief Regression tests for CaloSuperCellIDTool.
#

# Work around strange Gaudi problem: if we defer this until later, we get
# this in the log:
#    Error in <TCint::AutoLoad>: failure loading library libGaudiKernelDict for class StatusCode
#
import cppyy
import ROOT
ROOT.StatusCode

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )

from AthenaPython import PyAthena
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from CaloDetDescr.CaloDetDescrConf import CaloSuperCellIDTool
tt = CaloSuperCellIDTool()
tt.OutputLevel = DEBUG
ToolSvc += tt


LAREM   = PyAthena.CaloCell_ID.LAREM
LARHEC  = PyAthena.CaloCell_ID.LARHEC
LARFCAL = PyAthena.CaloCell_ID.LARFCAL
TILE    = PyAthena.CaloCell_ID.TILE


def toiter (range):
    beg = range.begin()
    end = range.end()
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return


class CaloSuperCellIDToolTest (PyAthena.Alg):
    cell_sc_tests = {
        LAREM :
        [[(1, 1, 0, 10, 5), (1, 1, 0, 1, 5)],
         [(2, 1, 0,  0, 5), (2, 1, 0, 0, 5)],
         [(2, 1, 1,  0, 5), (2, 1, 0, 0, 5)],
         [(2, 1, 1,  2, 5), (2, 1, 0, 0, 5)],
         [(3, 1, 0,  4, 5), (3, 1, 0, 2, 2)],
         [(3, 1, 0,  6, 5), (3, 1, 1, 0, 2)],
         ],

        LARHEC :
        # side samp reg eta phi
        [[( 2, 0, 0, 4, 12), ( 2, 0, 0, 4, 12)],
         [( 2, 1, 1, 2, 15), ( 2, 0, 1, 2, 15)],
         [(-2, 2, 0, 6, 16), (-2, 0, 0, 6, 16)],
         [(-2, 3, 1, 1, 17), (-2, 0, 1, 1, 17)],
         ],

        # side mod (reg) eta phi
        LARFCAL :
        [[(2, 1, 0,  1, 2), (2, 1, 999999,  0, 2)],
         [(2, 2, 0,  2, 3), (2, 2, 999999,  0, 3)],
         [(2, 3, 0, 10, 4), (2, 3, 999999,  2, 4)],

         [(-2, 1, 0, 20, 2), (-2, 1, 999999,  1, 2)],
         [(-2, 2, 0, 20, 5), (-2, 2, 999999,  5, 5)],
         [(-2, 3, 0, 10, 6), (-2, 3, 999999,  2, 6)],
         ],

        # section side module tower sample
        TILE :
        [[(1, 1, 3,  4, 0), (1, 1, 3,  4, 0)],
         [(2, 1, 3, 12, 1), (2, 1, 3, 12, 0)],
         [(1, 1, 3,  4, 2), (1, 1, 3,  4, 2)],
         [(1, 1, 3,  8, 2), (1, 1, 3,  8, 2)],
         [(1, 1, 3,  9, 0), (1, 1, 3,  8, 0)],
         [(3, 1, 3,  9, 1), (2, 1, 3,  9, 0)],
         [(3, 1, 3,  8, 2), (2, 1, 3,  9, 2)],
         [(3, 1, 3, 10, 3), (999999,999999,999999,999999,999999)],

         [(1, -1, 3,  4, 0), (1, -1, 3,  4, 0)],
         [(2, -1, 3, 12, 1), (2, -1, 3, 12, 0)],
         [(1, -1, 3,  4, 2), (1, -1, 3,  4, 2)],
         [(1, -1, 3,  8, 2), (1, -1, 3,  8, 2)],
         [(1, -1, 3,  9, 0), (1, -1, 3,  8, 0)],
         [(3, -1, 3,  9, 1), (2, -1, 3,  9, 0)],
         [(3, -1, 3,  8, 2), (2, -1, 3,  9, 2)],
         [(3, -1, 3, 10, 3), (999999,999999,999999,999999,999999)],

         ],
        }


    sc_cell_tests = {
        # LArEM
        LAREM :
        [[(1, 1, 0, 10, 5),
          [(1, 1, 0, 80, 5), (1, 1, 0, 81, 5), (1, 1, 0, 82, 5), (1, 1, 0, 83, 5), (1, 1, 0, 84, 5), (1, 1, 0, 85, 5), (1, 1, 0, 86, 5), (1, 1, 0, 87, 5)]],
         [(1, 1, 0, 0, 5),
          [(1, 1, 0, 1, 5), (1, 1, 0, 2, 5), (1, 1, 0, 3, 5), (1, 1, 0, 4, 5), (1, 1, 0, 5, 5), (1, 1, 0, 6, 5), (1, 1, 0, 7, 5)]],
         [(1, 3, 0, 10, 20),
          [(1, 3, 0, 20, 80), (1, 3, 0, 20, 81), (1, 3, 0, 20, 82), (1, 3, 0, 20, 83), (1, 3, 0, 21, 80), (1, 3, 0, 21, 81), (1, 3, 0, 21, 82), (1, 3, 0, 21, 83)]],
         [(3, 1, 0, 1, 20),
          [(3, 1, 0, 2, 40), (3, 1, 0, 2, 41), (3, 1, 0, 3, 40), (3, 1, 0, 3, 41)]],
         [(3, 1, 1, 0, 20),
          [(3, 1, 0, 6, 40), (3, 1, 0, 6, 41)]],
         [(2, 1, 0, 0, 20),
          [(2, 1, 0, 0, 20), (2, 1, 1, 0, 20), (2, 1, 1, 1, 20), (2, 1, 1, 2, 20)]],
         ],

        LARHEC :
        [[( 2, 0, 1, 2, 15),
          [(2, 0, 1, 2, 15), (2, 1, 1, 2, 15), (2, 2, 1, 2, 15), (2, 3, 1, 2, 15)]],

         [(-2, 0, 1, 1, 17),
          [(-2, 0, 1, 1, 17), (-2, 1, 1, 1, 17), (-2, 2, 1, 1, 17), (-2, 3, 1, 1, 17)]],
         ],

        LARFCAL :
        [[(2, 1, 0,  0, 2),
          [(2, 1, 999999, 0, 2), (2, 1, 999999, 1, 2), (2, 1, 999999, 2, 2), (2, 1, 999999, 3, 2), (2, 1, 999999, 4, 2), (2, 1, 999999, 5, 2), (2, 1, 999999, 6, 2), (2, 1, 999999, 7, 2), (2, 1, 999999, 8, 2), (2, 1, 999999, 9, 2), (2, 1, 999999, 10, 2), (2, 1, 999999, 11, 2), (2, 1, 999999, 12, 2), (2, 1, 999999, 13, 2), (2, 1, 999999, 14, 2), (2, 1, 999999, 15, 2)]],

         [(-2, 2, 0,  5, 5),
          [(-2, 2, 999999, 20, 5), (-2, 2, 999999, 21, 5), (-2, 2, 999999, 22, 5), (-2, 2, 999999, 23, 5)]],

         [(2, 3, 0,  2, 4),
          [(2, 3, 999999, 8, 4), (2, 3, 999999, 9, 4), (2, 3, 999999, 10, 4), (2, 3, 999999, 11, 4)]],
         ],

        # section side module tower sample
        TILE :
        [[(1, 1, 3,  4, 0),
          [(1, 1, 3, 4, 0), (1, 1, 3, 4, 1), (1, 1, 3, 4, 2)]],
         [(1, 1, 3,  4, 2),
          [(1, 1, 3, 4, 2)]],
         [(1, 1, 3,  5, 0),
          [(1, 1, 3, 5, 0), (1, 1, 3, 5, 1)]],

         [(1, 1, 3,  8, 0),
          [(1, 1, 3, 9, 0), (1, 1, 3, 8, 0), (1, 1, 3, 8, 1)]],
         [(1, 1, 3,  8, 2),
          []],
         [(1, 1, 3,  9, 0),
          [(3, 1, 3, 8, 2), (3, 1, 3, 9, 1)]],
         [(1, 1, 3,  9, 2),
          [(3, 1, 3, 8, 2)]],
         [(2, 1, 3,  10, 0),
          [(2, 1, 3, 10, 1), (2, 1, 3, 10, 2)]]
         ],
        }
        


    def __init__ (self, name):
        PyAthena.Alg.__init__ (self, name)
        self.calocell_helper = None
        self.calosc_helper = None
        self.tool = None
        return


    def initialize (self):
        return 1


    def init (self):
        if self.tool != None:
            return
        sg = PyAthena.py_svc('StoreGateSvc')
        ds = PyAthena.py_svc('DetectorStore')
        self.calocell_helper = ds['CaloCell_ID']
        self.calosc_helper = ds['CaloCell_SuperCell_ID']
        self.tool = PyAthena.py_tool ('CaloSuperCellIDTool',
                                      iface='ICaloSuperCellIDTool')
        return
        


    def execute (self):
        self.init()
        self.run_cell_sc_tests()
        self.run_sc_cell_tests()

        for subcalo in [LAREM, LARFCAL, LARHEC]:
            self.test_subcalo1 (subcalo)
            self.test_subcalo2 (subcalo)
            self.region_tests (subcalo)
        self.test_subcalo2 (TILE)
        return 1


    def unpack (self, id, helper):
        if helper.is_tile(id):
            return (helper.section(id), helper.side(id), helper.module(id),
                    helper.tower(id), helper.sample(id))
        return (helper.pos_neg(id), helper.sampling(id), helper.region(id),
                helper.eta(id), helper.phi(id))


    def unpack_list (self, idlist, helper):
        return [self.unpack (id, helper) for id in idlist]


    def run_cell_sc_tests (self):
        calocell_helper = self.calocell_helper
        calosc_helper = self.calosc_helper
        tool = self.tool
        
        for (subcalo, tests) in self.cell_sc_tests.items():
            for (cell_tuple, sc_tuple) in tests:
                cell_id = calocell_helper.cell_id (subcalo, *cell_tuple)
                sc_id = tool.offlineToSuperCellID (cell_id)
                if self.unpack (sc_id, calosc_helper) != sc_tuple:
                    print 'ERROR cell->sc', subcalo, cell_tuple, sc_tuple, \
                          self.unpack (sc_id, calosc_helper)
                    assert False
        return


    def run_sc_cell_tests (self):
        calocell_helper = self.calocell_helper
        calosc_helper = self.calosc_helper
        tool = self.tool
        
        for (subcalo, tests) in self.sc_cell_tests.items():
            for (sc_tuple, cell_list) in tests:
                sc_id = calosc_helper.cell_id (subcalo, *sc_tuple)
                idlist = tool.superCellToOfflineID (sc_id)
                if self.unpack_list (idlist, calocell_helper) != cell_list:
                    print 'ERROR sc->cell', subcalo, sc_tuple, cell_list, \
                          self.unpack_list (idlist, calocell_helper)
                    assert False
        return


    def get_hash_range (self, subcalo, helper):
        hmin = PyAthena.IdentifierHash()
        hmax = PyAthena.IdentifierHash()
        helper.calo_cell_hash_range (subcalo, hmin, hmax)
        hmax = hmax.value()
        hmin = hmin.value()
        return (hmin, hmax)


    # cell->supercell->cell
    def test_subcalo1 (self, subcalo):
        calosc = self.calosc_helper
        calocell = self.calocell_helper
        t = self.tool

        (hmin, hmax) = self.get_hash_range (subcalo, calosc)
        hvec = [0] * (hmax - hmin)

        for cell_id in toiter (calocell.cell_range (subcalo)):
            sc_id = t.offlineToSuperCellID (cell_id)
            if not sc_id.is_valid():
                if calocell.is_tile_gap (cell_id): continue
                if (calocell.is_em_barrel (cell_id) and
                    calocell.calo_sample (cell_id) == 0 and
                    calocell.eta (cell_id) == 60):
                    continue
                print 'ERROR unmapped cell subcalo', \
                      calocell.sub_calo (cell_id), \
                      'sample', calocell.calo_sample (cell_id),\
                      'posneg', calocell.pos_neg (cell_id),\
                      'region', calocell.region (cell_id),\
                      'eta', calocell.eta (cell_id),\
                      'phi', calocell.phi (cell_id)
                assert False
            hvec[calosc.calo_cell_hash(sc_id).value() - hmin] = 1
            idlist = t.superCellToOfflineID (sc_id)
            for id2 in idlist:
                if cell_id == id2: break
            else:
                raise Exception ("Didn't find cell")

        for i in range(len(hvec)):
            if hvec[i] == 0:
                raise Exception ("Didn't map sc hash %d" % i)
        return


    # supercell->cell->supercell
    def test_subcalo2 (self, subcalo):
        calosc = self.calosc_helper
        calocell = self.calocell_helper
        t = self.tool

        (hmin, hmax) = self.get_hash_range (subcalo, calocell)
        hvec = [0] * (hmax - hmin)

        for sc_id in toiter (calosc.cell_range (subcalo)):
            idlist = t.superCellToOfflineID (sc_id)
            for id in idlist:
                hvec[calocell.calo_cell_hash(id).value() - hmin] = 1
                assert id.is_valid()
                sc_id2 = t.offlineToSuperCellID (id)
                if calosc.is_tile(sc_id):
                    assert calosc.section(sc_id) == calosc.section(sc_id2)
                    assert calosc.side(sc_id) == calosc.side(sc_id)
                    assert calosc.module(sc_id) == calosc.module(sc_id)
                    assert calosc.tower(sc_id) == calosc.tower(sc_id)
                    samp1 = calosc.sample(sc_id)
                    samp2 = calosc.sample(sc_id2)
                    assert samp2 == 2 or samp1 == samp2
                else:
                    assert sc_id == sc_id2

        for i in range(len(hvec)):
            if hvec[i] == 0:
                h = PyAthena.IdentifierHash (i)
                id = calocell.cell_id (subcalo, h)
                if calocell.is_tile_gap(id): continue
                if (calocell.is_em_barrel (id) and
                    calocell.calo_sample (id) == 0 and
                    calocell.eta (id) == 60):
                    continue
                raise Exception ("Didn't map cell hash %d" % i)
        return


    def reg_minmax (self, reg_id, helper):
        eta0 = helper.eta0 (reg_id)
        neta = helper.eta_max (reg_id) - helper.eta_min (reg_id) + 1
        return (eta0, eta0 + neta * helper.etaGranularity (reg_id))
    
    def region_tests (self, subcalo):
        calosc = self.calosc_helper
        calocell = self.calocell_helper
        t = self.tool

        for calo_reg_id in toiter (calocell.reg_range (subcalo)):
            idlist = t.offlineToSuperCellRegion (calo_reg_id)
            for sc_reg_id in idlist:
                idlist2 = t.superCellToOfflineRegion (sc_reg_id)

                assert calo_reg_id in list(idlist2)

                assert calocell.sub_calo(calo_reg_id) == calosc.sub_calo(sc_reg_id)
                assert calocell.pos_neg(calo_reg_id) == calosc.pos_neg(sc_reg_id)
                if subcalo == LARHEC:
                    assert calosc.sampling(sc_reg_id) == 0
                else:
                    assert calocell.sampling(calo_reg_id) == calosc.sampling(sc_reg_id)
                assert not calocell.is_supercell (calo_reg_id)
                assert calosc.is_supercell (sc_reg_id)

                # Make sure there's overlap.
                (calo_min, calo_max) = self.reg_minmax (calo_reg_id, calocell)
                (sc_min, sc_max)     = self.reg_minmax (sc_reg_id,   calosc)

                ovmin = max (sc_min, calo_min)
                ovmax = min (sc_max, calo_max)
                assert (ovmin < ovmax)
        return
                



topSequence += [CaloSuperCellIDToolTest('test')]
theApp.EvtMax = 1


