# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#from AthenaCommon.Logging import logging
from AthenaConfiguration.CfgLogMsg import cfgLogMsg
from AthenaCommon.Configurable import Configurable,ConfigurableService,ConfigurableAlgorithm,ConfigurableAlgTool
from AthenaCommon.CFElements import isSequence,findSubSequence,findAlgorithm
from AthenaCommon.AlgSequence import AlgSequence

import GaudiKernel.GaudiHandles as GaudiHandles
import ast
from collections import defaultdict
from copy import deepcopy


class DeduplicatonFailed(RuntimeError):
    pass

class ConfigurationError(RuntimeError):
    pass 

class CurrentSequence:
    sequence = AlgSequence()

    @staticmethod
    def set( newseq ):
        #print "CurrentSequence set.... ", newseq.name()
        CurrentSequence.sequence = newseq

    @staticmethod
    def get(  ):
        #print "CurrentSequence ....get %s " %  CurrentSequence.sequence.name()
        return CurrentSequence.sequence


class ComponentAccumulator(object): 

    def __init__(self):
        self._msg=cfgLogMsg  #logging.getLogger('ComponentAccumulator')
        self._sequence=CurrentSequence.get()
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
        self._msg.info( "Event Algorithm Sequences" )
        self._msg.info( self._sequence )        
        self._msg.info( "Conditions Inputs" )
        self._msg.info( self._conditionsInput )
        self._msg.info( "Condition Algorithms" )
        self._msg.info( [ a.getName() for a in self._conditionsAlgs ] )
        self._msg.info( "Services" )
        self._msg.info( [ s.getName() for s in self._services ] )
        self._msg.info( "Outputs" )
        self._msg.info( self._outputPerStream )

    def addSequence(self, newseq, sequenceName = CurrentSequence.get().name() ):
        """ Adds new sequence. If second argument is present then it is added under another sequence  """
        
        seq = findSubSequence(self._sequence, sequenceName )
        if seq == None:
            raise ConfigurationError("Missing sequence %s to add new sequence to" % sequenceName )                
        if findSubSequence( self._sequence, newseq.name() ):
            raise ConfigurationError("Sequence %s already present" % newseq.name() )
        seq += newseq

    def addEventAlgo(self, algo ):                
        if not isinstance(algo, ConfigurableAlgorithm):
            raise TypeError("Attempt to add wrong type: %s as event algorithm" % type( algo ).__name__)
            pass
        seq = CurrentSequence.get()
        seq += algo
        self._msg.debug("  Adding %s to sequence %s" % ( algo.getFullName(), seq.name()) )
        pass


    def getEventAlgo(self,name):
        """ Looks for an algorithm given the name in all sequences """
        algo = findAlgorithm( AlgSeqeunce(), name )
        if algo == None:
            raise ConfigurationError("Can not find an algorithm of name %d ", name)
        return algo

    def addCondAlgo(self,algo):
        if not isinstance(algo, ConfigurableAlgorithm):
            raise TypeError("Attempt to add wrong type: %s as conditions algorithm" % type( algo ).__name__)
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
            raise TypeError("Attempt to add wrong type: %s as service" % type( newSvc ).__name__)
            pass
        self._deduplicate(newSvc,self._services)  #will raise on conflict
        return 


    def addPublicTool(self,newTool):
        if not isinstance(newTool,ConfigurableAlgTool):
            raise TypeError("Attempt to add wrong type: %s as AlgTool" % type( newTool ).__name__)
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
                                #print "concatenating list-property",comp.getJobOptname(),prop
                                setattr(comp,prop,oldprop)
                            else:
                                #self._msg.error("component '%s' defined multiple times with mismatching configuration" % svcs[i].getJobOptName())
                                raise DeduplicationFailed("component '%s' defined multiple times with mismatching property %s" % \
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
        """ Merging in the other accumulator """
        if not isinstance(other,ComponentAccumulator):
            raise TypeError("Attempt merge wrong type %s. Only instances of ComponentAccumulator can be added" % type(other).__name__)
                
        
        #destSubSeq = findSubSequence(self._sequence, sequence)
        #if destSubSeq == None:
        #    raise ConfigurationError( "Nonexistent sequence %s in %s (or its sub-sequences)" % ( sequence, self._sequence.name() ) )          #     
        # def mergeSequences( dest, src ):        
        #     for c in src.getChildren():
        #         if isSequence( c ):
        #             sub = findSubSequence( dest, c.name() )
        #             if sub:
        #                 mergeSequences(sub, c )
        #             else:
        #                 self._msg.debug("  Merging sequence %s to a sequnece %s" % ( c.name(), dest.name() ) )          
        #                 dest += c
        #         else: # an algorithm
        #             existingAlg = findAlgorithm( dest, c.name(), depth=1 )
        #             if existingAlg:
        #                 if existingAlg != c: # if it is the same we can just skip it, else this indicates an error
        #                     raise ConfigurationError( "Duplicate algorithm %s in source and destination sequences %s" % ( c.name(), src.name()  ) )           
        #             else: # absent, adding
        #                 self._msg.debug("  Merging algorithm %s to a sequnece %s" % ( c.name(), dest.name() ) )          
        #                 dest += c
                        
        # mergeSequences( destSubSeq, other._sequence )
        
        #mergeSequences(self._sequences, other._sequences)
        # Merge Algorithms per sequence
        # allkeys=set(self._eventAlgs.keys()) | set(other._eventAlgs.keys())
        # for k in allkeys:
        #     if not self._eventAlgs.has_key(k): 
        #         self._eventAlgs[k]=other._eventAlgs[k] #sequence only known to other
        #     elif not other._eventAlgs.has_key[k]: #sequence only known to self
        #         pass 
        #     else: #sequence known to both self and other
        #         self._eventAlgs[k]+=other._eventAlgs[k] 


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

    
    def executeModule(self,fct,configFlags, sequence=None, *args,**kwargs):
        """ The heart and soul of configuration system. You need to read the whole documentation. 

        This method eliminates possibility that a downstream configuration alters the upstream one. 
        It is done by a two-fold measures:
        - the flags are cloned so downstream access only the copy of the flags
        - the sub-accumulators can not access the upstream accumulators and thus alter any configuration.
          The combination process is defined in the __merge method of this class. Less flexibility == robustness.
        """
        
        currentSeq = seq = CurrentSequence.get()
        if sequence:            
            seq = findSubSequence(seq, sequence )            
            if seq == None:
                raise ConfigurationError("Can not add algorithms to sequence %s as it does not exist" % sequence )            
            CurrentSequence.set( seq )

        cfconst=deepcopy(configFlags)
        self._msg.info("Executing configuration function %s" % fct.__name__)
        subAccumulator = fct( cfconst, *args,**kwargs )
        self.__merge( subAccumulator )

        CurrentSequence.set( currentSeq )
        
 
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


        self._jocat["AthAlgSeq"] = self._sequence
        #EventAlgorithms
        # for (seqName,algoList) in self._eventAlgs.iteritems():        
        #     evtalgseq=[]
        #     for alg in algoList:
        #         self.appendConfigurable(alg)
        #         evtalgseq.append(alg.getFullName())

        #     self._jocat[seqName]["Members"]=str(evtalgseq)


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

        print self._jocat

        pickle.dump( self._jocat, outfile ) 
        pickle.dump( self._jocfg, outfile ) 
        pickle.dump( self._pycomps, outfile )     


    def run(self,nEvents):
        from AthenaCommon.AppMgr import theApp
        #theApp.setup()
        
        #        print theApp
        #        topSeq=theApp.TopAlg

        
        #        for alg in self._eventAlgs:
        #            theApp.TopAlg+=alg

        theApp.run()

# self test            
if __name__ == "__main__":
    # trivial case without any nested sequences
    from AthenaCommon.Configurable import ConfigurablePyAlgorithm as Algo # guinea pig algorithms
    from AthenaConfiguration.ConfigFlags import ConfigFlagContainer
    from AthenaCommon.CFElements import *
    cfgLogMsg.setLevel("debug")

    def AlgsConf1(flags):
        acc = ComponentAccumulator()
        acc.addEventAlgo( Algo("Algo1")  )
        acc.addEventAlgo( Algo("Algo2")  )
        return acc

    def AlgsConf2(flags):
        acc = ComponentAccumulator()
        acc.executeModule( AlgsConf1, flags )
        acc.addEventAlgo( Algo("Algo3") )
        return acc

    acc = ComponentAccumulator()
    flags=ConfigFlagContainer()
    
    acc.executeModule( AlgsConf2, flags )
    # checks
    assert findAlgorithm(AlgSequence(), "Algo1", 1), "Algorithm not added to a top sequence"
    assert findAlgorithm(AlgSequence(), "Algo2", 1), "Algorithm not added to a top sequence"
    assert findAlgorithm(AlgSequence(), "Algo3", 1), "Algorithm not added to a top sequence"
    print( "Simple Configuration construction OK ")

    def AlgsConf3(flags):
        acc = ComponentAccumulator()
        acc.addEventAlgo( Algo("NestedAlgo1") )
        return acc

    def AlgsConf4(flags):
        acc = ComponentAccumulator()
        acc.executeModule( AlgsConf3, flags )
        acc.addEventAlgo( Algo("NestedAlgo2", OutputLevel=7) )
        return acc


    acc.addSequence( seqAND("subSequence1") )
    acc.addSequence( parOR("subSequence2") )
    assert findSubSequence(AlgSequence(), "subSequence1"), "Adding sub-sequence failed"
    assert findSubSequence(AlgSequence(), "subSequence2"), "Adding sub-sequence failed"

    acc.addSequence( seqAND("sub2Sequence1"), "subSequence1")
    assert findSubSequence(AlgSequence(), "sub2Sequence1"), "Adding sub-sequence failed"
    assert findSubSequence( findSubSequence(AlgSequence(), "subSequence1"), "sub2Sequence1" ), "Adding sub-sequence doen in a wrong place"

    acc.executeModule( AlgsConf4, flags, sequence="subSequence1" )    
    assert findAlgorithm(AlgSequence(), "NestedAlgo1" ), "Algorithm added to nested seqeunce"
    assert findAlgorithm(AlgSequence(), "NestedAlgo1", 1 ) == None, "Algorithm mistakenly in top sequence"
    assert findAlgorithm( findSubSequence(AlgSequence(), "subSequence1"), "NestedAlgo1", 1 ), "Algorithm not in right sequence"
    print( "Complex sequences construction also OK ")

    
    #acc.printConfig()

    # try recording
    acc.store(open("testFile.pkl", "w"))
    f = open("testFile.pkl")
    import pickle
    u = pickle.load(f)
    srcSeq    = flatAlgorithmSequences( acc._sequence )
    storedSeq = flatAlgorithmSequences( u["AthAlgSeq"] )
    def sameAlg(a1, a2):
        return a1.getFullName() == a2.getFullName()\
            and a1.properties() == a2.properties()
    

    for k,v in srcSeq.iteritems():
        assert storedSeq.has_key(k), "Missing sequence in stored pickle"
        assert len(storedSeq[k]) == len(srcSeq[k]) , "Not the same number of algorithms in store and src"
        for a1, a2 in zip(storedSeq[k], srcSeq[k]):            
            assert sameAlg(a1, a2), "Differences in alg. congfig"
    print( "Sequences survived pickling" )

    #acc.run(1)
    print( "\nAll OK" )

