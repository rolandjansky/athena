# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: AthenaPython/python/Bindings.py
# @author: Sebastien Binet <binet@cern.ch>

from __future__ import print_function

### data
__author__  = """
Sebastien Binet (binet@cern.ch)
"""

### imports
from PyUtils.Decorators import memoize
from AthenaCommon.Logging import logging

@memoize
def _load_dict(lib):
    """Helper function to remember which libraries have been already loaded
    """
    import cppyy
    if not lib.startswith(lib):
        lib="lib"+lib
    return cppyy.load_library(lib)

@memoize
def _import_ROOT():
    import ROOT
    ROOT.gROOT.SetBatch(True)
    return ROOT

    
### data ----------------------------------------------------------------------

## A dictionary to handle special cases where CLID has been registered with a
## typedef instead of the real C++ class name.
_clid_typename_aliases = {
    ## these are the annoying ROOT-Reflex namespace dropping 'feature'!
    'vector<int>'                   : 'std::vector<int>',
    'vector<unsigned int>'          : 'std::vector<unsigned int>',
    'vector<float>'                 : 'std::vector<float>',
    'vector<double>'                : 'std::vector<double>',
    'string'                        : 'std::string',
    ## <-------
    ## various typenames registered with typedefs
    'INavigable4MomentumCollection' : 'DataVector<INavigable4Momentum>',
    'DataVector<IParticle>'         : 'IParticleContainer',
    'ParticleBaseContainer'         : 'DataVector<ParticleBase>',
    'TrackCollection'               : 'DataVector<Trk::Track>',
    'Trk::TrackCollection'          : 'DataVector<Trk::Track>',
    'DataVector<Track>'             : 'TrackCollection',
    'AthenaHitsVector<TrackRecord>' : 'TrackRecordCollection',
    'Trk::SegmentCollection'        : 'DataVector<Trk::Segment>',
    }


#Typed nullpointers:
_import_ROOT()
import cppyy
svcNull=cppyy.bind_object(cppyy.nullptr,cppyy.gbl.IService)
toolNull=cppyy.bind_object(cppyy.nullptr,cppyy.gbl.IAlgTool)
algNull=cppyy.bind_object(cppyy.nullptr,cppyy.gbl.IAlgorithm)

### ---------------------------------------------------------------------------

class _PyAthenaBindingsCatalog(object):
    """Placeholder class to register callbacks for 'pythonizations' of C++
    classes.
    FIXME: find a better mechanism ?
    """
    msg       = logging.getLogger('AthenaBindingsCatalog')
    instances = {}

    @staticmethod
    def register(klass, cb=None):
        """Register a class name `klass` with an initialization method.
        If no callback method has been given, the default is to call:
         _py_init_<klass>()
        """
        try:
            if cb is None: eval( 'cb = _py_init_%s'%klass )
        except Exception as err:
            _msg = _PyAthenaBindingsCatalog.msg
            _msg.error("Problem registering callback for [%s]", klass)
            _msg.error("Exception: %s", err)
            cb = lambda : None  # noqa: E731
        _PyAthenaBindingsCatalog.instances[klass] = cb
        return

    @staticmethod
    @memoize
    def init(name):
        """Initialize the python binding with the callback previously registered
        If no callback was registered, swallow the warning...
        """
        klass = None
        try: klass = _PyAthenaBindingsCatalog.instances[name]()
        except KeyError:
            ROOT = _import_ROOT()  # noqa: F841
            from AthenaServices.Dso import registry
            registry.load_type (name)
            try:
                import cppyy
                #klass = getattr(ROOT, name)
                #klass = cppyy.makeClass(name)
                klass=getattr(cppyy.gbl,name)
            except AttributeError:
                raise AttributeError("no reflex-dict for type [%s]"%name)
        return klass

