#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# $Id: jobasic.py,v 1.1.1.1 2006-07-31 15:09:44 stelzer Exp $
# Created by Andre DOS ANJOS <Andre.dos.Anjos@cern.ch>, 21-Mar-2006

# Defines a number of primitives that describe an HLT job and release
# capabilites. You can use them to describe your job and interact with
# the SQL or XML capabilities of this library.

# Todos:
# =====
# 1. Improve primitive initialization by using keyword-style arguments instead
# of anonymous ones. This can be done by doing something like:
# def __init__(self, **kwargs):
#   ...
# Instead of:
# def __init__(self, *args):
#   ...
# In python. Inside the __init__() method, checking for the initialization
# sub-procedure is the same as checking for keywords in the kwargs
# dictionary. The presence of certain keywords trigger the right construction.

import sqlutils as sql
from copy import deepcopy
import re
import xml.dom.minidom
import time

def _xml2py(f):
    """Transforms the XML string f into a normal python string"""
    f = str(f) #takes out unicode problems
    f = f.replace('&quot;', '"')
    f = f.replace('&lt;', '<')
    f = f.replace('&gt;', '>')
    return f

def _stdtype (t):
    "Transforms the given type string into one of the allowed types"
    save_type = t
    t = t.lower()
    t = t.lower().replace(' ','').replace('static', '', 1)
    t = t.replace('std::', '', 1)
    
    if t == "string" or t == "StringProperty".lower() \
           or t == "pattern_store":
        return "StringProperty"
    
    elif t == "int" or t == "long" or t == "unsigned" or \
             t == "unsignedint" or t == "longint" or \
             t == "unsignedshort" or t == "short" or \
             t == "uint32_t" or \
             t == "SimpleProperty<int>".lower() or \
             t == "IntegerProperty".lower() or \
             t == "UnsignedIntegerProperty".lower():
        return "IntegerProperty"

    elif t == "double" or t == "float" or \
             t == "FloatProperty".lower() or \
             t == "DoubleProperty".lower():
        return "DoubleProperty"

    elif t == "bool" or t == "BooleanProperty".lower():
        return "BooleanProperty"

    elif t == "vector<string>" or \
             t == "StringArrayProperty".lower() or \
             t == "CommandVector".lower() or \
             t == "vector<tool_key>".lower() or \
             t == "StringVector".lower() or \
             t == "VStrings".lower():
        return "StringArrayProperty"

    elif t == "vector<int>" or t == "vector<uint32_t>" or \
             t == "vector<unsignedint>" or \
             t == "vector<unsigned>" or \
             t == "IntegerArrayProperty".lower() or \
             t == "UnsignedLongArrayProperty".lower():
        return "IntegerArrayProperty"

    elif t == "vector<double>" or t == "vector<float>" or \
             t == "FloatArrayProperty".lower() or \
             t == "DoubleArrayProperty".lower():
        return "DoubleArrayProperty"

    elif t == "vector<bool>" or \
             t == "BooleanArrayProperty".lower():
        return "BooleanArrayProperty"

    elif t == "unknown" or t == "thefirst" or t == "where" or \
             t == 'UnknownTypeProperty'.lower():
        return "UnknownTypeProperty"

    elif t == "enum" or t == "enumeration" or \
             t == "enumerationproperty":
        return "EnumerationProperty"

    raise TypeError, "`%s' is not a valid type" % save_type

