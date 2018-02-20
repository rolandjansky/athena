# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import glob
import itertools
import os
import re
import subprocess

# DEPRECATED CODE #############################################################

import sys
from functools import wraps
def deprecated(message):
    def deco(fn):
        @wraps(fn)
        def wrapper(*args, **kwargs):
            print('WARNING: [InDetBeamSpotExample.DiskUtils]',
                    '{}() is deprecated and will be removed'.format( fn.__name__),
                    file=sys.stderr)
            print('WARNING:                                 ', message,
                    file=sys.stderr)
            return fn(*args, **kwargs)
        return wrapper
    return deco

import fnmatch
from collections import namedtuple
StorageManager = namedtuple('StorageManager', ['name', 'prefix', 'cp', 'ls', 'longls'])
CastorMgr = StorageManager(name='castor', prefix='root://castoratlas/', cp='xrdcp', ls='nsls %s', longls='nsls -l %s')
RFIOMgr = StorageManager(name='rfio', prefix='rfio:', cp='rfcp', ls='rfdir %s', longls='rfdir %s')
EOSMgr = StorageManager(name='eos', prefix='root://eosatlas.cern.ch/', cp='xrdcp', ls='/bin/sh -l -c "LD_LIBRARY_PATH=/usr/lib64/ eos ls %s"', longls='/bin/sh -l -c "LD_LIBRARY_PATH=/usr/lib64/ eos ls -l %s"')
UnixMgr = StorageManager(name='unix', prefix='', cp='cp', ls='ls %s', longls='ls -l %s')

def _rationalise(path):
    """
    Rationalise a path, removing prefix and esuring single leading slash
    """
    for p in ('root://castoratlas/', 'root://eosatlas.cern.ch/', 'rfio:', 'castor:'):
        if path.startswith(p):
            path = path[len(p):]
            if path.startswith('//'):
                path = path[1:]
            if not path.startswith('/'):
                path = '/'+path
            break

    return path

@deprecated("EOS is mounted on /eos with fuse, so you probably don't need this abstraction")
def storageManager(name):
    """
    Return SotrageManager to deal with listing, copying and reading files from various storage systems
    """
    name = _rationalise(name)
    if name.startswith('/castor/'):
        return CastorMgr
    elif name.startswith('/eos/'):
        return EOSMgr
    else:
        return UnixMgr

@deprecated("DiskUtils.FileSet replaces this functionality")
def filelist(files, prefix=None):
    """
    lists CASTOR/EOS name server directory/file entries.
    If path is a directory, filelist lists the entries in the directory;
    they are sorted alphabetically.

    `files` specifies the CASTOR/EOS pathname.
    `prefix` specifies the prefix one wants to prepend to the path found.
             (e.g. prefix='root://castoratlas/' or 'root://eosatlas.cern.ch//')
             if prefix=True it will determin the prefix based on the pathname

    ex:
    filelist('/castor/cern.ch/atlas/*')
    filelist('/castor/cern.ch/atl*/foo?[bar]/*.pool.root.?')
    filelist('/eos/atlas/*', prefix='root://eosatlas.cern.ch/')
    filelist('/castor/cern.ch/atlas/*', prefix=True)
    """

    path, fname = os.path.split(files)
    path = _rationalise(path)

    if ( path.count('*') > 0 or path.count('?') > 0 or
         path.count('[') > 0 or path.count(']') > 0 ) :
        paths = ls(path)
        return sum([ls(os.path.join(p,fname))
                    for p in paths], [])

    mgr = storageManager(path)

    try:
        flist = subprocess.check_output(mgr.ls % path, shell=True).split()
    except subprocess.CalledProcessError as err:
        print(err.output)
        return []

    if not (os.path.basename(files) in ['', '*']): # no need to filter
        pattern = fnmatch.translate(os.path.basename(files))
        flist = filter(lambda x: re.search(pattern, x), flist)

    if prefix:
        if isinstance(prefix, basestring):
            return [os.path.join(prefix+path, p) for p in flist]
        else:
            return [os.path.join(mgr.prefix+path, p) for p in flist]
    else:
        return [os.path.join(path, p) for p in flist]

@deprecated("EOS is mounted on /eos with fuse, so you probably don't need this abstraction")
def ls(path, longls=False):
    """
    Simple list of files

    `longls` specifies long listing format
    """

    path = _rationalise(path)
    mgr = storageManager(path)

    if longls:
        return subprocess.check_output(mgr.longls % path, shell=True)
    else:
        return subprocess.check_output(mgr.ls % path, shell=True)

@deprecated("EOS is mounted on /eos with fuse, so you probably don't need this abstraction")
def cp(src, dest='.'):
    src = _rationalise(src)
    dest = _rationalise(dest)
    srcmgr = storageManager(src)
    destmgr = storageManager(dest)

    cp = 'cp'
    if srcmgr.cp == 'xrdcp' or destmgr.cp == 'xrdcp': cp = 'xrdcp'

    return os.system('%s %s%s %s%s' %(cp, srcmgr.prefix, src, destmgr.prefix, dest))

