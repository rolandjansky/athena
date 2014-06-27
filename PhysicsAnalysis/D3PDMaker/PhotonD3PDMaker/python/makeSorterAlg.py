# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
import PhotonD3PDMaker

def makeElectronSorterAlg( InputCollection,
                           CollectionType,
                           OutputCollectionName) :

    newGetter = D3PDMakerCoreComps.SGDataVectorGetterTool(CollectionType+'_sorted',
                                                          TypeName = CollectionType,
                                                          SGKey = InputCollection);
    
    sorter = PhotonD3PDMaker.ElectronSorterAlg()
    sorter.Getter               = newGetter
    sorter.OutputKey            = OutputCollectionName

    return sorter

def makePhotonSorterAlg( InputCollection,
                         CollectionType,
                         OutputCollectionName) :

    newGetter = D3PDMakerCoreComps.SGDataVectorGetterTool(CollectionType+'_sorted',
                                                          TypeName = CollectionType,
                                                          SGKey = InputCollection);
    
    sorter                      = PhotonD3PDMaker.PhotonSorterAlg()
    sorter.Getter               = newGetter
    sorter.OutputKey            = OutputCollectionName

    return sorter

def makeTruthParticleSorterAlg( InputCollection,
                                CollectionType,
                                OutputCollectionName) :

    newGetter = D3PDMakerCoreComps.SGDataVectorGetterTool(CollectionType+'_sorted',
                                                          TypeName = CollectionType,
                                                          SGKey = InputCollection);
    
    sorter                      = PhotonD3PDMaker.TruthParticleSorterAlg()
    sorter.Getter               = newGetter
    sorter.OutputKey            = OutputCollectionName

    return sorter

