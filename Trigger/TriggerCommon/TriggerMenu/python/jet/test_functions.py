# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Excerciser functions for JetDef.py"""

from JetDef import generateHLTChainDef

def run_strawman_test(use_atlas_config=True, debug=True):
    from MC_pp_V5_dicts import MC_pp_V5_dicts

    return [generateHLTChainDef(d) for d in MC_pp_V5_dicts]


def run_from_dict_file():
    """Function to run jetdef from a text file containing a single dict"""
    
    from test_dicts import test_dicts
    return [generateHLTChainDef(d) for d in test_dicts]


