# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PyPerfMon.py
# @author: Sebastien Binet <binet@cern.ch>
from __future__ import with_statement

__author__  = "Sebastien Binet <binet@cern.ch>"
__version__ = "$Revision: 1.51 $"
__doc__     = """python module holding a python service to monitor athena performances (memory,cpu,I/O,persistency)
"""

import os,sys
from time import time
import resource
from resource import getrusage as resource_getrusage
import string

import array
import AthenaCommon.Logging as L

_perfMonStates = ('ini','evt','fin')

from PyMonUtils import *

from PyUtils.Decorators import memoize, forking

@memoize
def _import_ROOT():
    # FIXME: work-around ROOT's silly behaviour wrt graphics libraries
    # see: https://savannah.cern.ch/bugs/?35461
    import os
    orig_display=os.environ.get('DISPLAY',None)
    import ROOT
    if orig_display:
        os.environ['DISPLAY'] = orig_display
    del orig_display
    return ROOT

def retrieve_pmon_svc(fname):
    import PyUtils.dbsqlite as dbs
    db = dbs.open(fname, 'r')
    svc = db['pmon']
    db.close()
    import os
    os.remove(fname)
    return svc

class Svc(object):
    """
    Performance monitoring service
    """
    instances = {}

    def __init__(self, name, properties = None):
        import AthenaCommon.Logging as L
        ## init base class
        super(Svc,self).__init__()
        self.name  = name
        self.meta  = {}
        self.tools = {}
        self.stats = {}     # hold various statistics for ini/evt/fin
        # hold data for computing naive vmem-leak slope
        self._slope_data = None
        self.usr_infos = {} # hold all 'declareInfo' variables
                            # { owner : [ [name, var, descr] ] }
        self.outFileName  = properties['outFileName' ]
        self.components   = [ i for i in set(properties['components']+
                                                       ["PerfMonSlice"]) ]
        self.ioContainers = [ i for i in set(properties['ioContainers']) ]

        pflags = self._pmon_flags = dict(properties['pmon_flags'])

        if pflags['doFastMon']:
            self.components   = ["PerfMonSlice"]
            self.ioContainers = []
            pass
        self._doPushBackCheck = pflags['doPushBackCheck']
        self._do_post_processing = pflags['doPostProcessing']
        self._do_malloc_mon = bool(pflags['doMallocMonitoring'])
        
        from AthenaCommon.Configurable import (Configurable,
                                               ConfigurableAlgorithm,
                                               ConfigurableAlgTool,
                                               ConfigurableService)
        cfgs = Configurable.allConfigurables
        d = {}
        for c in self.components:
            cfg_type = cfg_class = cfg_module = None
            if c == 'PerfMonSlice':
                cfg_type   = 'summary'
                cfg_class  = 'PyPerfMonSvc'
                cfg_module = 'PerfMonComps'
            elif cfgs.has_key(c):
                cfg = cfgs[c]
                if isinstance(cfg, ConfigurableAlgorithm): cfg_type = 'alg'
                elif isinstance(cfg, ConfigurableAlgTool): cfg_type = 'algtool'
                elif isinstance(cfg, ConfigurableService): cfg_type = 'svc'
                cfg_class  = cfg.__class__.__name__
                cfg_module = cfg.__class__.__module__
            else:
                cfg_type = '*'
                pass
            d[c] = { 'type'  : cfg_type,
                     'class' : cfg_class,
                     'module': cfg_module,
                     }
        self.meta['components'  ] = d
        self.meta['iocontainers'] = self.ioContainers

        if self.outFileName.endswith(".root"):
            self.outFileName = os.path.splitext(self.outFileName)[0]+".pmon.gz"
        else:
            self.outFileName = self.outFileName+".pmon.gz"
            
        ## for child tools
        self._configurable_parent = cfgs['PerfMonSvc']
        _msg = self.msg
        if hasattr(cfgs['PerfMonSvc'], 'OutputLevel'):
            _msg.setLevel(getattr(cfgs['PerfMonSvc'], 'OutputLevel'))
        else:
            # default is fine with us
            pass

        self.tools = {}
        if not pflags['doFastMon']:
            self.tools['pool'] = PoolMonTool(self)
        import sys
        if pflags['doHephaestusMon']:
            if 'Hephaestus.atexit' in sys.modules.keys():
                self.tools['heph'] = HephaestusMonTool(self)
            else:
                _msg.info('Hephaestus was not correctly initialized !')
                _msg.info('(to fix this, run with --leak-check)')
            pass

        self._configurable_parent = None

        ## perfmon domains
        try:
            import DomainsRegistry as pdr
            self.meta['domains_a2d'] = pdr.a2d_db()
        except Exception:
            _msg.info('problem retrieving domains-registry...')
            self.meta['domains_a2d'] = {}
            
        # initialize stats
        for k in _perfMonStates:
            self.stats[k] = {
                'nentries':  0,
                'cpu':      (0.,0.),
                'cpu_user': (0.,0.),
                'cpu_sys':  (0.,0.),
                'real':     (0.,0.),
                'vmem':     (0.,0.),
                'malloc':   (0.,0.),
                'nalloc':   (0.,0.),
                'rt':       (0.,0.),
                }

        import gc; gc.collect(); del gc
        return

    def domains_db(self):
        try:
            return self.meta['domains_a2d']
        except Exception:
            return {}
    
    @property
    def msg(self):
        import AthenaCommon.Logging as L
        return L.logging.getLogger(self.name)

    def _set_stats(self, name,
                   nentries,
                   cpu_mean, cpu_mean_err,
                   cpu_user_mean, cpu_user_mean_err,
                   cpu_sys_mean, cpu_sys_mean_err,
                   real_mean, real_mean_err,
                   vmem_mean, vmem_mean_err,
                   malloc_mean, malloc_mean_err,
                   nalloc_mean, nalloc_mean_err,
                   rt_mean, rt_mean_err,
                   nevts=-1,
                   first_vmem=0,
                   evt10_vmem=0,
                   last_vmem=0,
                   vmem2to20_n=0,vmem2to20_slope=None,
                   vmem21to100_n=0,vmem21to100_slope=None,
                   vmem101plus_n=0,vmem101plus_slope=None):
        self.stats[name] = {'nentries': nentries,
                            'cpu': (cpu_mean, cpu_mean_err),
                            'cpu_user': (cpu_user_mean, cpu_user_mean_err),
                            'cpu_sys': (cpu_sys_mean, cpu_sys_mean_err),
                            'real': (real_mean, real_mean_err),
                            'vmem': (vmem_mean, vmem_mean_err),
                            'malloc': (malloc_mean, malloc_mean_err),
                            'nalloc': (nalloc_mean, nalloc_mean_err),
                            'rt':     (rt_mean, rt_mean_err)}
        self._slope_data = {'developments': (nevts, first_vmem, evt10_vmem, last_vmem),
                            'fits':[(' 2-20',vmem2to20_n,vmem2to20_slope),
                                    ('21-100',vmem21to100_n,vmem21to100_slope),
                                    ('101+  ',vmem101plus_n,vmem101plus_slope)]}
        
    def install_pmon_dso_logger(self):
        if self._pmon_flags['doDsoMonitoring']:
            return 1
        return 0
    
    def initialize(self):
        _res = pymon()
        _msg = self.msg
        _msg.info("initialization [cpu=%8.3f ms vmem=%8.3f MB]",
                  _res[0], _res[1])
        _msg.info("configuring %s to store perfmon-infos into [%s]",
                  self.name, self.outFileName)
        _msg.info("Components being monitored: %i", len(self.components))
        _msg.info("I/O containers: %i", len(self.ioContainers))

        import os
        self.pmon_stream = os.path.splitext(self.outFileName)[0]+'.stream'
        _msg.info('pmon-stream: [%s]', self.pmon_stream)
        if self._do_malloc_mon and '_MEMPROF_SOCKET' in os.environ:
            _msg.info('not installing malloc hooks b/c memprof is running')
            self._do_malloc_mon = False
        _msg.info('installing pmon-malloc hooks: %s', self._do_malloc_mon)
        import AthenaPython.PyAthena as PyAthena
        lib = PyAthena.load_library('PerfMonEventDict')
        memstats = PyAthena.PerfMon.MemStats
        memstats.enable(bool(self._do_malloc_mon))
        _msg.info('pmon-malloc hooks enabled: %s', bool(memstats.enabled()))
        memstats.start()
        _res = pymon()
        _msg.info("initialization [cpu=%8.3f ms vmem=%8.3f MB] [OK]",
                  _res[0], _res[1])
        return

    def finalize(self):
        _msg = self.msg
        _msg.info("Finalizing [%s]...", self.name)
        try:
            nevts = self.stats['evt']['nentries']+1
        except KeyError:
            nevts = 0
        _msg.info("Events processed: %r", nevts)
        nalgs = self.domains_db().keys()
        if nalgs is None:
            nalgs = 'N/A'
        else:
            nalgs = len(nalgs)
        _msg.info('job had [%s] algorithms', nalgs)

        self.meta['perf_summary'] = dict()
        summary=self.meta['perf_summary']
        for k in _perfMonStates:
            stats = self.stats[k]
            summary[k] = {}
            summary[k]['nentries'] = stats['nentries']
            for istat in ('cpu', 'cpu_user', 'cpu_sys', 'real',):
                summary[k][istat] = (float(stats[istat][0]),
                                     float(stats[istat][1]),
                                     'ms')
            for istat in ('vmem', 'malloc',):
                summary[k][istat] = (float(stats[istat][0])/1024.,
                                     float(stats[istat][1])/1024.,
                                     'MB')
            _msg.info("Statistics for '%s': (nbr entries = %i)",
                      k, stats['nentries'])
            _msg.info("<cpu>:      ( %10.3f +/- %10.3f ) ms",
                      stats['cpu'][0],
                      stats['cpu'][1])
            _msg.info("<cpu_user>: ( %10.3f +/- %10.3f ) ms",
                      stats['cpu_user'][0],
                      stats['cpu_user'][1])
            _msg.info("<cpu_sys>:  ( %10.3f +/- %10.3f ) ms",
                      stats['cpu_sys'][0],
                      stats['cpu_sys'][1])
            _msg.info("<real>:     ( %10.3f +/- %10.3f ) ms",
                      stats['real'][0],
                      stats['real'][1])
            _msg.info("<vmem>:     ( %10.3f +/- %10.3f ) MB",
                      stats['vmem'][0]/1024.,
                      stats['vmem'][1]/1024.)
            _msg.info("<malloc>:   ( %10.3f +/- %10.3f ) MB",
                      stats['malloc'][0]/1024.,
                      stats['malloc'][1]/1024.)
            _msg.info("<nalloc>:   ( %10.3f +/- %10.3f ) calls",
                      stats['nalloc'][0],
                      stats['nalloc'][1])
            _msg.info("<rt>:       ( %10.3f +/- %10.3f ) ns",
                      stats['rt'][0],
                      stats['rt'][1])
            pass
        # used later on: cache only part of that big dict of dict
        self._evt_cpu_mean = self.stats['evt']['cpu'][0]

        # store some job-level quantities
        summary['job'] = {}

        def _harvest_statm():
            # see: http://www.kernel.org/doc/man-pages/online/pages/man5/proc.5.html
            # most interesting are probably:
            # VmPeak : vmem high water mark
            # VmSize : virtual memory size
            # VmHWM  : rss high water mark
            statm = {}
            from sys import platform
            if platform != 'darwin' :
                for l in open('/proc/self/status', 'r'):
                    # lines are of the form:
                    # VmPeak: some value
                    ll = map(str.strip, l.split(':'))
                    k = ll[0]
                    v = ' '.join(ll[1:])
                    statm[k] = v
                _msg.info('memory infos from [/proc/self/status]:')
            else:
                # As far as I can tell, the ps command returns vsz in units of pages, despite
                # the man pages that claim they're in Kb. 
                from os import popen, getpid
                from resource import getpagesize
                vmem = int(popen('ps -p %d -o %s | tail -1' % (getpid(), "vsz")).read())
                rss  = int(popen('ps -p %d -o %s | tail -1' % (getpid(), "rss")).read())
                statm['VmSize'] = '%s kB' % (vmem * getpagesize() / 1024,)
                statm['VmRSS'] = '%s kB' % (rss * getpagesize() / 1024,)
                _msg.info('memory infos from ps:')
            keys = sorted([k for k in statm.keys() if k.startswith('Vm')])
            for k in keys:
                _msg.info('%s:\t%s', k, statm[k])
            return statm
        summary['job']['statm'] = _harvest_statm()

        # vmem development and slope in various event intervals
        if self._slope_data:
            nevts, first_vmem, evt10_vmem, last_vmem=self._slope_data['developments']
            _msg.info('vmem-leak estimation: (nevts=%i)', nevts)
            _msg.info('  first-evt vmem:  %8.3f Mb', first_vmem/1024.)
            _msg.info('  10th -evt vmem:  %8.3f Mb', evt10_vmem/1024.)
            _msg.info('  last -evt vmem:  %8.3f Mb',  last_vmem/1024.)
            maxfitn=0
            for evtstr,fitn,fitted_slope in self._slope_data['fits']:
                maxfitn=max(maxfitn,fitn)
            for evtstr,fitn,fitted_slope in self._slope_data['fits']:
                _msg.info( '  evt %s fitted vmem-slope (%s points): %s'%
                           (evtstr,str(fitn).rjust(len(str(maxfitn))),
                            '%7.1f kb/evt'%fitted_slope if fitn>=2 else 'N/A') )
            summary['job']['vmem_slope'] = self._slope_data
        else:
            _msg.info('vmem-leak estimation: [N/A]')
            summary['job']['vmem_slope'] = None
            
        ## try to recoup some memory by flushing out ROOT stuff...
        headerFile = os.path.splitext(self.outFileName)[0]+".dat"
        if os.path.exists(headerFile):       os.remove(headerFile)
        if os.path.exists(self.outFileName): os.remove(self.outFileName)

        ## build the callgraph...
        #import PerfMonComps.PerfMonSerializer as pmon_ser
        #callgraph = pmon_ser.build_callgraph(self.pmon_stream)
        
        outFiles = [
            headerFile] + [
            self.pmon_stream#.name
            ]

        basename=os.path.splitext(self.outFileName)[0]

        hdf5_file = basename+'.h5'
        if os.path.exists(hdf5_file):
            outFiles.append(hdf5_file)

        pmonsd_file=basename+'.pmonsd.txt'
        if os.path.exists(pmonsd_file):
            outFiles.append(pmonsd_file)

        
        ## flush some more stuff which may hang around and hold memory
        ## for no good reason anymore
        del self.stats
        
        try:
            self.__write_out_pmon_data(outFiles)
        except Exception:
            _msg.info('caught an exception:')
            _msg.info(sys.exc_info()[0])
            _msg.info(sys.exc_info()[1])
            _msg.info('graceful shutdown... (pmon data may be corrupted)')
            return

        # only run the post-processing if number of processed events
        # greater than some very big value
        if self._do_post_processing:
            try:
                _msg.info('running perfmon post-processing...')
                self.__run_postprocessing()
            except Exception:
                _msg.info('caught an exception:')
                _msg.info(sys.exc_info()[0])
                _msg.info(sys.exc_info()[1])
                _msg.info('could not run perfmon post-processing !')
                pass
            _msg.info('running perfmon post-processing... [done]')
            pass
        return

    # can't use 'fork' as this confuses grid monitoring tools
    #@forking
    def __write_out_pmon_data(self, outFiles):
        headerFile = outFiles[0]
        
        _msg = self.msg
        db_infos = ('err',) # lshosts_infos()
        if db_infos[0] != 'ok':
            host_infos = { 'si2k' : 0.,
                           'cpuf' : 0. }
        else:
            host_infos = db_infos[1]
            host_infos['si2k'] = float(host_infos['cpuf'])*1000.
        #stats = self.stats['evt']
        _msg.info("Si2k: %r => <cpu time>: %10.3f kSi2k",
                  host_infos['si2k'],
                  host_infos['si2k']*self._evt_cpu_mean/1000.)
        self.meta['hosts_infos'] = host_infos
        self.meta['pmon_version']= __version__
        self.meta['algs_names'] = getattr(self, '_algs_names', ['<N/A>'])
        
        _msg.info("Writing out collected data... [%s]", self.outFileName)

        ## write out meta-data
        import PyUtils.dbsqlite as dbs
        meta = dbs.open(headerFile, 'n')
        for k,v in self.meta.iteritems(): meta[k] = v
        meta['version_id'] = '0.4.0' # stream-format + header file
        meta['pmon_tuple_files'] = map( os.path.basename, outFiles[1:] )
        import socket
        try:   meta['hostname'] = socket.gethostname()
        except Exception: meta['hostname'] = '<unknown>'
        meta.close()

        
        cwd = os.getcwd()
        ## compress/package everything
        import tarfile
        tar = tarfile.open(self.outFileName, 'w:gz')
        try:
            for outFile in outFiles:
                outFileDirName = os.path.dirname(outFile)
                try: os.chdir(outFileDirName)
                except OSError,err: pass
                outFile = os.path.basename(outFile)
                _msg.info(' --> [%s] => %8.3f kb',
                          outFile,
                          os.stat(outFile).st_size/Units.kB)
                tar.add(outFile)
                os.remove(outFile)
                os.chdir(cwd)
                pass
        except Exception,err:
            _msg.warning(err)
            pass
        os.chdir(cwd)
        tar.close()
        _msg.info ("Writing out collected data... [%s] => %8.3f kb",
                   self.outFileName,
                   os.stat(self.outFileName).st_size / Units.kB)
        return

    @forking
    def __run_postprocessing(self):
        # remove the handlers which are not the athena ones
        from AthenaCommon.Logging import log as _msg
        _handlers = _msg.handlers[:]
        map( _msg.removeHandler, (h for h in _msg.handlers))
                
        from PerfMonAna import App as pm
        outFileName = os.path.basename(self.outFileName)\
                      .replace(".pmon.gz", ".perfmon.root")

        # loads and install a basic filtering function
        from PerfMonAna.UserFct import loadFilterFct
        loadFilterFct("m.type == 'summary'")

        ana = pm.AppMgr( inputFileNames = [self.outFileName],
                         outFileName    = outFileName,
                         analyzers      = ('mem','cpu'),
                         dsLabels       = ["chk"],
                         fitSlice       = "0.80" )
        ana.run()

        # restore handlers
        _msg.handlers[:] = _handlers
        return
    
    pass # class Svc
        
        
