# $Id$
#
# @file CaloTriggerTool/share/CaloSuperCellMgrDetDescrCnv_test.py
# @author scott snyder <snyder@bnl.gov>
# @date Sep, 2012
# @brief Regression tests for CaloSuperCellMgrDetDescrCnv.
#

# Work around strange Gaudi problem: if we defer this until later, we get
# this in the log:
#    Error in <TCint::AutoLoad>: failure loading library libGaudiKernelDict for class StatusCode
#
import cppyy 
import ROOT
ROOT.StatusCode

from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.LAr_setOn()
DetFlags.detdescr.Tile_setOn()


import sys
import string
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry
include( "CaloConditions/CaloConditions_jobOptions.py" ) 
include( "TileIdCnv/TileIdCnv_jobOptions.py" )

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
ServiceMgr += GeoModelSvc()
theApp.CreateSvc += [ "GeoModelSvc"]
from AtlasGeoModel import LArGM
from AtlasGeoModel import TileGM

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc().GlobalTag = 'OFLCOND-SDR-BS14T-IBL-06'


from AthenaPython import PyAthena
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

TILE    = PyAthena.CaloCell_ID.TILE
LARFCAL = PyAthena.CaloCell_ID.LARFCAL


def toiter (range):
    beg = range.begin()
    end = range.end()
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return


def get_descriptors (reg_id, mgr):
    if not mgr.getCaloCell_ID().is_tile (reg_id):
        return [mgr.get_descriptor (reg_id)]
    out = []
    for d in mgr.tile_descriptors_range():
        if d.identify() == reg_id:
            out.append (d)
    return out
    

