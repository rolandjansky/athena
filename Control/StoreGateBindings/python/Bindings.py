# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file: StoreGateBindings/python/Bindings.py
# @author: Wim Lavrijsen <WLavrijsen@lbl.gov>
# @author: Sebastien Binet <binet@cern.ch>

from __future__ import print_function

### data
__author__  = """
Wim Lavrijsen (WLavrijsen@lbl.gov),
Sebastien Binet (binet@cern.ch)
"""

### pythonizations for StoreGateSvc
def _setup():
    import cppyy
    # StoreGate bindings from dictionary
    cppyy.load_library( "libAthenaPythonDict" )      # for clidsvc
    cppyy.load_library( "libStoreGateBindingsDict" ) # for storegatesvc
    cppyy.load_library( "libStoreGateBindings" ) # not linked from libStoreGateBindingsDict in ROOT6

    
    global py_retrieve
    py_retrieve = cppyy.gbl.AthenaInternal.retrieveObjectFromStore

    global py_record
    py_record   = cppyy.gbl.AthenaInternal.recordObjectToStore

    global py_sg_contains
    py_sg_contains = cppyy.gbl.AthenaInternal.py_sg_contains

    global py_sg_getitem
    py_sg_getitem = cppyy.gbl.AthenaInternal.py_sg_getitem

    # retrieve the StoreGateSvc class
    global StoreGate, StoreGateSvc
    StoreGateSvc = cppyy.gbl.StoreGateSvc
    StoreGate    = cppyy.gbl.StoreGate

    # add specialized retrieve method
    def retrieve( self, klass, key = None ):
        ret = py_retrieve( self, klass, key )
        if ret and hasattr(ret,'setStore') and not ret.hasStore():
            if not hasattr(ret,'trackIndices') or ret.trackIndices():
                if py_sg_contains (self, 'SG::IConstAuxStore', key + 'Aux.'):
                    aux = py_retrieve (self, 'SG::IConstAuxStore', key + 'Aux.')
                    ret.setStore (aux)
        return ret
    StoreGateSvc.retrieve = retrieve

    # add specialized record method
    def record( self, obj, key, allowMods=True, resetOnly=True, noHist=False ):
        return py_record( self, obj, key, allowMods, resetOnly, noHist )
    StoreGateSvc.record = record

    # add specialized contains method
    def contains( self, klass_or_clid, key ):
        print ("---- StoreGateSvc.contains() ",  klass_or_clid, key)
        from builtins import int
        if isinstance(klass_or_clid, str):
            try:
                clid = int(klass_or_clid)
                klass = self._pyclidsvc.typename(clid)
            except ValueError:
                klass = str(klass_or_clid)
                pass
        elif isinstance(klass_or_clid, int):
            klass = self._pyclidsvc.typename(klass_or_clid)
        elif isinstance(klass_or_clid, type):
            klass = klass_or_clid.__name__
        else:
            raise TypeError(
                'argument 2 must be a typename, a clid or a type (got %r)' %
                type(klass_or_clid))
        return py_sg_contains( self, klass, key )
    StoreGateSvc.contains = contains

    # dict-pythonization of storegate: __setitem__
    def __setitem__ (self, key, obj):
        return py_record( self, obj, key, True, True, False )
    StoreGateSvc.__setitem__ = __setitem__

    # dict-pythonization of storegate: __getitem__
    def __getitem__ (self, key):
        try:
            ret = py_sg_getitem(self, key.encode())
        except LookupError as err:
            raise KeyError(str(err))
        if ret and hasattr(ret,'setStore') and not ret.hasStore():
            if not hasattr(ret,'trackIndices') or ret.trackIndices():
                if py_sg_contains (self, 'SG::IConstAuxStore', key + 'Aux.'):
                    aux = py_retrieve (self, 'SG::IConstAuxStore', key + 'Aux.')
                    ret.setStore (aux)
        return ret
    StoreGateSvc.__getitem__ = __getitem__

    # dict-pythonization of storegate: __len__
    def __len__ (self):
        return len(self.keys())
    StoreGateSvc.__len__ = __len__

    # make dump print, rather than return string
    def dump(self, fd = None):
        if fd is None:
            import sys
            fd = sys.stdout
        print (self.__class__._dump( self ), file=fd)
    StoreGateSvc._dump = StoreGateSvc.dump
    StoreGateSvc.dump  = dump

    # allow the use of the pythonized properties interface
    def __getattr__( self, attr ):
        try:                from GaudiPython.Bindings import iProperty
        except ImportError: from gaudimodule          import iProperty
        return getattr( iProperty("StoreGateSvc", self), attr )

    def __setattr__( self, attr, value ):
        try:                from GaudiPython.Bindings import iProperty
        except ImportError: from gaudimodule          import iProperty
        return setattr( iProperty("StoreGateSvc", self), attr, value )

    StoreGateSvc.__getattr__ = __getattr__
    StoreGateSvc.__setattr__ = __setattr__

    import AthenaPython.PyAthena as PyAthena
    StoreGateSvc._pyclidsvc = PyAthena.py_svc('ClassIDSvc')
    
    def keys(self, clid=None, allKeys=False):
        """return the list of keys for a given CLID (int, type or class_name)
           if clid is None, it will return the list of all keys.
           When 'allKeys' is True it will also return the key aliases.
        """
        if isinstance(clid, str):
            clid = self._pyclidsvc.clid(clid)
        if isinstance(clid, type):
            clid = self._pyclidsvc.clid(clid.__name__)
        if clid is None:
            return [p.name() for p in self.proxies()]
        return list(self._cpp_keys(clid, allKeys))
    StoreGateSvc._cpp_keys = StoreGateSvc.keys
    StoreGateSvc.keys = keys
    
    return

## execute method at module import
_setup()

## clean-up
del _setup
