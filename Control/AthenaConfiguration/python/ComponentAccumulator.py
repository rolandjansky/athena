

from AthenaCommon.Logging import logging
from AthenaCommon.Configurable import ConfigurableService,ConfigurableAlgorithm

class ComponentAccumulator(object): 
    
    def __init__(self):
        self._msg=logging.getLogger('ComponentAccumulator')
        self._eventAlgs=[]           #Unordered list of event processing algorithms + their private tools 
        self._conditionsAlgs=[]      #Unordered list of conditions algorithms + their private tools 
        self._services=[]            #List of service, not yet sure if the order matters here in the MT age
        self._conditionsInput=set()  #List of database folder (as string), eventually passed to IOVDbSvc
        self._eventInputs=set()      #List of items (as strings) to be read from the input (required at least for BS-reading).
        self._outputPerStream={}     #Dictionary of {streamName,set(items)}, all as strings
        pass


    def addEventAlgo(self,algo):
        if not isinstance(algo, ConfigurableAlgorithm):
            self._msg.error("Attempt to add wrong type as event algorithm")
            #raise exception ..
            pass
        self._eventAlgs.append(algo)
        pass

    def addCondAlgo(self,algo):
        if not isinstance(algo, ConfigurableAlgorithm):
            self._msg.error("Attempt to add wrong type as conditions algorithm")
            #raise exception ..
            pass
        self._conditionsAlgs.append(algo)
        pass

    def addService(self,svc):
        if not isinstance(svc,ConfigurableService):
            self._msg.error("Attempt to add wrong type as service")
            #raise exception ..
            pass
        self._services.append(svc)
        pass

    def addConditionsInput(self,condObj):
        #That's a string, should do some sanity checks on formatting
        self._conditionsInput.add(condObj);
        pass

    def addEventInput(self,condObj):
        #That's a string, should do some sanity checks on formatting
        self._eventInput.add(condObj);
        pass



    def addOutputToStream(self,streamName,outputs):
        
        if hasattr(outputs,'__iter__'):
            toAdd=list(outputs)
        else:
            toAdd=[outputs,]

        if streamName in self._outputsPerStream:
            self._outputsPerStream[streamName].update(set(outputs))
        else:
            self._outputsPerStream[streamName]=set(outputs)
            
        pass


    def merge(self,other):
        if not isinstance(other,ComponentAccumulator):
            self._msg.error("Attempt merge wrong type")
            #raise exception
            pass
        
        self._eventAlgs+=other._eventAlgs
        self._conditionsAlgs+=other._conditionsAlgs
        self._services+=other._services
        self._conditionsInput|=other._conditionsInputs

        for k in other._outputPerStreams.keys():
            if k in self._outputPerStream:
                self._outputPerStream[k].update(other._outputPerStream[k])
            else: #New stream type
                self._outputPerStream[k]=other._outputPerStream[k]
                
