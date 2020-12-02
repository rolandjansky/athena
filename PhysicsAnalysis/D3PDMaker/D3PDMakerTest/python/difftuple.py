# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: D3PDMakerTest/python/difftuple.py
# Author: snyder@bnl.gov
# Date: Feb, 2010
# Purpose: Diff two root tuple files.
#

from __future__ import print_function

# Always run in batch mode.
import os
if 'DISPLAY' in os.environ:
    del os.environ['DISPLAY']

import ROOT
import types
import os
from fnmatch import fnmatch
from PyUtils.Helpers import ROOT6Setup
ROOT6Setup()

# new : old
renames = {
#     'jet_antikt4h1topo_LArQuality'  : 'jet_antikt4h1topo_quality',
#     'jet_antikt4h1tower_LArQuality' : 'jet_antikt4h1tower_quality',
#     'jet_antikt4truth_LArQuality'   : 'jet_antikt4truth_quality',
#     'jet_antikt6h1tower_LArQuality' : 'jet_antikt6h1tower_quality',
    
#     'METJetsInfo_JetPtWeightedEventEMFraction' : 'MET_JetsInfo_JetPtWeightedEventEMFraction',
#     'METJetsInfo_JetPtWeightedNumAssociatedTracks' : 'MET_JetsInfo_JetPtWeightedNumAssociatedTracks',
#     'METJetsInfo_JetPtWeightedSize' : 'MET_JetsInfo_JetPtWeightedSize',
#     'METJetsInfo_LeadingJetEt'      : 'MET_JetsInfo_LeadingJetEt',
#     'METJetsInfo_LeadingJetEta'     : 'MET_JetsInfo_LeadingJetEta',

#     'MET_DM_All_et'                 : 'MET_DMAll_et',
#     'MET_DM_All_etx'                : 'MET_DMAll_etx',
#     'MET_DM_All_ety'                : 'MET_DMAll_ety',
#     'MET_DM_All_phi'                : 'MET_DMAll_phi',
#     'MET_DM_All_sumet'              : 'MET_DMAll_sumet',

#     'MET_DM_Crack1_et'              : 'MET_DMCrack1_et',
#     'MET_DM_Crack1_etx'             : 'MET_DMCrack1_etx',
#     'MET_DM_Crack1_ety'             : 'MET_DMCrack1_ety',
#     'MET_DM_Crack1_phi'             : 'MET_DMCrack1_phi',
#     'MET_DM_Crack1_sumet'           : 'MET_DMCrack1_sumet',

#     'MET_DM_Crack2_et'              : 'MET_DMCrack2_et',
#     'MET_DM_Crack2_etx'             : 'MET_DMCrack2_etx',
#     'MET_DM_Crack2_ety'             : 'MET_DMCrack2_ety',
#     'MET_DM_Crack2_phi'             : 'MET_DMCrack2_phi',
#     'MET_DM_Crack2_sumet'           : 'MET_DMCrack2_sumet',

#     'MET_DM_Cryo_et'                : 'MET_DMCryo_et',
#     'MET_DM_Cryo_etx'               : 'MET_DMCryo_etx',
#     'MET_DM_Cryo_ety'               : 'MET_DMCryo_ety',
#     'MET_DM_Cryo_phi'               : 'MET_DMCryo_phi',
#     'MET_DM_Cryo_sumet'             : 'MET_DMCryo_sumet',

#     'MET_Muid_Spectro_et'           : 'MET_MuidSpectro_et',
#     'MET_Muid_Spectro_etx'          : 'MET_MuidSpectro_etx',
#     'MET_Muid_Spectro_ety'          : 'MET_MuidSpectro_ety',
#     'MET_Muid_Spectro_phi'          : 'MET_MuidSpectro_phi',
#     'MET_Muid_Spectro_sumet'        : 'MET_MuidSpectro_sumet',

#     'MET_Muid_Track_et'             : 'MET_MuidTrack_et',
#     'MET_Muid_Track_etx'            : 'MET_MuidTrack_etx',
#     'MET_Muid_Track_ety'            : 'MET_MuidTrack_ety',
#     'MET_Muid_Track_phi'            : 'MET_MuidTrack_phi',
#     'MET_Muid_Track_sumet'          : 'MET_MuidTrack_sumet',

#     'MET_MuonBoy_Spectro_et'        : 'MET_MuonBoySpectro_et',
#     'MET_MuonBoy_Spectro_etx'       : 'MET_MuonBoySpectro_etx',
#     'MET_MuonBoy_Spectro_ety'       : 'MET_MuonBoySpectro_ety',
#     'MET_MuonBoy_Spectro_phi'       : 'MET_MuonBoySpectro_phi',
#     'MET_MuonBoy_Spectro_sumet'     : 'MET_MuonBoySpectro_sumet',

#     'MET_MuonBoy_Track_et'          : 'MET_MuonBoyTrack_et',
#     'MET_MuonBoy_Track_etx'         : 'MET_MuonBoyTrack_etx',
#     'MET_MuonBoy_Track_ety'         : 'MET_MuonBoyTrack_ety',
#     'MET_MuonBoy_Track_phi'         : 'MET_MuonBoyTrack_phi',
#     'MET_MuonBoy_Track_sumet'       : 'MET_MuonBoyTrack_sumet',

#     'MET_RefMuon_Track_et'          : 'MET_RefMuonTrack_et',
#     'MET_RefMuon_Track_etx'         : 'MET_RefMuonTrack_etx',
#     'MET_RefMuon_Track_ety'         : 'MET_RefMuonTrack_ety',
#     'MET_RefMuon_Track_phi'         : 'MET_RefMuonTrack_phi',
#     'MET_RefMuon_Track_sumet'       : 'MET_RefMuonTrack_sumet',

    #'vxp_trk_phi'                 : 'vxp_trk_phi0',
    #'ef_el_trackcov_d0'           : 'ef_el_trackcovd0',
    #'ef_el_trackcov_d0_phi'       : 'ef_el_trackcovd0phi',
    #'ef_el_trackcov_d0_qoverp'    : 'ef_el_trackcovd0qoverp',
    #'ef_el_trackcov_d0_theta'     : 'ef_el_trackcovd0theta',
    #'ef_el_trackcov_d0_z0'        : 'ef_el_trackcovd0z0',
    #'ef_el_trackcov_phi'          : 'ef_el_trackcovphi',
    #'ef_el_trackcov_phi_qoverp'   : 'ef_el_trackcovphiqoverp',
    #'ef_el_trackcov_phi_theta'    : 'ef_el_trackcovphitheta',
    #'ef_el_trackcov_theta'        : 'ef_el_trackcovtheta',
    #'ef_el_trackcov_theta_qoverp' : 'ef_el_trackcovthetaqoverp',
    #'ef_el_trackcov_qoverp'       : 'ef_el_trackcovqoverp',
    #'ef_el_trackcov_z0'           : 'ef_el_trackcovz0',
    #'ef_el_trackcov_z0_phi'       : 'ef_el_trackcovz0phi',
    #'ef_el_trackcov_z0_qoverp'    : 'ef_el_trackcovz0qoverp',
    #'ef_el_trackcov_z0_theta'     : 'ef_el_trackcovz0theta',#

    #'el_trackcov_d0'           : 'el_trackcovd0',
    #'el_trackcov_d0_phi'       : 'el_trackcovd0phi',
    #'el_trackcov_d0_qoverp'    : 'el_trackcovd0qoverp',
    #'el_trackcov_d0_theta'     : 'el_trackcovd0theta',
    #'el_trackcov_d0_z0'        : 'el_trackcovd0z0',
    #'el_trackcov_phi'          : 'el_trackcovphi',
    #'el_trackcov_phi_qoverp'   : 'el_trackcovphiqoverp',
    #'el_trackcov_phi_theta'    : 'el_trackcovphitheta',
    #'el_trackcov_theta'        : 'el_trackcovtheta',
    #'el_trackcov_theta_qoverp' : 'el_trackcovthetaqoverp',
    #'el_trackcov_qoverp'       : 'el_trackcovqoverp',
    #'el_trackcov_z0'           : 'el_trackcovz0',
    #'el_trackcov_z0_phi'       : 'el_trackcovz0phi',
    #'el_trackcov_z0_qoverp'    : 'el_trackcovz0qoverp',
    #'el_trackcov_z0_theta'     : 'el_trackcovz0theta',
 
    #'elfilt_trackcov_d0'           : 'elfilt_trackcovd0',
    #'elfilt_trackcov_d0_phi'       : 'elfilt_trackcovd0phi',
    #'elfilt_trackcov_d0_qoverp'    : 'elfilt_trackcovd0qoverp',
    #'elfilt_trackcov_d0_theta'     : 'elfilt_trackcovd0theta',
    #'elfilt_trackcov_d0_z0'        : 'elfilt_trackcovd0z0',
    #'elfilt_trackcov_phi'          : 'elfilt_trackcovphi',
    #'elfilt_trackcov_phi_qoverp'   : 'elfilt_trackcovphiqoverp',
    #'elfilt_trackcov_phi_theta'    : 'elfilt_trackcovphitheta',
    #'elfilt_trackcov_theta'        : 'elfilt_trackcovtheta',
    #'elfilt_trackcov_theta_qoverp' : 'elfilt_trackcovthetaqoverp',
    #'elfilt_trackcov_qoverp'       : 'elfilt_trackcovqoverp',
    #'elfilt_trackcov_z0'           : 'elfilt_trackcovz0',
    #'elfilt_trackcov_z0_phi'       : 'elfilt_trackcovz0phi',
    #'elfilt_trackcov_z0_qoverp'    : 'elfilt_trackcovz0qoverp',
    #'elfilt_trackcov_z0_theta'     : 'elfilt_trackcovz0theta',

    #'mu_trackcov_d0'           : 'mu_trackcovd0',
    #'mu_trackcov_d0_phi'       : 'mu_trackcovd0phi',
    #'mu_trackcov_d0_qoverp'    : 'mu_trackcovd0qoverp',
    #'mu_trackcov_d0_theta'     : 'mu_trackcovd0theta',
    #'mu_trackcov_d0_z0'        : 'mu_trackcovd0z0',
    #'mu_trackcov_phi'          : 'mu_trackcovphi',
    #'mu_trackcov_phi_qoverp'   : 'mu_trackcovphiqoverp',
    #'mu_trackcov_phi_theta'    : 'mu_trackcovphitheta',
    #'mu_trackcov_theta'        : 'mu_trackcovtheta',
    #'mu_trackcov_theta_qoverp' : 'mu_trackcovthetaqoverp',
    #'mu_trackcov_qoverp'       : 'mu_trackcovqoverp',
    #'mu_trackcov_z0'           : 'mu_trackcovz0',
    #'mu_trackcov_z0_phi'       : 'mu_trackcovz0phi',
    #'mu_trackcov_z0_qoverp'    : 'mu_trackcovz0qoverp',
    #'mu_trackcov_z0_theta'     : 'mu_trackcovz0theta',
 
    #'trk_cov_d0'           : 'trk_covd0',
    #'trk_cov_phi'          : 'trk_covphi',
    #'trk_cov_qoverp'       : 'trk_covqoverp',
    #'trk_cov_theta'        : 'trk_covtheta',
    #'trk_cov_z0'           : 'trk_covz0',
    #'trk_cov_d0_phi'       : 'trk_covd0phi',
    #'trk_cov_d0_qoverp'    : 'trk_covd0qoverp',
    #'trk_cov_d0_theta'     : 'trk_covd0theta',
    #'trk_cov_d0_z0'        : 'trk_covd0z0',
    #'trk_cov_z0_phi'       : 'trk_covz0phi',
    #'trk_cov_z0_qoverp'    : 'trk_covz0qoverp',
    #'trk_cov_z0_theta'     : 'trk_covz0theta',
    #'trk_cov_phi_qoverp'   : 'trk_covphiqoverp',
    #'trk_cov_phi_theta'    : 'trk_covphitheta',
    #'trk_cov_theta_qoverp' : 'trk_covthetaqoverp',

    #'vxp_x'                : 'vxp_vertx',
    #'vxp_y'                : 'vxp_verty',
    #'vxp_z'                : 'vxp_vertz',
    #'vxp_err_x'            : 'vxp_errx',
    #'vxp_err_y'            : 'vxp_erry',
    #'vxp_err_z'            : 'vxp_errz',
    #'vxp_E'                : 'vxp_E',
    #'vxp_m'                : 'vxp_vertM',
    #'vxp_px'               : 'vxp_vertPx',
    #'vxp_py'               : 'vxp_vertPy',
    #'vxp_pz'               : 'vxp_vertPz',
    #'vxp_sumPt'            : 'vxp_vertSumPt',
    #'vxp_nTracks'          : 'vxp_vertNtrk',
    }

