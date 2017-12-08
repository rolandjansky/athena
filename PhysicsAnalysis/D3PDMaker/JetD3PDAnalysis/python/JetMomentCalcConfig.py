# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: JetMomentCalcConfig.py 545469 2013-04-22 17:31:08Z ssnyder $
#
# @file JetD3PDAnalysis/python/JetMomentCalcConfig.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2012
# @brief Configure algorithms to recalculate jet moments.
#


from D3PDMakerCoreComps.resolveSGKey import resolveSGKey
from AthenaCommon.Logging import logging


def JetMomentCalcConfig (sgkey, moments, seq):
    """Configure algorithms to recalculate jet moments.

    SGKEY is the StoreGate key for the jet collection to process.
    It is expended with resolveSGKey.

    MOMENTS is the list of moments to calculate.  It can be either:
      None: To do the `recommended' set of moments.
      'all': To do all moments.
      A list: Calculate the moments given in the list.

    SEQ is the sequence to which the algorithm should be added.
    """

    # Skip this in releases for which it's not supported.
    try:
        from JetMomentTools import JetMomentsConfigHelpers
    except ImportError:
        return

    recommendedAreaAndJVFMoments = JetMomentsConfigHelpers.recommendedAreaAndJVFMoments
    addStandardMoments = getattr (JetMomentsConfigHelpers, 'addStandardMoments', None)
    if not addStandardMoments:
        addStandardMoments = JetMomentsConfigHelpers.specialMoment

    # Always turn this on if we're making jets --- it changes jvf results.
    from JetRec.JetRecFlags import jetFlags
    jetFlags.jetPerformanceJob = True

    key = resolveSGKey ('JetCollection', sgkey)

    # Don't do this if it isn't a known jet type.
    goodtypes = ['LCTopo','Topo', 'Truth']
    if not sum([key.find(k) > 0 for k in goodtypes]):
        return False

    # Further protection.
    from JetMomentTools.GhostAssociation import interpretJetName
    if interpretJetName(key) == None:
        return # Crash otherwise

    if type(moments) == type([]) and 'origin' in moments:
        if not hasattr (JetMomentsConfigHelpers,'setupSpecialMomentTools'):
            # This isn't in devval...
            log = logging.getLogger ('JetMomentCalcConfig')
            log.warn ('OriginCorrection moments not available; skipping.')
            moments = moments[:]
            moments.remove ('origin')

    if moments == None:
        alg = recommendedAreaAndJVFMoments(key, seq=seq)
    elif moments == 'all':
        alg = addStandardMoments(key, seq=seq)
    else:
        addStandardMoments(key, moments=moments, seq=seq)

    return
