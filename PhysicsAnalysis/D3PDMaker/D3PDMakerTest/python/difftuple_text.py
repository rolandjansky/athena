# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id$
#
# File: D3PDMakerTest/python/difftuple_text.py
# Author: snyder@bnl.gov
# Date: Oct, 2010
# Purpose: Diff a root tuple file against a text dump.
#


import operator
import os
import types
import string
from fnmatch import fnmatch
from D3PDMakerTest.split_list import split_list, split_list1

ignore_event_differences = False


# NEW:OLD
renames = {
    }

rename_keys = {
    # Hack for ordering difference when scell tuple is on.
    'caloMeta/EventInfoD3PDObject_000005' : 'caloMeta/EventInfoD3PDObject_000004',
    'physicsMeta/EventInfoD3PDObject_000003' : 'physicsMeta/EventInfoD3PDObject_000002',
    'physicsTrigDecMeta/EventInfoD3PDObject_000004' : 'physicsTrigDecMeta/EventInfoD3PDObject_000003',
    }

def atlasProduction(ver):
    return (ver and
            (ver.find('17.2')>=0 or
             ver.find('17.3')>=0) and
            #ver.find('AtlasProduction')>=0 and
            ver.find('AtlasPhysics')==-1 and
            ver.find('usatlas+u/snyder')==-1)


# List of branches to ignore for various versions.
# Format is a list of tuples: (V, V, V, [BRANCHES...])
# We consider the strings V in turn and see if they're contained
# in CMTPATH.  If the first V is `-', then we ignore the branches
# if there are any matches.  Otherwise, we ignore the branches
# unless there are any matches.
# If V is callable, then instead of looking for at match in CMTPATH,
# we call it with CMTPATH as an argument.
# The list of branches can include glob patterns.
_ignore_branches = [
    # Changed in devval.
    ('17.2',
     ['StreamRDO_ref',
      'StreamESD_ref',
      ]),
    
    # Changed by TriggerMenuPython in devval.
    ('17.2',
    ['trig_RoI_*',
     'trig_Nav_*']),
    
    # Changed in devval.
    ('17.2',
     ['trk_blayerPrediction_*',
      ]),
    # This also differs between 32/64 bits.
    # For now, we're generating reference files on 32-bits, so ignore
    # on 64-bits.
    ('-', 'x86_64',
     ['trk_blayerPrediction_*',
      ]),

    # TauDiscriminant changes not in AtlasProduction.
    ('-', atlasProduction,
     ['tau_*',
      ]),

    # MissingET-03-03-41 not in AtlasProduction.
    # Newer version in devval.
    ('-', atlasProduction, '/dev', '/18', '17.7', '17.X', '/mig0', 'localbuild',
     ['MET_RefFinal_STVF_*',
      ]),


    # Changed in TauDiscriminant-01-07-28 in devval.
    ('17.2',
     ['tau_EleBDTTight',
      ]),

    # Changed in devval and in 17.2.9.
    ('17.2.9',
     ['mu*_SpaceTime_*',
      ]),

    # Changed in devval --- TMVA change in root 34?
    ('17.2',
     ['ph_NN_passes',
      'ph_NN_discriminant',
      ]),

    # Changed by CaloClusterCorrection-00-07-74 in devval.
    ('17.2',
     ['*_rawcl_pt',
      '*_rawcl_eta',
      ]),
    
    # TriggerD3PDMaker-00-01-99, TrigAnalysisInterfaces-00-02-00,
    # TrigBunchCrossingTool-00-03-10, TriggerMenuNtuple-00-01-47
    # in AtlasPhysics but not AtlasProduction...
    ('-', atlasProduction,
     ['UnpairedBunches',
      'UnpairedBunchIntensities',
      'BunchIntensities',
      'FilledBunchIntBeam1',
      'FilledBunchIntBeam2',
      'UnpairedBeam1',
      'UnpairedBeam2',
      'UnpairedIntBeam1',
      'UnpairedIntBeam2',
      'trig_RoI_*',
      'trig_Nav_*',
      ]),

    # Changed in devval.
    ('17.2',
     ['StreamRAW_ref']),

    # Jet changes, not in AtlasProduction.
    ('-', atlasProduction,
     ['jet_*ActiveArea*',
      'jet_*_jvtx_x',
      'jet_*_jvtx_y',
      'jet_*_jvtx_z',
      'jet_*_jvtxfFull',
      'jet_*_jvtxf',
      'jet_*_nTrk',
      'jet_*_nTrk_pv0_500MeV',
      'jet_*_sumPtTrk',
      'jet_*_sumPtTrk_pv0_500MeV',
      'jet_*_flavor_component_ip3d_pc',
      'jet_*_flavor_component_sv1_pc',
      'jet_OriginIndex',
      'jet_*TruthMF',
      'jet_*TruthMFindex',
      'jet_*CENTER_LAMBDA',
      'jet_*SECOND_LAMBDA',
      'jet_*SECOND_R',
      'jet_*CLUSTER_PT',
      'jet_antikt4truth_isBad*',
      'jet_antikt4h1tower_isBad*',
      'jet_antikt4h1topo_isBad*',
      'jet_antikt6h1tower_isBad*',
      ]),

    # Changed by TrkGeometry-00-23-05 in devval.
    ('17.2',
     ['*_materialTraversed',
      ]),

    # Jet changes in devval.
    ('17.2',
     ['jet_*_ActiveArea*',
      'jet_*_TruthMFindex',
      'jet_*_TruthMF',
      'jet_*_jvtxf',
      'jet_*_jvtxfFull',
      'jet_*_isBad*',
      'jet_*_nTrk',
      'jet_*_sumPtTrk',
      'jet_*_jvtx_x',
      'jet_*_jvtx_y',
      'jet_*_jvtx_z',
      'jet_*CENTER_LAMBDA',
      'jet_*SECOND_LAMBDA',
      'jet_*SECOND_R',
      'jet_*CLUSTER_PT',
      ]),

    # Pool changes in devval.
    ('17.2',
     ['Token',
      ]),

    # Change by det descr bugfix in devval.
    ('17.2',
     ['ph_rawcl_etas1']),

    # Changed by TrkExTools-02-37-82 in devval.
    ('17.2',
     ['mu_curvatureSig',
      'mu_id_curvatureSig']),

    # Changed in devval.
    ('17.2',
     ['mu_*neighbourSig']),

    # Changed in devval.
    ('17.2',
     ['trig_L2_passedThrough',
      'trig_L2_resurrected',
      'trig_L2_passedPhysics',
      'trig_L2_passedRaw',
      'trig_EF_passedThrough',
      'trig_EF_resurrected',
      'trig_EF_passedPhysics',
      'trig_EF_passedRaw',
      'HLTRerunPrescaleMap',
      ]),

    # Changed in devval.
    ('17.2',
     ['trig_roidescriptor_forID_etaMinus',
      'trig_roidescriptor_forID_etaPlus',
      ]),

    # Changed in devval.
    ('17.2',
     ['el_jettrack_*',
      'el_trackd0pvunbiased',
      'el_tracksigd0pvunbiased',
      'el_trackz0pvunbiased',
      'el_tracksigz0pvunbiased',
      'el_trackIPEstimate*',
      'trk_*_wrtBL',
      'mu*_spec_surf_*',
      ]),
    ]
    

