# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
##
# @file PyUtils/python/moduleExists.py
# @author sss
# @date Oct 2019
# @brief Helper to test for the existence of a module.
#


import six


def moduleExists (modName):
    """Test for the existence of a module without actually importing it.

We could just do
  try:
    import modName
  except ImportError:
    ...
except that that has the potential to hide other errors."""

    # We have to do this differently in py2 vs py3.
    if six.PY34:
        import importlib.util
        return importlib.util.find_spec (modName) is not None
    else:
        import imp, importlib
        path = None
        if modName.find ('.') >= 0:
            parent, mod = modName.rsplit ('.', 1)
            parentmod = importlib.import_module (parent)
            path = parentmod.__path__
            modName = mod
        try:
            imp.find_module (modName, path)
        except ImportError:
            return False
        return True
