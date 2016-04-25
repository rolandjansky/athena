# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file FourMomUtils/python/Bindings.py
# @author Sebastien Binet <binet@cern.ch>
# @purpose load the python bindings to FourMomUtils' functions
#          this is to workaround a limitation of PyCintex which cannot (yet?)
#          autoload free functions (only classes)

def _installBindings():
    """load the python bindings to FourMomUtils' function, at module import
    """
    import ROOT
    import cppyy
    cppyy.loadDictionary('libFourMomUtilsDict')

    x = cppyy.gbl.xAOD.Helpers._importHelper()

    return

# execute at module import
_installBindings()

# clean-up
del _installBindings
