# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import pprint


def to_dict(o):
    """Recursive function to convert objects to dictionaries."""

    d = {}
    try:
        for k, v in o.__dict__.items():
            if hasattr(v, '__dict__'):
                d[k] = to_dict(v)
            elif isinstance(v, tuple):
                for e in v:
                    if hasattr(e, '__dict__'):
                        d[k] = to_dict(e)
                    else:
                        d[k] = str(e)
            elif isinstance(v, list):
                for e in v:
                    if hasattr(e, '__dict__'):
                        d[e.__class__.__name__] = to_dict(e)
                    else:
                        d[e] = str(e)
            else:
                d[k] = v
    except AttributeError:
        pass

    
    return d


class ChainConfig(object):
    """Object that contains data reorganized into a manner convenient
    for configuring jet chains. Data is organised by sequence
    functionality. For eacample Trigger Toweer Scan, Fex extraction,
    JetFinding and hypothesis."""

    def __init__(self):

        self.chain_name = None
        self.seed = None
        self.run_hypo = None
        self.data_scouting = None
        self.run_rtt_diags = None  # flag to run RTT the diagonistics.

        # menu_data: data acquired from chainParts of menu dict
        self.menu_data = None

    def __str__(self):
        d = to_dict(self)
        # return str(self.__dict__)

        pp = pprint.PrettyPrinter(indent=4, depth=8)
        return pp.pformat(d)

if __name__ == '__main__':
    cc = ChainConfig()
    print cc