class PoolMonTool(object):
    """
    Persistency monitoring tool: measures memory and disk sizes of input and
    output containers (in POOL files)
    """
    def __init__(self, svc):
        super(PoolMonTool,self).__init__()
        self.svc  = svc
        self.name = svc.name+".Pool"
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        inFiles  = set()
        if hasattr(svcMgr,'EventSelector') and \
           hasattr(svcMgr.EventSelector, 'InputCollections') :
            for inFile in svcMgr.EventSelector.InputCollections:
                if inFile.startswith("ROOTTREE:"):
                    inFile = inFile[len("ROOTTREE:"):]
                inFiles.add( inFile )
        outFiles = set()
        from AthenaCommon import CfgMgr
        from AthenaCommon.Configurable import Configurable
        for c in Configurable.allConfigurables.values():
            if not isinstance(c, CfgMgr.AthenaOutputStream): continue
            try:
                outFile = c.properties()["OutputFile"]
            except KeyError: continue
            if outFile.startswith("ROOTTREE:"):
                outFile = outFile[len("ROOTTREE:"):]
            outFiles.add( outFile )

        self.inputPoolFiles  = [ i for i in inFiles  ]
        self.outputPoolFiles = [ o for o in outFiles ]
        
    @property
    def msg(self):
        import AthenaCommon.Logging as L
        return L.logging.getLogger(self.name)

    def initialize(self):
        self.msg.info( "Initializing [%s]", self.name )
        self.msg.info( "InputPoolFiles:  %r", self.inputPoolFiles )
        self.msg.info( "OutputPoolFiles: %r", self.outputPoolFiles )
        return

    def finalize(self):
        import sys
        import PyUtils.PoolFile as PF
        class ShutUp:
            def __init__(self):
                self.save = os.dup( sys.stdout.fileno() )
                self.quiet= open( "/dev/null", "w" )
                return
            def mute(self):
                os.dup2( self.quiet.fileno(), sys.stdout.fileno() )
                return
            def unMute(self):
                os.dup2( self.save, sys.stdout.fileno() )
                return

        def _unfold(pr):
            return { 'name'         : pr.name,
                     'memSize'      : pr.memSize,
                     'diskSize'     : pr.diskSize,
                     'memSizeNoZip' : pr.memSizeNoZip,
                     'nEntries'     : pr.nEntries,
                     'dirType'      : pr.dirType,
                     'details'      : pr.details,
                     }
        import tempfile, atexit, os
        tmpfileno,tmpfile = tempfile.mkstemp(suffix='.py_shelve.dat')
        os.close(tmpfileno)
        atexit.register (os.unlink, tmpfile)

        import commands
        sc,checkfile = commands.getstatusoutput('which checkFile.py')
        if sc != 0:
            self.msg.info ('could not fetch checkFile.py !')
            self.msg.info ('no POOL file post-processing')
            return
        checkfile = os.path.realpath (checkfile)
        
        def run_check_file (pool_file, dbfile, app=checkfile):
            from subprocess import call
            with open('/dev/null','w') as dev_null:
                res= call ([app, "-f", pool_file, "-o", dbfile, "--fast"],
                           stdout=dev_null,
                           stderr=dev_null)
            return res
            
        
        _msg = ShutUp()
        self.msg.info( "Finalizing [%s]", self.name )
        ## FIXME: move to 'with' stmt when py2.5 is there for proper RAII
        ##        and leaner code...
        self.inputPoolFiles = [ f for f in self.inputPoolFiles ]
        if len(self.inputPoolFiles)>0:
            self.msg.info( "Content of input POOL files:" )
            _msg.mute()
            _msg.unMute()
            for i,inFileName in enumerate(self.inputPoolFiles[:20]):
                try:
                    _msg.mute()
                    sc = run_check_file (pool_file=inFileName,
                                         dbfile=tmpfile)
                    if sc != 0:
                        raise RuntimeError \
                              ('error running check file (rc=%r)'%sc)
                    inFile = PF.PoolFile (tmpfile)
                    _msg.unMute()
                    self.svc.meta['inputPoolFiles/%i'%i] = {
                        'infos'      : inFile._fileInfos,
                        'nbrEvts'    : inFile.dataHeader.nEntries,
                        'dataHeader' : [ _unfold(inFile.dataHeader) ],
                        'data'       : [ _unfold(p) for p in inFile.data]
                        }
                    self.msg.info(" - [%s] %8.3f kb (%i events)",
                                  inFile._fileInfos['name'],
                                  inFile._fileInfos['size'] / Units.kB,
                                  inFile.dataHeader.nEntries)
                    del inFile               
                except Exception,err:
                    _msg.unMute()
                    self.msg.info( "Could not run checkFile on [%s] !!",
                                   inFileName )
                    self.msg.info( "Reason: %s", err )
                    if 'inFile' in dir(): del inFile               
                _msg.unMute()
        if len(self.outputPoolFiles)>0:
            self.msg.info( "Content of output POOL files:" )
            for i,outFileName in enumerate(self.outputPoolFiles):
                try:
                    _msg.mute()
                    sc = run_check_file (pool_file=outFileName,
                                         dbfile=tmpfile)
                    if sc != 0:
                        raise RuntimeError \
                              ('error running check file (rc=%r)'%sc)
                        
                    outFile = PF.PoolFile (tmpfile)
                    _msg.unMute()
                    self.svc.meta['outputPoolFiles/%i'%i] = {
                        'infos'      : outFile._fileInfos,
                        'nbrEvts'    : outFile.dataHeader.nEntries,
                        'dataHeader' : [ _unfold(outFile.dataHeader) ],
                        'data'       : [ _unfold(p) for p in outFile.data]
                        }
                    self.msg.info( " - [%s] %8.3f kb (%i events)",
                                   outFile._fileInfos['name'],
                                   outFile._fileInfos['size'] / Units.kB,
                                   outFile.dataHeader.nEntries)
                    del outFile               
                except Exception,err:
                    _msg.unMute()
                    self.msg.info( "Could not run checkFile on [%s] !!",
                                   outFileName )
                    self.msg.info( "Reason: %s", err )
                    if 'outFile' in dir(): del outFile               
                _msg.unMute()
                
        return

    def startAud(self, step, comp, comp_id):
        return

    def stopAud(self, step, comp, comp_id):
        return

    pass # class PoolMonTool

