# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os, inspect
from past.builtins import execfile
from PyJobTransformsCore.trfutil import find_joboptions
from PyJobTransformsCore.trferr import JobOptionsNotFoundError
from PyJobTransformsCore.TransformLogger import TransformLogger
NO_EVSEL    = -99
MONTECARLO  = 1

ExcludedFiles = set()

def cleanUpEnv( envDict ):
    """Private variables, builtin objects and modules are removed from envDict"""
    for var, obj in envDict.items():
        if var.startswith( '_' ) or inspect.isbuiltin( obj ) or inspect.ismodule( obj ):
            envDict.pop( var )

def supplementEnv( envDict ):
    """Supplement envDict with some elements from this module and several AthenaCommon modules.
    Private variables, builtin objects and modules are omitted."""
    envDict.update( {
        'theApp' : theApp ,
        'Service' : Service ,
        'Algorithm' : Algorithm ,
        'MONTECARLO' : MONTECARLO ,
        'NO_EVSEL'   : NO_EVSEL ,
        'AuditorSvc' : lambda : Service("AuditorSvc") ,
        'MessageSvc' : lambda : Service("MessageSvc") } )
    # add some more stuff to the environment
    for athmod in [ 'SystemOfUnits', 'PhysicalConstants', 'Constants' ]:
        mod = __import__( 'AthenaCommon.' + athmod, globals(), locals(), ['*'] )
        for var, obj in mod.__dict__.iteritems():
            if var.startswith( '__' ) or inspect.isbuiltin( obj ) or inspect.ismodule( obj ):
                continue
            envDict[var] = obj

def Service(name):
    return theApp.service(name)

def Algorithm(name):
    return theApp.algorithm(name)

def include( filename ):
    """execute the given filename in a given environment."""
    if filename in ExcludedFiles:
        return
    if os.path.isabs( filename ):
        full = filename
    else:
        full = find_joboptions( filename )
        if not full:
            raise JobOptionsNotFoundError( filename )
    try:
        _e = include.env
    except AttributeError:
        _e = {}
    execfile( full, _e )

def exclude( filename ):
    ExcludedFiles.add( filename )

# for backward compatibility
include.block = exclude


class FakeProperty:
    def __init__(self,name,type):
        self.name = name
        self.type= type

    def __str__(self):
        me = [ '%s(\"%s\")' % (self.type,self.name) ]
        for n,v in self.__dict__.items():
            if not n.startswith('__') and n != 'name' and n != 'type':
                me += [ "   %s=%s" % (n,v) ]
        return os.linesep.join(me)

    def __setattr__(self,name,value):
        self.__dict__[name] = value
        # for AlgTools add each algtool
        if name == 'AlgTools':
            for tool in value:
                toolname = os.path.basename(tool)
                self.__dict__[toolname] = FakeProperty(tool,'AlgTool')

    def __getattr__(self,name):
        # default is needed for += (only for lists, hopefully...)
        setattr(self,name,[])
        return getattr(self,name)


class FakeAppMgr(TransformLogger):
    def __init__(self, outputLevel = 3, jobOptions = None ):
        TransformLogger.__init__(self,"FakeAppMgr")
        self.__dict__['_setupValue'] = None
        self.__dict__['_nEvents'] = 0
        self.__dict__['_propertyRepository'] = {}

    def __getattr__(self,name):
        try:
            return object.__getattr__(self,name)
        except AttributeError:
            # catch += when member does not exist yet (only for lists)
            self.__dict__[name] = []
            return self.__dict__[name]

    def __str__(self):
        me = []
        for n,v in self.__dict__.items():
            if not n.startswith('_') and not callable(v): me += [ "theApp.%s = %s" % (n,v) ]
        me += [ 'theApp.Algorithms = %s' % self.algorithmDict().keys() ,
                'theApp.Services = %s' % self.serviceDict().keys() ]
        return os.linesep.join(me)

    def exeJobOptions( self, filename, env ):
        """Execute jobOptions file <filename> in environment <env>"""
        # prepare environment
        env[ 'include' ] = include
        supplementEnv( env )
        include.env = env
        include( filename )
        cleanUpEnv( env )

    def setup(self,value=NO_EVSEL):
        if value == NO_EVSEL:
            self.logger().error( "use of theApp.setup( NO_EVSEL ) is deprecated" )
            self.logger().error( "use 'theApp.EvtSel = \"NONE\"' instead" )
        elif value == MONTECARLO:
            self.logger().error( "use of theApp.setup( MONTECARLO ) is deprecated" )
            self.logger().error( "use 'include( \"AthenaCommon/Atlas_Gen.UnixStandardJob.py\" )' instead" )
        else:
            self.__dict__['_setupValue'] = value

    def run(self,nEvents=None):
        self._nEvents = nEvents

    def propertyDict(self,type):
        l = self._propertyRepository.get(type)
        if not l:
            l = {}
            self._propertyRepository[type] = l
        return l

    def serviceDict(self):
        return self.propertyDict('Service')

    def algorithmDict(self):
        return self.propertyDict('Algorithm')

    def property(self,name,type):
        l = self.propertyDict(type)
        p = l.get(name)
        if p is None:
            p = FakeProperty(name,type)
            l[name] = p
        return p

    def service(self,name):
        return self.property(name,"Service")

    def algorithm(self,name):
        return self.property(name,"Algorithm")


theApp = FakeAppMgr()
