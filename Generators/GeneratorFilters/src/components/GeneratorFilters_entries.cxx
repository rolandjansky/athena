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

DECLARE_COMPONENT( ElectronFilter )
DECLARE_COMPONENT( LeptonFilter )
DECLARE_COMPONENT( ZtoLeptonFilter )
DECLARE_COMPONENT( MultiLeptonFilter )
DECLARE_COMPONENT( MultiMuonFilter )
DECLARE_COMPONENT( BSignalFilter )
DECLARE_COMPONENT( ATauFilter )
DECLARE_COMPONENT( JetFilter )
DECLARE_COMPONENT( JetForwardFilter )
DECLARE_COMPONENT( GapJetFilter )
DECLARE_COMPONENT( ParticleFilter )
DECLARE_COMPONENT( PhotonFilter )
DECLARE_COMPONENT( MissingEtFilter )
DECLARE_COMPONENT( TauFilter )
DECLARE_COMPONENT( TTbarFilter )
DECLARE_COMPONENT( TTbarPhotonWhizardHwgFilter )
DECLARE_COMPONENT( TTbarWToLeptonFilter )
DECLARE_COMPONENT( TTbarMassFilter )
DECLARE_COMPONENT( TruthJetFilter )
DECLARE_COMPONENT( SoftLeptonFilter )
DECLARE_COMPONENT( ParentChildFilter )
DECLARE_COMPONENT( ParentTwoChildrenFilter )
DECLARE_COMPONENT( ParentChildwStatusFilter )
DECLARE_COMPONENT( SoftLeptonInJetFilter )
DECLARE_COMPONENT( VBFForwardJetsFilter )
DECLARE_COMPONENT( METFilter )
DECLARE_COMPONENT( TTbarPlusJetsFilter )
DECLARE_COMPONENT( WMultiLeptonFilter )
DECLARE_COMPONENT( PtmissAndOrLeptonFilter )
DECLARE_COMPONENT( MuonFilter )
DECLARE_COMPONENT( WZtoLeptonFilter )
DECLARE_COMPONENT( AsymJetFilter )
DECLARE_COMPONENT( MultiObjectsFilter )
DECLARE_COMPONENT( MassRangeFilter )
DECLARE_COMPONENT( DecayLengthFilter )
DECLARE_COMPONENT( MultiElecMuTauFilter )
DECLARE_COMPONENT( WeightedBDtoElectronFilter )
DECLARE_COMPONENT( JetIntervalFilter )
DECLARE_COMPONENT( XXvvGGFilter )
DECLARE_COMPONENT( TruthJetWeightFilter )
DECLARE_COMPONENT( MultiElectronFilter )
DECLARE_COMPONENT( DiPhotonFilter )
DECLARE_COMPONENT( ChargedTracksFilter )
DECLARE_COMPONENT( DecayModeFilter )
DECLARE_COMPONENT( DiLeptonMassFilter )
DECLARE_COMPONENT( DirectPhotonFilter )
DECLARE_COMPONENT( HeavyFlavorHadronFilter )
DECLARE_COMPONENT( DstD0K3piFilter )
DECLARE_COMPONENT( FourLeptonMassFilter )
DECLARE_COMPONENT( FourLeptonInvMassFilter )
DECLARE_COMPONENT( HtoVVFilter )
DECLARE_COMPONENT( QCDTruthJetFilter )
DECLARE_COMPONENT( QCDTruthMultiJetFilter )  
DECLARE_COMPONENT( VBFHbbEtaSortingFilter )  
DECLARE_COMPONENT( TopCKMFilter )
DECLARE_COMPONENT( ForwardProtonFilter )
DECLARE_COMPONENT( BSubstruct )
DECLARE_COMPONENT( TopCKMKinFilter )
DECLARE_COMPONENT( MultiHiggsFilter )
DECLARE_COMPONENT( XtoVVDecayFilter )
DECLARE_COMPONENT( XtoVVDecayFilterExtended )
DECLARE_COMPONENT( VHtoVVFilter )
DECLARE_COMPONENT( TtHtoVVDecayFilter )
DECLARE_COMPONENT( SusySubprocessFinder )
DECLARE_COMPONENT( LeadingPhotonFilter )
DECLARE_COMPONENT( VHtoVVDiLepFilter )
DECLARE_COMPONENT( VBFMjjIntervalFilter )
DECLARE_COMPONENT( JetFilterWithTruthPhoton )
DECLARE_COMPONENT( Boosted2DijetFilter )
DECLARE_COMPONENT( DiBjetFilter )
DECLARE_COMPONENT( HiggsFilter )
DECLARE_COMPONENT( TTbarBoostCatFilter )
DECLARE_COMPONENT( MultiParticleFilter )
DECLARE_COMPONENT( LeptonPairFilter )
DECLARE_COMPONENT( DecayPositionFilter )
DECLARE_COMPONENT( HTFilter )
DECLARE_COMPONENT( TTbarPlusHeavyFlavorFilter )
DECLARE_COMPONENT( DuplicateEventFilter )
DECLARE_COMPONENT( BoostedHadTopAndTopPair )
DECLARE_COMPONENT( DecaysFinalStateFilter )
DECLARE_COMPONENT( HTFilter )
DECLARE_COMPONENT( MissingEtFilter )
DECLARE_COMPONENT( TrimuMassRangeFilter )
