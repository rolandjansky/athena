# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PerfMonComps/python/PerfMonSerializer

from __future__ import with_statement

__version__ = "$Revision: 524466 $"
__doc__ = "various utils to encode/decode perfmon (meta)data with base64"
__author__ = "Sebastien Binet, Thomas Kittlemann"

### functions ------------------------------------------------------------------
def extract_pmon_files(fname):
    """
    return a dict of string->file-like objects containing the
    perfmon data and metadata
    """
    if not fname.endswith('pmon.gz'):
        raise ValueError("expect a xyz.pmon.gz file (got [%s])"%(fname,))
    
    import os.path, sys
    import tarfile
    import tempfile
    tmpdir = tempfile.mkdtemp(prefix='pmon_ser_')

    import atexit, shutil
    def cleanup_pmon_ser(tmpdir=tmpdir):
        try:
            if os.path.exists(tmpdir):
                shutil.rmtree(tmpdir)
        except Exception,err:
            print >> sys.stderr, "pmon.cleanup_pmon_ser: **warning**:\n%s" % (
                err,
                )
    atexit.register(cleanup_pmon_ser)
            
    tar = tarfile.open(fname, 'r')
    tar.extractall(path=tmpdir)
    tar.close()

    import glob
    files = glob.glob(os.path.join(tmpdir,'*.stream'))
    stream_fname = files[0]

    files = glob.glob(os.path.join(tmpdir,"*.dat"))
    dat_fname = files[0]

    files = glob.glob(os.path.join(tmpdir,"*.pmonsd.txt"))
    semidet_fname = files[0] if files else None

    return {
        'infos': open(dat_fname),
        'data':  open(stream_fname),
        'semidet' : open(semidet_fname) if semidet_fname else None
        }

def extract_pmon_infos(fname, only_summary=False):
    """ extract the perfmon informations from ``fname``
    if ``fname`` ends with '.pmon.dat', assume it is a shelve.
    otherwise, assumes it's a tar-ball with a .pmon.dat shelve inside.

    returns None in case of errors
    """

    def dict_from_shelve(fname):
        db = {}
        if open(fname, 'r').read(1024).startswith('SQLite format'):
            import PyUtils.dbsqlite as dbs
            db = dbs.open(fname, 'r')
        else:
            import shelve
            try:
                db = shelve.open(fname)
            except Exception:
                pass
            pass
        out=dict(db)
        if not out:
            return None

        if only_summary:
            if 'perf_summary' in out.keys():
                return {'perf_summary':out['perf_summary']}
            else:
                return None
        return out

    if fname.endswith('pmon.dat'):
        return dict_from_shelve(fname)

    o = extract_pmon_files(fname)
    infos_file = o['infos']
    out = dict_from_shelve(infos_file.name)
    return out

def _init_pmon_data():
    from collections import defaultdict
    out = defaultdict(dict)

    import numpy as np
    iocpu_dtype = np.dtype([
        ('user','float32'),
        ('sys', 'float32'),
        ('real','float32'),
        ('cpu', 'float32'),
        ('rt',  'float32'),
        ])
    cpu_dtype = np.dtype([
        ('user','3float32'),
        ('sys', '3float32'),
        ('real','3float32'),
        ('cpu', '3float32'),
        ('rt',  '3float32'),
        ])
    mem_dtype = np.dtype([
        ('vmem', '3float32'),
        ('rss',  '3float32'),
        ('mall', '3float32'),
        ('nmall', '3int64'),
        ('nfree', '3int64'),
        ])
    comp_dtype=np.dtype([('cpu', cpu_dtype), ('mem', mem_dtype)])

    def new_component():
        return np.zeros(0, dtype=comp_dtype)

    iocomp_dtype = np.dtype([('r',  iocpu_dtype),
                             ('rr', iocpu_dtype),
                             ('w',  iocpu_dtype),])
    def new_iocomp():
        return np.zeros(0, dtype=iocomp_dtype)

    for step in ('ini', 'evt', 'fin', 'cbk', 'usr',
                 'preLoadProxy', 'dso',):
        out[step] = defaultdict(new_component)
    for step in ('io',):
        out[step] = defaultdict(new_iocomp)
        
    return out

