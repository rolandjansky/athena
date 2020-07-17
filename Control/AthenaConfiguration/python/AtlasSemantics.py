# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from past.builtins import basestring
import GaudiConfig2.semantics
from GaudiKernel.GaudiHandles import PrivateToolHandleArray, PublicToolHandle, ServiceHandle
import re
import collections
import copy

# collections.Sequence is deprecated as of python 3.3.
# As of 3.9, need to get it from collections.abc.
# But that doesn't exist in py2.
try:
    from collections import abc
    Sequence = abc.Sequence
except ImportError:
    Sequence = collections.Sequence

class AppendListSemantics(GaudiConfig2.semantics.SequenceSemantics):
    '''
    Extend the sequence-semantics with a merge-method that appends the lists
    Use 'appendList<T>' as fifth parameter of the Gaudi::Property<T> constructor 
    to invoke this merging method. The template parameter is important, also
    in the string that forms the fifth argument. 
    '''
    __handled_types__ = (re.compile(r"^appendList<.*>$"),)
    def __init__(self, cpp_type, name=None):
        super(AppendListSemantics, self).__init__(cpp_type, name)

    def merge(self,a,b):
        a.extend(b)
        return a

class MapMergeNoReplaceSemantics(GaudiConfig2.semantics.MappingSemantics):
    '''
    Extend the mapping-semantics with a merge-method that merges two mappings as long as they do not have different values for the same key
    Use 'mapMergeNoReplace<T>' as fifth parameter of the Gaudi::Property<T> constructor
    to invoke this merging method.
    '''
    __handled_types__ = (re.compile(r"^mapMergeNoReplace<.*>$"),)
    def __init__(self, cpp_type, name=None):
        super(MapMergeNoReplaceSemantics, self).__init__(cpp_type, name)

    def merge(self,a,b):
        for k in b.keys():
            if k in a and b[k] != a[k]:
                raise ValueError('conflicting values in map under key %r and %r %r' % (k, b[k], a[k]))
            a[k] = b[k]
        return a

class VarHandleSematics(GaudiConfig2.semantics.StringSemantics):
    '''
    Treat VarHandleKeys like strings
    '''
    __handled_types__ = ("SG::VarHandleKey",)
    def __init__(self,cpp_type,name=None):
        super(VarHandleSematics,self).__init__(cpp_type, name)
        pass


class VarHandleArraySematics(GaudiConfig2.semantics.PropertySemantics):
    '''
    Treat VarHandleKeyArrays like arrays of strings
    '''
    __handled_types__ = ("SG::VarHandleKeyArray",)
    def __init__(self,cpp_type,name=None):
        super(VarHandleArraySematics,self).__init__(cpp_type, name)
        pass

    def merge(self,aa,bb):
        for b in bb:
            if b not in aa:
                aa.append(b)
        return aa
        #union=set(a) | set(b) 
        #return list(union)

class ToolHandleSemantics(GaudiConfig2.semantics.PropertySemantics):
    '''
    Private alg-tools need recusive merging (de-duplication):
    ''' 
    __handled_types__ = ("PrivateToolHandle",)
    def __init__(self,cpp_type,name=None):
        super(ToolHandleSemantics, self).__init__(cpp_type,name)
        

    def merge(self,a,b):
        #Deal with 'None'
        if a is None or a=='': return b
        if b is None or b=='': return a
        return a.merge(b)

class PublicHandleSemantics(GaudiConfig2.semantics.PropertySemantics):
    '''
    ServiceHandles (and the deprecated PublicToolHandles) are assigned as strings
    '''
    __handled_types__ = ("PublicToolHandle","ServiceHandle")
       
    def __init__(self,cpp_type,name=None):
        super(PublicHandleSemantics, self).__init__(cpp_type,name)
        
    def default(self,value):
        return value.typeAndName

    def store(self,value):
        if isinstance(value,str): #Assume the string is correct and the tool/svc alreayd in the CA ... 
            return value
        
        if value is None: 
            return ""

        if not hasattr(value,"__component_type__"):
            raise TypeError("Got {}, expected Tool or Service in assignment to {}".format(type(value),self.name))

        if value.__component_type__ not in ('Service','AlgTool'):
            raise TypeError('{} expected, got {!r} in assignemnt to {}'.\
                            format(value.__component_type__,value, self.name))

        #It would be great if at this point we could verify that the service was
        #ineed added to the ComponentAccumulator. Latest, do that when bootstapping
        #the application

        return "{}/{}".format(value.__cpp_type__,value.name)

