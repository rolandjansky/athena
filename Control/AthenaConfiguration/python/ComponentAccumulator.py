# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#from AthenaCommon.Logging import logging
from AthenaConfiguration.CfgLogMsg import cfgLogMsg
from AthenaCommon.Configurable import Configurable,ConfigurableService,ConfigurableAlgorithm,ConfigurableAlgTool

import GaudiKernel.GaudiHandles as GaudiHandles
import ast
from collections import defaultdict
from copy import deepcopy


class DeduplicatonFailed(RuntimeError):
    pass

class ConfigurationError(RuntimeError):
    pass 

class CurrentSequence:
    sequence = 'AthAlgSeq'
    @staticmethod
    def set( newseq ):
        CurrentSequence.sequence = newseq

    @staticmethod
    def get():
        return CurrentSequence.sequence


class ComponentAccumulator(object): 

    def __init__(self):
        self._msg=cfgLogMsg  #logging.getLogger('ComponentAccumulator')
        self._eventAlgs={}     #Unordered list of event processing algorithms per sequence + their private tools         
        self._conditionsAlgs=[]                #Unordered list of conditions algorithms + their private tools 
        self._services=[]                      #List of service, not yet sure if the order matters here in the MT age
        self._conditionsInput=set()            #List of database folder (as string), eventually passed to IOVDbSvc
        self._eventInputs=set()                #List of items (as strings) to be read from the input (required at least for BS-reading).
        self._outputPerStream={}               #Dictionary of {streamName,set(items)}, all as strings

        #Properties of the ApplicationMgr
        self._theAppProps=dict()            

        #Backward compatiblity hack: Allow also public tools:
        self._publicTools=[]
        pass    

    def printConfig(self, withDetails=False):
        self._msg.info( "Event Inputs" )
        self._msg.info( self._eventInputs )
        self._msg.info( "Event algs" )
        for seq, algs in self._eventAlgs.iteritems():
            self._msg.info( " Sequence " +seq )            
            self._msg.info( "   "+  ' '.join( map( lambda x: x.getFullName(), algs)) )
        self._msg.info( "Conditions Inputs" )
        self._msg.info( self._conditionsInput )
        self._msg.info( "Conditions Algs" )
        self._msg.info( [ a.getName() for a in self._conditionsAlgs ] )
        self._msg.info( "Services" )
        self._msg.info( [ s.getName() for s in self._services ] )
        self._msg.info( "Outputs" )
        self._msg.info( self._outputPerStream )

    def addSequence(self, newseq, sequence = CurrentSequence.get() ):
        """ Adds new sequence. If second argument is present then it is added under another sequence  """
        
        def __findInSequenceTree( start, seq ):
            for sub in start

        if newseq not in self._eventAlgs.keys():
            self._msg.info("Adding Sequence sequence %s to the job" % sequence)
            self._eventAlgs[newseq]=[]
            


            pass
        else:
            raise ConfigurationError("Sequence %s already present" % newseq )
        
        pass


    def addEventAlgo(self, algo, sequence = CurrentSequence.get() ):                
        if not isinstance(algo, ConfigurableAlgorithm):
            raise TypeError("Attempt to add wrong type as event algorithm")
            pass

        if sequence not in self._eventAlgs.keys():
            self._msg.info("Adding EventAlg sequence %s to the job" % sequence)
            self._eventAlgs[sequence]=[]
            pass
        self._msg.debug("Adding EventAlgo %s to sequence %s" % (algo.getFullName(), sequence))
        self._eventAlgs[sequence].append(algo)
        pass


    def getEventAlgo(self,name,sequence= CurrentSequence.get() ):
        hits=[a for a in self._eventAlgs[sequence] if a.getName()==name]
        if (len(hits)>1):
            print hits
            raise ConfigurationError("More than one Algorithm with name %s found in sequence %s" %(name,sequence))
        
        return hits[0]

    def addCondAlgo(self,algo):
        if not isinstance(algo, ConfigurableAlgorithm):
            raise TypeError("Attempt to add wrong type as conditions algorithm")
            pass
        self._deduplicate(algo,self._conditionsAlgs) #will raise on conflict
        return
        

    def getCondAlgo(self,name):
        hits=[a for a in self._conditionsAlgs if a.getName()==name]
        if (len(hits)>1):
            raise ConfigurationError("More than one Algorithm with name %s found in sequence %s" %(name,sequence))
        return hits[0]

    def addService(self,newSvc):
        if not isinstance(newSvc,ConfigurableService):
            raise TypeError("Attempt to add wrong type as service")
            pass
        self._deduplicate(newSvc,self._services)  #will raise on conflict
        return 


    def addPublicTool(self,newTool):
        if not isinstance(newTool,ConfigurableAlgTool):
            raise TypeError("Attempt to add wrong type as AlgTool")
        newTool.setParent("ToolSvc")
        self._deduplicate(newTool,self._publicTools)
        return

    def _deduplicate(self,newComp,compList):
        #Check for duplicates:
        for comp in compList:
            if comp.getType()==newComp.getType() and comp.getJobOptName()==newComp.getJobOptName():
                #Found component of the same type and name
                #print "Checking ", comp.getType(), comp.getJobOptName()
                allProps=frozenset(comp.getValuedProperties().keys()+newComp.getValuedProperties().keys())
                for prop in allProps:
                    if not prop.startswith('_'):
                        try:
                            oldprop=getattr(comp,prop)
                        except AttributeError:
                            oldprop=None
                        try:
                            newprop=getattr(newComp,prop)
                        except AttributeError:
                            newprop=None

                        #Note that getattr for a list property works, even if it's not in ValuedProperties
                        if (oldprop!=newprop):
                            #found property mismatch
                            if isinstance(oldprop,list): #if properties are concatinable, do that!
                                oldprop+=newprop
                                #print "concatinating list-property",comp.getJobOptname(),prop
                                setattr(comp,prop,oldprop)
                            else:
                                #self._msg.error("component '%s' defined mutiple times with mismatching configuraton" % svcs[i].getJobOptName())
                                raise DeduplicationFailed("component '%s' defined mutiple times with mismatching property %s" % \
                                                                  comp.getJobOptName(),prop)
                            pass 
                            #end if prop-mismatch
                        pass
                        #end if startswith("_")
                    pass
                    #end loop over properties
                #We found a service of the same type and name and could reconcile the two instances
                self._msg.debug("Reconciled configuration of component %s" % comp.getJobOptName())
                return False #False means nothing got added
            #end if same name & type
        #end loop over existing components

        #No component of the same type & name found, simply append 
        self._msg.debug("Adding service/Tool/CondAlog %s to the job" % newComp.getFullName())
        compList.append(newComp)
        return True #True means something got added
    
    def getService(self,name):
        for svc in self._services: 
            if svc.getName()==name:
                return svc
        raise KeyError("No service with name %s known" % name)


    def getPublicTool(self,name):
        for pt in self._publicTools:
            if pt.getName()==name:
                return pt
        raise KeyError("No public tool with name %s known" % name)


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


    def setAppProperty(self,key,value):
        if self._theAppProps.has_key(key) and  self._theAppProps[key]!=value:
            #Not sure if we should allow that ...
            self._msg.info("ApplicationMgr property '%s' already set to '%s'. Overwriting with %s"% (key,str(self._theAppProps[key]),str(value)))
        self._theAppProps[key]=value
        pass

    def __merge(self,other):        
        if not isinstance(other,ComponentAccumulator):
            raise TypeError("Attempt merge wrong type. Only instances of ComponentAccumulator can be added")
        
        # Merge Algorithms per sequence
        allkeys=set(self._eventAlgs.keys()) | set(other._eventAlgs.keys())
        for k in allkeys:
            if not self._eventAlgs.has_key(k): 
                self._eventAlgs[k]=other._eventAlgs[k] #sequence only known to other
            elif not other._eventAlgs.has_key[k]: #sequence only known to self
                pass 
            else: #sequence known to both self and other
                self._eventAlgs[k]+=other._eventAlgs[k] 


        # Merge Conditions inputs
        self._conditionsInput|=other._conditionsInput
        
        #self._conditionsAlgs+=other._conditionsAlgs
        for condAlg in other._conditionsAlgs:
            self.addCondAlgo(condAlgo) #Profit from deduplicaton here


        for svc in other._services:
            self.addService(svc) #Profit from deduplicaton here

        for pt in other._publicTools:
            self.addPublicTool(pt) #Profit from deduplicaton here


        for k in other._outputPerStream.keys():
            if k in self._outputPerStream:
                self._outputPerStream[k].update(other._outputPerStream[k])
            else: #New stream type
                self._outputPerStream[k]=other._outputPerStream[k]
                
        #Merge AppMgr properties:
        for (k,v) in other._theAppProps.iteritems():
            self.setAppProperty(k,v)  #Will warn about overrides

    def __iadd__(self,other):
        self.__merge(other)
        return self

    def executeModule(self,fct,configFlags, withSequence=CurrentSequence.get(), *args,**kwargs):
        currentDefaultSequence = CurrentSequence.get()
        CurrentSequence.set( withSequence )
        cfconst=deepcopy(configFlags)
        self._msg.info("Excuting configuration function %s" % fct.__name__)
        cm=fct(cfconst,*args,**kwargs)
        self.__merge(cm)
        CurrentSequence.set( currentDefaultSequence )
 
    def appendConfigurable(self,confElem):
        name=confElem.getJobOptName() #FIXME: Don't overwrite duplicates! 
        #Hack for public Alg tools, drop multiple mentions of ToolSvc 
        nTS=0
        for n in name.split("."):
            if n=="ToolSvc":
                nTS+=1
            else:
                break
        if nTS>2:
            name=".".join(name.split(".")[nTS-1:])
        for k, v in confElem.getValuedProperties().items():
            if isinstance(v,Configurable):
                self._jocat[name][k]=v.getFullName()
            elif isinstance(v,GaudiHandles.GaudiHandleArray):
                self._jocat[name][k]=str([ v1.getFullName() for v1 in v ])
                #print name,k,self._jocat[name][k]
            else:
                self._jocat[name][k]=str(v)
        #print "All Children:",confElem.getAllChildren()
        for ch in confElem.getAllChildren():
            self.appendConfigurable(ch)
        return


    def store(self,outfile,nEvents=10):
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


        #Public Tools:
        for pt in self._publicTools:
            #print "Appending public Tool",pt.getFullName(),pt.getJobOptName()
            self.appendConfigurable(pt)



        svcList=ast.literal_eval(self._jocfg["ApplicationMgr"]["ExtSvc"])
        for svc in self._services:
            svcname=svc.getJobOptName()
            svcList.append(svc.getFullName())
            #for k, v in svc.getValuedProperties().items():
            #    self._jocat[svcname][k]=str(v)
            self.appendConfigurable(svc)
        self._jocfg["ApplicationMgr"]["ExtSvc"]=str(svcList)

        self._jocfg["ApplicationMgr"]["EvtMax"]=nEvents

        #Hack for now:   
        self._jocfg["ApplicationMgr"]["CreateSvc"]=['ToolSvc/ToolSvc', 'AthDictLoaderSvc/AthDictLoaderSvc', 'AthenaSealSvc/AthenaSealSvc', 'CoreDumpSvc/CoreDumpSvc','GeoModelSvc']

        for (k,v) in self._theAppProps.iteritems():
            self._jocfg["ApplicationMgr"][k]=v


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
            
