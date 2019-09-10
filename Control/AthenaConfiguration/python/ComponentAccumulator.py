# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaCommon.Logging import logging
from AthenaCommon.Configurable import Configurable,ConfigurableService,ConfigurableAlgorithm,ConfigurableAlgTool
from AthenaCommon.CFElements import isSequence,findSubSequence,findAlgorithm,flatSequencers,findOwningSequence,\
    checkSequenceConsistency, findAllAlgorithmsByName
from AthenaCommon.AlgSequence import AthSequencer

import GaudiKernel.GaudiHandles as GaudiHandles

from AthenaConfiguration.Deduplication import deduplicate, deduplicateComponent, DeduplicationFailed

import ast
import collections
import six
import copy

from AthenaConfiguration.UnifyProperties import unifySet

class ConfigurationError(RuntimeError):
    pass

_servicesToCreate=frozenset(('GeoModelSvc','TileInfoLoader','DetDescrCnvSvc'))

def printProperties(msg, c, nestLevel = 0):
    # Iterate in sorted order.
    props = c.getValuedProperties()
    propnames = list(props.keys())
    propnames.sort()
    for propname in propnames:
        propval = props[propname]
        # Ignore empty lists
        if propval==[]:
            continue
        # Printing EvtStore could be relevant for Views?
        if propname in ["DetStore","EvtStore"]:
            continue

        propstr = str(propval)
        if isinstance(propval,GaudiHandles.PublicToolHandleArray):
            ths = [th.getFullName() for th in propval]
            propstr = "PublicToolHandleArray([ {0} ])".format(', '.join(ths))
        elif isinstance(propval,GaudiHandles.PrivateToolHandleArray):
            ths = [th.getFullName() for th in propval]
            propstr = "PrivateToolHandleArray([ {0} ])".format(', '.join(ths))
        elif isinstance(propval,ConfigurableAlgTool):
            propstr = propval.getFullName()
        msg.info( " "*nestLevel +"    * {0}: {1}".format(propname,propstr))
    return