_ignore_keys = [
    # TauDiscriminant-01-07-19 / TauD3PDMaker-01-08-19 not in 17.2.X.Y.
    ('-', atlasProduction, 
     ['*Meta/TauD3PDObject_*']),


    # JetTagD3PDMaker-00-01-89 not in AtlasProduction.
    ('-', atlasProduction, 
     ['*Meta/JetD3PDObject_*']),


    # Types change when moving to 64-bit ids.
    ('17.2',
     ['trackMeta/PixelClusterD3PDObject_*']),
    ]



def get_ver():
    ver = os.environ.get ('CMTPATH')
    if ver:
        ver = string.split (ver, ':')
        ver = [s for s in ver if s.find('AtlasSetup') < 0]
        ver.append (os.environ.get ('CMTCONFIG'))
        ver = string.join (ver, ':')
    else:
        ver = 'localbuild'
    return ver
    

def ignore_p (match, ignore_list):
    ver = get_ver()

    for vlist in ignore_list:
        vv = vlist[:-1]
        ignore = True
        if vv and vv[0] == '-':
            ignore = False
            vv = vv[1:]
        for v in vv:
            if (callable(v) and v(ver) or
                not callable(v) and ver and ver.find (v) >= 0):
                ignore = not ignore
                break
        if not ignore: continue
        for b in vlist[-1]:
            if fnmatch (match, b):
                return True
    return False


