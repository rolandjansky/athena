# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
from AthenaCommon.Configurable import ConfigurableService,ConfigurableAlgorithm
import GaudiKernel.GaudiHandles as GaudiHandles
import ast
from collections import defaultdict

class ComponentAccumulator(object): 
    
    def __init__(self):
        self._msg=logging.getLogger('ComponentAccumulator')
        self._eventAlgs={}     #Unordered list of event processing algorithms per sequence + their private tools 
        self._conditionsAlgs=[]                #Unordered list of conditions algorithms + their private tools 
        self._services=[]                      #List of service, not yet sure if the order matters here in the MT age
        self._conditionsInput=set()            #List of database folder (as string), eventually passed to IOVDbSvc
        self._eventInputs=set()                #List of items (as strings) to be read from the input (required at least for BS-reading).
        self._outputPerStream={}               #Dictionary of {streamName,set(items)}, all as strings
        pass


    def addEventAlgo(self,algo,sequence="AthAlgSeq"):
        if not isinstance(algo, ConfigurableAlgorithm):
            self._msg.error("Attempt to add wrong type as event algorithm")
            #raise exception ..
            pass

        if sequence not in self._eventAlgs.keys():
            self._msg.info("Adding EventAlg sequence %s to the job" % sequence)
            self._eventAlgs[sequence]=[]
            pass
        self._eventAlgs[sequence].append(algo)
        pass


    def getEventAlgo(self,name,sequence="TopSequence"):
        #Fixme, deduplicate here? 
        hits=[a for a in self._eventAlgs if a.getName()==name]
        return hits

    def addCondAlgo(self,algo):
        if not isinstance(algo, ConfigurableAlgorithm):
            self._msg.error("Attempt to add wrong type as conditions algorithm")
            #raise exception ..
            pass
        self._conditionsAlgs.append(algo)
        pass

    def getCondAlgo(self,name):
        #Fixme, deduplicate here? 
        hits=[a for a in self._conditionsAlgs if a.getName()==name]
        return hits

    def addService(self,newSvc):
        if not isinstance(newSvc,ConfigurableService):
            self._msg.error("Attempt to add wrong type as service")
            #raise exception ..
            pass
        #self._services.append(svc)

        #Check for duplicates:
        for svc in self._services:
            if svc.getType()==newSvc.getType():
                #Found service of the same type
                if svc.getJobOptName()==newSvc.getJobOptName():
                    #Found service of the same type and name:
                    if (svc==newsvc):
                        #the new service is an identical copy of an existing one. Ignore it.
                        return
                    else:
                        #Now it gets tricky: Same name but different configuration
                        #Loop over properties:
                        for prop in svc.getProperties():
                            if not prop.startswith('_'):
                                oldprop=getattr(svc,prop)
                                newprop=getattr(newsvc,prop)
                                if (oldprop!=newprop):
                                    #found property mismatch
                                    if isinstance(oldprop,list): #if properties are concatinable, do that!
                                        oldprop+=newprop
                                        setattr(svc,oldprop)
                                    else:
                                        self._msg.error("service '%s' defined mutiple times with mismatching configuraton" % svcs[i].getJobOptName())
                                        return None
                                    pass 
                                #end if prop-mismatch
                            #end if startswith("_")
                        #end loop over properties
                    #end else
                return # We found a service of the same type and name and could reconcile the two instances
                #end if same name
            #end if same type
        

        #No duplication, simply append 
        self._services.append(newSvc)
        pass

    def getService(self,name):
        hits=[a for a in self._services if a.getName()==name]
        return hits

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
        #self._services+=other._services
        self._conditionsInput|=other._conditionsInput

        for svc in other._services:
            self.addService(svc) #Profit from deduplicaton here


        for k in other._outputPerStream.keys():
            if k in self._outputPerStream:
                self._outputPerStream[k].update(other._outputPerStream[k])
            else: #New stream type
                self._outputPerStream[k]=other._outputPerStream[k]
                
                

    def __iadd__(self,other):
        self.merge(other)
        return self

 
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
                    #print "Appending ",v1.getJobOptName(),"to", name, k
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
        for (seqName,algoList) in self._eventAlgs.iteritems():        
            evtalgseq=[]
            for alg in algoList:
                self.appendConfigurable(alg)
                evtalgseq.append(alg.getFullName())

            self._jocat[seqName]["Members"]=str(evtalgseq)


        #Conditions Algorithms:
        condalgseq=[]
        for alg in self._conditionsAlgs:
            self.appendConfigurable(alg)
            condalgseq.append(alg.getFullName())
        self._jocat["AthCondSeq"]["Members"]=str(condalgseq)


        svcList=ast.literal_eval(self._jocfg["ApplicationMgr"]["ExtSvc"])
        for svc in self._services:
            svcname=svc.getJobOptName()
            svcList.append(svc.getFullName())
            #for k, v in svc.getValuedProperties().items():
            #    self._jocat[svcname][k]=str(v)
            self.appendConfigurable(svc)
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
            
