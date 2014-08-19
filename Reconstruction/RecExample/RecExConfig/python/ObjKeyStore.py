# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author Sebastien Binet

__all__ = [ 'ObjKeyStore', 'objKeyStore', 'cfgKeyStore' ]

# do not trace within ObjKeyStore 
from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/RecExConfig/ObjKeyStore*")

from AthenaCommon.KeyStore import CfgItemList, CfgKeyStore
class _ObjKeyStore( object ):

    def __init__(self, name="KeyStore"):
        object.__init__(self)

        self._store = CfgKeyStore(name)
        for label in CfgKeyStore.__slots__['Labels']:
            labelName = "%s%s" % (
                str(label)[0].upper(), # capitalize first letter
                str(label)[1:] )
            self.__dict__['isIn%s' % labelName] = self.__isInDict(label)
            self.__dict__['add%s'  % labelName] = self.__addToDict(label)
            self.__dict__['addManyTypes%s'  % labelName] = self.__addDictToDict(label)            
            self.__dict__['write%s'% labelName] = self.__writePy(label)
            self.__dict__['read%s' % labelName] = self.__readPy(label)

        return

#    def __isInDict(self, label):
#        fct = lambda key: self.__all[label].has_key(key)
#        class __helper:
#            def __call__(self,key):
#                return fct(key)
#        return __helper()


    def __getitem__(self, k):
        return self._store[k]
    
    def __setitem__(self, k, v):
        return self._store.__setitem__(k, v)

    def keys(self):
        return self._store.keys()

    def clear(self, label = None):
        return self._store.clear(label)
    
    def __isInDict(self, label):
        def isIn(d, k, v):
            return d.has_item( "%s#%s" % (k,v) )
        fct = lambda key, value : isIn(self._store[label], key, value)
        class __helper:
            def __call__(self, key, value):
                return fct(key, value)
        return __helper()

    def __addToDict(self, label):
        def fill(d, k, v):
            d.add( { k : v } )
            return            
        fct = lambda key, value: fill(self._store[label], key, value)
        class __helper:
            def __call__(self, key, value):
                return fct(key, value)
        return __helper()

    def __addDictToDict(self, label):
        # merge the dictionaries
        def fill(d, thedict):
            d.add(thedict)
            return
        fct = lambda thedict : fill(self._store[label], thedict)
        class __helper:
            def __call__(self, thedict):
                return fct(thedict)
        return __helper()

    def __writePy(self, label):
        fct = lambda fileName: self._store.write( fileName, label )
        class __helper:
            def __call__(self, fileName):
                return fct(fileName)
        return __helper()
        
    def __readPy(self, label):
        fct = lambda fileName: self._store.read( fileName, label )
        class __helper:
            def __call__(self, fileName):
                return fct(fileName)
        return __helper()

    def isInInput(self,type="",key="*"):
        flag = self.isInTransient(type,key) or self.isInInputFile(type,key)

        if not flag:
             from RecExConfig.RecConfFlags import recConfFlags
             if recConfFlags.AllowBackNavigation() and self.isInInputBackNav(type,key):
                flag=True
        return flag
        

    def Print(self):
        print self._store

    def __repr__(self):
        return "%r" % self._store
        
    def __str__(self):
        return "%s" % self._store
        
    pass # _ObjKeyStore

class _Singleton( object ):

    ## the object this singleton is holding
    ## No other object will be created...
    __obj = _ObjKeyStore()

    def __call__( self ):
        return self.__obj

    pass #_Singleton

ObjKeyStore = _Singleton()

## house-cleaning
del _Singleton
del _ObjKeyStore

# create a default ObjKeyStore instance (and the only one)
cfgKeyStore = ObjKeyStore()

# backward compat:
objKeyStore = cfgKeyStore