### helper method to easily retrieve services by name -------------------------
@memoize
def py_svc(svcName, createIf=True, iface=None):
    """
    Helper function to retrieve a service by name, using Gaudi python bindings.
     @param svcName: the name of the service one wants to retrieve (possibly a
            fully qualified name as in: 'MySvcClass/TheSvcName')
     @param createIf: If True, the service will be created if it hasn't been yet
            instantiated.
     @param iface: type one wants to cast the service to (can be a string or the
            cppyy type)
    """
    fullName = svcName
    s        = svcName.split('/')
    svcType  = s[0]
    if len(s)==2: svcName=s[1]

    # handle pycomponents...
    from .Configurables import PyComponents
    if svcType in _PyAthenaBindingsCatalog.instances:
        pytype = _PyAthenaBindingsCatalog.init( svcType )
        # for types which have been pythonized, help the user
        # find the good interface...
        if iface is None: iface = pytype

    from GaudiPython.Bindings import gbl,InterfaceCast
    svcLocator = gbl.Gaudi.svcLocator()
    svc = gbl.GaudiPython.Helper.service(svcLocator, fullName, createIf)
    if svc and not(iface is None):
        svc = InterfaceCast(iface).cast(svc)

    # if the component is actually a py-component,
    # retrieve the python object from the registry
    if svcName in PyComponents.instances:
        svc = PyComponents.instances[svcName]

    #Error handling: Convert c++ ptr to python-None
    if svc == svcNull:
        return None
    
    from AthenaPython import PyAthena
    setattr(PyAthena.services, svcName, svc)
    return svc

### helper method to easily retrieve tools from ToolSvc by name ---------------
@memoize
def py_tool(toolName, createIf=True, iface=None):
    """
    Helper function to retrieve a tool (owned by the ToolSvc) by name, using
    Gaudi python bindings.
     @param toolName: the name of the tool one wants to retrieve (possibly a
            fully qualified name as in: 'MyToolClass/TheToolName')
     @param createIf: If True, the tool will be created if it hasn't been yet
            instantiated.
     @param iface: type one wants to cast the tool to (can be a string or the
            cppyy type)

    Ex:
     ## retrieve default interface (ie: GaudiKernel/IAlgTool)
     tool = py_tool('LArOnlDbPrepTool')
     assert(type(tool) == cppyy.gbl.IAlgTool)

     ## retrieve with specified interface
     tool = py_tool('LArOnlDbPrepTool', iface='ILArOnlDbPrepTool')
     assert(type(tool) == cppyy.gbl.ILArOnlDbPrepTool)

    """
    t        = toolName.split('/')
    toolType = t[0]
    if len(t)==2: toolName=t[1]
    
    # handle pycomponents...
    from .Configurables import PyComponents
    if toolType in _PyAthenaBindingsCatalog.instances:
        pytype = _PyAthenaBindingsCatalog.init( toolType )
        # for types which have been pythonized, help the user
        # find the good interface...
        if iface is None: iface = pytype

    from GaudiPython.Bindings import gbl,InterfaceCast
    _py_tool = gbl.GaudiPython.Helper.tool
    toolSvc = py_svc('ToolSvc', iface='IToolSvc')
    tool = _py_tool(toolSvc, toolType, toolName, 0, createIf)
    if tool and not(iface is None):
        tool = InterfaceCast(iface).cast(tool)

    # if the component is actually a py-component,
    # retrieve the python object from the registry
    if toolName in PyComponents.instances:
        tool = PyComponents.instances[toolName]

    #Error handling: Convert c++ ptr to python-None
    if tool == toolNull:
        return None

    from AthenaPython import PyAthena
    setattr(PyAthena.services.ToolSvc, toolName, tool)
    return tool

### helper method to easily retrieve algorithms by name -----------------------
# @c memoize # don't memoize ?
def py_alg(algName, iface='IAlgorithm'):
    """
    Helper function to retrieve an IAlgorithm (managed by the IAlgManager_) by
    name, using Gaudi python bindings.
     @param algName: the name of the algorithm's instance one wants to retrieve
            ex: 'McAodBuilder'
     @param iface: type one wants to cast the tool to (can be a string or the
            cppyy type)

    Ex:
     ## retrieve default interface (ie: GaudiKernel/IAlgorithm)
     alg = py_alg('McAodBuilder')
     assert(type(alg) == cppyy.gbl.IAlgorithm)

     ## retrieve with specified interface
     alg = py_alg('McAodBuilder', iface='Algorithm')
     assert(type(alg) == cppyy.gbl.Algorithm)

    """
    algmgr = py_svc('ApplicationMgr',iface='IAlgManager')
    if not algmgr:
        msg = logging.getLogger('PyAthena.py_alg')
        error = 'could not retrieve IAlgManager/ApplicationMgr'
        msg.error (error)
        raise RuntimeError (error)
    
    # handle pycomponents...
    from .Configurables import PyComponents
    import cppyy
    alg = cppyy.libPyROOT.MakeNullPointer(iface)
    if not algmgr.getAlgorithm(algName, alg).isSuccess():
        return

    # if the component is actually a py-component,
    # retrieve the python object from the registry
    if algName in PyComponents.instances:
        alg = PyComponents.instances[algName]


    if alg==algNull:
        return None

    from AthenaPython import PyAthena
    setattr(PyAthena.algs, algName, alg)
    return alg

