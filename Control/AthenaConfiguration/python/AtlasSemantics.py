# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import GaudiConfig2.semantics
from GaudiKernel.GaudiHandles import PrivateToolHandleArray
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

    def merge(self,bb,aa):
        for b in bb:
            if b not in aa:
                aa.append(b) 
        return aa
        #union=set(a) | set(b)
        #return union#GaudiConfig2.semantics._ListHelper(union)


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
        if a is None: return b
        if b is None: return a
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
        if not isinstance(value,collections.Sequence) and not isinstance(value,set):
            value=[value,]

        newValue=[]
        for v in value:
            if isinstance(v,GaudiConfig2._configurables.Configurable):
                if v.__component_type__ not in ('Service','AlgTool'):
                    raise TypeError('{} expected, got {!r} in assignemnt to {}'.\
                                    format(value.__component_type__,v, self.name))
                else:
                    newValue.append("{}/{}".format(v.__cpp_type__,v.name))
            elif isinstance(v,basestring):
                #Check if componet is known ...
                newValue.append(v)
                pass
            else:
                raise TypeError('Configurable expected, got {!r} in assignment to {}'.\
                                format(value,self.name))
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
GaudiConfig2.semantics.SEMANTICS.append(VarHandleArraySematics)
GaudiConfig2.semantics.SEMANTICS.append(ToolHandleSemantics)
GaudiConfig2.semantics.SEMANTICS.append(ToolHandleArraySemantics)
GaudiConfig2.semantics.SEMANTICS.append(PublicHandleSemantics)
GaudiConfig2.semantics.SEMANTICS.append(PublicHandleArraySemantics)
GaudiConfig2.semantics.SEMANTICS.append(SubAlgoSemantics)


#Hack until Gaudi::Property<std::set> arrives...

from GaudiConfig2 import Configurables as _cfgs
epsvc=_cfgs.EvtPersistencySvc
epsvc._descriptors["CnvServices"].semantics=SetSemantics(cpp_type="Set<std::string>")
epsvc._descriptors["CnvServices"].semantics.name="CnvServices"

audSvc=_cfgs.AuditorSvc
audSvc._descriptors["Auditors"].semantics=SetSemantics(cpp_type="Set<std::string>")
audSvc._descriptors["Auditors"].semantics.name="Auditors"
del _cfgs

from GaudiConfig2._configurables import Configurable
Configurable.getFullJobOptName= lambda self: "{}/{}".format(self.__cpp_type__,self.name)


#GaudiConfig2._DictHelper misses the update method that sets is_dirty to true

def _DictHelperUpdate(self,otherMap):
    self.is_dirty=True
    #Fixme: A proper implementation should invoke the value-semantics and key-semantics at this point
    self.data.update(otherMap)
    return

GaudiConfig2.semantics._DictHelper.update=_DictHelperUpdate
del _DictHelperUpdate


#For some obscure reason, _ListHelper object never compare equal. Therefore PropertySemantics merge() method fails
def _sequencemerge(instance,a,b):
    if a.data != b.data:
        raise ValueError('cannot merge values %r and %r' % (a, b))
    else:
        return a
    
from GaudiConfig2.semantics import SequenceSemantics
SequenceSemantics.merge = _sequencemerge
del _sequencemerge




