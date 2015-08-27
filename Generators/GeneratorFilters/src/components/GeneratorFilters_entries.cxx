#include "GaudiKernel/DeclareFactoryEntries.h"

#include "GeneratorFilters/ElectronFilter.h"
#include "GeneratorFilters/LeptonFilter.h"
#include "GeneratorFilters/ZtoLeptonFilter.h"
#include "GeneratorFilters/MultiLeptonFilter.h"
#include "GeneratorFilters/MultiMuonFilter.h"
#include "GeneratorFilters/BSignalFilter.h"
#include "GeneratorFilters/ATauFilter.h"
#include "GeneratorFilters/JetFilter.h"
#include "GeneratorFilters/JetForwardFilter.h"
#include "GeneratorFilters/GapJetFilter.h"
#include "GeneratorFilters/ParticleFilter.h"
#include "GeneratorFilters/PhotonFilter.h"
#include "GeneratorFilters/MissingEtFilter.h"
#include "GeneratorFilters/TauFilter.h"
#include "GeneratorFilters/TTbarFilter.h"
#include "GeneratorFilters/TTbarPhotonWhizardHwgFilter.h"
#include "GeneratorFilters/TTbarWToLeptonFilter.h"
#include "GeneratorFilters/TTbarMassFilter.h"
#include "GeneratorFilters/TruthJetFilter.h"
#include "GeneratorFilters/SoftLeptonFilter.h"
#include "GeneratorFilters/ParentChildFilter.h"
#include "GeneratorFilters/ParentTwoChildrenFilter.h"
#include "GeneratorFilters/ParentChildwStatusFilter.h"
#include "GeneratorFilters/SoftLeptonInJetFilter.h"
#include "GeneratorFilters/VBFForwardJetsFilter.h"
#include "GeneratorFilters/METFilter.h"
#include "GeneratorFilters/TTbarPlusJetsFilter.h"
#include "GeneratorFilters/WMultiLeptonFilter.h"
#include "GeneratorFilters/PtmissAndOrLeptonFilter.h"
#include "GeneratorFilters/MuonFilter.h"
#include "GeneratorFilters/WZtoLeptonFilter.h"
#include "GeneratorFilters/AsymJetFilter.h"
#include "GeneratorFilters/MultiObjectsFilter.h"
#include "GeneratorFilters/MassRangeFilter.h"
#include "GeneratorFilters/DecayLengthFilter.h"
#include "GeneratorFilters/MultiElecMuTauFilter.h"
#include "GeneratorFilters/WeightedBDtoElectronFilter.h"
#include "GeneratorFilters/JetIntervalFilter.h"
#include "GeneratorFilters/XXvvGGFilter.h"
#include "GeneratorFilters/TruthJetWeightFilter.h"
#include "GeneratorFilters/MultiElectronFilter.h"
#include "GeneratorFilters/DiPhotonFilter.h"
#include "GeneratorFilters/ChargedTracksFilter.h"
#include "GeneratorFilters/DecayModeFilter.h"
#include "GeneratorFilters/DiLeptonMassFilter.h"
#include "GeneratorFilters/DirectPhotonFilter.h"
#include "GeneratorFilters/HeavyFlavorHadronFilter.h"
#include "GeneratorFilters/DstD0K3piFilter.h"
#include "GeneratorFilters/FourLeptonMassFilter.h"
#include "GeneratorFilters/FourLeptonInvMassFilter.h"
#include "GeneratorFilters/HtoVVFilter.h"
#include "GeneratorFilters/QCDTruthJetFilter.h"
#include "GeneratorFilters/QCDTruthMultiJetFilter.h"  
#include "GeneratorFilters/VBFHbbEtaSortingFilter.h"  
#include "GeneratorFilters/TopCKMFilter.h"
#include "GeneratorFilters/ForwardProtonFilter.h"
#include "GeneratorFilters/BSubstruct.h"
#include "GeneratorFilters/TopCKMKinFilter.h"
#include "GeneratorFilters/MultiHiggsFilter.h"
#include "GeneratorFilters/XtoVVDecayFilter.h"
#include "GeneratorFilters/XtoVVDecayFilterExtended.h"
#include "GeneratorFilters/VHtoVVFilter.h"
#include "GeneratorFilters/TtHtoVVDecayFilter.h"
#include "GeneratorFilters/SusySubprocessFinder.h"
#include "GeneratorFilters/LeadingPhotonFilter.h"
#include "GeneratorFilters/VHtoVVDiLepFilter.h"
#include "GeneratorFilters/VBFMjjIntervalFilter.h"
#include "GeneratorFilters/JetFilterWithTruthPhoton.h"
#include "GeneratorFilters/Boosted2DijetFilter.h"
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

