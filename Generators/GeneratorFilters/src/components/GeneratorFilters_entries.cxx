// filters accepted for 22.6
#include "GeneratorFilters/QCDTruthJetFilter.h"
#include "GeneratorFilters/xAODBSignalFilter.h"
#include "GeneratorFilters/xAODChargedTracksFilter.h"
#include "GeneratorFilters/xAODChargedTracksWeightFilter.h"
#include "GeneratorFilters/xAODDecaysFinalStateFilter.h"
#include "GeneratorFilters/xAODDecayTimeFilter.h"
#include "GeneratorFilters/xAODDiLeptonMassFilter.h"
#include "GeneratorFilters/xAODDirectPhotonFilter.h"
#include "GeneratorFilters/xAODElectronFilter.h"
#include "GeneratorFilters/xAODLeptonFilter.h"
#include "GeneratorFilters/xAODLeptonPairFilter.h"
#include "GeneratorFilters/xAODM4MuIntervalFilter.h"
#include "GeneratorFilters/xAODMETFilter.h"
#include "GeneratorFilters/xAODMultiBjetFilter.h"
#include "GeneratorFilters/xAODMultiCjetFilter.h"
#include "GeneratorFilters/xAODMultiElecMuTauFilter.h"
#include "GeneratorFilters/xAODMultiLeptonFilter.h"
#include "GeneratorFilters/xAODMultiMuonFilter.h"
#include "GeneratorFilters/xAODMuonFilter.h"
#include "GeneratorFilters/xAODParentChildFilter.h"
#include "GeneratorFilters/xAODParentTwoChildrenFilter.h"
#include "GeneratorFilters/xAODPhotonFilter.h"
#include "GeneratorFilters/xAODSameParticleHardScatteringFilter.h"
#include "GeneratorFilters/xAODTauFilter.h"
#include "GeneratorFilters/xAODTTbarWithJpsimumuFilter.h"
#include "GeneratorFilters/xAODTTbarWToLeptonFilter.h"
#include "GeneratorFilters/xAODVBFForwardJetsFilter.h"
#include "GeneratorFilters/xAODVBFMjjIntervalFilter.h"
#include "GeneratorFilters/xAODXtoVVDecayFilterExtended.h"


// slimmers for 22.6
#include "GeneratorFilters/PileupTruthParticleSlimmer.h"
#include "GeneratorFilters/xAODTruthParticleSlimmerElectron.h"
#include "GeneratorFilters/xAODTruthParticleSlimmerLightLepton.h"
#include "GeneratorFilters/xAODTruthParticleSlimmerMET.h"
#include "GeneratorFilters/xAODTruthParticleSlimmerMuon.h"
#include "GeneratorFilters/xAODTruthParticleSlimmerPhoton.h"
#include "GeneratorFilters/xAODTruthParticleSlimmerTau.h"

