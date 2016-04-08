#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Miroslav Nozicka
#
# hltsetup classes
#

# Same includes as 

import hltpopdblib
import hltdumpxmllib
import hltmenu
import sqlutils as sql

import sys
import time                   # for time accounting
import socket                 # for host information
import xml.dom.minidom

from hltdbclasses import DBTable
from hltdbclasses import Column

from hltpopdblib import check_row
from hltpopdblib import insert_row

from hltutils import dbVerbose
from hltutils import traceInsert
from hltutils import get_lastversion
from hltutils import get_row_ids

# from jobasic import Parameter
# from jobasic import Component
# from jobasic import Setup


# Classes Setup, Component, Parameter are copied from jobasic.py and modified

#prog = sql.progressBar(0, limit, 50)
#for i in xrange(0,limit+1):
#    prog.printbar(i)
    

##########################################
#                                        #
#   Basic Parameter Class                #
#                                        #
##########################################
class Parameter(object):
  """An object that defines a parameter for an HLT job description"""
  
  def __init__(self, *args):
    """
    Initializes an object of this type. The input argument list is a
    variable parameter list and is filtered for the the cases which are
    supposed to be covered. At the moment, these are the possibilities:
    1) A triple containing, in order, the following information:
        name: The variable name
        value: The variable value
        op: The variable operation. Can be either 'set' or 'append'
    3) A double containing, in order, the following information. The 'op'
    (operator) becomes 'set' by default in that case.
        name: The variable name
        value: The variable value
    4) A cursor and a parameter identifier (long integer) for reading the
    information from an SQL database as defined in the SQL module
    5) An XML node from where I should extract my information
    """

    if len(args) == 3:
      (name, value, op) = args

    elif len(args) == 2:
      if type(args[0]) == str:
        #simplest initialization, op = 'set'
        (name, value) = args
        op = 'set'
        
      else:
        (name, value, op) = sql.read_parameter(args[0], long(args[1]))

    elif len(args) == 1 and isinstance(args[0], xml.dom.minidom.Node):
      name = args[0].getAttribute('name')
      op = args[0].getAttribute('op')
      value = args[0].getAttribute('value')
      
    else:
      raise SyntaxError, "Unsupported initialization method for Parameter"

    self.name = name
    self.value = str(value)
    if op != 'set' and op != 'append':
      raise SyntaxError, \
        "op can only be `set' or `append', not `%s'" % op
    self.op = op
        
  def __getitem__(self, key):
    """Implements the [] operator, so this looks like a dictionary."""
    if key == 'name': return self.name
    elif key == 'value': return self.value
    elif key == 'op': return self.op
    raise KeyError, "key %s not found in Parameter object" % key

  def __eq__(self, other):
    """Compares myself to another parameter"""
    if self.name == other.name and \
      self.value == other.value and \
      self.op == other.op:
      return True
    return False

  def __ne__(self, other):
    """Compares myself to another parameter"""
    return not self == other

  def validate(self, comp_name, prop):
    """Validates myself against the property given as parameter"""
    if str(prop.value) == str(self.value):
      print '[Information] The parameter %s.%s' % (comp_name, self.name),
      print 'is being actively set, but its value is',
      print 'set to the default in C++'
    native = _native_value(prop.type, self.value)
    if native is None:
      raise ValueError, \
        "Cannot coerce parameter %s.%s ('%s') into a valid %s" % \
        (comp_name, self.name, self.value, prop.type)
    return True

  def py_print(self, cname, ptype):
    """Prints myself in an standardized (python) way"""
    real_op = '='
    if self.op == 'append': real_op = '+='
    if ptype.lower() == 'StringProperty'.lower():
      retval = "%s.%s %s '%s'\n" % (cname, self.name, real_op, self.value)
    else:
      retval = "%s.%s %s %s\n" % (cname, self.name, real_op, self.value)
    return retval

  def xml_node(self, doc):
    """Creates myself, as an XML node"""
    retval = doc.createElement('parameter')
    retval.setAttribute('name', self.name)
    retval.setAttribute('value', str(self.value))
    retval.setAttribute('op', self.op)
    return retval
  
  def sql_insert(self, cursor, verbose=True):
    """Inserts myself into the DB"""
    
    return sql.insert_parameter(cursor, self, verbose)
  
  def _print_(self):
    print "----------------------------------------------------------------------------" \
      "----"
    print "- parameter: name=%s, value=%s, operation=%s" \
      % (self.name, self.value, self.op)


