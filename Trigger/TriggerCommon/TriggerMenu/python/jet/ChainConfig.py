# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import pprint


def to_dict(o):
    """Recursive function to convert objects to dictionaries."""

    d = {}

    for k, v in o.__dict__.items():
        if hasattr(v, '__dict__'):
            d[k] = to_dict(v)
        elif isinstance(v, tuple):
            d[k] = tuple([to_dict(e) for e in v])
        elif isinstance(v, list):
            d[k] = [to_dict(e) for e in v]
        else:
            d[k] = str(v)

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
                 data_scouting,
                 menu_data,
                 run_rtt_diags,
    ):

        self.chain_name = chain_name
        self.seed = seed
        self.run_hypo = run_hypo
        self.data_scouting = data_scouting
        self.run_rtt_diags = run_rtt_diags  # flag to run RTT the diagonistics.

        # menu_data: data acquired from chainParts of menu dict
        self.menu_data = menu_data  

    def __str__(self):
        d = to_dict(self)

        pp = pprint.PrettyPrinter(indent=4, depth=8)
        return pp.pformat(d)