def _native_value (t, value):
    "Coerces value (which is a string) to a resembling python type"
    if type(value) != str:
        if type(value) == list or type(value) == dict:
            value = deepcopy(value)
            return value

    if t == "StringProperty":
        re_value = re.match('(std::)?string\s*\(\s*"(?P<value>.+)"\s*\)', str(value))
        if re_value != None:
            #print "Changing %s by %s" % (value, re_value.group('value'))
            value = re_value.group('value')
        return str(value).strip('\'"')
    
    elif t == "IntegerProperty":
	if value.lower() == 'false': return 0
	elif value.lower() == 'true': return 1
        try:
            return int(value)
        except ValueError:
            return None

    elif t == "DoubleProperty":
        try:
            return float(value)
        except ValueError:
            return None

    elif t == "BooleanProperty":
        if type(value) == str and \
            (value.strip() == '0' or value.strip().lower() == 'false'):
             return False
        return bool(value)

    elif t == "StringArrayProperty":
        if str(value).find('[') == -1: return None
        if re.match('\s*\[\s*\]\s*', value) != None: return []
        value = str(value).replace(' ','').strip('[] ').split(',')
        return [_native_value("StringProperty", v) for v in value]

    elif t == "IntegerArrayProperty":
        if str(value).find('[') == -1: return None
        if re.match('\s*\[\s*\]\s*', value) != None: return []
        value = str(value).replace(' ','').strip('[] ').split(',')
        return [_native_value("IntegerProperty", v) for v in value]

    elif t == "DoubleArrayProperty":
        if str(value).find('[') == -1: return None
        if re.match('\s*\[\s*\]\s*', value) != None: return []
        value = str(value).replace(' ','').strip('[] ').split(',')
        return [_native_value("DoubleProperty", v) for v in value]

    elif t == "BooleanArrayProperty":
        if str(value).find('[') == -1: return None
        if re.match('\s*\[\s*\]\s*', value) != None: return []
        value = str(value).replace(' ','').strip('[] ').split(',')
        return [_native_value("BooleanProperty", v) for v in value]

    elif t == "UnknownTypeProperty": return None

    elif t == "EnumerationProperty": return value

    raise TypeError, "`%s' is not a valid type" % t

def _infer(v):
    """Infers the C++ type of the python-variable v and coerces its values to
    that type (if it is a list)."""

    if type(v) == int or type(v) == long: return ('IntegerProperty', v)
    elif type(v) == str or type(v) == unicode: return ('StringProperty', str(v))
    elif type(v) == float: return ('DoubleProperty', v)
    elif type(v) == list:
        if len(v) == 0:
            raise SyntaxError, 'Cannot infer type from empty python list. Use another initialization method for Parameter.'
        
        (unit_type, value) = _infer(v[0])
        par_type = unit_type.replace('Property', 'ArrayProperty')
        return (par_type, [_native_value(unit_type, u) for u in v])

    raise TypeError, 'I cannot infer the type of "%s"' % v

def _infer_type_from_str(v):
    """Tries to do its best for printing the value of a variable as a python
    statement, given that the variable is a string and the user does not know
    its type."""
    re_number = '^\s*[0-9e\+\-\.]+\s*$'
    re_array = '^\s*\[.*\]\s*$'
    if re.match(re_number, v) != None or re.match(re_array, v): \
       return 'IntegerProperty' #this will do in most of the cases
    else:
       return 'StringProperty'

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
        print retval.toxml()
        return retval

    def sql_insert(self, cursor, verbose=True):
        """Inserts myself into the DB"""
        return sql.insert_parameter(cursor, self, verbose)

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

        if len(args) == 4:
            (name, alias, parameter, topalg) = args
            if len(alias) == 0 or alias == None: alias = name
            parameter = deepcopy(parameter)

        elif len(args) == 3:
            alias = ''
            (name, parameter, topalg) = args
            if name.find('/') != -1:
                x = name[0:name.find('/')]
                alias = name[name.find('/')+1:]
                name = x
            if len(alias) == 0 or alias == None: alias = name
            parameter = deepcopy(parameter)
            
        elif len(args) == 2:
            if type(args[0]) == str:
                (name, parameter) = args
                alias = ''
                if name.find('/') != -1:
                    x = name[0:name.find('/')]
                    alias = name[name.find('/')+1:]
                    name = x
                topalg = False
                parameter = deepcopy(parameter)

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
            parameter = []
            for node in args[0].getElementsByTagName('parameter'):
                parameter.append(Parameter(node))
                
        else:
            raise SyntaxError, "Unsupported initialization method for Component"

        self.name = name
        self.alias = alias
        if len(alias) == 0: self.alias = self.name
        self.alias = self.alias.replace('::','__')
        self.topalg = bool(topalg)
        self.parameter = parameter
                
    def __getitem__(self, key):
        """Implements the [] operator, so this looks like a dictionary."""
        if key == 'name': return self.name
        elif key == 'alias': return self.alias
        elif key == 'topalg': return self.topalg
        elif key == 'parameter': return self.parameter
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
        #if you get here, it is ok!
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
        return retval

    def sql_insert(self, cursor, verbose=True):
        """Inserts myself into the DB"""
        return sql.insert_component(cursor, self, verbose)

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
            component = deepcopy(component)
            force_dll = deepcopy(force_dll)

        if len(args) == 3:
            (name, component, force_dll) = args
            version = ''
            component = deepcopy(component)
            force_dll = deepcopy(force_dll)
            
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
            version = ''
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
        if len(self.version) != 0: retval.setAttribute('version', self.version)
        for fd in self.force_dll:
            node = doc.createElement('dll')
            node.setAttribute('name', fd)
            retval.appendChild(node)
        for c in self.component: retval.appendChild(c.xml_node(doc))
        return retval

    def sql_insert(self, cursor, verbose=True):
        """Inserts myself into the DB"""
        return sql.insert_setup(cursor, self, verbose)

