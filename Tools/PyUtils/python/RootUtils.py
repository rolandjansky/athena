# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils.RootUtils
# @author Sebastien Binet
# @purpose a few utils to ease the day-to-day work with ROOT
# @date November 2009

from __future__ import with_statement, print_function

__doc__ = "a few utils to ease the day-to-day work with ROOT"
__author__ = "Sebastien Binet"

__all__ = [
    'import_root',
    'root_compile',
    ]

### imports -------------------------------------------------------------------
import os
import re
import six

from .Decorators import memoize

### functions -----------------------------------------------------------------
def import_root(batch=True):
    """a helper method to wrap the 'import ROOT' statement to prevent ROOT
    from screwing up the display or loading graphics libraries when in batch
    mode (which is the default.)

    e.g.
    >>> ROOT = import_root(batch=True)
    >>> f = ROOT.TFile.Open(...)
    """
    import ROOT
    ROOT.gROOT.SetBatch(batch)
    if batch:
        ROOT.PyConfig.IgnoreCommandLineOptions = True
    import cppyy  # noqa: F401
    if os.environ.get('GLIBCXX_USE_CXX11_ABI') == '0':
        cmd = ROOT.gSystem.GetMakeSharedLib()
        if cmd.find('GLIBCXX_USE_CXX11_ABI') < 0:
            cmd = cmd.replace ('$SourceFiles', '$SourceFiles -D_GLIBCXX_USE_CXX11_ABI=0 ')
            ROOT.gSystem.SetMakeSharedLib(cmd)
    return ROOT

_tempfiles = []
_first_compile = True
def root_compile(src=None, fname=None, batch=True):
    """a helper method to compile a set of C++ statements (via ``src``) or
    a C++ file (via ``fname``) via ACLiC
    """
    if src is not None and fname is not None:
        raise ValueError("'src' xor 'fname' should be not None, *not* both")

    if src is None and fname is None:
        raise ValueError("'src' xor 'fname' should be None, *not* both")

    # Cling bug workaround: Cling will try to find a definition for the
    # hidden __gmon_start__ by opening all libraries on LD_LIBRARY_PATH.
    # But it will crash if it encounters a separate-debug library.
    # Work around by adding a dummy definition of __gmon_start__.
    # See !31633.
    global _first_compile
    if _first_compile:
        _first_compile = False
        root_compile ('extern "C" { void __gmon_start__(){}; }', None, True)
    return _root_compile (src, fname, batch)

def _root_compile (src, fname, batch):
    import os
    from .Helpers import ShutUp as root_shutup
    
    ROOT = import_root(batch=batch)
    compile_options = "f"
    if 'dbg' in os.environ.get('CMTCONFIG', 'opt'):
        compile_options += 'g'
    else:
        compile_options += 'O'

    src_file = None
    if src:
        import textwrap
        import tempfile
        src_file = tempfile.NamedTemporaryFile(prefix='root_aclic_',
                                               suffix='.cxx')
        src_file.write(textwrap.dedent(src).encode())
        src_file.flush()
        src_file.seek(0)
        fname = src_file.name

        # Apparently, cling caches files by inode.
        # If you ask it to read a file that has the same inode as one
        # that it has already read, then it will just use the cached
        # contents rather than rereading.  This, however, doesn't play
        # very well if we're reading temp files, where inodes may be reused,
        # giving rise to hard-to-reproduce failures.
        #
        # Try to avoid this by keeping the temp files open until the
        # the program exits.
        _tempfiles.append (src_file)
        pass

    elif fname:
        import os.path as osp
        fname = osp.expanduser(osp.expandvars(fname))
        pass
        
    assert os.access(fname, os.R_OK), "could not read [%s]"%(fname,)
    orig_root_lvl = ROOT.gErrorIgnoreLevel
    ROOT.gErrorIgnoreLevel = ROOT.kWarning
    try:
        with root_shutup():
            sc = ROOT.gSystem.CompileMacro(fname, compile_options)
        if sc == ROOT.kFALSE:
            raise RuntimeError(
                'problem compiling ROOT macro (rc=%s)'%(sc,)
                )
    finally:
        ROOT.gErrorIgnoreLevel = orig_root_lvl
    return
        
