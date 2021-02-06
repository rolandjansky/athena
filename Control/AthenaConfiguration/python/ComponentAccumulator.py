# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.CFElements import isSequence,findSubSequence,findAlgorithm,flatSequencers,findOwningSequence,\
    checkSequenceConsistency, findAllAlgorithmsByName
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Debugging import DbgStage
from AthenaCommon.Constants import INFO

import GaudiKernel.GaudiHandles as GaudiHandles
import GaudiConfig2
import AthenaPython
from AthenaConfiguration.Deduplication import deduplicate, DeduplicationFailed
from AthenaConfiguration.DebuggingContext import Context, raiseWithCurrentContext, shortCallStack

import collections
import sys


class ConfigurationError(RuntimeError):
    pass
_basicServicesToCreateOrder=("CoreDumpSvc/CoreDumpSvc", "GeoModelSvc/GeoModelSvc", "DetDescrCnvSvc/DetDescrCnvSvc")



def printProperties(msg, c, nestLevel = 0, printDefaults=False, onlyComponentsOnly=False):
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
        if  not c.is_property_set(propname) and propname in ["DetStore","EvtStore", "AuditFinalize", "AuditInitialize", "AuditReinitialize", "AuditRestart", "AuditStart", "AuditStop", "AuditTools", "ExtraInputs", "ExtraOutputs"]:
            continue

        if isinstance( propval, GaudiConfig2.Configurable ):
            msg.info( "%s    * %s: %s/%s", " "*nestLevel, propname, propval.__cpp_type__, propval.getName() )
            printProperties(msg, propval, nestLevel+3)
            continue
        propstr=""
        if isinstance(propval,GaudiHandles.PublicToolHandleArray):
            ths = [th.getName() for th in propval]
            propstr = "PublicToolHandleArray([ {0} ])".format(', '.join(ths))
        elif isinstance(propval,GaudiHandles.PrivateToolHandleArray):
            ths = [th.getName() for th in propval]
            propstr = "PrivateToolHandleArray([ {0} ])".format(', '.join(ths))
        elif isinstance(propval,GaudiHandles.GaudiHandle): # Any other handle
            propstr = "Handle( {0} )".format(propval.typeAndName)
        elif not onlyComponentsOnly:
            propstr = str(propval)
        if propstr:
            msg.info( " "*nestLevel +"    * {}: {} {}".format(propname,
                                                              propstr,
                                                              "set" if c.is_property_set(propname) else "default"))
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
        self._servicesToCreate=[]
        self._privateTools=None          #A placeholder to carry a private tool(s) not yet attached to its parent
        self._primaryComp=None           #A placeholder to designate the primary service

        self._theAppProps=dict()        #Properties of the ApplicationMgr

        #Backward compatiblity hack: Allow also public tools:
        self._publicTools=[]

        #To check if this accumulator was merged:
        self._wasMerged=False
        self._isMergable=True
        self._lastAddedComponent="Unknown"
        self._creationCallStack = shortCallStack()
        self._debugStage=DbgStage()


    def setAsTopLevel(self):
        self._isMergable = False


    def _inspect(self): #Create a string some basic info about this CA, useful for debugging
        summary = "This CA contains {0} service, {1} conditions algorithms, {2} event algorithms and {3} public tools\n"\
            .format(len(self._services),len(self._conditionsAlgs),len(self._algorithms),len(self._publicTools))
        if (self._privateTools):
            if (isinstance(self._privateTools, list)):
                summary += "  Private AlgTool: " + self._privateTools[-1].getFullJobOptName() + "\n"
            else:
                summary += "  Private AlgTool: " + self._privateTools.getFullJobOptName() + "\n"
        if (self._primaryComp):
            summary += "  Primary Component: " + self._primaryComp.getFullJobOptName() + "\n"
        summary += "  Sequence(s): " + ", ".join([s.name+(" (main)" if s == self._sequence else "") for s in self._allSequences]) + "\n"
        summary += "  Last component added: " + self._lastAddedComponent+"\n"
        summary += "  Created by: " + self._creationCallStack
        return summary


    def empty(self):
        return len(self._sequence.Members)+len(self._conditionsAlgs)+len(self._services)+\
            len(self._publicTools)+len(self._theAppProps) == 0

    def __del__(self):
         if not getattr(self,'_wasMerged',True) and not self.empty() and not sys.exc_info():
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


    def printCondAlgs(self, summariseProps=False, onlyComponents=[], printDefaults=False, printComponentsOnly=False):
        self._msg.info( "Condition Algorithms" )
        for (c, flag) in filterComponents (self._conditionsAlgs, onlyComponents):
            self._msg.info( " \\__ %s (cond alg)", c.name )
            if summariseProps and flag:
                printProperties(self._msg, c, 1, printDefaults, printComponentsOnly)
        return


    # If onlyComponents is set, then only print components with names
    # that appear in the onlyComponents list.  If a name is present
    # in the list with a trailing `-', then only the name of the component
    # will be printed, not its properties.
    def printConfig(self, withDetails=False, summariseProps=False,
                    onlyComponents = [], printDefaults=False, printComponentsOnly=False):
        self._msg.info( "Event Inputs" )
        self._msg.info( "Event Algorithm Sequences" )

        def printSeqAndAlgs(seq, nestLevel = 0,
                            onlyComponents = []):
            def __prop(name):
                if name in seq._properties:
                    return seq._properties[name]
                return seq._descriptors[name].default
            if withDetails:
                self._msg.info( "%s\\__ %s (seq: %s %s)", " "*nestLevel, seq.name,
                                "SEQ" if __prop("Sequential") else "PAR",
                                "OR" if __prop("ModeOR") else "AND" )
            else:
                self._msg.info( "%s\\__ %s", " "*nestLevel, seq.name)

            nestLevel += 3
            for (c, flag) in filterComponents(seq.Members, onlyComponents):
                if isSequence(c):
                    printSeqAndAlgs(c, nestLevel, onlyComponents = onlyComponents )
                else:
                    if withDetails:
                        self._msg.info( "%s\\__ %s (alg)", " "*nestLevel, c.getFullJobOptName() )
                    else:
                        self._msg.info( "%s\\__ %s", " "*nestLevel, c.name )
                    if summariseProps and flag:
                        printProperties(self._msg, c, nestLevel, printDefaults, printComponentsOnly)


        for n,s in enumerate(self._allSequences):
            self._msg.info( "Top sequence %d", n )
            printSeqAndAlgs(s, onlyComponents = onlyComponents)

        self.printCondAlgs (summariseProps = summariseProps,
                            onlyComponents = onlyComponents)
        self._msg.info( "Services" )
        self._msg.info( [ s[0].name + (" (created) " if s[0].name in self._servicesToCreate else "")
                              for s in filterComponents (self._services, onlyComponents) ] )
        self._msg.info( "Public Tools" )
        self._msg.info( "[" )
        for (t, flag) in filterComponents (self._publicTools, onlyComponents):
            self._msg.info( "  %s,", t.getFullJobOptName() )
            # Not nested, for now
            if summariseProps and flag:
                printProperties(self._msg, t, printDefaults, printComponentsOnly)
        self._msg.info( "]" )
        self._msg.info( "Private Tools")
        self._msg.info( "[" )
        if (isinstance(self._privateTools, list)):
            for (t, flag) in filterComponents (self._privateTools, onlyComponents):
                self._msg.info( "  %s,", t.getFullJobOptsName() )
                # Not nested, for now
                if summariseProps and flag:
                    printProperties(self._msg, t, printDefaults, printComponentsOnly)
        else:
            if self._privateTools is not None:
                self._msg.info( "  %s,", self._privateTools.getFullJobOptName() )
                if summariseProps:
                    printProperties(self._msg, self._privateTools, printDefaults, printComponentsOnly)
        self._msg.info( "]" )
        self._msg.info( "theApp properties" )
        for k, v in self._theAppProps.items():
            self._msg.info("  %s : %s", k, v)


    def addSequence(self, newseq, parentName = None ):
        """ Adds new sequence. If second argument is present then it is added under another sequence  """
        from AthenaCommon.AlgSequence import AthSequencer as LegacySequence
        if isinstance( newseq, LegacySequence ):
            raise ConfigurationError('{} is not the Conf2 Sequence, ComponentAccumulator handles only the former'.format(newseq.name()))

        if not isSequence(newseq):
            raise TypeError('{} is not a sequence'.format(newseq.name))

        if parentName is None:
            parent=self._sequence
        else:
            parent = findSubSequence(self._sequence, parentName )
            if parent is None:
                raise ConfigurationError("Missing sequence {} to add new sequence to".format(parentName))

        parent.Members.append(newseq)
        algsByName = findAllAlgorithmsByName(newseq)
        for name, existingAlgs in algsByName.items():
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
            raise ConfigurationError("Can not find sequence to move {} ".format(sequence))

        owner = findOwningSequence(self._sequence, sequence)
        if owner is None:
            raise ConfigurationError("Can not find the sequence owning the {} ".format(sequence))

        dest = findSubSequence(self._sequence, destination )
        if dest is None:
            raise ConfigurationError("Can not find destination sequence {} to move to ".format(destination))

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

        if isinstance(privTool,collections.abc.Sequence):
            for t in privTool:
                if t.__component_type__ != 'AlgTool':
                    raise  ConfigurationError("ComponentAccumulator.setPrivateTools accepts only ConfigurableAlgTools or lists of ConfigurableAlgTools. Encountered {} in a list" % format(type(t)))
        else:
            if privTool.__component_type__ != "AlgTool":
                raise  ConfigurationError("ComponentAccumulator.setPrivateTools accepts only cCnfigurableAlgTools or lists of ConfigurableAlgTools. Encountered {} ".format(type(privTool)))

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
        if not isinstance(algorithms,collections.abc.Sequence):
            #Swallow both single algorithms as well as lists or tuples of algorithms
            algorithms=[algorithms,]

        if sequenceName is None:
            seq=self._sequence
        else:
            seq = findSubSequence(self._sequence, sequenceName )
        if seq is None:
            self.printConfig()
            raise ConfigurationError("Can not find sequence {}".format(sequenceName))

        for algo in algorithms:
            if not isinstance(algo,GaudiConfig2._configurables.Configurable) and not isinstance(algo,AthenaPython.Configurables.CfgPyAlgorithm):
                raise TypeError("Attempt to add wrong type: {} as event algorithm".format(type( algo ).__name__))
                
            if algo.__component_type__ != "Algorithm":
                raise TypeError("Attempt to add an {} as event algorithm".format(algo.__component_type__)) 

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
            raise ConfigurationError("Can not find an algorithm of name {} ".format(name))
        return self._algorithms[name]

    def getEventAlgos(self,seqName=None):
        if seqName is None:
            seq=self._sequence
        else:
            seq = findSubSequence(self._sequence, seqName )
        return list( set( sum( flatSequencers( seq, algsCollection=self._algorithms ).values(), []) ) )

    def addCondAlgo(self,algo,primary=False):
        if not isinstance(algo,GaudiConfig2._configurables.Configurable) and not isinstance(algo,AthenaPython.Configurables.CfgPyAlgorithm):
            raise TypeError("Attempt to add wrong type: {} as conditions algorithm".format(type( algo ).__name__))

        if algo.__component_type__ != "Algorithm":
            raise TypeError("Attempt to add wrong type: {} as conditions algorithm".format(algo.__component_type__))
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
            raise ConfigurationError("More than one conditions algorithm with name {} found".format(name))
        return hits[0]

    def addService(self,newSvc,primary=False,create=False):

        if not isinstance(newSvc,GaudiConfig2._configurables.Configurable) and not isinstance(newSvc,AthenaPython.Configurables.CfgPyService):
            raise TypeError("Attempt to add wrong type: {} as service".format(type( newSvc ).__name__))

        if newSvc.__component_type__ != "Service":
            raise TypeError("Attempt to add wrong type: {} as service".fomrat(newSvc.__component_type__))
            pass
        deduplicate(newSvc,self._services)  #will raise on conflict
        if primary:
            if self._primaryComp:
                self._msg.warning("Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.__cpp_type__,self._primaryComp.name,newSvc.__cpp_type__,newSvc.name)
            #keep a ref of the de-duplicated public tool as primary component
            self._primaryComp=self.__getOne( self._services, newSvc.name, "Services")
        self._lastAddedComponent=newSvc.name

        if create:
            sname = newSvc.getFullJobOptName()
            if sname not in self._servicesToCreate:
                self._servicesToCreate.append(sname)
        return

    def addPublicTool(self,newTool,primary=False):
        if not isinstance(newTool,GaudiConfig2._configurables.Configurable) and not isinstance(newTool,AthenaPython.Configurables.CfgPyAlgTool):
            raise TypeError("Attempt to add wrong type: {} as public AlgTool".format(type( newTool ).__name__))

        if newTool.__component_type__ != "AlgTool":
            raise TypeError("Attempt to add wrong type: {} as public AlgTool".format(newTool.__component_type__))

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
            raise ConfigurationError("Called getPrimary() but no primary component nor private AlgTool is known.\n{}".format(self._inspect()))




    def __call__(self):
        return self.getPrimary()

    def __getOne(self, allcomps, name=None, typename="???"):
        selcomps = allcomps if name is None else [ t for t in allcomps if t.name == name ]
        if len( selcomps ) == 1:
            return selcomps[0]
        raise ConfigurationError("Number of {} available {} which is != 1 expected by this API".format(typename, len(selcomps)) )

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
            elif isinstance(self._theAppProps[key],collections.abc.Sequence) and not isinstance(self._theAppProps[key],str):
                value=self._theAppProps[key] + [el for el in value if el not in self._theAppProps[key]]
                self._msg.info("ApplicationMgr property '%s' already set to '%s'. Overwriting with %s", key, self._theAppProps[key], value)
                self._theAppProps[key]=value
            else:
                raise DeduplicationFailed("AppMgr property {} set twice: {} and {}".format(key, self._theAppProps[key], value))


        pass


    def setDebugStage(self,stage):
        if stage not in DbgStage.allowed_values:
            raise RuntimeError("Allowed arguments for setDebugStage are [{}]".format(",".join(DbgStage.allowed_values)))
        self._debugStage.value=stage
        pass

    def merge(self,other, sequenceName=None):
        """Merging in the other accumulator"""
        if other is None:
            raise RuntimeError("merge called on object of type None: did you forget to return a CA from a config function?\n")

        if not isinstance(other,ComponentAccumulator):
            raise TypeError("Attempt merge wrong type {}. Only instances of ComponentAccumulator can be added".format(type(other).__name__))

        context = Context("When merging ComponentAccumulator\n{} \nto:\n{}".format(other._inspect(), self._inspect())) # noqa : F841
        if (other._privateTools is not None):
            if isinstance(other._privateTools,collections.abc.Sequence):
                raiseWithCurrentContext(RuntimeError("merge called with a ComponentAccumulator a dangling (array of) private tools\n"))
            else:
                raiseWithCurrentContext(RuntimeError("merge called with a ComponentAccumulator a dangling private tool {}/{}".format(
                                                       other._privateTools.__cpp_type__,other._privateTools.name)))



        if not other._isMergable:
            raiseWithCurrentContext(ConfigurationError("Attempted to merge the ComponentAccumulator that was unsafely manipulated (likely with foreach_component, ...) or is a top level ComponentAccumulator, in such case revert the order\n"))

        def mergeSequences( dest, src ):
            for childIdx, c in enumerate(src.Members):
                if isSequence( c ):
                    sub = findSubSequence( dest, c.name ) #depth=1 ???
                    if sub:
                        mergeSequences(sub, c )
                    else:
                        self._msg.debug("  Merging sequence %s to a sequence %s", c.name, dest.name )
                        algorithmsByName = findAllAlgorithmsByName(c)
                        for name, existingAlgs in algorithmsByName.items():
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
                        exception = None
                        try:
                            self._algorithms[c.name].merge(c)
                        except Exception as e:
                            exception = e
                        if exception:
                            raiseWithCurrentContext(exception)
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
        # if sequenceName is provided it means we should be ignoring the actual MAIN seq name there and use the sequenceName
        # that means the first search in the destination seqence needs to be cheated
        # the sequenceName argument is only relevant for the MAIN sequence, 
        # secondary top sequences are treated as if the sequenceName argument would not be provided

        for otherSeq in other._allSequences:
            found=False
            for ourSeq in self._allSequences:
                destSeqName = otherSeq.name
                if sequenceName and otherSeq == other._sequence: # if sequence moving is requested (sequenceName != None) it concerns only the main sequence
                    destSeqName = sequenceName
                    self._msg.verbose("   Will move sequence %s to %s", otherSeq.name, destSeqName )

#                destSeq = self.getSequence(sequenceName) if sequenceName else self._sequence:
                ourSeq = findSubSequence(ourSeq, destSeqName) # try to add sequence to the main structure first, to each seq in parent?
                if ourSeq:
                    mergeSequences(ourSeq, otherSeq)
                    found=True
                    self._msg.verbose("   Succeeded to merge sequence %s to %s", otherSeq.name, ourSeq.name )
                else:
                    self._msg.verbose("   Failed to merge sequence %s to any existing one, destination CA will have several top/dangling sequences", otherSeq.name )
            if not found: # just copy the sequence as a dangling one
                self._allSequences.append( otherSeq )
                mergeSequences( self._allSequences[-1], otherSeq )
        



        # Additional checking and updating other accumulator's algorithms list
        for name in other._algorithms:
            if name not in self._algorithms:
                raiseWithCurrentContext(ConfigurationError('Error in merging. Algorithm {} missing in destination accumulator\n'.format(name)))
            other._algorithms[name] = self._algorithms[name]

            #self._conditionsAlgs+=other._conditionsAlgs
        for condAlg in other._conditionsAlgs:
            self.addCondAlgo(condAlg) #Profit from deduplicaton here

        for svc in other._services:
            self.addService(svc, create = svc.getFullJobOptName() in other._servicesToCreate) #Profit from deduplicaton here

        for pt in other._publicTools:
            self.addPublicTool(pt) #Profit from deduplicaton here

        #Merge AppMgr properties:
        for (k,v) in other._theAppProps.items():
            self.setAppProperty(k,v)  #Will warn about overrides
            pass
        other._wasMerged=True

        self._lastAddedComponent = other._lastAddedComponent #+ ' (Merged)'


    def __verifyFinalSequencesStructure(self):
        if len(self._allSequences) != 1:
            raiseWithCurrentContext(ConfigurationError('It is not allowed for the storable CA to have more than one top sequence, now it has: {}'
                                         .format(','.join([ s.name for s in self._allSequences]))))


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
        ROOT.gROOT.SetBatch(True)
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

        appPropsToSet = {k: str(v) for k, v in self._theAppProps.items()}
        mspPropsToSet = {}
        bshPropsToSet = []
        svcToCreate = []
        extSvc = []
        for svc in self._services:
            extSvc += [
                svc.getFullJobOptName(),
            ]
            if svc.getFullJobOptName() in self._servicesToCreate:
                svcToCreate.append(svc.getFullJobOptName())

        # order basic services
        for bs in reversed(_basicServicesToCreateOrder):
            if bs in svcToCreate:
                svcToCreate.insert(0, svcToCreate.pop( svcToCreate.index(bs) ) )

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
                mspPropsToSet.update((k,str(v)) for k,v in svc._properties.items())
        try:
            from AthenaPython import PyAthenaComps

            PyAlg = PyAthenaComps.Alg
        except ImportError:
            PyAlg = type(None)

        for seqName, algoList in flatSequencers(self._sequence, algsCollection=self._algorithms).items():
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
    _log = logging.getLogger( "__setProperties" )
    for pname, pvalue in sourceConf2Instance._properties.items():
        if destConfigurableInstance.__class__.__name__ == 'AlgSequence' and pname == 'Members':
            continue
        propType = sourceConf2Instance._descriptors[pname].cpp_type
        if "PrivateToolHandleArray" in propType:
            setattr( destConfigurableInstance, pname, [conf2toConfigurable( tool, __indent( indent ) ) for tool in pvalue] )
            _log.debug( "%sSet the private tools array %s of %s", indent, pname, destConfigurableInstance.name() )
        elif "PrivateToolHandle" in propType or "GaudiConfig2.Configurables" in propType or "ServiceHandle" in propType:
            _log.debug( "%sSet the property %s that is private tool %s", indent,  pname, destConfigurableInstance.name() )
            try: #sometimes it is not printable
                _log.debug("%sTool: %s", indent, pvalue)
            except Exception:
                pass
            if pvalue is not None:
                setattr( destConfigurableInstance, pname, conf2toConfigurable( pvalue, indent=__indent( indent ) ) )
            else:
                setattr( destConfigurableInstance, pname, pvalue )

        else: # plain data
            if isinstance(pvalue,(GaudiConfig2.semantics._ListHelper,GaudiConfig2.semantics._DictHelper)):
                pvalue=pvalue.data
            try: #sometimes values are not printable
                _log.debug( "%sSetting property %s to value %s", indent, pname, pvalue )
            except Exception:
                pass
            setattr( destConfigurableInstance, pname, pvalue )

def conf2toConfigurable( comp, indent="", suppressDupes=False ):
    """
    Method converts from Conf2 ( comp argument ) to old Configurable
    If the Configurable of the same name exists, the properties merging process is invoked
    """
    _log = logging.getLogger( "conf2toConfigurable" )
    from AthenaCommon.CFElements import compName
    def __isOldConfigurable(c):
        from AthenaCommon.Configurable import Configurable
        return True if isinstance(c, Configurable) else False


    if __isOldConfigurable( comp ):
        _log.debug( "%sComponent is already OLD Configurable object %s, no conversion", indent, compName(comp) )
        return comp

    if isinstance( comp, str ):
        _log.warning( "%sComponent: \"%s\" is of type string, no conversion, some properties possibly not set?", indent, comp )
        return comp

    _log.debug( "%sConverting from GaudiConfig2 object %s type %s", indent, compName(comp), comp.__class__.__name__ )

    def __alreadyConfigured( instanceName ):
        from AthenaCommon.Configurable import Configurable
        if instanceName in Configurable.allConfigurables:
            return  Configurable.allConfigurables[instanceName]
        return None

    def __createConf2Object( name ):
        typename,instanceName =   name.split( "/" ) if "/" in name else (name,name)
        return CompFactory.getComp( typename.replace( "__", "::" ) )( instanceName )

    def __configurableToConf2( comp, indent="" ):
        _log.debug( "%sConverting Conf2 to Configurable class %s, type %s", indent, comp.getFullName(), type(comp) )
        conf2Object = __createConf2Object( comp.getFullName() )
        __getProperties( comp, conf2Object, __indent( indent ) )
        return conf2Object

    def __getProperties( sourceConfigurableInstance, destConf2Instance, indent="" ):
        for prop, value in sourceConfigurableInstance.getProperties().items():
            _log.debug( "%sDealing with class %s property %s value type %s",
                        indent, sourceConfigurableInstance.getFullJobOptName(), prop, type(value) )
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
            raise ConfigurationError( "CAtoGlobalWrapper could not find the component of type {} giving up ...".format(name) )

        return classObj


    def __listHelperToList(listOrDictHelper):
        if isinstance(listOrDictHelper,GaudiConfig2.semantics._ListHelper):
            return [ __listHelperToList(l) for l in listOrDictHelper.data]
        elif isinstance(listOrDictHelper,GaudiConfig2.semantics._DictHelper):
            return listOrDictHelper.data
        else:
            return listOrDictHelper

    def __areSettingsSame( existingConfigurableInstance, newConf2Instance, indent="" ):
        _log.debug( "%sChecking if settings are the same %s %s",
                    indent, existingConfigurableInstance.getFullName(), newConf2Instance.getFullJobOptName() )
        if (existingConfigurableInstance.getType() != newConf2Instance.__cpp_type__):
            raise ConfigurationError("Old/new ({} | {}) cpp types are not the same for ({} | {}) !".format(
                                    existingConfigurableInstance.getType(),newConf2Instance.__cpp_type__,
                                    existingConfigurableInstance.getFullName(), newConf2Instance.getFullJobOptName() ) )

        alreadySetProperties = existingConfigurableInstance.getValuedProperties().copy()
        _log.debug( "Existing properties: %s", alreadySetProperties )
        _log.debug( "New properties: %s", newConf2Instance._properties )
        for pname, pvalue in newConf2Instance._properties.items():
            if __isOldConfigurable( pvalue ):
                _log.warning( "%sNew configuration object %s property %s has legacy configuration "
                              "components assigned to it %s. Skipping comparison, no guarantees "
                              "about configuration consistency.",
                              indent, compName(newConf2Instance), pname, compName(pvalue) )
                continue
            propType = newConf2Instance._descriptors[pname].cpp_type
            _log.debug( "%sComparing type: %s for: %s in: %s", indent, propType, pname, existingConfigurableInstance.getFullJobOptName() )
            if  "PrivateToolHandleArray" in  propType:
                toolDict = {_.getName(): _ for _ in alreadySetProperties[pname]}
                _log.debug('Private tool properties? %s', toolDict)
                newCdict = {_.getName() : _ for _ in pvalue}
                oldCset = set(toolDict); newCset = set(newCdict)
                _log.debug('Private tool property names? %s %s', oldCset, newCset)
                if ( not (oldCset == newCset) ):
                    _log.warning('%s PrivateToolHandleArray %s  does not have the same named components',indent, pname )
                    _log.warning('%s Old (conf1) %s for %s',indent, oldCset, existingConfigurableInstance.getFullJobOptName())
                    _log.warning('%s New (conf2) %s for %s',indent, newCset, newConf2Instance.getFullJobOptName())
                    _log.warning('%s Will try to merge them, but this might go wrong!',indent)
                for oldC in oldCset & newCset:
                    __areSettingsSame( toolDict[oldC], newCdict[oldC], __indent(indent))
                # And now just the new properties in conf2 (the stuff just in conf1 is already in the objec)
                for newC in newCset-oldCset:
                    className = newCdict[newC].getFullJobOptName().split( "/" )[0]
                    _log.debug('%s %s not in oldconfig. Will try to create conf1 instance using this className: %s, and merge.',indent, newC, className)
                    configurableClass = __findConfigurableClass( className )
                    # Do not create with existing name, or it will try to get an existing public tool, if available
                    # (and public tools cannot be added to a PrivateToolHandleArray)
                    instance = configurableClass( newC + className + str(len(indent)) )

                    # Now give it the correct name
                    instance._name = newCdict[newC].name

                    __setProperties( instance, newCdict[newC], __indent( indent ) )
                    _log.debug('%s will now add %s to array.',indent, instance)
                    alreadySetProperties[pname].append(instance)
            elif "PublicToolHandleArray" in propType:
                toolSet = {_.getName() for _ in alreadySetProperties[pname]}
                _log.debug('Public tool handle array properties? %s %s', toolSet, pvalue)
                # strings?
                for newC in pvalue:
                    if isinstance(newC, str):
                        pubtoolclass, pubtoolname = newC.split('/')
                        if pubtoolname not in toolSet:
                            klass = __findConfigurableClass( pubtoolclass )
                            alreadySetProperties[pname].append(klass( pubtoolname ))
                    else:
                        _log.warning('Not handling actual Configurable2s for public tool merging yet')
                        raise Exception()
            elif "PrivateToolHandle" in propType or "GaudiConfig2.Configurables" in propType or "ServiceHandle" in propType:
                existingVal = getattr(existingConfigurableInstance, pname)
                if isinstance( pvalue, str ):
                    _log.warning("%sThe handle %s of component %s.%s is just a string %s, "
                                 "skipping deeper checks, configuration may be incorrect",
                                 indent, propType, newConf2Instance.name, pname, pvalue)
                else:
                    if pvalue is None:
                        _log.debug("%sThe property value for %s of %s is None. Skipping.", indent, pname, newConf2Instance.name )
                        continue
                    _log.debug( "%sSome kind of handle and, object type %s existing %s",
                                indent, type(pvalue), type(existingVal) )
                    __areSettingsSame( existingVal, pvalue, indent)
            else:
                if isinstance(pvalue,(GaudiConfig2.semantics._ListHelper,GaudiConfig2.semantics._DictHelper)):
                    pvalue=pvalue.data

                if pname not in alreadySetProperties:
                    _log.debug( "%sAdding property: %s for %s", indent, pname, newConf2Instance.getName() )
                    try:
                        setattr(existingConfigurableInstance, pname, pvalue)
                    except AttributeError:
                        _log.info("%s Could not set attribute. Type of existingConfigurableInstance %s.",indent, type(existingConfigurableInstance) )
                        raise
                elif alreadySetProperties[pname] != pvalue:
                    _log.debug( "%sMerging property: %s for %s", indent, pname, newConf2Instance.getName() )
                    # create surrogate
                    clone = newConf2Instance.getInstance("Clone")
                    setattr(clone, pname, alreadySetProperties[pname])
                    try:
                        updatedPropValue = __listHelperToList(newConf2Instance._descriptors[pname].semantics.merge( getattr(newConf2Instance, pname), getattr(clone, pname)))
                    except ValueError:
                        _log.warning( "Failed merging new config value (%s) and old config value (%s) for (%s) property of %s (%s) old (new). Will take value from NEW configuration, but this should be checked!",
                            getattr(newConf2Instance, pname),getattr(clone, pname),pname,existingConfigurableInstance.getFullJobOptName() ,newConf2Instance.getFullJobOptName() )
                        updatedPropValue=getattr(newConf2Instance, pname)
                        # Necessary because default value is returned for lists... see e.g.:
                        # https://gitlab.cern.ch/atlas/athena/-/blob/630b82d0540fff24c2a1da6716efc0adb53727c9/Control/AthenaCommon/python/PropertyProxy.py#L109
                    _log.debug("existingConfigurable.name: %s, pname: %s, updatedPropValue: %s", existingConfigurableInstance.name(), pname, updatedPropValue )

                    setattr(existingConfigurableInstance, pname, updatedPropValue)
                    del clone
                    _log.debug("%s invoked GaudiConf2 semantics to merge the %s and the %s to %s "
                               "for property %s of %s",
                               indent, alreadySetProperties[pname], pvalue, pname,
                               updatedPropValue, existingConfigurable.getFullName())

    _log.debug( "%s Conf2 Full name: %s ", indent, comp.getFullJobOptName() )
    existingConfigurable = __alreadyConfigured( comp.name )

    if existingConfigurable: # if configurable exists we try to merge with it
        _log.debug( "%sPre-existing configurable %s was found, checking if has the same properties", indent, comp.getName() )
        __areSettingsSame( existingConfigurable, comp )
        _log.debug( "%sPre-existing configurable %s was found to have the same properties", indent, comp.name )
        instance = existingConfigurable if not suppressDupes else None
    else: # create new configurable
        _log.debug( "%sCreating component configurable %s", indent, comp.getFullJobOptName() )
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
    _log = logging.getLogger( "conf2toConfigurable" )
    _log.info( "Merging ComponentAccumulator into global configuration" )

    from AthenaCommon.AppMgr import ServiceMgr,ToolSvc,theApp,athCondSeq,athOutSeq,athAlgSeq,topSequence
    if len( ca.getPublicTools() ) != 0:
        for comp in ca.getPublicTools():
            instance = conf2toConfigurable( comp, indent="  " )
            if instance not in ToolSvc:
                ToolSvc += instance

    if len(ca.getServices()) != 0:
        for comp in ca.getServices():
            instance = conf2toConfigurable( comp, indent="  " )
            if instance not in ServiceMgr:
                ServiceMgr += instance
        for svcName in ca._servicesToCreate:
            theApp.CreateSvc += [svcName]

    if  len(ca._conditionsAlgs) != 0:
        for comp in ca._conditionsAlgs:
            instance = conf2toConfigurable( comp, indent="  " )
            if instance not in athCondSeq:
                athCondSeq += instance

    if len( ca.getAppProps() ) != 0:
        for propName, propValue in ca.getAppProps().items():
            # Same logic as in ComponentAccumulator.setAppProperty()
            if not hasattr(theApp, propName):
                setattr(theApp, propName, propValue)
            else:
                origPropValue = getattr(theApp, propName)
                if origPropValue == propValue:
                    _log.info("ApplicationMgr property '%s' already set to '%s'.", propName, propValue)
                elif isinstance(origPropValue, collections.abc.Sequence) and not isinstance(origPropValue, str):
                    propValue =  origPropValue + [el for el in propValue if el not in origPropValue]
                    _log.info("ApplicationMgr property '%s' already set to '%s'. Overwriting with %s", propName, origPropValue, propValue)
                    setattr(theApp, propName, propValue)
                else:
                    raise DeduplicationFailed("ApplicationMgr property {} set twice: {} and {}".format(propName, origPropValue, propValue))

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
            _log.debug( "%sCreated missing AlgSequence %s and added to %s",
                        __indent( indent ), sequence.name(), currentConfigurableSeq.name() )

        for el in conf2Sequence.Members:
            if el.__class__.__name__ == "AthSequencer":
                __mergeSequences( sequence, el, __indent( indent ) )
            elif el.getGaudiType() == "Algorithm":
                toadd = conf2toConfigurable( el, indent=__indent( indent ), suppressDupes=True)
                if toadd is not None:
                    sequence += toadd
                    _log.debug( "%sAlgorithm %s and added to the sequence %s",
                                __indent( indent ),  el.getFullJobOptName(), sequence.name() )

    preconfigured = [athCondSeq,athOutSeq,athAlgSeq,topSequence]

    for seq in ca._allSequences:
        merged = False
        for pre in preconfigured:
            if seq.getName() == pre.getName():
                _log.debug( "%sfound sequence %s to have the same name as predefined %s",
                            __indent(), seq.getName(),  pre.getName() )
                __mergeSequences( pre, seq )
                merged = True
                break
            if findSubSequence( pre, seq.name ):
                _log.debug( "%sfound sequence %s in predefined %s",
                            __indent(), seq.getName(),  pre.getName() )
                __mergeSequences( pre, seq )
                merged = True
                break

        if not merged:
            _log.debug( "%snot found sequence %s merging it to AthAlgSeq", __indent(), seq.name )
            __mergeSequences( athAlgSeq, seq )

    ca.wasMerged()
    _log.debug( "Merging of CA to global done" )
