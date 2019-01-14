# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# @file:    PyDumper/python/PyComps.py
# @purpose: A set of PyAthena components to test reading/writing EDM classes
# @author:  Sebastien Binet <binet@cern.ch>
# $Id: PyComps.py,v 1.11 2008-12-17 10:19:03 binet Exp $

__doc__     = 'A set of PyAthena components to test reading/writing EDM classes'
__version__ = '$Revision: 1.11 $'
__author__  = 'Sebastien Binet <binet@cern.ch>'

import os
from fnmatch import fnmatch
import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

def _gen_key_from_type(t):
    "helper function to generate a storegate key from a typename"
    return 'pyathena_tests__%s__key' % t

def _decode_item_list(lst, msg):
    """helper function to decode a (python) item list of storegate keys"""
    items = None
    if (lst is None or lst == '*'):
        items = None
    else:
        if isinstance (lst, (list,set,tuple)):
            if any (map (lambda x: not isinstance(x, str), lst)):
                msg.error ('you have to provide a list of sg-keys !')
                msg.error ('got: %s', lst)
                raise ValueError()
        items = []
        for i in lst:
            if isinstance(i, str) and i.count('#')==1:
                item = i.split('#')
                if item[1] == '*':
                    err = 'sorry, wildcards for keys are NOT (yet?) supported'
                    msg.error(err)
                    raise ValueError(err)
                items.append(item)
            elif (isinstance(i, tuple) and len(i)==2 and 
                  isinstance(i[0], str) and 
                  isinstance(i[1], str)):
                # (type, key)
                items.append(i)
            elif (isinstance(i, tuple) and len(i)==1 and 
                  isinstance(i[0], str)):
                # (type, key)
                items.append ((None, i))
            else:
                err = 'invalid item format: %s'%i
                msg.error(err)
                raise ValueError(err)
    return items

class PyWriter (PyAthena.Alg):
    'algorithm to record a (dummy) container into storegate'
    def __init__(self, name='PyWriter', **kw):
        ## init base class
        kw['name'] = name
        super(PyWriter, self).__init__(**kw)

        ## properties and data members
        self.cont_type = kw.get('cont_type', '')
        self.cont_name = kw.get('cont_name', '')

        ## handle to event store
        self.sg = None
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('could not retrieve StoreGateSvc')
            return StatusCode.Failure

        if self.cont_type is None or \
           self.cont_type == '':
            self.msg.error('you have to provide a container type to record !')
            return StatusCode.Failure
            
        if self.cont_name is None or \
           self.cont_name == '':
            self.cont_name = _gen_key_from_type(self.cont_type)
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('==> execute...')
        try:
            k  = self.cont_name
            tp = self.cont_type
            _info('creating a C++ container of type [%s]...', tp)
            cont = getattr(PyAthena, tp)()
            _info('size= %s', len(cont))
            _info('recording in event store under [%s]...', k)
            if not self.sg.record(cont, k) == StatusCode.Success:
                self.msg.error('could not record container !')
                return StatusCode.Failure
            _info('all good')
        except Exception,err:
            self.msg.error('caught: %s',err)
        return StatusCode.Success
            

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

    pass # class PyWriter

class PyReader (PyAthena.Alg):
    'algorithm to retrieve a (dummy) container from storegate'
    def __init__(self, name='PyReader', **kw):
        ## init base class
        kw['name'] = name
        super(PyReader, self).__init__(**kw)

        ## properties and data members
        self.cont_type = kw.get('cont_type', '')
        self.cont_name = kw.get('cont_name', '')
        self.ofile     = kw.get('ofile',     '')

        ## handle to event store
        self.sg = None
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('could not retrieve StoreGateSvc')
            return StatusCode.Failure

        if self.cont_type is None or \
           self.cont_type == '':
            self.msg.error('you have to provide a container type to retrieve !')
            return StatusCode.Failure
            
        if self.cont_name is None or \
           self.cont_name == '':
            self.cont_name = _gen_key_from_type(self.cont_type)
            pass
        
        if self.ofile is None or self.ofile == '':
            self.ofile = open('pyreader_%s.log'%self.cont_name, 'w')
            pass
        elif isinstance(self.ofile, str):
            self.ofile = open(self.ofile, 'w')
        elif isinstance(self.ofile, file):
            pass
        else:
            self.msg.error("don't know how to handle ofile value/type [%s/%s]!",
                           str(self.ofile), type(self.ofile))
            return StatusCode.Failure
        
        from Dumpers import get_dumper_fct
        try:
            cont_type = getattr(PyAthena, self.cont_type)
        except AttributeError,err:
            self.msg.error(err)
            return StatusCode.Failure
        self.dumper = get_dumper_fct(klass=cont_type, ofile=self.ofile)

        if hasattr(self.ofile, 'name'):
            ofile_name = self.ofile.name
        else:
            ofile_name = '<temporary unnamed file>'
            
        self.msg.info ('dumping container [%s/%s]',
                       self.cont_type,
                       self.cont_name)
        self.msg.info ('             into [%s]', ofile_name)
        
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('==> execute...')
        try:
            k  = self.cont_name
            tp = self.cont_type
            _info('retrieving a C++ container of type [%s] and key [%s]...',
                  tp, k)
            cont = self.sg.retrieve(tp, k)
            if not cont:
                self.msg.error('could not retrieve container !')
                return StatusCode.Failure
            if hasattr (cont, '__len__'):
                _info('size= %s, type=%s', len(cont),cont.__class__.__name__)
            self.dumper(cont)
            self.ofile.flush()
            _info('all good')
        except Exception,err:
            self.msg.error('caught: %s',err)
        return StatusCode.Success
            

    def finalize(self):
        self.msg.info('==> finalize...')
        self.ofile.flush()
        if hasattr(self.ofile,'name'):
            o_name = self.ofile.name
        else:
            o_name = '<unnamed file>'
        import sys
        if not (self.ofile is sys.stdout) and \
           not (self.ofile is sys.stderr) and \
           not (o_name in ['/dev/stdout', '/dev/stderr',]):
            self.ofile.close()
        return StatusCode.Success

    # class PyReader

