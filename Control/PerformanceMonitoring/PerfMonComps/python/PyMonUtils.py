# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: PyMonUtils.py
# @author: Sebastien Binet <binet@cern.ch>

__author__  = "Sebastien Binet <binet@cern.ch>"
__version__ = "$Revision: 1.3 $"
__doc__     = """python module holding some general and/or miscellaneous tools, functions and classes to ease gathering monitoring data
"""

class Units:
    kB = 1024.
    MB = 1024. * 1024.
    pass

def mon_push_back (sgname='StoreGateSvc'):
    """
    Helper method to crawl through collections in a StoreGateSvc instance and
    assess how much memory they are wasting b/c they 'forgot' to reserve
    enough space before doing 'push_back'.

    This task is performed by just comparing what Klass::size() and
    Klass::capacity() are returning as a value.

    @param `sgname` the fully qualified Gaudi name of the StoreGateSvc instance
    @return a dictionary of { <sgkey> : (size, capacity, clid) }
    """
    wasted = {}
    try:
        from AthenaPython import PyAthena
    except ImportError:
        # arf... release 13...
        return wasted

    sg = PyAthena.py_svc (sgname)
    cl = PyAthena.py_svc ('ClassIDSvc')

    # retrieve all containers from StoreGate
    proxies = sg.proxies()

    for dp in proxies:
        k = dp.name()
        # no double counting from symlinks
        # FIXME: it is actually valid to have 2 different collections
        #        (=/= CLIDs) with the same key...
        if k in wasted:
            continue
        clid = dp.clID()
        klass = "%s" % cl.typename(clid)
        # 'handle' classes for which we were not able to fetch
        # a C++ typename from the SG::DataProxy classID (e.g. HistoryObjects)
        if klass == "None":
            continue

        try:
            data = sg.retrieve(klass, k)
        except Exception:
            # don't bother user: probably no Reflex dictionary...
            continue

        if not data:
            continue

        # reject objects we can't inspect or not interested in
        if not hasattr(data, 'size') or \
           not hasattr(data, 'capacity'):
            continue

        sz = data.size()
        cp = data.capacity()
        rt = 0.
        if cp != 0.:
            rt = sz/float(cp)*100.
        # simple minded filter
        if rt==0. or rt>=90.:
            continue
        wasted[k] = (sz, cp, clid)
        pass
    return wasted

def dump_smaps (fname=None):
    import os,sys
    if not (fname is None):
        o = open (fname, 'w')
    else:
        o = sys.stdout
    for z in open('/proc/%d/smaps'%os.getpid()):
        print(z, file=o)
    if not (fname is None):
        o.close()
    return

def loaded_libs (fname=None, pid=None, show=False):
    import os,sys,re
    if not (fname is None):
        o = open (fname, 'w')
    else:
        o = sys.stdout
    pat = re.compile(r'(?P<addr_beg>\w*?)\-(?P<addr_end>\w*?)\s'\
                     r'(?P<perm>.{4})\s(?P<offset>\w*?)\s'\
                     r'(?P<devmajor>\d{2}):(?P<devminor>\d{2})\s'\
                     r'(?P<inode>\d*?)\s(?P<libname>.*)')
    libs = set()
    _realpath = os.path.realpath
    if pid is None:
        pid = os.getpid()
    for line in open('/proc/%s/smaps'%pid):
        z = line.strip()
        res = re.match(pat,z)
        if res:
            g = res.group
            libname = g('libname').strip()
            libs.add(_realpath(libname))
    libs = sorted([z for z in libs], reverse=True)
    if show:
        for libname in libs:
            print(libname, file=o)
    return libs

import sys
if sys.platform == 'darwin':
    def pymon():
        from resource import getrusage, RUSAGE_SELF
        cpu = getrusage(RUSAGE_SELF)
        cpu = (cpu.ru_utime+cpu.ru_stime) * 1e3 # in milliseconds
        # The following is placeholder code for the Mac to get vmem and rss in bytes. This is available
        # via the C task_info() function, for which there isn't a Python wrapper yet. The following is 
        # a hack using the 'ps' command which is certainly not optimal! The vsz and rss are claimed to
        # be returned from the 'ps' command in units of Kb, but it looks like vsz is in pages rather than 
        # Kb, so I'll scale it by that, whereas rsz does appear to be in units of Kb.
        from resource import getpagesize
        from os import popen, getpid
        pageSize = getpagesize()
        vsz = int(popen('ps -p %d -o %s | tail -1' % (getpid(), "vsz")).read())
        rss = int(popen('ps -p %d -o %s | tail -1' % (getpid(), "rss")).read())
        vmem = vsz*pageSize / 1024.0 / 1024.0
        rss  = rss / 1024
        return cpu,vmem,rss
else:
    def pymon():
        from os import getpid,sysconf
        from resource import getrusage, RUSAGE_SELF
        cpu = getrusage(RUSAGE_SELF)
        cpu = (cpu.ru_utime+cpu.ru_stime) * 1e3 # in milliseconds
        pageSize = sysconf('SC_PAGE_SIZE')/Units.MB
        mem = open('/proc/%d/statm'%getpid(),'r')
        mem = mem.readlines()[0].split()
        vmem = int(mem[0])*pageSize
        rss  = int(mem[1])*pageSize
        return cpu,vmem,rss
    
def lshosts_infos():
    import socket,commands
    hostname = '<unknown>'
    try:
        hostname = socket.gethostname()
    except Exception:
        pass
    sc,out = commands.getstatusoutput('which lshosts')
    if sc != 0:
        return ('no lshosts command',0.) # no lshosts could be found
    cmd = out
    sc,out = commands.getstatusoutput("%s %s"%(cmd,hostname))
    if sc != 0:
        return ('host not in db', 0.)
    cpu_infos = {}
    try:
        title,data = out.splitlines()
        title = title.split()
        # title is of the form:
        # HOST_NAME  type    model  cpuf ncpus maxmem maxswp server RESOURCES
        data  = data.split()
        for i,k in enumerate(title[:-1]): # dropping the 'RESOURCES' field
            cpu_infos[k] = data[i]
    except Exception as err:
        return ('err', err)
    return ('ok', cpu_infos)


