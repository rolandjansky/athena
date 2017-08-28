# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file AthenaCommon/python/AthApi.py
# @purpose API for driving athena
# @author Sebastien Binet <binet@cern.ch>

from __future__ import with_statement

__version__ = "$Revision: 1.3 $"
__author__  = "Sebastien Binet <binet@cern.ch>"
__doc__ = """
a set of api functions for driving athena
"""

__all__ = [
    'AthCfg',
    'AthApp',
    ]

import sys, os
if not '' in sys.path:
    sys.path.insert(0, '')
    
from . import ExitCodes
from . import Logging
msg = Logging.log

class State:
    """Python equivalent of IService::State enum (kind of silly to load a whole
    dictionary library for just this quartet of integers
    """

    # for reference, the numbers below can also be obtained through a dict:
    #    import cppyy
    #    melm = cppyy.makeClass( 'MinimalEventLoopMgr' )
    #    print melm.OFFLINE, melm.CONFIGURED, melm.FINALIZED, melm.INITIALIZED

    OFFLINE     = 0
    CONFIGURED  = 1
    INITIALIZED = 2
    RUNNING     = 3

def cast_to(obj, iface):
    from GaudiPython.Bindings import InterfaceCast as icast
    return icast(iface)(obj)

class AthCfg(object):

    def __init__(self):

        self._jobo = open('athapi-%s-jobo.py' % os.getpid(), 'w+')
        for fname in ('seek', 'write', 'read'):
            setattr(self, fname, getattr(self._jobo, fname))
        return

    def include(self, jobo_name):
        self._jobo.write('include("%s")\n'%jobo_name)
        self._jobo.flush()
        
    def __lshift__(self, o):
        if isinstance(o, str):
            import textwrap
            self._jobo.write(textwrap.dedent(o))
            self._jobo.flush()
            return
        raise TypeError('unexpected type %s'%type(o))

    pass # AthCfg