### pythonizations for StoreGateSvc
@memoize
def _py_init_StoreGateSvc():
    ## most probably, user will want to interact with PyRoot objects
    ## => install the fixes for our user
    import RootUtils.PyROOTFixes
    ## try to install pickling protocol for PyRoot objects
    try: RootUtils.PyROOTFixes.enable_pickling()
    except Exception: pass # fwd compatibility
    from StoreGateBindings.Bindings import StoreGateSvc

    return StoreGateSvc

@memoize
def _py_init_StoreGate():
    _py_init_StoreGateSvc()
    from StoreGateBindings.Bindings import StoreGate
    return StoreGate

### pythonizations for IIncidentSvc
@memoize
def _py_init_IIncidentSvc():
    import cppyy
    # IIncidentSvc bindings from dictionary
    _load_dict( "libGaudiKernelDict" )

    # retrieve the IIncidentSvc class
    global IIncidentSvc
    IIncidentSvc = cppyy.gbl.IIncidentSvc

    IIncidentSvc._cpp_addListener = IIncidentSvc.addListener
    def addListener (self, *args):
        listener = args[0]
        if hasattr (listener, '_cppHandle'):
            args = (getattr (listener, '_cppHandle'),) + args[1:]
        return self._cpp_addListener (*args)
    addListener.__doc__ = IIncidentSvc._cpp_addListener.__doc__
    IIncidentSvc.addListener = addListener
    del addListener

    IIncidentSvc._cpp_removeListener = IIncidentSvc.removeListener
    def removeListener (self, *args):
        listener = args[0]
        if hasattr (listener, '_cppHandle'):
            args = (getattr (listener, '_cppHandle'),) + args[1:]
        return self._cpp_removeListener (*args)
    removeListener.__doc__ = IIncidentSvc._cpp_removeListener.__doc__
    IIncidentSvc.removeListener = removeListener
    del removeListener
    return IIncidentSvc
        
### pythonizations for ClassIDSvc
@memoize
def _py_init_ClassIDSvc():
    import cppyy
    # IClassIDSvc bindings from dictionary
    _load_dict( "libAthenaPythonDict" )

    # retrieve the IClassIDSvc class
    global IClassIDSvc
    IClassIDSvc = cppyy.gbl.IClassIDSvc

    _missing_clids  = {
        'DataHistory' : 83814411, 
        83814411     : 'DataHistory',
        }

    # re-use the python-based clid generator
    # (faster than calling back into C++ via Reflex bindings)
    from CLIDComps.clidGenerator import clidGenerator
    IClassIDSvc._clidgen = clidGenerator(db=None)

    # add pythonized methods
    @memoize
    def _clid (self, name):
        # handle special cases where CLID has been registered with a typedef
        try:   name = _clid_typename_aliases[name]
        except KeyError: pass
        try:
            return _missing_clids[name]
        except KeyError: pass
        return self._clidgen.getClidFromName(name)
    IClassIDSvc.clid = _clid
    del _clid
    
    @memoize
    def _typename (self, clid):
        # handle special cases of missing clids
        try:
            return _missing_clids[clid]
        except KeyError:
            pass
        return self._clidgen.getNameFromClid(clid)
    IClassIDSvc.typename = _typename
    del _typename
    
    return IClassIDSvc