@memoize
def _pythonize_tfile():
    import cppyy
    root = import_root()
    import PyUtils.Helpers as H
    with H.ShutUp(filters=[
        re.compile(
            'TClass::TClass:0: RuntimeWarning: no dictionary for.*'),
        re.compile(
            'Warning in <TEnvRec::ChangeValue>: duplicate entry.*'
            ),
        ]):
        cppyy.loadDict("RootUtilsPyROOTDict")
        rootutils = getattr(root, "RootUtils")
        pybytes        = getattr(rootutils, "PyBytes")  # noqa: F841
        #MN: lines below fail in ROOT6 if PCM from RootUtils is not found
        read_root_file = getattr(rootutils, "_pythonize_read_root_file")
        tell_root_file = getattr(rootutils, "_pythonize_tell_root_file")
        pass
    def read(self, size=-1):
        """read([size]) -> read at most size bytes, returned as a string.

        If the size argument is negative or omitted, read until EOF is reached.
        Notice that when in non-blocking mode, less data than what was requested
        may be returned, even if no size parameter was given.

        FIXME: probably doesn't follow python file-like conventions...
        """
        SZ = 4096

        # FIXME: Once we drop py2, we can simplify this by using a bytes
        # object directly instead of PyBytes.
        if size>=0:
            #size = _adjust_sz(size)
            #print ("-->0",self.tell(),size)
            c_buf = read_root_file(self, size)
            if c_buf and c_buf.sz:
                v = c_buf.buf
                if six.PY3:
                    return bytes([ord(v[i]) for i in range(v.size())])
                return ''.join([v[i] for i in range(v.size())])
            return ''
        else:
            size = SZ
            out = []
            while True:
                #size = _adjust_sz(size)
                c_buf = read_root_file(self, size)
                if c_buf and c_buf.sz:
                    v = c_buf.buf
                    if six.PY3:
                        chunk = bytes([ord(v[i]) for i in range(v.size())])
                    else:
                        chunk = ''.join([v[i] for i in range(v.size())])
                    out.append(chunk)
                else:
                    break
            if six.PY3:
                return b''.join(out)
            return ''.join(out)
            
    root.TFile.read = read
    del read
    
    root.TFile.seek = root.TFile.Seek
    root.TFile.tell = lambda self: tell_root_file(self)
    ## import os
    ## def tell(self):
    ##     fd = os.dup(self.GetFd())
    ##     return os.fdopen(fd).tell()
    ## root.TFile.tell = tell
    ## del tell
    return 


def _getLeaf (l):
    tname = l.GetTypeName()
    ndat = l.GetNdata()
    if tname in ['UInt_t', 'Int_t', 'ULong64_t', 'Long64_t']:
        return [l.GetValueLong64(i) for i in range(ndat)]
    if tname in ['Float_t', 'Double_t']:
        return [l.GetValue(i) for i in range(ndat)]
    if tname in ['Char_t']:
        try:
            return l.GetValueString() # TLeafC for variable size string
        except Exception:
            return [l.GetValue(i) for i in range(ndat)] # TLeafB for 8-bit integers
    return None

