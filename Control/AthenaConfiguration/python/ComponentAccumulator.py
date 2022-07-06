# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import GaudiConfig2
import GaudiKernel.GaudiHandles as GaudiHandles

import AthenaPython.Configurables
from AthenaCommon.Logging import logging
from AthenaCommon.Debugging import DbgStage
from AthenaCommon.Constants import INFO
from AthenaCommon.CFElements import (isSequence, findSubSequence, findAlgorithm, flatSequencers,
                                     checkSequenceConsistency, findAllAlgorithmsByName)

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Deduplication import deduplicate, deduplicateOne, DeduplicationFailed
from AthenaConfiguration.DebuggingContext import (Context, raiseWithCurrentContext, shortCallStack,
                                                  createContextForDeduplication)
from collections import OrderedDict
from collections.abc import Sequence
import sys

class ConfigurationError(RuntimeError):
    pass

# Always create these services in this order:
_basicServicesToCreateOrder=("CoreDumpSvc/CoreDumpSvc",
                             "GeoModelSvc/GeoModelSvc",
                             "DetDescrCnvSvc/DetDescrCnvSvc")


def printProperties(msg, c, nestLevel = 0, printDefaults=False, onlyComponentsOnly=False):
    # Iterate in sorted order.
    propnames= sorted(c._descriptors.keys())
    for propname in propnames:

        if not printDefaults and not c.is_property_set(propname):
            continue

        propval=getattr(c,propname)
        # Ignore empty lists

        if isinstance(propval,(GaudiConfig2.semantics._ListHelper,
                               GaudiConfig2.semantics._DictHelper)) and propval.data is None:
            continue
        # Printing EvtStore could be relevant for Views?
        if not c.is_property_set(propname) and propname in ["DetStore","EvtStore", "AuditFinalize", "AuditInitialize", "AuditReinitialize", "AuditRestart", "AuditStart", "AuditStop", "AuditTools", "ExtraInputs", "ExtraOutputs"]:
            continue

        if isinstance(propval, GaudiConfig2.Configurable):
            msg.info("%s    * %s: %s/%s", " "*nestLevel, propname, propval.__cpp_type__, propval.getName())
            printProperties(msg, propval, nestLevel+3)
            continue

        propstr = ""
        if isinstance(propval, GaudiHandles.PublicToolHandleArray):
            ths = [th.getName() for th in propval]
            propstr = "PublicToolHandleArray([ {0} ])".format(', '.join(ths))
        elif isinstance(propval, GaudiHandles.PrivateToolHandleArray):
            msg.info( "%s    * %s: PrivateToolHandleArray of size %s", " "*nestLevel, propname, len(propval))
            for el in propval:
                msg.info( "%s    * %s/%s", " "*(nestLevel+3), el.__cpp_type__, el.getName())
                printProperties(msg, el, nestLevel+6)
        elif isinstance(propval, GaudiHandles.GaudiHandle): # Any other handle
            propstr = "Handle( {0} )".format(propval.typeAndName)
        elif not onlyComponentsOnly:
            propstr = str(propval)
        if propstr:
            msg.info("%s    * %s: %s", " "*nestLevel, propname, propstr)
    return


def filterComponents (comps, onlyComponents = []):
    ret = []
    for c in comps:
        if not onlyComponents or c.getName() in onlyComponents:
            ret.append((c, True))
        elif c.getName()+'-' in onlyComponents:
            ret.append((c, False))
    return ret