# Should a branch be ignored?
def ignore_br_p (br):
    return ignore_p (br, _ignore_branches)


def ignore_key_p (key):
    return ignore_p (key, _ignore_keys)


def read_val (val):
    if not val: return val

    if val.find ('+') >= 0:
      psplit = split_list1 (val, '+')
      if len(psplit) > 1:
      #psplit = split_list1 (val, '+')
      #if len(psplit) > 1:
        good = True
        for i in range(len(psplit)):
            psplit[i] = read_val (psplit[i])
            if type(psplit[i]) != type([]):
                good = False
                break
        if good:
            return reduce (operator.__add__, psplit)

    if val[-1] == 'L':
        try:
            return long(val)
        except TypeError:
            pass

    if val[0] in '0123456789-':
        if val.find ('.') < 0:
            try:
                return int(val)
            except TypeError:
                pass
            except ValueError:
                pass

        try:
            return float (val)
        except TypeError:
            pass
        except ValueError:
            pass

    try:
        (count, elt) = val.split ('*', 1)
        count = int (count)
        if elt.startswith('[') and elt.endswith(']'):
            return count * [read_val(elt[1:-1])]
    except ValueError:
        pass

    if val[0] == '[':
        return [read_val(s) for s in split_list(val)]
    return val


class Reader (object):
    def __init__ (self, f):
        self.saved = None
        self.f = f
        return


    def getline (self):
        if self.saved != None:
            out = self.saved
            self.saved = None
            return out
        l = self.f.readline()
        if not l:
            return None
        if len(l) > 0 and l[-1] == '\n':
            l = l[:-1]
        return l


    def ungetline (self, l):
        assert self.saved == None
        self.saved = l
        return


class Tree (object):
    def __init__ (self, reader):
        self.dummy = False
        self.reader = reader
        self.branches = {}
        self.read()
        return


    def read (self):
        while True:
            l = self.reader.getline()
            if l == None: break
            if not l: continue
            if l == '[Dummy tree skipped]':
                self.dummy = True
                break
            if l.startswith ('Branches'):
                l = self.reader.getline()
                if not l.startswith ('-------'):
                    self.reader.ungetline (l)
                    break
                self.read_by_branches()
                return
                
            if not l.startswith ('Event '):
                self.reader.ungetline (l)
                break
            iev = int(l.split()[1])
            l = self.reader.getline()
            if not l.startswith ('-------'):
                self.reader.ungetline (l)
                break
            self.read_branches (iev)
        return


    def read_by_branches (self):
        lasttag = ''
        while True:
            l = self.reader.getline()
            if not l: break
            if l.startswith (':'):
                lasttag = l[1:]
                continue
            (name, val) = l.split (None, 1)
            if name == '$': name = ''
            name = lasttag + name
            br = self.branches.setdefault (name, {})
            val = read_val (val)
            for (iev, v) in enumerate(val):
                br[iev] = v
        return


    def read_branches (self, iev):
        while True:
            l = self.reader.getline()
            if not l: break
            (name, val) = l.split (None, 1)
            br = self.branches.setdefault (name, {})
            br[iev] = read_val (val)
        return



class String (object):
    def __init__ (self, reader):
        self.val = ''
        while True:
            l = reader.getline()
            if l == None: break
            ipos = l.find ('__END_OF_STRING__')
            if ipos >= 0:
                self.val = self.val + l[:ipos]
                break
            else:
                self.val = self.val + l + '\n'
        return


class Hist (object):
    def __init__ (self, reader):
        l = reader.getline()
        self.bins = []
        self.errs = []
        if l.startswith ('bins:'):
            self.bins = read_val (string.strip (l[5:]))
            l = reader.getline()
            if l.startswith ('errs:'):
                self.errs = read_val (string.strip (l[5:]))
        return


class Dumpreader (object):
    def __init__ (self, f):
        self.reader = Reader (f)
        self.keys = {}
        self.read()
        return


    def read (self):
        while True:
            l = self.reader.getline()
            if l == None: break
            if not l: continue
            if l.find ('wrong interface id') >= 0: continue
            ll = l.split()
            if len(ll) != 2:
                print 'Unknown line', l
                break
            (typ, key) = ll
            if typ == 'Tree':
                self.keys[key] = Tree (self.reader)
            elif typ == 'String':
                self.keys[key] = String (self.reader)
            elif typ == 'TH1':
                self.keys[key] = Hist (self.reader)
            else:
                print 'Unknown type', typ
                break


    def read_tree (self):
        return None


