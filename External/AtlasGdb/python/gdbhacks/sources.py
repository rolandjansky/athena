# Try to set source file paths automatically.
import os
import gdb

# Set source path for ROOT.
rootsys = os.environ.get ('ROOTSYS')
if rootsys:
    rootsys = os.path.dirname (rootsys)
    rootsys = os.path.dirname (rootsys)
    rootsys = os.path.join (rootsys, 'src', 'root')
    if os.path.exists (rootsys):
        gdb.execute ("dir %s" % rootsys)


# Hacks to set other source paths properly.

def _handle_atlas_buildarea (fname):
    # Work around realpath bug in py 2.3.
    if os.path.islink(fname):
        fname = os.path.join (os.path.dirname (fname),
                              os.readlink (fname))
    fname = os.path.realpath (fname)
    if os.path.exists (fname):
        fname = os.path.dirname (fname)
        pkgdir = os.path.dirname (fname)
        pkg = os.path.basename (pkgdir)
        gdb.execute ("dir %s" % os.path.join (pkgdir, 'src'))
        gdb.execute ("dir %s" % os.path.join (pkgdir, pkg))
    return

_lcg_done = ['ROOT'] # root's handled separately.
def _handle_lcgarea (fname):
    fname = os.path.realpath (fname)
    if os.path.exists (fname):
        fname = os.path.dirname (fname)  # .../lib
        fname = os.path.dirname (fname)  # .../arch
        fname = os.path.dirname (fname)  # .../PRODUCT_VERSION

        product = os.path.dirname (fname) # .../PRODUCT
        product = os.path.basename (fname) # PRODUCT
        if product in _lcg_done: return
        srcdir = os.path.join (fname, 'src')
        if not os.path.exists (srcdir): return
        paths = os.listdir (srcdir)
        for p in paths:
            dir = os.path.join (srcdir, p, 'src')
            if os.path.exists (dir):
                gdb.execute ("dir %s" % dir)
            dir = os.path.join (srcdir, p, p)
            if os.path.exists (dir):
                gdb.execute ("dir %s" % dir)
    return

def _objfile_hook (o):
    if os.path.splitext (o.filename)[1] == '.so':
        if o.filename.startswith ('/afs/cern.ch/atlas/software/builds/'):
            _handle_atlas_buildarea (o.filename)
        if o.filename.startswith ('/afs/cern.ch/atlas/offline/external/LCGCMT/'):
            _handle_lcgarea (o.filename)
        
gdb.objfile_hook = _objfile_hook
