# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "Factories to lazy instantiate Private/Public Tools and Algorithms"
__authors__ = "Bruno Lenzi, Christos Anastopoulos, Jovan Mitrevski"
from AthenaCommon.Logging import logging
from AthenaCommon.Resilience import treatException

def factoriesInfo ( log):
  logFactories = logging.getLogger( 'Factories' )
  logFactories.info(log)        

def isAlreadyInToolSvc( name ):
  "isAlreadyInToolSvc ( mane of the tool ) --> check if the tool with name is already in the service"
  from AthenaCommon.AppMgr import ToolSvc
  if hasattr(ToolSvc, name):
    return True
  else:
    return False 

def getFromToolSvc( name ):
  "getFromToolSvc ( name of the tool ) --> Get the tool from toolSvc by name "
  from AthenaCommon.AppMgr import ToolSvc
  return getattr (ToolSvc,name)
  
def addToToolSvc( tool ):
  "addToToolSvc( tool ) --> add tool to ToolSvc"
  from AthenaCommon.AppMgr import ToolSvc
  ToolSvc += tool
  return tool

def isAlreadyInTopSequence( name ):
  "isAlreadyInTopSequence ( mane of the alg ) --> check if the alg  with name is already in the Alg sequence"
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  if hasattr(topSequence, name):
    return True
  else:
    return False 

def getFromTopSequence( name ):
  "getFromTopSequence ( name of the alg ) --> Get the alg  from TopSequence  by name "
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  return getattr (topSequence,name)

def addToTopSequence( alg ):
  "addToTopSequence( alg ) --> add alg to TopSequence"
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  topSequence += alg
  return alg

def getPropertyValue(tool, property):
  """getPropertyValue( tool, property) --> return the set value or the default value
  of the given property for the given tool or alg"""
  try:
    return getattr(tool, property)
  except AttributeError:
    return tool.getDefaultProperty(property)

class FcnWrapper:
  """A simple wrapper to call a function with no arguments"""
  def __init__(self, fcn = None):
    self.fcn = fcn
  
  def __call__(self):
    return self.fcn()

class FullNameWrapper( FcnWrapper ):
  """FcnWrapper to instantiate tool and return tool.getFullName()
  usage: FullNameWrapper( ToolFactory(MyPkgConf.MyTool) )
  """
  def __call__(self):
    return self.fcn().getFullName()