def iextract_pmon_data(fname):
    """
    iterates over the file pointed at by `fname` and yields a tuple of
     (step, idx, comp, out)
    where:
      - `step` is in ('ini','evt','fin','cbk','usr','dso',...)
      - `idx` is 0 or 1 (resp. start or stop)
      - `comp` is the name of the entity being monitored (alg,library,cbk,...)
      - `out` is the up-to-date table of data collected so far

    the table is a dict of {'step':{'comp':numpy.array}}
    
    this table is the suitable python object for further analyses
    from a pmon file
    """
    if fname.endswith('.pmon.gz'):
        o = extract_pmon_files(fname)
        stream_file = o['data']
        stream_fname = stream_file.name
    elif fname.endswith('.stream'):
        stream_file = open(fname)
        stream_fname= fname
    else:
        raise ValueError("expect a xyz.pmon.gz or xyz.stream file (got [%s])"%(fname,))
    
    from collections import defaultdict
    import numpy as np
    out = _init_pmon_data()
        
    with open(stream_fname, 'r') as f:
        for l in f:
            data, step, idx, comp = (None, ) * 4
            if l.startswith('#'):
                continue
            #print "[%s]" % l.strip()
            # handle things like:
            # /io/std::vector<unsigned int>#L1CaloUnpackingErrors ...
            # /io/std::map<std::string,std::vector<int> >#mapdata ...
            l = l.replace('unsigned int', 'unsigned-int')\
                 .replace('> >', '>->')
            
            fields = l.split()
            #print "##",repr(l)
            if fields[0].startswith(('/ini/','/evt/','/fin/',
                                     '/cbk/','/usr/',
                                     '/preLoadProxy/',
                                     )):
                # -- handle bug #68142 (components with spaces...)
                # peek at field[1] and check it is an integer
                # otherwise, merge field[0] = field[0]+field[1] and shift
                # everything accordingly
                # FIXME: this doesn't handle all possible oddly-named comps
                #        but it should be good enough.
                while 1:
                    try:
                        idx = int(fields[1])
                    except ValueError:
                        fields[0] = fields[0]+fields[1]
                        del fields[1]
                    else:
                        break
                # --
                
                # should be of the form "/step/comp-name"
                step = fields[0][1:-1]
                step = step.split('/')[0]
                comp = fields[0][len(step)+2:]
                if step == 'cbk':
                    # remove absolute address from cbk
                    # should be like so:
                    # /cbk/PixelDCSSvc[0x1045f9d8]+15
                    # =>
                    # /cbk/PixelDCSSvc+15
                    # /cbk/(component)+(offset)
                    offset = comp.split(']+')[1]
                    bracket_idx = comp.find('[')
                    if bracket_idx > 0:
                        comp = '%s{+%s}' % (comp[:bracket_idx],offset)
                     
                idx = int(fields[1])

                data = out[step][comp]
                    
                if idx == 0:
                    if len(data) == 0:
                        data = np.zeros(1, data.dtype)
                    data = out[step][comp] = np.append(out[step][comp],
                                                       data[-1])
                # access last entry
                data = data[-1]
                
                cpu = data['cpu']
                cpu['user'][idx] = float(fields[2])
                cpu['sys'][idx]  = float(fields[3])
                cpu['real'][idx] = float(fields[4])
                cpu['rt'][idx]  = float(fields[5])
                cpu['cpu'][idx] = cpu['user'][idx] + cpu['sys'][idx]

                mem = data['mem']
                mem['vmem'][idx] = float(fields[6])/1024. # cnv to Mb
                mem['rss'][idx]  = float(fields[7])/1024.
                mem['mall'][idx] = float(fields[8])/1024.
                mem['nmall'][idx]= float(fields[9])
                mem['nfree'][idx]= float(fields[10])
                #print "==> [%s][%s][%s]" % (step, comp, idx), data, idx

                if idx == 1:
                    d = cpu
                    for n in d.dtype.names:
                        d[n][2] = d[n][1]
                        d[n][1] = d[n][0] + d[n][1]
                    d = mem
                    for n in d.dtype.names:
                        d[n][2] = d[n][1] - d[n][0]
                    pass
                
            elif fields[0].startswith('/io/'):
                step = "io"
                comp = fields[0][len(step)+2:]
                data = out[step][comp]
                if len(data) == 0:
                    data = np.zeros(1, data.dtype)
                    pass
                data = out[step][comp] = np.append(out[step][comp],
                                                   data[-1])
                # access last entry
                data = data[-1]
                r = data['r']
                r['user'] = float(fields[1])
                r['sys']  = float(fields[2])
                r['real'] = float(fields[3])
                r['cpu']  = r['user'] + r['sys']
                r['rt']   = float(fields[4])

                rr = data['rr']
                rr['user'] = float(fields[5])
                rr['sys']  = float(fields[6])
                rr['real'] = float(fields[7])
                rr['cpu']  = rr['user'] + rr['sys']
                rr['rt']   = float(fields[8])

                w = data['w']
                w['user'] = float(fields[9])
                w['sys']  = float(fields[10])
                w['real'] = float(fields[11])
                w['cpu']  = w['user'] + w['sys']
                w['rt']   = float(fields[12])

                #print "--> [%s][%s]" % (step, comp), data
                
            elif fields[0].startswith('/dso/'):
                step = "dso"
                comp = fields[0][len(step)+2:]
                data = out[step][comp]
                    
                idx  = int(fields[1])
                data = out[step][comp]
                    
                if idx == 0:
                    if len(data) == 0:
                        data = np.zeros(1, data.dtype)
                    data = out[step][comp] = np.append(out[step][comp],
                                                       data[-1])
                # access last entry
                data = data[-1]

                if len(fields) >= 10:
                    cpu = data['cpu']
                    cpu['user'][idx] = float(fields[2])
                    cpu['sys'][idx]  = float(fields[3])
                    cpu['real'][idx] = float(fields[4])
                    cpu['rt'][idx]  = float(fields[5])
                    cpu['cpu'][idx] = cpu['user'][idx] + cpu['sys'][idx]

                    mem = data['mem']
                    mem['vmem'][idx] = float(fields[6])/1024. # cnv to Mb
                    mem['rss'][idx]  = float(fields[7])/1024.
                    mem['mall'][idx] = float(fields[8])/1024.
                    mem['nmall'][idx]= float(fields[9])
                    mem['nfree'][idx]= float(fields[10])

                else: # old format
                    cpu = data['cpu']
                    cpu['user'][idx] = 0.
                    cpu['sys'][idx]  = 0.
                    cpu['real'][idx] = 0.
                    cpu['rt'][idx]  = 0.
                    cpu['cpu'][idx] = 0.

                    mem = data['mem']
                    mem['vmem'][idx] = float(fields[2])/1024. # cnv to Mb
                    mem['rss'][idx]  = 0.
                    mem['mall'][idx] = 0.
                    mem['nmall'][idx]= 0.
                    mem['nfree'][idx]= 0.
                #print "==> [%s][%s][%s]" % (step, comp, idx), data, idx

                if idx == 1:
                    d = cpu
                    for n in d.dtype.names:
                        d[n][2] = d[n][1]
                        d[n][1] = d[n][0] + d[n][1]
                    d = mem
                    for n in d.dtype.names:
                        d[n][2] = d[n][1] - d[n][0]
                    pass
                pass
            else:
                print "warning: unhandled field [%s]" % (fields[0],)
                print repr(l)

            # yields what we got so far
            yield step, idx, comp, out

            pass # dispatch on fields
        pass # stream_data
    