def dictkey_diff (d1, d2, msg, filter = None):
    keys = [k for k in d1.keys() if not d2.has_key(k)]
    if filter: keys = [k for k in keys if not filter(k)]
    if keys:
        keys.sort()
        print msg
        for k in keys:
            print '  ', k
    return


def apply_renames (d1, d2, renames):
    for (knew, kold) in renames.items():
        if (d2.has_key (kold) and not d2.has_key (knew) and
            not d1.has_key (kold) and d1.has_key (knew)):
            d2[knew] = d2[kold]
            del d2[kold]
    return


def diff_string (k, s1, s2):
    if s1.val != s2.val:
        print 'String', k, 'has value', s1.val, \
              'in new file but value', s2.val, 'in reference file'
    return


inttypes = [types.IntType, types.LongType]
def compare (o1, o2, thresh = 1e-6, ithresh = None, eltcmp = None):
    if eltcmp and type(o1) != type([]):
        return eltcmp (o1, o2)
    # Allow comparing int/long int.
    if type(o1) in inttypes and type(o2) in inttypes:
        if o1 < 0: o1 = o1 + (1<<32)
        if o2 < 0: o2 = o2 + (1<<32)
        return o1 == o2
    # Promote int to float if needed.
    if type(o1) in inttypes and type(o2) == type(1.1):
        o1 = float(o1)
    elif type(o2) in inttypes and type(o1) == type(1.1):
        o2 = float(o2)

    if type(o1) != type(o2):
        return False
    if type(o1) == type([]):
        if len(o1) != len(o2):
            return False

        # Inline this to speed up this common case.
        if len(o1) > 0 and type(o1[0]) == type(1.1) and not ithresh and not callable(thresh):
            xabs = abs
            for i in range(len(o1)):
                x1 = o1[i]
                x2 = o2[i]
                ax1 = xabs(x1)
                ax2 = xabs(x2)
                num = x1-x2
                den = ax1 + ax2
                if den == 0: continue
                x = xabs(num / den)
                if x > thresh:
                    print 'fnmismatch', x1, x2, x, thresh
                    return False
            return True

        for i in range(len(o1)):
            if not compare (o1[i], o2[i],
                            thresh=thresh,
                            ithresh=ithresh,
                            eltcmp=eltcmp):
                return False
        return True
    if type(o1).__name__ in ['map<string,int>',
                             'map<string,float>',
                             'map<string,string>']:
        return ROOT.D3PDTest.MapDumper.equal (o1, o2)
    if type(o1) == type(1.1):
        if callable(ithresh):
            ret = ithresh(o1, o2)
            if ret == True or ret == False: return ret
        elif ithresh and abs(o1) < ithresh and abs(o2) < ithresh:
            return True
        num = o1-o2
        den = abs(o1)+abs(o2)
        if den == 0: return True
        x = abs(num / den)
        if callable(thresh): thresh = thresh(den)
        if x > thresh:
            print 'fmismatch', o1, o2, x, thresh
            return False
        return True
    return o1 == o2


def mc_eta_thresh (x):
    if x > 36: return 1e-2
    if x > 34: return 2e-3
    if x > 32: return 2e-4
    if x > 30: return 1e-4
    if x > 28: return 1e-5
    return 1e-6


def eg_pid_compare (o1, o2):
    if o1 == 0xffffffff and o2 == 0: return True
    return None


def compare_detElementId (o1, o2):
    if o1 == o2: return True
    if o1 == (o2 << 32): return True
    return False


def compare_pixid (o1, o2):
    if o1 == o2: return True
    o2a = ((o2&0x7fffffff)-1)<<28
    if o1 == o2a : return True
    return False


