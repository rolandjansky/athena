# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Reformat input dictionary. Dictionary structure at
https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerCommon/ \
TriggerMenu/trunk/python/menu/SignatureDicts.py#L144
"""

import re
from eta_string_conversions import eta_string_to_floats
from smc_string_conversions import smc_string_to_strings
from clusterparams_factory import clusterparams_factory
from fexparams_factory import fexparams_factory
from hypo_factory import hypo_factory

# from lxml import etree as et
from ChainConfig import ChainConfig
from MenuData import MenuData


err_hdr = 'ChainConfigMaker error: '
    
def _check_smc(smc_min, smc_max):
    if smc_min == 'INF' and smc_max != 'INF':
        return 'min smc > max smc'

    try:
        float(smc_min)  # float('INF'), float('-INF') work...
    except:
        return 'min smc does not convert to float'

    try:
        float(smc_max)  # float('INF'), float('-INF') work...
    except:
        return 'max smc does not convert to float'


    if not float(smc_min) <= float(smc_max):
        return 'min smc > max smc'

    return ''


class JetAttributes(object):
    """Per jet attributes. Used by  hypo algorithms."""

    def __init__(self, threshold, eta_range, smc_range):
        self.threshold = threshold
        self.eta_range = eta_range  # string like '0eta320'
        self.smc_range = smc_range  # single jet mass 
        # eta_min, eta_max are floats
        self.eta_min, self.eta_max = eta_string_to_floats(eta_range)

        # smc (SingleJetMass) values are strings to allow 'INF'
        # to be passed to the C++ Algorithm
        self.smc_min, self.smc_max = smc_string_to_strings(smc_range)

        errstr =  _check_smc(self.smc_min, self.smc_max)
        if errstr:
            msg = '%s: illegal jet mass %s' % (err_hdr, errstr)
            raise RuntimeError(msg)
            

        self.asymmetricEta = 1 if (eta_range.startswith('n') or
                                   eta_range.startswith('p')) else 0

    def __str__(self):
        return 'thresh: %s eta_min: %s eta_max: %s smc_min: %s smc_max: %s' % (
            str(self.threshold),
            str(self.eta_min),
            str(self.eta_max),
            self.smc_min,
            self.smc_max
        )


hypo_type_dict = {('j', '', False, False, False): 'HLThypo2_etaet',
                  ('j', '', False, False, True): 'HLThypo2_singlemass',
                  ('j', '', False, True, False): 'HLThypo2_dimass_deta',
                  ('ht', '', False, False, False):'HLThypo2_ht',
                  ('j', '', True, False, False): 'HLThypo2_tla',}

    
cleaner_names = {
    'cleanL': 'looseCleaning',
    'cleanLA': 'antiLooseCleaning',
    'cleanT': 'tightCleaning',
    'cleanTA': 'antiTightCleaning',
    'cleanLLP': 'llpCleaning',
    'cleanLLPA':'antLlpCleaning',
    'noCleaning': 'noCleaning',
}

fex_alg_re = re.compile(r'^a(?P<merge_param>\d+)(r|t)?$')
recluster_alg_re = re.compile(r'^a\d+r$')
fex_alg_trim_re = re.compile(r'^a\d+t$')
invm_re = re.compile(r'^invm(?P<mass_min>\d+)$')
deta_re = re.compile(r'^deta(?P<dEta_min>\d+)$')
tla_re = \
         re.compile(r'^(?P<indexlo>\d+)i(?P<indexhi>\d+)c(?P<mass_min>\d+)m(?P<mass_max>\d+)TLA$')

cache = {}

def _update_cache(k, v):
    global cache
    cache[k] = v


def _reset_cache():
    global cache
    cache = {}


def _get_test_flag(parts):

    flag = cache.get('test_flag')
    if flag: return flag
    
    vals = set([])
    for part in parts:
        extra = part['extra']
        if extra.startswith('test'):
            vals.add(extra)

    if not vals:
        _update_cache('test_flag', '')
        return ''
    if len(vals) == 1:
        flag = vals.pop()
        _update_cache('test_flag', flag)
        return flag

    msg = '%s: multiple test flags set' % err_hdr
    raise RuntimeError(msg)


def _set_jet_build_calib(key, value):
    """extract a float from the string value for calibration name 'key' """

    defaults = {'recoCutCalib': 7.0,
                'recoCutUncalib': 7.0,}

    if key not in defaults:
        msg = '%s no default for jet calib label %s' % (err_hdr, key)
        raise RuntimeError(msg)

    val = None
    if 'Default' in value:
        val = defaults.get(key)
    else:
        try:
            val = float(value[3:])
        except:
            msg = '%s val for  jet calib not a float %s' % (err_hdr, value)
            raise RuntimeError(msg)

    _update_cache(key, val)
    return val

    
def _get_reco_cut_str(key, parts):
    """(de)cache and return a jet build tool ET jet cut"""

    
    if key not in ('recoCutCalib', 'recoCutUncalib'):
        msg = '%s unkown jet reco calib cut  %s' % (err_hdr, key)
        raise RuntimeError(msg)
        
    # it is a error to have different values if > 1 chain part
    vals = set([])
    for part in parts:
        val = part.get(key)
        if val is not None:
            vals.add(val)


    logical_defaults = {'recoCutCalib': 'rccDefault',
                        'recoCutUncalib': 'rcuDefault',}

    # not specified? use default
    if not vals:
        return logical_defaults.get(key)

    if len(vals) == 1:
        # non default values are never used for standard jet chains.
        # they are needed by MET, which _always_ uses j0 (no hypo) chains
        val= vals.pop()
        chain_name = cache['chain_name']
        if  (val not in logical_defaults.values() and
             not cache['chain_name'].startswith('j0')):
            
            msg = '%s: attempting to set a non-default reco cut %s for '\
            'a non j0 chain %s'
            msg = msg % (err_hdr, val, chain_name)
            raise RuntimeError(msg)
        else:
            return val

    msg = '%s: more than one reco cut set for %s' % (err_hdr, key)
    raise RuntimeError(msg)

    
def _get_reco_cut(key, parts):
    """(de)cache and return a jet build tool ET jet cut"""

    val = cache.get(key)
    if val is not None: return val

    val_str = _get_reco_cut_str(key, parts)

    return _set_jet_build_calib(key, val_str)


def _get_tla_string(parts):

    x = cache.get('tla_string')
    if x: return x

    vals = set([part['TLA'] for part in parts])
    if len(vals) == 1:
        s = vals.pop()
        _update_cache('tla_string', s)
        return s

    msg = '%s: multiple TLA string set' % err_hdr
    raise RuntimeError(msg)


def _get_cluster_calib(parts):

    x = cache.get('cluster_calib')
    if x: return x

    vals = set([part['calib'] for part in parts])
    if len(vals) == 1:
        s = vals.pop()
        _update_cache('cluster_calib', s)
        return s

    msg = '%s: multiple cluster_calib values' % err_hdr
    raise RuntimeError(msg)


def _get_topo(parts, target):

    targets = set([])
    for part in parts:
        p_set = set([x for x in part['topo'] if x.startswith(target)])
        targets = targets.union(p_set)

        
    if not targets: return ''
    if len(targets) == 1: return targets.pop()
        
    msg = '%s %s mass specied more than once' % (err_hdr, target)
    raise RuntimeError(msg)

def _get_invm_string(parts):
    
    x = cache.get('invm')
    if x: return x

    invm = _get_topo(parts, 'invm')
    _update_cache('invm', invm)
    return invm

def _get_deta_string(parts):

    x = cache.get('deta')
    if x: return x

    deta =  _get_topo(parts, 'deta')           
    _update_cache('deta', deta)
    return deta


def _get_jetmass_flag(parts):

    x = cache.get('jetmass')
    if x: return x

    result = False
    for p in parts:
        if p['smc'] != 'nosmc':
            result = True
            _update_cache('jetmass', result)
            return result

    _update_cache('jetmass', result)
    return False


def _get_data_scouting(parts):

    x = cache.get('data_scouting')
    if x: return x

    vals = set([])
    for part in parts:
        ds = part.get('dataScouting', '')
        ds1 = 'ds1' if 'ds1' in ds  else ''
        ds2 = 'ds2' if 'ds2' in ds  else ''

        if ds1 and ds2:
            msg = '%s both "ds1" and "ds2" present in data scouting' \
                  'string: %s' % (err_hdr, ds)
            raise RuntimeError(msg)

        vals.add(ds1 + ds2)

    if len(vals) != 1:
        msg = '%s error setting data scouting flag ' % (err_hdr)

    data_scouting = vals.pop()
    _update_cache('data_souting', data_scouting)
    return data_scouting


def _get_hypo_type(parts):

    test_flag = _get_test_flag(parts)
    tla_flag = bool(_get_tla_string(parts))

    invm_string = _get_invm_string(parts)
    deta_string = _get_deta_string(parts)
    dimass_deta_flag = bool(invm_string) or bool(deta_string)
    jetmass_flag = _get_jetmass_flag(parts)

    htypes =  [hypo_type_dict.get((part['trigType'],
                                   test_flag,
                                   tla_flag,
                                   dimass_deta_flag,
                                   jetmass_flag), None) for part in parts]

    if not htypes or None in htypes:
        part = parts[htypes.index(None)]
        msg = '%s: cannot determine hypo type '\
              'from trigger type: %s test flag: %s ' \
              'TLA: %s dimass_eta %s jetmass flag: %s' %  (
                  err_hdr, part['trigType'],
                  test_flag,
                  tla_flag,
                  dimass_deta_flag,
                  str(jetmass_flag))
        raise RuntimeError(msg)

    htypes = set(htypes)
    if len(htypes) == 1: return htypes.pop()
    if len(htypes) == 2:
        if 'HLThypo2_etaet' in htypes and 'HLThypo2_dimass_deta' in htypes:
            return 'HLThypo2_dimass_deta'

    msg = '%s: cannot determine hypo type, %s' %  (err_hdr, str(htypes))
    raise RuntimeError(msg)


def _get_data_type(parts):
    """ return the data type from which jets are made -
    topo cliusters, jets, towers..
    """
    x = cache.get('data_type')
    if x: return x

    vals = set([part['dataType']for part in parts])

    if len(vals) != 1:
        msg = '%s: cannot determine data_type ' %  err_hdr
        raise RuntimeError(msg)
        
    data_type = vals.pop()
    _update_cache('data_type', data_type)

    return data_type

def _get_run_hypo(parts):

    x = cache.get('run_hypo')
    if x: return x

    vals = set(['perf' not in p['addInfo'] for p in parts])

    is_perf = False
    for p in parts:
        if 'perf' in p['addInfo']:
            is_perf = True

    if len(vals) != 1:
        msg = '%s: cannot determine run_hypo ' %  err_hdr
        raise RuntimeError(msg)
        
    data_scouting_flag = bool(_get_data_scouting(parts))
    if data_scouting_flag and not is_perf:
        data_scouting_flag = False

    run_hypo =  vals.pop() and not data_scouting_flag
    _update_cache('run_hypo', run_hypo)

    return run_hypo

def _get_cluster_label(parts):

    data_type = _get_data_type(parts)
    scan = _get_scan_type(parts)
    # the cluster algorithm is always run with local calibration on
    cluster_label = reduce(lambda x, y: x + y, (data_type, 'lcw', scan))

    return cluster_label
  

def _get_cluster_params(parts):

    args = {}
    args['label'] = _get_cluster_label(parts)

    return clusterparams_factory(args)
    
def _get_jet_calib(parts):

    x = cache.get('jet_calib')
    if x: return x

    vals = set([part['jetCalib'] for part in parts])
    if len(vals) != 1:
        msg = '%s: cannot determine jet_calib ' %  err_hdr
        raise RuntimeError(msg)

    jet_calib =  vals.pop()
    _update_cache('jet_calib', jet_calib)

    return jet_calib


def _get_scan_type(parts):

    x = cache.get('scan_type')
    if x: return x


    vals = set([part['scan'] for part in parts])
    if len(vals) != 1:
        msg = '%s: cannot determine scan ' %  err_hdr
        raise RuntimeError(msg)

    
    scan_type = vals.pop()
    scan_types = ('', 'PS', 'FS')

    if scan_type not in scan_types:
        msg = '%s unknown scanType %s allowed: %s' % (err_hdr,
                                                      scan_type,
                                                      str(scan_types))

    if scan_type != 'FS':
        data_type = _get_data_type(parts)
        if data_type == 'TT':
            msg = '%s: scanType %s set for Trigger tower scan' % (
                err_hdr, scan_type)
            raise RuntimeError(msg)

    _update_cache('scan_type', scan_type)
    
    return scan_type

def _get_trk_option(parts):

    x = cache.get('trkopt')
    if x: return x


    vals = set([part['trkopt'] for part in parts])
    if len(vals) != 1:
        msg = '%s: cannot determine if trkopt ' %  err_hdr
        raise RuntimeError(msg)

    
    trkopt = vals.pop()
    trk_options = ('notrk', 'ftk', 'ftkrefit')

    if trkopt not in trk_options:
        msg = '%s unknown track option %s allowed: %s' % (err_hdr,
                                                      trkopt,
                                                      str(trk_options))

    if 'ftk' in trkopt:
        scan_type = _get_scan_type(parts)
        if scan_type != 'FS':
            msg = '%s: ftk tracking switched on for scan type %s' % (
                err_hdr, scan_type)
            raise RuntimeError(msg)
        data_type = _get_data_type(parts)
        if data_type != 'tc':
            msg = '%s: ftk tracking switched on for data type %s' % (
                err_hdr, data_type)
            raise RuntimeError(msg)

    _update_cache('trkopt', trkopt)
    
    return trkopt


def _get_fex_alg_name(parts):

    x = cache.get('reco_alg_name')
    if x: return x

    vals = set([part['recoAlg'] for part in parts])
    if len(vals) != 1:
        msg = '%s: cannot determine reco alg ' %  err_hdr
        raise RuntimeError(msg)

    fex_alg_name = vals.pop()
    m = fex_alg_re.search(fex_alg_name)
    if not m:
        msg = '%s unknown reco alg: %s' % (err_hdr, fex_alg_name)
        raise RuntimeError(msg)
    _update_cache('reco_alg_name', fex_alg_name)

    return fex_alg_name


def _get_fex_params(parts):
    """provide the fex paramters. The set of of paramters depends
    on the fex type."""

    args = {}

    fex_alg_name = _get_fex_alg_name(parts)

    m = fex_alg_re.search(fex_alg_name)
    merge_param = m.group('merge_param')
    
    fex_type = 'antikt'  # used for fexparams_factory key
    if fex_alg_trim_re.search(fex_alg_name) is not None:
        fex_type = 'jetrec_trimming'

    # For reclustering, the values of the fex given in the chain name are
    # overriden. This fex will provide the inputts to a second fex
    # which will use the chain name values

    if _do_recl(parts):
        args['fex_type'] = 'antikt'
        args['merge_param'] = 4
        args['fex_alg_name'] = 'a4'
    else:
        args['merge_param'] = int(merge_param)
        args['fex_alg_name'] = fex_alg_name
        args['fex_type'] = fex_type


    cluster_calib = _get_cluster_calib(parts)  #  ignored: in common sequence
    args['cluster_calib'] = cluster_calib
    args['cluster_calib_fex'] = cluster_calib.upper()
    args['data_type'] = _get_data_type(parts)
    args['jet_calib'] = _get_jet_calib(parts)

    scan = _get_scan_type(parts)
    # the trimming fex needs some further parameters
    if fex_type == 'jetrec_trimming':
        args['ptfrac'] = 0.04
        args['rclus'] = 0.2


    cut_keys = {'recoCutCalib': 'rcc', 'recoCutUncalib': 'rcu'}
    for k in cut_keys: args[k] = _get_reco_cut(k, parts)


    # set up the paramters to be put into the fex label
    fex_label_args = [args['fex_alg_name'],
                      _get_data_type(parts),
                      cluster_calib,
                      _get_jet_calib(parts),
                      scan]
    
    # add the reco pt cuts to the fex label only if these are not default values.
    for k in cut_keys:
        if 'Default' not in _get_reco_cut_str(k, parts):
            # like 'rcu' + str(int(args['recoCutUncalib'])),
            fex_label_args.append(cut_keys[k] + str(int(args[k])))
    
    fex_label = reduce(lambda x, y: x + y, fex_label_args)
   
    args['fex_label'] = fex_label 

    return fexparams_factory(fex_type, args)


def _do_recl(parts):
    fex_alg_name = _get_fex_alg_name(parts)
    return recluster_alg_re.search(fex_alg_name)


def _get_recl_params(parts):

    fex_alg_name = _get_fex_alg_name(parts)
    
    fex_label = reduce(lambda x, y: x + y,
                       (fex_alg_name,
                        '_',
                        _get_data_type(parts),
                        _get_cluster_calib(parts),
                        _get_jet_calib(parts),
                        _get_scan_type(parts)))

    fex_params = _get_fex_params(parts)
    fex_params.ptMinCut = 15.
    fex_params.etaMaxCut = 10.0  # no effect, to be removed
    fex_params.fex_label  =fex_label

    m = fex_alg_re.search(fex_alg_name)
    fex_params.merge_param  = m.group('merge_param')

    fex_params.jet_calib = 'nojcalib'
    fex_params.fex_alg_name  = fex_alg_name

    return fex_params


def _get_cleaner(parts):

    vals = set([part['cleaning'] for part in parts])

    if len(vals) != 1:
        msg = '%s: cannot determine cleaner ' %  err_hdr
        raise RuntimeError(msg)

    cleaner = vals.pop()

    _cleaner = cleaner_names.get(cleaner, None)
    if _cleaner is None:
        msg = '%s: unknown cleaner %s' %  (err_hdr, cleaner)
        raise RuntimeError(msg)

    return cleaner_names[cleaner]


def _setup_jet_vars(parts):
    "provide the attributes for each jet in a hypo"

    j_attrs = []
    for part in parts:
        mult = int(part['multiplicity'])
        for i in range(mult):
            j_attrs.append(JetAttributes(int(part['threshold']),
                                         part['etaRange'],
                                         part['smc']))

    return j_attrs

def _setup_etaet_vars(parts):
    """Delegate setting up the hypo parameters"""

    args = {}
    args['jet_attributes'] = _setup_jet_vars(parts)
    args['chain_name'] = cache['chain_name']
    args['cleaner'] = _get_cleaner(parts)
    args['isCaloFullScan'] = _get_scan_type(parts) == 'FS'
    args['triggertower'] = _get_data_type(parts) == 'TT'

    return hypo_factory('HLThypo2_etaet', args)


def _setup_singlemass_vars(parts):
    """Delegate setting up the hypo parameters
    16/01/2017 INCOMPLETE - work in prgress FIXME !!
    """

    args = {}
    args['jet_attributes'] = _setup_jet_vars(parts)
    args['chain_name'] = cache['chain_name']
    args['cleaner'] = _get_cleaner(parts)
    args['isCaloFullScan'] = _get_scan_type(parts) == 'FS'
    args['triggertower'] = _get_data_type(parts) == 'TT'

    return hypo_factory('HLThypo2_singlemass', args)


def _setup_dimass_deta_vars(parts):

    invm_string = _get_topo(parts, 'invm')

    args = {}

    m = invm_re.search(invm_string)
    if m:
        args['mass_min'] = float(m.groupdict()['mass_min'])
    else:
        args['mass_min'] = None

    deta_string = _get_topo(parts, 'deta')
    m = deta_re.search(deta_string)
    if m:
        args['dEta_min'] = float(m.groupdict()['dEta_min'])/10.
    else:
        args['dEta_min'] = None

    args['jet_attributes'] = _setup_jet_vars(parts)
    args['chain_name'] = cache['chain_name']
    args['cleaner'] = _get_cleaner(parts)

    return hypo_factory('HLThypo2_dimass_deta', args)


def _setup_ht_vars(parts):
    """make the HT hypo paramters attributes of this instance to 
    allow consistency checking with check_and_set"""

    ht_parts = [p for p in parts if p['trigType'] == 'ht']
    n_ht_parts = len(ht_parts)
    if n_ht_parts != 1:
        msg = '%s: _setup_ht_bars: expected 1 ht chainPart, found %d' % (
        err_hdr, n_ht_parts)

    ht_part = ht_parts[0]
    vals = set([part['etaRange'] for part in parts])
    if len(vals) > 1:
        msg = '%s: _setup_ht_bars: multiple eta ranges' % err_hdr
        raise RuntimeError(msg)

    args = {}
    args['eta_range'] = ht_part.get('etaRange', '')
    args['ht_threshold'] = int(ht_part['threshold'])
    args['chain_name'] = cache['chain_name']
    # set the default cuts on the jets contributing to the Et
    # sum to be 30 if not specified. Otherwise the expected form is
    #j\d+

    args['jet_et_threshold'] = 30.

    extra = ht_part['extra']
    if (not (extra == '' or extra.startswith('test'))):
        try:
            args['jet_et_threshold'] = float(part['extra'][1:])
        except:
            m = '%s unrecognized value for HT jet cut %s' % (
                err_hdr,
                str(part['extra'][1:]))
            
            raise RuntimeError(m)

    args['cleaner'] = ht_part.get('cleaning', 'noCleaning')

    return hypo_factory('HLThypo2_ht', args)

            
def _setup_tla_vars(parts):

    tla_string = _get_tla_string(parts)

    m = tla_re.search(tla_string)
    if m == None:
        m = '%s _setup_tla_vars unmatched ' \
            'tla string: %s regex: %s'  % (err_hdr,
                                           tla_string, 
                                           tla_re.pattern)
        raise RuntimeError(m)

    args = {}
    args.update(m.groupdict())
    args['chain_name'] = cache['chain_name']
    args['tla_string'] = tla_string
    args['indexhi'] = int(args['indexhi'])
    args['indexlo'] = int(args['indexlo'])
    args['mass_min'] = float(args['mass_min'])
    args['mass_max'] = float(args['mass_max'])


    return hypo_factory('HLThypo2_tla', args)


def _get_hypo_params(parts):

    hypo_type = _get_hypo_type(parts)

    hypo_setup_fn = {
        'HLThypo2_etaet': _setup_etaet_vars,
        'HLThypo2_dimass_deta': _setup_dimass_deta_vars,
        'HLThypo2_singlemass': _setup_singlemass_vars,
        'HLThypo2_tla': _setup_tla_vars,
        'HLThypo2_ht': _setup_ht_vars,}.get(hypo_type, None)

    if hypo_setup_fn is None:
        msg = '%s: unknown hypo type (JetDef bug) %s' % (
            err_hdr,
            str(hypo_type))
        raise RuntimeError(msg)

    return hypo_setup_fn(parts)

    
def chainConfigMaker(d):
    """Process input dictionary to create a ChainConfig object.
    A Chainfig obeject carries "algorithm-ready"" arguments (derived
    from the incoming dictionary) arranged by algorithm type.

    
    ChainConfigMaker.__init__ method acquires the incoming dictionary from
    the menu code, performs consistancy checks (eg the chainParts of the input
    dictionary are highly redundant. By affirmation of the menu code author
    many entries in the chain part dictionary will have the same values.
    If this is not the case, there is an error, or the chainParts has acquired
    new, currently unknown, semantics), and transforms its values into values
    directly usable by the Algoritm classes. These variables are attributes
    of this class.

    When the  function method (__call__) is invoked, these atrributes
    are transferred to a lighter weight ChainConfig object."""

    _reset_cache()

    # conerverter for cleaner names used in chain anmes, and those
    # used to instantiate the C++ converters (cleanerFactory keys)


    cc = ChainConfig()
    chain_name = d['chainName']
    _update_cache('chain_name', chain_name)
    cc.chain_name = chain_name

    parts = d['chainParts']
    _get_fex_alg_name(parts)

    do_recl =  _do_recl(parts)
        
    cc.run_hypo = _get_run_hypo(parts)
    cc.seed = d['L1item']
    cc.data_scouting = _get_data_scouting(parts)
    cc.run_rtt_diags = d['run_rtt_diags']

    md = MenuData()
    cc.menu_data = md
    
    md.cluster_params = _get_cluster_params(parts)
    md.data_type = _get_data_type(parts)
    md.fex_params = _get_fex_params(parts)
    md.scan_type = _get_scan_type(parts)
    md.trkopt = _get_trk_option(parts)

    if do_recl:
        md.second_fex_params = _get_recl_params(parts)

    md.hypo_params = _get_hypo_params(parts)
    
    return cc


if __name__ == '__main__':
    j85 = {'run_rtt_diags':False, 'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'smc': 'nosmc', 'trkopt': 'notrk','trigType': 'j', 'scan': 'FS', 'etaRange': '0eta320', 'threshold': '85', 'chainPartName': 'j85', 'recoAlg': 'a4', 'bTag': '', 'extra': '', 'calib': 'em', 'jetCalib': 'subjes', 'L1item': '', 'bTracking': '', 'dataType': 'tc', 'bMatching': [], 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'bConfig': [], 'multiplicity': '1', 'signature': 'Jet', 'addInfo': [], 'dataScouting': ''}], 'topo': [], 'groups': ['RATE:SingleJet', 'BW:Jet'], 'topoThreshold': None, 'topoStartFrom': False, 'chainCounter': 475, 'signature': 'Jet', 'L1item': 'L1_J20', 'chainName': 'j85'}

    j460_a10r = {'run_rtt_diags':False, 'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'threshold': '460', 'chainPartName': 'j460_a10r', 'recoAlg': 'a10r', 'trigType': 'j', 'bConfig': [], 'multiplicity': '1', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}], 'topo': [], 'chainCounter': 864, 'groups': ['RATE:SingleJet', 'BW:Jet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_HT150-J20s5.ETA31', 'chainName': 'j460_a10r'}

    _3j30 = {'run_rtt_diags':False, 'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'threshold': '30', 'chainPartName': '3j30', 'recoAlg': 'a4', 'trigType': 'j', 'bConfig': [], 'multiplicity': '3', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}], 'topo': [], 'chainCounter': 783, 'groups': ['RATE:MultiJet', 'BW:Jet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_TE10', 'chainName': '3j30'}

    j150_2j55_boffperf_split = {'run_rtt_diags':False, 'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'threshold': '150', 'chainPartName': 'j150', 'recoAlg': 'a4', 'trigType': 'j', 'bConfig': [], 'multiplicity': '1', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}, {'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': 'boffperf', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'threshold': '55', 'chainPartName': '2j55_boffperf_split', 'recoAlg': 'a4', 'trigType': 'j', 'bConfig': ['split'], 'multiplicity': '2', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}], 'topo': [], 'chainCounter': 1170, 'groups': ['RATE:MultiBJet', 'BW:BJet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_J75_3J20', 'chainName': 'j150_2j55_boffperf_split'}

    j0_0i1c200m400TLA = {'run_rtt_diags':False, 'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '0i1c200m400TLA', 'cleaning': 'noCleaning', 'threshold': '0', 'chainPartName': 'j0_0i1c200m400TLA', 'recoAlg': 'a4', 'trigType': 'j', 'bConfig': [], 'multiplicity': '1', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}], 'topo': [], 'chainCounter': 732, 'groups': ['RATE:MultiJet', 'BW:Jet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_J100', 'chainName': 'j0_0i1c200m400TLA'}

    j0_0i1c200m400TLA_1 = {'run_rtt_diags':False, 'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'recoCutCalib': 'rcc0', 'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '0i1c200m400TLA', 'cleaning': 'noCleaning', 'threshold': '0', 'chainPartName': 'j0_0i1c200m400TLA', 'recoAlg': 'a4', 'trigType': 'j', 'bConfig': [], 'multiplicity': '1', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}], 'topo': [], 'chainCounter': 732, 'groups': ['RATE:MultiJet', 'BW:Jet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_J100', 'chainName': 'j0_0i1c200m400TLA'}

    j0_0i1c200m400TLA_2 = {'run_rtt_diags':False, 'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'recoCutCalib': 'rcc0', 'recoCutUncalib': 'rcu0', 'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '0i1c200m400TLA', 'cleaning': 'noCleaning', 'threshold': '0', 'chainPartName': 'j0_0i1c200m400TLA', 'recoAlg': 'a4', 'trigType': 'j', 'bConfig': [], 'multiplicity': '1', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}], 'topo': [], 'chainCounter': 732, 'groups': ['RATE:MultiJet', 'BW:Jet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_J100', 'chainName': 'j0_0i1c200m400TLA'}

    _2j10_deta20_L1J12 = {'run_rtt_diags':False, 'EBstep': -1, 'signatures': '', 'stream': ['MinBias'], 'chainParts': [{'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': ['deta20'], 'TLA': '', 'cleaning': 'noCleaning', 'threshold': '10', 'chainPartName': '2j10_deta20_L1J12', 'recoAlg': 'a4', 'trigType': 'j', 'bConfig': [], 'multiplicity': '2', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}], 'topo': [], 'chainCounter': 721, 'groups': ['BW:MinBias', 'RATE:MinBias'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_J12', 'chainName': '2j10_deta20_L1J12'}

    _2j55_bmedium_ht300_L14J20 = {'run_rtt_diags':False, 'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'mergingOffset': -1, 'chainParts': [{'trkopt': 'notrk', 'smc': 'nosmc', 'bTracking': '', 'bTag': '', 'extra': '', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'threshold': '300', 'chainPartName': 'ht300_L14J20', 'recoAlg': 'a4', 'trigType': 'ht', 'bConfig': [], 'multiplicity': '1', 'scan': 'FS', 'L1item': '', 'signature': 'HT', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'dataScouting': ''}], 'signature': 'HT', 'mergingPreserveL2EFOrder': True, 'topo': [], 'chainCounter': 4032, 'L1item': 'L1_4J20', 'groups': ['RATE:MultiBJet', 'BW:BJet', 'BW:Jet'], 'mergingOrder': ['2j55_bmedium', 'ht300_L14J20'], 'topoThreshold': None, 'topoStartFrom': False, 'mergingStrategy': 'serial', 'chainName': '2j55_bmedium_ht300_L14J20'}

    j460_a10t_lcw_nojcalib_L1J100 = {'run_rtt_diags':False, 'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'threshold': '460', 'chainPartName': 'j460_a10t_lcw_nojcalib_L1J100', 'recoAlg': 'a10t', 'trigType': 'j', 'bConfig': [], 'multiplicity': '1', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'lcw', 'addInfo': [], 'jetCalib': 'nojcalib', 'L1item': ''}], 'topo': [], 'chainCounter': 757, 'groups': ['Rate:SingleJet', 'BW:Jet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_J100', 'chainName': 'j460_a10t_lcw_nojcalib_L1J100'}

    j440_a10r_L1J100 = {'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'threshold': '440', 'chainPartName': 'j440_a10r_L1J100', 'recoAlg': 'a10r', 'trigType': 'j', 'bConfig': [], 'multiplicity': '1', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}], 'topo': [], 'chainCounter': 663, 'groups': ['RATE:SingleJet', 'BW:Jet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_J100', 'chainName': 'j440_a10r_L1J100'}

    j440_a10r_L1J100_1 = {'run_rtt_diags':False,'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'recoCutCalib': 'rccDefault', 'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'threshold': '440', 'chainPartName': 'j440_a10r_L1J100', 'recoAlg': 'a10r', 'trigType': 'j', 'bConfig': [], 'multiplicity': '1', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}], 'topo': [], 'chainCounter': 663, 'groups': ['RATE:SingleJet', 'BW:Jet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_J100', 'chainName': 'j440_a10r_L1J100'}


    j440_a10r_L1J100_2 = {'run_rtt_diags':False,'EBstep': -1, 'signatures': '', 'stream': ['Main'], 'chainParts': [{'recoCutCalib': 'rcc0', 'smc': 'nosmc', 'trkopt': 'notrk', 'bTracking': '', 'bTag': '', 'scan': 'FS', 'dataType': 'tc', 'bMatching': [], 'etaRange': '0eta320', 'topo': [], 'TLA': '', 'cleaning': 'noCleaning', 'threshold': '440', 'chainPartName': 'j440_a10r_L1J100', 'recoAlg': 'a10r', 'trigType': 'j', 'bConfig': [], 'multiplicity': '1', 'extra': '', 'dataScouting': '', 'signature': 'Jet', 'calib': 'em', 'addInfo': [], 'jetCalib': 'subjesIS', 'L1item': ''}], 'topo': [], 'chainCounter': 663, 'groups': ['RATE:SingleJet', 'BW:Jet'], 'signature': 'Jet', 'topoThreshold': None, 'topoStartFrom': False, 'L1item': 'L1_J100', 'chainName': 'j440_a10r_L1J100'}

    # cc = chainConfigMaker(j460_a10r)
    # cc = chainConfigMaker(j85)
    # cc = chainConfigMaker(_3j30)
    # cc = chainConfigMaker(j150_2j55_boffperf_split)
    # cc = chainConfigMaker(j0_0i1c200m400TLA)
    # cc = chainConfigMaker(_2j10_deta20_L1J12)
    # cc = chainConfigMaker(_2j55_bmedium_ht300_L14J20)
    # cc = chainConfigMaker(_2j55_bmedium_ht300_L14J20)
    # cc = chainConfigMaker(j460_a10t_lcw_nojcalib_L1J100)
    # cc = chainConfigMaker(j460_a10t_lcw_nojcalib_L1J100)
    # cc = chainConfigMaker(j460_a10t_lcw_nojcalib_L1J100)
    # cc = chainConfigMaker(j440_a10r_L1J100_1)
    # cc = chainConfigMaker(j440_a10r_L1J100_2)
    # cc = chainConfigMaker(j0_0i1c200m400TLA_1)
    cc = chainConfigMaker(j0_0i1c200m400TLA_2)

    print cc
    
    def do_all():
        import sys
        sys.path.append('/tmp/sherwood')
        from triggerMenuXML_dicts import MC_pp_V5_dicts

        bad = []
        for d in MC_pp_V5_dicts: 
            d['run_rtt_diags'] = False
            print d['chainName']
            try:
                chainConfigMaker(d)
            except Exception, e:
                
                bad.append(d['chainName'])

                print 'bad', str(bad), str(e)
