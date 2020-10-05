# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

__doc__ = "Factories to lazy instantiate Private/Public Tools and Algorithms"
__authors__ = "Bruno Lenzi, Christos Anastopoulos, Jovan Mitrevski"
from AthenaCommon.Logging import logging
from AthenaCommon.Resilience import treatException


def factoriesInfo(log):
    logFactories = logging.getLogger('Factories')
    logFactories.info(log)

# tools


def isAlreadyInToolSvc(name):
    """isAlreadyInToolSvc (name of the tool)
    --> check if the tool with name is already in the service"""
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc, name):
        return True
    else:
        return False


def getFromToolSvc(name):
    """getFromToolSvc (name of the tool)
    --> Get the tool from toolSvc by name """
    from AthenaCommon.AppMgr import ToolSvc
    return getattr(ToolSvc, name)


def addToToolSvc(tool):
    """addToToolSvc(tool)
    --> add tool to ToolSvc"""
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += tool
    return tool

# services


def isAlreadyInServiceMgr(name):
    """isAlreadyInServiceMgr (name of the service)
    --> check if the service with name is already in the manager"""
    from AthenaCommon.AppMgr import ServiceMgr
    if hasattr(ServiceMgr, name):
        return True
    else:
        return False


def getFromServiceMgr(name):
    """getFromServiceMgr (name of the service)
    --> Get the service from manager by name """
    from AthenaCommon.AppMgr import ServiceMgr
    return getattr(ServiceMgr, name)


def addToServiceMgr(service):
    """addToServiceMgr( service )
    --> add service to ServiceMgr"""
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += service
    return service

# algs


def isAlreadyInTopSequence(name):
    """isAlreadyInTopSequence ( mane of the alg )
    --> check if the alg  with name is already in the Alg sequence"""
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    if hasattr(topSequence, name):
        return True
    else:
        return False


def getFromTopSequence(name):
    """getFromTopSequence (name of the alg)
    --> Get the alg  from TopSequence  by name """
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    return getattr(topSequence, name)


def addToTopSequence(alg):
    "addToTopSequence(alg) --> add alg to TopSequence"
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += alg
    return alg


def getPropertyValue(tool, property):
    """getPropertyValue(tool, property)
    --> return the set value or the default value
      of the given property for the given tool or alg"""
    try:
        return getattr(tool, property)
    except AttributeError:
        return tool.getDefaultProperty(property)


class FcnWrapper:
    """A simple wrapper to call a function with no arguments"""

    def __init__(self, fcn=None):
        self.fcn = fcn

    def __call__(self):
        return self.fcn()


class Factory:
    """Factory: base class of ToolFactory and AlgFactory.
    Allows to "lazy" instantiate the configuration of tools/algs/services.
    Properties are set in the c-tor,
    and can include other factories,tools.
    One can also define preInit/postInit methods.
    The end result is that the configuration
    can be defined/formed/composed before
    before its actual instantiation  which happens at demand
    via the () (__call__()) method.
    """

    def __init__(self, iclass, **defaults):
        """
        @param iclass Tool / Alg class
        @param defaults default values for configurables,
        can be overridden at instantiation.
        Special parameters:
        - preInit: list of functions to be called before tool/alg
          instantiation, take no arguments
        - preInit: list of functions to be called after tool/alg
        instantiation, take tool/alg as argument
        - doAdd: add tool (alg) to ToolSvc (TopSequence) (default: True)
        - doPrint: print tool/alg after instantiation (default: False)
        """
        self.iclass = iclass
        self.defaults = defaults

    def copy(self, name, **kw):
        """copy(self, name, **kw)
        --> return a new instance of the factory with new name and defaults"""
        kw['name'] = name
        deflt = self.defaults.copy()
        deflt.update(kw)
        return self.__class__(self.iclass, **deflt)

    def __call__(self, name='', **kw):
        """Call preInit functions, instantiate tool (alg),
        call postInit functions and add to ToolSvc (TopSequence)"""
        params = self.defaults.copy()
        params.update(kw)
        params['name'] = name or params.get('name', self.iclass.__name__)
        del name, kw  # to avoid silly mistakes

        # Get special parameters (or default values)
        # and remove them from dictionary
        preInit = params.pop('preInit', [])
        postInit = params.pop('postInit', [])
        doAdd = params.pop('doAdd', True)
        doPrint = params.pop('doPrint', False)

        # Call preInit functions
        for fcn in preInit:
            try:
                fcn()
            except Exception:
                treatException(
                    'calling preInit function %s on %s instantiation\n'
                    % (fcn.__name__, params['name']))
                raise

        # Instantiate the Factory  parameters or call the Function wrappers.
        # (if they are inside a list -->ToolHandleArray loop over them)
        classes = (FcnWrapper, ToolFactory, PublicToolFactory, ServiceFactory)
        for paramName, value in params.items():
            if isinstance(value, classes) or \
                    (isinstance(value, list) and
                     any(isinstance(v, classes) for v in value)):
                try:
                    params[paramName] = value() \
                        if not isinstance(value, list) else \
                        [v() if isinstance(v, classes) else v for v in value]
                except Exception:
                    treatException('setting property %s :: %s\n' %
                                   (params['name'], paramName))
                    raise

        # Instantiate tool / alg
        try:
            obj = self.iclass(**params)
        except Exception:
            treatException('instantiating %s, args: %s\n' %
                           (self.iclass.__name__, params))
            raise

        # Call postInit methods
        for fcn in postInit:
            try:
                fcn(obj)
            except Exception:
                treatException(
                    'calling postInit function %s on %s instantiation\n' % (
                        fcn.__name__, params['name']))
                raise

        # Add to ToolSvc or TopSequence
        if doAdd:
            self.add(obj)

        # Print
        if doPrint:
            print(obj)

        return obj

    def add(self, obj):
        pass


