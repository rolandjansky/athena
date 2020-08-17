# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags
import re

from .ThresholdType import ThrType

log = logging.getLogger("Menu.L1.Base.TopoAlgos") 

##
## These classes are base classes for the auto-generated algorithm python representations
## 
## C++ L1Topo algorithms are defined in Trigger/TrigT1/L1Topo/L1TopoAlgorithms
## During the build, from each class a python class is generated and put in the release
## Those generated python classes derive fro SortingAlgo and DecisionAlgo below.

class TopoAlgo(object):

    _availableVars = []

    #__slots__ = ['_name', '_selection', '_value', '_generic']
    def __init__(self, classtype, name, algoId=-1):
        self.classtype = classtype
        self.name = name
        self.algoId = algoId
        self.generics = []
        self.variables = []
        
    def __str__(self):  
        return self.name

    def isSortingAlg(self):
        return False

    def isDecisionAlg(self):
        return False

    def isMultiplicityAlg(self):
        return False

    def setThresholds(self, thresholds):
        # link to all thresholds in the menu need for configuration
        self.menuThr = thresholds

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

    def json(self):
        confObj = odict()
        confObj["algId"] = self.algoId
        confObj["klass"] = self.classtype
        return confObj

    def getScaleToCountsEM(self):
        tw = self.menuThr.typeWideThresholdConfig(ThrType["EM"])
        return 1000 // tw["resolutionMeV"]
    
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
            self.value = value

        
class SortingAlgo(TopoAlgo):
    
    def __init__(self, classtype, name, inputs, outputs, algoId):
        super(SortingAlgo, self).__init__(classtype=classtype, name=name, algoId=algoId)
        self.inputs = inputs
        self.outputs = outputs
        self.inputvalue=  self.inputs
        if self.inputs.find("Cluster")>=0: # to extract inputvalue (for FW) from output name
            if self.outputs.find("TAU")>=0:
                self.inputvalue= self.inputvalue.replace("Cluster","Tau")
            if self.outputs.find("EM")>=0:
                self.inputvalue= self.inputvalue.replace("Cluster","Em")

    def isSortingAlg(self):
        return True
        
    def json(self):
        confObj = super(SortingAlgo, self).json()
        confObj["input"] = self.inputvalue
        confObj["output"] = self.outputs
        confObj["fixedParameters"] = odict()
        confObj["fixedParameters"]["generics"] = odict()
        for (pos, genParm) in enumerate(self.generics):
            confObj["fixedParameters"]["generics"][genParm.name] = odict([("value", genParm.value), ("position", pos)]) 

        confObj["variableParameters"] = list()
        _emscale_for_decision = self.getScaleToCountsEM()
        _mu_for_decision=1 # MU4->3GeV, MU6->5GeV, MU10->9GeV
        for (pos, variable) in enumerate(self.variables): 
            # adjust MinET if outputs match with EM or TAU or MU  ==> this is a terrible hack that won't fly in Run 3
            if variable.name == "MinET":
                if "TAU" in self.outputs or "EM" in self.outputs:
                    variable.value *= _emscale_for_decision
                if "MU" in self.outputs and variable.value > _mu_for_decision:
                    variable.value -= _mu_for_decision
            confObj["variableParameters"].append(odict([("name", variable.name),("value", variable.value)]))

            if type(variable.value) == float:
                raise RuntimeError("In algorithm %s the variable %s with value %r is of type float but must be int" % (self.name,variable.name,variable.value))
        return confObj

    def xml(self):
        _emscale_for_decision=2
        _mu_for_decision=1 # MU4->3GeV, MU6->5GeV, MU10->9GeV
        if hasattr(TriggerFlags, 'useRun1CaloEnergyScale'):
            if TriggerFlags.useRun1CaloEnergyScale :
                _emscale_for_decision=1
                log.info("Changed mscale_for_decision %s for Run1CaloEnergyScale", _emscale_for_decision)
        
        s='  <SortAlgo type="%s" name="%s" output="%s" algoId="%i">\n' % (self.classtype, self.name, self.outputs, self.algoId)
        s+='    <Fixed>\n'
        s+='      <Input name="%s" value="%s"/>\n' % (self.inputs, self.inputvalue) 
        s+='      <Output name="TobArrayOut" value="%s"/>\n' % (self.outputs)
        for gene in self.generics:
            s += '      <Generic name="%s" value="%s"/>\n' % (gene.name, gene.value)
        s+='    </Fixed>\n'            
        s+='    <Variable>\n'

        for (pos, variable) in enumerate(self.variables):
            # scale MinET if outputs match with EM or TAU
            if variable.name=="MinET" and (self.outputs.find("TAU")>=0 or self.outputs.find("EM")>=0):
                variable.value = variable.value * _emscale_for_decision
            if variable.name=="MinET" and self.outputs.find("MU")>=0:
                variable.value = ((variable.value - _mu_for_decision) if variable.value>0 else variable.value)
            s+='      <Parameter pos="%i" name="%s" value="%i"/>\n' % ( pos, variable.name, variable.value )
        s+='    </Variable>\n'    
        s+='  </SortAlgo>\n'
        return s