### pythonizations for ITHistSvc
@memoize
def _py_init_THistSvc():
    import cppyy
    # ITHistSvc bindings from dictionary
    _load_dict( "libGaudiKernelDict" )


    # retrieve the ITHistSvc class
    global ITHistSvc
    ITHistSvc = cppyy.gbl.ITHistSvc

    ROOT = _import_ROOT()
    @property
    def _py_cache(self):
        try:
            return self.__py_cache
        except AttributeError:
            self.__py_cache = dict()
        return self.__py_cache
    ITHistSvc._py_cache = _py_cache

    # save original regXYZ methods: we'll use some modified ones
    # to improve look-up time from python
    for n in ('Hist', 'Graph', 'Tree'):
        code = "ITHistSvc._cpp_reg%s = ITHistSvc.reg%s" % (n,n)
        exec (code, globals(),locals())

    def book(self, oid, obj=None, *args, **kw):
        """book a histogram, profile or tree
         @param oid is the unique object identifier even across streams,
                ie: 'stream'+'id'
         @param obj either an already full-fledge constructed ROOT object
                or None (then `*args` or `**kw` need to be correctly setup)
         @param *args list of arguments to give to the constructor of the
                ROOT object one wants to book
         @param **kw a dictionary containing a key 'args' being the list of
                arguments to the constructor of the ROOT objects one wants to
                book
         examples:
         >>> th.book('/temp/1d/h1',    'TH1D', args=('h1','h1',100,0.,100.))
         >>> th.book('/temp/1d/h2', ROOT.TH1D, args=('h2','h2',100,0.,100.))
         >>> th.book('/temp/1d/h3', ROOT.TH1D, 'h3','h3',100,0.,100.)
         >>> th.book('/temp/1d/h4', ROOT.TH1D('h4','h4',100,0.,100.))
         >>> th.book('/temp/1d/h5', obj=ROOT.TH1D('h5','h5',100,0.,100.))
         >>> th.book('/temp/1d/h6', klass='TH1D', args=('h6','h6',100,0.,100.))

         >>> th.book('/temp/tree/t1', ROOT.TTree('t1','t1'))
         >>> th.book('/temp/tree/t2', obj=ROOT.TTree('t2','t2'))
         >>> th.book('/temp/tree/t3', klass='TTree', args=('t3','t3'))
        """
        _err = "please provide _either_ an already constructed ROOT object or"\
               "a class name/class type (with appropriate arguments)"
        klass = kw.get('klass', None)
        assert not (obj is None and klass is None),         _err
        assert not (obj is not None and klass is not None), _err
    
        if isinstance(obj, (str,type)):
            klass=obj
            obj=None
        if obj:
            if isinstance(obj, ROOT.TH1):
                # capture all of TH1x,TH2x,TH3x,TProfileXY
                meth = '_cpp_regHist'
            elif isinstance(obj, (ROOT.TGraph,)):
                meth = '_cpp_regGraph'
            elif isinstance(obj, (ROOT.TTree,)):
                meth = '_cpp_regTree'
            else:
                raise TypeError("invalid type '%r'"%type(obj))
            if getattr(self,meth)(oid, obj).isSuccess():
                self._py_cache[oid]=obj
                return obj
            raise RuntimeError('could not book object [%r]'%obj)

        if klass:
            if isinstance(klass, str):
                klass = getattr(ROOT, klass)
            if args:
                return self.book(oid, obj=klass(*args))
            if kw and 'args' in kw:
                return self.book(oid, obj=klass(*kw['args']))
            err = "invalid arguments: either provide a valid `*args` or "\
                  "a `**kw` containing a 'args' key"
            raise RuntimeError(err)
        raise RuntimeError("unforseen case: oid='%r' obj='%r' args='%r' "
                           "kw='%r'"%(oid,obj,args,kw))
    
    ITHistSvc.book = book

    def get(self, oid, klass=None):
        """retrieve an already booked ROOT object.
        If the object was booked on the C++ side, try to use the `klass` hint
        (the usual string or type) to find the object in the correct 'folder'
        (histograms, graphs or trees).
        If `klass` is None, then go through all the folders iteratively (slow)
        """
        try:
            return self._py_cache[oid]
        except KeyError:
            pass
        def _get_helper(klass, hsvc, meth, oid, update_cache=True):
            import cppyy
            makeNullPtr = cppyy.libPyROOT.MakeNullPointer
            o = makeNullPtr(klass)
            if meth(oid, o).isSuccess():
                if update_cache:
                    hsvc._py_cache[oid] = o
                return o
            return
        if klass:
            if isinstance(klass, str):
                klass = getattr(ROOT, klass)
            if issubclass(klass, (ROOT.TH1,)):
                return _get_helper(klass, self, self.getHist, oid)
            if issubclass(klass, (ROOT.TGraph,)):
                return _get_helper(klass, self, self.getGraph, oid)
            if issubclass(klass, (ROOT.TTree,)):
                return _get_helper(klass, self, self.getTree, oid)
            raise RuntimeError('unsupported type [%r]'%klass)

        # as we are sentenced to crawl through all these std::vector<str>
        # we might as well update our local cache...
        
        # first update histos
        oids = [n for n in self.getHists() if n not in self._py_cache.keys()]
        for name in oids:
            obj = _get_helper(ROOT.TH1, self, self.getHist, name,
                              update_cache=False)
            if obj:
                # now try with real class
                klass = getattr(ROOT, obj.ClassName())
                obj = _get_helper(klass, self, self.getHist, name)

        # then graphs
        ## FIXME: no 'ITHistSvc::getGraphs' method !!
        ## https://savannah.cern.ch/bugs/index.php?36379
