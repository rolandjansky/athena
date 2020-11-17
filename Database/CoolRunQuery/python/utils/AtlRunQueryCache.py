#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import pickle
import os
import sys

DEBUG=False

class Cache(object):
    def __init__(self,cachedir,form):
        self._cache = {}
        self.cachedir = cachedir.rstrip('/') if os.path.exists(cachedir) else None
        self.form = form
        if self.cachedir:
            print ("Initializing the cache at",cachedir.rstrip('/'))
            

    def __call__(self, f, *args, **kwds):
        if not self.cachedir:
            return f  # caching is automatically disabled if no cache directory exists

        print ("Providing cache functionality for function '%s()'" % f.func_name)
        def newf(*args, **kwds):
            key = kwds['cachekey']
            if not self.__is_cached(key):
                self.__write_to_cache(key,f(*args, **kwds))
            else:
                if DEBUG:
                    print ("DEBUG: returning cached value for '%s'" % (self.form % key))
                pass
            return self._cache[key]
        return newf

    def __name_pickle_cache(self,key):
        filename = self.form % key
        return '%s/%s.pickle' % (self.cachedir, filename)


    def __write_to_cache(self,key, value):
        # put into transient cache
        self._cache[key] = value

        # store pickled version
        pfname = self.__name_pickle_cache(key)
        pf = open( pfname, 'w' )
        try:
            pickle.dump(value, pf)
        except pickle.PicklingError as err:
            print ('ERROR: could not write to cache: %s (%s)' % (pfname, err))
            sys.exit(1)
        pf.close()
        
    def __is_cached(self,key):
        # check transient cache
        if key in self._cache:
            return True

        # check for pickled version
        pfname = self.__name_pickle_cache(key)
        try:
            pf = open( pfname, 'r' )
        except OSError:
            if DEBUG:
                print ('DEBUG: could not read from cache: ' + pfname)
            return False
        try:
            self._cache[key] = pickle.load(pf)
        except pickle.UnpicklingError as err:
            print ("ERROR: could not unpickle %s (%s)" % (pfname, err))
            sys.exit(1)
        pf.close()
        return True





if __name__ == "__main__":
    @Cache("/afs/cern.ch/user/s/stelzer/runsummary/cache/","simple_%i_second%i")
    def aFunction(x,y,cachekey):
        print ("expensive")
        return (3*x,y*y,x+y)

    x=13
    print (aFunction(x,y=2,cachekey=(x,2)))
    print (aFunction(x,1,cachekey=(x,1)))