##########################################
#                                        #
#   Basic Component Class                #
#                                        #
##########################################
""" Remarks:
    1. Checking of the table consistency not yet implemented
    2. Flag not yet implemented
"""
class Component(object):
  """Defines an object that describes a job component"""

  def __init__(self, *args):
    
    """
    Initializes an object of this type. The input argument list is a
    variable parameter list and is filtered for the the cases which are
    supposed to be covered. At the moment, these are the possibilities:
    1) A quadruple containing, in order, the following information:
        name: The component name (gaudi name)
        alias: The component alias
        parameter: A list containing the parameters of this component
        topalg: If this component should be a TopAlg
    2) A triple containing, in order, the following information:
        name: The component name (gaudi name), with an optional alias
        attached to it and separated by a '/' character.
        parameter: A list containing the parameters of this component
        topalg: If this component should be a TopAlg
    3) A double containing the parameters of this component in this order:
        name: The component name (gaudi name), with an optional alias
        attached to it and separated by a '/' character.
        parameter: A list containing the parameters of this component
        In this case it is assumed that this component is not a top
        algorithm.
    4) A cursor and a component identifier (long integer) for reading the
    information from an SQL database as defined in the SQL module.
    5) An XML node from where my information will be extracted from
    """
    # Component default parameters
    name = None
    alias = None
    topalg = False
    flag = ''
    version = 1.0
    parameter = []

    if len(args) == 4:
      (name, alias, parameter, topalg) = args
      if len(alias) == 0 or alias == None: alias = name
      parameter = parameter

    elif len(args) == 3:
      alias = ''
      (name, parameter, topalg) = args
      if name.find('/') != -1:
        x = name[0:name.find('/')]
        alias = name[name.find('/')+1:]
        name = x
      if len(alias) == 0 or alias == None: alias = name
      parameter = parameter
        
    elif len(args) == 2:
      if type(args[0]) == str:
        (name, parameter) = args
        alias = ''
        if name.find('/') != -1:
          x = name[0:name.find('/')]
          alias = name[name.find('/')+1:]
          name = x
        topalg = False
        parameter = parameter

      else:
        (name, alias, topalg) = sql.read_component(args[0], long(args[1]))
        parameter = []
        for (par_name, value, op) in \
            sql.read_component_parameter(args[0], long(args[1])):
          parameter.append(Parameter(par_name, value, op))
            
    elif len(args) == 1 and isinstance(args[0], xml.dom.minidom.Node):
      name = args[0].getAttribute('name')
      alias = args[0].getAttribute('alias')
      topalg = int(args[0].getAttribute('topalg'))
      # version = float(args[0].getAttribute('version'))
      # flag = args[0].getAttribute('flag')
      parameter = []
      for node in args[0].getElementsByTagName('parameter'):
        parameter.append(Parameter(node))
          
    else:
      raise SyntaxError, "Unsupported initialization method for Component"

    self.name = name
    self.alias = alias
    if len(alias) == 0: self.alias = self.name
    self.version = version
    self.flag = ''
    self.alias = self.alias.replace('::','__')
    self.topalg = bool(topalg)
    self.parameter = parameter
            
  def __getitem__(self, key):
    """Implements the [] operator, so this looks like a dictionary."""
    if key == 'name': return self.name
    elif key == 'alias': return self.alias
    elif key == 'topalg': return self.topalg
    elif key == 'parameter': return self.parameter
    elif key == 'version': return self.version
    raise KeyError, "key %s not found in Component object" % key

  def __eq__(self, other):
    """Compares myself to another component, disregarding the order in
    which its properties are declared."""
    if self.name == other.name and self.alias == other.alias \
      and self.topalg == other.topalg and \
      len(self.parameter) == len(other.parameter):
      for p in range(0, len(self.parameter)):
        if self.parameter[p] not in other.parameter:
          return False
        if other.parameter[p] not in self.parameter:
          return False
      return True
    return False

  def __ne__(self, other):
    """Compares myself to another component"""
    return not self == other

  def validate(self, s):
    """Validates myself against the source described by the parameter
    given."""
    #check all properties
    for par in self.parameter:
      if par.name.find('.') != -1:
        print '[Warning] Ignoring check of AlgTool property %s.%s' % \
            (self.name, par.name)
      else:
        prop = s.get_property(par.name)
        par.validate(self.name, prop)

    return True

  def py_print(self, source, toolsvc_exists):
    """Prints myself in an standardized (python) way"""
    retval = ''
    
    python_name = self.alias
    gaudi_type = source.gaudi_type
    if python_name.find('::') != -1:
      "This string contains a namespace"
      python_name = python_name.replace('::','__')
    
    #if it is a Tool, just make the "Gaudi Hack", otherwise go normally
    if gaudi_type.lower() == "tool" or gaudi_type.lower() == "algtool":
      if not toolsvc_exists[0]:
        retval += 'ToolSvc = Service( "ToolSvc" )\n'
        toolsvc_exists[0] = True
      retval += "%s = Algorithm( 'ToolSvc.%s' )\n" % \
              (python_name, self.name)
            
    if int(self.topalg):
      if self.alias != self.name:
        retval += 'theApp.TopAlg += [ "%s/%s" ]\n' % \
              (self.name, self.alias)
      else:
        retval += 'theApp.TopAlg += [ "%s" ]\n' % (self.name)
          
    if gaudi_type == "MTService".lower() or gaudi_type == "Service".lower():
      if self.alias != self.name:
        retval += 'theApp.ExtSvc += [ "%s/%s" ]\n' % \
                (self.name, self.alias)
      else:
        retval += 'theApp.ExtSvc += [ "%s" ]\n' % (python_name)

    if gaudi_type == "MTService".lower():
      retval += "%s = Service( \"%s\" )\n"  % (python_name,self.alias)
    else:
      retval += "%s = %s( \"%s\" )\n"  % ( python_name,
                                            gaudi_type.capitalize(),
                                            self.alias )
        
    for p in self.parameter:
      if p.name.find('.') == -1:
        try:
          prop = [i for i in source.property if i.name == p.name][0]
        except IndexError, e:
          print '[Error]', str(e)+': %s.%s' % (python_name, p.name)
          raise
        retval += p.py_print(python_name, prop.type)
      else:
        print '[Warning] Trying to infer type of %s.%s from value' % \
            (python_name, p.name)
        retval += p.py_print(python_name, _infer_type_from_str(p.value))
        
    retval += '\n'
    return retval

  def xml_node(self, doc):
    """Creates myself, as an XML node"""
    retval = doc.createElement('component')
    retval.setAttribute('name', self.name)
    retval.setAttribute('alias', self.alias)
    retval.setAttribute('topalg', str(int(self.topalg)))
    for p in self.parameter: retval.appendChild(p.xml_node(doc))
    #print retval.toprettyxml()
    return retval

  def sql_insert(self, cursor, verbose=True):
    """Inserts myself into the DB"""
    return sql.insert_component(cursor, self, verbose)
  
  def _print_(self):
    print "----------------------------------------------------------------------------" \
      "----"
    print "- component: name=%s, alias=%s, topalg=%s" \
      % (self.name, self.alias, self.topalg)
    for par in self.parameter:
      par._print_()

