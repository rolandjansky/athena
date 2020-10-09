# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""Functions to work around PyROOT bugs for accessing DataVector.

:author: scott snyder
:contact: snyder@bnl.gov

Importing this module will automatically apply workarounds
for known PyROOT issues.

"""
__docformat__ = "restructuredtext en"

import ROOT
import cppyy  # noqa: F401
ROOT.TClass

def fix_method (clname, methname):
    # Now only a stub.
    return

def enable_tree_fixes ():
    """Additional pythonizations for TTree."""
    ROOT.RootUtils.PyROOTTTreePatch.Initialize (ROOT.TTree,
                                                ROOT.TChain,
                                                ROOT.TBranch)
    return

def enable_pickling ():
    """Enable pickling of object proxy's"""
    import libPyROOT
    ROOT.RootUtils.PyROOTPickle.Initialize (libPyROOT, ROOT.ObjectProxy)
    del libPyROOT
    return


# Convert from class name to TClass; try to avoid autoparsing.
def _getClassIfDictionaryExists (cname):
    cl = ROOT.gROOT.GetListOfClasses().FindObject(cname)
    if cl:
        if cl.IsLoaded() and cl.HasDictionary(): return cl
        return None
    if ROOT.gInterpreter.GetClassSharedLibs (cname):
        cl = ROOT.TClass.GetClass (cname)
        if cl.HasDictionary(): return cl
    return None

# Force-load the main Gaudi library if it's available. If this package's library
# is loaded before Gaudi, the application gets into a weird state. Hopefully we
# will be able to remove this with future compiler versions. (GCC 10?...)
if cppyy.gbl.gSystem.FindDynamicLibrary( 'libGaudiKernel', True ):
    cppyy.load_library( 'libGaudiKernel' )
    pass

ROOT.RootUtils.PyLogger
ROOT.RootUtils.PyROOTTypePatch.initialize()
