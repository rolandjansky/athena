# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import GaudiConfig2.semantics
import re
import collections
import copy

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

class SetSemantics(GaudiConfig2.semantics.SequenceSemantics):
    '''
    Extend the sequence-semantics with a merge-method that treats the like 
    sets, eg creating a list with unique values. 
    Use 'Set<T>' as fifth parameter of the Gaudi::Property<T> constructor 
    to invoke this merging method. The template parameter is important, also
    in the string that forms the fifth argument. 
    '''
    __handled_types__ = (re.compile(r"^Set<.*>$"),)
    def __init__(self, cpp_type, name=None):
        super(SetSemantics, self).__init__(cpp_type, name)

    def merge(self,a,b):
        union=set(a) | set(b)
        return union#GaudiConfig2.semantics._ListHelper(union)


class VarHandleSematics(GaudiConfig2.semantics.StringSemantics):
    '''
    Treat VarHandleKeys like strings
    '''
    __handled_types__ = ("SG::VarHandleKey",)
    def __init__(self,cpp_type,name=None):
        super(VarHandleSematics,self).__init__(cpp_type, name)
        pass

class ToolHandleSemantics(GaudiConfig2.semantics.PropertySemantics):
    '''
    Private alg-tools need recusive merging (de-duplication):
    ''' 
    __handled_types__ = ("PrivateToolHandle",)
    def __init__(self,cpp_type,name=None):
        super(ToolHandleSemantics, self).__init__(cpp_type,name)
        

    def merge(self,a,b):
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
        if value.__component_type__ not in ('Service','AlgTool'):
            raise TypeError('{} expected, got {!r} in assignemnt to {}'.\
                            format(value.__component_type__,value, self.name))

        #It would be great if at this point we could verify that the service was
        #ineed added to the ComponentAccumulator. Latest, do that when bootstapping
        #the application

        return "{}/{}".format(value.__cpp_type__,value.name)


class ToolHandleArraySemantics(GaudiConfig2.semantics.PropertySemantics):
    '''
    Private alg-tools need recusive merging (de-duplication):
    ''' 
    __handled_types__ = ("PrivateToolHandleArray",)
    def __init__(self,cpp_type,name=None):
        super(ToolHandleArraySemantics, self).__init__(cpp_type,name)
    
    def default(self,value):
        return copy.copy(value)

    def merge(self,a,b):
        for bTool in b:
            try:
                #If a tool with that name exists in a, we'll merge it 
                a.__getitem__(bTool.name).merge(bTool)
            except IndexError:
                #Tool does not exists in a, append it
                a.append(bTool)
        return a

class SubAlgoSemantics(GaudiConfig2.semantics.PropertySemantics):
    __handled_types__  = ("SubAlgorithm",)
    def __init__(self,cpp_type,name=None):
        super(SubAlgoSemantics, self).__init__(cpp_type,name)
        
    def store(self,value):
        if not isinstance(value,collections.Sequence):
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


GaudiConfig2.semantics.SEMANTICS.append(SetSemantics)
GaudiConfig2.semantics.SEMANTICS.append(AppendListSemantics)
GaudiConfig2.semantics.SEMANTICS.append(VarHandleSematics)
GaudiConfig2.semantics.SEMANTICS.append(ToolHandleSemantics)
GaudiConfig2.semantics.SEMANTICS.append(ToolHandleArraySemantics)
GaudiConfig2.semantics.SEMANTICS.append(PublicHandleSemantics)
GaudiConfig2.semantics.SEMANTICS.append(SubAlgoSemantics)