class AthApp(object):

    def __init__(self, options, **kw):
        object.__init__(self)

        self.options = options

        from . import Logging
        self.msg = Logging.log
        self.msg.setLevel(Logging.logging.INFO)

        self.app = None
        self.cfg = AthCfg()
        
        self.__dict__['_exitstate'] = ExitCodes.ALL_OK
        self.__dict__['_current_event'] = 0
        
        return
    
    def configure(self, joboptions=None, commands=None,
                  dllname=None,
                  factname=None,
                  extra_options=None):

        if not (self.app is None):
            self.msg.info('C++ application already configured')
            return self.app

        self.msg.info('configuring application...')

        usr_cfg = AthCfg()
        self.cfg.seek(0)
        usr_cfg << self.cfg.read()

        # reset
        self.cfg = AthCfg()

        if commands:
            self.cfg << commands+'\n'

        # common configuration
        self.cfg << """
        # basic job configuration
        include('AthenaCommon/Atlas.UnixStandardJob.py')
        include.block('AthenaCommon/Atlas.UnixStandardJob.py')

        if not (not %(run_batch)s and
                theApp.EventLoop  == 'PyAthenaEventLoopMgr'):
           # make SIG_INT fatal
           svcMgr.CoreDumpSvc.FatalHandler = -1
        """ % {'run_batch' : self.options.run_batch}

        self.cfg << """
        # user level configuration
        try:
            include('$HOME/.athenarc')
        except IncludeError:
            pass
        """

        # another user level configuration
        usr_cfg.seek(0)
        self.cfg << usr_cfg.read()

        if isinstance(joboptions, (list,tuple)):
            for jobo_name in joboptions:
                self.cfg.include(jobo_name)

        if not self.options.run_batch:
            self.cfg << """
            theApp.EventLoop = 'PyAthenaEventLoopMgr'
            svcMgr += CfgMgr.PyAthenaEventLoopMgr()
            """

        self.cfg << """
        ### logging and messages ---------
        from AthenaCommon.Logging import *
        _msg = log
        _msg.setLevel(getattr(logging, '%(output_level)s'))
    
        import AthenaCommon.Constants as Lvl
        theApp.setOutputLevel(%(output_level)s)
        theApp.OutputLevel = Lvl.%(output_level)s
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.MessageSvc.OutputLevel = Lvl.%(output_level)s
        """ % dict(output_level=self.options.msg_lvl)

        self.cfg << """
        from AthenaCommon.Include import Include, IncludeError, include
        include.setShowIncludes(%(showincludes)s)
        if %(showincludes)s:
            import AthenaCommon.Include as AthCIncMod
            AthCIncMod.marker=' -#-' # distinguish bootstrap from other jo-code
        """ % dict(showincludes=self.options.showincludes)

        cfg_name = self.cfg._jobo.name.replace('.py','.pkl')
        self.msg.info('dumping job-configuration into [%s]...',
                      cfg_name)

        # run configuration in a forked-subprocess...
        sc = _app_configure(self.cfg, cfg_name, extra_options)
        if sc:
            err = 'could not configure application [sc=%d]' % sc
            self.msg.error(err)
            raise RuntimeError(err)
        self.msg.info('configuring application w/ [%s]', cfg_name)

        import os
        self.cfg._jobo.close()
        os.remove(self.cfg._jobo.name)

        import GaudiPython.Bindings as gaudi
        # remove the gaudimodule exit handler as to prevent them from clobering
        import atexit
        for hdlr in reversed(atexit._exithandlers[:]):
            module_name = hdlr[0].__module__
            if ('GaudiPython' in module_name or
                'gaudimodule' in module_name):
                atexit._exithandlers.remove(hdlr)
        del hdlr
        # install our own exit handler (if needed)
        import sys
        if hasattr(sys, 'ps1'): # ie: is interactive
            atexit.register(self.exit)
        del atexit
        
        from . import ResourceLimits
        ResourceLimits.SetMaxLimits()

        try: import cPickle as pickle
        except ImportError: import pickle
        import PyUtils.dbsqlite as dbs
        db = dbs.open(cfg_name, 'r')

        jobo_cfg = db['jobopts']
        
        kw = jobo_cfg['ApplicationMgr']
        for k in ('Go', 'Exit', 'AuditInitialize', 'AuditFinalize'):
            if k in kw:
                del kw[k]
        outputlevel = jobo_cfg['ApplicationMgr']['OutputLevel']

        self.app = gaudi.AppMgr(outputlevel=outputlevel,
                                selfoptions=kw,
                                dllname=dllname,
                                factname=factname)

        # open the pycomps folder
        pycomps = db.get('pycomps', None) # just opening it should do

        if pycomps:
            import AthenaPython.Configurables as _C
            _C.PyComponents.instances = dict((p.name, p) for p in pycomps)
            #_C.PyComponents.instances = pycomps
            for p in pycomps:
                if hasattr(p, 'setup'):
                    if callable(p.setup):
                        p.setup()
            
        setattr(self, '_pycomps', pycomps)
            
        import AthenaPython.PyAthena as PyAthena
        josvc = PyAthena.py_svc('JobOptionsSvc',
                                createIf=False,
                                iface='IJobOptionsSvc')
        assert josvc is not None

        for client in jobo_cfg:
            if client == 'ApplicationMgr':
                continue
            for n,v in jobo_cfg[client].iteritems():
                p = gaudi.StringProperty(n, v)
                if not josvc.addPropertyToCatalogue(client, p).isSuccess():
                    self.msg.error(
                        'could not add property [%s.%s = %s]',
                        client, n, v
                        )
                if client in ('MessageSvc', 'JobOptionsSvc'):
                    svc = PyAthena.py_svc(client, iface='IProperty')
                    svc.setProperty(p)

        db.close()
        
        import os
        if os.path.exists(cfg_name):
            os.remove(cfg_name)
            pass
        #import AthenaCommon.Debugging as dbg
        #dbg.hookDebugger()

        return self.app

    def __getattr__(self, name):
        if name[0]=='_':
            return object.__getattr__(self, name)
        if name in self.__dict__:
            return self.__dict__[name]
        else:
            return getattr(self.app, name)

    @property
    def exitstate(self):
        return self._exitstate

    @property
    def event(self):
        return self._current_event

    def state(self):
        if self.app is None:
            return State.OFFLINE
        return self.app.state()
    
    def service(self, name, iface=None):
        return self.app.service(name,interface=iface)

    def initialize(self):
        assert(self.app)

        try:
            sc = self.app.initialize()
            if not sc.isSuccess():
                self._exitstate = ExitCodes.INI_ALG_FAILURE
        except:
            self._exitstate = ExitCodes.INI_ALG_EXCEPTION
            raise
        return sc

    def reinitialize(self):
        assert(self.app)

        # ApplicationMgr::reinitialize does not re-init algorithms...
        for name in self.app.TopAlg:
            try:
                name = name.split('/')
                alg_name = name[1] if len(name)==2 else name[0]
                sc = self.app.algorithm(alg_name).reinitialize()
                if not sc.isSuccess() and not self._exitstate:
                    self._exitstate = ExitCodes.REINI_ALG_FAILURE
                    return sc
            except AttributeError:
                pass
            except:
                self._exitstate = ExitCodes.REINI_ALG_EXCEPTION
                raise

        # normal reinitialize kicks services, tools left out...
        sc = self.app.reinitialize()
        if not sc.isSuccess():
            self._exitstate = ExitCodes.REINI_ALG_FAILURE
        return sc
    
    def run(self, nevents=None):
        assert(self.app)
        
        if self.state() in (State.OFFLINE,
                            State.CONFIGURED,):
            self.msg.debug('Initializing application manager')
            sc = self.initialize()
        else:
            self.msg.debug('Re-initializing application manager')
            sc = self.reinitialize()

        if not sc.isSuccess():
            return sc

        sc = self.start()
        if not sc.isSuccess():
            # XXX add-in new states in ExitCodes !
            self._exitstate = ExitCodes.BEGIN_RUN_FAILURE
            return sc

        if nevents is None:
            nevents = self.app.EvtMax

        # XXX: gdb

        # actual run
        # XXX: capture beginRun() exceptions and failures
        #      this is not currently published by IEventProcessor iface
        try:
            sc = self.app._evtpro.executeRun(nevents)
            if not sc.isSuccess() and not self._exitstate:
                self.msg.error('failed to "executeRun(%d)"',nevents)
                self._exitstate = ExitCodes.EXE_ALG_FAILURE # likely an alg
        except:
            self._exitstate = ExitCodes.EXE_ALG_EXCEPTION   # idem
            raise

        # what to do when executeRun(nevents) failed ?
        # self.stop would override the value (and likely return success)
        # in effect 'poisonning' the next call to self.app !
        if not sc.isSuccess():
            return sc
        
        sc = self.stop()
        if not sc.isSuccess():
            # XXX add-in new states in ExitCodes !
            self._exitstate = ExitCodes.END_RUN_FAILURE
            return sc

        # done, update and report
        self._current_event = nevents

        return sc

    def nextEvent(self, nevents=None):
        assert(self.app)
        # start app if not already done
        sc = self.start()
        if not (sc.isSuccess()) and not self._exitstate:
            self._exitstate = ExitCodes.INI_ALG_FAILURE
            return sc
        
        if nevents is None:
            nevents = self.event + 1

        try:
            sc = self.app._evtpro.nextEvent(nevents)
            if (not sc.isSuccess()) and not self._exitstate:
                self._exitstate = ExitCodes.EXE_ALG_FAILURE
        except:
            self._exitstate = ExitCodes.EXE_ALG_EXCEPTION
            raise

        # TODO: would like NOT to advance on failure, but...
        self._current_event = nevents
        return sc

    def start(self):
        import GaudiPython.Bindings as gaudi
        sc = gaudi.SUCCESS
        can_start = gaudi.Gaudi.StateMachine.INITIALIZED == self.app.FSMState()
        if can_start:
            self.msg.debug('Starting application manager')
            sc = self.app.start()
        sc.ignore()
        return sc
    
    def stop(self):
        import GaudiPython.Bindings as gaudi
        sc = gaudi.SUCCESS
        can_stop = gaudi.Gaudi.StateMachine.RUNNING == self.app.FSMState()
        if can_stop:
            self.msg.debug('Stopping application manager')
            sc = self.app.stop()
        sc.ignore()
        return sc
    
    def finalize(self):
        assert(self.app)
        
        sc = self.stop()
        if not sc.isSuccess() and not self._exitstate:
            self._exitstate = ExitCodes.FIN_ALG_FAILURE

        try:
            sc = self.app._appmgr.finalize()
            if not sc.isSuccess():
                self._exitstate = ExitCodes.FIN_ALG_FAILURE
        except:
            self._exitstate = ExitCodes.FIN_ALG_EXCEPTION
            raise

        return sc

    def exit(self, code=None):
        try:
            if self.state() > State.CONFIGURED:
                sc1 = self.finalize()
                sc2 = self.app._appmgr.terminate()
                if ((not sc1.isSuccess() or not sc2.isSuccess()) and
                    not self._exitstate):
                    self._exitstate = ExitCodes.FIN_ALG_FAILURE
        except:
            if not self._exitstate:
                self._exitstate = ExitCodes.FIN_ALG_EXCEPTION
            import traceback
            traceback.print_exc() # no re-raise to allow sys.exit next

        self.msg.info('leaving with code %d: "%s"',
                      self.exitstate,
                      ExitCodes.what(self.exitstate))
        import sys
        sys.exit(code is None and self.exitstate or code)
        
    pass # AthApp

