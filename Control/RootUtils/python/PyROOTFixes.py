# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Functions to work around PyROOT bugs for accessing DataVector.

:author: scott snyder
:contact: snyder@bnl.gov

PyROOT (as of root 5.26.00) has a bug that bites us when accessing
DataVector classes.

 - Attempts to use element access via [] on a class deriving from DataVector
   will cause an infinite recursion.

Importing this module will automatically apply this workarounds
for all DataVector classes (and classes deriving from them).

This module will also add comparison operators for iterator classes T
that have a RootUtils::PyROOTIteratorFuncs<T> specialization
generated.

There are also some fixes for performance problems with the pythonization
of TTree.  These may be enabled by calling enable_tree_speedups().


"""
__docformat__ = "restructuredtext en"

import ROOT
import cppyy
ROOT.TClass.GetClass("TClass")

def fix_dv_container (clname):
    # Now only a stub.
    return


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

#
# This function will be called for every new pyroot class created.
#
# We use this to fix up iterator comparison operators.
# If the new class appears to be an iterator and the class
# RootUtils::PyROOTIteratorFuncs<CLS> exists, then we install
# the comparison functions from that template class in the new class.
# 
def _pyroot_class_hook (cls):
    ll = cls.__name__.split ('<')
    ll[0] = ll[0].split('.')[-1]
    if ll[0].find ('iterator') >= 0:
        name = '<'.join (ll)
        ifuncsname = 'RootUtils::PyROOTIteratorFuncs<' + name
        if ifuncsname[-1] == '>':
            ifuncsname = ifuncsname + ' '
        ifuncsname = ifuncsname + '>'
        if _getClassIfDictionaryExists (ifuncsname):
            ifuncs = getattr (ROOT, ifuncsname, None)
            if hasattr (ifuncs, 'eq'):
                # Note: To prevent Pythonize.cxx from changing these out from
                # under us, __cpp_eq__/__cpp_ne__ must be pyroot MethodProxy's,
                # and __eq__/__ne__ must _not_ be MethodProxy's.
                cls.__eq__ = lambda a,b: ifuncs.eq(a,b)
                cls.__ne__ = lambda a,b: ifuncs.ne(a,b)
                cls.__cpp_eq__ = ifuncs.eq
                cls.__cpp_ne__ = ifuncs.ne
            if hasattr (ifuncs, 'lt'):
                cls.__lt__ = lambda a,b: not not ifuncs.lt(a,b)
                cls.__gt__ = lambda a,b: not not ifuncs.lt(b,a)
                cls.__ge__ = lambda a,b: not ifuncs.lt(a,b)
                cls.__le__ = lambda a,b: not ifuncs.lt(b,a)
    return


ROOT.RootUtils.PyROOTTypePatch.initialize (ROOT.PyRootType)