// other accepted for 22.6
#include "GeneratorFilters/BSignalFilter.h"
#include "GeneratorFilters/ChargedTracksFilter.h"
#include "GeneratorFilters/ChargedTracksWeightFilter.h"
#include "GeneratorFilters/DecaysFinalStateFilter.h"
#include "GeneratorFilters/DecayTimeFilter.h"
#include "GeneratorFilters/DiLeptonMassFilter.h"
#include "GeneratorFilters/DirectPhotonFilter.h"
#include "GeneratorFilters/HeavyFlavorHadronFilter.h"
#include "GeneratorFilters/LeptonFilter.h" 
#include "GeneratorFilters/LeptonPairFilter.h"
#include "GeneratorFilters/M4MuIntervalFilter.h"
#include "GeneratorFilters/MissingEtFilter.h"
#include "GeneratorFilters/MultiBjetFilter.h"
#include "GeneratorFilters/MultiCjetFilter.h"
#include "GeneratorFilters/MultiElecMuTauFilter.h"
#include "GeneratorFilters/MultiLeptonFilter.h"
#include "GeneratorFilters/MultiMuonFilter.h"
#include "GeneratorFilters/ParentChildFilter.h"
#include "GeneratorFilters/ParentTwoChildrenFilter.h"
#include "GeneratorFilters/SameParticleHardScatteringFilter.h"
#include "GeneratorFilters/TauFilter.h"
#include "GeneratorFilters/TTbarWithJpsimumuFilter.h"
#include "GeneratorFilters/VBFForwardJetsFilter.h"
#include "GeneratorFilters/VBFMjjIntervalFilter.h"
#include "GeneratorFilters/XtoVVDecayFilterExtended.h"
// other
#include "GeneratorFilters/ElectronFilter.h"
#include "GeneratorFilters/ZtoLeptonFilter.h"
#include "GeneratorFilters/BSignalFilter.h"
#include "GeneratorFilters/ATauFilter.h"
#include "GeneratorFilters/JetFilter.h"
#include "GeneratorFilters/JetForwardFilter.h"
#include "GeneratorFilters/GapJetFilter.h"
#include "GeneratorFilters/ParticleFilter.h"
#include "GeneratorFilters/PhotonFilter.h"
#include "GeneratorFilters/TauFilter.h"
#include "GeneratorFilters/TTbarFilter.h"
#include "GeneratorFilters/TTbarPhotonWhizardHwgFilter.h"
#include "GeneratorFilters/TTbarWToLeptonFilter.h"
#include "GeneratorFilters/TTbarMassFilter.h"
#include "GeneratorFilters/TruthJetFilter.h"
#include "GeneratorFilters/SoftLeptonFilter.h"
#include "GeneratorFilters/ParentChildwStatusFilter.h"
#include "GeneratorFilters/SoftLeptonInJetFilter.h"
#include "GeneratorFilters/TTbarPlusJetsFilter.h"
#include "GeneratorFilters/WMultiLeptonFilter.h"
#include "GeneratorFilters/PtmissAndOrLeptonFilter.h"
#include "GeneratorFilters/MuonFilter.h"
#include "GeneratorFilters/WZtoLeptonFilter.h"
#include "GeneratorFilters/AsymJetFilter.h"
#include "GeneratorFilters/MultiObjectsFilter.h"
#include "GeneratorFilters/MassRangeFilter.h"
#include "GeneratorFilters/DecayLengthFilter.h"
#include "GeneratorFilters/WeightedBDtoElectronFilter.h"
#include "GeneratorFilters/JetIntervalFilter.h"
#include "GeneratorFilters/XXvvGGFilter.h"
#include "GeneratorFilters/TruthJetWeightFilter.h"
#include "GeneratorFilters/MultiElectronFilter.h"
#include "GeneratorFilters/DiPhotonFilter.h"
#include "GeneratorFilters/DecayModeFilter.h"
#include "GeneratorFilters/DstD0K3piFilter.h"
#include "GeneratorFilters/FourLeptonMassFilter.h"
#include "GeneratorFilters/FourLeptonInvMassFilter.h"
#include "GeneratorFilters/HtoVVFilter.h"
#include "GeneratorFilters/QCDTruthMultiJetFilter.h"  
#include "GeneratorFilters/VBFHbbEtaSortingFilter.h"  
#include "GeneratorFilters/TopCKMFilter.h"
#include "GeneratorFilters/ForwardProtonFilter.h"
#include "GeneratorFilters/BSubstruct.h"
#include "GeneratorFilters/TopCKMKinFilter.h"
#include "GeneratorFilters/MultiHiggsFilter.h"
#include "GeneratorFilters/XtoVVDecayFilter.h"
#include "GeneratorFilters/VHtoVVFilter.h"
#include "GeneratorFilters/TtHtoVVDecayFilter.h"
#include "GeneratorFilters/SusySubprocessFinder.h"
#include "GeneratorFilters/LeadingPhotonFilter.h"
#include "GeneratorFilters/VHtoVVDiLepFilter.h"
#include "GeneratorFilters/JetFilterWithTruthPhoton.h"
#include "GeneratorFilters/Boosted2DijetFilter.h"
#include "GeneratorFilters/DiBjetFilter.h"
#include "GeneratorFilters/HiggsFilter.h"
#include "GeneratorFilters/TTbarBoostCatFilter.h"
#include "GeneratorFilters/MultiParticleFilter.h" 
#include "GeneratorFilters/DecayPositionFilter.h"
#include "GeneratorFilters/HTFilter.h"
#include "GeneratorFilters/TTbarPlusHeavyFlavorFilter.h"
#include "GeneratorFilters/DuplicateEventFilter.h"
#include "GeneratorFilters/BoostedHadTopAndTopPair.h"
#include "GeneratorFilters/HTFilter.h"
#include "GeneratorFilters/TrimuMassRangeFilter.h"