class RootFileDumper(object):
    """
    A helper class to dump in more or less human readable form the content of
    any TTree.
    """
    
    def __init__(self, fname, tree_name="CollectionTree"):
        object.__init__(self)

        ROOT = import_root()

        # remember if an error or problem occurred during the dump
        self.allgood = True
        
        self.root_file = ROOT.TFile.Open(fname)
        if (self.root_file is None or
            not isinstance(self.root_file, ROOT.TFile) or
            not self.root_file.IsOpen()):
            raise IOError('could not open [%s]'% fname)

        self.tree = self.root_file.Get(tree_name)
        if self.tree is None or not isinstance(self.tree, ROOT.TTree):
            raise AttributeError('no tree [%s] in file [%s]', tree_name, fname)

        if 0:
            self._trees = []
            keys = [k.GetName() for k in self.root_file.GetListOfKeys()]
            for k in keys:
                o = self.root_file.Get(k)
                if isinstance(o, ROOT.TTree):
                    self._trees.append(k)
                    pass

        return

    def dump(self, tree_name, itr_entries, leaves=None):

        ROOT = import_root()
        import AthenaPython.PyAthena as PyAthena
        _pythonize = PyAthena.RootUtils.PyROOTInspector.pyroot_inspect2

        self.tree = self.root_file.Get(tree_name)
        if self.tree is None or not isinstance(self.tree, ROOT.TTree):
            raise AttributeError('no tree [%s] in file [%s]', tree_name, self.root_file.GetName())

        tree = self.tree
        nentries = tree.GetEntries()
        branches = sorted([b.GetName().rstrip('\0') for b in tree.GetListOfBranches()])
        if leaves is None: leaves = branches
        else:              leaves = [str(b).rstrip('\0') for b in leaves]
        
        # handle itr_entries
        if isinstance(itr_entries, str):
            if ':' in itr_entries:
                def toint(s):
                    if s == '':
                        return None
                    try:
                        return int(s)
                    except ValueError:
                        return s
                from itertools import islice
                itr_entries = islice(range(nentries),
                                     *map(toint, itr_entries.split(':')))
            elif ('range' in itr_entries or
                  ',' in itr_entries):
                itr_entries = eval(itr_entries)
            else:
                try:
                    _n = int(itr_entries)
                    itr_entries = range(_n)
                except ValueError:
                    print ("** err ** invalid 'itr_entries' argument. will iterate over all entries.")
                    itr_entries = range(nentries)
        elif isinstance(itr_entries, list):
            itr_entries = itr_entries
        else:
            itr_entries = range(itr_entries)
                
        for ientry in itr_entries:
            hdr = ":: entry [%05i]..." % (ientry,)
            #print (hdr)
            #print (hdr, file=self.fout)
            err = tree.LoadTree(ientry)
            if err < 0:
                print ("**err** loading tree for entry",ientry)
                self.allgood = False
                break

            nbytes = tree.GetEntry(ientry)
            if nbytes <= 0:
                print ("**err** reading entry [%s] of tree [%s]" % (ientry, tree_name))
                hdr = ":: entry [%05i]... [ERR]" % (ientry,)
                print (hdr)
                self.allgood = False
                continue

            for br_name in leaves:
                hdr = "::  branch [%s]..." % (br_name,)
                #print (hdr)
                #tree.GetBranch(br_name).GetEntry(ientry)
                py_name = [br_name]

                br = tree.GetBranch (br_name)
                if br.GetClassName() != '':
                    val = getattr(tree, br_name)
                else:
                    vals = [_getLeaf (l) for l in br.GetListOfLeaves()]
                    if len(vals) == 0:
                        val = None
                    elif len(vals) == 1:
                        val = vals
                    else:
                        val = tuple(vals)
                if not (val is None):
                    #print ("-->",val,br_name)
                    try:
                        vals = _pythonize(val, py_name, True)
                    except Exception as err:
                        print ("**err** for branch [%s] val=%s (type=%s)" % (
                            br_name, val, type(val),
                            ))
                        self.allgood = False
                        print (err)
                    for o in vals:
                        n = list(map(str, o[0]))
                        v = o[1]
                        yield tree_name, ientry, n, v

                pass # loop over branch names
            pass # loop over entries
    pass # class RootFileDumper

### test support --------------------------------------------------------------
def _test_main():
    root = import_root()  # noqa: F841
    def no_raise(msg, fct, *args, **kwds):
        caught = False
        err = None
        try:
            fct(*args, **kwds)
        except Exception as xerr:
            err = xerr
            caught = True
        assert not caught, "%s:\n%s\nERROR" % (msg, err,)

    no_raise("problem pythonizing TFile", fct=_pythonize_tfile)
    no_raise("problem compiling dummy one-liner",
             root_compile, "void foo1() { return ; }")
    no_raise("problem compiling dummy one-liner w/ kwds",
             fct=root_compile, src="void foo1a() { return ; }")
    import tempfile
    # PvG workaround for ROOT-7059
    dummy = tempfile.NamedTemporaryFile(prefix="foo_",suffix=".cxx")  # noqa: F841
    with tempfile.NamedTemporaryFile(prefix="foo_",suffix=".cxx") as tmp:
        tmp.write (b"void foo2() { return ; }\n")
        tmp.flush()
        no_raise("problem compiling a file",
                 fct=root_compile, fname=tmp.name)

    print ("OK")
    return True

if __name__ == "__main__":
    _test_main()
    