class CaloSuperCellDetDescrManagerCnvTest (PyAthena.Alg):
    def __init__ (self, name):
        PyAthena.Alg.__init__ (self, name)
        return


    def initialize (self):
        sg = PyAthena.py_svc('StoreGateSvc')
        ds = PyAthena.py_svc('DetectorStore')
        self.idol_helper = ds['CaloCell_ID']
        self.idsc_helper = ds['CaloCell_SuperCell_ID']
        self.tool = PyAthena.py_tool ('CaloSuperCellIDTool',
                                      iface='ICaloSuperCellIDTool')
        self.mgrol = ds['CaloMgr']
        self.mgrsc = ds['CaloSuperCellMgr']
        return 1


    def execute (self):
        for d in toiter (self.mgrsc.calo_descriptors_range()):
            if not d: continue #xxx
            self.dump_descriptor (d)
            print
        for d in toiter (self.mgrsc.tile_descriptors_range()):
            if not d: continue #xxx
            self.dump_descriptor (d)
            print
        self.dump_tile_cells()
        self.dump_fcal_cells()
        return 1


    def dump_descriptor (self, desc, f = sys.stdout):
        idsc = self.idsc_helper
        idol = self.idol_helper
        mgrol = self.mgrol

        id = desc.identify()
        idlist = self.tool.superCellToOfflineRegion (id)


        def pr_line (typ, d):
            print >> f, '%s %2d ' % (typ, d.calo_hash().value()),
            return

        def posneg_or_side (id, helper):
            if helper.is_tile(id):
                return helper.side(id)
            return helper.pos_neg(id)
        def sampling_or_sample (id, helper):
            if helper.is_tile(id):
                return helper.sample(id)
            return helper.sampling(id)
        def region_or_section (id, helper):
            if helper.is_tile(id):
                return helper.section(id)
            return helper.region(id)
        def pr_id (typ, d, helper):
            pr_line (typ, d)
            print >> f, ('subcal %d BE %2d samp %d reg %d sc/sg/ly/sm %d/%2d/%d/%d flgs %d%d%d%d%d%d%d%d' %
                         (helper.sub_calo(id),
                          posneg_or_side(id, helper),
                          sampling_or_sample(id, helper),
                          region_or_section(id, helper),
                          d.getSubCalo(),
                          d.calo_sign(),
                          d.layer(),
                          d.getSampling(),
                          d.is_lar_em(),
                          d.is_lar_em_barrel(),
                          d.is_lar_em_endcap(),
                          d.is_lar_em_endcap_inner(),
                          d.is_lar_em_endcap_outer(),
                          d.is_lar_hec(),
                          d.is_lar_fcal(),
                          d.is_tile(),
                          ))
            return


        def pr_etaphi (typ, d, helper):
            pr_line (typ, d)
            print >> f, ('calo_eta %7.4f %7.4f calo_phi %7.4f %7.4f de/p %7.4f %7.4f' %
                         (d.calo_eta_min(),
                          d.calo_eta_max(),
                          d.calo_phi_min(),
                          d.calo_phi_max(),
                          d.deta(),
                          d.dphi(),
                          ))
            return

        def pr_rz (typ, d, helper):
            pr_line (typ, d)
            print >> f, ('calo_r %7.2f %7.2f   calo_z %8.2f %8.2f ne/p %3d %3d' %
                         (d.calo_r_min(),
                          d.calo_r_max(),
                          d.calo_z_min(),
                          d.calo_z_max(),
                          d.n_eta(),
                          d.n_phi(),
                          ))
            return

        def pr_lar (typ, d, helper):
            pr_line (typ, d)
            print >> f, ('lar %7.4f %7.4f %7.4f %7.4f' %
                         (d.reg_min(),
                          d.reg_max(),
                          d.lar_eta_min(),
                          d.lar_phi_min(),
                          ))
            return


        def pr_depth (typ, d, helper):
            pr_line (typ, d)
            def get_depth (which):
                v = getattr (ROOT, 'vector<double>')()
                getattr(d, 'get_depth_' + which)(v)
                return '[' + string.join (['%7.2f' % x for x in v], ',') + ']'
            print >> f, ('depth %d %s %s' %
                         (d.n_calo_depth(),
                          get_depth('in'),
                          get_depth('out')))
            return


        def pr_xform (typ, d, helper):
            pr_line (typ, d)
            xf = d.transform()
            r = xf.rotation()
            t = xf.translation()
            ea = r.eulerAngles (2, 0, 2)
            ss = ('xform %f %f %f %f %f %f' %
                  (ea[0], ea[1], ea[2],
                   t[0], t[1], t[2]))
            print >> f, ss.replace ('-0.000000', '0.000000')
            return


        def pr (func):
            func ('SC', desc, idsc)
            descs = []
            for olid in idlist:
                descs += get_descriptors (olid, self.mgrol)
            for d in descs:
                func ('OL', d, idol)
            return

        pr (pr_id)
        pr (pr_etaphi)
        pr (pr_rz)
        pr (pr_lar)
        pr (pr_depth)
        pr (pr_xform)
        return


    def dump_tile_cells1 (self, mgr, idhelper):
        i1 = 0
        i2 = 0
        i3 = 0
        i4 = 0
        for id in toiter (idhelper.cell_range (TILE)):
            i1 += 1
            if idhelper.module(id) != 0: continue
            i2 += 1
            if idhelper.side(id) < 0: continue
            i3 += 1
            if idhelper.sample(id) not in [0, 2]: continue
            i4 += 1
            e = mgr.get_element(id)
            print '%d %2d %d %d %f %f %f %f' % \
                  (idhelper.section(id), \
                   idhelper.tower(id), \
                   idhelper.sample(id), \
                   e.getSampling(), \
                   e.eta(), \
                   e.phi(), \
                   e.deta(), \
                   e.dphi())
        print i1, i2, i3, i4
        return


    def dump_tile_cells (self):
        print
        print 'Supercell tiles'
        self.dump_tile_cells1 (self.mgrsc, self.idsc_helper)
        print
        print 'Offline tiles'
        self.dump_tile_cells1 (self.mgrol, self.idol_helper)
        return


    def print_fcal_cell (self, tag, id, mgr, idhelper):
        elt = mgr.get_element (id)
        print '%2s %2d %1d %2d %5.1f %5.1f %7.1f %5.1f %5.1f %5.1f %5.2f %5.2f %5.2f %5.2f' % \
              (tag,
               idhelper.pos_neg(id),
               idhelper.module(id),
               idhelper.eta(id),
               elt.x(),
               elt.y(),
               elt.z(),
               elt.dx(),
               elt.dy(),
               elt.dz(),
               elt.eta(),
               elt.phi(),
               elt.deta(),
               elt.dphi())
        return

    
    def dump_fcal_cells (self):
        print
        print 'FCAL cells'

        idsc_helper = self.idsc_helper.fcal_idHelper()
        idol_helper = self.idol_helper.fcal_idHelper()

        for id in toiter (self.idsc_helper.cell_range (LARFCAL)):
            if idsc_helper.phi(id) != 1: continue
            self.print_fcal_cell ('SC', id, self.mgrsc, idsc_helper)
            idlist = self.tool.superCellToOfflineID (id)
            for id_ol in idlist:
                self.print_fcal_cell ('  ', id_ol, self.mgrol, idol_helper)
            pass
    
    
    

alg = CaloSuperCellDetDescrManagerCnvTest('test')
topSequence += [alg]
theApp.EvtMax = 1


# For when testing by hand.
def init():
    theApp.initialize()
    theApp.nextEvent()
    global sg, ds, m, mm
    sg = PyAthena.py_svc('StoreGateSvc')
    ds = PyAthena.py_svc('DetectorStore')
    mm=ds['CaloMgr']
    m=ds['CaloSuperCellMgr']
    return
