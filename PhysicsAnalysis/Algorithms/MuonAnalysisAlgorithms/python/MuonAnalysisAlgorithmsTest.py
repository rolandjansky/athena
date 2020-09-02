# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack

from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm

def makeSequence (dataType) :

    algSeq = AlgSequence()

    # Set up the systematics loader/handler algorithm:
    sysLoader = createAlgorithm( 'CP::SysListLoaderAlg', 'SysLoaderAlg' )
    sysLoader.sigmaRecommended = 1
    algSeq += sysLoader


    # Include, and then set up the pileup analysis sequence:
    from AsgAnalysisAlgorithms.PileupAnalysisSequence import \
        makePileupAnalysisSequence
    pileupSequence = makePileupAnalysisSequence( dataType )
    pileupSequence.configure( inputName = 'EventInfo', outputName = 'EventInfo_%SYS%' )

    # Add the pileup sequence to the job:
    algSeq += pileupSequence

    # Include, and then set up the muon analysis algorithm sequence:
    from MuonAnalysisAlgorithms.MuonAnalysisSequence import makeMuonAnalysisSequence
    muonSequenceMedium = makeMuonAnalysisSequence( dataType, deepCopyOutput = True, shallowViewOutput = False,
                                                   workingPoint = 'Medium.Iso', postfix = 'medium',
                                                   enableCutflow=True, enableKinematicHistograms=True )
    muonSequenceMedium.configure( inputName = 'Muons',
                                  outputName = 'AnalysisMuonsMedium_%SYS%' )

    # Add the sequence to the job:
    algSeq += muonSequenceMedium

    muonSequenceTight = makeMuonAnalysisSequence( dataType, deepCopyOutput = True, shallowViewOutput = False,
                                                  workingPoint = 'Tight.Iso', postfix = 'tight',
                                                  enableCutflow=True, enableKinematicHistograms=True )
    muonSequenceTight.removeStage ("calibration")
    muonSequenceTight.configure( inputName = 'AnalysisMuonsMedium_%SYS%',
                                 outputName = 'AnalysisMuons_%SYS%',
                                 affectingSystematics = muonSequenceMedium.affectingSystematics())

    # Add the sequence to the job:
    algSeq += muonSequenceTight

    # Add an ntuple dumper algorithm:
    treeMaker = createAlgorithm( 'CP::TreeMakerAlg', 'TreeMaker' )
    treeMaker.TreeName = 'muons'
    algSeq += treeMaker
    ntupleMaker = createAlgorithm( 'CP::AsgxAODNTupleMakerAlg', 'NTupleMakerEventInfo' )
    ntupleMaker.TreeName = 'muons'
    ntupleMaker.Branches = [ 'EventInfo.runNumber     -> runNumber',
                             'EventInfo.eventNumber   -> eventNumber', ]
    ntupleMaker.systematicsRegex = '(^$)'
    algSeq += ntupleMaker
    ntupleMaker = createAlgorithm( 'CP::AsgxAODNTupleMakerAlg', 'NTupleMakerMuons' )
    ntupleMaker.TreeName = 'muons'
    ntupleMaker.Branches = [ 'AnalysisMuons_NOSYS.eta -> mu_eta',
                             'AnalysisMuons_NOSYS.phi -> mu_phi',
                             'AnalysisMuons_%SYS%.pt  -> mu_%SYS%_pt', ]
    ntupleMaker.systematicsRegex = '(^MUON_.*)'
    algSeq += ntupleMaker
    treeFiller = createAlgorithm( 'CP::TreeFillerAlg', 'TreeFiller' )
    treeFiller.TreeName = 'muons'
    algSeq += treeFiller

    return algSeq