class PySgDumper (PyAthena.Alg):
    'algorithm to retrieve and dump a list of keys from storegate'
    _ignore_clids = (
        1118613496, # old ParticleJetContainer class
        )
    def __init__(self, name='PySgDumper', **kw):
        ## init base class
        kw['name'] = name
        super(PySgDumper, self).__init__(**kw)

        ## properties and data members
        self.items = kw.get('items', '*')
        self.exclude = kw.get('exclude', '')
        self.ofile = kw.get('ofile', '')

        ## handle to event store
        self.sg = None

        ## list of objects and/or classes for which the dump failed
        ##  ( (key, clid-or-class, 'reason'), ... )
        self.failed_dumps = set()
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('could not retrieve StoreGateSvc')
            return StatusCode.Failure

        self.clidsvc = PyAthena.py_svc ('ClassIDSvc')
        if not self.clidsvc:
            self.msg.error ('could not retrieve ClassIDSvc')
            return StatusCode.Failure

        try:
            self.items = _decode_item_list(self.items, self.msg)
        except Exception,err:
            self.msg.error(err)
            return StatusCode.Failure
            
        if self.ofile is None or self.ofile == '':
            from time import time
            self.ofile = open('pysgdump_%s.log'%int(time()*100), 'w')
            pass
        elif isinstance(self.ofile, str):
            self.ofile = open(self.ofile, 'w')
        elif isinstance(self.ofile, file):
            pass
        else:
            self.msg.error("don't know how to handle ofile value/type [%s/%s]!",
                           str(self.ofile), type(self.ofile))
            return StatusCode.Failure
        
        from Dumpers import get_dumper_fct
        self._dumper_fct = get_dumper_fct

        if hasattr(self.ofile, 'name'):
            ofile_name = self.ofile.name
        else:
            ofile_name = '<temporary unnamed file>'
            
        self.msg.info ('dumping containers %s',
                       '<ALL>' if self.items is None else self.items)
        self.msg.info ('into [%s]', ofile_name)

        self._evt_nbr = 0
        return StatusCode.Success

    def execute(self):
        _debug= self.msg.debug
        _info = self.msg.info
        _warn = self.msg.warning
        _add_fail = self.failed_dumps.add
        _info('==> processing event [%s]...', self._evt_nbr)
        self.ofile.writelines (['%s\n'%('='*40),
                                '==> evt nbr [%s]\n' % self._evt_nbr])
        self._evt_nbr += 1

        if self.items is None:
            elist = self.exclude.split(',')
            proxies = self.sg.proxies()
            sg = dict()
            _typename = self.clidsvc.typename
            for p in proxies:
                clid = p.clID()
                if clid in self._ignore_clids:
                    continue
                n  = str(p.name())
                tp = _typename(clid)
                if n.endswith('Aux.'): continue
                excluded = False
                for exc in elist:
                    if fnmatch (n, exc) or (tp and fnmatch(tp, exc)):
                        excluded = True
                        break
                if excluded:
                    continue
                if tp:
                    sg[n] = tp
                else:
                    self.msg.warning ('no typename for key=[%s], clid=[%i]',
                                      n, clid)
                    _add_fail ((n,clid,'no typename from clid'))
                    
            items = [(i[1], i[0]) for i in sg.iteritems() if i[1] != "EventInfo"]
            items.sort()
            evt_info = [(clid, key) for key,clid in sg.iteritems() if clid=="EventInfo"]
            if len(evt_info)==1:
                items.insert (0, evt_info[0])
        else:
            items = self.items
            
        _retrieve = self.sg.retrieve
        _get = self.sg.__getitem__
        for cont_type, cont_key in items:
            _debug ('dumping object [%s#%s]', cont_type, cont_key)
            try:
                if cont_type is None:
                    o = _get (cont_key)
                else:
                    o = _retrieve (cont_type, cont_key)
                if not o:
                    _add_fail ((cont_key, cont_type, 'retrieve failed'))
                    if self._evt_nbr==1:
                        _warn ('could not retrieve object [%s#%s]',
                               cont_type, cont_key)
                    continue
                try:
                    dumper = self._dumper_fct (klass=o.__class__,
                                               ofile=self.ofile)
                except RuntimeError, err:
                    _add_fail ((cont_key, cont_type, 'dump failed'))
                    if self._evt_nbr==1:
                        _warn (err)
                    continue
                self.ofile.writelines (
                    ['%s%s[%s/%s]%s\n'%(
                        os.linesep,
                        '-'*10,
                        o.__class__.__name__,
                        cont_key,
                        '-'*10)
                     ]
                    )
                self.ofile.flush()
                dumper (o)
                self.ofile.flush()
            except Exception, err:
                _add_fail ((cont_key, cont_type, 'sg-retrieve failed'))
                if self._evt_nbr==1:
                    _warn ('caught exception:\n%s', err)
                    _warn ('could not retrieve object [%s#%s]',
                           cont_type, cont_key)
                continue
        return StatusCode.Success
            

    def finalize(self):
        self.msg.info ('==> finalize...')
        self.msg.info ('processed [%s] events', self._evt_nbr)
        
        self.ofile.flush()
        if hasattr(self.ofile,'name'):
            o_name = self.ofile.name
        else:
            o_name = '<unnamed file>'
        import sys
        if not (self.ofile is sys.stdout) and \
           not (self.ofile is sys.stderr) and \
           not (o_name in ['/dev/stdout', '/dev/stderr',]):
            self.ofile.close()

        _info = self.msg.info
        if len(self.failed_dumps)>0:
            _info ("the following objects could not be dumped:")
            from collections import defaultdict
            reasons = defaultdict(list)
            for name,klass,reason in self.failed_dumps:
                reasons[reason].append ((name,klass))
            for reason in reasons.iterkeys():
                _info (' ==> [%s]', reason)
                for name,klass in reasons[reason]:
                    _info ("     [%s#%s]", klass, name)
        return StatusCode.Success

    # class PySgDumper

