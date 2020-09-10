# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaCommon.Logging import logging
from AthenaCommon.CFElements import isSequence,findSubSequence,findAlgorithm,flatSequencers,findOwningSequence,\
    checkSequenceConsistency, findAllAlgorithmsByName
from AthenaConfiguration.ComponentFactory import CompFactory
#from AthenaConfiguration.AlgSequence import AthSequencer
from AthenaCommon.Debugging import DbgStage
from AthenaCommon.Constants import INFO

import GaudiKernel.GaudiHandles as GaudiHandles
import GaudiConfig2
from AthenaConfiguration.Deduplication import deduplicate, DeduplicationFailed

import collections
import six
import copy
import sys

from AthenaConfiguration.UnifyProperties import unifySet



class ConfigurationError(RuntimeError):
    pass

_basicServicesToCreate=frozenset(('GeoModelSvc','TileInfoLoader','DetDescrCnvSvc','CoreDumpSvc','VTuneProfilerService','EvtIdModifierSvc'))

def printProperties(msg, c, nestLevel = 0, printDefaults=False):
    # Iterate in sorted order.
    propnames= sorted(c._descriptors.keys())
    for propname in propnames:
        
        if not printDefaults and not c.is_property_set(propname):
            continue
        propval=getattr(c,propname)
        # Ignore empty lists
        
        if isinstance(propval,(GaudiConfig2.semantics._ListHelper,GaudiConfig2.semantics._DictHelper)) and propval.data is None:
            continue
        # Printing EvtStore could be relevant for Views?
        if propname in ["DetStore","EvtStore"]:
            continue

        if isinstance( propval, GaudiConfig2.Configurable ):
            msg.info( " "*nestLevel +"    * {0}: {1}/{2}".format(propname, propval.__cpp_type__, propval.getName()))
            printProperties(msg, propval, nestLevel+3)
            continue
        if isinstance(propval,GaudiHandles.PublicToolHandleArray):
            ths = [th.getName() for th in propval]
            propstr = "PublicToolHandleArray([ {0} ])".format(', '.join(ths))
        elif isinstance(propval,GaudiHandles.PrivateToolHandleArray):
            ths = [th.getName() for th in propval]
            propstr = "PrivateToolHandleArray([ {0} ])".format(', '.join(ths))
        elif isinstance(propval,GaudiHandles.GaudiHandle): # Any other handle
            propstr = "Handle( {0} )".format(propval.typeAndName)
        else:
            propstr = str(propval)            
        msg.info( " "*nestLevel +"    * {0}: {1}".format(propname,propstr))
    return


def filterComponents (comps, onlyComponents = []):
    ret = []
    for c in comps:
        if not onlyComponents or c.getName() in onlyComponents:
            ret.append ((c, True))
        elif c.getName()+'-' in onlyComponents:
            ret.append ((c, False))
    return ret


