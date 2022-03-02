# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

logger = logging.getLogger( __name__)
logger.setLevel(DEBUG)

import math
from copy import deepcopy

class ConditionDefaults:
    def __init__(self):

        # we have neta, ceta and p eta. Each is a key for
        # TrigJetConditionConfig_signed_eta. We need a way to
        # interprete the scenario, atking into account minus signs
        # cannot appear in the chain name.
        self.defaults = {
            'et': {'min': '0', 'max': 'inf'},
            'pt': {'min': '0', 'max': 'inf'},
            'eta': {'min': '0', 'max': 'inf'},
            'neta': {'min': '-inf', 'max': '0'},  # scenario etas: -eta  -eta
            'ceta': {'min': '-inf', 'max': 'inf'},  # scenario etas: -eta, +eta
            'peta': {'min': '0', 'max': 'inf'},  # scenario etas: +eta +eta
            'nphi': {'min': '-inf', 'max': '0'},  # scenario phis: -phi  -phi
            'cphi': {'min': '-inf', 'max': 'inf'},  # scenario phis: -phi, +phi
            'pphi': {'min': '0', 'max': 'inf'},  # scenario phis: +phi +phi
            'djmass': {'min': '0', 'max': 'inf'},
            'djdeta': {'min': '0', 'max': 'inf'},
            'djdphi': {'min': '0', 'max': str(math.pi)},
            'ht': {'min': '0', 'max': 'inf'},
            'smc': {'min': '0', 'max': 'inf'},
            'jvt': {'min': '0', 'max': 'inf'},
            'bdips': {'min': '-inf', 'max': 'inf'},
            'momCuts': {'min': '-inf', 'max': 'inf'},
        }

        self.scale_factor = {
            'et': 1000.,
            'pt': 1000.,
            'eta': 0.01,
            'neta': 0.01,
            'ceta': 0.01,
            'peta': 0.01,
            'nphi': 0.01,
            'cphi': 0.01,
            'pphi': 0.01,
            'djmass': 1000.,
            'djdeta': 0.01, 
            'djdphi': 0.01,
            'ht': 1000.,
            'smc': 1000.,
            'jvt': 0.01,
            'bdips': 1.,
            'momCuts': 0.01,
        }
  
    def __call__(self, key, lo='', hi=''):
        vals = deepcopy(self.defaults[key])
        if lo: vals['min'] = self.scale(key, lo)
        if hi: vals['max'] = self.scale(key, hi)
        return vals

    def scale(self, key, val):
        return str(float(val) * self.scale_factor[key])

defaults = ConditionDefaults()
    