class DataProxyLoader(PyAthena.Alg):
    'algorithm to load a list of objects via their DataProxy from storegate'

    def __init__(self, name='DataProxyLoader', **kw):
        ## init base class
        kw['name'] = name
        super(DataProxyLoader, self).__init__(**kw)

        ## properties and data members
        self.items = kw.get('items', '*')

        ## handle to event store
        self.sg = None

        ## list of objects and/or classes for which the dump failed
        ##  ( (key, clid-or-class, 'reason'), ... )
        self.failed_dumps = set()
        return

    def initialize(self):

        self.msg.info('==> initialize...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('could not retrieve StoreGateSvc')
            return StatusCode.Failure

        try:
            self.items = _decode_item_list(self.items, self.msg)
        except Exception,err:
            self.msg.error(err)
            return StatusCode.Failure
            
        self.msg.info ('dumping containers %s',
                       '<ALL>' if self.items is None else self.items)
        self._evt_nbr = 0
        return StatusCode.Success

    def execute(self):
        _debug= self.msg.debug
        _info = self.msg.info
        _warn = self.msg.warning
        _fatal= self.msg.fatal
        _add_fail = self.failed_dumps.add
        _info('==> processing event [%s]...', self._evt_nbr)
        self._evt_nbr += 1
        if self.items is None:
            # take all from storegate
            proxies = list(self.sg.proxies())
        else:
            proxies = []
            all_proxies = list(self.sg.proxies())
            for i in self.items:
                i_clid = i[0]
                i_name = i[1]
                for p in all_proxies:
                    if (i_clid == p.clID() and
                        i_name == p.name()):
                        proxies.append(p)
                        continue
        all_good = True
        for p in proxies:
            clid = p.clID()
            sgkey= p.name()
            _debug('loading proxy [%s#%s]...', clid, sgkey)
            try:
                dobj = p.accessData()
                if not dobj:
                    all_good = False
            except Exception, err:
                _fatal('problem loading proxy [%s#%s]', clid, sgkey)
                _add_fail((sgkey, clid, str(err)))
                all_good = False
                
        if all_good:
            return StatusCode.Success
        return StatusCode.Failure
    
    def finalize(self):
        _info = self.msg.info
        _info ('==> finalize...')
        _info ('processed [%s] events', self._evt_nbr)
        if len(self.failed_dumps)>0:
            _info ("the following objects could not be dumped:")
            from collections import defaultdict
            reasons = defaultdict(list)
            for name,klass,reason in self.failed_dumps:
                reasons[reason].append ((name,klass))
            for reason in reasons.iterkeys():
                _info (' ==> [%s]', reason)
                for name,klass in reasons[reason]:
                    _info ("     [%s#%s]", klass, name)
        return StatusCode.Success
        