##         oids = [n for n in self.getGraphs() if not n in self._py_cache.keys()]
##         for name in oids:
##             _get_helper(ROOT.TGraph, self, self.getGraph, name)
        
        # finally try ttrees
        oids = [n for n in self.getTrees() if n not in self._py_cache.keys()]
        for name in oids:
            _get_helper(ROOT.TTree, self, self.getTree, name)

        ## all done, crossing fingers
        return self._py_cache[oid]

    ITHistSvc.get = get
    del get

    def getitem(self, oid):
        return self.get(oid)
    ITHistSvc.__getitem__ = getitem
    del getitem
    
    def delitem(self, oid):
        if isinstance(oid, str):
            self.get(oid)
            del self._py_cache[oid]
        assert self.deReg(oid).isSuccess(), \
               "could not remove object [%r]"%oid
        return
    ITHistSvc.__delitem__ = delitem

    def setitem(self, k, v):
        return self.book(k, obj=v)
    ITHistSvc.__setitem__  = setitem
    del setitem

    ## ties some loose ends
    for n in ('Hist', 'Graph', 'Tree'):
        code = """\
def reg%s(self, oid, oid_type=None):
    if not (oid_type is None):
        return self.book(oid,obj=oid_type)
    if ITHistSvc._cpp_reg%s(self,oid).isSuccess():
        # update py_cache
        return self.get(oid)
    err = ''.join(['invalid arguments oid=',repr(oid),' oid_type=',
                   repr(oid_type)])
    raise ValueError(err)
ITHistSvc.reg%s = reg%s
del reg%s""" % (n,n,n,n,n)
        exec (code, globals(),locals())
        pass
    def load(self, oid, oid_type):
        """Helper method to load a given object `oid' from a stream, knowing
        its type. `oid_type' is a string whose value is either:
         - 'hist',  to load any THx and TProfiles
         - 'tree',  to load TTrees
         - 'graph', to load TGraph and TGraphErrors
        """
        _allowed_values = ('hist','tree','graph')
        if oid_type not in _allowed_values:
            raise ValueError(
                'oid_type (=%r) MUST be one of %r'%(oid_type,
                                                    _allowed_values)
                )
        return getattr(self, 'reg%s'%oid_type.capitalize())(oid)
    ITHistSvc.load = load
    del load
    ## --
    
    ## 'dict-onization' of ITHistSvc
    for n in ('__contains__',
              '__iter__',
              '__len__',
              'has_key',
              'items', 'iteritems',
              'iterkeys', 'itervalues',
              'keys',
              'values'):
        code = """\
def %s(self, *args, **kw):
    return self._py_cache.%s(*args,**kw)
ITHistSvc.%s = %s
del %s""" % (n,n,n,n,n)
        exec (code, globals(),locals())
    def pop(self, k):
        obj = self.get(k)
        assert self.deReg(obj).isSuccess(), \
               "could not remove object [%r]"%k
        return obj
    ITHistSvc.pop = pop
    del pop

    def popitem(self):
        k = self.iterkeys().next()
        return (k, self.pop(k))
    ITHistSvc.popitem = popitem
    del popitem
    ## ------------------------------

    ## FIXME: would be nice... but somehow interferes with _py_cache property
##     ## allow the use of the pythonized properties interface
##     def __getattr__( self, attr ):
##         try:                from GaudiPython.Bindings import iProperty
##         except ImportError: from gaudimodule          import iProperty
##         return getattr( iProperty('THistSvc', self), attr )

##     def __setattr__( self, attr, value ):
##         try:                from GaudiPython.Bindings import iProperty
##         except ImportError: from gaudimodule          import iProperty
##         return setattr( iProperty('THistSvc', self), attr, value )

##     ITHistSvc.__getattr__ = __getattr__
##     ITHistSvc.__setattr__ = __setattr__
    return ITHistSvc

