# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: AthenaServices/python/AthDsoLogger.py
# @purpose: log the vmem usage at each dso being dlopen'd
# @author: Sebastien Binet <binet@cern.ch>

from __future__ import with_statement

__doc__ = "log the vmem usage at each dso being dlopen'd"
__version__ = "$Revision: 1.3 $"
__author__  = "Sebastien Binet <binet@cern.ch>"

import os
import ctypes
import sys

from os.path import realpath as _realpath
from os.path import basename as _basename

class DsoEvent(ctypes.Structure):
    _fields_ = [
        ('fname', ctypes.c_char_p),
        ('step',  ctypes.c_int),
        ]
    pass

class DsoLogger (object):
    def __init__ (self, fname=None, cb_fct=None):
        """log Dso loading by calling-back into `cb_fct`

        if `cb_fct` is None we use a default callback function which logs
        the dVmem on stdout.
        """
        import ctypes
        self.lib = ctypes.cdll.LoadLibrary ("libAthDSoCallBacks.so")
##         self.lib.fetch_vmem.argtypes = None
##         self.lib.fetch_vmem.restype  = ctypes.c_float

        
        self._cb_fct_type = ctypes.CFUNCTYPE(ctypes.c_int,
                                             ctypes.POINTER(DsoEvent),
                                             ctypes.c_void_p)
        fct = self.lib.ath_dso_cbk_register
        fct.argtypes = [self._cb_fct_type, ctypes.c_void_p]
        fct.restype  = ctypes.c_int

        fct = self.lib.ath_dso_cbk_unregister
        fct.argtypes = [self._cb_fct_type]
        fct.restype  = ctypes.c_int

        from collections import defaultdict
        self._data = defaultdict(dict)

        from AthenaCommon.Logging import logging
        self.msg = logging.getLogger ("AthDsoLogger")
        self.msg.setLevel (logging.INFO)
        
        if cb_fct is None:
            cb_fct = self.default_cb_fct
        # create the C-callback function wrapper
        self.msg.info ("creating the C-callback function")
        self.cb_fct = self._cb_fct_type(cb_fct)

        # create a CSV file holding the monitoring data
        import csv
        if fname is None:
            fname = 'vmem-dso.csv'
        if os.path.exists (fname):
            os.remove (fname)
        self._fd = open (fname, 'w')
        self.out = csv.writer (self._fd, delimiter=';')
        map (self.out.writerow,
             [ ['master-pid', os.getpid()],
               ['pid', 'libname',
                'vmem-start (kb)', 'vmem-stop (kb)', 'dvmem (kb)'],
               ])
        
        # start logging...
        self.msg.info ("initializing the C-dlopen-logger")
        self.lib.ath_dso_cbk_register(self.cb_fct, None)

    def __del__ (self):
        # release library
        self.lib.ath_dso_cbk_unregister(self.cb_fct)
        self._fd.close()

    if sys.platform != 'darwin':
        def default_cb_fct (self, evt, usrdata):
            import os
            from os.path import realpath as _realpath
            from os.path import basename as _basename
            libname = evt.contents.fname
            step = evt.contents.step

            if libname is None:
                return 0
            n = _basename (_realpath (libname))

            pid = os.getpid()
            def vmem():
                from os import sysconf
                PAGE_SIZE = sysconf ('SC_PAGE_SIZE') # in bytes

                from string import split as ssplit
                m = 0
                from sys import platform
                with open('/proc/self/statm') as f:
                    m = int(ssplit (f.readlines()[0])[0])
                return m * PAGE_SIZE # in bytes

            if step == 0:
                self._data[pid][n] = [vmem(), None]
            else:
                data = self._data[pid][n]
                data[1] = vmem()
                vmem_start = data[0]/1024.
                vmem_stop  = data[1]/1024.
                dvmem      = vmem_stop - vmem_start
                self.msg.info (
                    "[%d] loading lib: vmem=(%10.1f + %10.1f) kb [%s]",
                    pid, vmem_start, dvmem, n
                    )
                self.out.writerow ([pid, n, vmem_start, vmem_stop, dvmem])
                #del self._data[pid][n]
            return 0
    else:
        def default_cb_fct (self, evt, usrdata):
            import os
            from os.path import realpath as _realpath
            from os.path import basename as _basename
            libname = evt.fname
            step = evt.step

            if libname is None:
                return 0
            n = _basename (_realpath (libname))

            pid = os.getpid()
            def vmem():
                from os import sysconf
                PAGE_SIZE = sysconf ('SC_PAGE_SIZE') # in bytes
                return PAGE_SIZE

            if step == 0:
                self._data[pid][n] = [vmem(), None]
            else:
                data = self._data[pid][n]
                data[1] = vmem()
                vmem_start = data[0]/1024.
                vmem_stop  = data[1]/1024.
                dvmem      = vmem_stop - vmem_start
                self.msg.info (
                    "[%d] loading lib: vmem=(%10.1f + %10.1f) kb [%s]",
                    pid, vmem_start, dvmem, n
                    )
                self.out.writerow ([pid, n, vmem_start, vmem_stop, dvmem])
                #del self._data[pid][n]
            return 0
