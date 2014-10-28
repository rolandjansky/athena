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
                 name,
                 seed,
                 test,
                 tt_menudata,
                 jr_menudata):

        self.name = name
        self.seed = seed

        # the following flags will guide sequence construction
        self.tt_menudata = tt_menudata
        self.jr_menudata = jr_menudata
        self.test = test

    def __str__(self):
        d = to_dict(self)

        pp = pprint.PrettyPrinter(indent=4, depth=8)
        return pp.pformat(d)