class Property(object):
    """An object that defines a property for an HLT release description"""

    def __init__(self, *args):
        """
        Initializes an object of this type. The input argument list is a
        variable parameter list and is filtered for the the cases which are
        supposed to be covered. At the moment, these are the possibilities:
        1) A triple containing, in order, the following information:
           name: The variable name
           value: The variable value
           type: The variable valid C++ type
        2) A double containing:
           name: The variable name
           value: The variable value
           In this case, the type will be inferred from the value of the
           variable, the same was as for a Parameter (see the Parameter class).
        2) A cursor and a parameter identifier (long integer) for reading the
        information from an SQL database as defined in the SQL module
        3) A xml.dom.Element, which contains the description of this property
        """

        if len(args) == 3:
            (name, value, prop_type) = args
            prop_type = _stdtype(prop_type)

        elif len(args) == 2:
            if type(args[0]) == str:
                #simplest initialization, op = 'set', type is inferred
                (name, value) = args
                (prop_type, value) = _infer(value)
                
            else:
                (name, prop_type, value) = sql.read_property(args[0], long(args[1]))
                prop_type = _stdtype(prop_type)
                value = _native_value(prop_type, value)

        elif len(args) == 1 and isinstance(args[0], xml.dom.minidom.Node):
            name = args[0].getAttribute('name')
            value = None
            if args[0].hasAttribute('default'):
                value = _xml2py(args[0].getAttribute('default'))
            strength = args[0].getAttribute('type_strength') 
            prop_type= 'UnknownTypeProperty'
            if args[0].hasAttribute('type'):
                prop_type = _stdtype(args[0].getAttribute('type'))
            if prop_type != 'UnknownTypeProperty' and value is not None:
                value = _native_value(prop_type, value)
            
        else:
            raise SyntaxError, "Unsupported initialization method for Property"

        self.name = name
        self.type = prop_type
        self.value = value
        if value is None: self.value = '__C++DEFAULT__'
        
    def __getitem__(self, key):
        """Implements the [] operator, so this looks like a dictionary."""
        if key == 'name': return self.name
        elif key == 'type': return self.type
        elif key == 'value': return str(self.value)
        raise KeyError, "key %s not found in Parameter object" % key

    def __eq__(self, other):
        """Compares myself to another parameter"""
        if self.name == other.name and \
           self.type == other.type and \
           self.value == other.value:
            return True
        return False

    def __ne__(self, other):
        """Compares myself to another parameter"""
        return not self == other

    def xml_node(self, doc):
        """Creates myself, as an XML node"""
        retval = doc.createElement('property')
        retval.setAttribute('name', self.name)
        retval.setAttribute('type', self.type)
        retval.setAttribute('default', str(self.value))
        return retval

    def sql_insert(self, cursor, verbose=True):
        """Inserts myself into the DB"""
        return sql.insert_property(cursor, self, verbose)