branch_thresh = [

    ['mu*_cov_theta_qoverp_exPV'       , (2e-2, 2e-14)],
    ['mu_cov_phi_exPV'                 , 4e-4],
    ['mu*_cov_*_exPV'                  , 1e-2],
    ['mu*_d0_exPV'                     , 5e-3],
    ['mu*_z0_exPV'                     , 2e-5],
    ['mu*_theta_exPV'                  , 1e-5],
    ['mu*_spec_surf_*'                 , 6e-2],
    ['mu*_trackz0pv'                   , 2e-6],
    ['mu*_trackd0pv'                   , 1e-5],
    ['mu*_trackz0pvunbiased'           , 2e-6],
    ['mu*_trackd0pvunbiased'           , 1e-5],
    ['mu*_trackd0beam'                 , 2e-5],
    ['mu*_trackIPEstimate_d0_unbiasedpvunbiased', 1e-5],
    ['mu*_trackIPEstimate_z0_unbiasedpvunbiased', 1e-5],
    ['mu*_trackIPEstimate_d0_biasedpvunbiased', 1e-5],
    ['mu*_trackIPEstimate_z0_biasedpvunbiased', 1e-5],

    ['trk_z0_wrtPV'                    , 2e-4],
    ['trk_d0_wrtPV'                    , 2e-4],
    ['trk_phi_wrtPV'                   , 1e-4],

    ['trk_err_theta_wrtPV'             , 1e-5],
    ['trk_err_phi_wrtPV'               , 2e-5],

    ['trk_phi_wrtBL'                   , 2e-2],
    ['trk_z0_wrtBL'                    , 1],
    ['trk_d0_wrtBL'                    , 3e-1],

    ['trk_theta_err_wrtBL'             , 3e-2],
    ['trk_qoverp_err_wrtBL'            , 1e-4],
    ['trk_phi_err_wrtBL'               , 3e-2],
    ['trk_z0_err_wrtBL'                , 6e-2],
    ['trk_d0_err_wrtBL'                , 7e-2],

    ['trk_d0_phi_err_wrtBL'            , 1e-1],
    ['trk_d0_qoverp_err_wrtBL'         , 4e-3],
    ['trk_d0_theta_err_wrtBL'          , 1],
    ['trk_d0_z0_err_wrtBL'             , 1],
    ['trk_phi_theta_err_wrtBL'         , 1],
    ['trk_phi_qoverp_err_wrtBL'        , 4e-3],
    ['trk_theta_qoverp_err_wrtBL'      , 1],
    ['trk_z0_phi_err_wrtBL'            , 1],
    ['trk_z0_qoverp_err_wrtBL'         , 1],
    ['trk_z0_theta_err_wrtBL'          , 8e-2],

    ['trk_d0_wrtBS'                    , 2e-4],
    ['trk_z0_wrtBS'                    , 5e-5],
    ['trk_phi_wrtBS'                   , 5e-5],

    ['trk_err_theta_wrtBS'             , 1e-5],
    ['trk_err_phi_wrtBS'               , 2e-5],

    ['trk_cov_theta_qoverp_wrtBS'      , (4e-1, 1e-14)],
    ['trk_cov_phi_theta_wrtBS'         , 5e-2],
    ['trk_cov_phi_qoverp_wrtBS'        , 4e-5],
    ['trk_cov_z0_qoverp_wrtBS'         , 5e-3],
    ['trk_cov_z0_phi_wrtBS'            , 2e-3],
    ['trk_cov_z0_theta_wrtBS'          , 1e-5],
    ['trk_cov_d0_phi_wrtBS'            , 1e-5],
    ['trk_cov_d0_z0_wrtBS'             , 3e-1],
    ['trk_cov_d0_theta_wrtBS'          , 6e-3],

    ['trk_mc_z0'                       , 4e-2],
    ['trk_mc_d0'                       , 2e-2],
    ['trk_mc_phi'                      , 2e-2],
    ['trk_mc_theta'                    , 1e-3],

    ['vxp_trk_d0'                      , (9e-4, 2e-4)],
    ['vxp_trk_z0'                      , 9e-4],
    ['vxp_trk_phi'                     , 5e-5],
    ['vxp_trk_theta'                   , 2e-5],
    ['vxp_trk_unbiased_d0'             , (1e-3, 2e-4)],
    ['vxp_trk_unbiased_z0'             , 1e-3],

    ['el*_jettrack_m'                   , 6e-6],
    ['el*_jettrack_reducedPt'           , 2e-6],
    ['el*_jettrack_ptrel'               , 1e-5],
    ['el*_jettrack_dr'                  , 1e-5],
    ['el*_jettrack_phi'                 , 1e-5],

    ['el*_refittedTrack_d0_err_wrtBL'   , 1e-4],
    ['el*_refittedTrack_phi_err_wrtBL'  , 3e-3],
    ['el*_refittedTrack_theta_err_wrtBL' , 4e-5],
    ['el*_refittedTrack_phi_theta_err_wrtBL', (2e-2, 1e-10)],
    ['el*_refittedTrack_phi_qoverp_err_wrtBL', 5e-3],
    ['el*_refittedTrack_theta_qoverp_err_wrtBL', 1],
    ['el*_refittedTrack_z0_theta_err_wrtBL', 3e-4],
    ['el*_refittedTrack_z0_qoverp_err_wrtBL', 6e-3],
    ['el*_refittedTrack_z0_phi_err_wrtBL', 9e-3],
    ['el*_refittedTrack_d0_wrtBL'       , 6e-4],
    ['el*_refittedTrack_d0_phi_err_wrtBL', 3e-3],
    ['el*_refittedTrack_d0_z0_err_wrtBL', 5e-4],
    ['el*_refittedTrack_d0_theta_err_wrtBL', 6e-2],
    ['el*_refittedTrack_d0_qoverp_err_wrtBL', 6e-4],
    ['el*_tracksigz0pvunbiased'         , 1e-5],
    ['el*_trackz0pvunbiased'            , 7e-4],
    ['el*_trackd0pvunbiased'            , 2e-4],
    ['el*_trackz0pv'                    , 6e-4],
    ['el*_trackd0pv'                    , 1e-4],
    ['el*_trackz0beam'                  , 4e-5],
    ['el*_trackd0beam'                  , 1e-3],
    ['el*_trackIPEstimate_z0_unbiasedpvunbiased', 1e-3],
    ['el*_trackIPEstimate_d0_unbiasedpvunbiased', 2e-4],
    ['el*_trackIPEstimate_sigz0_unbiasedpvunbiased', 1e-5],
    ['el*_trackIPEstimate_z0_biasedpvunbiased', 6e-4],
    ['el*_trackIPEstimate_d0_biasedpvunbiased', 1e-4],
    ['el*_trackIPEstimate_sigz0_biasedpvunbiased', 1e-5],

    ['ph_vx_Dist'                      , (6e-4, 2e-2)],

    ['egtruth_phiCalo'                 , 3e-5],
    ['egtruth_etaCalo'                 , 6e-5],

    ]