class HephaestusMonTool(object):
    """
    """
    def __init__(self, svc):
        super(HephaestusMonTool,self).__init__()
        self.svc  = svc
        self.name = svc.name+".Heph"
        self.lag  = 2 # event lag between execute and report
        # guard against not correctly initialized Hephaestus
        # This can happen when e.g. an error occurs during initialize
        # preventing Hephaestus to install a checkPoint, leading to a segfault
        # during our finalize.
        self._heph_has_checkPoint = False 
        import sys
        if not 'Hephaestus.atexit' in sys.modules.keys():
            self.msg.warning('Hephaestus was not correctly initialized !')
            self.msg.warning('Final report may be inaccurate...')
            self.msg.warning('(to fix this, run athena with --leak-check)')

        import dl, Hephaestus.MemoryTracker as m
        _hephLib = dl.open (m.__file__, dl.RTLD_GLOBAL | dl.RTLD_NOW)
        memtrack = m

        from os.path import splitext
        self.outFileName = splitext(self.svc.outFileName)[0]+'.heph.log'
        outFile = open(self.outFileName,'w')
        from time import gmtime, strftime
        now = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())

        outFile.writelines( [
            "#"*80        + os.linesep,
            "## Report file for Hephaestus [%s]" %
            outFile.name + os.linesep,
            "## %s" % now + os.linesep,
            "#"*80        + os.linesep,
            ] )
        outFile.flush()
        
        memtrack.outstream( outFile )

    @property
    def _hephLib(self):
        import dl, Hephaestus.MemoryTracker as m
        return dl.open (m.__file__, dl.RTLD_GLOBAL | dl.RTLD_NOW)

    @property
    def memtrack(self):
        import Hephaestus.MemoryTracker as m
        return m
        
    @property
    def msg(self):
        import AthenaCommon.Logging as L
        return L.logging.getLogger(self.name)

    def initialize(self):
        self.msg.info( "Initializing [%s]", self.name )
        self.outFile = open(self.outFileName, 'a')
        
        if self.svc._pmon_flags['doPerEvtLeakCheck']:
            self.msg.debug("activating per-event leak check")
        else:
            def _stopAud(step, compName, compId): return
            self.stopAud = _stopAud
            del _stopAud
            self.msg.debug("de-activating per-event leak check")

        self.msg.info( "Hephaestus report will be stored in [%s]",
                       self.outFile.name )
        return

    def finalize(self):
        self.msg.info( "Finalizing [%s]", self.name )
        if not self._heph_has_checkPoint:
            return
        
        # consolidate last events with end-of-job leak report
        _clearCheckPoint = self.memtrack.CheckPoints.clearCheckPoint
        for _ in xrange(self.lag):
            _clearCheckPoint( 0 )

        # put the per-evt leaks into a different file
        perEvtFileName = self.outFile.name.replace(".heph.log",
                                                   ".heph.perevt.log")
        self.outFile.flush()
        self.outFile.seek(0)
        from shutil import copyfile
        copyfile(self.outFile.name, perEvtFileName)

        self.outFile.seek(0)
        self.outFile.truncate(0)
        from time import gmtime, strftime
        now = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())

        self.outFile.writelines( [
            "#"*80        + os.linesep,
            "## Report file for Hephaestus [%s]" %
            self.outFile.name + os.linesep,
            "## %s" % now + os.linesep,
            "#"*80        + os.linesep,
            ] )
        self.outFile.flush()
        
        return

    def startAud(self, step, compName, comp_id):
        return

    def stopAud(self, step, compName, comp_id):
        if step != 'evt' or compName != "PerfMonSlice":
            return
        self.outFile.writelines( [
            os.linesep,
            "="*80 + os.linesep,
            "=== hephaestus report for evt [%i] ===" %
            self.svc.evtNbr[0] + os.linesep,
            ] )
        self.outFile.flush()
        
        memtrack = self.memtrack.CheckPoints
        memtrack.check( self.lag )
        memtrack.clearCheckPoint( self.lag )
        memtrack.setCheckPoint()
        self._heph_has_checkPoint = True
        
        return
    pass # class HephaestusMonTool