class ComponentAccumulator:
    # the debug mode is combination of the following strings:
    # trackCA - to track CA creation,
    # track[EventAlgo|CondAlgo|PublicTool|PrivateTool|Service|Sequence] - to track categories components addition
    debugMode=""
    def __init__(self,sequence='AthAlgSeq'):
        self._msg=logging.getLogger('ComponentAccumulator')
        if isinstance(sequence, str):
            kwargs={'IgnoreFilterPassed' : True,
                    'StopOverride'       : True }
            if sequence == 'AthAlgSeq' :
                kwargs.setdefault('ProcessDynamicDataDependencies',True)
                kwargs.setdefault('ExtraDataForDynamicConsumers',[])

            # (Nested) default sequence of event processing algorithms per sequence + their private tools
            sequence = CompFactory.AthSequencer(sequence, **kwargs)

        self._sequence = sequence
        self._allSequences = [self._sequence]
        self._algorithms = {}            #Flat algorithms list, useful for merging
        self._conditionsAlgs = []        #Unordered list of conditions algorithms + their private tools
        self._services = []              #List of service, not yet sure if the order matters here in the MT age
        self._servicesToCreate = []
        self._auditors = []              #List of auditors
        self._privateTools = None        #A placeholder to carry a private tool(s) not yet attached to its parent
        self._primaryComp = None         #A placeholder to designate the primary service
        self._currentDomain = None       #Currently marked PerfMon domain
        self._domainsRegistry = {}       #PerfMon domains registry

        self._theAppProps = dict()        #Properties of the ApplicationMgr

        #Backward compatibility hack: Allow also public tools:
        self._publicTools = []

        #To check if this accumulator was merged:
        self._wasMerged = False
        self._isMergable = True
        self._lastAddedComponent = "Unknown"
        self._creationCallStack = Context.hint if "trackCA" not in ComponentAccumulator.debugMode else shortCallStack()
        self._componentsContext = dict()
        self._debugStage = DbgStage()

    def setAsTopLevel(self):
        self._isMergable = False

    def _inspect(self): #Create a string some basic info about this CA, useful for debugging
        summary = "This CA contains {0} service, {1} conditions algorithms, {2} event algorithms and {3} public tools\n"\
            .format(len(self._services),len(self._conditionsAlgs),len(self._algorithms),len(self._publicTools))

        if self._privateTools:
            if isinstance(self._privateTools, list):
                summary += "  Private AlgTool: " + self._privateTools[-1].getFullJobOptName() + "\n"
            else:
                summary += "  Private AlgTool: " + self._privateTools.getFullJobOptName() + "\n"

        if self._primaryComp:
            summary += "  Primary Component: " + self._primaryComp.getFullJobOptName() + "\n"

        summary += "  Sequence(s): " + ", ".join([s.name+(" (main)" if s == self._sequence else "") for s in self._allSequences]) + "\n"
        summary += "  Last component added: " + self._lastAddedComponent+"\n"
        summary += "  Created by: " + self._creationCallStack
        return summary


    def empty(self):
        return (len(self._sequence.Members)+len(self._conditionsAlgs)+len(self._services)+
                len(self._publicTools)+len(self._theAppProps) == 0)

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
             log.error("Deleting a ComponentAccumulator with dangling private tool(s): %s", 
                        " ".join([t.name for t in self._privateTools]) if isinstance(self._privateTools, Sequence) else self._privateTools.name)

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
            self._msg.info( " \\__ %s (cond alg)%s", c.name, self._componentsContext.get(c.name,""))
            if summariseProps and flag:
                printProperties(self._msg, c, 1, printDefaults, printComponentsOnly)
        return


    # If onlyComponents is set, then only print components with names
    # that appear in the onlyComponents list.  If a name is present
    # in the list with a trailing `-', then only the name of the component
    # will be printed, not its properties.
    def printConfig(self, withDetails=False, summariseProps=False,
                    onlyComponents = [], printDefaults=False, printComponentsOnly=False, prefix=None):
        msg = logging.getLogger(prefix) if prefix else self._msg

        msg.info( "Event Inputs" )
        msg.info( "Event Algorithm Sequences" )

        def printSeqAndAlgs(seq, nestLevel = 0,
                            onlyComponents = []):
            def __prop(name):
                if name in seq._properties:
                    return seq._properties[name]
                return seq._descriptors[name].default
            if withDetails:
                msg.info( "%s\\__ %s (seq: %s %s)", " "*nestLevel, seq.name,
                                "SEQ" if __prop("Sequential") else "PAR",
                                "OR" if __prop("ModeOR") else "AND" + self._componentsContext.get(seq.name, "") )
            else:
                msg.info( "%s\\__ %s", " "*nestLevel, seq.name)

            nestLevel += 3
            for (c, flag) in filterComponents(seq.Members, onlyComponents):
                if isSequence(c):
                    printSeqAndAlgs(c, nestLevel, onlyComponents = onlyComponents )
                else:
                    if withDetails:
                        msg.info( "%s\\__ %s (alg) %s", " "*nestLevel, c.getFullJobOptName(), self._componentsContext.get(c.name, ""))
                    else:
                        msg.info( "%s\\__ %s", " "*nestLevel, c.name )
                    if summariseProps and flag:
                        printProperties(msg, c, nestLevel, printDefaults, printComponentsOnly)


        for n,s in enumerate(self._allSequences):
            msg.info( "Top sequence %d", n )
            printSeqAndAlgs(s, onlyComponents = onlyComponents)

        self.printCondAlgs (summariseProps = summariseProps,
                            onlyComponents = onlyComponents)
        msg.info( "Services" )
        msg.info( [ s[0].name + (" (created) " if s[0].name in self._servicesToCreate else "")
                              for s in filterComponents (self._services, onlyComponents) ] )
        msg.info( "Public Tools" )
        msg.info( "[" )
        for (t, flag) in filterComponents (self._publicTools, onlyComponents):
            msg.info( "  %s,", t.getFullJobOptName() + self._componentsContext.get(t.name,""))
            # Not nested, for now
            if summariseProps and flag:
                printProperties(msg, t, printDefaults, printComponentsOnly)
        msg.info( "]" )
        msg.info( "Private Tools")
        msg.info( "[" )
        if self._privateTools:
            for tool in self._privateTools if isinstance(self._privateTools, Sequence) else [self._privateTools]:
                msg.info( "  %s,", tool.getFullJobOptName() + self._componentsContext.get(tool.name,""))
                if summariseProps:
                    printProperties(msg, tool, printDefaults, printComponentsOnly)
        msg.info( "]" )
        if self._auditors:
            msg.info( "Auditors" )
            msg.info( [ a[0].name for a in filterComponents(self._auditors, onlyComponents) ] )

        msg.info( "theApp properties" )
        for k, v in self._theAppProps.items():
            msg.info("  %s : %s", k, v)

    def getIO(self):
        """
        Returns information about inputs needed and outputs produced by this CA

        It is a list of dictionaries containing the: type, key, R / W, the component and name of the property via which it is set
        """
        def __getHandles(comp):
            io = []
            for i in comp.ExtraInputs:
                io.append({"type": i.split("#")[0],
                            "key": i.split("#")[1],
                            "comp": comp.getFullJobOptName(),
                            "mode":  "R",
                            "prop": "ExtraInputs"})
            for i in comp.ExtraOutputs:
                io.append({"type": i.split("#")[0],
                            "key": i.split("#")[1],
                            "comp": comp.getFullJobOptName(),
                            "mode":  "W",
                            "prop": "ExtraOutputs"})
            from GaudiKernel.DataHandle import DataHandle
            for prop, descr in comp._descriptors.items():
                if isinstance(descr.default, DataHandle):
                    io.append( {"type": descr.default.type(),
                                "key":  comp._properties[prop] if prop in comp._properties else  descr.default.path(),
                                "comp": comp.getFullJobOptName(),
                                "mode": descr.default.mode(),
                                "prop": prop })
                # TODO we should consider instantiating c++ defaults and fetching corresponsing props
                if "PrivateToolHandle" == descr.cpp_type and prop in comp._properties:
                    io.extend( __getHandles(comp._properties[prop]) )
                if "PrivateToolHandleArray" == descr.cpp_type and prop in comp._properties:
                    for tool in getattr(comp, prop):
                        io.extend( __getHandles(tool))
            return io

        ret = []
        for comp in self._allComponents():
            ret.extend(__getHandles(comp))
        return ret


    def addSequence(self, newseq, parentName = None ):
        """ Adds new sequence. If second argument is present then it is added under another sequence  """
        from AthenaCommon.AlgSequence import AthSequencer as LegacySequence
        if isinstance( newseq, LegacySequence ):
            raise ConfigurationError('{} is not the Conf2 Sequence, ComponentAccumulator handles only the former'.format(newseq.name))

        if not isSequence(newseq):
            raise TypeError('{} is not a sequence'.format(newseq.name))

        algorithmsInside = findAllAlgorithmsByName(newseq)
        if len(algorithmsInside) != 0:
            raise ConfigurationError('{} contains algorithms (or sub-sequences contain them). That is not supported. Construct ComponentAccumulator and merge it instead'.format(newseq.name))


        if parentName is None:
            parent=self._sequence
        else:
            parent = findSubSequence(self._sequence, parentName )
            if parent is None:
                raise ConfigurationError("Missing sequence {} to add new sequence to".format(parentName))

        parent.Members.append(newseq)
        if "trackSequence" in ComponentAccumulator.debugMode:
            self._componentsContext[newseq] = shortCallStack()
        return newseq


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
            raise ConfigurationError("This ComponentAccumulator holds already a (list of) private tool(s). "
                                     "Only one (list of) private tool(s) is allowed")

        if isinstance(privTool, Sequence):
            for t in privTool:
                if t.__component_type__ != 'AlgTool':
                    raise ConfigurationError("ComponentAccumulator.setPrivateTools accepts only ConfigurableAlgTools "
                                             f"or lists of ConfigurableAlgTools. Encountered {type(t)} in a list")
        else:
            if privTool.__component_type__ != "AlgTool":
                raise ConfigurationError("ComponentAccumulator.setPrivateTools accepts only ConfigurableAlgTools "
                                          f"or lists of ConfigurableAlgTools. Encountered {type(privTool)}")

        self._privateTools=privTool
        if "trackPrivateTool" in ComponentAccumulator.debugMode:
            for tool in self._privateTools if isinstance(privTool, Sequence) else [self._privateTools]:
                self._componentsContext[tool.name] = shortCallStack()

        return

    def popPrivateTools(self, quiet=False):
        """Get the (list of) private AlgTools from this ComponentAccumulator.
        The CA will not keep any reference to the AlgTool. Throw an exception if
        no tools are available unless quiet=True.
        """
        tool = self._privateTools
        if not quiet and tool is None:
            raise ConfigurationError("Private tool(s) requested, but none are present")
        self._privateTools=None
        return tool

    def popToolsAndMerge(self, other):
        """ Merging in the other accumulator and getting the (list of) private AlgTools
        from this ComponentAccumulator.
        """
        if other is None:
            raise RuntimeError("popToolsAndMerge called on object of type None: "
                               "did you forget to return a CA from a config function?")
        tool = other.popPrivateTools()
        self.merge(other)
        return tool

    def flagPerfmonDomain(self, name):
        """ Mark the beginning of a new PerfMon domain. """
        self._msg.debug(f"Toggling the current algorithm domain to {name}")
        self._currentDomain = name

    def getInvertedPerfmonDomains(self):
        """ The actual registry keeps "alg":"domain".
        This function inverts the registry to get "domain":["algs"].
        """
        result = {}
        for i, v in self._domainsRegistry.items():
            result[v] = [i] if v not in result.keys() else result[v] + [i]
        return result

    def getAlgPerfmonDomain(self, name):
        """ Return the PerfMon domain of the given algorithm """
        if name in self._domainsRegistry:
            return self._domainsRegistry[name]
        else:
            self._msg.info(f"Algorithm {name} is not in PerfMon domains registry")
            return None

    def addAlgToPerfmonDomains(self, name, domain, overwrite=False):
        """ Add the algorithm to the domains registry. """
        if name not in self._domainsRegistry:
            if domain:
                self._domainsRegistry[name] = domain
                self._msg.debug(f"Added algorithm {name} to the PerfMon domain {domain}")
        else:
            if overwrite and domain:
                self._msg.info(f"Reassigned algorithm {name} "
                               f"from {self._domainsRegistry[name]} "
                               f"to {domain} PerfMon domain")
                self._domainsRegistry[name] = domain
            else:
                self._msg.debug(f"Algorithm {name} is already in the PerfMon "
                                 "domain, if you want to reassign do overwrite=True")

    def printPerfmonDomains(self):
        """ Print the PerfMon domains. """
        invertedDomains = self.getInvertedPerfmonDomains()
        self._msg.info(":: This CA contains the following PerfMon domains ::")
        self._msg.info(f":: There are a total of {len(self._domainsRegistry)} "
                       f"registered algorithms in {len(invertedDomains)} domains  ::")
        for domain, algs in invertedDomains.items():
            self._msg.info(f"+ Domain : {domain}")
            for alg in algs:
                self._msg.info("\\_ %s", alg)
        self._msg.info(":: End of PerfMon domains ::")

    def addEventAlgo(self, algorithms,sequenceName=None,primary=False,domain=None):
        if not isinstance(algorithms, Sequence):
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
            if not isinstance(algo, (GaudiConfig2._configurables.Configurable,
                                     AthenaPython.Configurables.CfgPyAlgorithm)):
                raise TypeError(f"Attempt to add wrong type: {type(algo).__name__} as event algorithm")

            if algo.__component_type__ != "Algorithm":
                raise TypeError(f"Attempt to add an {algo.__component_type__} as event algorithm")

            if algo.name in self._algorithms:
                context = createContextForDeduplication("Merging with existing Event Algorithm", algo.name, self._componentsContext) # noqa : F841
                deduplicateOne(algo, self._algorithms[algo.name])
                deduplicateOne(self._algorithms[algo.name], algo)
            else:
                self._algorithms[algo.name]=algo

            existingAlgInDest = findAlgorithm(seq, algo.name)
            if not existingAlgInDest:
                seq.Members.append(self._algorithms[algo.name])
                # Assign the algorithm to a domain
                self.addAlgToPerfmonDomains(algo.name, self._currentDomain if not domain else domain)

        if primary:
            if len(algorithms)>1:
                self._msg.warning("Called addEvenAlgo with a list of algorithms and primary==True. "
                                  "Designating the first algorithm as primary component")
            if self._primaryComp:
                self._msg.warning("addEventAlgo: Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.__cpp_type__, self._primaryComp.name,
                                  algorithms[0].__cpp_type__, algorithms[0].name)
            #keep a ref of the algorithm as primary component
            self._primaryComp = algorithms[0]
        self._lastAddedComponent = algorithms[-1].name

        if "trackEventAlgo" in ComponentAccumulator.debugMode:
            for algo in algorithms:
                self._componentsContext[algo.name] = shortCallStack()

        return None

    def getEventAlgo(self, name=None):
        """Get algorithm with `name`"""
        if name not in self._algorithms:
            raise ConfigurationError("Can not find an algorithm of name {} ".format(name))
        return self._algorithms[name]

    def getEventAlgos(self, seqName=None):
        """Get all algorithms within sequence"""
        if seqName is None:
            seq=self._sequence
        else:
            seq = findSubSequence(self._sequence, seqName )
        return list( OrderedDict.fromkeys( sum( flatSequencers( seq, algsCollection=self._algorithms ).values(), []) ).keys() )

    def addCondAlgo(self,algo,primary=False,domain=None):
        """Add Conditions algorithm"""
        if not isinstance(algo, (GaudiConfig2._configurables.Configurable,
                                 AthenaPython.Configurables.CfgPyAlgorithm)):
            raise TypeError(f"Attempt to add wrong type: {type(algo).__name__} as conditions algorithm")

        if algo.__component_type__ != "Algorithm":
            raise TypeError(f"Attempt to add wrong type: {algo.__component_type__} as conditions algorithm")

        context = createContextForDeduplication("Merging with existing Conditions Algorithm", algo.name, self._componentsContext) # noqa : F841

        deduplicate(algo, self._conditionsAlgs) #will raise on conflict
        if primary:
            if self._primaryComp:
                self._msg.warning("addCondAlgo: Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.__cpp_type__, self._primaryComp.name,
                                  algo.__cpp_type__, algo.name)
            #keep a ref of the de-duplicated conditions algorithm as primary component
            self._primaryComp = self.__getOne(self._conditionsAlgs, algo.name, "ConditionsAlgos")

        self._lastAddedComponent=algo.name
        if "trackCondAlgo" in ComponentAccumulator.debugMode:
            self._componentsContext[algo.name] = shortCallStack()

        # Assign the algorithm to a domain
        self.addAlgToPerfmonDomains(algo.name, 'Conditions' if not domain else domain)

        return algo


    def getCondAlgo(self, name):
        """Get Conditions algorithm"""
        hits = [a for a in self._conditionsAlgs if a.name==name]
        if len(hits)!=1:
            raise ConfigurationError(f"{len(hits)} conditions algorithms with name {name} found")

        return hits[0]


    def addService(self, newSvc, primary=False, create=False):
        """Add service"""
        if not isinstance(newSvc, (GaudiConfig2._configurables.Configurable,
                                   AthenaPython.Configurables.CfgPyService)):
            raise TypeError(f"Attempt to add wrong type: {type(newSvc).__name__} as service")

        if newSvc.__component_type__ != "Service":
            raise TypeError(f"Attempt to add wrong type: {newSvc.__component_type__} as service")

        context = createContextForDeduplication("Merging with existing Service", newSvc.name, self._componentsContext) # noqa : F841

        deduplicate(newSvc, self._services)  #may raise on conflict
        if primary:
            if self._primaryComp:
                self._msg.warning("addService: Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.__cpp_type__, self._primaryComp.name,
                                  newSvc.__cpp_type__, newSvc.name)
            #keep a ref of the de-duplicated service as primary component
            self._primaryComp=self.__getOne( self._services, newSvc.name, "Services")
        self._lastAddedComponent=newSvc.name

        if create:
            sname = newSvc.getFullJobOptName()
            if sname not in self._servicesToCreate:
                self._servicesToCreate.append(sname)
        if "trackService" in  ComponentAccumulator.debugMode:
            self._componentsContext[newSvc.name] = shortCallStack()


    def addAuditor(self, auditor):
        """Add Auditor to ComponentAccumulator.
        This function will also create the required AuditorSvc."""
        if not isinstance(auditor, (GaudiConfig2._configurables.Configurable,
                                    AthenaPython.Configurables.CfgPyAud)):
            raise TypeError(f"Attempt to add wrong type: {type(auditor).__name__} as auditor")

        if auditor.__component_type__ != "Auditor":
            raise TypeError(f"Attempt to add wrong type: {auditor.__component_type__} as auditor")

        context = createContextForDeduplication("Merging with existing auditors", auditor.name, self._componentsContext) # noqa : F841

        deduplicate(auditor, self._auditors)  #may raise on conflict
        self.addService(CompFactory.AuditorSvc(Auditors=[auditor.getFullJobOptName()]))
        self._lastAddedComponent = auditor.name


    def addPublicTool(self, newTool, primary=False):
        """Add public tool"""
        if not isinstance(newTool, (GaudiConfig2._configurables.Configurable,
                                    AthenaPython.Configurables.CfgPyAlgTool)):
            raise TypeError(f"Attempt to add wrong type: {type(newTool).__name__} as public AlgTool")

        if newTool.__component_type__ != "AlgTool":
            raise TypeError(f"Attempt to add wrong type: {newTool.__component_type__} as public AlgTool")

        context = createContextForDeduplication("Merging with existing Public Tool", newTool.name, self._componentsContext) # noqa : F841

        deduplicate(newTool,self._publicTools)
        if primary:
            if self._primaryComp:
                self._msg.warning("addPublicTool: Overwriting primary component of this CA. Was %s/%s, now %s/%s",
                                  self._primaryComp.__cpp_type__, self._primaryComp.name,
                                  newTool.__cpp_type__, newTool.name)
            #keep a ref of the de-duplicated tool as primary component
            self._primaryComp=self.__getOne( self._publicTools, newTool.name, "Public Tool")
        self._lastAddedComponent=newTool.name
        if "trackPublicTool" in ComponentAccumulator.debugMode:
            self._componentsContext[newTool.name] = shortCallStack()


    def getPrimary(self):
        """Get designated primary component"""
        if self._privateTools:
            return self.popPrivateTools()
        elif self._primaryComp:
            return self._primaryComp
        else:
            raise ConfigurationError("Called getPrimary() but no primary component nor private AlgTool is known.\n{}".format(self._inspect()))

    def getPrimaryAndMerge(self, other):
        """ Merging in the other accumulator and getting the primary component"""
        if other is None:
            raise RuntimeError("merge called on object of type None: did you forget to return a CA from a config function?")
        comp = other.getPrimary()
        self.merge(other)
        return comp


    def __call__(self):
        return self.getPrimary()

    def __getOne(self, allcomps, name=None, typename="???"):
        selcomps = allcomps if name is None else [ t for t in allcomps if t.name == name ]
        if len( selcomps ) == 0:
            raise ConfigurationError(f"Requested component of name {name} but is missing" )

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
            elif isinstance(self._theAppProps[key], Sequence) and not isinstance(self._theAppProps[key],str):
                value=self._theAppProps[key] + [el for el in value if el not in self._theAppProps[key]]
                self._msg.info("ApplicationMgr property '%s' already set to '%s'. Overwriting with %s", key, self._theAppProps[key], value)
                self._theAppProps[key]=value
            else:
                raise DeduplicationFailed("AppMgr property {} set twice: {} and {}".format(key, self._theAppProps[key], value))


    def setDebugStage(self,stage):
        if stage not in DbgStage.allowed_values:
            raise RuntimeError("Allowed arguments for setDebugStage are [{}]".format(",".join(DbgStage.allowed_values)))
        self._debugStage.value = stage


    def merge(self,other, sequenceName=None):
        """Merging in the other accumulator"""
        if other is None:
            raise RuntimeError("merge called on object of type None: "
                               "did you forget to return a CA from a config function?")

        if not isinstance(other,ComponentAccumulator):
            raise TypeError(f"Attempt to merge wrong type {type(other).__name__}. "
                            "Only instances of ComponentAccumulator can be added")

        context = (Context.hint if not ComponentAccumulator.debugMode else   # noqa: F841
                   Context("When merging the ComponentAccumulator:\n{} \nto:\n{}".format(other._inspect(), self._inspect())))

        if other._privateTools is not None:
            if isinstance(other._privateTools, Sequence):
                raiseWithCurrentContext(RuntimeError(
                    "merge called on ComponentAccumulator with a dangling (array of) private tools\n"))
            else:
                raiseWithCurrentContext(RuntimeError(
                    "merge called on ComponentAccumulator with a dangling private tool "
                    f"{other._privateTools.__cpp_type__}/{other._privateTools.name}"))

        if not other._isMergable:
            raiseWithCurrentContext(ConfigurationError(
                "Attempted to merge the ComponentAccumulator that was unsafely manipulated "
                "(likely with foreach_component, ...) or is a top level ComponentAccumulator, "
                "in such case revert the order\n"))

        def mergeSequences( dest, src ):
            if dest.name == src.name:
                for seqProp in dest._descriptors.keys():
                    if getattr(dest, seqProp) != getattr(src, seqProp) and seqProp != "Members":
                        raise RuntimeError(
                            f"merge called with sequences: '{(dest.name, src.name)}' having property '{seqProp}'"
                            f"of different values {getattr(dest, seqProp)} vs {getattr(src, seqProp)}")

            for childIdx, c in enumerate(src.Members):
                if isSequence( c ):
                    sub = findSubSequence( dest, c.name ) #depth=1 ???
                    if sub:
                        mergeSequences(sub, c )
                    else:
                        self._msg.debug("  Merging sequence %s to a destination sequence %s", c.name, dest.name )
                        algorithmsByName = findAllAlgorithmsByName(c) # dictionary: algName (alg, parentSeq, indexInParentSeq)                        
                        for name, existingAlgs in algorithmsByName.items():
                            # all algorithms from incoming CA are already deduplicated, so we can only handle the fist one
                            algInstance, _, _ = existingAlgs[0]
                            if name not in self._algorithms:
                                self._algorithms[name] = algInstance
                            else:
                                dedupContext1 = createContextForDeduplication("While merging sequences adding incoming algorithm", c.name, other._componentsContext) # noqa : F841
                                dedupContext2 = createContextForDeduplication("While merging sequences adding to existing algorithm", c.name, self._componentsContext) # noqa : F841
                                deduplicateOne(self._algorithms[name], algInstance)
                                deduplicateOne(algInstance, self._algorithms[name])
                            for _, parent, idx in existingAlgs: # put the deduplicated algo back into original sequences
                                parent.Members[idx] = self._algorithms[name]
                            # Add the algorithm to the PerfMon domains
                            self.addAlgToPerfmonDomains(name, other._domainsRegistry[name] if name in other._domainsRegistry else self._currentDomain)
                        dest.Members.append(c)

                else: # an algorithm
                    if c.name in self._algorithms:
                        dedupContext1 = createContextForDeduplication("While merging sequences adding incoming algorithm", c.name, other._componentsContext) # noqa : F841
                        dedupContext2 = createContextForDeduplication("While merging sequences adding to existing algorithm", c.name, self._componentsContext) # noqa : F841

                        deduplicateOne(self._algorithms[c.name], c)
                        deduplicateOne(c, self._algorithms[c.name])
                        src.Members[childIdx] = self._algorithms[c.name]
                    else:
                        self._algorithms[c.name] = c

                    existingAlgInDest = findAlgorithm( dest, c.name, depth=1 )
                    if not existingAlgInDest:
                        self._msg.debug("   Adding algorithm %s to a sequence %s", c.name, dest.name )
                        dest.Members.append(c)

                    # Add the algorithm to the PerfMon domains
                    self.addAlgToPerfmonDomains(c.name, other._domainsRegistry[c.name] if c.name in other._domainsRegistry else self._currentDomain)

        # Merge sequences:
        # mergeSequences(destSeq, other._sequence)
        # if sequenceName is provided it means we should be ignoring the actual MAIN seq name there and use the sequenceName
        # that means the first search in the destination sequence needs to be cheated
        # the sequenceName argument is only relevant for the MAIN sequence, 
        # secondary top sequences are treated as if the sequenceName argument would not be provided

        for otherSeq in other._allSequences:
            found=False
            for ourSeq in self._allSequences:
                destSeqName = otherSeq.name
                if sequenceName and otherSeq == other._sequence: # if sequence moving is requested (sequenceName != None) it concerns only the main sequence
                    destSeqName = sequenceName
                    self._msg.verbose("   Will move sequence %s to %s", otherSeq.name, destSeqName )

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
            addContext = createContextForDeduplication("Merging incoming Conditions Algorithm", condAlg.name, other._componentsContext) # noqa : F841
            self.addCondAlgo(condAlg) #Profit from deduplicaton here

        for svc in other._services:
            addContext = createContextForDeduplication("Merging incoming Service", svc.name, other._componentsContext) # noqa : F841
            self.addService(svc, create = svc.getFullJobOptName() in other._servicesToCreate) #Profit from deduplicaton here

        for pt in other._publicTools:
            addContext = createContextForDeduplication("Merging incoming Public Tool", pt.name, other._componentsContext) # noqa : F841
            self.addPublicTool(pt) #Profit from deduplicaton here

        #Merge AppMgr properties:
        for (k,v) in other._theAppProps.items():
            self.setAppProperty(k,v)  #Will warn about overrides
            pass
        other._wasMerged=True

        self._lastAddedComponent = other._lastAddedComponent #+ ' (Merged)'
        self._componentsContext.update(other._componentsContext) # update the context so it contains an information about the new components (and refreshed old components)

    def __verifyFinalSequencesStructure(self):
        if len(self._allSequences) != 1:
            raiseWithCurrentContext(ConfigurationError('It is not allowed for the storable CA to have more than one top sequence, now it has: {}'
                                         .format(','.join([ s.name for s in self._allSequences]))))


    def wasMerged(self):
        """ Declares CA as merged

        This is temporarily needed by HLT and should not be used elsewhere
        """
        self._wasMerged=True

    def _allComponents(self):
        """ returns iterable over all components """
        import itertools
        return itertools.chain(self._publicTools,
                               self._privateTools if self._privateTools else [],
                               self._algorithms.values(),
                               self._conditionsAlgs)


    def store(self,outfile, withDefaultHandles=False):
        """
        Saves CA in pickle form

        when withDefaultHandles is True, also the handles that are not set are saved
        """

        checkSequenceConsistency(self._sequence)
        
        self.wasMerged()
        if withDefaultHandles:
            from AthenaConfiguration.Utils import loadDefaultComps, exposeHandles
            loadDefaultComps(self._allComponents())
            exposeHandles(self._allComponents())
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
        for comp, name, value in bshPropsToSet:
            self._msg.debug("Adding %s.%s = %s", comp, name, value)
            app.setOption(f"{comp}.{name}", value)

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
                    # Recursively add properties of this tool to the JobOptionSvc
                    getCompsToBeAdded(v, namePrefix=name + ".")
                # 2. PrivateToolHandleArray
                elif isinstance(v, GaudiHandles.PrivateToolHandleArray):
                    # Add names of tools as properties to the parent
                    bshPropsToSet.append(
                        (name, k, str([v1.getFullName() for v1 in v]),)
                    )
                    # Recursively add properties of tools to JobOptionsSvc
                    for v1 in v:
                        getCompsToBeAdded(v1, namePrefix=name + ".")
                elif (
                    not isSequence(comp) and k != "Members"
                ):  # This property is handled separately
                    # For a list of DataHandle, we need to stringify
                    # each element individually.  Otherwise, we get the repr
                    # version of the elements, which Gaudi JO will choke on.
                    from GaudiKernel.DataHandle import DataHandle
                    if isinstance(v, list) and v and isinstance(v[0], DataHandle):
                        v = [str(x) for x in v]
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
                if k != "Members":  # This property his handled separately
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

        for aud in self._auditors:
            getCompsToBeAdded(aud)

        return appPropsToSet, mspPropsToSet, bshPropsToSet

    def run(self,maxEvents=None,OutputLevel=INFO):
        # Make sure python output is flushed before triggering output from Gaudi.
        # Otherwise, observed output ordering may differ between py2/py3.
        sys.stdout.flush()

        from AthenaCommon.Debugging import allowPtrace, hookDebugger
        allowPtrace()

        checkSequenceConsistency(self._sequence)
        
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

        The components name & locations in the CF tree are translated into the unix like path.
        Components of matching path are taken under consideration in setting the property.
        If the property is set successfully an INFO message is printed. Else, a warning is printed.

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


# Legacy support
from AthenaConfiguration.LegacySupport import (conf2toConfigurable,  # noqa: F401 (for client use)
                                               CAtoGlobalWrapper,
                                               appendCAtoAthena)