##########################################
#                                        #
#   Basic Setup Class                    #
#                                        #
##########################################
class Setup(object):
  """Describes an object that defines a job"""
  def __init__(self, *args):
    
    """
    Initializes an object of this type. The input argument list is a
    variable parameter list and is filtered for the the cases which are
    supposed to be covered. At the moment, these are the possibilities:
    1) A quadruple containing, in order, the following information:
      name: The setup name
      version: The setup version number (double, in the format 11.5)
      component: A list with the setup components
      force_dll: A list with the setup DLLs to be forced
    2) A triple containing, in order, the following information:
      name: The setup name
      component: A list with the setup components
      force_dll: A list with the setup DLLs to be forced
      In this case, the version is considered to be empty
    3) A cursor and a component identifier (long integer) for reading the
    information from an SQL database as defined in the SQL module.
    4) An XML node from where I should extract my information 
    """    
    if len(args) == 4:
      (name, version, component, force_dll) = args
      component = component
      force_dll = force_dll
      
    elif len(args) == 3:
      (name, component, force_dll) = args
      version = ''
      component = component
      force_dll = force_dll
      
    elif len(args) == 2:
      #todo: This implementation is clearly optimizeable by using views
      #to cleverly reduce the number of queries to only 2. One for
      #retrieving the components, their names and aliases and a second
      #one for retrieving the components properties all in one shot.
      (name, version) = sql.read_setup(args[0], long(args[1]))
      component = []
      force_dll = []
      for id in sql.read_setup_component_id(args[0], long(args[1])):
        component.append(Component(args[0], id[0]))
      for dll_name in sql.read_setup_force_dll(args[0], long(args[1])):
        force_dll.append(dll_name[0])
          
    elif len(args) == 1 and isinstance(args[0], xml.dom.minidom.Node):
      name = args[0].getAttribute('name')
      version = None
      if args[0].hasAttribute('version'):
        version = args[0].getAttribute('version')
      force_dll = []
      for d in args[0].getElementsByTagName('dll'):
        force_dll.append(d.getAttribute('name'))
      component = []
      for c in args[0].getElementsByTagName('component'):
        component.append(Component(c))
    
    else:
      raise SyntaxError, "Unsupported initialization method for Setup"

    self.name = name
    self.version = version
    self.component = component
    self.force_dll = force_dll

  def __getitem__(self, key):
    """Implements the [] operator, so this looks like a dictionary."""
    if key == 'name': return self.name
    elif key == 'version': return self.version
    elif key == 'component': return self.component
    elif key == 'force_dll': return self.force_dll
    raise KeyError, "key %s not found in Setup object" % key

  def __eq__(self, other):
    """Compares myself to another setup, disregarding the order in which
    the components and forced DLLs are declared. The version is ignored!"""
    if self.name == other.name and \
        len(self.force_dll) == len(other.force_dll) and \
        len(self.component) == len(other.component):
      for f in range(0, len(self.force_dll)):
        if self.force_dll[f] not in other.force_dll:
          return False
        if other.force_dll[f] not in self.force_dll:
          return False
      for c in range(0, len(self.component)):
        if self.component[c] not in other.component:
          return False
        if other.component[c] not in self.component:
          return False
      return True
    return False

  def __ne__(self, other):
      """Compares myself to another setup"""
      return not self == other

  def validate(self, release):
    """Validates this setup against a particular release. In practice this
    means the following:
    1. All components have equivalent sources
    2. The components properties set exist in the relating source
    3. The types of all properties match correctly
    4. The Dll's that have to be forced exist.
    5. Other minor rules?
    """
    for c in self.component:
      s = release.get_source(c.name)
      c.validate(s) #validates component against source
        
    for d in self.force_dll:
      dll = release.get_dll(d)

    #if you survived until here, you are valid!
    return True
              
  def py_print(self, release):
    """Prints myself in an standardized (python) way, getting missing
    python elements from the release. Warning: this is **very** non-optimal
    since you are going to have to download the whole release to be able to
    run this method."""
    
    retval = '# Setup "%s"' % self.name
    if len(self.version) != 0: retval += ', version "%s"' % self.version
    retval += '\n'
    retval += '# automatically generated at %s' % time.asctime()
    retval += '\n\n'

    # Instantiates the ApplicationMgr first
    all_but_theapp = [c for c in self.component if c.name != 'ApplicationMgr']
    the_app = [c for c in self.component if c.name == 'ApplicationMgr'][0]
    retval += 'from AthenaCommon.Logging import *\n'
    retval += 'logLevel=logLevel.upper()\n'
    retval += 'logLevel=logLevel.split(",")\n'
    retval += 'if len(logLevel)==0: logLevel=["INFO", "ERROR"]\n'
    retval += 'if len(logLevel)==1: logLevel.append("ERROR")\n'
    retval += 'exec("log.setLevel(logging.%s)" % logLevel[0])\n'
    retval += 'exec "L2OUTPUTLEVEL = %s" % logLevel[0]\n\n'
    
    retval += 'from AthenaCommon.Include import Include\n'
    retval += 'include = Include(1)\n\n'
    
    retval += 'from gaudimodule import *\n'
    retval += 'include("AthenaCommon/Compat.py", show=0)\n'
    retval += 'theApp = iService("ApplicationMgr")\n'
    
    # Loads all DLLs
    # This is part of a trick to instantiate tools and set their properties
    # inside Gaudi. It is used in the __str__() method of the Component
    # class.
    dll = []
    for c in all_but_theapp:
      new_dll = release.get_source_dll_name(c.name)
      if new_dll not in dll: dll.append(new_dll)
    for d in self.force_dll: 
      if d not in dll: dll.append(d)
    for d in dll:
      if d == 'TrigL2Result': continue
      retval += "theApp.DLLs += [ \"%s\" ]\n" % (d)
        
    # Load the rest
    retval += '\ntheApp = AppMgr(L2OUTPUTLEVEL)\n'
    src = release.get_source('ApplicationMgr')
    for p in the_app.parameter:
      prop = [i for i in src.property if i.name == p.name][0]
      retval += p.py_print('theApp', prop.type)

    retval += '\n'

    toolsvc_exists = [False] #trick, to get a "pass-by-reference"
    for c in all_but_theapp:
      src = release.get_source(c.name)
      retval += c.py_print(src, toolsvc_exists)
    return retval

  def xml_node(self, doc):
    """Creates myself, as an XML node"""
    retval = doc.createElement('setup')
    retval.setAttribute('name', self.name)
    retval.setAttribute('version', str(self.version))
    #if len(self.version) != 0: retval.setAttribute('version', self.version)
    for fd in self.force_dll:
      node = doc.createElement('dll')
      node.setAttribute('name', fd)
      retval.appendChild(node)
    for c in self.component: retval.appendChild(c.xml_node(doc))
    #print retval.toprettyxml()
    return retval

  def sql_insert(self, cursor, verbose=True):
    """Inserts myself into the DB"""
    return sql.insert_setup(cursor, self, verbose)
  
  def writeToXMLFile(self, xmlFileName):
    header = """ <?xml version="1.0" encoding="UTF-8"?> """
    footer = """ """
    fout = open( xmlFileName,  "w" )
    fout.write( header )
    self.writeSeqList(fout, "        ")
    self.writeChainList(fout, "        ")
    fout.write( footer )
    fout.close()
    print "... wrote file %s" % xmlSeqFileName
    

  
  def _print_( self ):
    print "----------------------------------------------------------------------------" \
          "----"
    print "- Setup: menu_name/version         = %s/%s" % (self.name, self.version)
    for comp in self.component:
      comp._print_()


