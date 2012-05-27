# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigJetD3PD.py 491506 2012-03-29 08:55:23Z krasznaa $

#
# Set up the trigger navigation flattening for the jet chains. Done ins the
# same wat as it was done for the egammaD3PD...
#
from AthenaCommon.AlgSequence import AlgSequence
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
InitTrigD3PDObject = make_Void_D3PDObject( default_name = 'InitTrig' )
def _initTrigHook( c, **kw ):
    import TrigJetD3PDMaker.Setup_RoILinksCnvTool_IBv3
    preseq = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )
    if not hasattr( preseq, 'FlatHltNaviMaker' ):
        from TriggerMenuAnalysis.TriggerMenuAnalysisConf import FlatHltNaviMaker
        preseq += FlatHltNaviMaker( 'FlatHltNaviMaker' )
    return
InitTrigD3PDObject.defineHook( _initTrigHook )

##
# @short Function for passing parameters to individual D3PDObjects
#
# This function helps in making it possible to pass specific options
# to the D3PDObjects added by the TrigEgammaD3PDObjects function.
# The formalist is the same as for all the other D3PDMaker function.
#
# @param level The overall detail level of the D3PDObject
# @param name  An object name to refer to this object later on
# @param kwin  Input keyworded parameters
# @param kw    Extra keywords provided here
# @returns The list of parameters to give to the D3PDObject
#
def _args( level, name, kwin, **kw ):
    result = kw.copy()
    result[ "level" ] = level
    for ( k, v ) in kwin.items():
        if k.startswith( name + "_" ):
            result[ k[ len( name ) + 1 : ] ] = v
            pass
        pass
    return result

## Function adding the full jet trigger information to a D3PD
#
# This function can be used to add jet trigger information from all the trigger levels
# to a D3PD. By default the function adds the jet trigger information with the
# highest detail level. This can be modified by changing the "level" parameter to a
# lower value, and/or turning off the "addNaviInfo" and "addConfigInfo" parameters.
#
def TrigJetD3PDObjects( alg, level = 10, addNaviInfo = True, addConfigInfo = True, **kw ):

    #
    # Configure the navigation flattening if it's requested:
    #
    if addNaviInfo:
        alg += InitTrigD3PDObject( level )

    #
    # Add the LVL1 information if it's not added yet:
    #
    if not hasattr( alg, "trig_L1_jet_Filler" ):
        from TriggerD3PDMaker.JetROID3PDObject import JetROID3PDObject
        alg += JetROID3PDObject( **_args( level, "JetRoI", kw ) )
        pass

    #
    # Add the LVL2 information:
    #
    from TrigJetD3PDMaker.TrigJetD3PDObject import TrigJetD3PDObject
    alg += TrigJetD3PDObject( **_args( level, "L2Jet", kw ) )

    #
    # Add the EF information:
    #
    from TrigJetD3PDMaker.EFJetD3PDObject import EFJetD3PDObject
    alg += EFJetD3PDObject( **_args( level, "EFJet", kw ) )

    #
    # Trigger navigation info:
    #
    if addNaviInfo:
        from TriggerD3PDMaker.TrigNaviD3PDObject import \
             ChainEntryD3PDObject, CombLinksD3PDObjects
        if not hasattr( alg, "trig_Nav_Filler" ):
            alg += ChainEntryD3PDObject( **_args( level, "ChainEntry", kw ) )
            pass
        alg += CombLinksD3PDObjects[ 'L2_j' ]( **_args( level, "CombLinksL2", kw ) )
        alg += CombLinksD3PDObjects[ 'EF_j' ]( **_args( level, "CombLinksEF", kw ) )
        pass

    # Add the trigger configuration metadata:
    if addConfigInfo:
        from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
        addTrigConfMetadata( alg )

    return alg

def TrigJetD3PD( fname, level=10, tname='trigjet', seq = AlgSequence() ):

    import D3PDMakerCoreComps
    alg = D3PDMakerCoreComps.MakerAlg( tname, seq = seq, file = fname )
    TrigJetD3PDObjects( alg, level )
    return alg
