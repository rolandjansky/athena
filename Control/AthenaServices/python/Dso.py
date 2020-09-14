# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: AthenaServices/python/Dso.py
# @purpose: simple interface to the rootmap files to easily locate reflex
#           dictionaries (and load them)
# @author: Sebastien Binet <binet@cern.ch>
# @date:   September 2008

from __future__ import with_statement

__version__ = "$Revision: 1.12 $"
__author__  = "Sebastien Binet <binet@cern.ch>"
__doc__ = """\
simple interface to the rootmap files to easily locate reflex dictionaries \
(and load them)
"""

__all__ = [
    'registry',
    ]

### imports -------------------------------------------------------------------
import os
from PyUtils import Dso as _Dso
from PyUtils.Dso import _aliases

try:
    # attempt at fixing up the pyroot iterators...
    import RootUtils.PyROOTFixes  # noqa: F401
except ImportError:
    pass

### classes -------------------------------------------------------------------

class RflxEnums(object):
    S   = 4 # SCOPED
    SF  = 5 # SCOPED|FINAL
    SQ  = 6 # SCOPED|QUALIFIED
    SFQ = 7 # SCOPED|FINAL|QUALIFIED
    DICTSCOPE =SF

class DsoDb(_Dso.PyDsoDb):
    """
    The repository of 'rootmap' files (location, content,...) and a set of
    operations one can apply on them (load dict, query dicts,...)
    """
    def __init__(self):
        super(DsoDb, self).__init__('AthenaDsoDb')
        import AthenaCommon.Logging
        self.msg = AthenaCommon.Logging.logging.getLogger("AthenaDsoDb")
        #self.msg.setLevel(AthenaCommon.Logging.logging.VERBOSE)
        
        # inject the known aliases NOW
        for k,v in _aliases.iteritems():
            try:
                self.db[k] = self.db[v]
            except KeyError:
                self.msg.info("could not install alias [%s] -> [%s]", k,v)
        # make sure we'll be able to load dicts
        import cppyy
        #self._load_dict = cppyy.loadDict
        self._rflx_type = cppyy.gbl.RootType.ByName  
        return

    def has_type (self, typename):
        global _cpp_builtins
        if typename in _cpp_builtins:
            return True
        # need to massage a bit the typename to match ROOT naming convention
        typename = self._to_rootmap_name (typename)
        return self.db.has_key (typename) or \
               self.pf.has_key (typename)

    def load_type (self, typename):
        rflx_type = self.rflx_type(typename)
        if not(rflx_type is None):
            rflx_name = rflx_type.Name(RflxEnums.DICTSCOPE)
            if rflx_name != '':
                return rflx_name
        return None

    def rflx_type(self, typename):
        _rootmap_name = self._to_rootmap_name (typename)
        _rflx_name    = self._to_rflx_name    (typename)
        self.msg.verbose("------- loading type [%s]...", typename)
        
        global _cpp_builtins
        if typename in _cpp_builtins:
            self.msg.verbose(" ==> [ok] (builtin)")
            return self._rflx_type (_rflx_name)

        if not self.has_type (_rootmap_name):
            self.msg.verbose(" ==> [err] (no '%s')",_rootmap_name)
            return None
        libs = (self.db.get(_rootmap_name) or
                self.pf.get(_rootmap_name))
        if libs is None or len(libs) <= 0:
            return None
        from ctypes import cdll
        _load = cdll.LoadLibrary
        from cppyy import loadDict as _load
        lib = libs[0]
        self.msg.verbose("... %s",lib)
        _load (os.path.basename(lib.strip()))
        self.msg.verbose("... %s [done]",lib)
        self.msg.verbose(" ==> [ok]")

        # give the non-massaged name to reflex as this is what it expects
        # ...ROOT is sooo self-consistent...
        rflx_type = self._rflx_type (_rflx_name)
        if rflx_type.Name() != '':
            return rflx_type
        return None


    def visit(self, typename, visitor=None, ctx=None):
        self.msg.verbose("=== visiting %s", typename)
        if ctx is None:
            ctx = set()
        if isinstance (typename, str):
            rflx_type = self.load_type (typename)
            typename = rflx_type.Name (RflxEnums.DICTSCOPE)
        else:
            rflx_type = typename
            
        if not rflx_type:
            self.msg.verbose("**warning** no dict for [%s] !!", typename)
            return ctx
        
        ctx.add (typename)
        if visitor:
            visitor (rflx_type)
            
        if rflx_type.IsClass() or rflx_type.IsStruct():
            for i in range(rflx_type.BaseSize()):
                itype = rflx_type.BaseAt(i).ToType()
                itype_name = itype.Name(RflxEnums.DICTSCOPE)
                if itype_name != '' and not (itype_name in ctx):
                    self.load_type (itype_name)
                    ctx.add (itype_name)
                    ctx.update (self.visit (itype, visitor, ctx))
                            
        for i in range(rflx_type.DataMemberSize()):
            itype = rflx_type.DataMemberAt (i).TypeOf()
            itype = itype.RawType() if itype.IsPointer() else itype
            itype_name = itype.Name(RflxEnums.DICTSCOPE)
            if itype_name != '' and not (itype_name in ctx):
                self.load_type (itype_name)
                ctx.add (itype_name)
                # restore original type
                itype = rflx_type.DataMemberAt (i).TypeOf()
                itype_name = itype.Name(RflxEnums.DICTSCOPE)
                ctx.update (self.visit (itype, visitor, ctx))
        return ctx
    
    pass # class DsoDb

### clean-up ------------------------------------------------------------------
registry = _Dso.DsoDb()