class Factory:
  """Factory: base class of ToolFactory and AlgFactory.
  Allows to "lazy" instantiate tools (algs) and add them to ToolSvc (TopSequence) if needed.
  Allows to set properties in the c-tor , including other factories,tools and also define preInit/postInit methods.
  The end result is that the configuration can be defined/formed/composed before the actual instantiation  
  which happens when needed, i.e at demand  via () (__call__())  method.
  
  # Definition
  from MyPkg import MyPkgConf
  MyTool1 = ToolFactory(MyPkgConf.MyTool1) # does not set any default
  
  # set default via value and pass a method that will be called after instantiation
  def setPropertyX(tool): tool.propertyX = 'x'
  MyTool2 = ToolFactory(MyPkgConf.MyTool2, postInit=[setPropertyX], name='MyTool2a', PropertyY = 'y')
  
  # default via FcnWrapper or ToolFactory instance, called when the tool is instantiated
  def getNameOfTool2(): return MyTool2().getFullName()
  MyTool3 = ToolFactory(MyPkgConf.MyTool3, 
    Tool1 = MyTool1, # ToolFactory instance, not actual tool instance (the tool intantiated when needed)
    NameOfTool2 = FcnWrapper(getNameOfTool2), # could have used FullNameWrapper( MyTool2 )
  ) 
  
  # Actual tool instantiation at demand
  MyTool1() # create tool instance with default values
  MyTool2(propertyY = 'xx') # create a tool instance overriding default propertyY. At this point setPropertyX will be called
  MyTool3() # create default instance, will all try to instantiate MyTool1 (effect depends on Private/Public usage)
  
  tool2a = MyTool2('myTool2a') # create another factory instance with the same properties as MyTool2 above ('x')
  tool3a = MyTool3('myTool3a', Tool1 = MyTool1('myTool1a', propertyA = 'B'),NameOfTool2 = 'myTool2a') 
  # create another factory instance overriding  both properties set above for MyTool3
  """

  def __init__(self, iclass, **defaults ):
    """
    @param iclass Tool / Alg class
    @param defaults default values for configurables, can be overridden at instantiation.
     Special parameters: 
    - preInit: list of functions to be called before tool/alg instantiation, take no arguments
    - preInit: list of functions to be called after tool/alg instantiation, take tool/alg as argument
    - doAdd: add tool (alg) to ToolSvc (TopSequence) (default: True)
    - doPrint: print tool/alg after instantiation (default: False)
    """
    self.iclass = iclass
    self.defaults = defaults
  
  def copy(self, name, **kw):
    "copy(self, name, **kw) --> return a new instance of the factory with new name and defaults"
    kw['name'] = name
    return self.__class__(self.iclass, **dict(self.defaults, **kw) )

  def __call__(self, name = '', **kw ):
    """Call preInit functions, instantiate tool (alg), call postInit functions and add
       to ToolSvc (TopSequence)"""
    params = dict(self.defaults, **kw)
    params['name'] = name or params.get('name', self.iclass.__name__)
    del name, kw # to avoid silly mistakes    

    # Get special parameters (or default values) and remove them from dictionary
    preInit = params.pop('preInit', [])
    postInit = params.pop('postInit', [])
    doAdd = params.pop('doAdd', True)
    doPrint = params.pop('doPrint', False)  
    
    # Call preInit functions
    for fcn in preInit:
      try:
        fcn()
      except:
       treatException('calling preInit function %s on %s instantiation\n' %  (fcn.__name__, params['name']))
       raise
    
    # Call FcnWrapper or ToolFactory parameters 
    # (or if they are inside a list, for ToolHandleArray)
    classes = (FcnWrapper, ToolFactory, PublicToolFactory)
    for paramName, value in params.items():
      if isinstance(value, classes) or \
        (isinstance(value, list) and any(isinstance(v, classes) for v in value) ):
        try:
          params[paramName] = value() if not isinstance(value, list) else \
            [v() if isinstance(v, classes) else v for v in value]
        except:
          treatException('setting property %s :: %s\n' % (params['name'], paramName))
          raise
    
    # Instantiate tool / alg
    try:
      obj = self.iclass(**params)
    except Exception:
      treatException( 'instantiating %s, args: %s\n' % (self.iclass.__name__, params))
      raise
    
    # Call postInit methods
    for fcn in postInit:
      try:
        fcn(obj)
      except:
        treatException('calling postInit function %s on %s instantiation\n' % (fcn.__name__, params['name']))
        raise    

    # Add to ToolSvc or TopSequence
    if doAdd:
      self.add(obj)
    
    # Print, perhaps at some point could use a functio so as to override the behavioue
    if doPrint:
      print obj
    
    return obj  

  def add(self,obj):
    pass

class ToolFactory( Factory ):
  """ToolFactory: to instantiate tools. Does not add in ToolSvc (private tools). See Factory"""

  def __init__(self, iclass, **defaults ):
    self.iclass = iclass
    self.defaults = dict({'doAdd': False}, **defaults)

  def add(self, obj):
      factoriesInfo("Tool with name ==> %s  will not be added in ToolSvc, use PublicToolFactory for public tools" %  obj.getFullName() )
      pass

  def copyPublic(self, name, **kw):
    "copy(self, name, **kw) --> return a new instance of the factory as a puclic tool with new name and defaults"

    deflt = dict(self.defaults, **kw)

    deflt['name'] = name
    deflt['doAdd'] = True

    return PublicToolFactory(self.iclass, **deflt )


class PublicToolFactory( Factory ):
  """ToolFactory: to instantiate tools. Adds in ToolSvc (public tools). See Factory"""

  def __init__(self, iclass, **defaults ):
    self.iclass = iclass
    self.defaults = dict({'doAdd': True}, **defaults)

  def add(self, obj):
    if not isAlreadyInToolSvc(obj.getName()):
      factoriesInfo("Adding new Tool ===> %s" % obj.getFullName())
      addToToolSvc(obj)
    else :
      factoriesInfo("Tool with name ==> %s  already in ToolSvc, use existing instance" %  obj.getFullName() )

class AlgFactory( Factory ):
  """AlgFactory: to instantiate algs and add them to TopSequence. See Factory"""
  def __init__(self, iclass, **defaults ):
    self.iclass = iclass
    self.defaults = dict({'doAdd': True}, **defaults)
    
  def add(self, obj):
    factoriesInfo("Adding new Algorithm ==> %s " %obj.getFullName())
    addToTopSequence(obj)

def instantiateAll(module = None):
  """Instantiate all tools/algs defined by Factories in a given module. 
  WARNING: in alphabetical order, not in the order they are defined"""
  return [obj() for obj in vars(module).values() if isinstance(obj, Factory)]

