# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

def _local_apply_core(func, args, q):
    import os
    try:
        q.put(func(*args))
    except BaseException as e:
        q.put(e)
        os._exit(1)

def apply(func, args):
    from six.moves.queue import Empty
    from multiprocessing import Process
    from multiprocessing.managers import SyncManager

    with SyncManager() as m:
        q = m.Queue()
        p = Process(target=_local_apply_core, args=(func, args, q))
        p.start()
        p.join()
        print('Manager socket is', m.address)
        try:
            rv = q.get(False)
        except Empty:
            raise RuntimeError('daughter died while trying to execute %s%s' % (func.__name__, args))
        if isinstance(rv, BaseException):
            if isinstance(rv, SystemExit):
                print('SystemExit raised by daughter; ignoring')
                return None
            else:
                raise rv
    return rv
