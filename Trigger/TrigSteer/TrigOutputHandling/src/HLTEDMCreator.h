/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_HLTEDMCREATOR_H
#define TRIGOUTPUTHANDLING_HLTEDMCREATOR_H 1

// STL includes
#include <string>
#include <functional>
// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// OutputHandling includes
#include "DecisionHandling/IHLTOutputTool.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"
#include "AthViews/View.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigCalo/TrigCaloClusterContainer.h"
#include "xAODTrigCalo/TrigCaloClusterAuxContainer.h"

#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonAuxContainer.h"
#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "xAODTrigMuon/L2IsoMuonAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"
#include "xAODCore/ShallowAuxContainer.h"

/**
 * @class HLTEDMCreator
 * @brief Tool capable of creating collections missing (early rejection) after HLT processing.
 * It declares output handles for all possible collections produced by the trigger
 *
 * to add more classes follow these steps:
 * 1) add EDM pkg in CMakeLists
 * 2) add includes in this file
 * 3) add DEF_*_KEY here
 * 4) initialise it 
 * 5) add create in the createOutput
 * 6) test 
 **/

class HLTEDMCreator: public extends<AthAlgTool, IHLTOutputTool>  { 
 public: 
  HLTEDMCreator( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  virtual ~HLTEDMCreator(){}
  virtual StatusCode createOutput(const EventContext& context) const override;
  virtual StatusCode initialize() override;


 private: 

  HLTEDMCreator();
  Gaudi::Property<std::vector<std::string>> m_fixLinks{ this, "FixLinks", {}, "Which keys of the TrigCompositeContainer WriteHandleKeyArray might need to have their (e.g. feature) element links re-mapped outside of views"};
  SG::WriteDecorHandleKeyArray<xAOD::TrigCompositeContainer, std::vector<uint32_t> > m_remapLinkColKeys{ this, "DoNotSet_RemapLinkColKeys", {}, "Do not set, it is configured accordingly to FixLinks & TC output property"};
  SG::WriteDecorHandleKeyArray<xAOD::TrigCompositeContainer, std::vector<uint16_t> > m_remapLinkColIndices{ this, "DoNotSet_RemapLinkColIndices", {}, "Do not set, it is configured accordingly to FixLinks & TC output property"};

  Gaudi::Property<bool> m_dumpSGBefore{ this, "dumpSGBefore", false, "Dump SG content before the merging"}; // for debugging 
  Gaudi::Property<bool> m_dumpSGAfter { this, "dumpSGAfter", false, "Dump SG content after the merging"};

#define DEF_VIEWS(__TYPE) \
  SG::ReadHandleKeyArray< ViewContainer > m_##__TYPE##Views{ this, #__TYPE"Views", {}, "Name  views from where the "#__TYPE" will be read"}



#define DEF_KEY(__TYPE)	\
  SG::WriteHandleKeyArray<__TYPE> m_##__TYPE{ this, #__TYPE, {}, "Required collections of "#__TYPE}; \
  DEF_VIEWS(__TYPE); \
  SG::ReadHandleKeyArray<__TYPE> m_##__TYPE##InViews{ this, #__TYPE"InViews", {}, "Names of "#__TYPE" in respective views"}

#define DEF_XAOD_KEY(__TYPE) \
  SG::WriteHandleKeyArray<xAOD::__TYPE> m_##__TYPE{ this, #__TYPE, {}, "Required collections of xAOD::"#__TYPE};\
  DEF_VIEWS(__TYPE); \
  SG::ReadHandleKeyArray<xAOD::__TYPE> m_##__TYPE##InViews{ this, #__TYPE"InViews", {}, "Names of xAOD::"#__TYPE" in respective views"}

  DEF_KEY( TrigRoiDescriptorCollection );

  DEF_XAOD_KEY( TrigCompositeContainer );
  DEF_XAOD_KEY( TrigEMClusterContainer );
  DEF_XAOD_KEY( TrigCaloClusterContainer );
  DEF_XAOD_KEY( TrigRingerRingsContainer );
  DEF_XAOD_KEY( TrigElectronContainer );
  DEF_XAOD_KEY( ElectronContainer );
  DEF_XAOD_KEY( PhotonContainer );
  DEF_XAOD_KEY( TrigPhotonContainer );
  DEF_XAOD_KEY( TrackParticleContainer );
  DEF_XAOD_KEY( TrigMissingETContainer );
  DEF_XAOD_KEY( L2StandAloneMuonContainer );
  DEF_XAOD_KEY( L2CombinedMuonContainer );
  DEF_XAOD_KEY( L2IsoMuonContainer );
  DEF_XAOD_KEY( MuonContainer );
  DEF_XAOD_KEY( TauJetContainer );
  DEF_XAOD_KEY( TauTrackContainer );
  DEF_XAOD_KEY( CaloClusterContainer );
  DEF_XAOD_KEY( JetContainer );
  DEF_XAOD_KEY( VertexContainer );
  DEF_XAOD_KEY( BTaggingContainer );
  DEF_XAOD_KEY( BTagVertexContainer );
  DEF_XAOD_KEY( TrigBphysContainer );
#undef DEF_VIEWS
#undef DEF_KEY
#undef DEF_XAOD_KEY


  // special cases coded by hand (container not in views
  SG::ReadHandleKeyArray<xAOD::CaloClusterContainer> m_CaloClusterContainerShallowCopy{ this, "CaloClusterContainerShallowCopy", {}, "Required collections of xAOD::CaloClusterContainer wiht Aux of type ShallowAuxContainer"};
  SG::WriteHandleKeyArray<xAOD::CaloClusterContainer> m_CaloClusterContainerShallowCopyOut;

  template<typename T>
  struct HandlesGroup {
    HandlesGroup(SG::WriteHandleKeyArray<T>& out_,
		 SG::ReadHandleKeyArray<T>& in_,
		 SG::ReadHandleKeyArray< ViewContainer >& views_)
      : out(out_), in(in_), views(views_) {}

    SG::WriteHandleKeyArray<T>& out;
    SG::ReadHandleKeyArray<T>& in;
    SG::ReadHandleKeyArray< ViewContainer >& views;
  };

  /**
   * Init related handles
   * 
   * Fails when they are setup inconsistently: the only allowed setup is:
   * views and in empty - then out can be of any length - this is mode to fill missing containers
   * views and in not empty, have to be the same size - out has to be 1 - this mode is for merging from several views into one output
   * if merging from several views into several separate output containers is required another instance of that tool can be setup
   **/
  template<typename T>
  StatusCode initHandles( const HandlesGroup<T>&  handles );

  template<typename T>
  struct ConstHandlesGroup {
    ConstHandlesGroup(const SG::WriteHandleKeyArray<T>& out_,
		      const SG::ReadHandleKeyArray<T>& in_,
		      const SG::ReadHandleKeyArray< ViewContainer >& views_)
      : out(out_), in(in_), views(views_) {}

    const SG::WriteHandleKeyArray<T>& out;
    const SG::ReadHandleKeyArray<T>& in;
    const SG::ReadHandleKeyArray< ViewContainer >& views;
  };

  StatusCode fixLinks() const;

  template<typename T, typename G, typename M >
    StatusCode createIfMissing( const EventContext& context, const ConstHandlesGroup<T>& handles, 
				G& generator, M merger ) const;


  template<typename T>
  StatusCode  viewsMerge( ViewContainer const& views, const SG::ReadHandleKey<T>& inViewKey,
			  EventContext const& context, T & output ) const;
  
  template<typename T>
  StatusCode  noMerge( ViewContainer const& views, const SG::ReadHandleKey<T>& inViewKey,
		       EventContext const& context, T & output ) const;
  
}; 


#endif //> !TRIGOUTPUTHANDLING_HLTEDMCREATOR_H
