# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


__all__ = [ 'setAlgOutputLevel' ]

# get levels defined VERBOSE=1 etc.
from GaudiKernel.Constants import *
# reverse lookup for printing
_levels=['ZERO','VERBOSE','DEBUG','INFO', 'WARNING', 'ERROR', 'FATAL']

def setAlgOutputLevel(lookfor,level):
    """
    Look for algorithms whose full names match the text given in 'lookfor' option,
    using a simple string.find. Set their output level to the given 'level' and
    print confirmation. Algorithms will be sought in a deep iteration of all algorithms
    and sub algorithms under the topSequence

    'lookfor' is a string.
    The full name of an algorithm take the form 'ClassName/instanceName', e.g.
    'TrigEFMultiMuFex/EFMultiMuFex_DiMu'. Some example strings to use:
    a) match anything with EFMultiMu in the class name or instance
    lookfor='EFMultiMu'
    b) match all instances of TrigEFMultiMuFex class
    lookfor='TrigEFMultiMuFex/'
    b) look for a specific instance
    lookfor='/EFMultiMuFex_DiMu'

    'outputLevel' is an integer from 1 to 6,
    or VERBOSE to FATAL if you use their definitions from GaudiKernel.Constants
    """

    import sys
    try:
        if (level <1 or level>6):
            print ">>> setAlgOutputLevel: error: level must be an integer in the range (1,6)"
            raise Exception('bad arguments')
    except Exception,e:
        print ">>> setAlgOutputLevel: exception: ",e
        raise Exception(e)

    print ">>> setAlgOutputLevel: look for algorithms named like '%s' and set their output level to %s" % (lookfor,_levels[level])
    # find algorithms that match required name
    from AthenaCommon.AlgSequence import AlgSequence,iter_algseq
    topSequence = AlgSequence()
    instances = [i for i in iter_algseq(topSequence) if i.getFullName().find(lookfor)>=0]
    uniques=sorted(set(instances)) # get rid of duplicates in list
    if len(uniques)==0:
        print ">>> setAlgOutputLevel: no algorithms found with matching name"

    # Set output levels and print confirmation
    for i in uniques:
        i.OutputLevel=level
        print ">>> setAlgOutputLevel: Set %s OutputLevel to %s" % (i.getFullName(),_levels[i.OutputLevel])

