# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createService
from AsgAnalysisAlgorithms.AsgAnalysisAlgorithmsTest import pileupConfigFiles

def makeSequence (dataType, likelihood=True) :
    algSeq = AlgSequence()

    # Set up the systematics loader/handler service:
    sysService = createService( 'CP::SystematicsSvc', 'SystematicsSvc', sequence = algSeq )
    sysService.sigmaRecommended = 1

    prwfiles, lumicalcfiles = pileupConfigFiles(dataType)

    # Include, and then set up the pileup analysis sequence:
    from AsgAnalysisAlgorithms.PileupAnalysisSequence import \
        makePileupAnalysisSequence
    pileupSequence = makePileupAnalysisSequence(
        dataType,
        userPileupConfigs=prwfiles,
        userLumicalcFiles=lumicalcfiles,
    )
    pileupSequence.configure( inputName = {}, outputName = {} )
    algSeq += pileupSequence

    # Include, and then set up the electron analysis sequence:
    from EgammaAnalysisAlgorithms.ElectronAnalysisSequence import \
        makeElectronAnalysisSequence
    if likelihood:
        workingpoint = 'LooseLHElectron.Loose_VarRad'
    else:
        workingpoint = 'LooseDNNElectron.Loose_VarRad'
    electronSequence = makeElectronAnalysisSequence( dataType, workingpoint, postfix = 'loose',
                                                     recomputeLikelihood=True, enableCutflow=True, enableKinematicHistograms=True )
    electronSequence.configure( inputName = 'Electrons',
                                outputName = 'AnalysisElectrons_%SYS%' )
    algSeq += electronSequence

    # Include, and then set up the photon analysis sequence:
    from EgammaAnalysisAlgorithms.PhotonAnalysisSequence import \
        makePhotonAnalysisSequence
    photonSequence = makePhotonAnalysisSequence( dataType, 'Tight.FixedCutTight', postfix = 'tight',
                                                 recomputeIsEM=False, enableCutflow=True, enableKinematicHistograms=True )
    photonSequence.configure( inputName = 'Photons',
                              outputName = 'AnalysisPhotons_%SYS%' )
    algSeq += photonSequence

    return algSeq