class DecisionAlgo(TopoAlgo):

    def __init__(self, classtype, name, inputs, outputs, algoId):
        super(DecisionAlgo, self).__init__(classtype=classtype, name=name, algoId=algoId)
        self.inputs = inputs if type(inputs)==list else [inputs]
        self.outputs = outputs if type(outputs)==list else [outputs]

    def isDecisionAlg(self):
        return True

    def json(self):
        confObj = super(DecisionAlgo, self).json()
        confObj["input"] = self.inputs # list of input names
        confObj["output"] = self.outputs # list of output names
        # fixed parameters
        confObj["fixedParameters"] = odict()
        confObj["fixedParameters"]["generics"] = odict()
        for (pos, genParm) in enumerate(self.generics):
            confObj["fixedParameters"]["generics"][genParm.name] = odict([("value", genParm.value), ("position", pos)]) 

        # variable parameters
        confObj["variableParameters"] = list()
        _emscale_for_decision = self.getScaleToCountsEM()
        _mu_for_decision = 1 # MU4->3GeV, MU6->5GeV, MU10->9GeV
        for (pos, variable) in enumerate(self.variables):
            # scale MinET if inputs match with EM or TAU
            for _minet in ["MinET"]:
                if variable.name==_minet+"1" or variable.name==_minet+"2" or variable.name==_minet+"3" or variable.name==_minet:
                    for (tobid, _input) in enumerate(self.inputs):
                        if (_input.find("TAU")>=0 or _input.find("EM")>=0):
                            if (len(self.inputs)>1 and (variable.name==_minet+str(tobid+1) or (tobid==0 and variable.name==_minet))) or (len(self.inputs)==1 and (variable.name.find(_minet)>=0)):
                                variable.value *= _emscale_for_decision

                        if _input.find("MU")>=0:
                            if (len(self.inputs)>1 and (variable.name==_minet+str(tobid+1) or (tobid==0 and variable.name==_minet))) or (len(self.inputs)==1 and (variable.name.find(_minet)>=0)):
                                variable.value = ((variable.value - _mu_for_decision ) if variable.value>0 else variable.value)

            if type(variable.value) == float:
                raise RuntimeError("In algorithm %s the variable %s with value %r is of type float but must be int" % (self.name,variable.name,variable.value))

            if variable.selection >= 0:
                confObj["variableParameters"].append(odict([("name", variable.name), ("selection",variable.selection), ("value", variable.value)]))
            else:
                confObj["variableParameters"].append(odict([("name", variable.name), ("value", variable.value)]))

        return confObj


    def xml(self): 
        _emscale_for_decision=2
        _mu_for_decision=1 
        if hasattr(TriggerFlags, 'useRun1CaloEnergyScale'):
            if TriggerFlags.useRun1CaloEnergyScale :
                _emscale_for_decision=1
                log.info("Changed mscale_for_decision %s for Run1CaloEnergyScale", _emscale_for_decision)
        
        s='  <DecisionAlgo type="%s" name="%s" algoId="%i">\n' % (self.classtype, self.name, self.algoId )
        s+='    <Fixed>\n'
        input_woovlp = []
        for (tobid, _input) in enumerate(self.inputs):
            if len(self.inputs)>1:
                if _input not in input_woovlp:
                    s+='      <Input name="Tob%s" value="%s" position="%s"/>\n' % (str(tobid+1), _input, str(tobid))
                    input_woovlp += [_input]
                else:
                    s+='      <Input name="Tob%s" value="%s" position="%s"/>\n' % (str(tobid+1), _input, str(tobid))
            else:
                s+='      <Input name="Tob" value="%s" position="%s"/>\n' % (_input, str(tobid))
        s+='      <Output name="Results" bits="%s">\n' % str(len(self.outputs))
        for (bitid, _output) in enumerate(self.outputs):
            s+='        <Bit selection="%s" name="%s"/>\n' % (str(bitid), _output)
        s+='      </Output>\n'
        for gene in self.generics:
            s += '      <Generic name="%s" value="%s"/>\n' % (gene.name, gene.value)
        s+='    </Fixed>\n'     
        s+='    <Variable>\n'

        for (pos, variable) in enumerate(self.variables):
            # scale MinET if inputs match with EM or TAU
            for _minet in ["MinET"]:
                if variable.name==_minet+"1" or variable.name==_minet+"2" or variable.name==_minet+"3" or variable.name==_minet:
                    for (tobid, _input) in enumerate(self.inputs):
                        if (_input.find("TAU")>=0 or _input.find("EM")>=0):
                            if (len(self.inputs)>1 and (variable.name==_minet+str(tobid+1) or (tobid==0 and variable.name==_minet))) or (len(self.inputs)==1 and (variable.name.find(_minet)>=0)):
                                variable.value = variable.value * _emscale_for_decision

                        if _input.find("MU")>=0:
                            if (len(self.inputs)>1 and (variable.name==_minet+str(tobid+1) or (tobid==0 and variable.name==_minet))) or (len(self.inputs)==1 and (variable.name.find(_minet)>=0)):
                                variable.value = ((variable.value - _mu_for_decision ) if variable.value>0 else variable.value)
                            
            s+='      <Parameter pos="%i" name="%s"%s value="%i"/>\n' % ( pos, variable.name, ((' selection="%i"'%variable.selection) if (variable.selection>=0) else ""), variable.value )
        s+='    </Variable>\n'    
        s+='  </DecisionAlgo>\n'
        return s