## ## helper class to hold a joboptionssvc configuration
## import ConfigParser
## class JobOptCfg(ConfigParser.RawConfigParser):
##     # We want to retain case sensitivity
##     def optionxform(self, optionstr):
##         return optionstr  

## --- helper functions for configuration storage -----------------------------
try:     import cPickle as pickle
except ImportError: import pickle
import PyUtils.dbsqlite as dbs
from collections import defaultdict
def store_configuration(cfg_fname=None):
    """helper function to inspect the 'configured' JobOptionsSvc and dump
    the properties of each component into a (sqlite) shelve.
    it will eventually dump the properties of py-components too.
    """
    jobo_cfg = defaultdict(dict)
    if cfg_fname is None:
        import tempfile
        tmpfile = tempfile.NamedTemporaryFile(suffix='-jobo.pkl')
        cfg_fname = tmpfile.name
        tmpfile.close()
        import os
        if os.path.exists(cfg_fname):
            os.remove(cfg_fname)

    assert cfg_fname

    from AthenaCommon.AppMgr import theApp

    def _fill_cfg(client, props):
        for p in props:
            n = p[0]
            v = p[1]
            if hasattr(v, 'toStringProperty'):
                v = str(v.toStringProperty().toString())
            elif hasattr(v, 'toString'):
                v = str(v.toString())
            else:
                v = str(v)
            jobo_cfg[client][n] = v
    
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    
    # special cases: joboptionsvc and messagesvc
    def _fill_props(svcname):
        if not hasattr(svcMgr, svcname):
            return
        svc = getattr(svcMgr, svcname)
        props = []
        for k,v in svc.properties().iteritems():
            if v == svc.propertyNoValue:
                v = svc.getDefaultProperty(k)
            props.append((k,v))
        _fill_cfg(svcname, props)
    _fill_props('JobOptionsSvc')
    _fill_props('MessageSvc')
    
    # tickle C++ and configure the whole app
    theApp.setup()
    
    app_props = [(k,v.value())
                 for k,v in theApp.getHandle().properties().iteritems()]
    _fill_cfg(theApp.name(), app_props)

    import AthenaPython.PyAthena as PyAthena
    josvc = PyAthena.py_svc('JobOptionsSvc', iface='IJobOptionsSvc')
    assert josvc is not None

    clients = list(josvc.getClients())
    for client in clients:
        props = josvc.getProperties(client)
        for prop in props:
            n = prop.name()
            v = prop.toString()
            jobo_cfg[client][n] = v

    cfg = dbs.open(cfg_fname, 'w')
    cfg['jobopts'] = jobo_cfg

    pycomps = []
    import sys
    from .AppMgr import ServiceMgr as svcMgr
        
    # all other pycomps
    from .Configurable import Configurable as C
    for c in C.allConfigurables.itervalues():
        if not isinstance(c, (PyAthena.Alg,
                              PyAthena.AlgTool,
                              PyAthena.Svc,
                              PyAthena.Aud)):
            continue
        # FIXME: should check if the component is still 'active'
        #        ie: is it in one of the TopAlg,SvcMgr,ToolSvc and children ?
        if hasattr(c, 'msg'):
            delattr(c, 'msg')
        pycomps.append(c)
        pass
    
    # folder for the pyathena-components
    cfg['pycomps'] = pycomps
    cfg.close()

    return cfg_fname

