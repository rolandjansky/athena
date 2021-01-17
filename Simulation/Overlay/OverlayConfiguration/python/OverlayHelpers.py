#!/usr/bin/env python
"""Overlay configuration helpers

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

def accFromFragment(acc_string, flags):
    """Return initialized accumulator from post include fragment."""
    parts = acc_string.split('.')
    if len(parts) < 3:
        raise ValueError('Post include should be of the form Package.Module.Function')

    function = parts[-1]
    module = '.'.join(parts[:-1])

    from importlib import import_module
    loaded_module = import_module(module)
    function_def = getattr(loaded_module, function)

    return function_def(flags)
