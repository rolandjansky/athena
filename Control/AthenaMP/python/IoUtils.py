# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file AthenaMP.IoUtils
# @purpose I/O utils 
# @author Mous Tatarkhanov <tmmous@berkeley.edu>

__doc__     = "I/O related utils for AthenaMP"       
__version__ = "$Revision: 1.0$"
__author__  = "Mous Tatarkhanov <tmmous@berkeley.edu>"

from AthenaCommon.Logging import log as msg     #logging handle
from GaudiMP.IoRegistry import IoRegistry
_debug = msg.debug
_info = msg.info


def create_worker_dir(top_wkdir):
    """create tmp wkdir for forked worker under top_wkdir"""
    #changing the workdir to tmp location  
    import os
    from os.path import abspath as _abspath, join as _join
    ppid = os.getppid()
    pid  = os.getpid()
    curdir = _abspath (os.curdir)
    wkdir  = _abspath (_join (top_wkdir,str(pid)))
    _info("curdir=[%s]", curdir)
    _info("top_wkdir=[%s]", top_wkdir)
    _info("wkdir=[%s]", wkdir)
    if os.path.exists(wkdir):
        import shutil
        shutil.rmtree (wkdir)
    os.makedirs (wkdir)
        
    return wkdir

def update_io_registry(wkdir, mpid, iocomp_types=None):
    """helper method to correctly update the IoRegistry instances
    """
    import os
    from os.path import join as _join
    from os.path import basename as _basename
    from os.path import isabs as _isabs

    from PyUtils.PoolFile import PoolFileCatalog
    
    # ioreg is a dict:
    # {'iocomp-name' : { 'old-fname' : ['iomode', 'new-fname'] }, ... }
    ioreg = IoRegistry.instances
    msg.debug("ioreg::: %s" % ioreg)
    
    pfc = PoolFileCatalog()

    ioreg_items = IoRegistry.instances.iteritems()
    for iocomp,iodata in ioreg_items:
        #print "--iocomp,len(iodata)",iocomp, len(iodata)
        io_items = iodata.iteritems()
        for ioname,ioval in io_items:
            # handle logical filenames...
            #ioname=pfc(ioname)
            pfc_name = pfc(ioname)
            if (pfc_name != ioname):
                ioreg[iocomp][ioname][1]=pfc_name
        
            ##print " --iocomp,ioname,ioval",iocomp,ioname,ioval
            iomode,newname = ioval[0], ioval[1] or ioname
            if iomode == '<output>':
                newname = _join (wkdir,
                                 "mpid_%s__%s"%(str(mpid).zfill(3),
                                                _basename(ioname)))
                msg.debug ("update_io_registry:<output>: newname=%s" % newname)
            elif iomode == '<input>':
                if not _isabs(ioname) and not ioname.startswith("root:") and not ioname.startswith("rfio"):
                # FIXME: handle URLs/URIs...
                    src = os.path.abspath(_join(os.curdir, ioname))
                    dst = _join(wkdir, ioname)
                    os.symlink(src, dst)
                    msg.debug( "update_io_registry:<input> created symlink %s for" % dst)
            else:
                raise ValueError, "unexpected iomode value: %r"%iomode
            ioreg[iocomp][ioname][1] = newname
            pass
        pass
    msg.debug( "IoRegistry.instances=%s" % IoRegistry.instances )
    return # update_io_registry

def redirect_log(wkdir):
    """redirect stdout and stderr of forked worker to tmp wkdir"""
    import os, sys
    # define stdout and stderr names
    
    stdout = os.path.join(wkdir, 'stdout')
    stderr = os.path.join(wkdir, 'stderr')
    _info(" stdout->[%s]" % stdout)
    _info(" stderr->[%s]" % stderr)

    # synch-ing log
    map(lambda x: x.flush(), (sys.stdout, sys.stderr))

    flags = os.O_CREAT | os.O_WRONLY
    fdout = os.open (stdout, flags)
    assert fdout>=0, \
        "problem child [%r] opening stdout" % mp.current_process()
    fileno = sys.stdout.fileno()
    os.close (fileno)
    os.dup2  (fdout, fileno)

    fderr = os.open (stderr, flags)
    assert fderr>=0, \
        "problem child [%r] opening stderr" % mp.current_process()
    fileno = sys.stderr.fileno()
    os.close (fileno)
    os.dup2  (fderr, fileno)

def reopen_fds(wkdir=""):
    """redirect workers fds by setting proper flags
       <INPUT> ->  duplicate fd in RDONLY mode
       <OUTPUT> -> redirect fd to local output-copy w/ duplicate flags.
    """
    _info("reopen_fds: in dir [%s]" % wkdir)
    import os, fcntl
    #from IoRegistry import IoRegistry as ioreg
    _join = os.path.join
    
    _fds = IoRegistry.fds_dict
    _fds.create_symlinks(wkdir)

    for k, v in _fds.iteritems():
        fd = k; 
        (real_name, iomode, flags) = v
        if not os.path.isfile (real_name):
            _debug("reopen_fds: %s is not file" % real_name)
            continue
        
        if (iomode == '<OUTPUT>'):
            #expect the copy of <OUTPUT> to be in curdir
            _debug("reopen_fds: <OUTPUT> %s " % real_name)
            pos = os.lseek(fd, 0, os.SEEK_CUR)
            #PC HACK remove!
            try:
                new_fd = os.open (_join(wkdir, os.path.basename(real_name)), flags)
                os.lseek(new_fd, pos, os.SEEK_SET)
            except Exception, err:         
                msg.warning("Exception caught handling OUTPUT file %s: %s" %  (real_name, err) )
                msg.warning(" ...ignoring file FIXME!")
                continue
        else:
            _debug("reopen_fds:<INPUT> %s" % real_name)
            new_fd = os.open (real_name, os.O_RDONLY)
        
        os.close (fd)
        os.dup2  (new_fd, fd)
        fcntl.fcntl (fd, fcntl.F_SETFL, flags)
        os.close (new_fd)

def reopen_fds_old():
    import os, fcntl
    _realpath = os.path.realpath
    _join = os.path.join
    # reopen file descriptors
    procfd = '/proc/self/fd'
    fds = os.listdir(procfd)
    for i in fds:
        fd = int(i)
        real_name = _realpath(_join(procfd,i))
        if not os.path.exists (real_name):
            continue
        flags = fcntl.fcntl (fd, fcntl.F_GETFL)
        new_fd = os.open (real_name, os.O_RDONLY)
        os.close (fd)
        os.dup2  (new_fd, fd)
        fcntl.fcntl (fd, fcntl.F_SETFL, flags)
        os.close (new_fd)


def print_fds(msg):
    """print all file descriptors of current process"""
    import os, fcntl
    _realpath = os.path.realpath
    _join = os.path.join
    # print out file descriptors
    procfd = '/proc/self/fd'
    fds = os.listdir(procfd)
    for fd in fds:
        i = int(fd)
        realname = _realpath(_join(procfd,fd))
        msg.info("fd=[%i], realname=[%s] exists=[%s]",
                      i, realname, os.path.exists(realname))
    return