//filters accepted for 22.6
DECLARE_COMPONENT( QCDTruthJetFilter )
DECLARE_COMPONENT( xAODBSignalFilter )
DECLARE_COMPONENT( xAODChargedTracksFilter )
DECLARE_COMPONENT( xAODChargedTracksWeightFilter )
DECLARE_COMPONENT( xAODDecaysFinalStateFilter )
DECLARE_COMPONENT( xAODDecayTimeFilter )
DECLARE_COMPONENT( xAODDiLeptonMassFilter )
DECLARE_COMPONENT( xAODDirectPhotonFilter )
DECLARE_COMPONENT( xAODElectronFilter )
DECLARE_COMPONENT( xAODLeptonFilter ) 
DECLARE_COMPONENT( xAODLeptonPairFilter )
DECLARE_COMPONENT( xAODM4MuIntervalFilter )
DECLARE_COMPONENT( xAODMETFilter)
DECLARE_COMPONENT( xAODMultiBjetFilter )
DECLARE_COMPONENT( xAODMultiCjetFilter )
DECLARE_COMPONENT( xAODMultiElecMuTauFilter )
DECLARE_COMPONENT( xAODMultiLeptonFilter)
DECLARE_COMPONENT( xAODMultiMuonFilter)
DECLARE_COMPONENT( xAODMuonFilter )
DECLARE_COMPONENT( xAODParentChildFilter )
DECLARE_COMPONENT( xAODParentTwoChildrenFilter )
DECLARE_COMPONENT( xAODPhotonFilter )
DECLARE_COMPONENT( xAODSameParticleHardScatteringFilter )
DECLARE_COMPONENT( xAODTauFilter )
DECLARE_COMPONENT( xAODTTbarWithJpsimumuFilter )
DECLARE_COMPONENT( xAODTTbarWToLeptonFilter )
DECLARE_COMPONENT( xAODVBFForwardJetsFilter )
DECLARE_COMPONENT( xAODVBFMjjIntervalFilter )
DECLARE_COMPONENT( xAODXtoVVDecayFilterExtended )

//slimmers accepted for 22.6
DECLARE_COMPONENT( PileupTruthParticleSlimmer )
DECLARE_COMPONENT( xAODTruthParticleSlimmerElectron )
DECLARE_COMPONENT( xAODTruthParticleSlimmerLightLepton )
DECLARE_COMPONENT( xAODTruthParticleSlimmerMET)
DECLARE_COMPONENT( xAODTruthParticleSlimmerMuon )
DECLARE_COMPONENT( xAODTruthParticleSlimmerPhoton)
DECLARE_COMPONENT( xAODTruthParticleSlimmerTau )

// other accepted for 22.6
DECLARE_COMPONENT( BSignalFilter )
DECLARE_COMPONENT( ChargedTracksFilter )
DECLARE_COMPONENT( ChargedTracksWeightFilter )
DECLARE_COMPONENT( DecaysFinalStateFilter ) 
DECLARE_COMPONENT( DecayTimeFilter )
DECLARE_COMPONENT( DiLeptonMassFilter )
DECLARE_COMPONENT( DirectPhotonFilter )
DECLARE_COMPONENT( HeavyFlavorHadronFilter )
DECLARE_COMPONENT( LeptonFilter )
DECLARE_COMPONENT( LeptonPairFilter )
DECLARE_COMPONENT( M4MuIntervalFilter )
DECLARE_COMPONENT( MissingEtFilter )
DECLARE_COMPONENT( MultiBjetFilter )
DECLARE_COMPONENT( MultiCjetFilter )
DECLARE_COMPONENT( MultiElecMuTauFilter )
DECLARE_COMPONENT( MultiLeptonFilter )   
DECLARE_COMPONENT( MultiMuonFilter )  
DECLARE_COMPONENT( ParentChildFilter )
DECLARE_COMPONENT( ParentTwoChildrenFilter ) 
DECLARE_COMPONENT( SameParticleHardScatteringFilter )
DECLARE_COMPONENT( TauFilter )
DECLARE_COMPONENT( TTbarWithJpsimumuFilter )
DECLARE_COMPONENT( VBFForwardJetsFilter )
DECLARE_COMPONENT( VBFMjjIntervalFilter )
DECLARE_COMPONENT( XtoVVDecayFilterExtended )