##########################################
#                                        #
#   The DB HLT Parameter                 #
#                                        #
##########################################
class DBHltParameter(DBTable):
  def __init__(self, parents, par):
    # Check number of parents
    if len(parents) == 2:
      (self.setup, self.component) = parents
    elif len(parents) == 3:
      (self.mastertable, self.setup, self.component) = parents
    elif len(parents) == 5:
      (self.mastertable, self.triggerchain, self.triggermenu, self.setup, self.component) = parents
    else:
      raise SyntaxError, "Unsupported initialization method for DBHltParameter"

    self.name = par.name
    self.value = par.value
    self.op = par.op
    self.par = par
  
  def dbInsert(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
    if traceInsert():
      print "TRACE: Prepare to insert HltParameter '%s'" % self.name
    needsNewConnector = False
    needsNewThis = False

    colList = []
    colList.append( Column( "HPA_NAME",   self.name,  "S", True) )
    colList.append( Column( "HPA_OP",     self.op,    "S", True) )
    colList.append( Column( "HPA_VALUE",  self.value, "S", True) )
    if verbose:
      print "Checking Parameter: %s" % self.name
    id = check_row( cursor, self.TableName, self.Praefix, colList )
    if isinstance(id,tuple) : id = id[0]
    self.dbID = id
    
    if id<0:  # does not exist yet
      self.dbID = self.dbInsertThis(cursor, username, timestamp) # force insert
      
    return self.dbID
  
  def dbInsertThis(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
    colList = []
    colList.append( Column( "HPA_NAME",           self.name,  "S",  True ) )
    colList.append( Column( "HPA_OP",             self.op,    "S",  True ) )
    colList.append( Column( "HPA_VALUE",          self.value, "S",  True ) )
    colList.append( Column( "HPA_USERNAME",       username,   "S",  True ) )
    colList.append( Column( "HPA_MODIFIED_TIME",  timestamp,  "T",  True ) )
    colList.append( Column( "HPA_USED",           "0",        "S",  True ) )
    
    # insert_row from hltpopdblib checks the row before insertion anyway
    id = hltpopdblib.insert_row( cursor, self.TableName, self.Praefix, colList )
    if traceInsert():
      print "TRACE: Insert Parameter '%s' ==> %i" % (self.name,id)

    if isinstance(id,tuple) : id = id[0]
    return id
  TableName = "HLT_PARAMETER"
  Praefix = "HPA"

##########################################
#                                        #
#   The DB HLT Component                 #
#                                        #
##########################################
""" Remarks:
      1. HLT_FLAG not yet implemented
"""
class DBHltComponent(DBTable):
  TableName = "HLT_COMPONENT"
  Praefix = "HCP"

  def __init__(self, parents, comp):
    # Check number of parents
    if len(parents) == 1:
      self.setup = parents[0]
    elif len(parents) == 2:
      (self.mastertable, self.setup) = parents
    elif len(parents) == 4:
      (self.mastertable, self.triggerchain, self.triggermenu, self.setup) = parents
    else:
      raise SyntaxError, "Unsupported initialization method for DBHltComponent"
    
    # Assignement of simple members
    self.comp = comp
    self.name = comp.name
    self.alias = comp.alias
    self.version = comp.version
    self.topalg = comp.topalg
    self.flag = comp.flag
    
    # Create parameter list
    self.parameterList = []
    for parameter in comp.parameter:
      self.parameterList.append(DBHltParameter(parents+(self,),parameter))

  def dbInsert(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
    if traceInsert():
        print "TRACE: Prepare to insert Component '%s'" % self.name
    needsNewConnector = False
    needsNewThis = False
    
    # First Insert parameters. DBHltParameter checks itself
    self.parameterIdList = []
    for parameter in self.parameterList:
      self.parameterIdList.append(parameter.dbInsert(cursor, username, timestamp, verbose))
      
    self.parameterIdList.sort()
    
    # Look if there are some somponents in the database with same name and alias
    colList = []
    colList.append( Column( "HCP_NAME",     self.name,    "S" ) )
    colList.append( Column( "HCP_ALIAS",    self.alias,   "S" ) )

    id = get_row_ids( cursor, self.TableName, self.Praefix, colList )
    
    self.dbID = -1
    
    if isinstance(id,tuple) or isinstance(id,list) and len(id) > 0:
      foundID = -1
      for compID in id: # Check parameter list
        self.dbID = compID
        dbparIdList = []
        dbparIdList = self.getParameterIDsFromDB(cursor)
        dbparIdList.sort()
        
        if dbparIdList == self.parameterIdList:
          foundID = compID
          
        if foundID > 0:
          break
 
      
      if foundID > 0: # Component with the same parameter list found 
        self.dbID = foundID
      else: # No component with the same parameter list found
        # Reinitalise column list before getting the las version in te database
        colList = []
        colList.append( Column( "HCP_NAME",     self.name,    "S" ) )
        colList.append( Column( "HCP_ALIAS",    self.alias,   "S" ) )
        self.version = get_lastversion(cursor, self.TableName, self.Praefix, colList) + 1
        self.dbID = self.dbInsertThis(cursor, username, timestamp, verbose) # force insert
        
    else:
      self.dbID = self.dbInsertThis(cursor, username, timestamp, verbose) # force insert

    if self.dbID>0:
      self.insertConnectorCPtoPA(cursor, username, timestamp, verbose)
    else:
      raise SyntaxError, "Component %s/%s ID missing" % (self.name, self.alias)
    
    return self.dbID

  def dbInsertThis(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
    colList = []
    colList.append( Column( "HCP_NAME",           self.name,          "S", True ) )
    colList.append( Column( "HCP_VERSION",        self.version,       "F", True ) )
    colList.append( Column( "HCP_ALIAS",          self.alias,         "S", True ) )
    colList.append( Column( "HCP_TOPALG",         self.topalg,        "I", True ) )
    colList.append( Column( "HCP_FLAG",           "~",                "S", True ) )
    colList.append( Column( "HCP_USERNAME",       username,           "S", True ) )
    colList.append( Column( "HCP_MODIFIED_TIME",  timestamp,          "T", True ) )
    colList.append( Column( "HCP_USED",           "0",                "S", True ) )

    # id = check_row( cursor, self.TableName, self.Praefix, colList )
    # if id<0:
    id = hltpopdblib.insert_row( cursor, self.TableName, self.Praefix, colList )
    if traceInsert():
      print "TRACE: Insert Component '%s' ==> %i" % (self.name,id)
    
    if isinstance(id,tuple) or isinstance(id,list): id = id[0]
    return id
  
  def insertConnectorCPtoPA(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
    for parID in self.parameterIdList:
      cp2paID = self.findConnectorCPtoPA(cursor, parID, verbose)
      if cp2paID <= 0 :
        colList = []
        colList.append( Column( "HCP2PA_COMPONENT_ID",   "%i" % self.dbID,   "I", True ) )
        colList.append( Column( "HCP2PA_PARAMETER_ID",   "%i" % parID,       "I", True ) )
        colList.append( Column( "HCP2PA_USERNAME",       username,           "S", True ) )
        colList.append( Column( "HCP2PA_MODIFIED_TIME",  timestamp,          "T", True ) )
        colList.append( Column( "HCP2PA_USED",           "0",                "S", True ) )
        
        hltpopdblib.insert_row( cursor, "HLT_CP_TO_PA", "HCP2PA", colList )
      
  def findConnectorCPtoPA(self, cursor, parID, verbose=False):
    colList = []
    colList.append( Column( "HCP2PA_COMPONENT_ID",   "%i" % self.dbID,   "I", True ) )
    colList.append( Column( "HCP2PA_PARAMETER_ID",   "%i" % parID,       "I", True ) )
    
    cp2paID = hltpopdblib.check_row(cursor, "HLT_CP_TO_PA", "HCP2PA", colList)
    if isinstance(cp2paID, tuple) or isinstance(cp2paID, list):
      if len(cp2paID) == 1:
        cp2paID = cp2paID[0]
      elif len(cp2paID) == 0:
        cp2paID = -1
      else:
        raise SyntaxError, "duplicated parameter link %s" % cp2paID
    
    return cp2paID
      
  def getParametersFromDB(self, cursor, parIdList = [], verbose=False):
    # parIdList is optional, since it is not changed to the outside may be it is worth of using it. I need to have a lok how to modify it
    parPraefix = "HPA"
    parTableName = "HLT_PARAMETER"
    connPraefix = "HCP2PA"
    connTableName = "HLT_CP_TO_PA"
    
    parUniqueKeys = "%s.%s_ID, %s.%s_NAME, %s.%s_OP, %s.%s_VALUE" % \
    (parTableName, parPraefix, parTableName, parPraefix, parTableName, parPraefix, parTableName, parPraefix)
    
    query = "SELECT DISTINCT %s FROM %s WHERE %s.%s_ID IN (" % (parUniqueKeys, parTableName, parTableName, parPraefix)
    query += "SELECT DISTINCT %s.%s_PARAMETER_ID FROM %s " % (connTableName, connPraefix, connTableName)
    query += "JOIN %s ON (%s.%s_ID = %s.%s_COMPONENT_ID )" % (self.TableName, self.TableName, self.Praefix, connTableName, connPraefix)
    query += "WHERE %s.%s_ID = %d" % (self.TableName, self.Praefix, self.dbID)
    query += ")"
    
    if dbVerbose(): print type(query), query
    
    cursor.execute( query )
    result = cursor.fetchall()
    dbparList = []
    parIdList = []
    
    for dbpar in result:
      name, op, value = dbpar[1:]
      dbparList.append(Parameter(name, value, op))
      parIdList.append(dbpar[0])
    
    return dbparList
    
  def getParameterIDsFromDB(self, cursor, verbose=False):
    # parIdList is optional, since it is not changed to the outside may be it is worth of using it. I need to have a lok how to modify it
    parPraefix = "HPA"
    parTableName = "HLT_PARAMETER"
    connPraefix = "HCP2PA"
    connTableName = "HLT_CP_TO_PA"
    
    parUniqueKeys = "%s.%s_ID" % (parTableName, parPraefix)
    

    #query = "SELECT DISTINCT %s FROM %s AS %s WHERE %s.%s_ID IN (" % (parUniqueKeys, parTableName, parPraefix, parPraefix, parPraefix)
    #query += "SELECT DISTINCT %s.%s_PARAMETER_ID FROM %s AS %s " % (connPraefix, connPraefix, connTableName, connPraefix)
    #query += "JOIN %s AS %s ON (%s.%s_ID = %s.%s_COMPONENT_ID )" % (self.TableName, self.Praefix, self.Praefix, self.Praefix, connPraefix, connPraefix)
    #query += "WHERE %s.%s_ID = %d" % (self.Praefix, self.Praefix, self.dbID)
    #query += ");"
    
    
    query = "SELECT DISTINCT %s FROM %s WHERE %s.%s_ID IN ( " % (parUniqueKeys, parTableName, parTableName, parPraefix)
    query += "SELECT DISTINCT %s.%s_PARAMETER_ID FROM %s " % (connTableName, connPraefix, connTableName)
    query += "JOIN %s ON ( %s.%s_ID = %s.%s_COMPONENT_ID )" % (self.TableName, self.TableName, self.Praefix, connTableName, connPraefix)
    query += "WHERE %s.%s_ID = %d " % (self.TableName, self.Praefix, self.dbID)
    query += ")"
    
    if dbVerbose(): print type(query), query
    
    cursor.execute( query )
    result = cursor.fetchall()
    parIdList = []
    
    for dbpar in result:
      parIdList.append(dbpar[0])
    
    return parIdList





##########################################
#                                        #
#   The DB HLT Force Dll                 #
#                                        #
##########################################
class DBHltForceDll(DBTable):
  def __init__(self, parents, seq):
    self.name = seq
    (self.mastertable, self.hltsetup) = parents

  def dbInsert(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
    if traceInsert():
        print "TRACE: Prepare to insert Force Dll '%s'" % self.name
    needsNewConnector = False
    needsNewThis = False

    colList = []
    colList.append( Column( "HFD_NAME", self.name, "S" ) )
    if self.mastertable.verbose:
        print "Checking Force Dll: %s" % self.name
    id = check_row( cursor, self.TableName, self.Praefix, colList )

    if id<0:  # does not exist yet
      needsNewThis = True
      self.dbID = self.dbInsertThis(cursor, username, timestamp, verbose) # force insert
    
    return self.dbID

  def dbInsertThis(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
    colList = []
    colList.append( Column( "HFD_NAME",          self.name, "S", True ) )
    colList.append( Column( "HFD_VERSION",       "%i" % 1,  "I", True ) )
    colList.append( Column( "HFD_USERNAME",      username,  "S", True ) )
    colList.append( Column( "HFD_MODIFIED_TIME", timestamp, "T", True ) )
    colList.append( Column( "HFD_USED",          "0",       "S", True ) )

    # id = check_row( cursor, self.TableName, self.Praefix, colList )
    # if id<0:
    id = hltpopdblib.insert_row( cursor, self.TableName, self.Praefix, colList )
    if traceInsert():
      print "TRACE: Insert Force Dll '%s' ==> %i" % (self.name,id)
    else:
      if traceInsert():
        print "TRACE: Return existing Force Dll '%s' ==> %i" % (self.name,id)
    return id
  
  TableName = "HLT_FORCE_DLL"
  Praefix = "HFD"


##########################################
#                                        #
#   The DB HLT Setup                     #
#                                        #
##########################################
class DBHltSetup(DBTable):
  #def __init__(self, parents, setup):
    # First Leave out whole parenting stuff
  def __init__(self, setup):
    # Check the number of parents
    #if len(parents) == 1:
      #self.mastertable = parents
    #elif len(parents) == 3:
      #(self.mastertable, self.triggerchain, self.triggermenu) = parents
    #else:
      #raise SyntaxError, "Unsupported initialization method for DBHltSetup"
    # Assign simple members
    self.name = setup.name
    self.version = setup.version
    self.setup = setup
    #self.forcedll = setup.forcedll

    # Assign components
    self.componentList = []
    for component in setup.component:
      parents = ()
      self.componentList.append(DBHltComponent(parents+(self,),component))

  def dbInsert(self, cursor, username="atltrig", timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
    if traceInsert():
      print "TRACE: Prepare to insert Setup '%s'" % self.name
    
    self.componentIdList = []
    for dbcomponent in self.componentList:
      self.componentIdList.append(dbcomponent.dbInsert(cursor, username, timestamp, verbose))
    self.componentIdList.sort()
    
    # Search for components in the DB with same name and alias
    colList = []
    colList.append( Column( "HST_NAME", self.name, "S",  True ) )
    id = get_row_ids( cursor, self.TableName, self.Praefix, colList )
    
    self.dbID = -1
    
    if isinstance(id,tuple) or isinstance(id,list) and len(id) > 0:
      foundID = -1
      for compID in id: # Check component list
        self.dbID = compID
        dbcompIdList = []
        dbcompIdList = self.getComponentIDsFromDB(cursor)
        dbcompIdList.sort()
        
        if dbcompIdList == self.componentIdList:
          foundID = compID
          
        if foundID > 0:
          break
        
      if foundID > 0: # Component with the same component list found 
        self.dbID = foundID
      else: # No component with the same component list found
        # Reinitalise column list before getting the last version from DB
        colList = []
        colList.append( Column( "HST_NAME", self.name, "S", True ) )
        self.version = get_lastversion(cursor, self.TableName, self.Praefix, colList) + 1 # Obtain last Setup version and add 1
        self.dbID = self.dbInsertThis(cursor, username, timestamp, verbose) # force insert
        
    else:
      self.dbID = self.dbInsertThis(cursor, username, timestamp, verbose) # force insert

    if self.dbID>0:
      self.insertConnectorSTtoCP(cursor, username, timestamp, verbose)
    else:
      raise SyntaxError, "Setup %s ID missing" % (self.name)
    
    sql._commit(cursor)
    return self.dbID
  
  def dbInsertThis(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
    colList = []
    colList.append( Column( "HST_NAME",          self.name,     "S", True  ) )
    colList.append( Column( "HST_VERSION",       self.version,  "F", True  ) )
    colList.append( Column( "HST_USERNAME",      username,      "S", False ) )
    colList.append( Column( "HST_MODIFIED_TIME", timestamp,     "T", False ) )
    colList.append( Column( "HST_USED",          "0",           "S", False ) )

    id = hltpopdblib.insert_row( cursor, self.TableName, self.Praefix, colList )
    if isinstance(id,tuple) or isinstance(id,list): id = id[0]
    if traceInsert():
      print "TRACE: Insert Parameter '%s' ==> %i" % (self.name,id)
    else:
      if traceInsert():
        print "TRACE: Return existing Parameter '%s' ==> %i" % (self.name,id)
        
    return id
  
  def insertConnectorSTtoCP(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
    for componentID in self.componentIdList:
      st2cpID = self.findConnectorSTtoCP(cursor, componentID, verbose)
      if st2cpID <= 0 :
        colList = []
        colList.append( Column( "HST2CP_SETUP_ID",      self.dbID,   "I", True ) )
        colList.append( Column( "HST2CP_COMPONENT_ID",  componentID, "I", True ) )
        colList.append( Column( "HST2CP_USERNAME",      username,    "S", True ) )
        colList.append( Column( "HST2CP_MODIFIED_TIME", timestamp,   "T", True ) )
        colList.append( Column( "HST2CP_USED",          "0",         "S", True ) )
        
        hltpopdblib.insert_row( cursor, "HLT_ST_TO_CP", "HST2CP", colList )
  
  def insertConnectorFStoST(self, cursor, username, timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime()), verbose=False):
      colList = []
      colList.append( Column( "HFD2ST_ID",           "%i" % self.dbID, "I" ) )
      ## colList.append( Column( "HFD2ST_FORCE_DLL_ID", "%i" % , "I" ) )
      ## colList.append( Column( "HFD2ST_SETUP_ID", "%i" % , "I" ) )
      hltpopdblib.insert_row( cursor, "HLT_FS_TO_ST", "HFD2FS", colList )
  
  def findConnectorSTtoCP(self, cursor, componentID, verbose=False):
    
    colList = []
    colList.append( Column( "HST2CP_SETUP_ID",      "%i" % self.dbID,   "I", True ) )
    colList.append( Column( "HST2CP_COMPONENT_ID",  "%i" % componentID, "I", True ) )
    
    st2cpID = check_row( cursor, "HLT_ST_TO_CP", "HST2CP", colList )

    if isinstance(st2cpID, tuple) or isinstance(st2cpID,list):
      if len(st2cpID) == 1:
        st2cpID = st2cpID[0]
      elif len(st2cpID) == 0:
        st2cpID = -1
      else:
        raise SyntaxError, "duplicated component link %s" % st2cpID
    
    return st2cpID

  def getComponentsFromDB(self, cursor, componentIdList = [], verbose=False):
    componentList = getSetupComponentsFromDB(self.dbID, cursor, componentIdList, verbose)
    if not isinstance(componentList, list):
      raise SyntaxError, "Wrong type of component list obtained:", type(componentList)
    return componentList


  def getComponentIDsFromDB(self, cursor, verbose=False):
    # compIdList is optional, since it is not changed to the outside may be it is worth of using it. I need to have a lok how to modify it
    compPraefix = "HCP"
    compTableName = "HLT_COMPONENT"
    connPraefix = "HST2CP"
    connTableName = "HLT_ST_TO_CP"
    
    compUniqueKeys = "%s.%s_ID " % (compTableName, compPraefix)
    
    query = "SELECT DISTINCT %s FROM %s WHERE %s.%s_ID IN (" % (compUniqueKeys, compTableName, compTableName, compPraefix)
    query += "SELECT DISTINCT %s.%s_COMPONENT_ID FROM %s " % (connTableName, connPraefix, connTableName)
    query += "JOIN %s ON (%s.%s_ID = %s.%s_SETUP_ID )" % (self.TableName, self.TableName, self.Praefix, connTableName, connPraefix)
    query += "WHERE %s.%s_ID = %d" % (self.TableName, self.Praefix, self.dbID)
    query += ")"
    
    if dbVerbose(): print type(query), query
    
    cursor.execute( query )
    result = cursor.fetchall()
    dbcompIdList = []
    
    for component in result:
      dbcompIdList.append(component[0])
    
    return dbcompIdList

  
  TableName = "HLT_SETUP"
  Praefix = "HST"


def findComponent(cursor, *args):
  TableName = "HLT_COMPONENT"
  Praefix = "HCP"

  colList = []
  if len(args) > 0 and len(args)<= 9:
    colList.append( Column( "HCP_NAME",   args[0], "S" ) )
    if len(args) >= 2:
      colList.append( Column( "HCP_ALIAS",  arg[1], "S" ) )
      if len(args) == 3:
        colList.append( Column( "HCP_VERSION",  arg[2], "I" ) )
  else:
    raise SyntaxError, "Unsupported number of arguments: %d" % len(args)
  
  setupid = check_row( cursor, TableName, Praefix, colList )
  if isinstance(setupid, tuple) : setupid = setupid[0]
    
  return setupid
  
def insertDummyComponent(cursor, name, alias, topalg, username="", timestamp=time.strftime('%Y-%m-%d %H:%M:%S',time.localtime())):
  TableName = "HLT_COMPONENT"
  Praefix = "HCP"

  colList = []
  colList.append( Column( "HCP_NAME",           name,          "S", True ) )
  colList.append( Column( "HCP_ALIAS",          alias,         "S", True ) )
  version = get_lastversion(cursor, TableName, Praefix, colList) + 1

  colList = []
  colList.append( Column( "HCP_NAME",           name,          "S", True ) )
  colList.append( Column( "HCP_VERSION",        version,       "I", True ) )
  colList.append( Column( "HCP_ALIAS",          alias,         "S", True ) )
  colList.append( Column( "HCP_TOPALG",         topalg,        "I", True ) )
  colList.append( Column( "HCP_FLAG",           "~",           "S", True ) )
  colList.append( Column( "HCP_USERNAME",       username,      "S", True ) )
  colList.append( Column( "HCP_MODIFIED_TIME",  timestamp,     "T", True ) )
  colList.append( Column( "HCP_USED",           "0",           "S", True ) )

  setupid = hltpopdblib.insert_row(cursor, TableName, Praefix, colList)
  if isinstance(setupid, tuple) : setupid = setupid[0]
  
  return setupid


def getCompParametersFromDB(compID, cursor, parIdList = [], verbose=False):
  # parIdList is optional, since it is not changed to the outside may be it is worth of using it. I need to have a lok how to modify it
  Praefix = "HCP"
  TableName = "HLT_COMPONENT"
  parPraefix = "HPA"
  parTableName = "HLT_PARAMETER"
  connPraefix = "HCP2PA"
  connTableName = "HLT_CP_TO_PA"
  
  parUniqueKeys = "%s.%s_ID, %s.%s_NAME, %s.%s_OP, %s.%s_VALUE" % \
    (parTableName, parPraefix, parTableName, parPraefix, parTableName, parPraefix, parTableName, parPraefix)
  
  #query = "SELECT DISTINCT %s FROM %s AS %s WHERE %s.%s_ID IN (" % \
    #(parUniqueKeys, parTableName, parPraefix, parPraefix, parPraefix)
  #query += "SELECT DISTINCT %s.%s_PARAMETER_ID FROM %s AS %s " % \
    #(connPraefix, connPraefix, connTableName, connPraefix)
  #query += "JOIN %s AS %s ON (%s.%s_ID = %s.%s_COMPONENT_ID )" % \
    #(TableName, Praefix, Praefix, Praefix, connPraefix, connPraefix)
  #query += "WHERE %s.%s_ID = %d" % \
    #(Praefix, Praefix, compID)
  #query += ");"
  
  query = "SELECT DISTINCT %s FROM %s WHERE %s.%s_ID IN (" % \
    (parUniqueKeys, parTableName, parTableName, parPraefix)
  query += "SELECT DISTINCT %s.%s_PARAMETER_ID FROM %s " % \
    (connTableName, connPraefix, connTableName)
  query += "JOIN %s ON (%s.%s_ID = %s.%s_COMPONENT_ID )" % \
    (TableName, TableName, Praefix, connTableName, connPraefix)
  query += "WHERE %s.%s_ID = %d" % \
    (TableName, Praefix, compID)
  query += ")"
  
  if dbVerbose(): print type(query), query
  
  cursor.execute( query )
  result = cursor.fetchall()
  dbparList = []
  parIdList = []
  
  for dbpar in result:
    name, op, value = dbpar[1:]
    dbparList.append(Parameter(name, value, op))
    parIdList.append(dbpar[0])
  
  return dbparList


def getSetupComponentsFromDB(setupID, cursor, componentIdList = [], verbose=False):
  # compIdList is optional, since it is not changed to the outside may be it is worth of using it. I need to have a lok how to modify it
  Praefix = "HST"
  TableName = "HLT_SETUP"
  compPraefix = "HCP"
  compTableName = "HLT_COMPONENT"
  connPraefix = "HST2CP"
  connTableName = "HLT_ST_TO_CP"
  
  compUniqueKeys = ", ".join([ "%s.%s_%s" % (compPraefix, compPraefix, x) for x in ["ID", "NAME", "ALIAS", "TOPALG"] ])

  query = "SELECT DISTINCT %s FROM %s AS %s WHERE %s.%s_ID IN (" % \
    (compUniqueKeys, compTableName, compPraefix, compPraefix, compPraefix)
  query += "SELECT DISTINCT %s.%s_COMPONENT_ID FROM %s AS %s " % \
    (connPraefix, connPraefix, connTableName, connPraefix)
  query += "JOIN %s AS %s ON (%s.%s_ID = %s.%s_SETUP_ID )" % \
    (TableName, Praefix, Praefix, Praefix, connPraefix, connPraefix)
  query += " WHERE %s.%s_ID = %d" % \
    (Praefix, Praefix, setupID)
  query += ")"
  
  if dbVerbose(): print type(query), query
  
  sql._run_single( cursor, query )
  result = cursor.fetchall()
  dbcompList = []
  componentIdList = []
  
  for dbcomp in result:
    componentIdList.append(dbcomp[0])
    name, alias, topalg = dbcomp[1:]
    parIdList = []
    parameterList = getCompParametersFromDB(dbcomp[0], cursor, parIdList, verbose)
    dbcompList.append(Component(name, alias, parameterList, topalg))
  
  return dbcompList
  
def getSetupFromDB(cursor, *args):
  setupID = -1
  name = ''
  version = 0
  componentList = []
  
  TableName = "HLT_SETUP"
  Praefix = "HST"
  # Check the input
  if len(args) == 1:
    if isinstance(args[0],int) or isinstance(args[0],long):
      setupID = args[0]
    else:
      raise SyntaxError, "Unsupported type of argument:", type(args[0])
  elif len(args) == 2: # unique ID's name and version supplied
    if isinstance(args[0],str):
      name = args[0]
      version = float(args[1])
    elif isinstance(args[1],str):
      name = args[1]
      version = float(args[0])
    else:
      raise SyntaxError, "Unsupported method"
    
    colList = []
    colList.append( Column( "HST_NAME",    name,    "S",  True ) )
    colList.append( Column( "HST_VERSION", version, "S",  True ) )

    setupID = hltpopdblib.check_row( cursor, TableName, Praefix, colList )
    if setupID < 0:
      print "Warning: Setup: %s version: %f not found in table HLT_SETUP" % (name, version)
      return -1
  else:
    raise SyntaxError, "Unsupported method"

  SetupUniqueKeys = "%s.%s_ID, %s.%s_NAME, %s.%s_VERSION " % \
    (TableName, Praefix, TableName, Praefix, TableName, Praefix)
  
  query = "SELECT DISTINCT %s FROM %s WHERE %s.%s_ID = %d" % \
    (SetupUniqueKeys, TableName, TableName, Praefix, setupID)
  
  if dbVerbose(): print type(query), query
  
  cursor.execute( query )
  result = cursor.fetchall()
  
  # The result length can be only 1 or 0
  if len(result) == 0:
    print "Warning: Setup ID: %d not found in table HLT_SETUP" % setupID
    return -1
  
  elif len(result) == 1:
    for dbsetup in result:
      name, version = dbsetup[1:]
      setupID = dbsetup[0]
      
  else:
    raise SyntaxError, "Multiple Setups found - Database data corrupted"
  
  force_dll = []
  
  componentList = getSetupComponentsFromDB(setupID, cursor)
  setup = Setup(name, version, componentList, force_dll)
  
  return setup

def getL2SetupIDs(cursor, menuID):
  htcTablename = "HLT_TRIGGER_CHAIN"
  htcPraefix = "HTC"
  connTableName = "HLT_TM_TO_TC"
  connPraefix = "HTM2TC"
  joincriteria = " (%s.%s_ID = %s.%s_TRIGGER_CHAIN_ID) " % (htcPraefix, htcPraefix, connPraefix, connPraefix)
  criteria = "%s.%s_TRIGGER_MENU_ID = %i AND %s.%s_L2_OR_EF = 'L2'" % (connPraefix, connPraefix, menuID, htcPraefix, htcPraefix)
  outputVar = "%s.%s_SETUP_ID" % (htcPraefix, htcPraefix)
  
  query = "SELECT DISTINCT %s FROM \n" % (outputVar)
  query += "%s AS %s \n" % (connTableName, connPraefix)
  query += "JOIN %s AS %s \n" % (htcTablename, htcPraefix)
  query += "ON %s\n" % (joincriteria)
  query += "WHERE %s" % criteria
  
  if dbVerbose(): print type(query), query
  
  sql._run_single( cursor, query )
  result = cursor.fetchall()
  
  if isinstance(result, tuple) or isinstance(result, list):
    if len(result) == 1:
      result = result[0]
      if isinstance(result, list) or isinstance(result, tuple):
        if len(result) == 1:
          result = result[0]
        elif len(result) == 0:
          raise SyntaxError, "No EF ID found"
        else:
          raise SyntaxError, "Multiple EF ID's found", result
      elif isinstance(result, long) or isinstance(result, int):
        result = result
      else:
        raise SyntaxError, "Unsupported result type:", type(result)
    else:
      raise SyntaxError, "No EF ID found"    
    
    if result <=0 :
      print "Warning:", "EF ID:", result
  return result

def getEFSetupIDs(cursor, menuID):
  htcTablename = "HLT_TRIGGER_CHAIN"
  htcPraefix = "HTC"
  connTableName = "HLT_TM_TO_TC"
  connPraefix = "HTM2TC"
  joincriteria = " (%s.%s_ID = %s.%s_TRIGGER_CHAIN_ID) " % (htcPraefix, htcPraefix, connPraefix, connPraefix)
  criteria = "%s.%s_TRIGGER_MENU_ID = %i AND %s.%s_L2_OR_EF = 'EF'" % (connPraefix, connPraefix, menuID, htcPraefix, htcPraefix)
  outputVar = "%s.%s_SETUP_ID" % (htcPraefix, htcPraefix)
  
  query = "SELECT DISTINCT %s FROM \n" % (outputVar)
  query += "%s AS %s \n" % (connTableName, connPraefix)
  query += "JOIN %s AS %s \n" % (htcTablename, htcPraefix)
  query += "ON %s\n" % (joincriteria)
  query += "WHERE %s" % criteria
  
  if dbVerbose(): print type(query), query
  
  sql._run_single( cursor, query )
  result = cursor.fetchall()
  
  if isinstance(result, tuple) or isinstance(result, list):
    if len(result) == 1:
      result = result[0]
      if isinstance(result, list) or isinstance(result, tuple):
        if len(result) == 1:
          result = result[0]
        elif len(result) == 0:
          raise SyntaxError, "No EF ID found"
        else:
          raise SyntaxError, "Multiple EF ID's found", result
      elif isinstance(result, long) or isinstance(result, int):
        result = result
      else:
        raise SyntaxError, "Unsupported result type:", type(result)
    else:
      raise SyntaxError, "No EF ID found"    
    
    if result <=0 :
      print "Warning:", "EF ID:", result
  return result

