###############################################################################

class AccessError(RuntimeError): pass

def get_lumi_blocks(root_file):
    import ROOT
    from AthenaROOTAccess.transientTree import makeTree
    try:
        f = ROOT.TFile(root_file)
        if not f.IsOpen(): raise AccessError('Unable to open file: ' + root_file)
        t = f.Get('MetaData')
        tt = makeTree(t, dhTreeName='MetaDataHdr')
        for branch in tt.GetListOfBranches():
            if branch.GetClassName() == 'EventStreamInfo':
                if branch.GetEntry(0) < 0:
                    raise AccessError('Unable to read entry from: ' + branch.GetClassName())
                esi = getattr(tt, branch.GetName())
                return esi.getLumiBlockNumbers()
        raise RuntimeError('No EventStreamInfo object found in file: ' + root_file)
    finally:
        f.Close()

def make_lumi_block_map_file(file_set, path):
    with open(path, 'w') as mapfile:
        for f, lbs in file_set.with_lumi_blocks():
            print('Reading:', f)
            mapfile.write('{} {}\n'.format(
                os.path.basename(f),
                ','.join(str(x) for x in lbs)))


class Backend:
    def exists(self, path): raise NotImplemented
    def is_file(self, path): raise NotImplemented
    def is_directory(self, path): raise NotImplemented
    def children(self, path): raise NotImplemented
    def glob(self, pattern): raise NotImplemented
    def wrap(self, path): return path

class Local(Backend):
    def exists(self, path): return os.path.exists(path)
    def is_directory(self, path): return os.path.isdir(path)
    def is_file(self, path): return os.path.isfile(path)

    def children(self, path):
        def generator(p):
            for dir_name, dirs, files in os.walk(p):
                for f in files:
                    yield os.path.join(dir_name, f)
        return generator(path)

    def glob(self, pattern):
        return glob.glob(pattern)

class EOS(Backend):
    """ Accesses EOS using the command line interface.
    NB: when EOS is fuse-mounted on /eos this class is not really necessary.
    """

    def __init__(self, prefix='root://eosatlas.cern.ch/'):
        self.prefix = prefix

    def wrap(self, path):
        if path.startswith('/'):
            path = self.prefix + path
        return path

    def unwrap(self, path):
        if path.startswith(self.prefix):
          path = path[len(self.prefix):]
        return path

    def exists(self, path):
        return self._call('eos', '-b', 'ls', '-s', self.unwrap(path)) == 0

    def is_file(self, path):
        return self._call('eos', '-b', 'sat', '-f', self.unwrap(path)) == 0

    def is_directory(self, path):
        return self._call('eos', '-b', 'sat', '-d', self.unwrap(path)) == 0

    def children(self, path):
        with open(os.devnull, 'w') as null:
            output = subprocess.check_output(['eos', '-b', 'find', '-f',
                self.unwrap(path)], stderr=null)
        return [l.strip() for l in output.split('\n')]

    def _call(self, *args):
        with open(os.devnull, 'w') as null:
          retcode = subprocess.call(args, stderr=null)
        return retcode

class FilterError(RuntimeError): pass