def diff_branch (k, kb, v1, v2):
    head = '%s branch %s:' % (k, kb)
    ev1 = v1.keys()
    ev2 = v2.keys()
    ev1.sort()
    ev2.sort()
    if ev1 != ev2:
        if ignore_event_differences:
            ev1 = list(set(ev1).intersection(set(ev2)))
            ev1.sort()
        else:
            print head, 'Different set of events; new file: ', \
                  ev1, 'reference file', ev2
            return

    ithresh = None
    eltcmp = None

    thresh = 1e-6
    for (pat, th) in branch_thresh:
        if fnmatch (kb, pat):
            if type(th) == type(()):
                thresh = th[0]
                ithresh = th[1]
            else:
                thresh = th
            break

    # GSF reco run in d3pd job, so we have dbg/opt diffs.
    if kb.startswith ('el_gsf_'): thresh = 1e-4
    if kb == 'el_gsf_deltaeta2': thresh = 3e-3
    if kb == 'el_gsf_deltaphi2': thresh = 2e-1
    if kb == 'el_gsf_deltaphiRescaled': thresh = 2e-2
    if kb == 'el_gsf_deltaeta1': thresh = 3e-3
    if kb == 'el_gsf_trackz0pv': thresh = 1e-1
    if kb == 'el_gsf_covqoverp': thresh = 5e-2
    if kb == 'el_gsf_L2_dr': thresh = 3e-2
    if kb == 'el_gsf_EF_dr': thresh = 7e-2
    if kb == 'el_gsf_covd0theta': thresh = 3e-4
    if kb == 'el_gsf_electronweight': thresh = 2e-2
    if kb == 'el_gsf_electronbgweight': thresh = 3e-1
    if kb == 'el_gsf_adaboost': thresh = 7e-2
    if kb == 'el_gsf_neuralnet': thresh = 2e-1
    if kb == 'el_gsf_isolationlikelihoodhqelectrons': thresh = 4e-2
    if kb == 'el_gsf_jet_dr': thresh = 2e-2
    if kb == 'el_gsf_Et': thresh = 3e-3
    if kb == 'el_gsf_E': thresh = 4e-3
    if kb == 'sc_yCells': thresh = 2e-6
    if kb.startswith ('el_gsf_p'): thresh = 3e-3
    if kb.startswith ('el_gsf_refittedTrack_cov'): thresh = 3e-1
    if kb.startswith ('el_gsf_trackcov'): thresh = 5e-1
    if kb.startswith ('el_gsf_tracksig'): thresh = 4e-2
    if kb.startswith ('el_gsf_trackd0'): thresh = 6e-2
    if kb.startswith ('el_gsf_trackz0'): thresh = 3e-2
    if kb.startswith ('el_gsf_trackfitchi2'): thresh = 7e-3
    if kb.startswith ('el_gsf_trackqoverp'): thresh = 2e-2
    if kb.startswith ('el_gsf_trackpt'): thresh = 2e-2
    if kb.startswith ('el_gsf_refittedTrack_d0'): thresh = 3e-3
    if kb.startswith ('el_gsf_refittedTrack_LMqoverp'): thresh = 3e-3
    if kb.startswith ('el_gsf_refittedTrack_qoverp'): thresh = 2e-2
    if kb.startswith ('el_gsf_refittedTrack_covphitheta'): thresh = 5e-1
    if kb.startswith ('el_gsf_mvaptcone'): ithresh = 1e-10
    if kb == 'el_gsf_m':
        thresh = 2e-2
        ithresh = 0.01

    if kb.find('jet_')>=0 and kb.endswith ('_m'): ithresh = 0.1
    if kb.find('jetcone_')>=0 and kb.endswith ('_m'): ithresh = 1e-3
    if kb.find('muonTruth_')>=0 and kb.endswith ('_m'): ithresh = 1e-3
    if kb == 'mc_m':
        ithresh = 0.1
        thresh = 2e-5
    if kb.endswith ('_effTopoInvMass'): thresh = 1e-2
    if kb.endswith ('_topoInvMass'): thresh = 1e-2
    if (kb.endswith ('_topoMeanDeltaR') or
        kb.endswith ('_effTopoMeanDeltaR')):
        ithresh = 1e-6
    if kb.find ('_rawcl_etas') >= 0: thresh = 2e-4
    if kb.endswith ('_convIP'): thresh = 4e-4
    if kb.endswith ('_convIPRev'): thresh = 6e-5
    if kb.endswith ('_emscale_E'): thresh = 9e-5
    if kb.endswith ('_emscale_eta'): thresh = 9e-5
    if kb.endswith ('_emscale_m'): ithresh = 0.1
    if kb.endswith ('_constscale_E'): thresh = 9e-5
    if kb.endswith ('_constscale_eta'): thresh = 9e-5
    if kb == 'mc_eta': thresh = mc_eta_thresh
    if kb.endswith ('_seg_locX'): ithresh = 2e-12
    if kb.endswith ('_seg_locY'): ithresh = 2e-12
    if kb == 'MET_Goodness_DeltaEt_JetAlgs_Jet': ithresh = 3e-11
    if kb == 'MET_Goodness_EEM_Jet': thresh = 2e-5
    if kb == 'MET_Goodness_HECf_Jet': thresh = 3e-6
    if kb.find ('_blayerPrediction') >= 0: thresh = 1e-2
    if kb.endswith ('_hecf'): thresh = 4e-6
    if kb.endswith ('_SpaceTime_t'): thresh = 4e-6
    if kb.endswith ('_SpaceTime_weight'): thresh = 2e-5
    if kb.endswith ('_SpaceTime_tError'): thresh = 8e-6
    if kb.endswith ('_calcVars_ChPiEMEOverCaloEME'): thresh = 1e-5
    if kb.endswith ('_vx_m'): ithresh = 1e-2
    if kb.endswith ('MET_Goodness_DeltaEt_JetAlgs_Jet'): ithresh = 1e-10

    # Diffs btn 32/64-bit
    if kb == 'mc_perigee_theta': thresh = 1
    if kb == 'mc_perigee_d0': thresh = 1
    if kb == 'mc_perigee_phi': thresh = 1 #(ev 2,9)
    if kb == 'mc_perigee_z0': thresh = 1
    if kb == 'tau_likelihood': thresh = 1e-5
    if kb == 'tau_SafeLikelihood': thresh = 1e-5
    if kb == 'el_gsf_softeweight': thresh = 1e-1
    if kb == 'el_gsf_softebgweight': thresh = 2e-2

    if kb.endswith ('_mvaptcone20'): ithresh = 1e-10
    if kb.endswith ('_mvaptcone30'): ithresh = 1e-10
    if kb.endswith ('_mvaptcone40'): ithresh = 1e-10

    # more 32/64 dbg/opt diffs
    if kb == 'tau_SafeLikelihood': thresh = 0.4
    if kb == 'tau_likelihood': thresh = 0.4
    if kb == 'tau_BDTJetScore': thresh = 1.0
    if kb == 'tau_calcVars_corrFTrk': thresh = 0.02
    if kb == 'tau_calcVars_corrCentFrac': thresh = 0.4
    if kb == 'tau_JetBDTSigLoose': thresh = 1.0
    if kb == 'tau_JetBDTSigMedium': thresh = 1.0
    if kb == 'tau_JetBDTSigTight': thresh = 1.0
    if kb == 'tau_tauLlhMedium': thresh = 1.0
    if kb == 'tau_pi0_cl1_pt': thresh = 1e-5

    if kb.endswith ('_neighbourSig'): thresh = 1e-4
    if kb.endswith ('_curvatureSig'): thresh = 1e-4

    if kb == 'pixClus_detElementId': eltcmp = compare_detElementId
    if kb == 'pixClus_id': eltcmp = compare_pixid

    for e in ev1:
        head = '%s branch %s event %d:' % (k, kb, e)
        b1 = v1[e]
        b2 = v2[e]

        # Promote int to float if needed.
        if type(b1) in inttypes and type(b2) == type(1.1):
            b1 = float(b1)
        elif type(b2) in inttypes and type(b1) == type(1.1):
            b2 = float(b2)

        if type(b1) != type(b2):
            print head, 'Type differs; new file: ', type(b1).__name__, \
                  'reference file:', type(b2).__name__
        elif not compare (b1, b2, thresh = thresh, ithresh = ithresh,
                          eltcmp = eltcmp):
            print head, 'Branch mismatch'
            print '  new file:', b1
            print '  ref file:', b2
        
    return

