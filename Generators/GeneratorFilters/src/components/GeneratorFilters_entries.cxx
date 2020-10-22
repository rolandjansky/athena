/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "GeneratorFilters/ElectronFilter.h"
#include "GeneratorFilters/LeptonFilter.h"
#include "GeneratorFilters/ZtoLeptonFilter.h"
#include "GeneratorFilters/ZHWWLeptonPairMassFilter.h"
#include "GeneratorFilters/MultiLeptonFilter.h"
#include "GeneratorFilters/MultiLeptonWithParentFilter.h"
#include "GeneratorFilters/MultiMuonFilter.h"
#include "GeneratorFilters/BSignalFilter.h"
#include "GeneratorFilters/JetFilter.h"
#include "GeneratorFilters/ParticleFilter.h"
#include "GeneratorFilters/PhotonFilter.h"
#include "GeneratorFilters/MissingEtFilter.h"
#include "GeneratorFilters/TauFilter.h"
#include "GeneratorFilters/TTbarWToLeptonFilter.h"
#include "GeneratorFilters/TTbarMassFilter.h"
#include "GeneratorFilters/TruthJetFilter.h"
#include "GeneratorFilters/ParentChildFilter.h"
#include "GeneratorFilters/ParentTwoChildrenFilter.h"
#include "GeneratorFilters/ParentChildwStatusFilter.h"
#include "GeneratorFilters/VBFForwardJetsFilter.h"
#include "GeneratorFilters/METFilter.h"
#include "GeneratorFilters/MuonFilter.h"
#include "GeneratorFilters/WZtoLeptonFilter.h"
#include "GeneratorFilters/MassRangeFilter.h"
#include "GeneratorFilters/DecayLengthFilter.h"
#include "GeneratorFilters/MultiElecMuTauFilter.h"
#include "GeneratorFilters/TruthJetWeightFilter.h"
#include "GeneratorFilters/MultiElectronFilter.h"
#include "GeneratorFilters/DiPhotonFilter.h"
#include "GeneratorFilters/ChargedTracksFilter.h"
#include "GeneratorFilters/DecayModeFilter.h"
#include "GeneratorFilters/DiLeptonMassFilter.h"
#include "GeneratorFilters/DirectPhotonFilter.h"
#include "GeneratorFilters/SUSYGGMHiggsinoPhotonFilter.h"
#include "GeneratorFilters/HeavyFlavorHadronFilter.h"
#include "GeneratorFilters/FourLeptonMassFilter.h"
#include "GeneratorFilters/FourLeptonInvMassFilter.h"
#include "GeneratorFilters/FourTauLepLepHadHadFilter.h"
#include "GeneratorFilters/QCDTruthJetFilter.h"
#include "GeneratorFilters/QCDTruthMultiJetFilter.h"  
#include "GeneratorFilters/VBFHbbEtaSortingFilter.h"  
#include "GeneratorFilters/ForwardProtonFilter.h"
#include "GeneratorFilters/MultiHiggsFilter.h"
#include "GeneratorFilters/XtoVVDecayFilter.h"
#include "GeneratorFilters/XtoVVDecayFilterExtended.h"
#include "GeneratorFilters/VHtoVVFilter.h"
#include "GeneratorFilters/TtHtoVVDecayFilter.h"
#include "GeneratorFilters/VBFMjjIntervalFilter.h"
#include "GeneratorFilters/DiBjetFilter.h"
#include "GeneratorFilters/LeadingDiBjetFilter.h"
#include "GeneratorFilters/HiggsFilter.h"
#include "GeneratorFilters/TTbarBoostCatFilter.h"
#include "GeneratorFilters/MultiParticleFilter.h" 
#include "GeneratorFilters/LeptonPairFilter.h"
#include "GeneratorFilters/DecayPositionFilter.h"
#include "GeneratorFilters/HTFilter.h"
#include "GeneratorFilters/TTbarPlusHeavyFlavorFilter.h"
#include "GeneratorFilters/DuplicateEventFilter.h"
#include "GeneratorFilters/BoostedHadTopAndTopPair.h"
#include "GeneratorFilters/DecaysFinalStateFilter.h"
#include "GeneratorFilters/HTFilter.h"
#include "GeneratorFilters/MissingEtFilter.h"
#include "GeneratorFilters/TrimuMassRangeFilter.h"
#include "GeneratorFilters/MultiBjetFilter.h"
#include "GeneratorFilters/ParentsTracksFilter.h"
#include "GeneratorFilters/SplitPhotonFilter.h"
#include "GeneratorFilters/TripletChainFilter.h"
#include "GeneratorFilters/TransverseMassVVFilter.h"
#include "GeneratorFilters/TTbarWithJpsimumuFilter.h"
#include "GeneratorFilters/TTbarLJetsVptFilter.h"
#include "GeneratorFilters/ALPDecayPositionFilter.h"