class MultiplicityAlgo(TopoAlgo):

    def __init__(self, classtype, name, algoId, threshold, input, output, nbits):
        super(MultiplicityAlgo, self).__init__(classtype=classtype, name=name, algoId=algoId)
        self.threshold = threshold
        self.input = input
        self.outputs = output
        self.nbits = nbits

    def isMultiplicityAlg(self):
        return True            

    def configureFromThreshold(self, thr):
        pass

    def json(self):
        confObj = super(MultiplicityAlgo, self).json()
        confObj["threshold"] = self.threshold
        confObj["input"] = self.input
        confObj["output"] = self.outputs
        confObj["nbits"] = self.nbits
        return confObj


class EMMultiplicityAlgo(MultiplicityAlgo):
    def __init__(self, name, algoId, threshold, nbits, classtype = "EMMultiplicity" ):
        super(EMMultiplicityAlgo, self).__init__(classtype=classtype, name=name, 
                                                 algoId=algoId, 
                                                 threshold = threshold, 
                                                 input=None, output="%s" % threshold,
                                                 nbits=nbits)
        mres = re.match("(?P<type>[A-z]*)[0-9]*(?P<suffix>[VHI]*)",threshold).groupdict()
        self.input = mres["type"]


class TauMultiplicityAlgo(MultiplicityAlgo):
    def __init__(self, name, algoId, threshold, nbits, classtype = "TauMultiplicity" ):
        super(TauMultiplicityAlgo, self).__init__(classtype=classtype, name=name, 
                                                  algoId=algoId, 
                                                  threshold = threshold, 
                                                  input=None, output="%s" % threshold,
                                                  nbits=nbits)

class JetMultiplicityAlgo(MultiplicityAlgo):
    def __init__(self, name, algoId, threshold, nbits, classtype = "JetMultiplicity" ):
        super(JetMultiplicityAlgo, self).__init__(classtype=classtype, name=name, 
                                                  algoId=algoId, 
                                                  threshold = threshold, 
                                                  input=None, output="%s" % threshold,
                                                  nbits=nbits)

class XEMultiplicityAlgo(MultiplicityAlgo):
    def __init__(self, name, algoId, threshold, nbits, classtype = "EnergyThreshold"):
        super(XEMultiplicityAlgo, self).__init__( classtype = classtype, name=name, 
                                                  algoId = algoId, 
                                                  threshold = threshold, 
                                                  input=None, output="%s" % threshold,
                                                  nbits=nbits)


class MuMultiplicityAlgo(MultiplicityAlgo):
    def __init__(self, classtype, name, algoId, input, output, nbits):
        super(MuMultiplicityAlgo, self).__init__(classtype=classtype, name=name, algoId=algoId, input=input, output=output, nbits=nbits)

    def configureFromThreshold(self, thr):
        pass
