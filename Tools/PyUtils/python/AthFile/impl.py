# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PyUtils/python/AthFile/impl.py
# @purpose a simple abstraction of a file to retrieve informations out of it
# @author Sebastien Binet <binet@cern.ch>
# @date November 2009

from __future__ import with_statement, print_function

__author__  = "Sebastien Binet"
__doc__ = "implementation of AthFile-server behind a set of proxies to isolate environments"

import errno
import os
import six

import PyUtils.Helpers as H
from .timerdecorator import timelimit, TimeoutError

# see bug #95942 for the excruciating details
try:
    from AthenaCommon.Include import excludeTracePattern
    excludeTracePattern.append("*cache.ascii.gz")
    del excludeTracePattern
except Exception:
    pass

### globals -------------------------------------------------------------------
DEFAULT_AF_RUN = os.environ.get('DEFAULT_AF_RUN', False)
'''Revert to old file peeking via Athena sub-process if True.'''

DEFAULT_AF_CACHE_FNAME = os.environ.get('DEFAULT_AF_CACHE_FNAME',
                                        'athfile-cache.ascii.gz')

DEFAULT_AF_TIMEOUT = 20
'''Default timeout for commands to be completed.'''

### utils ----------------------------------------------------------------------

# Try to convert long->int in output when we're running with python2,
# to prevent interoperability problems if we then read with python3.
def _clean_dict (d):
    for k, v in d.items():
        if isinstance(v, dict):
            _clean_dict(v)
        elif isinstance(v, list):
            _clean_list(v)
        elif type(v) in six.integer_types:
            d[k] = int(v)
    return
def _clean_list (l):
    for i in range(len(l)):
        v = l[i]
        if isinstance(v, dict):
            _clean_dict(v)
        elif isinstance(v, list):
            _clean_list(v)
        elif type(v) in six.integer_types:
            l[i] = int(v)
    return

def _get_real_ext(fname):
    """little helper to get the 'real' extension of a filename, handling 'fake' extensions (e.g. foo.ascii.gz -> .ascii)"""
    se = os.path.splitext
    f,ext = se(fname)
    if ext in ('.gz',):
        _,ext = se(f)
    return ext

def _my_open(name, mode='r', bufsiz=-1):
    """helper method to handle gzipped or not files.
    if `name` ends with '.gz' the correct gzip.open function will be called.
    """
    f,ext = os.path.splitext(name)
    if ext in ('.gz',):
        import gzip
        def gzip_exit(self, type, value, traceback):
            return self.close()
        def gzip_enter(self):
            return self
        gzip.GzipFile.__exit__ = gzip_exit
        gzip.GzipFile.__enter__= gzip_enter
        return gzip.open(name, mode + 't')
    else:
        return open(name, mode, bufsiz)
    
def _find_file(filename, pathlist, access):
    """Find <filename> with rights <access> through <pathlist>."""
    # special case for those filenames that already contain a path
    if os.path.dirname(filename):
        if os.access(filename, access):
            return filename

    # test the file name in all possible paths until first found
    for path in pathlist:
        f = os.path.join(path, filename)
        if os.access(f, access):
            return f

    # no such accessible file avalailable
    return None

def _setup_ssl(msg, root):
    x509_proxy = os.environ.get('X509_USER_PROXY', '')
    if x509_proxy:
        # setup proper credentials
        root.TSSLSocket.SetUpSSL(
            x509_proxy,
            "/etc/grid-security/certificates",
            x509_proxy,
            x509_proxy)
    else:
        msg.warning("protocol https is requested but no X509_USER_PROXY was found! (opening the file might fail.)")
        pass
    return
    
def _create_file_infos():
    """simple helper function to create consistent dicts for the
    fileinfos attribute of AthFile
    """
    d = {
        'file_md5sum': None,
        'file_name': None,
        'file_size': -1,
        'file_type': None,
        'file_guid': None,
        'nentries' : 0, # to handle empty files
        'run_number': [],
        'run_type': [],
        'evt_type': [],
        'evt_number': [],
        'lumi_block': [],
        'beam_energy': [],
        'beam_type':   [],
        'stream_tags': [],
        'mc_channel_number': [],
        'metadata_items': None,
        'eventdata_items': None,
        'stream_names': None,
        'geometry': None,
        'conditions_tag': None,
        'det_descr_tags': None,
        'metadata': None,
        'tag_info': None,
        }
    return d