def extract_pmon_data(fname):
    """
    return a suitable python object for further analyses from a pmon file
    """
    out = None
    for step, idx, comp, table in iextract_pmon_data(fname):
        out = table
        pass

    # to prevent unintended use...
    for k in out:
        out[k] = dict(out[k])
    return out

def pmon_load(fname):
    """ load all the pmon data from a .pmon.gz file ``fname`` and return
    the tuple (infos, data)
    """
    infos = extract_pmon_infos(fname)
    data  = extract_pmon_data(fname)
    return (infos, data)
    
def encode(data, use_base64=True):
    """encode some data into a (compressed) string, using base64 (or not)
    """
    # Tests show that pickle(protocol==1)+zlib(level=6) gives the best results:
    import zlib
    import cPickle as pickle
    s=zlib.compress(pickle.dumps(data,1),6)
    if use_base64:
        import base64
        return 'B'+base64.b64encode(s)
    else:
        return 'R'+s

def decode(s):
    """decode a (compressed) string into a python object
    """
    if not s: return None
    import zlib
    import cPickle as pickle
    if s[0]=='B':
        import base64
        s=base64.b64decode(s[1:])
    else:
        s=s[1:]
    d=pickle.loads(zlib.decompress(s))
    return pickle.loads(zlib.decompress(s))

