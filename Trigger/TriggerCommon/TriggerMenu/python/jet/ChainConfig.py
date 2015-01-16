# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import pprint


def to_dict(o):
    """Recursive function to convert objects to dictionaries."""

    d = {}

    for k, v in o.__dict__.items():
        if hasattr(v, '__dict__'):
            d[k] = to_dict(v)
        else:
            d[k] = v

    return d


class ChainConfig(object):
    """Object that contains data reorganized into a manner convenient
    for configuring jet chains. Data is organised by sequence
    functionality. For eacample Trigger Toweer Scan, Fex extraction,
    JetFinding and hypothesis."""

    def __init__(self,
                 chain_name,
                 seed,
                 run_hypo,
                 test,
                 data_scouting,
                 menu_data,):

        self.chain_name = chain_name
        self.seed = seed
        self.run_hypo = run_hypo
        self.data_scouting = data_scouting

        # menu_data: data acquired from chainParts of menu dict
        self.menu_data = menu_data  
        self.test = test

    def __str__(self):
        d = to_dict(self)

        pp = pprint.PrettyPrinter(indent=4, depth=8)
        return pp.pformat(d)