# List of branches to ignore for various versions.
# Format is a list of tuples: (V, V, V, [BRANCHES...])
# If any of the strings in V appear in CMTPATH, then
# we do not ignore the branches.  Otherwise, we ignore
# branches matching any of the glob paths in BRANCHES.
_ignore_branches = [
    ('15.6',
     ['mb_eta',
      'mb_phi',
      'tau_calcVars_*',
      'el_rawcl_*',
      'ph_rawcl_*',
      'elfilt_rawcl_*',
      'ph_mvaptcone*',
      'MET_Goodness_N_Jets',
      'MET_Goodness_DeltaTrackTopo',
      'MET_Goodness_isBad_Jet',
      'MET_Goodness_isGood_Jet',
      'MET_Goodness_isUgly_Jet',
      'MET_Goodness_tileGap3F_Jet',
      'MET_Goodness_MET_CorrTopo_etx',
      'MET_Goodness_MET_CorrTopo_ety',
      'MET_Goodness_MET_CorrTopo_sumet',
      'MET_Goodness_MET_Topo_etx',
      'MET_Goodness_MET_Topo_ety',
      'MET_Goodness_MET_Topo_sumet',
      'jet_*',
      '*_OriginIndex',
      '*_Timing',
      '*_timing',
      '*ntrk',
      '*sumPtTrk',
      'el_medium',
      'elfilt_medium',
      'trk_blayerPrediction_locX',
         ]
     ),
    ('15.6', '16.0',
     [
         ]
     )
    ]