DECLARE_ALGORITHM_FACTORY(ElectronFilter)
DECLARE_ALGORITHM_FACTORY(LeptonFilter)
DECLARE_ALGORITHM_FACTORY(ZtoLeptonFilter)
DECLARE_ALGORITHM_FACTORY(ZHWWLeptonPairMassFilter)
DECLARE_ALGORITHM_FACTORY(MultiLeptonFilter)
DECLARE_ALGORITHM_FACTORY(MultiLeptonWithParentFilter)
DECLARE_ALGORITHM_FACTORY(MultiMuonFilter)
DECLARE_ALGORITHM_FACTORY(BSignalFilter)
DECLARE_ALGORITHM_FACTORY(JetFilter)
DECLARE_ALGORITHM_FACTORY(ParticleFilter)
DECLARE_ALGORITHM_FACTORY(PhotonFilter)
DECLARE_ALGORITHM_FACTORY(MissingEtFilter)
DECLARE_ALGORITHM_FACTORY(TauFilter)
DECLARE_ALGORITHM_FACTORY(TTbarWToLeptonFilter)
DECLARE_ALGORITHM_FACTORY(TTbarMassFilter)
DECLARE_ALGORITHM_FACTORY(TruthJetFilter)
DECLARE_ALGORITHM_FACTORY(ParentChildFilter)
DECLARE_ALGORITHM_FACTORY(ParentTwoChildrenFilter)
DECLARE_ALGORITHM_FACTORY(ParentChildwStatusFilter)
DECLARE_ALGORITHM_FACTORY(VBFForwardJetsFilter)
DECLARE_ALGORITHM_FACTORY(METFilter)
DECLARE_ALGORITHM_FACTORY(MuonFilter)
DECLARE_ALGORITHM_FACTORY(WZtoLeptonFilter)
DECLARE_ALGORITHM_FACTORY(MassRangeFilter)
DECLARE_ALGORITHM_FACTORY(DecayLengthFilter)
DECLARE_ALGORITHM_FACTORY(MultiElecMuTauFilter)
DECLARE_ALGORITHM_FACTORY(TruthJetWeightFilter)
DECLARE_ALGORITHM_FACTORY(MultiElectronFilter)
DECLARE_ALGORITHM_FACTORY(DiPhotonFilter)
DECLARE_ALGORITHM_FACTORY(ChargedTracksFilter)
DECLARE_ALGORITHM_FACTORY(DecayModeFilter)
DECLARE_ALGORITHM_FACTORY(DiLeptonMassFilter)
DECLARE_ALGORITHM_FACTORY(DirectPhotonFilter)
DECLARE_ALGORITHM_FACTORY(SUSYGGMHiggsinoPhotonFilter)
DECLARE_ALGORITHM_FACTORY(HeavyFlavorHadronFilter)
DECLARE_ALGORITHM_FACTORY(FourLeptonMassFilter)
DECLARE_ALGORITHM_FACTORY(FourLeptonInvMassFilter)
DECLARE_ALGORITHM_FACTORY(FourTauLepLepHadHadFilter)
DECLARE_ALGORITHM_FACTORY(QCDTruthJetFilter)
DECLARE_ALGORITHM_FACTORY(QCDTruthMultiJetFilter)  
DECLARE_ALGORITHM_FACTORY(VBFHbbEtaSortingFilter)  
DECLARE_ALGORITHM_FACTORY(ForwardProtonFilter)
DECLARE_ALGORITHM_FACTORY(MultiHiggsFilter)
DECLARE_ALGORITHM_FACTORY(XtoVVDecayFilter)
DECLARE_ALGORITHM_FACTORY(XtoVVDecayFilterExtended)
DECLARE_ALGORITHM_FACTORY(VHtoVVFilter)
DECLARE_ALGORITHM_FACTORY(TtHtoVVDecayFilter)
DECLARE_ALGORITHM_FACTORY(VBFMjjIntervalFilter)
DECLARE_ALGORITHM_FACTORY(DiBjetFilter)
DECLARE_ALGORITHM_FACTORY(LeadingDiBjetFilter)
DECLARE_ALGORITHM_FACTORY(HiggsFilter)
DECLARE_ALGORITHM_FACTORY(TTbarBoostCatFilter)
DECLARE_ALGORITHM_FACTORY(MultiParticleFilter)
DECLARE_ALGORITHM_FACTORY(LeptonPairFilter)
DECLARE_ALGORITHM_FACTORY(DecayPositionFilter)
DECLARE_ALGORITHM_FACTORY(HTFilter)
DECLARE_ALGORITHM_FACTORY(TTbarPlusHeavyFlavorFilter)
DECLARE_ALGORITHM_FACTORY(DuplicateEventFilter)
DECLARE_ALGORITHM_FACTORY(BoostedHadTopAndTopPair)
DECLARE_ALGORITHM_FACTORY(DecaysFinalStateFilter)
DECLARE_ALGORITHM_FACTORY(HTFilter)
DECLARE_ALGORITHM_FACTORY(MissingEtFilter)
DECLARE_ALGORITHM_FACTORY(TrimuMassRangeFilter)
DECLARE_ALGORITHM_FACTORY(MultiBjetFilter)
DECLARE_ALGORITHM_FACTORY(ParentsTracksFilter)
DECLARE_ALGORITHM_FACTORY(SplitPhotonFilter)
DECLARE_ALGORITHM_FACTORY(TripletChainFilter)
DECLARE_ALGORITHM_FACTORY(TransverseMassVVFilter)
DECLARE_ALGORITHM_FACTORY(TTbarWithJpsimumuFilter)
DECLARE_ALGORITHM_FACTORY(TTbarLJetsVptFilter)
DECLARE_ALGORITHM_FACTORY(ALPDecayPositionFilter)