DECLARE_ALGORITHM_FACTORY(ElectronFilter)
DECLARE_ALGORITHM_FACTORY(LeptonFilter)
DECLARE_ALGORITHM_FACTORY(ZtoLeptonFilter)
DECLARE_ALGORITHM_FACTORY(MultiLeptonFilter)
DECLARE_ALGORITHM_FACTORY(MultiMuonFilter)
DECLARE_ALGORITHM_FACTORY(BSignalFilter)
DECLARE_ALGORITHM_FACTORY(ATauFilter)
DECLARE_ALGORITHM_FACTORY(JetFilter)
DECLARE_ALGORITHM_FACTORY(JetForwardFilter)
DECLARE_ALGORITHM_FACTORY(GapJetFilter)
DECLARE_ALGORITHM_FACTORY(ParticleFilter)
DECLARE_ALGORITHM_FACTORY(PhotonFilter)
DECLARE_ALGORITHM_FACTORY(MissingEtFilter)
DECLARE_ALGORITHM_FACTORY(TauFilter)
DECLARE_ALGORITHM_FACTORY(TTbarFilter)
DECLARE_ALGORITHM_FACTORY(TTbarPhotonWhizardHwgFilter)
DECLARE_ALGORITHM_FACTORY(TTbarWToLeptonFilter)
DECLARE_ALGORITHM_FACTORY(TTbarMassFilter)
DECLARE_ALGORITHM_FACTORY(TruthJetFilter)
DECLARE_ALGORITHM_FACTORY(SoftLeptonFilter)
DECLARE_ALGORITHM_FACTORY(ParentChildFilter)
DECLARE_ALGORITHM_FACTORY(ParentTwoChildrenFilter)
DECLARE_ALGORITHM_FACTORY(ParentChildwStatusFilter)
DECLARE_ALGORITHM_FACTORY(SoftLeptonInJetFilter)
DECLARE_ALGORITHM_FACTORY(VBFForwardJetsFilter)
DECLARE_ALGORITHM_FACTORY(METFilter)
DECLARE_ALGORITHM_FACTORY(TTbarPlusJetsFilter)
DECLARE_ALGORITHM_FACTORY(WMultiLeptonFilter)
DECLARE_ALGORITHM_FACTORY(PtmissAndOrLeptonFilter)
DECLARE_ALGORITHM_FACTORY(MuonFilter)
DECLARE_ALGORITHM_FACTORY(WZtoLeptonFilter)
DECLARE_ALGORITHM_FACTORY(AsymJetFilter)
DECLARE_ALGORITHM_FACTORY(MultiObjectsFilter)
DECLARE_ALGORITHM_FACTORY(MassRangeFilter)
DECLARE_ALGORITHM_FACTORY(DecayLengthFilter)
DECLARE_ALGORITHM_FACTORY(MultiElecMuTauFilter)
DECLARE_ALGORITHM_FACTORY(WeightedBDtoElectronFilter)
DECLARE_ALGORITHM_FACTORY(JetIntervalFilter)
DECLARE_ALGORITHM_FACTORY(XXvvGGFilter)
DECLARE_ALGORITHM_FACTORY(TruthJetWeightFilter)
DECLARE_ALGORITHM_FACTORY(MultiElectronFilter)
DECLARE_ALGORITHM_FACTORY(DiPhotonFilter)
DECLARE_ALGORITHM_FACTORY(ChargedTracksFilter)
DECLARE_ALGORITHM_FACTORY(DecayModeFilter)
DECLARE_ALGORITHM_FACTORY(DiLeptonMassFilter)
DECLARE_ALGORITHM_FACTORY(DirectPhotonFilter)
DECLARE_ALGORITHM_FACTORY(HeavyFlavorHadronFilter)
DECLARE_ALGORITHM_FACTORY(DstD0K3piFilter)
DECLARE_ALGORITHM_FACTORY(FourLeptonMassFilter)
DECLARE_ALGORITHM_FACTORY(FourLeptonInvMassFilter)
DECLARE_ALGORITHM_FACTORY(HtoVVFilter)
DECLARE_ALGORITHM_FACTORY(QCDTruthJetFilter)
DECLARE_ALGORITHM_FACTORY(QCDTruthMultiJetFilter)  
DECLARE_ALGORITHM_FACTORY(VBFHbbEtaSortingFilter)  
DECLARE_ALGORITHM_FACTORY(TopCKMFilter)
DECLARE_ALGORITHM_FACTORY(ForwardProtonFilter)
DECLARE_ALGORITHM_FACTORY(BSubstruct)
DECLARE_ALGORITHM_FACTORY(TopCKMKinFilter)
DECLARE_ALGORITHM_FACTORY(MultiHiggsFilter)
DECLARE_ALGORITHM_FACTORY(XtoVVDecayFilter)
DECLARE_ALGORITHM_FACTORY(XtoVVDecayFilterExtended)
DECLARE_ALGORITHM_FACTORY(VHtoVVFilter)
DECLARE_ALGORITHM_FACTORY(TtHtoVVDecayFilter)
DECLARE_ALGORITHM_FACTORY(SusySubprocessFinder)
DECLARE_ALGORITHM_FACTORY(LeadingPhotonFilter)
DECLARE_ALGORITHM_FACTORY(VHtoVVDiLepFilter)
DECLARE_ALGORITHM_FACTORY(VBFMjjIntervalFilter)
DECLARE_ALGORITHM_FACTORY(JetFilterWithTruthPhoton)
DECLARE_ALGORITHM_FACTORY(Boosted2DijetFilter)
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