def ami_dsinfos(dsname):
    """a helper function to query AMI for informations about a dataset name.
    `dsname` can be either a logical dataset name (a bag of files) or a
    logical filename.
    """
    import PyUtils.AmiLib as A
    import PyUtils.xmldict as _x
    import xml.etree.cElementTree as ET

    # keep order of tokens !
    for token in ('ami://', '//', '/'):
        if dsname.startswith(token):
            dsname = dsname[len(token):]
            pass
        pass
    
    ami = A.Client()
    try:
        res = ami.exec_cmd(cmd="GetDatasetInfo", logicalFileName=dsname)
    except A.PyAmi.AMI_Error:
        # maybe a logical dataset name then ?
        res = ami.exec_cmd(cmd="GetDatasetInfo", logicalDatasetName=dsname)
    res = _x.xml2dict(ET.fromstring(res.transform('xml')))
    data = res['AMIMessage']['Result']
        
    # get only interesting informations...
    rowset = data['rowset']
    if isinstance(rowset, list):
        fields = rowset[-1]['row']['field']
    else:
        fields = rowset['row']['field']

    # translate into athfile-infos format
    af_infos = _create_file_infos()
    for i in fields:
        if not ('name' in i and '_text' in i):
            continue
        k = i['name']
        v = i['_text']

        if v.lower() == 'none':
            v = None
            
        if k == 'logicalDatasetName':
            af_infos['file_name'] = 'ami://'+v

        elif k == 'totalEvents':
            af_infos['nentries'] = int(v)

        elif k == 'runNumber':
            af_infos['run_number'] = [int(v)]
            
        elif k == 'geometryVersion':
            af_infos['geometry'] = v

        elif k == 'conditionsTag':
            af_infos['conditions_tag'] = v

        elif k == 'beamType':
            af_infos['beam_type'] = [v]
            
        elif k == 'dataType':
            af_infos['file_type'] = 'bs' if v.lower() == 'raw' else 'pool'
            stream_name = 'Stream' + v.upper()
            af_infos['stream_names'] = [stream_name]

        elif k == 'streamName':
            stream_type,stream_name = v.split('_')
            af_infos['stream_tags'] = [
                {'obeys_lbk': None,
                 'stream_type': stream_type,
                 'stream_name': stream_name}
                ]
    # FIXME !!
    af_infos['file_guid'] = af_infos['file_name']
    # FIXME !!
    if not af_infos['run_number']:
        dsname = af_infos['file_name']
        idx = [i for i,j in enumerate(fields)
               if j['name']=='contained_dataset']
        if len(idx)==1:
            #try to extract run-number from the name of the first dataset
            dsname = fields[idx[0]]['_text'].split(';')[0]
            try:
                # an AMI dsname is of the form:
                # (project_name).(run_nbr).[...]
                run_number = dsname.split('.')[1]
                af_infos['run_number'] = [int(run_number)]
            except ValueError:
                pass
                
        else:
            try:
                # an AMI dsname is of the form:
                # (project_name).(run_nbr).[...]
                run_number = dsname.split('.')[1]
                af_infos['run_number'] = [int(run_number)]
            except ValueError:
                pass
        pass
    
    return af_infos

 
### classes -------------------------------------------------------------------
class AthFile (object):
    """A handle to an athena file (POOL,ROOT or ByteStream)
    """
    __slots__ = ('fileinfos',)
    
    @staticmethod
    def from_infos(infos):
        o = AthFile()
        o.fileinfos = _create_file_infos() # ensure basic layout
        o.fileinfos.update(infos.copy())
        return o

    @staticmethod
    def from_fname(fname):
        import PyUtils.AthFile as af
        return af.fopen(fname)

    @property
    def name(self):
        return self.fileinfos['file_name']
    
    @property
    def nentries(self):
        return self.fileinfos['nentries']
    
    @property
    def infos(self):
        return self.fileinfos

    @property
    def run_number (self):
        """return the list of unique run-numbers the @c AthFile contains"""
        return list(set(self.infos['run_number']))
    # backward compatibility
    run_numbers = run_number
    
    @property
    def mc_channel_number (self):
        """return the list of unique mc_channel-numbers the @c AthFile contains"""
        return list(set(self.infos['mc_channel_number']))
    # ATEAM-168: requested for derivations
    mc_channel_numbers = mc_channel_number
    
    @property
    def evt_number (self):
        """return the list of unique evt-numbers the @c AthFile contains"""
        return list(set(self.infos['evt_number']))
    
    @property
    def lumi_block (self):
        """return the list of unique lumi-block nbrs the @c AthFile contains
        """
        return list(set(self.infos['lumi_block']))
    
    @property
    def run_type (self):
        """return the list of unique run-types the @c AthFile contains"""
        return list(set(self.infos['run_type']))
    
    @property
    def beam_type (self):
        """return the list of unique beam-types the @c AthFile contains"""
        return list(set(self.infos['beam_type']))
    
    @property
    def beam_energy (self):
        """return the list of unique beam-energies the @c AthFile contains"""
        return list(set(self.infos['beam_energy']))
    
    pass # AthFile class