DECLARE_FACTORY_ENTRIES(GeneratorFilters) {
    DECLARE_ALGORITHM(LeptonFilter)
    DECLARE_ALGORITHM(ZtoLeptonFilter)
    DECLARE_ALGORITHM(ZHWWLeptonPairMassFilter)
    DECLARE_ALGORITHM(BSignalFilter)
    DECLARE_ALGORITHM(MultiLeptonFilter)
    DECLARE_ALGORITHM(MultiLeptonWithParentFilter)
    DECLARE_ALGORITHM(MultiMuonFilter)
    DECLARE_ALGORITHM(JetFilter)
    DECLARE_ALGORITHM(ParticleFilter)
    DECLARE_ALGORITHM(PhotonFilter)
    DECLARE_ALGORITHM(MissingEtFilter)
    DECLARE_ALGORITHM(TauFilter)
    DECLARE_ALGORITHM(TTbarWToLeptonFilter)
    DECLARE_ALGORITHM(TTbarMassFilter)
    DECLARE_ALGORITHM(TruthJetFilter)
    DECLARE_ALGORITHM(ParentChildFilter)
    DECLARE_ALGORITHM(ParentTwoChildrenFilter)
    DECLARE_ALGORITHM(ParentChildwStatusFilter)
    DECLARE_ALGORITHM(VBFForwardJetsFilter)
    DECLARE_ALGORITHM(METFilter)
    DECLARE_ALGORITHM(MuonFilter)
    DECLARE_ALGORITHM(WZtoLeptonFilter)
    DECLARE_ALGORITHM(MassRangeFilter)
    DECLARE_ALGORITHM(DecayLengthFilter)
    DECLARE_ALGORITHM(MultiElecMuTauFilter)
    DECLARE_ALGORITHM(TruthJetWeightFilter)
    DECLARE_ALGORITHM(MultiElectronFilter)
    DECLARE_ALGORITHM(DiPhotonFilter)
    DECLARE_ALGORITHM(ChargedTracksFilter)
    DECLARE_ALGORITHM(DecayModeFilter)
    DECLARE_ALGORITHM(DiLeptonMassFilter)
    DECLARE_ALGORITHM(DirectPhotonFilter)
    DECLARE_ALGORITHM(SUSYGGMHiggsinoPhotonFilter)
    DECLARE_ALGORITHM(HeavyFlavorHadronFilter)
    DECLARE_ALGORITHM(FourLeptonMassFilter)
    DECLARE_ALGORITHM(FourLeptonInvMassFilter)
    DECLARE_ALGORITHM(FourTauLepLepHadHadFilter)
    DECLARE_ALGORITHM(QCDTruthJetFilter)
    DECLARE_ALGORITHM(QCDTruthMultiJetFilter)  
    DECLARE_ALGORITHM(VBFHbbEtaSortingFilter)  
    DECLARE_ALGORITHM(ForwardProtonFilter )
    DECLARE_ALGORITHM(MultiHiggsFilter)
    DECLARE_ALGORITHM(XtoVVDecayFilter)
    DECLARE_ALGORITHM(XtoVVDecayFilterExtended)
    DECLARE_ALGORITHM(VHtoVVFilter)
    DECLARE_ALGORITHM(TtHtoVVDecayFilter)
    DECLARE_ALGORITHM(VBFMjjIntervalFilter)
    DECLARE_ALGORITHM(DiBjetFilter)
    DECLARE_ALGORITHM(LeadingDiBjetFilter)
    DECLARE_ALGORITHM(HiggsFilter)
    DECLARE_ALGORITHM(TTbarBoostCatFilter)
    DECLARE_ALGORITHM(MultiParticleFilter) 
    DECLARE_ALGORITHM(LeptonPairFilter)
    DECLARE_ALGORITHM(DecayPositionFilter)
    DECLARE_ALGORITHM(HTFilter)
    DECLARE_ALGORITHM(TTbarPlusHeavyFlavorFilter)
    DECLARE_ALGORITHM(DuplicateEventFilter)
    DECLARE_ALGORITHM(BoostedHadTopAndTopPair)
    DECLARE_ALGORITHM(DecaysFinalStateFilter)
    DECLARE_ALGORITHM(HTFilter)
    DECLARE_ALGORITHM(MissingEtFilter)
    DECLARE_ALGORITHM(TrimuMassRangeFilter)
    DECLARE_ALGORITHM(MultiBjetFilter)
    DECLARE_ALGORITHM(ParentsTracksFilter)  
    DECLARE_ALGORITHM(SplitPhotonFilter)  
    DECLARE_ALGORITHM(TripletChainFilter)
    DECLARE_ALGORITHM(TransverseMassVVFilter)
    DECLARE_ALGORITHM(TTbarWithJpsimumuFilter)
    DECLARE_ALGORITHM(TTbarLJetsVptFilter)
    DECLARE_ALGORITHM(ALPDecayPositionFilter)

}