class PublicHandleArraySemantics(GaudiConfig2.semantics.PropertySemantics):
    '''
    Semantics for arrays of string-based pointers to components defined elsewhere
    '''
    __handled_types__ = ("PublicToolHandleArray","ServiceHandleArray")
    def __init__(self,cpp_type,name=None):
        super(PublicHandleArraySemantics, self).__init__(cpp_type,name)
        
    def store(self, value):
        if not isinstance(value,Sequence) and not isinstance(value,set):
            value=[value,]

        newValue=[]
        for v in value:
            if isinstance(v,GaudiConfig2._configurables.Configurable):
                if v.__component_type__ not in ('Service','AlgTool'):
                    raise TypeError('{} expected, got {!r} in assignemnt to {}'.\
                                    format(value.__component_type__,v, self.name))
                else:
                    newValue.append("{}/{}".format(v.__cpp_type__,v.name))

            elif isinstance(v,(PublicToolHandle,ServiceHandle)):
                newValue.append("{}/{}".format(v.getType(),v.getName()))

            elif isinstance(v,basestring):
                #Check if componet is known ...
                newValue.append(v)
                pass
            else:
                raise TypeError('Configurable expected, got {!r} in assignment to {}'.\
                                format(v,self.name))
        return newValue
            
    def default(self, value):
        return copy.copy(value)
        

    def merge(self,aa,bb):
        for b in bb:
            if b not in aa:
                aa.append(b)
        return aa
        #union=set(a) | set(b)
        #return union


class ToolHandleArraySemantics(GaudiConfig2.semantics.PropertySemantics):
    '''
    Private alg-tools need recusive merging (de-duplication):
    ''' 
    __handled_types__ = ("PrivateToolHandleArray",)
    def __init__(self,cpp_type,name=None):
        super(ToolHandleArraySemantics, self).__init__(cpp_type,name)
    
    def default(self,value):
        return copy.copy(value)

    def store(self,value):
        if not isinstance(value,PrivateToolHandleArray):
            #try to convert the value to a PrivateToolHandleArray
            value=PrivateToolHandleArray(value)
        return value

    def merge(self,a,b):
        for bTool in b:
            try:
                #If a tool with that name exists in a, we'll merge it
                a.__getitem__(bTool.getName()).merge(bTool)
            except IndexError:
                #Tool does not exists in a, append it
                a.append(bTool)
        return a

class SubAlgoSemantics(GaudiConfig2.semantics.PropertySemantics):
    __handled_types__  = ("SubAlgorithm",)
    def __init__(self,cpp_type,name=None):
        super(SubAlgoSemantics, self).__init__(cpp_type,name)
        
    def store(self,value):
        if not isinstance(value,Sequence):
            value=[value,]
        
        for v in value:
            if v.__component_type__ != 'Algorithm':
                raise TypeError('Algorithm expected, got {!r} in assignemnt to {}'.\
                                format(value, self.name))
        return value


    #Without explicitly definig a default, calling .append or += will change the class-default, 
    #affecting all instances of the same class. 
    def default(self,value):
        return []


GaudiConfig2.semantics.SEMANTICS.append(AppendListSemantics)
GaudiConfig2.semantics.SEMANTICS.append(VarHandleSematics)
GaudiConfig2.semantics.SEMANTICS.append(VarHandleArraySematics)
GaudiConfig2.semantics.SEMANTICS.append(ToolHandleSemantics)
GaudiConfig2.semantics.SEMANTICS.append(ToolHandleArraySemantics)
GaudiConfig2.semantics.SEMANTICS.append(PublicHandleSemantics)
GaudiConfig2.semantics.SEMANTICS.append(PublicHandleArraySemantics)
GaudiConfig2.semantics.SEMANTICS.append(SubAlgoSemantics)
GaudiConfig2.semantics.SEMANTICS.append(MapMergeNoReplaceSemantics)


#For some obscure reason, _ListHelper object never compare equal. Therefore PropertySemantics merge() method fails
def _sequencemerge(instance,a,b):
    if a.data != b.data:
        raise ValueError('cannot merge sequence of values %r and %r' % (a, b))
    else:
        return a
    
from GaudiConfig2.semantics import SequenceSemantics
SequenceSemantics.merge = _sequencemerge
del _sequencemerge