### pythonizations for EventStreamInfo
@memoize
def _py_init_EventStreamInfo():
    import cppyy
    # EventStreamInfo bindings from dictionary
    _load_dict( "libEventInfoDict" )

    # retrieve the EventStreamInfo class
    ESI = cppyy.gbl.EventStreamInfo
    # retrieve the PyEventStreamInfo helper class
    PyESI= cppyy.gbl.PyEventStreamInfo
    def run_numbers(self):
        self._run_numbers = PyESI.runNumbers(self)
        return list(self._run_numbers)
    def evt_types(self):
        self._evt_types = PyESI.eventTypes(self)
        return list(self._evt_types)
    def item_list(self):
        self._item_list = PyESI.itemList(self)
        return list(tuple(i) for i in self._item_list)
    def lumi_blocks(self):
        self._lumi_blocks = PyESI.lumiBlockNumbers(self)
        return list(self._lumi_blocks)
    def processing_tags(self):
        self._processing_tags = PyESI.processingTags(self)
        return list(self._processing_tags)
    for fct in ('run_numbers', 'evt_types', 'item_list',
                'lumi_blocks', 'processing_tags'):
        setattr(ESI, fct, locals()[fct])

    return ESI

### pythonizations for EventType
@memoize
def _py_init_EventType():
    import cppyy
    # EventStreamInfo bindings from dictionary
    _load_dict( "libEventInfoDict" )

    # retrieve the EventType class
    cls = cppyy.gbl.EventType
    cls.bit_mask_typecodes = [
        ('IS_DATA','IS_SIMULATION'),    #0
        ('IS_ATLAS', 'IS_TESTBEAM'),    #1
        ('IS_PHYSICS','IS_CALIBRATION'),#2
        ]
    # retrieve the PyEventType class
    py_cls = cppyy.gbl.PyEventType
    def raw_bit_mask(self):
        self._raw_bit_mask = py_cls.bit_mask(self)
        return self._raw_bit_mask
    cls.raw_bit_mask = property(raw_bit_mask)
    def bit_mask(self):
        def decode_bitmask(idx):
            if len(self.raw_bit_mask) <= idx:
                return self.bit_mask_typecodes[idx][0]
            isa_idx = self.raw_bit_mask[idx]
            return self.bit_mask_typecodes[idx][isa_idx]
        bm = map(decode_bitmask,
                 range(len(self.bit_mask_typecodes)))
        return tuple(bm)
    cls.bit_mask = property(bit_mask)
    return cls

### pythonizations for DataLink
@memoize
def _py_init_DataLink():
    return _gen_data_link

### pythonizations for ElementLink
@memoize
def _py_init_ElementLink():
    return _gen_element_link

### pythonizations for ElementLinkVector
@memoize
def _py_init_ElementLinkVector():
    return _gen_elv

### pythonizations for NavigationToken
@memoize
def _py_init_NavigationToken():
    return _gen_navtok

### helper method to easily instantiate DataLink ------------------------------
@memoize
def _gen_data_link(klass, storage_policy=None):
    """helper method to easily instantiate a DataLink class.
    Sensible default for the storage policy is chosen if none given (it usually
    boils down to DataProxyStorage)
    
    @example:
     >>> DLink = PyAthena.DataLink('CompositeParticleContainer')
     >>> cp = DLink()
     >>> cp = DLink('MyStoreGateKey')
    """
    ROOT = _import_ROOT ()
    if isinstance(klass, str):
        klass = getattr(ROOT, klass)
    if storage_policy is None:
        storage_policy = ROOT.DataProxyStorage(klass)
    return ROOT.DataLink(klass, storage_policy)

### helper method to easily instantiate ElementLink ---------------------------
@memoize
def _gen_element_link(klass, storage_policy=None, indexing_policy=None):
    """helper method to easily instantiate an ElementLink class.
    Sensible defaults for the storage and indexing policies are chosen if none
    given (it usually boils down to DataProxyStorage and ForwardIndexingPolicy)
    
    @example:
     >>> CPLink = PyAthena.ElementLink('CompositeParticleContainer')
     >>> cp = CPLink()
     >>> EleLink = PyAthena.ElementLink(PyAthena.ElectronContainer)
     >>> ele = EleLink()
    """
    ROOT = _import_ROOT ()
    if isinstance(klass, str):
        klass = getattr(ROOT, klass)
    #if storage_policy is None:
    #    storage_policy = ROOT.DataProxyStorage(klass)
    #if indexing_policy is None:
    #    indexing_policy = ROOT.ForwardIndexingPolicy(klass)
    #return ROOT.ElementLink(klass, storage_policy, indexing_policy)
    return ROOT.ElementLink(klass)

