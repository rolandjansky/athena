# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file FourMomUtils/python/Bindings.py
# @author Sebastien Binet <binet@cern.ch>
# @purpose load the python bindings to FourMomUtils' functions
#          this is to workaround a limitation of ROOT6 which cannot
#          autoload free functions (only classes)

def _installBindings():
    """load the python bindings to FourMomUtils' function, at module import
    """
    import cppyy
    cppyy.load_library('liblibFourMomUtilsDict')

    x = cppyy.gbl.xAOD.Helpers._importHelper()  # noqa: F841

    return

# execute at module import
_installBindings()

# clean-up
del _installBindings