// others
DECLARE_COMPONENT( ElectronFilter )
DECLARE_COMPONENT( LeptonFilter )
DECLARE_COMPONENT( ZtoLeptonFilter )
DECLARE_COMPONENT( BSignalFilter )
DECLARE_COMPONENT( ATauFilter )
DECLARE_COMPONENT( JetFilter )
DECLARE_COMPONENT( JetForwardFilter )
DECLARE_COMPONENT( GapJetFilter )
DECLARE_COMPONENT( ParticleFilter )
DECLARE_COMPONENT( PhotonFilter )
DECLARE_COMPONENT( TTbarFilter )
DECLARE_COMPONENT( TTbarPhotonWhizardHwgFilter )
DECLARE_COMPONENT( TTbarWToLeptonFilter )
DECLARE_COMPONENT( TTbarMassFilter )
DECLARE_COMPONENT( TruthJetFilter )
DECLARE_COMPONENT( SoftLeptonFilter )
DECLARE_COMPONENT( ParentChildwStatusFilter )
DECLARE_COMPONENT( SoftLeptonInJetFilter )
DECLARE_COMPONENT( TTbarPlusJetsFilter )
DECLARE_COMPONENT( WMultiLeptonFilter )
DECLARE_COMPONENT( PtmissAndOrLeptonFilter )
DECLARE_COMPONENT( MuonFilter )
DECLARE_COMPONENT( WZtoLeptonFilter )
DECLARE_COMPONENT( AsymJetFilter )
DECLARE_COMPONENT( MultiObjectsFilter )
DECLARE_COMPONENT( MassRangeFilter )
DECLARE_COMPONENT( DecayLengthFilter )
DECLARE_COMPONENT( WeightedBDtoElectronFilter )
DECLARE_COMPONENT( JetIntervalFilter )
DECLARE_COMPONENT( XXvvGGFilter )
DECLARE_COMPONENT( TruthJetWeightFilter )
DECLARE_COMPONENT( MultiElectronFilter )
DECLARE_COMPONENT( DiPhotonFilter )
DECLARE_COMPONENT( DecayModeFilter )
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
DECLARE_COMPONENT( VHtoVVFilter )
DECLARE_COMPONENT( TtHtoVVDecayFilter )
DECLARE_COMPONENT( SusySubprocessFinder )
DECLARE_COMPONENT( LeadingPhotonFilter )
DECLARE_COMPONENT( VHtoVVDiLepFilter )
DECLARE_COMPONENT( JetFilterWithTruthPhoton )
DECLARE_COMPONENT( Boosted2DijetFilter )
DECLARE_COMPONENT( DiBjetFilter )
DECLARE_COMPONENT( HiggsFilter )
DECLARE_COMPONENT( TTbarBoostCatFilter )
DECLARE_COMPONENT( MultiParticleFilter )
DECLARE_COMPONENT( DecayPositionFilter )
DECLARE_COMPONENT( HTFilter )
DECLARE_COMPONENT( TTbarPlusHeavyFlavorFilter )
DECLARE_COMPONENT( DuplicateEventFilter )
DECLARE_COMPONENT( BoostedHadTopAndTopPair )
DECLARE_COMPONENT( HTFilter )
DECLARE_COMPONENT( TrimuMassRangeFilter )
DECLARE_COMPONENT( M4MuIntervalFilter )