class AthFileServer(object):
    """the object serving AthFile requests
    """
    
    def __init__(self):

        import PyUtils.Logging as _L
        self._msg = _L.logging.getLogger("AthFile")
        self.set_msg_lvl(_L.logging.INFO)
        
        if os.environ.get('ATHFILE_DEBUG', '0') == '1':
            import PyUtils.Logging as _L
            self.set_msg_lvl(_L.logging.VERBOSE)
            pass
        
        self.msg().debug('importing ROOT...')
        import PyUtils.RootUtils as ru
        self.pyroot = ru.import_root()
        try:
            ru._pythonize_tfile()
        except Exception as err:
            self.msg().warning('problem during TFile pythonization:\n%s', err)
            
        self.msg().debug('importing ROOT... [done]')

        # a cache of already processed requests
        self._cache = {}
        self._do_pers_cache = True
        self.enable_pers_cache()
        return

    # make the _peeker on-demand to get an up-to-date os.environ
    @property
    def _peeker(self):
        return FilePeeker(self)
    
    def _cleanup_pyroot(self):
        import PyUtils.RootUtils as ru
        root = ru.import_root()
        tfiles = root.gROOT.GetListOfFiles()[:]
        for i,f in enumerate(tfiles):
            try:
                if f:
                    f.Close()
                    del f
            except Exception as err:
                self._msg.info('could not close a TFile:\n%s', err)
                pass
        tfiles[:] = []

    def msg(self):
        return self._msg
    
    def set_msg_lvl(self, lvl):
        self.msg().setLevel(lvl)
        
    def _md5_for_file(self, f, block_size=2**20, do_fast_md5=True):
        """helper function to calculate a MD5 checksum
        ``f`` can be filename, an open python file or an open TFile
        """
        import hashlib
        md5 = hashlib.md5()
        do_close = False
        if isinstance(f, str):
            protocol,fname = self.fname(f)
            f = self._root_open(fname)
            do_close = True
            if f is None or not f:
                raise IOError(errno.ENOENT,
                              "No such file or directory",
                              fname)

        assert hasattr(f, 'read'), \
               "'f' must be a file-like object. (f=%r, type=%s)"%(
            f,type(f),
            )
        orig_pos = f.tell()
        f.seek(0)
        try:
            while True:
                data = f.read(block_size)
                if not data:
                    break
                md5.update(data)
                if do_fast_md5:
                    break
        finally:
            f.seek(orig_pos)
        if do_close:
            f.Close()
        return md5.hexdigest()
    
    def _root_open(self, fname, raw=True):
        root = self.pyroot
        import re
        with H.ShutUp(filters=[
            re.compile('TClass::TClass:0: RuntimeWarning: no dictionary for class.*') ]):
            root.gSystem.Load('libRootCollection')
            root_open = root.TFile.Open

            # we need to get back the protocol b/c of the special
            # case of secure-http which needs to open TFiles as TWebFiles...
            protocol, _ = self.fname(fname)
            if protocol == 'https':
                _setup_ssl(self.msg(), root)
                root_open = root.TWebFile.Open
            if raw:
                if protocol == 'https' and '?' in fname:
                   # append filetype to existing parameters
                   f = root_open(fname+'&filetype=raw', 'READ')
                else:
                   f = root_open(fname+'?filetype=raw', 'READ')
            else:
                f = root_open(fname, 'READ')
            if f is None or not f:
                raise IOError(errno.ENOENT,
                              'No such file or directory',fname)
            return f
        return

    def pfopen(self, fnames, evtmax=1):
        if isinstance(fnames, (list, tuple)):
            self.msg().debug("using mp.pool... (files=%s)", len(fnames))
            fct = _do_fopen
            do_pers_cache = self._do_pers_cache
            self.disable_pers_cache()
            import multiprocessing as mp
            from multiprocessing.pool import ThreadPool
            # Never run more than 4 parallel instances
            pool_sz = min(mp.cpu_count(), 4)
            pool = ThreadPool(pool_sz)

            infos = None
            try:
                setattr(self, '_evtmax', evtmax)
                infos = pool.map(fct, fnames)
            finally:
                delattr(self, '_evtmax')
                if do_pers_cache:
                    self.enable_pers_cache()
                    pass
                pass
            # collect back infos into ourself
            for f in infos:
                fname = f.infos['file_name']
                self._cache[fname] = f
                pass
            # synchronize once
            try:
                self._sync_pers_cache()
            except Exception as err:
                self.msg().info('could not synchronize the persistent cache:\n%s', err)
                pass
                
            return infos
        return self._fopen_file(fnames, evtmax)
        
    def fopen(self, fnames, evtmax=1):
        if isinstance(fnames, (list, tuple)):
            infos = []
            for fname in fnames:
                info = self._fopen_file(fname, evtmax)
                infos.append(info)
            return infos
        return self._fopen_file(fnames, evtmax)
        
    def _fopen_stateless(self, fname, evtmax):
        msg = self.msg()
        cache = dict(self._cache)
        fids = []
        for k,v in six.iteritems (cache):
            v = v.infos
            fid = v.get('file_md5sum', v['file_guid'])
            if fid:
                fids.append((fid,k))
            pass
        for v in fids:
            fid, k = v
            cache[fid] = k
            pass
        
        protocol, fname = self.fname(fname)
        if protocol in ('fid', 'lfn'):
            protocol, fname = self.fname(fname)

        use_cache = False
        sync_cache = True
        if protocol in ('', 'file') :
            fid = self.md5sum(fname)
            fid_in_cache = fid in cache
            # also check the cached name in case 2 identical files
            # are named differently or under different paths
            fid_match_fname = cache.get(fid,None) == fname
            if fid_in_cache and fid_match_fname:
                use_cache = True
                sync_cache = False
                msg.debug('fetched [%s] from cache (md5sum is a match)', fname)
                f = cache[cache[fid]]
        elif protocol in ('ami',):
            use_cache = True
            sync_cache = True # yes, we want to update the pers. cache
            # take data from AMI
            infos = ami_dsinfos(fname[len('ami://'):])
            msg.debug('fetched [%s] from cache', fname)
            fid = infos.get('file_md5sum', infos['file_guid'])
            cache[fid] = fname
            f = AthFile.from_infos(infos)
            cache[fname] = f
            # hysteresis...
            cache[infos['file_name']] = f
        else:
            # use the cache indexed by name rather than md5sums to
            # skip one TFile.Open...
            # Note: we assume files on mass storage systems do not
            # change very often.
            if fname in self._cache:
                use_cache = True
                sync_cache = False
                msg.debug('fetched [%s] from cache', fname)
                f = cache[fname]

        if not use_cache:
            msg.info("opening [%s]...", fname)
            infos = self._peeker(fname, evtmax)
            f = AthFile.from_infos(infos)
            cache[fname] = f
            # hysteresis...
            cache[infos['file_name']] = f
            sync_cache = True
            pass

        # remove the fids we added...
        for v in fids:
            fid, k = v
            # in case there were duplicate fids
            try:             del cache[fid]
            except KeyError: pass
            pass

        return (fname, cache, sync_cache)

    def _fopen_file(self, fname, evtmax):
        msg = self.msg()
        fname, cache, sync_cache = self._fopen_stateless(fname, evtmax)
        if sync_cache:
            try:
                self._cache = cache
                self._sync_pers_cache()
            except Exception as err:
                msg.info('could not synchronize the persistent cache:\n%s', err)
            pass
        return self._cache[fname]
    
    def md5sum(self, fname):
        """return the md5 checksum of file ``fname``
        """
        if isinstance(fname, str):
            protocol,fname = self.fname(fname)
        
        md5 = self._md5_for_file(fname)
        return md5
    
    @timelimit(timeout=DEFAULT_AF_TIMEOUT)
    def fname(self, fname):
        """take a file name, return the pair (protocol, 'real' file name)
        """
        fname = os.path.expanduser(os.path.expandvars(fname))

        msg = self.msg()
        
        def _normalize_uri(uri):
            if uri.startswith('/'):
                return 'file:'+uri
            return uri
        
        from six.moves.urllib.parse import urlsplit
        url = urlsplit(_normalize_uri(fname))
        protocol = url.scheme
        def _normalize(fname):
            from posixpath import normpath
            fname = normpath(fname)
            if fname.startswith('//'): fname = fname[1:]
            return fname

        if protocol in ('', 'file', 'pfn'):
            protocol = ''
            fname = _normalize(url.path)

            ## hack for '/castor/cern.ch/...' paths
            if fname.startswith('/castor/'):
                protocol = 'rfio'
                fname = protocol + ':' + fname
                
        elif protocol in ('rfio', 'castor'):
            protocol = 'rfio'
            fname = _normalize(url.path)
            fname = protocol+':'+fname

        elif protocol in ('root','dcap', 'dcache', 'http', 'https', 'dav', 'davs'):
            pass

        elif protocol in ('gsidcap',):
            protocol = 'gfal:gsidcap'
            pass
        
        elif protocol in ('lfn','fid',):
            # percolate through the PoolFileCatalog
            from PyUtils.PoolFile import PoolFileCatalog as pfc
            fname = pfc().pfn(protocol+':'+url.path)
            pass

        elif protocol in ('ami',):
            # !! keep order of tokens !
            for token in ('ami:', '//', '/'):
                if fname.startswith(token):
                    fname = fname[len(token):]
            fname = 'ami://' + fname
            pass
        
        else:
            msg.warning('unknown protocol [%s]. we\'ll just return our input',
                        protocol)
            pass
        
        return (protocol, fname)

    def cache(self):
        return self._cache

    def enable_pers_cache(self):
        """configure the file server to write out the persistent cache
        of inspected files.
        """
        # first disable previous cache, if any, to prevent hysteresis...
        self.disable_pers_cache()
        msg = self.msg()
        self._do_pers_cache = True
        
        fname = DEFAULT_AF_CACHE_FNAME
        if (fname and
            os.path.exists(fname) and
            os.access(fname, os.R_OK)):
            msg.info('loading cache from [%s]...', fname)
            try:
                self.load_cache(fname)
                msg.info('loading cache from [%s]... [done]', fname)
            except TimeoutError:
                msg.info('loading cache timed out!')
        return

    def disable_pers_cache(self):
        """configure the file server to NOT write out the persistent cache
        of inspected files.
        if the persistent cache wasn't enabled, this is a no-op.
        """
        self._do_pers_cache = False
        return
    
    def _sync_pers_cache(self):
        if not self._do_pers_cache:
            return
        msg = self.msg()
        fname = DEFAULT_AF_CACHE_FNAME
        if not fname:
            # protect against empty or invalid (None) cache file names
            return
        import uuid
        pid = str(os.getpid())+'-'+str(uuid.uuid4())
        fname_,fname_ext = os.path.splitext(fname)
        if fname_ext in ('.gz',):
            fname_,fname_ext = os.path.splitext(fname_)
            pid_fname = fname_ + '.' + pid + fname_ext + ".gz"
        else:
            pid_fname = fname_ + '.' + pid + fname_ext
        msg.debug('synch-ing cache to [%s]...', fname)
        try:
            msg.debug('writing to [%s]...', pid_fname)
            self.save_cache(pid_fname)
            if os.path.exists(pid_fname):
                # should be atomic on most FS...
                os.rename(pid_fname, fname)
            else:
                msg.warning("could not save to [%s]", pid_fname)
            msg.debug('synch-ing cache to [%s]... [done]', fname)
        except Exception as err:
            msg.debug('synch-ing cache to [%s]... [failed]', fname)
            msg.debug('reason:\n%s', err)
            pass
        return

    # dead-lock on self.msg (I think!)...
    #@timelimit(timeout=DEFAULT_AF_TIMEOUT)
    def load_cache(self, fname=DEFAULT_AF_CACHE_FNAME):
        """load file informations from a cache file.
        the back-end (JSON, ASCII, pickle, ...) is inferred from the
        extension of the `fname` parameter.
        defaults to py-ASCII.
        """
        msg = self.msg()

        ext = _get_real_ext(os.path.basename(fname))
        if len(ext) == 0:
            # illegal file...
            msg.info('load_cache: invalid file [%s]', fname)
            return

        ext = ext[1:] if ext[0]=='.' else ext
        try:
            loader = getattr(self, '_load_%s_cache'%ext)
        except AttributeError:
            msg.info('load_cache: could not find a suitable backend for '
                     'extension [.%s] => using [ascii]', ext)
            loader = self._load_ascii_cache

        try:
            search_path = os.environ.get('DATAPATH',os.getcwd())
            search_path = search_path.split(os.pathsep)
            fname = _find_file(os.path.expanduser(os.path.expandvars(fname)),
                             search_path,
                             os.R_OK) or fname
        except ImportError:
            # not enough karma... tough luck!
            pass

        # ensure one can read that file...
        with open(fname, 'r'):
            pass

        msg.debug('loading cache from [%s]...', fname)
        cache = {}
        try:
            cache = loader(fname)
        except Exception as err:
            msg.info("problem loading cache from [%s]!", fname)
            msg.info(repr(err))
            pass

        self._cache.update(cache)
        msg.debug('loading cache from [%s]... [done]', fname)

    def save_cache(self, fname=DEFAULT_AF_CACHE_FNAME):
        """save file informations into a cache file.
        the back-end (JSON, ASCII, pickle, ...) is inferred from the
        extension of the `fname` parameter.
        falls back to py-ASCII.
        """
        msg = self.msg()
        if os.path.exists(fname):
            os.rename(fname, fname+'.bak')
        ext = _get_real_ext(fname)
        ext = ext[1:] # drop the dot
        try:
            saver = getattr(self, '_save_%s_cache'%ext)
        except AttributeError:
            msg.info('save_cache: could not find a suitable backend for '
                     'extension [.%s] => using [ascii]', ext)
            saver = self._save_ascii_cache
        try:
            saver(fname)
        except IOError as err:
            import errno
            if err.errno != errno.EACCES:
                raise
            else:
                msg.info('could not save cache in [%s]', fname)
        except Exception as err:
            msg.warning('could not save cache into [%s]:\n%s', fname, err)
        return
    
    def _load_pkl_cache(self, fname):
        """load file informations from pickle/shelve 'fname'"""
        try: import cPickle as pickle
        except ImportError: import pickle
        import shelve
        db = shelve.open(fname, protocol=pickle.HIGHEST_PROTOCOL)
        return db['fileinfos_cache'].copy()

    def _save_pkl_cache(self, fname):
        """save file informations into pickle/shelve 'fname'"""
        try: import cPickle as pickle
        except ImportError: import pickle
        import shelve
        db = shelve.open(fname, protocol=pickle.HIGHEST_PROTOCOL)
        db['fileinfos_cache'] = self._cache.copy()
        db.close()
        return
    
    def _load_json_cache(self, fname):
        """load file informations from a JSON file"""
        import json
        with _my_open(fname) as fd:
            cache = json.load(fd)
        return dict((k,AthFile.from_infos(v)) for k,v in cache)
        
    def _save_json_cache(self, fname):
        """save file informations using JSON"""
        import json
        cache = self._cache
        with _my_open(fname, 'w') as fd:
            json.dump([(k, cache[k].fileinfos) for k in cache],
                      fd,
                      indent=2,
                      sort_keys=True)
        return
    
    def _load_ascii_cache(self, fname):
        """load file informations from a pretty-printed python code"""
        dct = {}
        ast = compile(_my_open(fname).read(), fname, 'exec')
        exec (ast, dct,dct)
        del ast
        try:
            cache = dct['fileinfos']
        except Exception:
            raise
        finally:
            del dct
        return dict((k,AthFile.from_infos(v)) for k,v in cache)
    
    def _save_ascii_cache(self, fname):
        """save file informations into pretty-printed python code"""
        from pprint import pprint
        cache = self._cache
        with _my_open(fname, 'w') as fd:
            print ("# this is -*- python -*-", file=fd)
            print ("# this file has been automatically generated.", file=fd)
            print ("fileinfos = [", file=fd)
            fd.flush()
            for k in cache:
                print ("\n## new-entry", file=fd)
                if six.PY2:
                    _clean_dict (cache[k].fileinfos)
                pprint((k, cache[k].fileinfos),
                       stream=fd,
                       width=120)
                fd.flush()
                print (", ", file=fd)
            print ("]", file=fd)
            print ("### EOF ###", file=fd)
            fd.flush()
        return
    
    def _load_db_cache(self, fname):
        """load file informations from a sqlite file"""
        import PyUtils.dbsqlite as dbsqlite
        cache = dbsqlite.open(fname)
        d = {}
        for k,v in six.iteritems (cache):
            d[k] = AthFile.from_infos(v)
        return d
        
    def _save_db_cache(self, fname):
        """save file informations using sqlite"""
        import PyUtils.dbsqlite as dbsqlite
        db = dbsqlite.open(fname,flags='w')
        cache = self._cache
        for k in cache:
            db[k] = cache[k].fileinfos
        db.close()
        return
    
    def flush_cache(self):
        self._cache = {}
        return

    @timelimit(timeout=DEFAULT_AF_TIMEOUT)
    def ftype(self, fname):
        """
        returns the type of a file ('pool' or 'bs') together with its
        canonical name. `fname` can be a string or a `ROOT.TFile` handle.

        example:
        >>> import PyUtils.AthFile as af
        >>> af.ftype ('castor:/castor/cern.ch/foo.pool')
        ('pool', 'rfio:/castor/cern.ch/foo.pool')
        
        >>> af.ftype ('LFN:ttbar.pool')
        ('pool', '/afs/cern.ch/somewhere/ttbar.pool')
        
        >>> af.ftype ('rfio:/castor/cern.ch/bs.data')
        ('bs', 'rfio:/castor/cern.ch/bs.data')
        
        >>> af.ftype ('rfio:/castor/cern.ch/bs.data')
        ('bs', 'rfio:/castor/cern.ch/bs.data')
        """

        _is_root_file = None
        do_close = True
        if isinstance(fname, str):
            if not self.exists(fname):
                import errno
                raise IOError(
                    errno.ENOENT,
                    'No such file or directory',
                    fname
                    )
            protocol,fname = self.fname(fname)
            if protocol == 'ami':
                # FIXME: what (else) can we do ?
                ami_infos = self.fopen(fname).infos
                return ami_infos['file_type'], fname

            f = self._root_open(fname)
        else:
            do_close = False
            f = fname
            
        _is_root_file= bool(f and f.IsOpen() and b'root' in f.read(10))
        if f and do_close:
            f.Close()
            del f

        ftype = 'pool' if _is_root_file else 'bs'
        return (ftype, fname)

    @timelimit(timeout=DEFAULT_AF_TIMEOUT)
    def exists(self, fname):
        """helper function to test if a fiven `fname` exists.

        handles local filesystems as well as RFIO.
        usage example:
        >>> import PyUtils.AthFile as af
        >>> af.exists('/castor/cern.ch/user/b/binet/reffiles/14.1.0.x/AllBasicSamples.AOD.pool.root')
        False
        >>> af.exists('rfio:/castor/cern.ch/user/b/binet/reffiles/14.1.0.x/AllBasicSamples.AOD.pool.root')
        True
        >>> af.exists('castor:/castor/cern.ch/user/b/binet/reffiles/14.1.0.x/AllBasicSamples.AOD.pool.root')
        True
        >>> # you need a valid PoolFileCatalog.xml file for this to work:
        >>> af.exists('LFN:top_CSC-01-02-00_RDO_extract.pool')
        True
        >>> af.exists('/afs/cern.ch/atlas/offline/ReleaseData/v2/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root')
        True
        """

        def _root_exists(fname):
            exists = False
            f = None
            try:
                f = self._root_open(fname)
                exists = f and f.IsOpen()
            except Exception:
                # swallow...
                pass
            finally:
                if f:
                    f.Close()
                    del f
            return bool(exists)

        protocol,fname = self.fname(fname)

        if protocol in ('fid', 'lfn'):
            return self.exists(fname)

        elif protocol in ('ami',):
            # FIXME: what else can we do ?
            try:
                ami_dsinfos(fname)
                return True
            except Exception:
                return False
        
        else:
            return _root_exists(fname)
        # un-reachable
        return False

    pass # class AthFileServer

