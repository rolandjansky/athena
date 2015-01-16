# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Reformat input dictionary. Dictionary structure at
https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerCommon/ \
TriggerMenu/trunk/python/menu/SignatureDicts.py#L144
"""

import re
from fexparams_factory import fexparams_factory
from hypo_factory import hypo_factory
from clusterparams_factory import clusterparams_factory
from MenuData import MenuData
from ChainConfig import ChainConfig

class JetAttributes(object):
    """Per jet attributes. Used by  hypo algorithms."""
    eta_range_re = re.compile(
        r'(?P<eta_min>\d{1,3})eta(?P<eta_max>\d{1,3})')

    def __init__(self, threshold, eta_range):
        self.threshold = threshold
        self.eta_range = eta_range  # string like '0eta320'
        # eta_min, eta_max are floats
        self.eta_min, self.eta_max = self._etaFromString(eta_range)

    def _etaFromString(self, eta_range):
        match = self.eta_range_re.search(eta_range)
        if not match:
            msg = '%s.process_part() unknown eta range: %s does not match %s'
            msg = msg % (self.__class__.__name__,
                         eta_range,
                         self.eta_range_re.pattern)
            raise RuntimeError(msg)

        eta_min = float(match.group('eta_min'))/100.
        eta_max = float(match.group('eta_max'))/100.
        
        if eta_min > eta_max:
            msg = '%s.process_part()  eta range inverted: [%s,%s]' % (
                self.__class__.__name__,
                str(eta_min),
                str(eta_max)
                )
            raise RuntimeError(msg)

        return eta_min, eta_max

    def __str__(self):
        return 'thresh: %s eta_min: %s eta_max: %s' % (str(self.threshold),
                                                       str(self.eta_min),
                                                       str(self.eta_max))


class ChainConfigMaker(object):
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

    reco_alg_re = re.compile(r'^a\d+$')
    recluster_alg_re = re.compile(r'^a\d+r$')

    def __init__(self, d):
            
        self.err_hdr = '%s.process_part: ' % self.__class__.__name__
        self.n_parts = 0

        self.data_type = ''
        self.scan_type = ''
        self.jet_attributes = []

        self.chain_name = d['chainName']
        self.seed = d['L1item']
        self.test = d.get('test', '')
        [self.process_part(p) for p in d['chainParts']]

    def process_part(self, part):
        """Process chain parts. If there is more than one chain part,
        the fex data must always be the same: multiple chain parts
        convey information about different thresholds in for the
        jet hypo"""

        # -------------- cluster parameters -------------

        # the following dictionary translates the incoming value of
        # the calibration string to arguments relevant for
        # python class instantiation. The first is used by TrigCaloClusterMaker
        # while the second affects how JetRecTool is set up by
        # TrigHLTJetRec.

        # calib = {
        #    'em': {'cluster_dolc': False, 'do_jes': False},
        #    'lcw': {'cluster_dolc': True, 'do_jes': False},
        #    'had': {'cluster_dolc': False, 'do_jes': True}
        # }

        # calib used to (prior to 1/12/2014) refer to cluster
        # and jet calibration. Now it only determines the
        # cluster calibration.
        cluster_do_lcs = {
            'em':  False,
            'lcw': True,
            'had': False,
        }
        
        cluster_do_lc = cluster_do_lcs.get(part['calib'])
        
        if cluster_do_lc is None:
            msg = '%s Unknown cluster calibration %s, possible values: %s' % (
                err_hdr, p['calib'], str(cluster_do_lcs.keys())) 
            raise RuntimeError(msg)
            
        self.check_and_set('cluster_do_lc', cluster_do_lc)

        cluster_calib = 'lc' if self.cluster_do_lc else 'em'

        # set up an identifier for the clutering algorithm
        cluster_label = reduce(lambda x, y: x + y,
                               (part['dataType'],
                                cluster_calib,
                                part["scan"]))
        self.check_and_set('cluster_label', cluster_label)


        # the input data changes meaning depending on the chain,
        # all in the interest of having chain names that are alledgedly
        # easy for analysts to understand...
        # 13/01/2015... jet reclustering changes the meaning ofthe
        # dictionary contents. Straighten this out here to allow
        # systematic use of the dictionary.
        
        match = self.recluster_alg_re.search(part['recoAlg'])
        if match:
            # reclustering has been requested - fix dict:
            self.check_and_set('do_recluster', True)

            # ------- recluster parameters ----------
            # - the Et cut on input jets is hardwired.
            # - the eta cut on input jets is hardwired.
            # - the algo is calculated from the input dictionary value

            self.check_and_set('recl_fex_name', 'antikt')
            self.check_and_set('recl_ptMinCut', 15.)
            self.check_and_set('recl_etaMaxCut', 2.0)
            self.check_and_set('recl_merge_param', part['recoAlg'][1:-1])
            self.check_and_set('recl_jet_calib', 'nojcalib')

            fex_label = reduce(lambda x, y: x + y,
                           (part["recoAlg"],
                            '_',
                            part["dataType"],
                            cluster_calib,
                            part["jetCalib"],
                            part["scan"]))

            self.check_and_set('recl_fex_label', fex_label)
            # - input jets are made from akt4
            part['recoAlg'] = 'a4'
        else:
            self.check_and_set('do_recluster', False)

        jet_calib = part['jetCalib']
        self.check_and_set('jet_calib', jet_calib)
        
        # find the type of data the clustering will act on: topoclusters
        # or trigger towers are the possibilities for now.
        data_type = part['dataType']
        self.check_and_set('data_type', data_type)

        # -------------- fex parameters ----------------

        reco_alg = part['recoAlg']
        m = self.reco_alg_re.search(reco_alg)
        if not m:
            msg = '%s unknown reco alg: %s' % (self.err_hdr, reco_alg)
            raise RuntimeError(msg)

        merge_param = int(reco_alg[1:])
        self.check_and_set('fex_name', 'antikt')
        self.check_and_set('merge_param', merge_param)

        # generate a string that will be used to label the fex
        # instance and associated entities.
        fex_label = reduce(lambda x, y: x + y,
                           (part["recoAlg"],
                            part["dataType"],
                            cluster_calib,
                            part["jetCalib"],
                            part["scan"]))

        self.check_and_set('fex_label',
                           fex_label)
                                          
        # check whether to run the hypo
        run_hypo =  'perf' not in part['addInfo']
        
        self.check_and_set('run_hypo', run_hypo)


        # ------- data scouting parameters ----------
        
        ds = part.get('dataScouting', '')
        ds1 = 'ds1' if 'ds1' in ds  else ''
        ds2 = 'ds2' if 'ds2' in ds  else ''

        if ds1 and ds2:
            msg = '%s both "ds1" and "ds2" present in data scouting' \
            'string: %s' % (self.err_hdr, ds)
            raise RuntimeError(msg)

        self.check_and_set('data_scouting', ds1 + ds2)

        # --------  hypo parameters ----------------
        scan_type = part['scan']
        scan_types = ('', 'PS', 'FS')
        if scan_type not in scan_types:
            msg = '%s unknown scanType %s allowed: %s' % (self.err_hdr,
                                                          scan_type,
                                                          str(scan_types))
            raise RuntimeError(msg)
        
        # -----------------------------------------

        if scan_type != 'FS' and self.data_type == 'TT':
            msg = '%s: scanType %s set for Trigger tower scan' % (
                self.err_hdr, scan_type)
            raise RuntimeError(msg)

        self.check_and_set('scan_type', scan_type)

        mult = int(part['multiplicity'])
        threshold = int(part['threshold'])
        eta_range = part['etaRange']

        self.check_and_set('eta_range', eta_range)

        self.jet_attributes.extend(
            [(JetAttributes(threshold, eta_range)) for i in range(mult)])
        self.n_parts += 1

    def check_and_set(self, attr, val):
        """Set the value of attribute attr to val if the first chain part
        is being processed. For subsequent chain parts, ensure their
        attributevalues is the same (chain parts have much redundant
        information)."""

        if self.n_parts == 0:
            setattr(self, attr, val)

        if self.n_parts:
            my_val = getattr(self, attr)
            if my_val != val:
                msg = '%s attribute clash %s %s %s' % (self.err_hdr,
                                                       attr,
                                                       my_val,
                                                       val)
                raise RuntimeError(msg)

    def __call__(self):

        # obtain objects which state which the
        # algorithm to use, and the corresponding algorithm dependent
        # parameters

        cluster_args = {'do_lc': self.cluster_do_lc,
                        'label': self.cluster_label}
        
        cluster_params = clusterparams_factory(cluster_args)

        fex_args = {'merge_param': self.merge_param,
                    'jet_calib': self.jet_calib,
                    'fex_label': self.fex_label}

        fex_params = fexparams_factory(self.fex_name, fex_args)

        hypo_args = {
            'chain_name': self.chain_name,
            'eta_str': '',   # '' for now
            'jet_attributes': self.jet_attributes,
            'isCaloFullScan': self.scan_type == 'FS',
            'triggertower': self.data_type == 'TT'}

        hypo_params = hypo_factory(hypo_args)

        recluster_params = None
        if self.do_recluster:
            recl_args = {
                'ptMinCut': self.recl_ptMinCut,
                'etaMaxCut': self.recl_etaMaxCut,
                'merge_param': self.recl_merge_param,
                'jet_calib': self.recl_jet_calib,
                'fex_label': self.recl_fex_label,
            }
            recluster_params = fexparams_factory(
                'jetrec_recluster', recl_args)
            
        menu_data = MenuData(self.scan_type,
                             self.data_type,
                             fex_params=fex_params,
                             hypo_params=hypo_params,
                             cluster_params=cluster_params,
                             recluster_params=recluster_params,
                         )


        

        return ChainConfig(chain_name=self.chain_name,
                           seed=self.seed,
                           run_hypo=self.run_hypo,
                           test=self.test,
                           data_scouting=self.data_scouting,
                           menu_data=menu_data)