DECLARE_FACTORY_ENTRIES(GeneratorFilters) {
    DECLARE_ALGORITHM(LeptonFilter)
    DECLARE_ALGORITHM(ZtoLeptonFilter)
    DECLARE_ALGORITHM(BSignalFilter)
    DECLARE_ALGORITHM(MultiLeptonFilter)
    DECLARE_ALGORITHM(MultiMuonFilter)
    DECLARE_ALGORITHM(ATauFilter)
    DECLARE_ALGORITHM(JetFilter)
    DECLARE_ALGORITHM(JetForwardFilter)
    DECLARE_ALGORITHM(GapJetFilter)
    DECLARE_ALGORITHM(ParticleFilter)
    DECLARE_ALGORITHM(PhotonFilter)
    DECLARE_ALGORITHM(MissingEtFilter)
    DECLARE_ALGORITHM(TauFilter)
    DECLARE_ALGORITHM(TTbarFilter)
    DECLARE_ALGORITHM(TTbarPhotonWhizardHwgFilter)
    DECLARE_ALGORITHM(TTbarWToLeptonFilter)
    DECLARE_ALGORITHM(TTbarMassFilter)
    DECLARE_ALGORITHM(TruthJetFilter)
    DECLARE_ALGORITHM(SoftLeptonFilter)
    DECLARE_ALGORITHM(ParentChildFilter)
    DECLARE_ALGORITHM(ParentTwoChildrenFilter)
    DECLARE_ALGORITHM(ParentChildwStatusFilter)
    DECLARE_ALGORITHM(SoftLeptonInJetFilter)
    DECLARE_ALGORITHM(VBFForwardJetsFilter)
    DECLARE_ALGORITHM(METFilter)
    DECLARE_ALGORITHM(TTbarPlusJetsFilter)
    DECLARE_ALGORITHM(WMultiLeptonFilter)
    DECLARE_ALGORITHM(PtmissAndOrLeptonFilter)
    DECLARE_ALGORITHM(MuonFilter)
    DECLARE_ALGORITHM(WZtoLeptonFilter)
    DECLARE_ALGORITHM(AsymJetFilter)
    DECLARE_ALGORITHM(MultiObjectsFilter)
    DECLARE_ALGORITHM(MassRangeFilter)
    DECLARE_ALGORITHM(DecayLengthFilter)
    DECLARE_ALGORITHM(MultiElecMuTauFilter)
    DECLARE_ALGORITHM(WeightedBDtoElectronFilter)
    DECLARE_ALGORITHM(JetIntervalFilter)
    DECLARE_ALGORITHM(XXvvGGFilter)
    DECLARE_ALGORITHM(TruthJetWeightFilter)
    DECLARE_ALGORITHM(MultiElectronFilter)
    DECLARE_ALGORITHM(DiPhotonFilter)
    DECLARE_ALGORITHM(ChargedTracksFilter)
    DECLARE_ALGORITHM(DecayModeFilter)
    DECLARE_ALGORITHM(DiLeptonMassFilter)
    DECLARE_ALGORITHM(DirectPhotonFilter)
    DECLARE_ALGORITHM(HeavyFlavorHadronFilter)
    DECLARE_ALGORITHM(DstD0K3piFilter)
    DECLARE_ALGORITHM(FourLeptonMassFilter)
    DECLARE_ALGORITHM(FourLeptonInvMassFilter)
    DECLARE_ALGORITHM(HtoVVFilter)
    DECLARE_ALGORITHM(QCDTruthJetFilter)
    DECLARE_ALGORITHM(QCDTruthMultiJetFilter)  
    DECLARE_ALGORITHM(VBFHbbEtaSortingFilter)  
    DECLARE_ALGORITHM(TopCKMFilter)
    DECLARE_ALGORITHM(ForwardProtonFilter )
    DECLARE_ALGORITHM(BSubstruct)
    DECLARE_ALGORITHM(TopCKMKinFilter)
    DECLARE_ALGORITHM(MultiHiggsFilter)
    DECLARE_ALGORITHM(XtoVVDecayFilter)
    DECLARE_ALGORITHM(XtoVVDecayFilterExtended)
    DECLARE_ALGORITHM(VHtoVVFilter)
    DECLARE_ALGORITHM(TtHtoVVDecayFilter)
    DECLARE_ALGORITHM(SusySubprocessFinder)
    DECLARE_ALGORITHM(LeadingPhotonFilter)
    DECLARE_ALGORITHM(VHtoVVDiLepFilter)
    DECLARE_ALGORITHM(VBFMjjIntervalFilter)
    DECLARE_ALGORITHM(JetFilterWithTruthPhoton)
    DECLARE_ALGORITHM(Boosted2DijetFilter)
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
}
