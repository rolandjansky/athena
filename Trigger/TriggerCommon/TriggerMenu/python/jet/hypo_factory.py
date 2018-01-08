# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict

def hypo_factory(key, args):

    klass = {
        'HLThypo2_etaet': HLThypo2_etaet,
        'HLThypo2_singlemass': HLThypo2_singlemass,
        'HLThypo2_ht': HLThypo2_ht,
        'HLThypo2_tla': HLThypo2_tla,
        'HLThypo2_dimass_deta': HLThypo2_dimass_deta,
         }.get(key)

    if key == None:
        raise RuntimeError('hypo_factory: unknown key %s' % key)
    else:
        return klass(args)

class HypoAlg(object):
    """ Argument checking class that holds the  parameters for an
    Hypo (decision taking) python alg. Provide funtions to help
    with the various formatting needs of jet multiplicities."""

    def __init__(self, ddict):

        self._check_args(ddict)
        self.__dict__.update(ddict)

    def check_missing_args(self, must_have, ddict):

        missing = [k for k in must_have if k not in ddict]
        if missing:
            m = '%s._check_args: missing item %s, expected: %s' % (
                self.__class__.__name__,
                ' '.join(missing),
                ' '.join(must_have))

            raise RuntimeError(m)

    def __str__(self):
        s = ['%s: %s' % (k, str(v)) for k, v in self.__dict__.items()]
        return '\n'.join(s)



def jet_attributes_toString(jet_attributes):
    """Return a string of form '%d+_%s_%d+' where the digits prehe ceeding the
    string are multiplicy, the string is the eta range and the digits
    after the string is the threshold. Used for trigger element and
    standard jet hypo names.

    The function has been etended to add the jet mass range cut t othe
    eta range cut if the mass cuts are provided.
    """

    counter = defaultdict(lambda: defaultdict(int))

    def bump(j):
        counter[(j.eta_range, j.smc_range)][j.threshold] += 1

    [bump(j) for j in jet_attributes]

    def mult_range_thresh(eta_range, smc_range, thresh_dict, thresh):
        if smc_range == 'nosmc':
            return '%d_%s_%d' % (thresh_dict[thresh], eta_range, thresh)
        
        return '%d_%s_%s_%d' % (
            thresh_dict[thresh], eta_range, smc_range, thresh)
        
    l = []
    for (eta_range, smc_range), thresh_dict in counter.items():
        l.extend([mult_range_thresh(eta_range, smc_range, thresh_dict, thresh)
                  for thresh in thresh_dict])
    l.sort()
    return '_'.join(l)


class JetHypo(HypoAlg):
    """ Argument checking class that holds the  parameters for an
    Hypo (decision taking) python alg. Derives from HypoAlg, adds
    the information as to whether the trigger tower sequences are
    present in the chain."""

    def __init__(self, ddict):
        HypoAlg.__init__(self, ddict)

    def _check_args(self, ddict):
        """check the constructor args"""

        must_have = ('chain_name',
                     'jet_attributes',
                     'isCaloFullScan',
                     'triggertower')


        HypoAlg.check_missing_args(self, must_have, ddict)

        jet_attributes = ddict['jet_attributes']
        if not jet_attributes:
            m = '%s._check_args: "jet_attributes" list is empty  %s' % (
                self.__class__.__name__,
                str(ddict))
            raise RuntimeError(m)

        for ja in jet_attributes:
            if not isinstance(ja.threshold, int):
                m = '%s._check_args: non-integer threshold %s' % (
                    self.__class__.__name__,
                    ja.threshold)
                raise RuntimeError(m)

    def attributes_toString(self):
        """Return a string of form '%d+_%s_%d+' where the digits preceeding the
        string are multiplicy, the string is the eta range and the digits
        after the string is the threshold. Used for trigger element and
        standard jet hypo names."""

        return jet_attributes_toString(self.jet_attributes)


