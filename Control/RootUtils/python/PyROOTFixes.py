# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Functions to work around PyROOT bugs for accessing DataVector.

:author: scott snyder
:contact: snyder@bnl.gov

Importing this module will automatically apply workarounds
for known PyROOT issues.

There are also some fixes for performance problems with the pythonization
of TTree.  These may be enabled by calling enable_tree_speedups().


"""
__docformat__ = "restructuredtext en"

import ROOT
import cppyy
ROOT.TClass.GetClass("TClass")

def fix_method (clname, methname):
    # Now only a stub.
    return

def enable_tree_speedups ():
    """Enable speedups for TTree element access"""
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


ROOT.RootUtils.PyLogger
ROOT.RootUtils.PyROOTTypePatch.initialize()