class Source(object):
    """Defines an object that describes a release source"""

    def __init__(self, *args):
        
        """
        Initializes an object of this type. The input argument list is a
        variable parameter list and is filtered for the the cases which are
        supposed to be covered. At the moment, these are the possibilities:
        1) A triple containing, in order, the following information:
           name: The source name (the Gaudi string by which instances of this
                 component are recognized if not aliased)
           gaudi_type: The Gaudi type (service/algorithm/tool/etc) 
           property: A list containing the properties of this component
        2) A cursor and a component identifier (long integer) for reading the
        information from an SQL database as defined in the SQL module.
        3) A single XML Element that contain the description of this component
        """

        if len(args) == 3:
            (name, gaudi_type, property) = args
            property = deepcopy(property)
            
        elif len(args) == 2:
            (name, gaudi_type) = sql.read_source(args[0], long(args[1]))
            property = []
            for (prop_name, prop_type, value) in \
                    sql.read_source_property(args[0], long(args[1])):
                property.append(Property(prop_name,
                                         _native_value(prop_type, value),
                                         prop_type))

        elif len(args) == 1 and isinstance(args[0], xml.dom.minidom.Node):
            gaudi_type = args[0].tagName
            name = args[0].getAttribute('name')
            property = []
            for p in args[0].getElementsByTagName('property'):
                property.append(Property(p))

        else:
            raise SyntaxError, "Unsupported initialization method for Source"

        self.name = name
        self.gaudi_type = gaudi_type

        #build self-property list, checking for duplicates
        self.property = []
        property_names = []
        for p in property:
            if p.name in property_names:
                print "[Warning] Ignored duplicated value for property %s at %s" % \
                      (p.name, name)
            else:
                self.property.append(p)
                property_names.append(p.name)

    def __getitem__(self, key):
        """Implements the [] operator, so this looks like a dictionary."""
        if key == 'name': return self.name
        elif key == 'gaudi_type': return self.gaudi_type
        elif key == 'property': return self.property
        raise KeyError, "key %s not found in Source object" % key

    def __eq__(self, other):
        """Compares myself to another source, disregarding the order in
        which its properties are declared."""
        if self.name == other.name and self.gaudi_type == other.gaudi_type and \
               len(self.property) == len(other.property):
            for p in range(0, len(self.property)):
                if self.property[p] not in other.property:
                    return False
                if other.property[p] not in self.property:
                    return False
            return True
        return False

    def __ne__(self, other):
        """Compares myself to another component"""
        return not self == other

    def xml_node(self, doc):
        """Creates myself, as an XML node."""
        
        retval = doc.createElement(self.gaudi_type)
        retval.setAttribute('name', self.name)
        for p in self.property: retval.appendChild(p.xml_node(doc))
        return retval

    def sql_insert(self, cursor, verbose=True):
        """Inserts myself into the DB"""
        return sql.insert_source(cursor, self, verbose)

    def get_property(self, name):
        """Gets the property defined by the name parameter or raise a
        KeyError, if I cannot find such property"""
        for p in self.property:
            if p.name == name: return p
        raise KeyError, 'I cannot find property "%s" in component "%s"' % \
              (name, self.name)

