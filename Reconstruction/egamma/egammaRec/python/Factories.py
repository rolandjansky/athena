# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "Factories to instantiate Tools and Algorithms"
__author__ = "Bruno Lenzi"

def addToToolSvc( tool ):
  "addToToolSvc( tool ) --> add tool to ToolSvc"
  from AthenaCommon.AppMgr import ToolSvc
  if not hasattr(ToolSvc, tool.getName()):
    ToolSvc += tool
    print tool
  return tool

def addToTopSequence( alg ):
  "addToTopSequence( alg ) --> add alg to TopSequence"
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  topSequence += alg
  print alg
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
  To instantiate tools (algs) and add them to ToolSvc (TopSequence).
  Allows to set default values to c-tor and call postInit methods, such that the default
  configuration of the tool / alg, including other tools that are used is defined before
  the actual instantiation and can be overridden:
  
  # Definition
  from MyPkg import MyPkgConf
  MyTool1 = ToolFactory(MyPkgConf.MyTool1) # does not set any default
  
  # set default via value and pass a method that will be called after instantiation
  def setPropertyX(tool): tool.propertyX = 'x'
  MyTool2 = ToolFactory(MyPkgConf.MyTool2, postInit=[setPropertyX], name='MyTool2a', PropertyY = 'y')
  
  # default via FcnWrapper or ToolFactory instance, called when the tool is instantiated
  def getNameOfTool2(): return MyTool2().getFullName()
  MyTool3 = ToolFactory(MyPkgConf.MyTool3, 
    Tool1 = MyTool1, # ToolFactory instance, not actual tool instance which will be created later
    NameOfTool2 = FcnWrapper(getNameOfTool2), # could have used FullNameWrapper( MyTool2 )
  ) 
  
  # Tool instantiation. All will be added to ToolSvc
  MyTool1() # create default instance with default values
  MyTool2(propertyY = 'xx') # create default instance overriding default propertyY. At this point setPropertyX will be called
  MyTool3() # create default instance, will try to instantiate MyTool1 (already done, no effect)
  
  tool2a = MyTool2('myTool2a') # create another instance with default properties set above ('x')
  tool3a = MyTool3('myTool3a', Tool1 = MyTool1('myTool1a', propertyA = 'B'),
    NameOfTool2 = 'myTool2a') # create another instance overriding defaults of both properties set above
  """

  def __init__(self, iclass, **defaults ):
    """
    @param iclass Tool / Alg class
    @param defaults default values for configurables, can be overridden at instantiation.
     Special parameters: 
     - preInit: list of functions to be called before tool/alg instantiation, take no arguments
    - preInit: list of functions to be called after tool/alg instantiation, take tool/alg as argument
    - doPrint: print tool/alg after instantiation (default: False)
    - doAdd: add tool (alg) to ToolSvc (TopSequence) (default: True)
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
    doPrint = params.pop('doPrint', False)
    doAdd = params.pop('doAdd', True)
    
    # Call preInit functions
    for fcn in preInit:
      try:
        fcn()
      except:
        print '\nERROR calling preInit function %s on %s instantiation\n' % \
          (fcn.__name__, params['name'])
        raise
    
    # Call FcnWrapper or ToolFactory parameters
    for paramName, value in params.items():
      if isinstance(value, (FcnWrapper, ToolFactory)):
        try:
          params[paramName] = value()
        except:
          print '\nERROR setting property %s :: %s\n' % (params['name'], paramName)
          raise
    
    # Instantiate tool / alg
    try:
      obj = self.iclass(**params)
    except Exception:
      print '\nERROR instantiating %s, args: %s\n' % (self.iclass.__name__, params)
      raise
    
    # Call postInit methods
    for fcn in postInit:
      try:
        fcn(obj)
      except:
        print '\nERROR calling postInit function %s on %s instantiation\n' % \
          (fcn.__name__, params['name'])
        raise
    
    # Add to ToolSvc or TopSequence
    if doAdd:
      self.add(obj)
    # Print
    if doPrint:
      print obj
    return obj  

class ToolFactory( Factory ):
  """ToolFactory: to instantiate tools and add them to TopSequence. See Factory"""
  def add(self, obj):
    addToToolSvc(obj)

class AlgFactory( Factory ):
  """AlgFactory: to instantiate algs and add them to TopSequence. See Factory"""
  def add(self, obj):
    addToTopSequence(obj)

def instantiateAll(module = None):
  """Instantiate all tools/algs defined by Factories in a given module. 
  WARNING: in alphabetical order, not in the order they are defined"""
  return [obj() for obj in vars(module).values() if isinstance(obj, Factory)]