import code
class AthenaInteractiveConsole(code.InteractiveConsole):
    def __init__(self, *args, **kw):
        code.InteractiveConsole.__init__(self, *args, **kw)
        import sys
        sys.ps1 = 'athena> '
        sys.ps2 = '.   ... '
    pass

### helper functions ----------------------------------------------------------
def enter_interactive_loop(banner=None, app=None, options=None):
    import sys
    import os

    # retrieve  the current frame
    frame = sys._getframe(0)

    # evaluate commands in current namespace
    namespace = frame.f_globals.copy()
    namespace.update(frame.f_locals)
    # NOTE: the above namespace won't capture the below :)
    
    try:
        import readline
    except ImportError:
        pass
    else:
        import rlcompleter
        readline.set_completer(rlcompleter.Completer(namespace).complete)
        readline.parse_and_bind('tab:complete')
        readline.parse_and_bind('set show-all-if-ambiguous On')

        # history support
        history = os.path.expanduser('~/.athena.history')
        if os.path.exists(history):
            readline.read_history_file(history)
        readline.set_history_length(1024)
        
    # we want to honor both $PYTHONSTARTUP and .pythonrc.py, so follow system
    # conventions and get $PYTHONSTARTUP first, then import user
    pythonrc = os.environ.get('PYTHONSTARTUP')
    if pythonrc and os.path.isfile(pythonrc):
        try:
            execfile(pythonrc)
        except NameError:
            pass
    # this will import .pythonrc.py as a side effect
    import user

    # use of shell escapes
    from . import ShellEscapes as SE
    sys.excepthook = SE.ShellEscapes()
    
    namespace.update(frame.f_globals)
    namespace.update(frame.f_locals)
    
    # now ready for the full "interactive python experience"
    shell = AthenaInteractiveConsole(locals=namespace)
    shell.interact(banner=banner)