class FilePeeker(object):
    def __init__(self, server):
        self.server= server
        self.msg   = server.msg
        self.pyroot= server.pyroot
        self._sub_env = dict(os.environ)
        # prevent ROOT from looking into $HOME for .rootrc files
        # we carefully (?) set this environment variable *only* in the
        # subprocess to not stomp on the toes of our parent one which is
        # user-driven (and might need user-customized macros or configurations)
        self._sub_env['ROOTENV_NO_HOME'] = '1'

        # prevent from running athena in interactive mode (and freeze)
        if 'PYTHONINSPECT' in self._sub_env:
            del self._sub_env['PYTHONINSPECT']

        # prevent from running athena with igprof
        for k in ('LD_PRELOAD', 'IGPROF'):
            if k in self._sub_env:
                del self._sub_env[k]

    def _root_open(self, fname, raw=False):
        return self.server._root_open(fname, raw)
 
    def _is_tag_file(self, fname, evtmax):
        is_tag = False
        tag_ref= None
        tag_guid=None
        nentries = 0
        runs=[]
        evts=[]
        do_close = True
        if isinstance(fname, str):
            f = self._root_open(fname, raw=False)
        else:
            f = fname
            do_close = False
        schema = f.Get('Schema') if f else None
        if schema:
            is_tag  = True
            # note: we used to use .rstrip('\0') b/c of the change in
            # semantics in PyROOT (char[] and const char* may not mean
            # the same thing)
            # see https://savannah.cern.ch/bugs/?100920 for the gory details
            # but in the end, we use ctypes...
            # see https://savannah.cern.ch/bugs/?101200 for the gory details
            import ctypes
            tag_ref = str(ctypes.c_char_p(schema.m_eventRefColumnName).value)
        del schema
        metadata= f.Get('CollectionMetadata') if f else None
        if metadata:
            metadata.GetEntry(0)
            # note: we used to use .rstrip('\0') b/c of the change in
            # semantics in PyROOT (char[] and const char* may not mean
            # the same thing)
            # see https://savannah.cern.ch/bugs/?100920 for the gory details
            # but in the end, we use ctypes...
            # see https://savannah.cern.ch/bugs/?101200 for the gory details
            # 
            # make sure it is what we think it is
            import ctypes
            key_name = str(ctypes.c_char_p(metadata.Key).value)
            assert key_name == 'POOLCollectionID' 
            tag_guid = str(ctypes.c_char_p(metadata.Value).value)
        del metadata
        coll_tree = f.Get('POOLCollectionTree') if f else None
        if coll_tree:
            nentries = coll_tree.GetEntries()
            if evtmax in (-1, None):
                evtmax = nentries
            evtmax = int(evtmax)
            for row in range(evtmax):
                if coll_tree.GetEntry(row) < 0:
                    break
                # With root 5.34.22, trying to access leaves of a
                # fundamental type like this gives an error:
                #   TypeError: attempt to bind ROOT object w/o class
                # Rewrite like this for now to work around the problem.
                #runnbr = coll_tree.RunNumber
                runnbr = coll_tree.GetBranch('RunNumber').GetListOfLeaves()[0].GetValueLong64()
                runs.append(runnbr)
                #evtnbr = coll_tree.EventNumber
                evtnbr = coll_tree.GetBranch('EventNumber').GetListOfLeaves()[0].GetValueLong64()
                evts.append(evtnbr)
        del coll_tree
        if f and do_close:
            f.Close()
            del f
        return (is_tag, tag_ref, tag_guid, nentries, runs, evts)

    def _is_empty_pool_file(self, fname):
        is_empty = False
        do_close = True
        if isinstance(fname, str):
            f = self._root_open(fname, raw=False)
        else:
            f = fname
            do_close = False
        payload = f.Get('CollectionTree') if f else None
        if payload:
            is_empty = False
        else:
            is_empty = True
        del payload

        if f and do_close:
            f.Close()
            del f
        return is_empty
     
    def _process_call(self, fname, evtmax, projects=['AtlasCore']):
        msg = self.msg()
        f = _create_file_infos()
        protocol, _ = self.server.fname(fname)
        f_raw  = self._root_open(fname, raw=True)
        if f_raw is None or not f_raw:
            raise IOError(
                errno.ENOENT,
                'No such file or directory',
                fname)
        f_root = f_raw
        try:
            file_type, file_name = self.server.ftype(f_raw)

            protocol,file_name = self.server.fname(fname)
            f['file_md5sum'] = self.server.md5sum(f_raw)
            f['file_name'] = file_name
            f['file_type'] = file_type
            f['file_size'] = f_raw.GetSize()
            if file_type == 'pool':
                f_root = self._root_open(fname, raw=False)
                # POOL files are most nutritious when known to PoolFileCatalog.xml
                # FIXME: best would be to do that in athfile_peeker.py but
                #        athena.py closes sys.stdin when in batch, which confuses
                #        PyUtils:subprocess.getstatusoutput
                #
                # ATEAM-192: avoid the PoolFileCatalog.xml conflict
                #cmd = ['pool_insertFileToCatalog.py',
                #       file_name,]
                #subprocess.call(cmd, env=self._sub_env)
                #
                if True:
                    is_tag, tag_ref, tag_guid, nentries, runs, evts = self._is_tag_file(f_root, evtmax)
                    if is_tag:
                        f['stream_names'] = ['TAG']
                        f['file_guid'] = tag_guid
                        f['nentries'] = nentries
                        f['run_number'] = runs
                        f['evt_number'] = evts
                    else:
                        import tempfile
                        fd_pkl,out_pkl_fname = tempfile.mkstemp(suffix='.pkl')
                        os.close(fd_pkl)
                        if os.path.exists(out_pkl_fname):
                            os.remove(out_pkl_fname)
                        print ("\n  ---------   running Athena peeker")
                        print (os.environ.get('CMTPATH',''))

                        import AthenaCommon.ChapPy as api
                        app = api.AthenaApp(cmdlineargs=["--nprocs=0"])
                        app << """
                            FNAME = %s
                            """ % str([file_name])
                        app << """
                            import os
                            # prevent from running athena in interactive mode (and freeze)
                            if 'PYTHONINSPECT' in os.environ:
                                del os.environ['PYTHONINSPECT']
            

                            include('AthenaPython/athfile_peeker.py')
                            from AthenaCommon.AlgSequence import AlgSequence
                            job = AlgSequence()
                            # we don't really need this...
                            job.peeker.outfname='%(outfname)s'
                            job.peeker.infname='%(infname)s'

                            # metadata + taginfo
                            import IOVDbSvc.IOVDb

                            # evt-max
                            theApp.EvtMax = %(evtmax)i
                            """ % {
                            'infname' : file_name,
                            'outfname': out_pkl_fname,
                            'evtmax': evtmax,
                            }
                        import uuid
                        stdout_fname = (
                            'athfile-%i-%s.log.txt' %
                            (os.getpid(), uuid.uuid4())
                            )
                        stdout = open(stdout_fname, "w")
                        print ("="*80, file=stdout)
                        print (self._sub_env, file=stdout)
                        print ("="*80, file=stdout)
                        stdout.flush()
                        if DEFAULT_AF_RUN:
                            sc = app.run(stdout=stdout, env=self._sub_env)
                        else:
                            import PyUtils.FilePeekerTool as fpt
                            fp = fpt.FilePeekerTool(f_root)
                            sc, fp_pkl_fname = fp.run()
                            # revert to athena sub-process in case of file with old schema
                            if sc == 0:
                                out_pkl_fname = fp_pkl_fname
                            else:
                                sc = app.run(stdout=stdout, env=self._sub_env)
                        stdout.flush()
                        stdout.close()
                        import AthenaCommon.ExitCodes as ath_codes
                        if sc == 0:
                            #import shelve
                            import PyUtils.dbsqlite as dbsqlite
                            msg.info('extracting infos from [%s]...',
                                     out_pkl_fname)
                            db = dbsqlite.open(out_pkl_fname)
                            msg.info('keys: %s',db.keys())
                            f.update(db['fileinfos'])
                            db.close()
                            msg.info('extracting infos from [%s]... [ok]',
                                     out_pkl_fname)
                            os.remove(stdout.name)
                        else:
                            # maybe an empty file
                            # trust but verify
                            if not self._is_empty_pool_file(f_root):
                                # actually a problem in athena !
                                from textwrap import dedent
                                err = dedent("""
                                %s
                                problem running chappy!
                                code: [%s (%s)]
                                what: [%s]
                                => corrupted input file ?
                                %s
                                logfile: [%s]
                                """% (":"*25,
                                      sc,errno.errorcode.get(sc,sc),
                                      ath_codes.codes.get(sc,sc),
                                      ":"*25,
                                      stdout.name
                                      ))
                                msg.error(err)
                                raise IOError(sc, err)
                            msg.info('athena failed to initialize.')
                            msg.info('=> probably an empty input POOL file')
                    # TAG-file
            else: # bytestream
                bs_fileinfos = self._process_bs_file(file_name,
                                                     evtmax=evtmax,
                                                     full_details=False)
                del bs_fileinfos['file_name']
                del bs_fileinfos['file_size']
                del bs_fileinfos['file_type']
                del bs_fileinfos['file_md5sum']
                f.update(bs_fileinfos)
        finally:
            try:
                f_raw.Close()
                f_root.Close()
                del f_raw
                del f_root
            except Exception as err:
                msg.warning(
                    'problem while closing raw and root file handles:\n%s',
                    err
                    )
        return f

    def __call__(self, fname, evtmax):
        import re
        import PyUtils.Helpers as H
        with H.ShutUp(filters=[re.compile('.*')]):
            f = self._process_call(fname, evtmax, projects=None)

        return f

    def _process_bs_file (self, fname, evtmax=1, full_details=True):
        msg = self.msg()
        import eformat as ef

        data_reader = ef.EventStorage.pickDataReader(fname)
        assert data_reader, \
               'problem picking a data reader for file [%s]'%fname

        beam_type   = '<beam-type N/A>'
        try:
            beam_type = data_reader.beamType()
        except Exception:
            msg.warning ("problem while extracting beam-type information")
            pass

        beam_energy = '<beam-energy N/A>'
        try:
            beam_energy = data_reader.beamEnergy()
        except Exception:
            msg.warning ("problem while extracting beam-type information")
            pass

        bs = ef.istream(fname)

        file_infos = _create_file_infos()
        nentries = bs.total_events
        file_infos['nentries'] = nentries
        import uuid
        def _uuid():
            return str(uuid.uuid4()).upper()
        bs_metadata = {}
        for md in data_reader.freeMetaDataStrings():
            if md.startswith('Event type:'):
                k = 'evt_type'
                v = []
                if 'is sim' in md:   v.append('IS_SIMULATION')
                else:                v.append('IS_DATA')
                if 'is atlas' in md: v.append('IS_ATLAS')
                else:                v.append('IS_TESTBEAM')
                if 'is physics' in md: v.append('IS_PHYSICS')
                else:                  v.append('IS_CALIBRATION')
                bs_metadata[k] = tuple(v)
            elif md.startswith('GeoAtlas:'):
                k = 'geometry'
                v = md.split('GeoAtlas:')[1].strip()
                bs_metadata[k] = v
            elif md.startswith('IOVDbGlobalTag:'):
                k = 'conditions_tag'
                v = md.split('IOVDbGlobalTag:')[1].strip()
                bs_metadata[k] = v
            elif '=' in md:
                k,v = md.split('=')
                bs_metadata[k] = v

        # for bwd/fwd compat...
        # see: https://savannah.cern.ch/bugs/?73208
        for key_name,fct_name in (
            ('GUID','GUID'),
            ('Stream','stream'),
            ('Project', 'projectTag'),
            ('LumiBlock', 'lumiblockNumber'),
            ('run_number', 'runNumber'),
            ):
            if key_name in bs_metadata:
                # no need: already in bs metadata dict
                continue
            if hasattr(data_reader, fct_name):
                v = getattr(data_reader, fct_name)()
                bs_metadata[key_name] = v
        # for bwd/fwd compat... -- END
            
        # fix for ATEAM-122
        if len(bs_metadata.get('evt_type','')) == 0 : # see: ATMETADATA-6
            evt_type = ['IS_DATA', 'IS_ATLAS']
            if   bs_metadata.get('Stream', '').startswith('physics_'):
                evt_type.append('IS_PHYSICS')
            elif bs_metadata.get('Stream', '').startswith('calibration_'):
                evt_type.append('IS_CALIBRATION')
            elif bs_metadata.get('Project', '').endswith('_calib'):        
                evt_type.append('IS_CALIBRATION')
            else:
                evt_type.append('Unknown')
            bs_metadata['evt_type'] = evt_type

        file_infos['file_guid'] = bs_metadata.get('GUID', _uuid())
        file_infos['evt_type']  = bs_metadata.get('evt_type', [])
        file_infos['geometry']  = bs_metadata.get('geometry', None)
        file_infos['conditions_tag'] = bs_metadata.get('conditions_tag', None)
        file_infos['bs_metadata'] = bs_metadata

        if not data_reader.good():
            # event-less file...
            file_infos['run_number'].append(bs_metadata.get('run_number', 0))
            file_infos['lumi_block'].append(bs_metadata.get('LumiBlock', 0))
            # FIXME: not sure how to do that...
            return file_infos
        
        if evtmax == -1:
            evtmax = nentries
            
        ievt = iter(bs)
        for i in range(evtmax):
            try:
                evt = next(ievt)
                evt.check() # may raise a RuntimeError
                stream_tags = [dict(stream_type=tag.type,
                                    stream_name=tag.name,
                                    obeys_lbk=bool(tag.obeys_lumiblock))
                               for tag in evt.stream_tag()]
                file_infos['run_number'].append(evt.run_no())
                file_infos['evt_number'].append(evt.global_id())
                file_infos['lumi_block'].append(evt.lumi_block())
                file_infos['run_type'].append(ef.helper.run_type2string(evt.run_type()))
                file_infos['beam_type'].append(beam_type)
                file_infos['beam_energy'].append(beam_energy)
                file_infos['stream_tags'].extend(stream_tags)

            except RuntimeError as err:
                print ("** WARNING ** detected a corrupted bs-file:\n",err)
        """
        detailed dump how-to:
        ---------------------
        import eformat as ef
        import eformat.dump as edump
        edump.event_callback.append (('.+', edump.fullevent_handler))
        edump.dump (stream=ef.istream(fname), skip=0, total=0)
        """
        return file_infos

    pass # class FilePeeker

### globals
g_server = AthFileServer()

def _do_fopen(fname):
    self = g_server
    evtmax= getattr(g_server, '_evtmax', 1)
    return self._fopen_file(fname, evtmax)
