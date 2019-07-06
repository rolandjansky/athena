# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags
import re

log = logging.getLogger("Menu.L1.Base.TopoAlgos") 

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

    def isSortingAlg(self):
        return False

    def isDecisionAlg(self):
        return False

    def isMultiplicityAlg(self):
        return False


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
        from collections import OrderedDict as odict
        confObj = odict()
        confObj["algId"] = self.algoId
        return confObj

    def getEnergyScaleEM(self):
        emscale=2
        if hasattr(TriggerFlags, 'useRun1CaloEnergyScale'):
            if TriggerFlags.useRun1CaloEnergyScale :
                emscale=1
                log.info("Trigger flag useRun1CaloEnergyScale was set to true. Using EM energy scale %i", emscale)  
        return emscale
    
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
            if self.outputs.find("TAU")>=0:
                self.inputvalue= self.inputvalue.replace("Cluster","Tau")            
            if self.outputs.find("EM")>=0:
                self.inputvalue= self.inputvalue.replace("Cluster","Em")

    def isSortingAlg(self):
        return True
        
    def json(self):
        from collections import OrderedDict as odict
        confObj = odict()
        confObj["algId"] = self.algoId
        confObj["output"] = self.outputs
        confObj["type"] = self.classtype
        confObj["fixedParameters"] = odict()
        confObj["fixedParameters"]["input"] = odict([(self.inputs,self.inputvalue)])
        confObj["fixedParameters"]["output"] = odict([("TobArrayOut",self.outputs)])
        confObj["fixedParameters"]["generics"] = [(genParm.name, genParm.value) for genParm in self.generics]

        confObj["fixedParameters"]["generics"] = odict()
        for (pos, genParm) in enumerate(self.generics):
            confObj["fixedParameters"]["generics"][genParm.name] = odict([("value", genParm.value), ("position", pos)]) 

        confObj["variableParameters"] = list()
        _emscale_for_decision = self.getEnergyScaleEM()
        _mu_for_decision=1 # MU4->3GeV, MU6->5GeV, MU10->9GeV
        for (pos, variable) in enumerate(self.variables): 
            # adjust MinET if outputs match with EM or TAU or MU  ==> this is a terrible hack that won't fly in Run 3
            if variable.name == "MinET":
                if "TAU" in self.outputs or "EM" in self.outputs:
                    variable.value *= _emscale_for_decision
                if "MU" in self.outputs and variable.value > _mu_for_decision:
                    variable.value -= _mu_for_decision
            confObj["variableParameters"].append(odict([("name", variable.name),("value", variable.value)]))
        return confObj




class DecisionAlgo(TopoAlgo):

    def __init__(self, classtype, name, inputs, outputs, algoId):
        super(DecisionAlgo, self).__init__(classtype=classtype, name=name, algoId=algoId)
        self.inputs = inputs if type(inputs)==list else [inputs]
        self.outputs = outputs if type(outputs)==list else [outputs]

    def isDecisionAlg(self):
        return True

    def json(self):
        from collections import OrderedDict as odict
        confObj = odict()
        confObj["algId"] = self.algoId
        confObj["type"] = self.classtype
        confObj["output"] = self.outputs # list of output names
        # fixed parameters
        confObj["fixedParameters"] = odict()
        if len(self.inputs)==1:
            confObj["fixedParameters"]["inputs"] = [ odict([ ("name","Tob"), ("value",self.inputs[0]), ("position",0) ]) ]
        else:
            confObj["fixedParameters"]["inputs"] = list()
            input_woovlp = set()
            for (tobid, input) in enumerate(self.inputs):
                if input in input_woovlp:
                    continue
                input_woovlp.add(input)
                confObj["fixedParameters"]["inputs"] += [ odict( [ ("name","Tob%i" % (tobid+1) ), ("value",input), ("position",tobid) ] ) ]
                
        #confObj["fixedParameters"]["outputs"] = [odict([("selection",bit),("name", name)]) for bit,name in enumerate(self.outputs)]
        confObj["fixedParameters"]["generics"] = odict()
        for (pos, genParm) in enumerate(self.generics):
            confObj["fixedParameters"]["generics"][genParm.name] = odict([("value", genParm.value), ("position", pos)]) 

        # variable parameters
        confObj["variableParameters"] = list()
        _emscale_for_decision = self.getEnergyScaleEM()
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

            if variable.selection >= 0:
                confObj["variableParameters"].append(odict([("name", variable.name), ("selection",variable.selection), ("value", variable.value)]))
            else:
                confObj["variableParameters"].append(odict([("name", variable.name), ("value", variable.value)]))

        return confObj


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
        from collections import OrderedDict as odict
        confObj = odict()
        confObj["threshold"] = self.threshold
        confObj["input"] = self.input
        confObj["output"] = self.outputs
        confObj["nbits"] = self.nbits
        return confObj


class EMMultiplicityAlgo(MultiplicityAlgo):
    def __init__(self, classtype, name, algoId, threshold, nbits):
        super(EMMultiplicityAlgo, self).__init__(classtype=classtype, name=name, 
                                                 algoId=algoId, 
                                                 threshold = threshold, 
                                                 input=None, output="%s" % threshold,
                                                 nbits=nbits)
        mres = re.match("(?P<type>[A-z]*)[0-9]*(?P<suffix>[VHI]*)",threshold).groupdict()
        self.input = mres["type"]


class TauMultiplicityAlgo(MultiplicityAlgo):
    def __init__(self, classtype, name, algoId, threshold, nbits):
        super(TauMultiplicityAlgo, self).__init__(classtype=classtype, name=name, 
                                                  algoId=algoId, 
                                                  threshold = threshold, 
                                                  input=None, output="%sMult" % threshold,
                                                  nbits=nbits)

class JetMultiplicityAlgo(MultiplicityAlgo):
    def __init__(self, classtype, name, algoId, threshold, nbits):
        super(JetMultiplicityAlgo, self).__init__(classtype=classtype, name=name, 
                                                  algoId=algoId, 
                                                  threshold = threshold, 
                                                  input=None, output="%sMult" % threshold,
                                                  nbits=nbits)

class XEMultiplicityAlgo(MultiplicityAlgo):
    def __init__(self, classtype, name, algoId, threshold, nbits):
        super(XEMultiplicityAlgo, self).__init__( classtype = classtype, name=name, 
                                                  algoId = algoId, 
                                                  threshold = threshold, 
                                                  input=None, output="%sMult" % threshold,
                                                  nbits=nbits)


class MuMultiplicityAlgo(MultiplicityAlgo):
    def __init__(self, classtype, name, algoId, input, output, nbits):
        super(MuMultiplicityAlgo, self).__init__(classtype=classtype, name=name, algoId=algoId, input=input, output=output, nbits=nbits)

    def configureFromThreshold(self, thr):
        pass
