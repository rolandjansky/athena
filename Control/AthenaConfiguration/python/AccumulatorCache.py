#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
_msg = logging.getLogger('AccumulatorCache')

import functools
from copy import deepcopy

class AccumulatorDecorator:
    """Class for use in function decorators, implements memoization.

    Instances are callable objects that use the
    hash value calculated from positional and keyword arguments
    to implement memoization. Methods for suspending and 
    resuming memoization are provided.
    """

    _memoize = True

    VERIFY_NOTHING = 0
    VERIFY_HASH = 1

    def __init__(self , func , size , verify , deepCopy): 
        """The constructor is typically called in a function returning a decorator.

        Args:
            func:       function for memoization
            size:       maximum size for the cache of function results
            verify:     takes two possible values
                        
                        AccumulatorDecorator.VERIFY_NOTHING -   the cached function result is returned with no verification
                        AccumulatorDecorator.VERIFY_HASH -      before returning the cached function value, the hash of the
                                                                result is checked to verify if this object was not modified
                                                                between function calls
            deepCopy:   if True a deep copy of the function result will be stored in the cache.

        """
        functools.update_wrapper(self , func)
        self._maxSize = size
        self._func = func
        self._cache = {}
        self._resultCache = {}
        self._hits = 0
        self._misses = 0
        self._verify = verify
        self._deepcopy = deepCopy

    def __str__(self):
        return "|cache size : " + str(len(self._cache)) + " , misses : " + str(self._misses) + " , hits : " + str(self._hits) + " , function : " + str(self._func.__name__) + "|"

    def getInfo(self):
        """Rerurn a dictionary with information about the cache size and cache usage"""
        return {"cache_size" : len(self._cache) , "misses" : self._misses , "hits" : self._hits , "function" : self._func , "result_cache_size" : len(self._resultCache)}

    @classmethod
    def suspendCaching(cls):
        """Suspend memoization for all instances of AccumulatorDecorator."""
        cls._memoize = False

    @classmethod
    def resumeCaching(cls):
        """Resume memoization for all instances of AccumulatorDecorator."""
        cls._memoize = True

    def __call__(self , *args , **kwargs):
        if(AccumulatorDecorator._memoize):
            hashable_args = True
            for a in args:
                if(not hasattr(a , "athHash")):
                    hashable_args = False
                    _msg.debug("Positional argument to AccumulatorDecorator __call__ is not hashable.")
                    break
            for k , v in kwargs.items():
                if(not hasattr(k , "athHash")):
                    hashable_args = False
                    _msg.debug("Key value in keyword argument to AccumulatorDecorator __call__ is not hashable.")
                    break
                if(not hasattr(v , "athHash")):
                    hashable_args = False
                    _msg.debug("Value in keyword argument to AccumulatorDecorator __call__ is not hashable.")
                    break
            if(hashable_args):
                # frozen set makes the order of keyword arguments irrelevant
                hsh = hash(tuple((tuple(a.athHash() for a in args) , frozenset((k.athHash() , v.athHash()) for k , v in kwargs.items()))))

                if(hsh in self._cache):
                    res = self._cache[hsh]

                    if(AccumulatorDecorator.VERIFY_HASH == self._verify):
                        resHsh = self._resultCache[hsh]
                        chkHsh = None
                        if(hasattr(res , "athHash")):
                            chkHsh = res.athHash()
                        if((not (chkHsh is None)) and (not (resHsh is None))): 
                            # hashes are available and can be compared
                            if(chkHsh != resHsh):
                                _msg.debug("Hash of function result, cached using AccumulatorDecorator, changed.")
                        else:
                                _msg.debug("Hash of function result, cached using AccumulatorDecorator, not available for verification.")
                        if((chkHsh is None) or (resHsh is None) or resHsh != chkHsh): 
                            # at least one hash is not available (None) so no verification can be performed 
                            # or
                            # hashes are different
                            self._misses += 1
                            res = self._func(*args , **kwargs)
                            self._cache[hsh] = res
                            self._resultCache[hsh] = None
                            if(hasattr(res , "athHash")):
                                self._resultCache[hsh] = res.athHash()
                        else:
                            self._hits += 1
                    elif(AccumulatorDecorator.VERIFY_NOTHING == self._verify):
                        self._hits += 1
                    else:
                        _msg.debug("Incorrect value of verify in AccumulatorDecorator, assuming AccumulatorDecorator.VERIFY_NOTHING.")
                        self._hits += 1

                    if(self._deepcopy):
                        return deepcopy(res)
                    else:
                        return res
                else:
                    self._misses += 1
                    if(len(self._cache) >= self._maxSize):
                        del self._cache[next(iter(self._cache))]

                    res = self._func(*args , **kwargs)

                    if(AccumulatorDecorator.VERIFY_HASH == self._verify):
                        if(len(self._resultCache) >= self._maxSize):
                            del self._resultCache[next(iter(self._resultCache))]
                        self._resultCache[hsh] = None
                        if(hasattr(res , "athHash")):
                            self._resultCache[hsh] = res.athHash()
                        self._cache[hsh] = res
                    elif(AccumulatorDecorator.VERIFY_NOTHING == self._verify):
                        self._cache[hsh] = res
                    else:
                        _msg.debug("Incorrect value of verify in AccumulatorDecorator, assuming AccumulatorDecorator.VERIFY_NOTHING.")
                        self._cache[hsh] = res

                    if(self._deepcopy):
                        return deepcopy(res)
                    else:
                        return res
            else:
                self._misses += 1 
                return self._func(*args , **kwargs)
        else:
            return self._func(*args , **kwargs)

def AccumulatorCache(func = None , maxSize = 128 , verifyResult = AccumulatorDecorator.VERIFY_NOTHING , deepCopy = True): 
    """Function decorator, implements memoization.

    Keyword arguments:
        maxSize: maximum size for the cache associated with the function (default 128)
        verifyResult:   takes two possible values
                        
                        AccumulatorDecorator.VERIFY_NOTHING -   default, the cached function result is returned with no verification
                        AccumulatorDecorator.VERIFY_HASH -      before returning the cached function value, the hash of the
                                                                result is checked to verify if this object was not modified
                                                                between function calls
        deepCopy:   if True (default) a deep copy of the function result will be stored in the cache.
    
    Returns:
        An instance of AccumulatorDecorator.
    """

    def newWrapper(funct):
        return AccumulatorDecorator(funct , maxSize , verifyResult , deepCopy)

    if(func):
        return newWrapper(func)
    else:
        return newWrapper
