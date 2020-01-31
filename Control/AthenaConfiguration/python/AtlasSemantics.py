# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import GaudiConfig2.semantics
import re

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
        
class ToolHandleArraySemantics(GaudiConfig2.semantics.PropertySemantics):
    '''
    Private alg-tools need recusive merging (de-duplication):
    ''' 
    __handled_types__ = ("PrivateToolHandleArray",)
    def __init__(self,cpp_type,name=None):
        super(ToolHandleArraySemantics, self).__init__(cpp_type,name)
    
    def default(self,value):
        from GaudiKernel.GaudiHandles import PrivateToolHandleArray
        return PrivateToolHandleArray()

    def merge(self,a,b):
        for bTool in b:
            try:
                #If a tool with that name exists in a, we'll merge it 
                a.__getitem__(bTool.name).merge(bTool)
            except IndexError:
                #Tool does not exists in a, append it
                a.append(bTool)
        return a

GaudiConfig2.semantics.SEMANTICS.append(SetSemantics)
GaudiConfig2.semantics.SEMANTICS.append(AppendListSemantics)
GaudiConfig2.semantics.SEMANTICS.append(VarHandleSematics)
GaudiConfig2.semantics.SEMANTICS.append(ToolHandleSemantics)
GaudiConfig2.semantics.SEMANTICS.append(ToolHandleArraySemantics)