class FileSet:
    """ Represents a list of input files.
    This class abstracts over the different ways files can be specified, and
    the different storage backends/protocols on which they reside. It is an
    iterator, and provides some methods for filtering the file set. E.g.:

        fs = FileSet.from_input('/eos/atlas/path/to/dataset/')
        for f in fs.matching(r'.*AOD.*').only_existing():
            print(f)
    """

    def __init__(self, iterator, backend):
        self.backend = backend
        self._iter = iterator
        self._existing = False
        self._white_pattern = None
        self._black_pattern = None
        self._strict = True
        self._explicit = None
        self._dedup = False
        self._single_dataset = False
        self.broken = []
        self.lb_map = {}

    @classmethod
    def from_single_file(cls, path, backend=None):
        return cls(iter([path]), backend or Local())

    @classmethod
    def from_directory(cls, path, backend=None):
        be = backend or Local()
        return cls(be.children(path), be)

    @classmethod
    def from_file_containing_list(cls, path, backend=None):
        with open(path) as lf:
            iterator = iter([l.strip() for l in lf.readlines()])
        return cls(iterator, backend or Local())

    @classmethod
    def from_glob(cls, pattern, backend=None):
        be = backend or Local()
        return cls(be.glob(pattern), be)

    @classmethod
    def from_ds_info(cls, run, project, stream, base, backend=None):
        path = os.path.join(base, project, stream,
                '{:0{digits}d}'.format(int(run), digits=8))
        return cls.from_directory(path, backend=backend)

    @classmethod
    def from_input(cls, input_string, backend=None):
        ''' Guess what kind of input file specification was provided. '''
        be = backend or Local()
        if be.is_directory(input_string):
            return cls.from_directory(input_string, be)
        elif Local().is_file(input_string) and not (
                input_string.endswith('.root') or
                input_string[-7:-2] == '.root'):
            return cls.from_file_containing_list(input_string, be)
        elif be.is_file(input_string):
            return cls.from_single_file(input_string, be)
        elif '*' in input_string or '?' in input_string or '[' in input_string:
            return cls.from_glob(input_string, be)
        else:
            raise AccessError('Unable to resolve input: ' + repr(input_string))

    def __iter__(self):
        it = self._iter
        if self._white_pattern:
            it = itertools.ifilter(lambda x: self._white_pattern.search(x), it)
        if self._black_pattern:
            it = itertools.ifilter(lambda x: not self._black_pattern.search(x), it)
        if self._existing: # see: only_existing
            if self._strict:
                def generator(i, b):
                    for f in i:
                        if b.exists(f):
                            yield f
                        else:
                            raise AccessError('File not found: ' + f)
                it = generator(it, self.backend)
            else:
                it = itertools.ifilter(lambda x: self.backend.exists(x), it)
        if self._explicit is not None: # see: use_files_from
            def generator(i, strict):
                for f in i:
                    name = os.path.basename(f)
                    if self._explicit.pop(name, False):
                        yield f
                if strict and self._explicit:
                    for f in self._explicit: print('Missing:', f)
                    raise FilterError('Not all explicit files were found.')
            it = generator(it, self._strict)
        if self._dedup: # see: only_latest
            def fn(m, f):
                name, ext = os.path.splitext(f)
                if name in m:
                    m[name] = str(max(int(m[name]), int(ext[1:])))
                else:
                    m[name] = ext[1:]
                return m
            def generator(em):
                for name, ext in em.items():
                    yield '.'.join([name, ext])
            it = generator(reduce(fn, self, {}))
        if self._single_dataset: # see: only_single_dataset
            def generator(i):
                dataset = None
                for f in i:
                    ds = '.'.join(f.split('.')[0:3])
                    if dataset is None:
                        dataset = ds
                    if ds == dataset:
                        yield f
                    else:
                        raise FilterError(
                                "Files found from more than one dataset: '{}' != '{}'"
                                .format(ds, dataset))
            it = generator(it)
        it = itertools.imap(lambda x: self.backend.wrap(x), it)
        return it

    def strict_mode(self, setting=True):
        """ When strict, errors are raised in the following cases (which
        otherwise cause the corresponding files to be silently skipped):

          * When LB info is requested but cannot be found for a file (because
            it was not in the map file, or ARA couldn't open the ROOT file).
          * When `only_existing` is set and a file is missing.
          * When a file list is provided and not all of the files it mentions
            were encountered by the end of iteration.
        """
        self._strict = setting
        return self

    def matching(self, pattern):
        ''' Only accept filenames matching the provided regular expression. '''
        self._white_pattern = re.compile(pattern) if pattern else None
        return self

    def excluding(self, pattern):
        ''' Skip filenames matching the provided regular expression. '''
        self._black_pattern = re.compile(pattern) if pattern else None
        return self

    def use_files_from(self, path):
        ''' Use specific filenames from within the provided dataset. '''
        if path:
            with open(path) as lf:
                self._explicit = [l.strip() for l in lf.readlines()]
        else:
            self._explicit = None
        return self

    def only_existing(self, setting=True):
        ''' Only use existing files. '''
        self._existing = setting
        return self

    def only_latest(self, setting=True):
        ''' Keep only the latest retry from sets like `*.1`, `*.2`. '''
        self._dedup = setting
        return self

    def only_single_dataset(self, setting=True):
        ''' Require all files to be from the same dataset. '''
        self._single_dataset = setting
        return self

    def with_lumi_blocks(self, map_file=None):
        """ Lookup the luminosity blocks contained in each file.
        If a map file is provided it will be queried for the LB mapping,
        otherwise each file will be opened and accessed using AthenaROOTAccess
        which can be a little slow.
        """
        if map_file:
            return self._with_lumi_blocks_from_map(map_file)
        else:
            return self._with_lumi_blocks_from_ara()

    def _with_lumi_blocks_from_map(self, map_file):
        with open(map_file) as mf:
            for line in mf:
                fname = line.split(' ')[0]
                lbs = set(int(l) for l in line.split(' ')[1].split(','))
                self.lb_map[fname] = lbs
        def generator(s):
            for f in s:
                try:
                    yield f, s.lb_map[os.path.basename(f)]
                except KeyError:
                    if s._strict:
                        raise
                    else:
                        s.broken.append(f)
        return generator(self)

    def _with_lumi_blocks_from_ara(self):
        def generator(s):
            for f in s:
                try:
                    lbs = get_lumi_blocks(f)
                except AccessError:
                    if s._strict:
                        raise
                    else:
                        s.broken.append(f)
                        continue
                yield f, set(lbs)
        return generator(self)
