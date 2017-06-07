from AthenaCommon.Logging import logging
from AthenaCommon.Configurable import ConfigurableService,ConfigurableAlgorithm
import GaudiKernel.GaudiHandles as GaudiHandles
import ast

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
                
    def __iadd__(self,other):
        merge(other)


    def appendTool(self,tool,jocat):
        toolname=tool.getJobOptName() #FIXME: Don't overwrite duplicates! 
        for k, v in tool.getValuedProperties().items():
            if isinstance(v,GaudiHandles.GaudiHandle):
                jocat[toolname][k]=v.getJobOptName()
                self.appendTool(v,jocat)
            elif isinstance(v,GaudiHandles.GaudiHandleArray):
                jocat[toolname][k]=[]
                for v1 in v:
                    jocat[algname][k].append(v1.getJobOptName())
                    self.appendTool(v1,jocat)
            else:
                jocat[toolname][k]=str(v)


    def appendConfigurable(self,confElem):
        name=confElem.getJobOptName() #FIXME: Don't overwrite duplicates! 
        for k, v in confElem.getValuedProperties().items():
            if isinstance(v,GaudiHandles.GaudiHandle):
                self._jocat[name][k]=v.getJobOptName()
                self.appendConfigurable(v)
            elif isinstance(v,GaudiHandles.GaudiHandleArray):
                #self._jocat[name][k]=[]#[ v1.getJobOptName() for v1 in v ]
                children=[]
                for v1 in v:
                    children.append(v1.getFullName())
                    print "Appending ",v1.getJobOptName(),"to", name, k
                    self.appendConfigurable(v1)
                self._jocat[name][k]=str(children)
            else:
                self._jocat[name][k]=str(v)
            
        
        return
        

    def store(self,outfile):
        from AthenaCommon.Utils.unixtools import find_datafile
        from collections import defaultdict
        import pickle
        #first load basics from the bootstrap-pickle
        bsfilename=find_datafile("bootstrap.pkl")
        bsfile=open(bsfilename)
        self._jocat=pickle.load(bsfile)
        self._jocfg=pickle.load(bsfile)
        self._pycomps=pickle.load(bsfile)
        bsfile.close()


        #EventAlgorithms
        evtalgseq=[]
        for alg in  self._eventAlgs:
            self.appendConfigurable(alg)
            evtalgseq.append(alg.getFullName())
        #print "EventAlgs:",evtalgseq
        self._jocat["AthAlgSeq"]["Members"]=str(evtalgseq)


        #Conditions Algorithms:
        condalgseq=[]
        for alg in self._conditionsAlgs:
            self.appendConfigurable(alg)
            condalgseq.append(alg.getFullName())
        self._jocat["AthCondSeq"]["Members"]=str(condalgseq)

        #Service:
        svcList=ast.literal_eval(self._jocfg["ApplicationMgr"]["ExtSvc"])
        for svc in self._services:
            svcname=svc.getJobOptName()
            svcList.append(svc.getFullname())
            for k, v in svc.getValuedProperties().items():
                self._jocat[svcname][k]=str(v)
        self._jocfg["ApplicationMgr"]["ExtSvc"]=str(svcList)
        pickle.dump( self._jocat, outfile ) 
        pickle.dump( self._jocfg, outfile ) 
        pickle.dump( self._pycomps, outfile )     


    def run(self,nEvents):
        from AthenaCommon.AppMgr import theApp
        #theApp.setup()
        
        print theApp
        topSeq=theApp.TopAlg

        
        for alg in self._eventAlgs:
            theApp.TopAlg+=alg

        theApp.run()
            