# Should a branch be ignored?
def ignore_p (br):
    ver = os.environ.get ('CMTPATH')

    for vlist in _ignore_branches:
        for v in vlist[:-1]:
            found = False
            if ver and ver.find (v) >= 0:
                found = True
                break
        if found: continue
        for b in vlist[-1]:
            if fnmatch (br, b):
                return True
    return False
        


def topy (o):
    if type(o).__name__.startswith ('vector<'):
        ll = list(o)
        if ll and type(ll[0]).__name__.startswith ('vector<'):
            ll = [list(l) for l in ll]
        return ll
    return o


inttypes = [types.IntType, types.LongType]
def compare (o1, o2, thresh = 1e-6, ithresh = None):
    # Allow comparing int/long int.
    if type(o1) in inttypes and type(o2) in inttypes:
        if o1 < 0: o1 = o1 + (1<<32)
        if o2 < 0: o2 = o2 + (1<<32)
        return o1 == o2
    if type(o1) != type(o2):
        return False
    if isinstance(o1,list):
        if len(o1) != len(o2):
            return False
        for i in range(len(o1)):
            if not compare (o1[i], o2[i],
                            thresh=thresh, ithresh=ithresh): return False
        return True
    if type(o1).__name__ in ['map<string,int>',
                             'map<string,float>',
                             'map<string,string>']:
        return ROOT.D3PDTest.MapDumper.equal (o1, o2)
    if isinstance(o1, float):
        if ithresh and abs(o1) < ithresh and abs(o2) < ithresh:
            return True
        num = o1-o2
        den = abs(o1)+abs(o2)
        if den == 0: return True
        x = abs(num / den)
        if callable(thresh): thresh = thresh(den)
        if x > thresh:
            print ('fmismatch', o1, o2, x)
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


