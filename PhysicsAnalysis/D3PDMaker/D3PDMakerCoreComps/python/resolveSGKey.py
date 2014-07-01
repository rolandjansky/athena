# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: resolveSGKey.py 550442 2013-06-10 12:16:17Z ssnyder $
#
# @file D3PDMakerCoreComps/python/resolveSGKey.py
# @author scott snyder <snyder@bnl.gov>
# @date Jan, 2010
# @brief Pick proper SG key out of a list from ObjKeyStore.
#


from AthenaCommon.Logging    import logging
from RecExConfig.ObjKeyStore import cfgKeyStore


# This is normally done for us in RecExCommon.
# But if we're being configured somehow other than RecExCommon,
# we may have to do it ourselves.
_initKeyStoreDone = False
def _initKeyStore():
    global _initKeyStoreDone
    if _initKeyStoreDone: return
    _initKeyStoreDone = True
    from AthenaCommon.GlobalFlags  import globalflags
    if globalflags.InputFormat.is_pool():
        from RecExConfig.InputFilePeeker import inputFileSummary
        cfgKeyStore.addManyTypesInputFile\
                               (inputFileSummary['eventdata_itemsList'])
    return


def resolveSGKey (type, keystr):
    """Pick proper SG key out of a list from ObjKeyStore.
    
KEYSTR is a comma-separated list of StoreGate keys.
Return the first one from that list that exists in ObjKeyStore for TYPE.
Raise an exception if none of them exist.
"""

    _initKeyStore()

    log = logging.getLogger ('D3PD')
    kl = keystr.split(',')
    for k in kl:
        if cfgKeyStore.isInInput (type, k):
            log.verbose ("Using SG key %s for type %s." % (k, type))
            return k
    if len (kl) == 1:
        # Just one, hope for the best.
        return k

    from RecExConfig.InputFilePeeker import inputFileSummary
    if inputFileSummary['nentries'] == 0:
        # No input events, can't autoconfig.  Just return the first key...
        log.warning ("No input events, can't autoconfigure. "
                     "Using SG key %s for type %s." % (kl[0], type))
        return kl[0]

    raise Exception ("No keys among `%s' for type `%s' in ObjKeyStore." %
                     (keystr, type))



def testSGKey (type, keystr):
    """Test to see if SG keys are in ObjKeyStore.
    
KEYSTR is a comma-separated list of StoreGate keys.
Return true if any key from that list exists in ObjKeyStore for TYPE.
"""

    _initKeyStore()

    kl = keystr.split(',')
    for k in kl:
        if cfgKeyStore.isInInput (type, k):
            return True
    return False

    