class ComponentAccumulator(object):

    def __init__(self,sequenceName='AthAlgSeq'):
        self._msg=logging.getLogger('ComponentAccumulator')
        if not Configurable.configurableRun3Behavior:
            msg = "discovered Configurable.configurableRun3Behavior=False while working with ComponentAccumulator"
            self._msg.error(msg)
            raise ConfigurationError(msg)
        
        self._sequence=AthSequencer(sequenceName,Sequential=True)    #(Nested) default sequence of event processing algorithms per sequence + their private tools
        self._allSequences = [ self._sequence ] 
        self._algorithms = {}            #Flat algorithms list, useful for merging
        self._conditionsAlgs=[]          #Unordered list of conditions algorithms + their private tools
        self._services=[]                #List of service, not yet sure if the order matters here in the MT age
        self._privateTools=None          #A placeholder to carry a private tool(s) not yet attached to its parent
        self._primaryComp=None           #A placeholder to designate the primary service 

        self._theAppProps=dict()        #Properties of the ApplicationMgr

        #Backward compatiblity hack: Allow also public tools:
        self._publicTools=[]

        #To check if this accumulator was merged:
        self._wasMerged=False
        self._isMergable=True
        self._lastAddedComponent="Unknown" 

    def setAsTopLevel(self):
        self._isMergable = False


    def _inspect(self): #Create a string some basic info about this CA, useful for debugging
        summary="This CA contains {0} service, {1} conditions algorithms, {2} event algorithms and {3} public tools\n"\
            .format(len(self._services),len(self._conditionsAlgs),len(self._algorithms),len(self._publicTools))
        if (self._privateTools): 
            if (isinstance(self._privateTools, list)):
                summary+="  Private AlgTool: "+ self._privateTools[-1].getFullName()+"\n"
            else:
                summary+="  Private AlgTool: "+ self._privateTools.getFullName()+"\n"
        if (self._primaryComp): 
            summary+="  Primary Component: " + self._primaryComp.getFullName()+"\n" 
        summary+="  Last component added: "+self._lastAddedComponent+"\n" 
        return summary


    def empty(self):
        return len(self._sequence)+len(self._conditionsAlgs)+len(self._services)+\
            len(self._publicTools)+len(self._theAppProps) == 0

    def __del__(self):
         if not getattr(self,'_wasMerged',True) and not self.empty():
             #can't raise an exception in __del__ method (Python rules) so this is a warning
             log = logging.getLogger("ComponentAccumulator")
             log.error("This ComponentAccumulator was never merged!")
             log.error(self._inspect())
         if getattr(self,'_privateTools',None) is not None:
             log = logging.getLogger("ComponentAccumulator")
             log.error("Deleting a ComponentAccumulator with dangling private tool(s)")

        #pass



    def printCondAlgs(self, summariseProps=False):
        self._msg.info( "Condition Algorithms" )
        for c in self._conditionsAlgs:
            self._msg.info( " " +"\\__ "+ c.name() +" (cond alg)" )
            if summariseProps:
                printProperties(self._msg, c, 1)
        return
        

    def printConfig(self, withDetails=False, summariseProps=False):
        self._msg.info( "Event Inputs" )
        self._msg.info( "Event Algorithm Sequences" )


        if withDetails:
            self._msg.info( self._sequence )
        else:
            def printSeqAndAlgs(seq, nestLevel = 0):
                def __prop(name):
                    if name in seq.getValuedProperties():
                        return seq.getValuedProperties()[name]
                    return seq.getDefaultProperties()[name]

                self._msg.info( " "*nestLevel +"\\__ "+ seq.name() +" (seq: %s %s)",
                                "SEQ" if __prop("Sequential") else "PAR", "OR" if __prop("ModeOR") else "AND" )
                nestLevel += 3
                for c in seq.getChildren():
                    if isSequence(c):
                        printSeqAndAlgs(c, nestLevel )
                    else:
                        self._msg.info( " "*nestLevel +"\\__ "+ c.name() +" (alg)" )
                        if summariseProps:
                            printProperties(self._msg, c, nestLevel)

            for n,s in enumerate(self._allSequences):
                self._msg.info( "Top sequence {}".format(n) )
                printSeqAndAlgs(s)

        self.printCondAlgs (summariseProps = summariseProps)
        self._msg.info( "Services" )
        self._msg.info( [ s.getName() for s in self._services ] )
        self._msg.info( "Public Tools" )
        self._msg.info( "[" )
        for t in self._publicTools:
            self._msg.info( "  {0},".format(t.getFullName()) )
            # Not nested, for now
            if summariseProps:
                printProperties(self._msg, t)
        self._msg.info( "]" )
        self._msg.info( "Private Tools")
        self._msg.info( "[" )
        if (isinstance(self._privateTools, list)):
            for t in self._privateTools:
                self._msg.info( "  {0},".format(t.getFullName()) )
                # Not nested, for now
                if summariseProps:
                    printProperties(self._msg, t)
        else:
            if self._privateTools is not None:
                self._msg.info( "  {0},".format(self._privateTools.getFullName()) )
                if summariseProps:
                    printProperties(self._msg, self._privateTools)
        self._msg.info( "]" )
        self._msg.info( "TheApp properties" )
        for k,v in six.iteritems(self._theAppProps):
            self._msg.info("  {} : {}".format(k,v))


    def addSequence(self, newseq, parentName = None ):
        """ Adds new sequence. If second argument is present then it is added under another sequence  """
        if not isSequence(newseq):
            raise TypeError('{} is not a sequence'.format(newseq.name()))

        if parentName is None:
            parent=self._sequence
        else:
            parent = findSubSequence(self._sequence, parentName )
            if parent is None:
                raise ConfigurationError("Missing sequence %s to add new sequence to" % parentName )

        parent += newseq
        algsByName = findAllAlgorithmsByName(newseq)
        for name, existingAlgs in six.iteritems(algsByName):
            startingIndex = 0
            if name not in self._algorithms:
                firstAlg, parent, idx = existingAlgs[0]
                self._algorithms[name] = firstAlg
                startingIndex = 1
            for alg, parent, idx in existingAlgs[startingIndex:]:
                deduplicateComponent(self._algorithms[name], alg)
                parent.overwriteChild(idx, self._algorithms[name])

        checkSequenceConsistency(self._sequence)
        return newseq

    def moveSequence(self, sequence, destination ):
        """ moves sequence from one sub-sequence to another, primary use case HLT Control Flow """
        seq = findSubSequence(self._sequence, sequence )
        if seq is None:
            raise ConfigurationError("Can not find sequence to move %s " % sequence )

        owner = findOwningSequence(self._sequence, sequence)
        if owner is None:
            raise ConfigurationError("Can not find the sequence owning the %s " % sequence )

        dest = findSubSequence(self._sequence, destination )
        if dest is None:
            raise ConfigurationError("Can not find destination sequence %s to move to " % destination )

        owner.remove( seq )
        dest += seq
        checkSequenceConsistency(self._sequence)
        return seq


    def getSequence(self,sequenceName=None):
        if sequenceName is None:
            return self._sequence
        else:
            return findSubSequence(self._sequence,sequenceName)

    def setPrivateTools(self,privTool):
        """Use this method to carry private AlgTool(s) to the caller when returning this ComponentAccumulator. 
        The method accepts either a single private AlgTool or a list of private AlgTools (typically assigned to ToolHandleArray)
        """
        if self._privateTools is not None:
            raise ConfigurationError("This ComponentAccumulator holds already a (list of) private tool. Only one (list of)  private tool(s) is allowed")

        if isinstance(privTool,collections.Sequence):
            for t in privTool:
                if not isinstance(t,ConfigurableAlgTool):
                    raise  ConfigurationError("ComponentAccumulator.setPrivateTools accepts only ConfigurableAlgTools or lists of ConfigurableAlgTools. Encountered %s in a list" % type(t))
        else: 
            if not isinstance(privTool,ConfigurableAlgTool):
                raise  ConfigurationError("ComponentAccumulator.setPrivateTools accepts only cCnfigurableAlgTools or lists of ConfigurableAlgTools. Encountered %s " % type(privTool))
                
        self._privateTools=privTool
        return
        
    def popPrivateTools(self):
        """Get the (list of) private AlgTools from this ComponentAccumulator. 
        The CA will not keep any reference to the AlgTool.
        """
        tool=self._privateTools
        self._privateTools=None
        return tool
        
    def popToolsAndMerge(self, other):
        """ Merging in the other accumulator and getting the (list of) private AlgTools from this CompoentAccumulator.
        """
        if other is None:
            raise RuntimeError("merge called on object of type None: did you forget to return a CA from a config function?")
        tool = other.popPrivateTools()
        self.merge(other)
        return tool

    def addEventAlgo(self, algorithms,sequenceName=None,primary=False):
        if not isinstance(algorithms,collections.Sequence):
            #Swallow both single algorithms as well as lists or tuples of algorithms
            algorithms=[algorithms,]

        if sequenceName is None:
            seq=self._sequence
        else:
            seq = findSubSequence(self._sequence, sequenceName )
        if seq is None:
            self.printConfig()
            raise ConfigurationError("Can not find sequence %s" % sequenceName )

        for algo in algorithms:
            if not isinstance(algo, ConfigurableAlgorithm):
                raise TypeError("Attempt to add wrong type: %s as event algorithm" % type( algo ).__name__)
            if algo.name() in self._algorithms:
                deduplicateComponent(self._algorithms[algo.name()], algo)
            else:
                self._algorithms[algo.name()] = algo
            existingAlgInDest = findAlgorithm(seq, algo.name())
            if not existingAlgInDest:
                seq += self._algorithms[algo.name()]

        if primary:
            if len(algorithms)>1:
                self._msg.warning("Called addEvenAlgo with a list of algorithms and primary==True. Designating the first algorithm as primary component")
            if self._primaryComp: 
                self._msg.warning("Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.getType(), self._primaryComp.getName(), algorithms[0].getType(), algorithms[0].getName())
            #keep a ref of the algorithm as primary component
            self._primaryComp=algorithms[0]
        self._lastAddedComponent=algorithms[-1].getFullName()
        return None

    def getEventAlgo(self,name=None):
        if name not in self._algorithms:
            raise ConfigurationError("Can not find an algorithm of name %s "% name)
        return self._algorithms[name]

    def getEventAlgos(self,seqName=None):
        if seqName is None:
            seq=self._sequence
        else:
            seq = findSubSequence(self._sequence, seqName )
        return list( set( sum( flatSequencers( seq, algsCollection=self._algorithms ).values(), []) ) )

    def addCondAlgo(self,algo,primary=False):
        if not isinstance(algo, ConfigurableAlgorithm):
            raise TypeError("Attempt to add wrong type: %s as conditions algorithm" % type( algo ).__name__)
            pass
        deduplicate(algo,self._conditionsAlgs) #will raise on conflict
        if primary: 
            if self._primaryComp: 
                self._msg.warning("Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.getType(),self._primaryComp.getName(),algo.getType(),algo.getName())
            #keep a ref of the de-duplicated conditions algorithm as primary component
            self._primaryComp=self.__getOne( self._conditionsAlgs, algo.getName(), "ConditionsAlgos")
        self._lastAddedComponent=algo.getFullName()
        return algo


    def getCondAlgo(self,name):
        hits=[a for a in self._conditionsAlgs if a.getName()==name]
        if (len(hits)>1):
            raise ConfigurationError("More than one conditions algorithm with name %s found" % name)
        return hits[0]

    def addService(self,newSvc,primary=False):
        if not isinstance(newSvc,ConfigurableService):
            raise TypeError("Attempt to add wrong type: %s as service" % type( newSvc ).__name__)
            pass
        deduplicate(newSvc,self._services)  #will raise on conflict
        if primary: 
            if self._primaryComp: 
                self._msg.warning("Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.getType(),self._primaryComp.getName(),newSvc.getType(),newSvc.getName())
            #keep a ref of the de-duplicated public tool as primary component
            self._primaryComp=self.__getOne( self._services, newSvc.getName(), "Services") 
        self._lastAddedComponent=newSvc.getFullName()
        return 


    def addPublicTool(self,newTool,primary=False):
        if not isinstance(newTool,ConfigurableAlgTool):
            raise TypeError("Attempt to add wrong type: %s as AlgTool" % type( newTool ).__name__)
        if newTool.getParent() != "ToolSvc":
            newTool.setParent("ToolSvc")
        deduplicate(newTool,self._publicTools)
        if primary: 
            if self._primaryComp: 
                self._msg.warning("Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.getType(),self._primaryComp.getName(),newTool.getType(),newTool.getName())
            #keep a ref of the de-duplicated service as primary component
            self._primaryComp=self.__getOne( self._publicTools, newTool.getName(), "Public Tool") 
        self._lastAddedComponent=newTool.getFullName()
        return


    def getPrimary(self):
        if self._privateTools:
            return self.popPrivateTools()
        elif self._primaryComp:
            return self._primaryComp
        else:
            raise ConfigurationError("Called getPrimary() but no primary component nor private AlgTool is known.\n"\
                                     +self._inspect())



    def __call__(self):
        return self.getPrimary()

    def __getOne(self, allcomps, name=None, typename="???"):
        selcomps = allcomps if name is None else [ t for t in allcomps if t.getName() == name ]
        if len( selcomps ) == 1:
            return selcomps[0]            
        raise ConfigurationError("Number of %s available %d which is != 1 expected by this API" % (typename, len(selcomps)) )
        
    def getPublicTools(self):
        return self._publicTools

    def getPublicTool(self, name=None):        
        """Returns single public tool, exception if either not found or to many found"""
        return self.__getOne( self._publicTools, name, "PublicTools")

    def getServices(self):
        return self._services

    def getService(self, name=None):        
        """Returns single service, exception if either not found or to many found"""
        if name is None:
            return self._primarySvc
        else:
            return self.__getOne( self._services, name, "Services")
    

    def setAppProperty(self,key,value,overwrite=False):
        if (overwrite or key not in (self._theAppProps)):
            self._theAppProps[key]=value
        else:
            if self._theAppProps[key] == value:
                self._msg.debug("ApplicationMgr property '%s' already set to '%s'.", key, value)
            elif isinstance(self._theAppProps[key],collections.Sequence) and not isinstance(self._theAppProps[key],str):
                value=unifySet(self._theAppProps[key],value)
                self._msg.info("ApplicationMgr property '%s' already set to '%s'. Overwriting with %s", key, self._theAppProps[key], value)
                self._theAppProps[key]=value
            else:
                raise DeduplicationFailed("AppMgr property %s set twice: %s and %s" % (key, self._theAppProps[key], value))


        pass

    def merge(self,other, sequenceName=None):
        """Merging in the other accumulator"""
        if other is None:
            raise RuntimeError("merge called on object of type None: did you forget to return a CA from a config function?")

        if not isinstance(other,ComponentAccumulator):
            raise TypeError("Attempt merge wrong type %s. Only instances of ComponentAccumulator can be added" % type(other).__name__)

        if (other._privateTools is not None):
            if isinstance(other._privateTools,ConfigurableAlgTool):
                raise RuntimeError("merge called with a ComponentAccumulator a dangling private tool %s/%s" %
                                   (other._privateTools.getType(),other._privateTools.getName()))
            else:
                raise RuntimeError("merge called with a ComponentAccumulator a dangling (array of) private tools")
        
        if not other._isMergable:
            raise ConfigurationError("Attempted to merge the ComponentAccumulator that was unsafely manipulated (likely with foreach_component, ...) or is a top level ComponentAccumulator, in such case revert the order")

        #destSubSeq = findSubSequence(self._sequence, sequence)
        #if destSubSeq == None:
        #    raise ConfigurationError( "Nonexistent sequence %s in %s (or its sub-sequences)" % ( sequence, self._sequence.name() ) )          #
        def mergeSequences( dest, src ):
            for childIdx, c in enumerate(src.getChildren()):
                if isSequence( c ):
                    sub = findSubSequence( dest, c.name() ) #depth=1 ???
                    if sub:
                        mergeSequences(sub, c )
                    else:
                        self._msg.debug("  Merging sequence %s to a sequence %s", c.name(), dest.name() )
                        algorithmsByName = findAllAlgorithmsByName(c)
                        for name, existingAlgs in six.iteritems(algorithmsByName):
                            startingIndex = 0
                            if name not in self._algorithms:
                                firstAlg, parent, idx = existingAlgs[0]
                                self._algorithms[name] = firstAlg
                                startingIndex = 1
                            for alg, parent, idx in existingAlgs[startingIndex:]:
                                deduplicateComponent(self._algorithms[name], alg)
                                parent.overwriteChild(idx, self._algorithms[name])
                        dest += c

                else: # an algorithm
                    if c.name() in self._algorithms:
                        deduplicateComponent(self._algorithms[c.name()], c)
                        src.overwriteChild(childIdx, self._algorithms[c.name()])
                    else:
                        self._algorithms[c.name()] = c

                    existingAlgInDest = findAlgorithm( dest, c.name(), depth=1 )
                    if not existingAlgInDest:
                        self._msg.debug("   Adding algorithm %s to a sequence %s", c.name(), dest.name() )
                        dest += c

            checkSequenceConsistency(self._sequence)

        # Merge sequences:
        # mergeSequences(destSeq, other._sequence)
        # if sequenceName is provided it means we should be ignoring the actual main seq name there and use the sequenceName
        # that means the first search in the destination seqence needs to be cheated
        # the sequenceName argument is ambigous when the other CA has more than one sequence and this is checked
        
        if sequenceName is None:
            for otherSeq in other._allSequences:
                found=False
                for ourSeq in self._allSequences:
                    ourSeq = findSubSequence(ourSeq, otherSeq.name()) # try to add sequence to the main structure first, to each seq in parent?
                    if ourSeq:
                        mergeSequences(ourSeq, otherSeq)
                        found=True
                        self._msg.verbose("   Succeeded to merge sequence %s to %s", otherSeq.name(), ourSeq.name() )
                    else:
                        self._msg.verbose("   Failed to merge sequence %s to any existing one, destination CA will have several top/dangling sequences", otherSeq.name() )
                if not found: # just copy the sequence as a dangling one
                    self._allSequences.append( copy.copy(otherSeq) )
                    mergeSequences( self._allSequences[-1], otherSeq )
        else:
            if len(other._allSequences) > 1:
                raise ConfigurationError('Merging of the accumulator that has mutiple top level sequences and changing the destination sequence is not supported')
            destSeq = self.getSequence(sequenceName) if sequenceName else self._sequence
            mergeSequences(destSeq, other._sequence)


            
        # Additional checking and updating other accumulator's algorithms list
        for name, alg in six.iteritems(other._algorithms):
            if name not in self._algorithms:
                raise ConfigurationError('Error in merging. Algorithm {} missing in destination accumulator'.format(name))
            other._algorithms[name] = self._algorithms[name]

        #self._conditionsAlgs+=other._conditionsAlgs
        for condAlg in other._conditionsAlgs:
            self.addCondAlgo(condAlg) #Profit from deduplicaton here

        for svc in other._services:
            self.addService(svc) #Profit from deduplicaton here

        for pt in other._publicTools:
            self.addPublicTool(pt) #Profit from deduplicaton here

        #Merge AppMgr properties:
        for (k,v) in six.iteritems(other._theAppProps):
            self.setAppProperty(k,v)  #Will warn about overrides
            pass
        other._wasMerged=True
        self._lastAddedComponent = other._lastAddedComponent+' (Merged)'


    def appendToGlobals(self):
        self.wasMerged()
        #Cache old configurable behavior
        oldstat=Configurable.configurableRun3Behavior

        #Turn configurable behavior to old-style (eg return pre-existing instances instead of new'ing them)
        Configurable.configurableRun3Behavior=0
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr, theApp

        self._msg.debug("Merging services with global setup")
        for s in self._services:
            if s.getFullName() in [fn.getFullName() for fn in ServiceMgr.getChildren()]: 
                existingS=getattr(ServiceMgr,s.getName())
                deduplicateComponent(existingS,s)
            else:
                ServiceMgr+=s

            if s.getJobOptName() in _servicesToCreate \
                    and s.getJobOptName() not in theApp.CreateSvc:
                theApp.CreateSvc.append(s.getJobOptName())

        self._msg.debug("Merging AlgTools with global setup")
        for t in self._publicTools:
            if t.getFullName() in [fn.getFullName() for fn in ToolSvc.getChildren()]:
                #deduplicate
                existingT=getattr(ToolSvc,t.getName())
                deduplicateComponent(existingT,t)
                pass
            else:
                ToolSvc+=t

        self._msg.debug("Merging conditions algorithms with global setup")
        condseq=AthSequencer ("AthCondSeq")
        for c in self._conditionsAlgs:
            deduplicate(c, condseq.getChildren() )


        for seqName, algoList in six.iteritems(flatSequencers( self._sequence )):
            seq=AthSequencer(seqName)
            for alg in algoList:
                seq+=alg


        for (k,v) in six.iteritems(self._theAppProps):
            if k not in [ 'CreateSvc', 'ExtSvc']:
                setattr(theApp,k,v)

        #Re-instante previous configurable behavior
        Configurable.configurableRun3Behavior=oldstat
        return


    def appendConfigurable(self,confElem):
        name=confElem.getJobOptName() # to be FIXED
        # if self._jocat.has_key(name):
        #     self._msg.info("Not adding duplicate configurable: %s" % name )
        #     return

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
            else:
                if name not in self._jocat:
                    self._jocat[name] = {}
                self._jocat[name][k]=str(v)

        #print ("All Children:",confElem.getAllChildren())
        for ch in confElem.getAllChildren():
            self.appendConfigurable(ch)
        return

    def __verifyFinalSequencesStructure(self):
        if len(self._allSequences) != 1:
            raise ConfigurationError('It is not allowed for the storable CA to have more than one top sequence, now it has: {}'\
                                         .format(','.join([ s.name() for s in self._allSequences])))

    def store(self,outfile,nEvents=10,useBootStrapFile=True,threaded=False):
        from AthenaCommon.Utils.unixtools import find_datafile
        import pickle
        self._isMergable = True # this is CA we store, it is ok it is not mergable
        if useBootStrapFile:
            import glob
            # first load basics from the bootstrap-pickle
            # a better solution to be discussed
            # prefer local file
            localbs = glob.glob("bootstrap.pkl")
            if len( localbs ) == 0:
                # if local bootstrap is missing, use one from the release
                bsfilename=find_datafile("bootstrap.pkl")
            else:
                bsfilename = "./"+localbs[0]

            bsfile=open(bsfilename,'rb')
            self._jocat=pickle.load(bsfile)
            self._jocfg=pickle.load(bsfile)
            self._pycomps=pickle.load(bsfile)
            bsfile.close()
  
        else:
            from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg, MainServicesSerialCfg
            if threaded:
              from AthenaConfiguration.AllConfigFlags import ConfigFlags
              flags = ConfigFlags.clone()
              flags.Concurrency.NumThreads = 1
              flags.Concurrency.NumConcurrentEvents = 1
              basecfg = MainServicesThreadedCfg(flags)
              basecfg.printConfig()
              basecfg.merge(self)
              self = basecfg
              self.printConfig()
            else: #Serial
              basecfg = MainServicesSerialCfg()
              basecfg.merge(self)
              self = basecfg
              self.__verifyFinalSequencesStructure()

            self._jocat={}
            self._jocfg={}
            self._pycomps={}
            self._jocfg["ApplicationMgr"]={}
            self._jocfg["ApplicationMgr"]["ExtSvc"] = "['ToolSvc/ToolSvc', \
                                                        'AuditorSvc/AuditorSvc', \
                                                        'MessageSvc/MessageSvc', \
                                                        'IncidentSvc/IncidentSvc',\
                                                        'EvtPersistencySvc/EventPersistencySvc',\
                                                        'HistogramSvc/HistogramDataSvc',\
                                                        'NTupleSvc/NTupleSvc',\
                                                        'RndmGenSvc/RndmGenSvc',\
                                                        'ChronoStatSvc/ChronoStatSvc',\
                                                        'StatusCodeSvc/StatusCodeSvc',\
                                                        'StoreGateSvc/StoreGateSvc',\
                                                        'StoreGateSvc/DetectorStore',\
                                                        'StoreGateSvc/HistoryStore',\
                                                        'ClassIDSvc/ClassIDSvc',\
                                                        'AthDictLoaderSvc/AthDictLoaderSvc',\
                                                        'AthenaSealSvc/AthenaSealSvc',\
                                                        'CoreDumpSvc/CoreDumpSvc',\
                                                        'JobOptionsSvc/JobOptionsSvc']"

            #Code seems to be wrong here
            for seqName, algoList in six.iteritems(flatSequencers( self._sequence, algsCollection=self._algorithms )):
                self._jocat[seqName] = {}
                for alg in algoList:
                  self._jocat[alg.name()] = {}
            for k, v in self._sequence.getValuedProperties().items():
                self._jocat[self._sequence.getName()][k]=str(v)

        #EventAlgorithms
        for seqName, algoList  in six.iteritems(flatSequencers( self._sequence, algsCollection=self._algorithms )):
            evtalgseq=[]
            for alg in algoList:
                self.appendConfigurable( alg )
                evtalgseq.append( alg.getFullName() )

        for seqName, algoList  in six.iteritems(flatSequencers( self._sequence, algsCollection=self._algorithms )):
            # part of the sequence may come from the bootstrap, we need to retain the content, that is done here
            for prop in self._jocat[seqName]:
                if prop == "Members":
                    mergedSequence = ast.literal_eval(self._jocat[seqName]["Members"]) +  [alg.getFullName() for alg in algoList]
                    self._jocat[seqName]["Members"] = str( mergedSequence )


        #Conditions Algorithms:
        condalgseq=[]
        for alg in self._conditionsAlgs:
            self.appendConfigurable(alg)
            condalgseq.append(alg.getFullName())
        self._jocat["AthCondSeq"]["Members"]=str(condalgseq)


        #Public Tools:
        for pt in self._publicTools:
            #print ("Appending public Tool",pt.getFullName(),pt.getJobOptName())
            self.appendConfigurable(pt)



        #Hack for now:
        self._jocfg["ApplicationMgr"]["CreateSvc"]=['ToolSvc/ToolSvc', 'AthDictLoaderSvc/AthDictLoaderSvc', 'AthenaSealSvc/AthenaSealSvc', 'CoreDumpSvc/CoreDumpSvc']

        svcList=ast.literal_eval(self._jocfg["ApplicationMgr"]["ExtSvc"])

        for svc in self._services:
            if svc.getJobOptName() in _servicesToCreate:
                self._jocfg["ApplicationMgr"]["CreateSvc"].append( svc.getFullName() )

            svcList.append(svc.getFullName())
            #for k, v in svc.getValuedProperties().items():
            #    self._jocat[svcname][k]=str(v)
            self.appendConfigurable(svc)
        self._jocfg["ApplicationMgr"]["ExtSvc"]=str(svcList)

        self._jocfg["ApplicationMgr"]["EvtMax"]=nEvents


        for (k,v) in six.iteritems(self._theAppProps):
            if k not in [ 'CreateSvc', 'ExtSvc']:
                self._jocfg["ApplicationMgr"][k]=v

        #from pprint import pprint
        #pprint (self._jocat)

        pickle.dump( self._jocat, outfile )
        pickle.dump( self._jocfg, outfile )
        pickle.dump( self._pycomps, outfile )
        self._wasMerged=True

    def wasMerged(self):
        """ Declares CA as merged

        This is temporarily needed by HLT and should not be used elsewhere
        """
        self._wasMerged=True

    def createApp(self,OutputLevel=3):
        self._wasMerged=True
        from Gaudi.Main import BootstrapHelper
        bsh=BootstrapHelper()
        app=bsh.createApplicationMgr()

        for (k,v) in six.iteritems(self._theAppProps):
            app.setProperty(k,str(v))

        #Assemble createSvc property:
        svcToCreate=[]
        extSvc=[]
        for svc in self._services:
            extSvc+=[svc.getFullName(),]
            if svc.getJobOptName() in _servicesToCreate:
                svcToCreate+=[svc.getFullName(),]

        #print (self._services)
        #print (extSvc)
        #print (svcToCreate)
        app.setProperty("ExtSvc",str(extSvc))
        app.setProperty("CreateSvc",str(svcToCreate))

        app.configure()

        msp=app.getService("MessageSvc")
        bsh.setProperty(msp,b"OutputLevel",str(OutputLevel).encode())
        #Feed the jobO service with the remaining options
        jos=app.getService("JobOptionsSvc")

        def addCompToJos(comp):
            name=comp.getJobOptName()
            for k, v in comp.getValuedProperties().items():
                if isinstance(v,Configurable):
                    self._msg.debug("Adding "+name+"."+k+" = "+v.getFullName())
                    bsh.addPropertyToCatalogue(jos,name.encode(),k.encode(),v.getFullName().encode())
                    addCompToJos(v)
                elif isinstance(v,GaudiHandles.GaudiHandleArray):
                    bsh.addPropertyToCatalogue(jos,name.encode(),k.encode(),str([ v1.getFullName() for v1 in v ]).encode())
                else:
                    if not isSequence(comp) and k!="Members": #This property his handled separatly
                        self._msg.debug("Adding "+name+"."+k+" = "+str(v))
                        bsh.addPropertyToCatalogue(jos,name.encode(),k.encode(),str(v).encode())
                    pass
                pass
            for ch in comp.getAllChildren():
                addCompToJos(ch)
            return

        #Add services
        for svc in self._services:
            addCompToJos(svc)
            pass

        #Add tree of algorithm sequences:
        try:
            from AthenaPython import PyAthenaComps
            PyAlg = PyAthenaComps.Alg
        except ImportError:
            PyAlg = type(None)

        for seqName, algoList in six.iteritems(flatSequencers( self._sequence, algsCollection=self._algorithms )):
            self._msg.debug("Members of %s : %s", seqName, str([alg.getFullName() for alg in algoList]))
            bsh.addPropertyToCatalogue(jos,seqName.encode(),b"Members",str( [alg.getFullName() for alg in algoList]).encode())
            for alg in algoList:
                addCompToJos(alg)
                if isinstance (alg, PyAlg):
                    alg.setup()
            pass


        condalgseq=[]
        for alg in self._conditionsAlgs:
            addCompToJos(alg)
            condalgseq.append(alg.getFullName())
            bsh.addPropertyToCatalogue(jos,b"AthCondSeq",b"Members",str(condalgseq).encode())
            if isinstance (alg, PyAlg):
                alg.setup()
            pass


        #Public Tools:
        for pt in self._publicTools:
            addCompToJos(pt)
            pass

        return app

    def run(self,maxEvents=None,OutputLevel=3):
        from AthenaCommon.Debugging import allowPtrace
        allowPtrace()

        app = self.createApp (OutputLevel)
        self.__verifyFinalSequencesStructure()

        #Determine maxEvents
        if maxEvents is None:
            if "EvtMax" in self._theAppProps:
                maxEvents=self._theAppProps["EvtMax"]
            else:
                maxEvents=-1

        self._msg.info("INITIALIZE STEP")
        sc = app.initialize()
        if not sc.isSuccess():
            self._msg.error("Failed to initialize AppMgr")
            return sc

        app.printAlgsSequences() #could be removed later ....

        sc = app.start()
        if not sc.isSuccess():
            self._msg.error("Failed to start AppMgr")
            return sc

        sc = app.run(maxEvents)
        if not sc.isSuccess():
            self._msg.error("Failure running application")
            return sc

        app.stop().ignore()

        app.finalize().ignore()

        sc1 = app.terminate()
        return sc1

    def foreach_component(self, path):
        """ Utility to set properties of components using wildcards
        
        Example:
        forcomps(ca, "*/HLTTop/*/*Hypo*").OutputLevel=VERBOSE
        
        The compoments name & locations in the CF tree are translated into the unix like path. 
        Components of matching path are taken under consideration in setting the property.
        If the property is set succesfully an INFO message is printed. Else, a warning is printed.      
        
        The convention for path of nested components is as follows:
        Sequencer - only the name is used in the path
        Algorithm - full name - type/instance_name (aka full name) is used
        PrivateTools - the name of the property + the type/instance_name are added
        PublicTools - are located under ToolSvc/ and type/instance_name is used
        Services - located under SvcMgr/ and type/instance_name is used
        """
        from AthenaConfiguration.PropSetterProxy import PropSetterProxy
        self._isMergable=False
        return PropSetterProxy(self, path)



def CAtoGlobalWrapper(cfgmethod,flags):
    Configurable.configurableRun3Behavior+=1
    result=cfgmethod(flags)
    Configurable.configurableRun3Behavior-=1

    result.appendToGlobals()
    return
