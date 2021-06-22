# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def getPhysicsVariablePlotsCfg():

    from AthenaCommon.Constants import INFO
    from FlavourTaggingTests.histoConfigurationWrapper import getHistoDefs

    variables1D,variables2D = getHistoDefs()

    print( '1D:::',len(variables1D) )
    for el in variables1D:
        print( el )
    print( '2D:::',len(variables2D) )
    for el in variables2D:
        print( el )



    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    output = ComponentAccumulator()

    options = {}
    options['name'] = 'PhysicsVariablePlotsAlg'
    options['OutputLevel'] = INFO
    options['JetCollectionKey'] = 'AntiKt4EMTopoJets'
    options['VertexCollectionKey'] = 'PrimaryVertices'
    options['TrackParticleCollectionKey'] = 'InDetTrackParticles'
    options['THistSvc'] = "THistSvc"
    options[ 'Histograms1D' ] = variables1D
    options[ 'Histograms2D' ] = variables2D

    from AthenaConfiguration.ComponentFactory import CompFactory
    FTAGValidation__myAlg = CompFactory.FTAGValidation.PhysicsVariablePlots( **options )
    output.addEventAlgo( FTAGValidation__myAlg )

    return output