def build_callgraph(fname):
    """ take a `fname` pmon file, iterate over its data and return a callgraph
    for each of the 'ini', 'evt' and 'fin' steps.
    all over pmon steps are folded into the 3 above ones.
    each event during the 'evt' step has its own callgraph structure
    """
    graph = {
        'ini': [],
        'evt': [],
        'fin': [],
        }

    current_step = 'ini'
    local_ctx = None
    
    out = None
    for step, idx, comp, table in iextract_pmon_data(fname):
        out = table
        if idx is None:
            if comp == 'PerfMonSliceIo':
                # ignore this component for now...
                continue
            elif step == 'io':
                # ignore io-step for now...
                continue
            else:
                raise RuntimeError(
                    'logic error step=[%s], idx=[%s], comp=[%s]' %
                    (step, idx, comp,)
                    )
            
        if comp in ('PerfMonSvc', 'AuditorSvc'):
            # ignore this component...
            continue

        ## print "::: cur [%s] ctx=%s idx=[%s]... [%s%s]" % (
        ##     current_step, local_ctx, idx, '->' if idx==0 else '<-',comp,
        ##     )
        if step != current_step and step in graph.keys():
            # new step...
            if current_step == 'ini' and step == 'evt':
                # transition ini -> first evt
                current_step = 'evt'
            elif current_step == 'evt' and step == 'fin':
                # transition evt -> fin
                current_step = 'fin'
                # new context
                ## print "-"*40, "[NEW CONTEXT]"
                local_ctx = GraphNode('PerfMonSlice')
                graph[current_step].append(local_ctx)

            elif current_step == 'evt' and step == 'ini':
                # late initialization...
                # fold into 'evt': do not open a new context
                pass
            else:
                raise RuntimeError(
                    'unknown transition [%s -> %s]' % (current_step, step)
                    )

        if comp == 'PerfMonSlice':
            if idx == 0:
                if current_step != 'fin':
                    # new context
                    ## print "-"*40, "[NEW CONTEXT]"
                    local_ctx = GraphNode(comp)
                    graph[current_step].append(local_ctx)
                else:
                    # handled by the state transition stuff above.
                    # this is b/c PerfMonSlice isn't triggered at the real
                    # step boundaries, at least for finalize.
                    # the proper way to address this issue would be to
                    # have our own ApplicationMgr firing the right incidents
                    # and/or custom auditor events...
                    pass
            elif idx == 1:
                local_ctx.data_idx = (step, comp, idx)
                local_ctx.data = table[step][comp][-1]
                # close context
                ## print "==>",local_ctx
                ## print "-"*40, "[DEL CONTEXT] (%s)" % current_step
                if current_step == 'fin':
                    return graph
                pass
            else:
                raise RuntimeError('unknown index [%s]' % (idx,))
        else:
            if idx == 0:
                # push the stack of contexes
                parent_ctx = local_ctx
                local_ctx = GraphNode(comp, parent=parent_ctx)
                if not step in graph.keys():
                    local_ctx.ctype = step
                parent_ctx.children.append(local_ctx)
            elif idx == 1:
                if comp != local_ctx.name:
                    raise RuntimeError(
                        'closing context [%s] within scope of [%s]' %
                        (comp, local_ctx.name)
                        )
                assert local_ctx.parent, "unrooted local context [%r]" % (
                    local_ctx,)
                local_ctx.data_idx = (step, comp, idx)
                local_ctx.data = table[step][comp][-1]
                # pop the stack of contexes
                local_ctx = local_ctx.parent
            else:
                raise RuntimeError('unknown index [%s]' % (idx,))
            pass
        
        pass
    
    return graph

def iter_callgraph(node):
    """helper function to iterate over a bunch of GraphNodes
       @return a tuple (node, nest-level)
    """
    def _igraph(node, indent=0):
        yield node,indent
        for c in node.children:
            for cc in _igraph(c,indent+1):
                yield cc
    return _igraph(node)

