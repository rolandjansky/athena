# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from copy import deepcopy 

log = logging.getLogger("TopoAlgo") 

class TopoAlgo(object):

    _availableVars = []

    #__slots__ = ['_name', '_selection', '_value', '_generic']
    def __init__(self, classtype, name, algoId=0):
        self.classtype = classtype
        self.name = name
        self.algoId = int(algoId)
        self.generics = []
        self.variables = []
        
    def __str__(self):  
        return self.name

    def addvariable(self, name, value, selection = -1):
        if name in self._availableVars:
            self.variables += [ Variable(name, selection, value) ]
        else:
            raise RuntimeError("Variable parameter '%s' does not exist for algorithm %s of type %s,\navailable parameters are %r" % (name,self.name, self.classtype, self._availableVars))
        return self

    def addgeneric(self, name, value):
        if name in self._availableVars:
            self.generics += [ Generic(name, value) ]
        else:
            raise RuntimeError("Generic parameter '%s' does not exist for algorithm %s of type %s,\navailable parameters are %r" % (name,self.name, self.classtype, self._availableVars))
        return self


    
class Variable(object):
    def __init__(self, name, selection, value):
        self.name = name
        self.selection = int(selection)
        self.value = int(value)
            
class Generic(object):
    def __init__(self, name, value):
        self.name = name
        from L1TopoHardware.L1TopoHardware import HardwareConstrainedParameter
        if isinstance(value,HardwareConstrainedParameter):
            self.value = ":%s:" % value.name
        else:
            self.value = str(int(value))

        
class SortingAlgo(TopoAlgo):
    
    def __init__(self, classtype, name, inputs, outputs, algoId):
        super(SortingAlgo, self).__init__(classtype=classtype, name=name, algoId=algoId)
        self.inputs = inputs
        self.outputs = outputs
        self.inputvalue=  self.inputs
        if self.inputs.find("Cluster")>=0: # to extract inputvalue (for FW) from output name
            if self.outputs.find("TAU")>=0: self.inputvalue= self.inputvalue.replace("Cluster","Tau")            
            if self.outputs.find("EM")>=0:  self.inputvalue= self.inputvalue.replace("Cluster","Em")
            
    def xml(self): 
        s='  <SortAlgo type="%s" name="%s" output="%s" algoId="%i">\n' % (self.classtype, self.name, self.outputs, self.algoId)
        s+='    <Fixed>\n'
        s+='      <Input name="%s" value="%s"/>\n' % (self.inputs, self.inputvalue) 
        s+='      <Output name="TobArrayOut" value="%s"/>\n' % (self.outputs)
        for gene in self.generics:
            s += '      <Generic name="%s" value="%s"/>\n' % (gene.name, gene.value)
        s+='    </Fixed>\n'            
        s+='    <Variable>\n'

        for (pos, variable) in enumerate(self.variables):
            s+='      <Parameter pos="%i" name="%s" value="%i"/>\n' % ( pos, variable.name, variable.value )
        s+='    </Variable>\n'    
        s+='  </SortAlgo>\n'
        return s
        
class DecisionAlgo(TopoAlgo):

    def __init__(self, classtype, name, inputs, outputs, algoId):
        super(DecisionAlgo, self).__init__(classtype=classtype, name=name, algoId=algoId)
        self.inputs = inputs if type(inputs)==list else [inputs]
        self.outputs = outputs if type(outputs)==list else [outputs]
        
    def xml(self): 
        s='  <DecisionAlgo type="%s" name="%s" algoId="%i">\n' % (self.classtype, self.name, self.algoId)
        s+='    <Fixed>\n'

        input_woovlp = []
        for (tobid, _input) in enumerate(self.inputs):
            
            if len(self.inputs)>1:
                if _input not in input_woovlp:
                    s+='      <Input name="Tob%s" value="%s" position="%s"/>\n' % (str(tobid+1), _input, "0")
                    input_woovlp += [_input]
                else:
                    s+='      <Input name="Tob%s" value="%s" position="%s"/>\n' % (str(tobid+1), _input, str(tobid))
            else:
                s+='      <Input name="Tob" value="%s"/>\n' % (_input)
        s+='      <Output name="Results" bits="%s">\n' % str(len(self.outputs))
        for (bitid, _output) in enumerate(self.outputs):
            s+='        <Bit selection="%s" name="%s"/>\n' % (str(bitid), _output)
        s+='      </Output>\n'
        for gene in self.generics:
            s += '      <Generic name="%s" value="%s"/>\n' % (gene.name, gene.value)
        s+='    </Fixed>\n'     
        s+='    <Variable>\n'
        
        for (pos, variable) in enumerate(self.variables):
            s+='      <Parameter pos="%i" name="%s"%s value="%i"/>\n' % ( pos, variable.name, ((' selection="%i"'%variable.selection) if (variable.selection>=0) else ""), variable.value )
        s+='    </Variable>\n'    
        s+='  </DecisionAlgo>\n'
        return s