class Dll(object):
    """An object that defines a DLL for an HLT release description"""

    def __init__(self, *args):
        """
        Initializes an object of this type. The input argument list is a
        variable parameter list and is filtered for the the cases which are
        supposed to be covered. At the moment, these are the possibilities:
        1) A triple containing, in order, the following information:
           name: The DLL name (without the lib and following .so)
           cmt_version: The CMT tagged version of the package in CVS
           source: A list of source components declared inside this DLL
        2) A cursor and a parameter identifier (long integer) for reading the
        information from an SQL database as defined in the SQL module
        3) An XML Element
        """

        if len(args) == 4:
            (name, package, cmt_version, source) = args

        elif len(args) == 2:
            #todo: This can be improved to allow very fast reading of the
            #database, if in one shot we got all components and in a second
            #query all the properties.
            (name, package, cmt_version) = sql.read_dll(args[0], long(args[1]))
            source = []
            for id in sql.read_dll_source_id(args[0], long(args[1])):
                source.append(Source(args[0], id[0]))

        elif len(args) == 1 and isinstance(args[0], xml.dom.minidom.Node):
            name = args[0].getAttribute('name')
            package = ''
            cmt_version = ''
            if args[0].parentNode.tagName.lower() == 'package':
                if args[0].parentNode.hasAttribute('name'):
                    package = args[0].parentNode.getAttribute('name')
                if args[0].parentNode.hasAttribute('version'):
                    cmt_version = args[0].parentNode.getAttribute('version')
                
            source = []
            source_types = [ 'service', 'algorithm', 'tool', 'algtool',
                             'converter', 'applicationmgr' ]
            for t in source_types:
                for s in args[0].getElementsByTagName(t):
                    source.append(Source(s))

        else:
            raise SyntaxError, "Unsupported initialization method for Dll"

        self.name = name
        self.package = package
        self.cmt_version = cmt_version

        #build self-source list, checking for duplicates
        self.source = []
        source_names = []
        for s in source:
            if s.name in source_names:
                print "[Warning] Ignored duplicated value for source %s at %s" % \
                      (s.name, name)
            else:
                self.source.append(s)
                source_names.append(s.name)
        
    def __getitem__(self, key):
        """Implements the [] operator, so this looks like a dictionary."""
        if key == 'name': return self.name
        elif key == 'package': return self.package
        elif key == 'cmt_version': return self.cmt_version
        elif key == 'source': return self.source
        raise KeyError, "key %s not found in Dll object" % key

    def __eq__(self, other):
        """Compares myself to another dll"""
        if self.name == other.name and \
           self.package == other.package and \
           self.cmt_version == other.cmt_version and \
           len(self.source) == len(other.source):
            for s in range(0, len(self.source)):
                if self.source[s] not in other.source: return False
                if other.source[s] not in self.source: return False
            return True
        return False

    def __ne__(self, other):
        """Compares myself to another parameter"""
        return not self == other

    def xml_node(self, doc):
        """Creates this Dll as a triple (package, version, xml_node)"""
        
        retval = doc.createElement('library')
        retval.setAttribute('name', self.name)
        for s in self.source: retval.appendChild(s.xml_node(doc))
        return (self.package, self.cmt_version, retval)

    def xml_node_package(self, doc):
        """This will do exactly the same as xml_node() above, but will return a
        full tree starting from the package level This way you can directly
        test the reverse process of instantiating from XML."""
        (pack, version, node) = self.xml_node(doc)
        retval = doc.createElement('package')
        retval.setAttribute('name', pack)
        retval.setAttribute('version', version)
        retval.appendChild(node)
        return retval

    def sql_insert(self, cursor, verbose=True):
        """Inserts myself into the DB"""
        return sql.insert_dll(cursor, self, verbose)

def DllsFromXMLPackage(pack_xml):
    """Creates DLLs from package XML nodes. This may be necessary when parsing
    whole releases from a single XML file. The single-parameter is the
    top-level node of a package"""
    retval = []
    for entry in pack_xml.getElementsByTagName('library'):
        retval.append(Dll(entry))
    return retval

