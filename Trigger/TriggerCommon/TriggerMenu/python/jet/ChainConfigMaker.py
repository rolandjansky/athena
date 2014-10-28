# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Reformat input dictionary. Dictionary structure at
https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerCommon/ \
TriggerMenu/trunk/python/menu/SignatureDicts.py#L144
"""

import re
from fex_factory import fex_factory
from hypo_factory import hypo_factory
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
    """Process input dictionary to create a ChainConfig object"""
    reco_alg_re = re.compile(r'^a\d+$')
    menudata_labels = {'tc': 'jr_menudata', 'TT': 'tt_menudata'}

    def __init__(self, d):
            
        self.err_hdr = '%s.process_part: ' % self.__class__.__name__
        self.n_parts = 0

        self.data_type = ''
        self.scan_type = ''
        self.menudata_label = ''
        self.jet_attributes = []

        self.chain_name = d['chainName']
        self.chainpart_name = d['chainParts'][0]['chainPartName']  
        self.seed = d['L1item']
        self.test = d.get('test', '')
        [self.process_part(p) for p in d['chainParts']]

    def process_part(self, part):
        """Process chain parts. If there is more than one chain part,
        the fex data must always be the same: multiple chain parts
        convey information about different thresholds in for the
        jet hypo"""

        data_type = part['dataType']
        self.check_and_set('data_type', data_type)

        menudata_label = self.menudata_labels[data_type]
        self.check_and_set('menudata_label', menudata_label)
        
        # fex parameters

        reco_alg = part['recoAlg']
        m = self.reco_alg_re.search(reco_alg)
        if not m:
            msg = '%s unknown reco alg: %s' % (self.err_hdr, reco_alg)
            raise RuntimeError(msg)

        merge_param = int(reco_alg[1:])
        self.check_and_set('fex_name', 'antikt')
        self.check_and_set('merge_param', merge_param)

        calib = part['calib']
        self.check_and_set('calib', calib)

        # ----------- pack the hypo parameters

        scan_type = part['scan']
        scan_types = ('', 'PS', 'FS')
        if scan_type not in scan_types:
            msg = '%s unknown scanType %s allowed: %s' % (self.err_hdr,
                                                          scan_type,
                                                          str(scan_types))
            raise RuntimeError(msg)
        
        if scan_type and data_type == 'TT':
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

        kwds = {'jr_menudata': None, 'tt_menudata': None}

        fex_args = {'merge_param': self.merge_param,
                    'calib': self.calib,
                    'fex_input': self.data_type,
                    'level': self.data_type}

        fex = fex_factory(self.fex_name, fex_args)

        hypo_args = {#'chain_name': self.chain_name,
                     'chain_name': self.chainpart_name,
                     'eta_str': '',   # '' for now
                     'jet_attributes': self.jet_attributes,
                     'isCaloFullScan': self.scan_type == 'FS',
                     'triggertower': self.data_type == 'TT'}

        hypo = hypo_factory(hypo_args)

        menu_data = MenuData(self.scan_type, fex, hypo)

        kwds[self.menudata_label] = menu_data
        return ChainConfig(name=self.chain_name,
                           seed=self.seed,
                           test=self.test,
                           **kwds)
