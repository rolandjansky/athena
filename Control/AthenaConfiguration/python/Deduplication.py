# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


#Functions used by the ComponentAccumulator to de-duplicate componentes defined multiple times



from AthenaCommon.Logging import logging

_msg=logging.getLogger('ComponentAccumulator') #'Deduplication' would the better name but breaks tons of unit-test log comparison


class DeduplicationFailed(RuntimeError):
    pass

def deduplicate(newComp,compList):
    #Check for duplicates:

    for idx,comp in enumerate(compList):
        if comp.__cpp_type__==newComp.__cpp_type__ and comp.name==newComp.name:
            newComp.merge(comp)
            #We found a service of the same type and name and could reconcile the two instances
            _msg.debug("Reconciled configuration of component %s", comp.name)
            #_deduplicated worked on 'newComp'. Overwrite the component in the list with the new, merged component
            compList[idx]=newComp
            return False #False means nothing got added
        #end if same name & type
    #end loop over existing components

    #No component of the same type & name found, simply append
    _msg.debug("Adding component {}/{} to the job".format(newComp.__cpp_type__,newComp.name))

    #The following is to work with internal list of service as well as gobal svcMgr as second parameter
    try:
        compList.append(newComp)
    except Exception:
        compList+=newComp
        pass
    return True #True means something got added