def diff_tree (k, t1, t2):
    if t1.dummy and t2.dummy: return
    apply_renames (t1.branches, t2.branches, renames)
    dictkey_diff (t1.branches, t2.branches, '%s: Branches only in new file:'%k,
                  ignore_br_p)
    dictkey_diff (t2.branches, t1.branches, '%s: Branches only in reference file:'%k,
                  ignore_br_p)
    for kb in t1.branches:
        if t2.branches.has_key (kb):
            if not ignore_br_p(kb):
                diff_branch (k, kb, t1.branches[kb], t2.branches[kb])
    return


def diff_hist (k, h1, h2):
    if not compare (h1.bins, h2.bins):
        print 'Hist', k, 'has bins', h1.bins
        print 'in new file but bins', h2.bins
        print 'in reference file'
    if not compare (h1.errs, h2.errs):
        print 'Hist', k, 'has errs', h1.errs
        print 'in new file but errs', h2.errs
        print 'in reference file'
    return


def diff_files (d1, d2):
    apply_renames (d1.keys, d2.keys, rename_keys)
    dictkey_diff (d1.keys, d2.keys, 'Keys only in new file:',
                  ignore_key_p)
    dictkey_diff (d2.keys, d1.keys, 'Keys only in reference file:',
                  ignore_key_p)
    for k in d1.keys.keys():
        if d2.keys.has_key(k) and not ignore_key_p (k):
            v1 = d1.keys[k]
            v2 = d2.keys[k]
            if type(v1) != type(v2):
                print 'Key', k, 'has type', type(v1).__name__, \
                      'in new file, but type', type(v2).__name__, \
                      'in reference file'
            elif type(v1) == String:
                diff_string (k, v1, v2)
            elif type(v1) == Tree:
                diff_tree (k, v1, v2)
            elif type(v1) == Hist:
                diff_hist (k, v1, v2)
            else:
                print 'Unknown type for diff:', type(v1).__name__
    return


if __name__ == '__main__':
    import sys
    import subprocess

    if sys.argv[1] == '--ignore-event-differences':
        ignore_event_differences = True
        del sys.argv[1]

    # New tuple
    p = subprocess.Popen ('python -m D3PDMakerTest.dumptuple_any %s' %
                            sys.argv[1],
                          shell = True,
                          stdout = subprocess.PIPE)
    d1 = Dumpreader (p.stdout)
    # Reference file
    d2 = Dumpreader (open (sys.argv[2]))

    diff_files (d1, d2)
    