class Release(object):
    """An object that defines an HLT release description"""

    def __init__(self, *args):
        """
        Initializes an object of this type. The input argument list is a
        variable parameter list and is filtered for the the cases which are
        supposed to be covered. At the moment, these are the possibilities:
        1) A triple containing, in order, the following information:
           name: The release name
           version: The version of this release
           dll: A list of dlls available in this release
        2) A cursor and a parameter identifier (long integer) for reading the
        information from an SQL database as defined in the SQL module
        3) A name, a version and a list of XML nodes describing releases that
        this name/version unique set should put together
        4) An unique release XML node, from where I should read all info from
        """

        if len(args) == 3:
            (name, version, dll) = args

        elif len(args) == 2:
            #todo: This can be improved to allow very fast reading of the
            #database, if in one shot we got all dlls, in a second query
            #all the sources and in a third query, all the properties.
            (name, version) = sql.read_release(args[0], long(args[1]))
            dll = []
            for id in sql.read_release_dll_id(args[0], long(args[1])):
                dll.append(Dll(args[0], id[0]))

        elif len(args) == 1 and isinstance(args[0], xml.dom.minidom.Node):
            name = args[0].getAttribute('name')
            version = args[0].getAttribute('version')
            dll = []
            for package in args[0].getElementsByTagName('package'):
                dll.extend(DllsFromXMLPackage(package))

        else:
            raise SyntaxError, "Unsupported initialization method for Release"

        self.name = name
        self.version = version

        #Build DLL list, checking for duplicates
        dll_names = []
        self.dll = []
        for d in dll:
            if d.name in dll_names:
                print "[Warning] Ignored duplicated value for dll %s at %s" % \
                      (d.name, name)
            else:
                self.dll.append(d)
                dll_names.append(d.name)

    def __getitem__(self, key):
        """Implements the [] operator, so this looks like a dictionary."""
        if key == 'name': return self.name
        elif key == 'version': return self.version
        elif key == 'dll': return self.dll
        raise KeyError, "key %s not found in Release object" % key

    def __eq__(self, other):
        """Compares myself to another parameter"""
        if self.name == other.name and \
           self.version == other.version and \
           len(self.dll) == len(other.dll):
            for d in range(0, len(self.dll)):
                if self.dll[d] not in other.dll: return False
                if other.dll[d] not in self.dll: return False
            return True
        return False

    def __ne__(self, other):
        """Compares myself to another parameter"""
        return not self == other

    def merge(self, other):
        """Copies the data from the other release into myself, creating a
        merged release."""
        for d in other.dll: self.dll.append(deepcopy(d))

    def xml_node(self, doc):
        """Builds itself, recursively and into the document"""
        retval = doc.createElement('release')
        retval.setAttribute('name', self.name)
        retval.setAttribute('version', self.version)
        packs = {}
        for d in self.dll:
            (package, version, node) = d.xml_node(doc)
            hash = '%s/%s' % (package, version)
            if hash in packs.keys(): packs[hash].append(node)
            else: packs[hash] = [node]
        for hash, dll in packs.items():
            package_node = doc.createElement('package')
            (name, version) = hash.split('/')
            package_node.setAttribute('name', name)
            package_node.setAttribute('version', version)
            retval.appendChild(package_node)
            for d in dll: package_node.appendChild(d)
        return retval

    def sql_insert(self, cursor, verbose=True):
        """Inserts myself into the DB"""
        return sql.insert_release(cursor, self, verbose)

    def get_source(self, name):
        """Returns the object that represents the source with the given name or
        raise a KeyError exception."""
        for d in self.dll:
            for s in d.source:
                if name == s.name: return s

        raise KeyError, 'I cannot find source for "%s"' % name

    def get_source_dll(self, name):
        """Returns the dll that contains the source or raise a
        KeyError exception."""
        for d in self.dll:
            for s in d.source:
                if name == s.name: return d

        raise KeyError, 'I cannot find (source) dll for "%s"' % name

    def get_source_dll_name(self, name):
        """Returns the name of the dll that contains the source or raise a
        KeyError exception."""
        return self.get_source_dll(name).name

    def get_dll(self, name):
        """Returns the object that represents the Dll with the given name or
        raise a KeyError exception."""
        for d in self.dll:
            if name == d.name: return d

        raise KeyError, 'I cannot find dll for "%s"' % name

class ReleaseSummary(Release):
    """A summary is a mini-release that can be only used to check the validity
    of a job. You cannot insert a SummaryRelease into the database or read one
    from the database. It is, in a sense, a temporary thing, used to make an
    XML job description complete."""
    
    def __init__(self, *args):
        """
        Initializes this SummaryRelease. These are the currently implemented
        possibilities:
        1. a double, with a release and a setup. In this case the release will
        be filtered according to the setup components available
        2. an XML top node where I should be able to read myself from.
        """
        if len(args) == 2:
            self.name = args[0].name
            self.version = args[0].version
            self.dll = []
            for c in args[1].component:
                to_include = args[0].get_source_dll(c.name)
                if to_include not in self.dll: self.dll.append(to_include)
            for d in args[1].force_dll:
                to_include = args[0].get_dll(d)
                if to_include not in self.dll: self.dll.append(to_include)

        elif len(args) == 1 and isinstance(args[0], xml.dom.minidom.Node):
            self.name = args[0].getAttribute('name')
            self.version = args[0].getAttribute('version')
            self.dll = []
            for l in args[0].getElementsByTagName('library'):
                to_include = Dll(l)
                if to_include not in self.dll: self.dll.append(to_include)

        else:
            raise SyntaxError, "Unsupported initialization method for SummaryRelease"

    def sql_insert(self, cursor, verbose=True):
        """Inserts myself into the DB"""
        raise AssertionError, "You cannot insert a SummaryRelease into the DB."

    def xml_node(self, doc):
        """Builds itself, recursively and into the document"""
        retval = doc.createElement('summary')
        retval.setAttribute('name', self.name)
        retval.setAttribute('version', self.version)
        for d in self.dll: retval.appendChild(d.xml_node(doc)[2])
        return retval