class ComponentAccumulator(object):

    def __init__(self,sequenceName='AthAlgSeq'):
        self._msg=logging.getLogger('ComponentAccumulator')
        AthSequencer=CompFactory.AthSequencer
        self._sequence=AthSequencer(sequenceName,Sequential=True)    #(Nested) default sequence of event processing algorithms per sequence + their private tools
        self._allSequences = [ self._sequence ] 
        self._algorithms = {}            #Flat algorithms list, useful for merging
        self._conditionsAlgs=[]          #Unordered list of conditions algorithms + their private tools
        self._services=[]                #List of service, not yet sure if the order matters here in the MT age
        self._servicesToCreate=set(_basicServicesToCreate)
        self._privateTools=None          #A placeholder to carry a private tool(s) not yet attached to its parent
        self._primaryComp=None           #A placeholder to designate the primary service 

        self._theAppProps=dict()        #Properties of the ApplicationMgr

        #Backward compatiblity hack: Allow also public tools:
        self._publicTools=[]

        #To check if this accumulator was merged:
        self._wasMerged=False
        self._isMergable=True
        self._lastAddedComponent="Unknown" 
        self._debugStage=DbgStage()


    def setAsTopLevel(self):
        self._isMergable = False


    def _inspect(self): #Create a string some basic info about this CA, useful for debugging
        summary="This CA contains {0} service, {1} conditions algorithms, {2} event algorithms and {3} public tools\n"\
            .format(len(self._services),len(self._conditionsAlgs),len(self._algorithms),len(self._publicTools))
        if (self._privateTools): 
            if (isinstance(self._privateTools, list)):
                summary+="  Private AlgTool: "+ self._privateTools[-1].getFullJobOptName()+"\n"
            else:
                summary+="  Private AlgTool: "+ self._privateTools.getFullJobOptName()+"\n"
        if (self._primaryComp): 
            summary+="  Primary Component: " + self._primaryComp.getFullJobOptName()+"\n" 
        summary+="  Last component added: "+self._lastAddedComponent+"\n" 
        return summary


    def empty(self):
        return len(self._sequence.Members)+len(self._conditionsAlgs)+len(self._services)+\
            len(self._publicTools)+len(self._theAppProps) == 0

    def __del__(self):
         if not getattr(self,'_wasMerged',True) and not self.empty():
             #can't raise an exception in __del__ method (Python rules) so this is a warning
             log = logging.getLogger("ComponentAccumulator")
             log.error("This ComponentAccumulator was never merged!")
             log.error(self._inspect())
             import traceback
             traceback.print_stack()
         if getattr(self,'_privateTools',None) is not None:
             log = logging.getLogger("ComponentAccumulator")
             log.error("Deleting a ComponentAccumulator with dangling private tool(s)")

        #pass

    def __getstate__(self):
        state = self.__dict__.copy()
        # Remove the unpicklable entries.
        del state['_msg']
        return state

    def __setstate__(self,state):
        self.__dict__.update(state)
        #Re-enstate logger
        self._msg=logging.getLogger('ComponentAccumulator')


    def printCondAlgs(self, summariseProps=False, onlyComponents=[], printDefaults=False):
        self._msg.info( "Condition Algorithms" )
        for (c, flag) in filterComponents (self._conditionsAlgs, onlyComponents):
            self._msg.info( " " +"\\__ "+ c.name +" (cond alg)" )
            if summariseProps and flag:
                printProperties(self._msg, c, 1, printDefaults)
        return
        

    # If onlyComponents is set, then only print components with names
    # that appear in the onlyComponents list.  If a name is present
    # in the list with a trailing `-', then only the name of the component
    # will be printed, not its properties.
    def printConfig(self, withDetails=False, summariseProps=False,
                    onlyComponents = [], printDefaults=False):
        self._msg.info( "Event Inputs" )
        self._msg.info( "Event Algorithm Sequences" )


        if withDetails:# The WithDetails option does work with GaudiConfi2 (for now) 
            self._msg.info( self._sequence )
        else:
            def printSeqAndAlgs(seq, nestLevel = 0,
                                onlyComponents = []):
                def __prop(name):
                    if name in seq._properties:
                        return seq._properties[name]
                    return seq._descriptors[name].default

                self._msg.info( " "*nestLevel +"\\__ "+ seq.name +" (seq: %s %s)",
                                "SEQ" if __prop("Sequential") else "PAR", "OR" if __prop("ModeOR") else "AND" )
                nestLevel += 3
                for (c, flag) in filterComponents (seq.Members, onlyComponents):
                    if isSequence(c):
                        printSeqAndAlgs(c, nestLevel, onlyComponents = onlyComponents )
                    else:
                        self._msg.info( " "*nestLevel +"\\__ "+ c.name +" (alg)" )
                        if summariseProps and flag:
                            printProperties(self._msg, c, nestLevel, printDefaults)

            for n,s in enumerate(self._allSequences):
                self._msg.info( "Top sequence {}".format(n) )
                printSeqAndAlgs(s, onlyComponents = onlyComponents)

        self.printCondAlgs (summariseProps = summariseProps,
                            onlyComponents = onlyComponents)
        self._msg.info( "Services" )
        self._msg.info( [ s[0].name for s in filterComponents (self._services, onlyComponents) ] )
        self._msg.info( "Public Tools" )
        self._msg.info( "[" )
        for (t, flag) in filterComponents (self._publicTools, onlyComponents):
            self._msg.info( "  {0},".format(t.getFullJobOptName()) )
            # Not nested, for now
            if summariseProps and flag:
                printProperties(self._msg, t, printDefaults)
        self._msg.info( "]" )
        self._msg.info( "Private Tools")
        self._msg.info( "[" )
        if (isinstance(self._privateTools, list)):
            for (t, flag) in filterComponents (self._privateTools, onlyComponents):
                self._msg.info( "  {0},".format(t.getFullJobOptsName()) )
                # Not nested, for now
                if summariseProps and flag:
                    printProperties(self._msg, t, printDefaults)
        else:
            if self._privateTools is not None:
                self._msg.info( "  {0},".format(self._privateTools.getFullJobOptName()) )
                if summariseProps:
                    printProperties(self._msg, self._privateTools, printDefaults)
        self._msg.info( "]" )
        self._msg.info( "TheApp properties" )
        for k,v in six.iteritems(self._theAppProps):
            self._msg.info("  {} : {}".format(k,v))


    def addSequence(self, newseq, parentName = None ):
        """ Adds new sequence. If second argument is present then it is added under another sequence  """
        from AthenaCommon.AlgSequence import AthSequencer as LegacySequence
        if isinstance( newseq, LegacySequence ):
            raise ConfigurationError('{} is not the Conf2 Sequence, ComponentAccumulator handles only the former'.format(newseq.name()))

        if not isSequence(newseq):
            raise TypeError('%s is not a sequence' % newseq.name)

        if parentName is None:
            parent=self._sequence
        else:
            parent = findSubSequence(self._sequence, parentName )
            if parent is None:
                raise ConfigurationError("Missing sequence %s to add new sequence to" % parentName )

        parent.Members.append(newseq)
        algsByName = findAllAlgorithmsByName(newseq)
        for name, existingAlgs in six.iteritems(algsByName):
            startingIndex = 0
            if name not in self._algorithms:
                firstAlg, parent, idx = existingAlgs[0]
                self._algorithms[name] = firstAlg
                startingIndex = 1
            for alg, parent, idx in existingAlgs[startingIndex:]:
                self._algorithms[name].merge(alg)
                parent.Members[idx] = self._algorithms[name]

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

        owner.Members.remove( seq )
        dest.Members.append( seq )
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
                if t.__component_type__ != 'AlgTool':
                    raise  ConfigurationError("ComponentAccumulator.setPrivateTools accepts only ConfigurableAlgTools or lists of ConfigurableAlgTools. Encountered %s in a list" % type(t))
        else: 
            if privTool.__component_type__ != "AlgTool":
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
            if algo.__component_type__ != "Algorithm": 
                raise TypeError("Attempt to add wrong type: %s as event algorithm" % type( algo ).__name__)

            if algo.name in self._algorithms:
                self._algorithms[algo.name].merge(algo)
            else:
                self._algorithms[algo.name]=algo

            existingAlgInDest = findAlgorithm(seq, algo.name)
            if not existingAlgInDest:
                seq.Members.append(self._algorithms[algo.name])

        if primary:
            if len(algorithms)>1:
                self._msg.warning("Called addEvenAlgo with a list of algorithms and primary==True. Designating the first algorithm as primary component")
            if self._primaryComp: 
                self._msg.warning("Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.__cpp_type__, self._primaryComp.name, 
                                  algorithms[0].__cpp_type__, algorithms[0].name)
            #keep a ref of the algorithm as primary component
            self._primaryComp=algorithms[0]
        self._lastAddedComponent=algorithms[-1].name
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
        if algo.__component_type__ != "Algorithm":
            raise TypeError("Attempt to add wrong type: %s as conditions algorithm" % type( algo ).__name__)
            pass
        deduplicate(algo,self._conditionsAlgs) #will raise on conflict
        if primary: 
            if self._primaryComp: 
                self._msg.warning("Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.__cpp_type__,self._primaryComp.name,algo.__cpp_type__,algo.name)
            #keep a ref of the de-duplicated conditions algorithm as primary component
            self._primaryComp=self.__getOne( self._conditionsAlgs, algo.name, "ConditionsAlgos")
        self._lastAddedComponent=algo.name
        return algo


    def getCondAlgo(self,name):
        hits=[a for a in self._conditionsAlgs if a.name==name]
        if (len(hits)>1):
            raise ConfigurationError("More than one conditions algorithm with name %s found" % name)
        return hits[0]

    def addService(self,newSvc,primary=False,create=False):
        if newSvc.__component_type__ != "Service":
            raise TypeError("Attempt to add wrong type: %s as service" % newSvc.__component_type__)
            pass
        deduplicate(newSvc,self._services)  #will raise on conflict
        if primary: 
            if self._primaryComp: 
                self._msg.warning("Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.__cpp_type__,self._primaryComp.name,newSvc.__cpp_type__,newSvc.name)
            #keep a ref of the de-duplicated public tool as primary component
            self._primaryComp=self.__getOne( self._services, newSvc.name, "Services") 
        self._lastAddedComponent=newSvc.name
        if create: self._servicesToCreate.add(newSvc.name)
        return 


    def addPublicTool(self,newTool,primary=False):
        if newTool.__component_type__ != "AlgTool":
            raise TypeError("Attempt to add wrong type: %s as AlgTool" % type( newTool ).__name__)
            
        deduplicate(newTool,self._publicTools)
        if primary: 
            if self._primaryComp: 
                self._msg.warning("Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.__cpp_type__,self._primaryComp.name,newTool.__cpp_type__,newTool.name)
            #keep a ref of the de-duplicated service as primary component
            self._primaryComp=self.__getOne( self._publicTools, newTool.name, "Public Tool") 
        self._lastAddedComponent=newTool.name
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
        selcomps = allcomps if name is None else [ t for t in allcomps if t.name == name ]
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

    def getAppProps(self):
        return self._theAppProps

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


    def setDebugStage(self,stage):
        if stage not in DbgStage.allowed_values:
            raise RuntimeError("Allowed arguments for setDebugStage are [%s]" % (",".join(DbgStage.allowed_values)))
        self._debugStage.value=stage
        pass

    def merge(self,other, sequenceName=None):
        """Merging in the other accumulator"""
        if other is None:
            raise RuntimeError("merge called on object of type None: did you forget to return a CA from a config function?")

        if not isinstance(other,ComponentAccumulator):
            raise TypeError("Attempt merge wrong type %s. Only instances of ComponentAccumulator can be added" % type(other).__name__)

        if (other._privateTools is not None):
            if isinstance(other._privateTools,collections.Sequence):
                raise RuntimeError("merge called with a ComponentAccumulator a dangling (array of) private tools")
            else:
                raise RuntimeError("merge called with a ComponentAccumulator a dangling private tool %s/%s" %
                                   (other._privateTools.__cpp_type__,other._privateTools.name))
            

        
        if not other._isMergable:
            raise ConfigurationError("Attempted to merge the ComponentAccumulator that was unsafely manipulated (likely with foreach_component, ...) or is a top level ComponentAccumulator, in such case revert the order")

        #destSubSeq = findSubSequence(self._sequence, sequence)
        #if destSubSeq == None:
        #    raise ConfigurationError( "Nonexistent sequence %s in %s (or its sub-sequences)" % ( sequence, self._sequence.name() ) )          #
        def mergeSequences( dest, src ):
            for childIdx, c in enumerate(src.Members):
                if isSequence( c ):
                    sub = findSubSequence( dest, c.name ) #depth=1 ???
                    if sub:
                        mergeSequences(sub, c )
                    else:
                        self._msg.debug("  Merging sequence %s to a sequence %s", c.name, dest.name )
                        algorithmsByName = findAllAlgorithmsByName(c)
                        for name, existingAlgs in six.iteritems(algorithmsByName):
                            startingIndex = 0
                            if name not in self._algorithms:
                                firstAlg, parent, idx = existingAlgs[0]
                                self._algorithms[name] = firstAlg
                                startingIndex = 1
                            for alg, parent, idx in existingAlgs[startingIndex:]:
                                self._algorithms[name].merge(alg)
                                parent.Members[idx] = self._algorithms[name]
                        dest.Members.append(c)
                else: # an algorithm
                    if c.name in self._algorithms:
                        self._algorithms[c.name].merge(c)
                        src.Members[childIdx] = self._algorithms[c.name]
                    else:
                        self._algorithms[c.name] = c

                    existingAlgInDest = findAlgorithm( dest, c.name, depth=1 )
                    if not existingAlgInDest:
                        self._msg.debug("   Adding algorithm %s to a sequence %s", c.name, dest.name )
                        dest.Members.append(c)

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
                    ourSeq = findSubSequence(ourSeq, otherSeq.name) # try to add sequence to the main structure first, to each seq in parent?
                    if ourSeq:
                        mergeSequences(ourSeq, otherSeq)
                        found=True
                        self._msg.verbose("   Succeeded to merge sequence %s to %s", otherSeq.name, ourSeq.name )
                    else:
                        self._msg.verbose("   Failed to merge sequence %s to any existing one, destination CA will have several top/dangling sequences", otherSeq.name )
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

        self._servicesToCreate.update(other._servicesToCreate)

        for pt in other._publicTools:
            self.addPublicTool(pt) #Profit from deduplicaton here

        #Merge AppMgr properties:
        for (k,v) in six.iteritems(other._theAppProps):
            self.setAppProperty(k,v)  #Will warn about overrides
            pass
        other._wasMerged=True

        self._lastAddedComponent = other._lastAddedComponent #+ ' (Merged)'


    def __verifyFinalSequencesStructure(self):
        if len(self._allSequences) != 1:
            raise ConfigurationError('It is not allowed for the storable CA to have more than one top sequence, now it has: {}'\
                                         .format(','.join([ s.name for s in self._allSequences])))

    
    def wasMerged(self):
        """ Declares CA as merged

        This is temporarily needed by HLT and should not be used elsewhere
        """
        self._wasMerged=True


    def store(self,outfile):
        self.wasMerged()
        import pickle
        pickle.dump(self,outfile)
        return


    def createApp(self, OutputLevel=INFO):
        # Create the Gaudi object early.
        # Without this here, pyroot can sometimes get confused
        # and report spurious type mismatch errors about this object.
        import ROOT
        ROOT.Gaudi

        appPropsToSet, mspPropsToSet, bshPropsToSet = self.gatherProps(OutputLevel)

        self._wasMerged = True
        from Gaudi.Main import BootstrapHelper

        bsh = BootstrapHelper()
        app = bsh.createApplicationMgr()

        for k, v in appPropsToSet.items():
            self._msg.debug("Setting property %s : %s", k, v)
            app.setProperty(k, v)

        app.configure()

        msp = app.getService("MessageSvc")
        for k, v in mspPropsToSet.items():
            self._msg.debug("Setting property %s : %s", k, v)
            bsh.setProperty(msp, k.encode(), v.encode())

        # Feed the jobO service with the remaining options
        jos = app.getService("JobOptionsSvc")
        for comp, name, value in bshPropsToSet:
            self._msg.debug("Adding %s.%s = %s", comp, name, value)
            bsh.addPropertyToCatalogue(
                jos, comp.encode(), name.encode(), value.encode()
            )

        sys.stdout.flush()
        return app

    def gatherProps(self, OutputLevel=INFO):
        from GaudiConfig2._configurables import Configurable

        # Convenice hack
        Configurable.getFullName = lambda self: "{}/{}".format(
            self.__cpp_type__, self.name
        )

        appPropsToSet = {}
        mspPropsToSet = {}
        bshPropsToSet = []
        for (k, v) in six.iteritems(self._theAppProps):
            appPropsToSet[k] = str(v)

        svcToCreate = []
        extSvc = []
        for svc in self._services:
            extSvc += [
                svc.getFullJobOptName(),
            ]
            if svc.name in self._servicesToCreate:
                svcToCreate.append(svc.getFullJobOptName())

        extSvc.append("PyAthena::PyComponentMgr/PyComponentMgr")

        appPropsToSet["ExtSvc"] = str(extSvc)
        appPropsToSet["CreateSvc"] = str(svcToCreate)

        mspPropsToSet["OutputLevel"] = str(OutputLevel)

        def getCompsToBeAdded(comp, namePrefix=""):
            name = namePrefix + comp.getName()
            for k, v in comp._properties.items():
                # Handle special cases of properties:
                # 1.PrivateToolHandles
                if isinstance(v, Configurable):
                    # Add the name of the tool as property to the parent
                    bshPropsToSet.append((name, k, v.getFullName()))
                    # Recursivly add properties of this tool to the JobOptionSvc
                    getCompsToBeAdded(v, namePrefix=name + ".")
                # 2. PrivateToolHandleArray
                elif isinstance(v, GaudiHandles.PrivateToolHandleArray):
                    # Add names of tools as properties to the parent
                    bshPropsToSet.append(
                        (name, k, str([v1.getFullName() for v1 in v]),)
                    )
                    # Recusivly add properties of tools to JobOptionsSvc
                    for v1 in v:
                        getCompsToBeAdded(v1, namePrefix=name + ".")
                elif (
                    not isSequence(comp) and k != "Members"
                ):  # This property his handled separatly
                    vstr = "" if v is None else str(v)
                    bshPropsToSet.append((name, k, vstr))

        for svc in self._services:
            if (
                svc.getName() != "MessageSvc"
            ):  # MessageSvc will exist already! Needs special treatment
                getCompsToBeAdded(svc)
            else:
                for k, v in svc._properties.items():
                    mspPropsToSet[k] = str(v)
        try:
            from AthenaPython import PyAthenaComps

            PyAlg = PyAthenaComps.Alg
        except ImportError:
            PyAlg = type(None)

        for seqName, algoList in six.iteritems(
            flatSequencers(self._sequence, algsCollection=self._algorithms)
        ):
            seq = self.getSequence(seqName)
            for k, v in seq._properties.items():
                if k != "Members":  # This property his handled separatly
                    vstr = "" if v is None else str(v)
                    bshPropsToSet.append((seqName, k, vstr))
            bshPropsToSet.append(
                (seqName, "Members", str([alg.getFullName() for alg in algoList]),)
            )
            for alg in algoList:
                if isinstance(
                    alg, PyAlg
                ):  # Hack for py-algs deriving from old-style configurables
                    alg._properties = alg.getValuedProperties()
                    if "OutputLevel" not in alg._properties:
                        alg._properties["OutputLevel"] = 0

                getCompsToBeAdded(alg)

                if isinstance(alg, PyAlg):
                    alg.setup2()

        condalgseq = []
        for alg in self._conditionsAlgs:
            getCompsToBeAdded(alg)
            condalgseq.append(alg.getFullName())
            bshPropsToSet.append(("AthCondSeq", "Members", str(condalgseq)))
            if isinstance(alg, PyAlg):
                alg.setup2()

        for pt in self._publicTools:
            pt.name = "ToolSvc." + pt.name
            getCompsToBeAdded(pt)

        return appPropsToSet, mspPropsToSet, bshPropsToSet

    def run(self,maxEvents=None,OutputLevel=INFO):
        # Make sure python output is flushed before triggering output from Gaudi.
        # Otherwise, observed output ordering may differ between py2/py3.
        sys.stdout.flush()

        from AthenaCommon.Debugging import allowPtrace, hookDebugger
        allowPtrace()

        app = self.createApp (OutputLevel)
        self.__verifyFinalSequencesStructure()

        #Determine maxEvents
        if maxEvents is None:
            if "EvtMax" in self._theAppProps:
                maxEvents=self._theAppProps["EvtMax"]
            else:
                maxEvents=-1

        if (self._debugStage.value == "init"): 
            hookDebugger()
        sc = app.initialize()
        if not sc.isSuccess():
            self._msg.error("Failed to initialize AppMgr")
            return sc

        sc = app.start()
        if not sc.isSuccess():
            self._msg.error("Failed to start AppMgr")
            return sc

        if (self._debugStage.value=="exec"): 
            hookDebugger()
        sc = app.run(maxEvents)
        if not sc.isSuccess():
            self._msg.error("Failure running application")
            return sc

        app.stop().ignore()

        if (self._debugStage.value == "fini"): 
            hookDebugger()
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

##################################################################################
# Compatibility layer allowing to convert between new (as of 2020) Gaudi Configurable2
# and old Confiburable classes

def __indent( indent = ""):
    return indent + "  "


def __setProperties( destConfigurableInstance, sourceConf2Instance, indent="" ):
    _log = logging.getLogger( "__setProperties".ljust(30) )
    for pname, pvalue in six.iteritems( sourceConf2Instance._properties ):
        propType = sourceConf2Instance._descriptors[pname].cpp_type
        if "PrivateToolHandleArray" in propType:
            setattr( destConfigurableInstance, pname, [conf2toConfigurable( tool, __indent( indent ) ) for tool in pvalue] )
            _log.debug( "{}Set the private tools array {} of {}".format( indent, pname,  destConfigurableInstance.name() ) )
        elif "PrivateToolHandle" in propType or "GaudiConfig2.Configurables" in propType or "ServiceHandle" in propType:
            _log.debug( "{}Set the property {}  that is private tool {} ".format( indent,  pname, destConfigurableInstance.name() ) )
            try: #sometimes it is not printable
                _log.debug("{}Tool: {}".format(indent, pvalue))
            except Exception:
                _log.debug("{}Could not print it".format(indent))
                pass
            if pvalue is not None:
                setattr( destConfigurableInstance, pname, conf2toConfigurable( pvalue, indent=__indent( indent ) ) )
            else:
                setattr( destConfigurableInstance, pname, pvalue )

        else: # plain data
            if isinstance(pvalue,(GaudiConfig2.semantics._ListHelper,GaudiConfig2.semantics._DictHelper)):
                pvalue=pvalue.data
            try: #sometimes values are not printable
                _log.debug( "{}Setting property {} to value {}".format( indent, pname, pvalue ) )
            except Exception:
                pass
            setattr( destConfigurableInstance, pname, pvalue )

def conf2toConfigurable( comp, indent="" ):
    """
    Method converts from Conf2 ( comp argument ) to old Configurable
    If the Configurable of the same name exists, the properties merging process is invoked
    """
    _log = logging.getLogger( "conf2toConfigurable".ljust(30) )
    from AthenaCommon.CFElements import compName
    def __isOldConfigurable(c):
        from AthenaCommon.Configurable import Configurable
        return True if isinstance(c, Configurable) else False


    if __isOldConfigurable( comp ):
        _log.debug( "{}Component is already OLD Configurable object {}, no conversion".format(indent, compName(comp) ) )
        return comp

    if isinstance( comp, str ):
        _log.warning( "{}Component: \"{}\" is of type string, no conversion, some properties possibly not set?".format(indent, comp ) )
        return comp

    _log.info( "{}Converting from GaudiConfig2 object {} type {}".format(indent, compName(comp), comp.__class__.__name__ ))

    def __alreadyConfigured( instanceName ):
        from AthenaCommon.Configurable import Configurable
        if instanceName in Configurable.allConfigurables:
            return  Configurable.allConfigurables[instanceName]
        return None

    def __createConf2Object( name ):
        typename,instanceName =   name.split( "/" ) if "/" in name else (name,name)
        return CompFactory.getComp( typename.replace( "__", "::" ) )( instanceName )

    def __configurableToConf2( comp, indent="" ):
        _log.debug( "{}Converting Conf2 to Configurable class {}, type {}".format( indent, comp.getFullName(), type(comp) ) )
        conf2Object = __createConf2Object( comp.getFullName() )
        __getProperties( comp, conf2Object, __indent( indent ) )
        return conf2Object

    def __getProperties( sourceConfigurableInstance, destConf2Instance, indent="" ):
        for prop, value in six.iteritems( sourceConfigurableInstance.getProperties() ):
            _log.debug( "{}Dealing with class {} property {} value type {}".format( indent, sourceConfigurableInstance.getFullJobOptName(), prop,  str( type( value ) ) ) )
            if "ServiceHandle" in str( type( value ) ):
                instance = __alreadyConfigured(value)
                if instance:
                    setattr( destConf2Instance, prop, __configurableToConf2(instance, __indent(indent)) )
            else:
                if isinstance(value,(GaudiConfig2.semantics._ListHelper,GaudiConfig2.semantics._DictHelper)):
                    value=value.data
                setattr( destConf2Instance, prop, value )

    def __findConfigurableClass( name ):
        if "::" in name: # cure namespaces
            name = name.replace( "::", "__" )

        if "<" in name:
            raise ConfigurationError( "Can not yet handle the templated components" )

        from AthenaCommon import CfgMgr
        classObj = getattr( CfgMgr, name )

        if not classObj:
            raise ConfigurationError( "CAtoGlobalWrapper could not find the component of type "+ name +" giving up ..." )

        return classObj


    def __listHelperToList(listOrDictHelper):
        if isinstance(listOrDictHelper,GaudiConfig2.semantics._ListHelper):
            return [ __listHelperToList(l) for l in listOrDictHelper.data]
        elif isinstance(listOrDictHelper,GaudiConfig2.semantics._DictHelper):
            return listOrDictHelper.data
        else:
            return listOrDictHelper

    def __areSettingsSame( existingConfigurableInstance, newConf2Instance, indent="" ):
        _log.debug( "{}Checking if setting is the same {}".format( indent, existingConfigurableInstance.getFullName() ) )
        alreadySetProperties = dict([ (pname, pvalue) for pname,pvalue
                                      in six.iteritems(existingConfigurableInstance.getValuedProperties()) ])
        for pname, pvalue in six.iteritems( newConf2Instance._properties ): # six.iteritems(comp._properties):
            if __isOldConfigurable( pvalue ):
                _log.warning( "{}New configuration object {} property {} has legacy configuration components assigned to it {}"
                              .format(indent, compName(newConf2Instance), pname, compName(pvalue) ) )
                _log.warning( "Skipping comparison, no guarantees about configuration consistency" )
                continue
            propType = newConf2Instance._descriptors[pname].cpp_type
            _log.debug("{}Comparing type: {}".format(indent, propType))
            if  "PrivateToolHandleArray" in  propType:
                for oldC, newC in zip( alreadySetProperties[pname], pvalue):
                    __areSettingsSame( oldC, newC, __indent(indent))
            elif "PrivateToolHandle" in propType or "GaudiConfig2.Configurables" in propType or "ServiceHandle" in propType:
                existingVal = getattr(existingConfigurableInstance, pname)
                if isinstance( pvalue, str ):
                    _log.warning("{}The handle {} of component {}.{} is just a string {}, skipping deeper checks, configuration may be incorrect".format(indent, propType, newConf2Instance.name, pname, pvalue))
                else:
                    _log.debug( "{}Some kind of handle  and, object type {} existing {}".format( indent, type(pvalue), type(existingVal) ) )
                    __areSettingsSame( existingVal, pvalue, indent)
            else:
                pvalue=__listHelperToList(pvalue)
                if pname not in alreadySetProperties:
                    continue
                if alreadySetProperties[pname] != pvalue:
                    _log.info("{}Merging property: {} for {}".format(indent, pname, newConf2Instance.getName() ))
                    # create surrogate
                    clone = newConf2Instance.getInstance("Clone")
                    setattr(clone, pname, alreadySetProperties[pname])
                    updatedPropValue = __listHelperToList(newConf2Instance._descriptors[pname].semantics.merge( getattr(newConf2Instance, pname), getattr(clone, pname)))
                        
                    setattr(existingConfigurable, pname, updatedPropValue)
                    del clone
                    _log.info("{} invoked GaudiConf2 semantics to merge the {} and the {} to {} for property {} of {}".format(
                        indent, alreadySetProperties[pname], pvalue, pname,  updatedPropValue, existingConfigurable.getFullName()))

    existingConfigurable = __alreadyConfigured( comp.name )
    if existingConfigurable: # if configurable exists we try to merge with it
        _log.debug( "{}Pre-existing configurable {} was found, checking if has the same properties".format( indent, comp.getName() ) )
        __areSettingsSame( existingConfigurable, comp )
        _log.debug( "{}Pre-existing configurable was found to have the same properties".format( indent, comp.name ) )
        instance = existingConfigurable
    else: # create new configurable
        _log.debug( "{}Creating component configurable {}".format( indent, comp.getFullJobOptName() ) )
        configurableClass = __findConfigurableClass( comp.getFullJobOptName().split( "/" )[0] )
        instance = configurableClass( comp.name )
        __setProperties( instance, comp, __indent( indent ) )
    return instance


def CAtoGlobalWrapper(cfgFunc, flags, **kwargs):
    """
    Temporarily available method allowing to merge CA into the configurations based on Configurable classes
    """
    if not callable(cfgFunc):
        raise TypeError("CAtoGlobalWrapper must be called with a configuration-function as parameter")
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    result = cfgFunc(flags, **kwargs)
    if isinstance(result, tuple):
        ca = result[0]
    else:
        ca = result
    Configurable.configurableRun3Behavior=0

    appendCAtoAthena(ca)
    pass

def appendCAtoAthena(ca):
    _log = logging.getLogger( "conf2toConfigurable".ljust(32) )
    _log.info( "Merging of CA to global ..." )


    from AthenaCommon.AppMgr import ServiceMgr,ToolSvc,theApp,athCondSeq,athOutSeq,athAlgSeq,topSequence
    if len(ca.getServices()) != 0:
        _log.info( "Merging services" )
        for comp in ca.getServices():
            instance = conf2toConfigurable( comp, indent="  " )
            if instance not in ServiceMgr:
                ServiceMgr += instance

    if  len(ca._conditionsAlgs) != 0:
        _log.info( "Merging condition algorithms" )
        for comp in ca._conditionsAlgs:
            instance = conf2toConfigurable( comp, indent="  " )
            if instance not in athCondSeq:
                athCondSeq += instance

    if len( ca.getPublicTools() ) != 0:
        _log.info( "Merging public tools" )
        for comp in ca.getPublicTools():
            instance = conf2toConfigurable( comp, indent="  " )
            if instance not in ToolSvc:
                ToolSvc += instance

    if len( ca.getAppProps() ) != 0:
        _log.info( "Merging ApplicationMgr properties" )
        for (propName, propValue) in six.iteritems(ca.getAppProps()):
            # Same logic as in ComponentAccumulator.setAppProperty()
            if not hasattr(theApp, propName):
                setattr(theApp, propName, propValue)
            else:
                origPropValue = getattr(theApp, propName)
                if origPropValue == propValue:
                    _log.debug("ApplicationMgr property '%s' already set to '%s'.", propName, propValue)
                elif isinstance(origPropValue, collections.Sequence) and not isinstance(origPropValue, str):
                    propValue = unifySet(origPropValue, propValue)
                    _log.info("ApplicationMgr property '%s' already set to '%s'. Overwriting with %s", propName, origPropValue, propValue)
                    setattr(theApp, propName, propValue)
                else:
                    raise DeduplicationFailed("ApplicationMgr property %s set twice: %s and %s" % (propName, origPropValue, propValue))

    _log.info( "Merging sequences and algorithms" )
    from AthenaCommon.CFElements import findSubSequence

    def __fetchOldSeq(name=""):
        from AthenaCommon.Configurable import Configurable
        currentBehaviour = Configurable.configurableRun3Behavior
        Configurable.configurableRun3Behavior=0
        from AthenaCommon.AlgSequence import AlgSequence
        seq =  AlgSequence(name)
        Configurable.configurableRun3Behavior=currentBehaviour
        return seq

    def __mergeSequences( currentConfigurableSeq, conf2Sequence, indent="" ):
        sequence = findSubSequence( currentConfigurableSeq, conf2Sequence.name )
        if not sequence:
            sequence = __fetchOldSeq( conf2Sequence.name )
            __setProperties( sequence, conf2Sequence, indent=__indent( indent ) )
            currentConfigurableSeq += sequence
            _log.info( "{}Created missing AlgSequence {} and added to {}".format( __indent( indent ), sequence.name(), currentConfigurableSeq.name() ) )

        for el in conf2Sequence.Members:
            if el.__class__.__name__ == "AthSequencer":
                __mergeSequences( sequence, el, __indent( indent ) )
            elif el.getGaudiType() == "Algorithm":
                sequence += conf2toConfigurable( el, indent=__indent( indent ) )
                _log.info( "{}Algorithm {} and added to the sequence {}".format( __indent( indent ),  el.getFullJobOptName(), sequence.name() ) )


    preconfigured = [athCondSeq,athOutSeq,athAlgSeq,topSequence]

    for seq in ca._allSequences:
        merged = False
        for pre in preconfigured:
            if seq.getName() == pre.getName():
                _log.info( "{}found sequence {} to have the same name as predefined {}".format( __indent(), seq.getName(),  pre.getName() ) )
                __mergeSequences( pre, seq )
                merged = True
                break
            if findSubSequence( pre, seq.name ):
                _log.info( "{}found sequence {} in predefined {}".format( __indent(), seq.getName(),  pre.getName() ) )
                __mergeSequences( pre, seq )
                merged = True
                break

        if not merged:
            _log.info( "{}not found sequence {} merging it to AthAlgSeq".format( __indent(), seq.name ) )
            __mergeSequences( athAlgSeq, seq )

    ca.wasMerged()
    _log.info( "Merging of CA to global done ..." )

