# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


#Functions used by the ComponentAccumulator to de-duplicate componentes defined multiple times
from GaudiKernel.GaudiHandles import GaudiHandleArray, PublicToolHandle, PublicToolHandleArray, ServiceHandle, ServiceHandleArray, PrivateToolHandle
from AthenaCommon.Configurable import ConfigurableAlgTool
import collections
from AthenaCommon.Logging import logging

from AthenaConfiguration.UnifyProperties import unifyProperty 

_msg=logging.getLogger('ComponentAccumulator') #'Deduplication' would the better name but breaks tons of unit-test log comparison


class DeduplicationFailed(RuntimeError):
    pass

def deduplicate(newComp,compList):
    #Check for duplicates:
    for idx,comp in enumerate(compList):
        if comp.getType()==newComp.getType() and comp.getFullName()==newComp.getFullName():
            #Found component of the same type and name
            if isinstance(comp,PublicToolHandle) or isinstance(comp,ServiceHandle):
                continue # For public tools/services we check only their full name because they are already de-duplicated in addPublicTool/addSerivce
            deduplicateComponent(newComp,comp)
            #We found a service of the same type and name and could reconcile the two instances
            _msg.debug("Reconciled configuration of component %s", comp.getJobOptName())
            #_deduplicated worked on 'newComp'. Overwrite the component in the list with the new, merged component
            compList[idx]=newComp
            return False #False means nothing got added
        #end if same name & type
    #end loop over existing components

    #No component of the same type & name found, simply append
    _msg.debug("Adding component %s to the job", newComp.getFullName())

    #The following is to work with internal list of service as well as gobal svcMgr as second parameter
    try:
        compList.append(newComp)
    except Exception:
        compList+=newComp
        pass
    return True #True means something got added



def deduplicateComponent(newComp,comp):
    #print "Checking ", comp, comp.getType(), comp.getJobOptName()
    allProps=frozenset(list(comp.getValuedProperties().keys())+list(newComp.getValuedProperties().keys()))
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

            # both are defined but with distinct type
            if type(oldprop) != type(newprop):
                raise DeduplicationFailed("Property  '%s' of component '%s' defined multiple times with conflicting types %s and %s" % \
                                          (prop,comp.getJobOptName(),type(oldprop),type(newprop)))

            propid = "%s.%s" % (comp.getType(), str(prop))

            #Note that getattr for a list property works, even if it's not in ValuedProperties
            if (oldprop!=newprop):
                #found property mismatch

                #Case 1: A public tool handle or a service handle
                if isinstance(oldprop,PublicToolHandle) or isinstance(oldprop,ServiceHandle):
                    if oldprop.getFullName()==newprop.getFullName():
                        # For public tools/services we check only their full name because they are already de-duplicated in addPublicTool/addSerivce
                        continue
                    else:
                        raise DeduplicationFailed("PublicToolHandle / ServiceHandle '%s.%s' defined multiple times with conflicting values %s and %s" % \
                                                  (comp.getType(),comp.getJobOptName(),oldprop.getFullName(),newprop.getFullName()))
                        pass
                    #Case 2: A list of public tools (PublicToolHandleArray) or a list of service (ServiceHandelArray):
                elif isinstance(oldprop,PublicToolHandleArray) or isinstance(oldprop,ServiceHandleArray):
                    mergeprop=oldprop
                    for newtool in newprop:
                        if newtool not in oldprop:
                            mergeprop+=[newtool,]
                    setattr(newComp,prop,mergeprop)
                    continue

                # Case 3: A private AlgTool:
                elif isinstance(oldprop,ConfigurableAlgTool):
                    #Recursive de-duplication of that AlgTool
                    _msg.debug("Recursivly deduplicating ToolHandle %s", oldprop)
                    mergedTool=deduplicateComponent(oldprop,newprop)
                    setattr(newComp,prop,mergedTool)
                    continue

                #Case 4: A privateToolHandleArray
                elif isinstance(oldprop,GaudiHandleArray):
                    _msg.debug("Recursivly deduplicating ToolHandleArray %s", oldprop)
                    #Unnecessary by now?
                        #if matchProperty(propid):
                        #    mergeprop = unifyProperty(propid, oldprop, newprop)
                        #    setattr(comp, prop, mergeprop)
                        #    continue

                    #Recursivly add or deduplicated tools for the new components (tools) to the old list of tools
                    #Updating the ToolHandleArray attached to newComp but preserving the order (old comes before new)
                    mergedHandleArray=oldprop
                    for newTool in newprop:
                        deduplicate(newTool,mergedHandleArray)
                    setattr(newComp,prop,mergedHandleArray)
                    pass
                    
                elif isinstance(oldprop,collections.Sequence) or isinstance(oldprop,dict): #if properties are mergeable, do that!
                    #Try merging this property. Will raise on failure
                    mergeprop=unifyProperty(propid,oldprop,newprop)
                    setattr(newComp,prop,mergeprop)

                elif isinstance(oldprop,PrivateToolHandle):
                    # This is because we get a PTH if the Property is set to None, and for some reason the equality doesn't work as expected here.
                    continue
                else:
                    raise DeduplicationFailed("component '%s' defined multiple times with mismatching property %s" % \
                                                      (comp.getJobOptName(),str(prop)))
                pass
                #end if prop-mismatch
            pass
        #end if startswith("_")
    return newComp