def diff_trees (t1, t2):
    n1 = t1.GetEntries()
    n2 = t2.GetEntries()
    if n1 != n2:
        print ('Different nentries for tree', t1.GetName(), ': ', n1, n2)
        n1 = min(n1, n2)
    b1 = [b.GetName() for b in t1.GetListOfBranches()]
    b2 = [b.GetName() for b in t2.GetListOfBranches()]
    b1.sort()
    b2.sort()
    branchmap = renames.copy()
    for b in b1:
        if b not in b2:
            bb = branchmap.get (b)
            if not bb or bb not in b2:
                if not ignore_p(b):
                    print ('Branch', b, 'in first tree but not in second.')
                if bb: del branchmap[b]
            else:
                b2.remove (bb)
        else:
            b2.remove (b)
            branchmap[b] = b
    for b in b2:
        if not ignore_p(b):
            print ('Branch', b, 'in second tree but not in first.')

    for i in range (n1):
        t1.GetEntry(i)
        t2.GetEntry(i)
        for b in b1:
            if ignore_p(b): continue
            bb = branchmap.get(b)
            if not bb: continue
            o1 = topy (getattr(t1, b))
            o2 = topy (getattr(t2, bb))

            ithresh = None
            thresh = 1e-6
            if b.find('jet_')>=0 and b.endswith ('_m'): ithresh = 0.1
            if b == 'mc_m': ithresh = 0.1
            if b.find ('_rawcl_etas') >= 0: thresh = 2e-4
            if b.endswith ('_convIP'): thresh = 3e-5
            if b.endswith ('_emscale_E'): thresh = 9e-5
            if b.endswith ('_emscale_eta'): thresh = 9e-5
            if b.endswith ('_emscale_m'): ithresh = 0.1
            if b.endswith ('_constscale_E'): thresh = 9e-5
            if b.endswith ('_constscale_eta'): thresh = 9e-5
            if b == 'mc_eta': thresh = mc_eta_thresh
            if b.endswith ('_seg_locX'): ithresh = 2e-12
            if b.endswith ('_seg_locY'): ithresh = 2e-12
            if b == 'MET_Goodness_DeltaEt_JetAlgs_Jet': ithresh = 2e-11
            if b == 'MET_Goodness_EEM_Jet': thresh = 2e-5
            if b == 'MET_Goodness_HECf_Jet': thresh = 3e-6
            if b.find ('_blayerPrediction') >= 0: thresh = 4e-4
            if not compare (o1, o2, thresh = thresh, ithresh = ithresh):
                print ('Branch mismatch', b, 'entry', i, ':', ithresh)
                print (o1)
                print (o2)

    return


