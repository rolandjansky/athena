# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict


def hypo_factory(args):

    # no choice yet
    return JetRecHypoAlg(args)

class HypoAlg(object):
    """ Argument checking class that holds the  parameters for an
    Hypo (decision taking) python alg. Provide funtions to help
    with the various formatting needs of jet multiplicities."""

    def __init__(self, ddict):

        self._check_args(ddict)
        self.__dict__.update(ddict)

    def __str__(self):
        s = ['%s: %s' % (k, str(v)) for k, v in self.__dict__.items()]
        return '\n'.join(s)

    def _check_args(self, ddict):
        """check the constructor args"""

        must_have = ('chain_name',
                     'eta_str',
                     'jet_attributes',
                     'isCaloFullScan')

        missing = [k for k in must_have if k not in ddict]
        if missing:
            m = '%s._check_args: missing item %s, expected: %s' % (
                self.__class__.__name__,
                ' '.join(missing),
                ' '.join(must_have))

            raise RuntimeError(m)

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

        eta_ranges = set([j.eta_range for j in jet_attributes])
        if len(eta_ranges) != 1:
            raise RuntimeError('expected 1 eta range, found %d' %
                               len(eta_ranges))

    def eta_range(self):
        """return the of the eta_range of the first jet attribute instance.
        self._check_arhgs ensures all jet_attributes
        have the same eta_range."""
        return self.jet_attributes[0].eta_range

    def jet_attributes_tostring(self):
        """Return a string of form '%d+_%s_%d+' where the digits preceeding the
        string are multiplicy, the string is the eta range and the digits
        after the string is the threshold. Used for trigger element"""

        counter = defaultdict(lambda: defaultdict(int))

        def bump(j):
            counter[j.eta_range][j.threshold] += 1

        [bump(j) for j in self.jet_attributes]

        def mult_range_thresh(eta_range, thresh_dict, thresh):
            return '%d_%s_%d' % (thresh_dict[thresh], eta_range, thresh)
        
        l = []
        for eta_range, thresh_dict in counter.items():
            l.extend([mult_range_thresh(eta_range, thresh_dict, thresh)
                      for thresh in thresh_dict])
        l.sort()
        return '_'.join(l)


class TriggerTowerHypoAlg(HypoAlg):
    def __init__(self, ddict):
        HypoAlg.__init__(self, ddict)


class JetRecHypoAlg(HypoAlg):
    """ Argument checking class that holds the  parameters for an
    Hypo (decision taking) python alg. Derives from HypoAlg, adds
    the information as to whether the trigger tower sequences are
    present in the chain."""

    def __init__(self, ddict):
        HypoAlg.__init__(self, ddict)

    def _check_args(self, ddict):
        """check the constructor args"""

        HypoAlg._check_args(self, ddict)
        if 'triggertower' not in ddict:
            m = '%s._check_args: missing triggertower flag'
            raise RuntimeError(m)
