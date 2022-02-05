# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def getPhysicsTriggerVariablePlotsCfg():

    from AthenaCommon.Constants import INFO
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from FlavourTaggingTests.histoConfigurationWrapper import getHistoDefs

    # Chains we want to run over. To find out all available chains, run code ones and see printout in finalize step.
    bJetChains = ["HLT_j20_0eta290_020jvt_pf_ftf_boffperf_L1J15"]


    variables1D,variables2D = getHistoDefs( chains=bJetChains )

    print( '1D:::',len(variables1D) )
    for el in variables1D:
        print( el )
    print( '2D:::',len(variables2D) )
    for el in variables2D:
        print( el )

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    output = ComponentAccumulator()

    from AthenaMonitoring.TriggerInterface import TrigDecisionToolCfg
    output.merge(TrigDecisionToolCfg(ConfigFlags))

    options = {}
    options['name'] = 'PhysicsTriggerVariablePlotsAlg'
    options['OutputLevel'] = INFO
    options['TrigVertexCollectionKey'] = 'HLT_IDVertex_FS'
    options['TrigTrackParticleCollectionKey'] = 'HLT_IDTrack_Bjet_IDTrig'
    options['OfflineJetCollectionKey'] = 'AntiKt4EMPFlowJets'
    options['TrigDecisionTool'] = output.getPublicTool("TrigDecisionTool")
    options['bJetChains'] = bJetChains
    options['THistSvc'] = "THistSvc"
    options[ 'Histograms1D' ] = variables1D
    options[ 'Histograms2D' ] = variables2D

    from AthenaConfiguration.ComponentFactory import CompFactory
    FTAGValidation__myAlg = CompFactory.FTAGValidation.PhysicsTriggerVariablePlots( **options )
    output.addEventAlgo( FTAGValidation__myAlg )

    return output