from PyUtils.Decorators import forking
#@forking
def _app_configure(cfg, ascii_cfg_name, extra_options=None):
    cfg << """
    import AthenaCommon.AthApi as api
    cfg_fname = '%s'
    api.store_configuration(cfg_fname)
    """ % ascii_cfg_name

    cfg << """
    api.msg.info('configuration dumped to [%s]', cfg_fname)
    import os
    import sys
    devnull = open(os.devnull, 'w')

    # redirect stderr to devnull
    #os.dup2(devnull.fileno(), sys.stderr.fileno())

    # redirect stdout to devnull
    os.dup2(devnull.fileno(), sys.stdout.fileno())
    #sys.stderr = devnull
    #sys.stdout = devnull

    sys.exit(0)
    """
    import os
    import subprocess as sub
    athena_exe = sub.Popen(['which', 'athena.py'],
                           stdout=sub.PIPE).communicate()[0].strip()

    athena_exe = os.path.expanduser(os.path.expandvars(athena_exe))
    athena_exe = os.path.realpath(athena_exe)
    athena_exe = os.path.abspath(athena_exe)

    athena_args = [athena_exe, cfg._jobo.name]
    if extra_options and isinstance(extra_options, (list, tuple)):
        athena_args.extend(list(extra_options))
        
    sc = sub.call(athena_args)#,
    # stdout=None,#open(os.devnull,'w'),
    # stderr=sub.STDOUT)
    return sc

