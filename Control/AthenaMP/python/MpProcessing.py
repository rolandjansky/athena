# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import multiprocessing, os, sys, types
dlflags = sys.getdlopenflags()
sys.setdlopenflags( 0x100 | 0x2 )    # RTLD_GLOBAL | RTLD_NOW
import _athenamp as amp
sys.setdlopenflags( dlflags )

__all__ = [ 'cpu_count' ]


# cpu_count is pure python (but does call sysconf for Linux)
cpu_count = multiprocessing.cpu_count

# the following sets are replacements, which are accessed on C++ through the
# _athenamp extesion module for now

# current_process is used for identification purposes in multiprocessing; it
# serves no real purpose in AthenaMP (it's only used for printing a message)
# since there is a single relation of a mother process with multiple children

def current_process():
    '''
    Return process object representing the current process
    '''
    return amp.Process( os.getpid() )

# active_children does not match exactly, but since AthenaMP only starts
# readers/workers/writers from the mother, an aggregate of all groups will
# do; note also that b/c of the AthenaMP model, no cleanup is needed (as is
# in multiprocessing: Pools could close and re-open there)

_process_groups = []
def active_children():
    """
    Return list of process objects corresponding to live child processes
    """
    ac = list()

    global _process_groups
    for g in _process_groups:
        ac += g._children()
    return ac


##### class Queue
Queue = amp.SharedQueue


##### class Pool
class MapResults( object ):
    def __init__( self, group ):
        self._group = group

    def get( self, *args, **kw ):
        status = self._group.wait()

     # there are two parts to the exit code: the reported result from the worker
     # function and the process' exit code

     # TODO: handle process' exit code in a cleaner way (fix in PyComps?), as this
     # obviously does not work in general ...

        result = []
        for proc_result in status:
        # The result from run_worker_queue is extremely circumspect: it can either
        # contain a single tuple of 5 entries, 1 tuple of 4 entries, or 2 tuples of
        # 4 entries. It is checked on being 'OK' on the last entry of each tuple.
        # By sheer coincidence, that happens to work.

        # Now, the tuple can not be edited, so it's turned into a list, which takes
        # care of the spurious arrangement of lengths. Then for all possible returns,
        # the third entry is always theApp._exitstate and may need modification.

        # General exceptions are eaten, b/c this is so PyComps specific. :P
             r = proc_result[ 2 ]  # the python-posted result
             try:
                 import types
                 if type(r[0]) == types.TupleType:              # early return case
                     z = [ list(y) for y in r ]
                     for l2 in z:
                         if l2[2] == 0: l2[2] = proc_result[1]
                 else:
                     z = [ [ list(y) for y in x ] for x in r ]  # late return case
                     for l1 in z:
                         for l2 in l1:
                             if l2[2] == 0: l2[2] = proc_result[1]
             except Exception:
             # make-believe? can happen e.g. on early exit() or e.g. SIGQUIT
                 r = [[(proc_result[0], -1, proc_result[1], 'ERR')]]
             result.append( r )
        return result

class Pool( object ):
    packaged_count = 0

    def __init__( self, processes = None, initializer = None, initargs = (),
                  maxtasksperchild = None ):
 
        if not callable( initializer ):
            raise TypeError( 'initializer must be a callable' )

      # this workaround is needed b/c initargs can (and do) contain an amp.SharedQueue,
      # which can not be marshalled, but COW will deal with it properly by binding it
      # into this local 'packaged_initializer'
        def packaged_initializer( initializer = initializer, initargs = initargs ):
            return initializer( *initargs )

        self.packaged_count += 1
        self._initializer = '_amp_pool_%s_%d' % (initializer.__name__,self.packaged_count)

        import __main__
        setattr( __main__, self._initializer, packaged_initializer )

        self._group = amp.ProcessGroup( processes )
        global _process_groups
        _process_groups.append( self._group )

    def map_async( self, func, iterable, chunksize=1 ):
     # NOTE: initializer results are ignored (same as in multiprocessing)
        self._group.map_async( self._initializer )

     # TODO: hand over iterable properly (it just so happens that in AthenaMP, it is
     # a repeated list of MaxEvent, for use of reading from the queue)
        self._group.map_async( '%s.%s' % (func.__module__,func.__name__), iterable[0] )
        return MapResults( self._group )

    def close( self ):
        self._group.map_async( 'exit' )

        global _process_groups
        _process_groups.remove( self._group )

    def join( self ):
        pass  # alternative functionality for now

# other features of multiprocessing are not used by AthenaMP (but might be
# used by others; the following facade forwards for now

class ModuleFacade( types.ModuleType ):
    def __init__( self, module, name ):
        types.ModuleType.__init__( self, name )
        self.__dict__[ 'module' ] = module

        import multiprocessing
        self.__dict__[ 'pmp' ] = multiprocessing

    def __getattr__( self, attr ):
        try:
            return getattr( self.module, attr )
        except AttributeError:
            pass

        import AthenaCommon.Logging, logging
        log = logging.getLogger( 'AthenaMP.MpProcessing' )
        log.error( 'missing attribute %s (falling back on multiprocessing)', attr )
        return getattr( self.pmp, attr )
   

sys.modules[ __name__ ] = ModuleFacade( sys.modules[ __name__ ], __name__ )
del ModuleFacade