class JetStandardHypo(JetHypo):

    def __init__(self, ddict):
        JetHypo.__init__(self, ddict)
        self.hypo_type = 'run1hypo'
        
    def _check_args(self, ddict):
        JetHypo._check_args(self, ddict)

        jet_attributes = ddict['jet_attributes']
        eta_ranges = set([j.eta_range for j in jet_attributes])
        if len(eta_ranges) != 1:
            raise RuntimeError('expected 1 eta range, found %d' %
                               len(eta_ranges))
    def eta_range(self):
        """return the of the eta_range of the first jet attribute instance.
        self._check_arhgs ensures all jet_attributes
        have the same eta_range."""
        return self.jet_attributes[0].eta_range


class JetSingleEtaRegionHypo(JetStandardHypo):

    def __init__(self, ddict):
        JetStandardHypo.__init__(self, ddict)
        self.hypo_type = 'HLTSRhypo'


class JetMaximumBipartiteHypo(JetHypo):

    def __init__(self, ddict):
        JetHypo.__init__(self, ddict)
        self.hypo_type = 'HLThypo'


class HLThypo2_etaet(JetHypo):

    def __init__(self, ddict):
        JetHypo.__init__(self, ddict)
        self.hypo_type = 'HLThypo2_etaet'


class HLThypo2_singlemass(JetHypo):

    def __init__(self, ddict):
        JetHypo.__init__(self, ddict)
        self.hypo_type = 'HLThypo2_singlemass'


class HTHypoBase(HypoAlg):
    """ Store paramters for the HT hypoAlg"""

    
    def __init__(self, ddict):
        HypoAlg.__init__(self, ddict)
        self.hypo_type = 'HT'


    def _check_args(self, ddict):
        """check the constructor args"""

        must_have = ('chain_name',
                     'eta_range',
                     'ht_threshold',
                     'jet_et_threshold')

        HypoAlg.check_missing_args(self, must_have, ddict)

    def attributes_toString(self):
        return '_'.join(['ht' + str(int(self.ht_threshold)),
                         self.eta_range,
                         'j' + str(int(self.jet_et_threshold))])


class HTHypo(HTHypoBase):
    """ Store paramters for the HT hypoAlg"""

    def __init__(self, ddict):
        HTHypoBase.__init__(self, ddict)
        self.hypo_type = 'HT'


class HLThypo2_ht(HTHypoBase):
    """ Store paramters for the HT hypoAlg"""

    def __init__(self, ddict):
        HTHypoBase.__init__(self, ddict)
        self.hypo_type = 'HLThypo2_ht'



class TLABase(HypoAlg):
    """ Store paramters for the TLA hypoAlg"""

    def __init__(self, ddict):
        HypoAlg.__init__(self, ddict)


    def _check_args(self, ddict):
        """check the constructor args"""

        must_have = ('chain_name',
                     'tla_string',
                     'indexlo',
                     'indexhi',
                     'mass_min',
                     'mass_max',)
#                    'ystar_min',
#                    'ystar_max')


        HypoAlg.check_missing_args(self, must_have, ddict)

    def attributes_toString(self):
        return 'tla_' + self.tla_string
        

class TLAHypo(TLABase):
    """ Store paramters for the HT hypoAlg"""

    def __init__(self, ddict):
        TLABase.__init__(self, ddict)
        self.hypo_type = 'tla'
        

class HLThypo2_tla(TLABase):
    """ Store paramters for the HT hypoAlg"""

    def __init__(self, ddict):
        TLABase.__init__(self, ddict)
        self.hypo_type = 'HLThypo2_tla'


class HLThypo2_dimass_deta(HypoAlg):
    """ Store paramters for the dijet mass DEta hypoAlg"""

    def __init__(self, ddict):
        HypoAlg.__init__(self, ddict)
        self.hypo_type = 'HLThypo2_dimass_deta'

    def _check_args(self, ddict):
        """check the constructor args"""
        
        must_have = ('mass_min',
                     'dEta_min',)

        HypoAlg.check_missing_args(self, must_have, ddict)

    def attributes_toString(self):
        # use ints for invm, deta as this string is used
        # in the Algorithm name, and Gausi does not allow '.'
        # in names.
        
        s = jet_attributes_toString(self.jet_attributes)
        if self.mass_min is not None:
            s +=  '_invm_' + str(int(self.mass_min))
        if self.dEta_min is not None:
            s +=  '_deta_' + str(int(self.dEta_min))

        return s