def callgraph_node_get_data(node, op_name='self'):
    """return the monitoring data attached to a node of the callgraph
    ``op_name`` can be: 'self' or 'incl'
    """
    assert op_name in ('self', 'incl'),\
           'op_name must be either \'self\' or \'incl\''
    
    data = None
    op = getattr(node, op_name)
    if node.ctype in ('comp','dso',
                      'usr','preLoadProxy','cbk'):
        cpu_usr = op('cpu','user',2)
        cpu_sys = op('cpu','sys', 2)
        cpu_real= op('cpu','real',2)
        cpu_cpu = op('cpu','cpu', 2)
        mem_vmem= op('mem','vmem',2)
        mem_rss=  op('mem','rss', 2)
        mem_mall= op('mem','mall',2)
        mem_nmall=op('mem','nmall',2)
        mem_nfree=op('mem','nfree',2)
        data= (cpu_cpu, mem_vmem, cpu_usr, cpu_sys, cpu_real, 
               mem_rss, mem_mall, mem_nmall, mem_nfree)
    else:
        raise RuntimeError('unknown node type [%s]' % node.ctype)
    return ' '.join(map(str,map(int,data)))
    
def cnv_callgraph_to_cachegrind(root, oname, metadata=None):
    f = open(oname, 'w+b')
    write = f.write

    def _get_data(node, op_name='self'):
        try:
            return callgraph_node_get_data(node, op_name)
        except Exception,err:
            print "."*80
            print "node=%r" % node
            print err
            raise
        
    if metadata:
        def _get_meta(node):
            if node.ctype in ('dso','usr','preLoadProxy','cbk'):
                return node.ctype
            try:
                return metadata[node.name]
            except KeyError:
                ## print "**err** domaindb[%s] does not exist !! ==> [%s]" % (
                ##     node.name, metadata.get(node.parent.name,'N/A'))
                # fold everything into parent, if any
                if node.parent:
                    return _get_meta(node.parent)
                return node.ctype
    else:
        def _get_meta(node):
            return node.ctype
        
    write("events: cpu_cpu mem_vmem cpu_user cpu_sys cpu_real "
          "mem_rss mem_malloc mem_nallocs mem_nfrees\n")
    import collections
    import itertools
    # a dict of component-name => monotonic-counter
    # to fake line numbers
    line_nbr = collections.defaultdict(itertools.count().next)
    for idx, (node, indent) in enumerate(iter_callgraph(root)):
        write(
            "\n"
            "fl=%s.cxx\n"
            "ob=%s.so\n"
            "fn=%s\n"
            "%i %s\n" % (node.ctype,
                         _get_meta(node),
                         node.name,
                         line_nbr[node.name],
                              _get_data(node, op_name='self'),)
            )
        for c in node.children:
            write(
                "cfl=%s.cxx\n"
                "cob=%s.so\n"
                "cfn=%s\n"
                "calls=%i %i\n%i %s\n" % (
                    c.ctype,
                    _get_meta(c),
                    c.name,
                    1, line_nbr[c.name], line_nbr[node.name],
                    _get_data(c, op_name='incl')
                    )
                )
    f.flush()
    f.close()
    return

def create_cachegrind_files(fname='ntuple.pmon.gz', domainsdb=None):
    """take (the path to) a pmon file name `fname` and create a bunch of
    cachegrind files:
     - one for the 'ini' stage
     - one for each event
     - one for the 'fin' stage
    the cachegrind files will be named like so:
     <fname>.ini.cgc.out
     <fname>.evt.xxxx.cgc.out
     <fname>.fin.cgc.out
    """
    # extract graph(s) from perfmon data
    root_graph = build_callgraph(fname)
    # get some metadata to help clustering the callgraph
    if domainsdb is None:
        domainsdb = extract_pmon_infos(fname)['domains_a2d']
    for n in ('PerfMonSlice', 'AthMasterSeq',
              'AthAlgSeq',
              'AthOutSeq',
              'AthRegSeq',):
        domainsdb[n] = 'admin'
    #
    tmpl_name = fname.replace('.pmon.gz', '.cgc.out')
    cnv_callgraph_to_cachegrind(root_graph['ini'][-1],
                                tmpl_name.replace('.cgc', '.ini.cgc'),
                                domainsdb)
    for ievt, graph in enumerate(root_graph['evt']):
        cnv_callgraph_to_cachegrind(
            graph,
            tmpl_name.replace('.cgc.out',
                              '.evt.%s.cgc.out' % str(ievt).zfill(4)),
            domainsdb
            )
    cnv_callgraph_to_cachegrind(root_graph['fin'][-1],
                                tmpl_name.replace('.cgc.out', '.fin.cgc.out'),
                                domainsdb)
    return
    
