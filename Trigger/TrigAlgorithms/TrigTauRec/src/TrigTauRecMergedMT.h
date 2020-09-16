/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAURECMERGEDMT_H
#define TRIGTAURECMERGEDMT_H

// general athena stuff
#include "TrigTimeAlgs/TrigTimerSvc.h"

//Gaudi 
#include "GaudiKernel/ToolHandle.h" 
#include "GaudiKernel/ServiceHandle.h"

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "tauRecTools/ITauToolBase.h"

#include "BeamSpotConditionsData/BeamSpotData.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

class TrigTauRecMergedMT: public AthReentrantAlgorithm {

 public:

  TrigTauRecMergedMT(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigTauRecMergedMT();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

 private:

  template<class T, class U, class V> StatusCode deepCopy(T*& containerOut, U*& containerStoreOut, const V* dummyContainerType,
                                 const T*& oldContainer);
  template<class T, class U, class V> StatusCode deepCopy(T*& containerOut,
                                                          U*& containerStoreOut,
                                                          const V* dummyContainerType,
                                                          const T*& oldContainer) const;

  enum TAUEFCALOMON{
    NoROIDescr=0,
    NoCellCont=1,
    EmptyCellCont=2,
    NoClustCont=3,
    NoClustKey=4,
    EmptyClustCont=5,
    NoJetAttach=6,
    NoHLTtauAttach=7,
    NoHLTtauDetAttach=8,
    NoHLTtauXdetAttach=9
  };

  enum TAUEFTRKMON{
    NoTrkCont=0,
    NoVtxCont=1
  };

  /** internal tool store */
  const ToolHandleArray<ITauToolBase> m_tools{this, "Tools", {}, "List of ITauToolBase tools"};

  // Monitoring tool
  const ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

  //Gaudi::Property< std::string > m_outputName {this,"OutputCollection","TrigTauRecMerged","Name of output collection"};
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roIInputKey { this,"RoIInputKey","InputRoI","Input RoI name"};
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_L1RoIKey    { this, "L1RoIKey","L1RoI","L1 RoI name"};
  SG::ReadHandleKey< xAOD::CaloClusterContainer > m_clustersKey  { this, "clustersKey", "CaloClusters", "caloclusters in view" };
  SG::ReadHandleKey< xAOD::TrackParticleContainer > m_tracksKey  { this, "Key_trackPartInputContainer", "InDetTrackParticles", "input track particle container key"};
  SG::ReadHandleKey< xAOD::VertexContainer> m_vertexKey          { this, "Key_vertexInputContainer", "PrimaryVertices", "input vertex container key"};
  SG::ReadHandleKey< xAOD::TauJetContainer> m_trigTauJetKey      { this, "Key_trigTauJetInputContainer", "HLT_taujet", "input taujet container" };
  SG::ReadHandleKey< xAOD::TauTrackContainer> m_trigTauTrackInKey      { this, "Key_trigTauTrackInputContainer", "HLT_tautrack_input", "input tautrack container" };

  SG::WriteHandleKey< xAOD::JetContainer > m_trigtauSeedOutKey   { this,"Key_trigJetSeedOutputKey","HLT_jet_seed","Key for output jets which are seed for tau jets"};
  SG::WriteHandleKey< xAOD::TauJetContainer > m_trigtauRecOutKey {this,"Key_trigTauJetOutputContainer","HLT_taujet","Output taujet container"};
  SG::WriteHandleKey< xAOD::TauTrackContainer > m_trigtauTrkOutKey {this,"Key_trigTauTrackOutputContainer","HLT_tautrack","Output tautrack container"};

  Gaudi::Property< float > m_maxeta         { this, "maxeta", 2.5,"max eta for tau"};
  Gaudi::Property< float > m_minpt          { this, "minpt", 10000.0, "min pt for tau"};
  Gaudi::Property< std::string > m_beamType { this, "BeamType", "collisions", "Beam type"};
  Gaudi::Property< float > m_trkcone        { this, "trkcone", 0.2, "max distance track seed from roi center"};

};

  // Function to perform deep copy on container
  template<class T, class U, class V>
  StatusCode TrigTauRecMergedMT::deepCopy(T*& container,
                                          U*& containerStore,
                                          const V* /*dummyContainerElementType*/,
                                          const T*& oldContainer) const {
   // The new container should be null, check here
   if(container==0 && containerStore==0){
     container = new T();
     containerStore = new U();
     container->setStore(containerStore);
   }else{
     ATH_MSG_FATAL("Proviced non-null containters, not initializing please provide null containers: ");
     return StatusCode::FAILURE;
   }
   if(oldContainer != nullptr){
     for( const V* v : *oldContainer ){
       V* newV = new V();
       // Put objects into new container
       container->push_back(newV);
       // Copy across aux store
       *newV = *v;
     }
   }
   return StatusCode::SUCCESS;
  }

#endif