def diff_objstrings (k, s1, s2):
    # nb. != not working correctly for TObjString in 5.26.00c_python2.6
    if not (s1 == s2):
        print ('Objstring', k, 'mismatch:')
        print (repr(s1))
        print (repr(s2))
    return


def diff_dirs (f1, f2):
    k1 = [k.GetName() for k in f1.GetListOfKeys()]
    k2 = [k.GetName() for k in f2.GetListOfKeys()]
    k1.sort()
    k2.sort()
    if k1 != k2:
        print ("Key list mismatch for", f1.GetName(), f2.GetName(), ":")
        print (k1)
        print (k2)
    for k in k1:
        if k not in k2: continue
        o1 = f1.Get(k)
        o2 = f2.Get(k)
        if type(o1) != type(o2):
            print ('Type mismatch for ', k)
            print (o1, o2)
        if k == 'Schema':
            pass
        elif isinstance (o1, ROOT.TTree):
            diff_trees (o1, o2)
        elif isinstance (o1, ROOT.TDirectory):
            diff_dirs (o1, o2)
        elif isinstance (o1, ROOT.TObjString):
            diff_objstrings (k, o1, o2)
        else:
            print (k, type(o1))
    return


if __name__ == '__main__':
    import sys
    f1 = ROOT.TFile (sys.argv[1])
    f2 = ROOT.TFile (sys.argv[2])
    diff_dirs (f1, f2)
