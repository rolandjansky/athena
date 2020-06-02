# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm

def makeSequence (dataType) :
    algSeq = AlgSequence()

    # Create the algorithm's configuration. Note that we'll be able to add
    # algorithm property settings here later on.
    alg = createAlgorithm( 'CP::SysListLoaderAlg', 'SysLoaderAlg' )
    alg.sigmaRecommended = 1
    algSeq += alg

    # Include, and then set up the pileup analysis sequence:
    from AsgAnalysisAlgorithms.PileupAnalysisSequence import \
        makePileupAnalysisSequence
    pileupSequence = makePileupAnalysisSequence( dataType )
    pileupSequence.configure( inputName = 'EventInfo', outputName = 'EventInfo_%SYS%' )
    algSeq += pileupSequence

    # Include, and then set up the electron analysis sequence:
    from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import \
        makeElectronAnalysisSequence
    electronSequence = makeElectronAnalysisSequence( dataType, 'LooseLHElectron.GradientLoose', postfix = 'loose',
                                                     recomputeLikelihood=True, enableCutflow=True, enableKinematicHistograms=True )
    electronSequence.configure( inputName = 'Electrons',
                                outputName = 'AnalysisElectrons_%SYS%' )
    algSeq += electronSequence

    # Include, and then set up the photon analysis sequence:
    from EgammaAnalysisAlgorithms.PhotonAnalysisSequence import \
        makePhotonAnalysisSequence
    photonSequence = makePhotonAnalysisSequence( dataType, 'Tight.FixedCutTight', postfix = 'tight',
                                                 recomputeIsEM=True, enableCutflow=True, enableKinematicHistograms=True )
    photonSequence.configure( inputName = 'Photons',
                              outputName = 'AnalysisPhotons_%SYS%' )
    algSeq += photonSequence

    return algSeq
