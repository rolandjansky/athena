# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigMETD3PD.py 487405 2012-03-08 17:28:58Z krasznaa $
#

# Some common import(s):
from AthenaCommon.AlgSequence import AlgSequence

##
# @short Function adding the Trigger MET information to the D3PD
#
# The idea here is that one could add all the HLT MET information to his/her
# D3PD with something like this:
#
#  <code>
#    from TrigMissingETD3PDMaker.TrigMETD3PD import TrigMETD3PDObjects
#    TrigMETD3PDObjects( myd3pdalg, 2 )
#  </code>
#
# $Revision: 487405 $
# $Date: 2012-03-08 18:28:58 +0100 (Thu, 08 Mar 2012) $
#
def TrigMETD3PDObjects( alg, level = 10 ):

    # The LVL1 information:
    from TriggerD3PDMaker.EnergySumROID3PDObject import EnergySumROID3PDObject
    alg += EnergySumROID3PDObject( level, prefix = "trig_L1_esum_" )

    # The LVL2 information:
    from TrigMissingETD3PDMaker.TrigMETD3PDObject import TrigMETD3PDObject
    alg += TrigMETD3PDObject( level, prefix = "trig_L2_met_",
                                  sgkey = "HLT_T2MissingET" )

    # The New LVL2 FEB information:
    from RecExConfig.ObjKeyStore import cfgKeyStore
    if cfgKeyStore.isInInputFile( "TrigMissingETContainer", "HLT_L2MissingET_FEB" ):
        alg += TrigMETD3PDObject( level, prefix = "trig_L2_feb_met_",
                                  sgkey = "HLT_L2MissingET_FEB" )
        pass

    # The EF information:
    alg += TrigMETD3PDObject( level, prefix = "trig_EF_met_",
                              sgkey = "HLT_TrigEFMissingET" )
    if cfgKeyStore.isInInputFile( "TrigMissingETContainer",
                                  "HLT_TrigEFMissingET_FEB" ):
        alg += TrigMETD3PDObject( level, prefix = "trig_EF_feb_met_",
                                  sgkey = "HLT_TrigEFMissingET_FEB" )
        pass
    if cfgKeyStore.isInInputFile( "TrigMissingETContainer",
                                  "HLT_TrigEFMissingET_topocl" ):
        alg += TrigMETD3PDObject( level, prefix = "trig_EF_topocl_met_",
                                  sgkey = "HLT_TrigEFMissingET_topocl" )
        pass

    # The jet ETsum info:
    if cfgKeyStore.isInInputFile( "TrigMissingETContainer",
                                  "HLT_L2JetEtSum" ):
        alg += TrigMETD3PDObject( level, prefix = "trig_L2_jetetsum_",
                                  sgkey = "HLT_L2JetEtSum" )
        pass
    if cfgKeyStore.isInInputFile( "TrigMissingETContainer",
                                  "HLT_EFJetEtSum" ):
        alg += TrigMETD3PDObject( level, prefix = "trig_EF_jetetsum_",
                                  sgkey = "HLT_EFJetEtSum" )
        pass

    return alg

##
# @short Function creating the MET trigger D3PD
#
# This function can be used in a top jobOption to create the trigger MET
# D3PD.
#
# $Revision: 487405 $
# $Date: 2012-03-08 18:28:58 +0100 (Thu, 08 Mar 2012) $
#
def TrigMETD3PD( fname, level = 10, tname = "met", sequence = AlgSequence() ):

    # Create a D3PD maker algorithm:
    import D3PDMakerCoreComps
    alg = D3PDMakerCoreComps.MakerAlg( tname, seq = sequence, file = fname )

    # Add all the blocks to the D3PD:
    TrigMETD3PDObjects( alg, level )

    return alg