### helper method to easily instantiate ElementLinkVector ---------------------
@memoize
def _gen_elv(klass, storage_policy=None, indexing_policy=None):
    """helper method to easily instantiate an ElementLinkVector class.
    Sensible defaults for the storage and indexing policies are chosen if none
    given (it usually boils down to DataProxyStorage and ForwardIndexingPolicy)
    
    @example:
     >>> IELV = PyAthena.ElementLinkVector('INavigable4MomentumCollection')
     >>> ielv = IELV()
    """
    ROOT = _import_ROOT ()
    if isinstance(klass, str):
        klass = getattr(ROOT, klass)
    if storage_policy is None:
        storage_policy = ROOT.DataProxyStorage(klass)
    if indexing_policy is None:
        indexing_policy = ROOT.ForwardIndexingPolicy(klass)
    return ROOT.ElementLinkVector(klass, storage_policy, indexing_policy)

### helper method to easily instantiate NavigationToken -----------------------
@memoize
def _gen_navtok(klass, weight_cls=None, hash_cls=None):
    """helper method to easily instantiate a NavigationToken class.
    Sensible default for the weight and hash parameters are chosen if none are
    given
    
    @example:
     >>> cls = PyAthena.NavigationToken('CaloCell')
     >>> token = cls()
    """
    ROOT = _import_ROOT ()
    if isinstance(klass, str):
        klass = getattr(ROOT, klass)
    if weight_cls is None:
        weight_cls = getattr(ROOT, 'NavigationDefaults::DefaultWeight')
    if hash_cls is None:
        hash_cls = getattr(ROOT, 'SG::hash<const %s *>' % (klass.__name__,))
    return ROOT.NavigationToken(klass, weight_cls, hash_cls)

### helper method to pythonize further std::map
def _std_map_pythonize(cls, key_type, value_type):
    def __contains__(self, k):
        return self.find(k) != self.end()
    cls.__contains__ = __contains__

    def __setitem__(self, k, v):
        itr = self.find(k)
        self.insert(itr, self.__class__.value_type(k,v))
        return v
    cls.__setitem__ = __setitem__

    cls.__cxx_getitem__ = cls.__getitem__
    def __getitem__(self, k):
        # python's dict semantics
        if k not in self:
            raise KeyError(k)
        return self.__cxx_getitem__(k)
    cls.__getitem__ = __getitem__

    if not hasattr(cls, '__iter__'):
        def toiter(beg, end):
            while beg != end:
                yield beg.__deref__()
                beg.__preinc__()
            return

        def __iter__(self):
            for i in toiter(self.begin(), self.end()):
                yield i
        cls.__iter__ = __iter__
        
    def keys(self):
        keys = []
        for i in self:
            keys.append(i.first)
        return keys
    cls.keys = keys
    
    def values(self):
        vals = []
        for i in self:
            vals.append(i.first)
        return vals
    cls.values = values
    
    def iterkeys(self):
        for i in self:
            yield i.first
    cls.iterkeys = iterkeys
    
    def itervalues(self):
        for i in self:
            yield i.second
    cls.itervalues = itervalues
    
    def iteritems(self):
        for i in self:
            yield (i.first, i.second)
    cls.iteritems = iteritems
    
    return cls

# -----------------------------------------------------------------------------
## initialize the bindings' registration
def _setup():
    _register = _PyAthenaBindingsCatalog.register
    _register('StoreGateSvc', _py_init_StoreGateSvc)
    _register('StoreGate',    _py_init_StoreGate   )

    _register( 'IncidentSvc', _py_init_IIncidentSvc)
    _register('IIncidentSvc', _py_init_IIncidentSvc)

    _register( 'ClassIDSvc', _py_init_ClassIDSvc)
    _register('IClassIDSvc', _py_init_ClassIDSvc)

    _register( 'THistSvc', _py_init_THistSvc)
    _register('ITHistSvc', _py_init_THistSvc)

    _register('EventStreamInfo',   _py_init_EventStreamInfo)
    _register('EventType',         _py_init_EventType)

    _register('DataLink',          _py_init_DataLink)
    _register('ElementLink',       _py_init_ElementLink)
    _register('ElementLinkVector', _py_init_ElementLinkVector)
    
    pass

## execute method at module import
_setup()

## clean-up
del _setup