class ToolFactory(Factory):
    """ToolFactory: to instantiate tools.
    Does not add in ToolSvc (private tools). See Factory"""

    def __init__(self, iclass, **defaults):
        self.iclass = iclass
        self.defaults = {'doAdd': False}
        self.defaults.update(defaults)

    def add(self, obj):
        factoriesInfo(
            "Tool with name ==> %s  is a Private Tool,"
            "will not be added in ToolSvc" % obj.getFullName())
        pass

    def copyPublic(self, name, **kw):
        """copy(self, name, **kw)
        --> return a new instance of the factory
        as a puclic tool with new name and defaults"""
        deflt = self.defaults.copy()
        deflt.update(kw)
        deflt['name'] = name
        deflt['doAdd'] = True
        return PublicToolFactory(self.iclass, **deflt)


class PublicToolFactory(Factory):
    """ToolFactory: to instantiate tools.
    Adds in ToolSvc (public tools). See Factory"""

    def __init__(self, iclass, **defaults):
        self.iclass = iclass
        self.defaults = {'doAdd': True}
        self.defaults.update(defaults)

    def add(self, obj):
        if not isAlreadyInToolSvc(obj.getName()):
            factoriesInfo("Adding new Public Tool ===> %s" % obj.getFullName())
            addToToolSvc(obj)
        else:
            factoriesInfo(
                "Public Tool with name == > % s  already "
                "in ToolSvc using existing instance" % obj.getFullName())


class ServiceFactory(Factory):
    """ServiceFactory: to instantiate services.
    Adds to ServiceMgr. See Factory"""

    def __init__(self, iclass, **defaults):
        self.iclass = iclass
        self.defaults = {'doAdd': True}
        self.defaults.update(defaults)

    def add(self, obj):
        if not isAlreadyInServiceMgr(obj.getName()):
            factoriesInfo("Adding new Service ===> %s" % obj.getFullName())
            addToServiceMgr(obj)
        else:
            factoriesInfo(
                "Service with name ==> %s  already in ServiceMgr,"
                "using existing instance" % obj.getFullName())


class AlgFactory(Factory):
    """AlgFactory: to instantiate algs and
    add them to TopSequence. See Factory"""

    def __init__(self, iclass, **defaults):
        self.iclass = iclass
        self.defaults = {'doAdd': True}
        self.defaults.update(defaults)

    def add(self, obj):
        factoriesInfo("Adding new Algorithm ==> %s " % obj.getFullName())
        addToTopSequence(obj)


def instantiateAll(module=None):
    """Instantiate all tools/algs defined by Factories in a given module.
    WARNING: in alphabetical order, not in the order they are defined"""
    return [obj() for obj in vars(module).values() if isinstance(obj, Factory)]
