# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils/python/AthFile/__init__.py
# @purpose a simple abstraction of a file to retrieve informations out of it
# @author Sebastien Binet <binet@cern.ch>
# @date October 2008
from __future__ import with_statement, print_function

__doc__ = "a simple abstraction of a file to retrieve informations out of it"
__author__  = "Sebastien Binet <binet@cern.ch>"

__all__        = []
__pseudo_all__ = [
    'AthFile',
    'ftype',
    'fopen',
    'exists',
    'server',
    ]

import PyUtils.Decorators as _decos
from . import impl as _impl
from . import tests as _tests
AthFile = _impl.AthFile

from decorator import decorator as _dec
@_dec
def _update_cache(fct, *args):
    res = fct(*args)
    import PyUtils.AthFile as af
    if af.server._do_pers_cache:
        try:
            af.server.load_cache()
        except Exception:
            pass
    return res

### classes -------------------------------------------------------------------
import types
class ModuleFacade(types.ModuleType):
    """a helper class to manage the instantiation of the ``AthFileMgr`` and
    ``AthFileServer`` objects and allow attribute-like access to methods
    (stolen from PyRoot)
    """
    def __init__( self, module ):
        types.ModuleType.__init__(self, module.__name__)
        self.__dict__['module'] = module
        self.__dict__[ '__doc__'  ] = module.__doc__
        self.__dict__[ '__name__' ] = module.__name__
        self.__dict__[ '__file__' ] = module.__file__

        self.__dict__['_tests'] = _tests
        self.__dict__['_impl']  = _impl
        self.__dict__['_guess_file_type'] = _guess_file_type

        self.__dict__['server'] = _impl.g_server
        
        import atexit
        atexit.register(self.shutdown)
        del atexit
        
    def __getattr__(self, k):
        if k in self.__dict__:
            return self.__dict__.get(k)
        if k.startswith('__'):
            return types.ModuleType.__getattribute__(self, k)
        return object.__getattribute__(self, k)
    
    def restart_server(self):
        return
    
    def shutdown(self):
        #self.server._cleanup_pyroot()
        return
    
    @property
    def msg(self):
        return self.server.msg()
    
    @property
    def cache(self):
        return self.server.cache()

    @property
    def save_cache(self):
        return self.server.save_cache

    @property
    def load_cache(self):
        return self.server.load_cache

    @property
    def flush_cache(self):
        return self.server.flush_cache
    
    @_decos.forking
    def ftype(self, fname):
        return self.server.ftype(fname)

    @_decos.forking
    def fname(self, fname):
        return self.server.fname(fname)

    @_decos.forking
    def exists(self, fname):
        return self.server.exists(fname)

    @property
    def tests(self):
        return self._tests

    @_update_cache  # also decorate with _update_cache to pick-up the changes 
    @_decos.forking # from the forked athfile server...
    def fopen(self, fnames, evtmax=1):
        """
        helper function to create @c AthFile instances
        @param `fnames` name of the file (or a list of names of files) to inspect
        @param `nentries` number of entries to process (for each file)
        
        Note that if `fnames` is a list of filenames, then `fopen` returns a list
        of @c AthFile instances.
        """
        if isinstance(fnames, (list, tuple)):
            infos = []
            for fname in fnames:
                info = self.server.fopen(fname, evtmax)
                infos.append(info)
                pass
            return infos
        return self.server.fopen(fnames, evtmax)

    @_update_cache  # also decorate with _update_cache to pick-up the changes 
    @_decos.forking # from the forked athfile server...
    def pfopen(self, fnames, evtmax=1):
        """
        helper function to create @c AthFile instances
        @param `fnames` name of the file (or a list of names of files) to inspect
        @param `nentries` number of entries to process (for each file)
        
        Note that if `fnames` is a list of filenames, then `fopen` returns a list
        of @c AthFile instances.

        This is a parallel (multi-threaded) version of ``fopen``.
        """
        return self.server.pfopen(fnames, evtmax)

    ## def __del__(self):
    ##     self._mgr.shutdown()
    ##     return super(ModuleFacade, self).__del__()
    
    pass # class ModuleFacade

###

def _guess_file_type(fname, msg):
    """guess the type of an input file (bs,rdo,esd,aod,...)
    """
    input_type = None
    import PyUtils.AthFile as af
    try:
        file_type,file_name = af.ftype(fname)
    except Exception:
        raise # for now
    if file_type == 'bs':
        input_type = 'bs'
    elif file_type == 'pool':
        import PyUtils.PoolFile as pf
        stream_names = pf.extract_stream_names(fname)
        stream_names = [s.lower() for s in stream_names]
        if len(stream_names) > 1:
            msg.warning('got many stream names: %r', stream_names)
            msg.warning('only considering the 1st one...')
        elif len(stream_names) <= 0:
            msg.warning('got an empty list of stream names')
            raise SystemExit(1)
        stream_name = stream_names[0]
        input_type = {
            'stream1':    'rdo',
            'streamesd' : 'esd',
            'streamaod' : 'aod',
            # FIXME: TODO: TAG, DPD
            }.get(stream_name, 'aod')

    else:
        msg.error('unknown file type (%s) for file [%s]',
                  file_type, file_name)
    return input_type


### exec at import ------------------------------------------------------------
import sys
sys.modules[ __name__ ] = ModuleFacade( sys.modules[ __name__ ] )
del ModuleFacade