### classes -------------------------------------------------------------------
class GraphNode(object):
    _sentinel = object()
    def __init__(self, name, parent=None, children=None,
                 comp_type='comp',
                 stage=None):
        self.name = name
        self.parent= parent
        self.children= []
        if children:
            self.children = list(children)[:]
        self.ctype = comp_type

    def __repr__(self):
        parent_name = '<root>'
        if self.parent:
            parent_name = self.parent.name
        return '<GraphNode[%s] p=[%s] c=[%s] type=[%s]>' % (
            self.name,
            parent_name,
            len(self.children),
            self.ctype,
            )

    def _get_data(self, *idx):
        data = self.data
        for i in idx:
            try:
                data = data[i]
            except IndexError:
                return self._sentinel
        return data
    
    def incl(self, *idx):
        var = self._get_data(*idx)
        if var is self._sentinel:
            raise IndexError('no such index %r' % (tuple(idx),))
        return var

    def excl(self, *idx):
        tot = 0.
        for c in self.children:
            try:
                tot += c.incl(*idx)
            except IndexError:
                pass
        return tot

    def self(self, *idx):
        incl = self.incl(*idx)
        excl = self.excl(*idx)
        return incl - excl

    def ies(self, *idx):
        return {
            'incl': self.incl(*idx),
            'excl': self.excl(*idx),
            'self': self.self(*idx),
            }

    def has_index(self, *idx):
        return not (self._get_data(*idx) is self._sentinel)

    def __getitem__(self, idx):
        return self.children[idx]
    
    pass # class GraphNode
                                                  
        
### tests ---------------------------------------------------------------------
if __name__=='__main__':
    import sys
    import os.path

    if len(sys.argv)<2:
        print "Please supply one or more perfmon files to investigate (*.pmon, *.pmon.gz or *.pmon.dat)"
        exit(1)
    for arg in sys.argv[1:]:
        print "Investigating file",arg
        if not os.path.exists(arg):
            print "  ==> Not found!"
            continue
        print "::: keys:",extract_pmon_infos(fname=arg).keys()
        for only_summary in [True,False]:
            infodict=extract_pmon_infos(fname=arg,only_summary=only_summary)
            for use_base64 in [False,True]:
                s_info="summary" if only_summary else 'full'
                s_enc="base64" if use_base64 else 'binary'
                p = encode(data=infodict,use_base64=use_base64)
                s='  ==> Reading '+s_info+' info with '+s_enc+' encoding: <'+(str(len(p))+' bytes'if p else 'failed')+'>'
                if p:
                    decoded = decode(p)
                    decodeOk = (decoded==infodict)
                    s+=' [Decoding '+('OK' if decodeOk else 'FAILED')+']'
                print s




"""
import PerfMonComps.PerfMonSerializer as pmon_ser
infos,data = pmon_ser.pmon_load('foo.pmon.gz')
ddb = infos['domains_a2d']
root_graph = pmon_ser.build_callgraph('foo.pmon.gz')
evt = root_graph['evt'][-1]
domains_stats = collections.defaultdict(float)
for c in evt[0][0].children:
 #print c.name
 if c.name in ddb:
  domain = ddb[c.name]
  domains_stats[domain] += c.incl('cpu','cpu',2)
 else:
  print '**err**',c.name

print dict(domains_stats)
{'admin': 0.0,         
 'aod': 4.0,           
 'calo': 889.0,        
 'egamma': 1703.0,     
 'id': 2777.0,         
 'jet': 3228.0,        
 'muon': 202.0,        
 'muoncomb': 720.0,    
 'output': 562.0,      
 'tau': 482.0}         
"""
