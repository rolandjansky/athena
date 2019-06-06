/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkHI/HIGlobalAugmentationTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include <vector>

namespace DerivationFramework
{
  HIGlobalAugmentationTool::HIGlobalAugmentationTool( 	const std::string& t,
								const std::string& n,
								const IInterface* p ) :   AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("InDetTrackParticlesKey", m_TP_key="InDetTrackParticles");
    declareProperty("TrackSelectionTools", m_trkSelTools, "Track selection tools" );
    declareProperty("cutLevels", m_cutLevels, "Cut levels");
    declareProperty("nHarmonic", m_nHarmonic = 1, "Flow harmonic starting from v2" );
  }
  
  // Destructor
  HIGlobalAugmentationTool::~HIGlobalAugmentationTool() 
  {
  }  

  // Athena initialize and finalize
  StatusCode HIGlobalAugmentationTool::initialize()
  {
    unsigned int nTrackSels  = m_trkSelTools.size();
    for (unsigned int its = 0; its < nTrackSels; ++its) {      
        ATH_CHECK(m_trkSelTools[its].retrieve());
        m_decTrack_count.push_back(new SG::AuxElement::Decorator<int>("TrackParticleMultiplicity_" + m_cutLevels[its] ));
    }
    //initilize decorators for flow vectors
    for (int vn = 1; vn <= m_nHarmonic; ++vn){    
        m_decFCalEtA_Qnx.push_back(new SG::AuxElement::Decorator<float>("FCalEtA_Q" + std::to_string(vn+1) + "x"));
        m_decFCalEtA_Qny.push_back(new SG::AuxElement::Decorator<float>("FCalEtA_Q" + std::to_string(vn+1) + "y"));
        m_decFCalEtC_Qnx.push_back(new SG::AuxElement::Decorator<float>("FCalEtC_Q" + std::to_string(vn+1) + "x"));
        m_decFCalEtC_Qny.push_back(new SG::AuxElement::Decorator<float>("FCalEtC_Q" + std::to_string(vn+1) + "y"));
    }     
    
    return StatusCode::SUCCESS;
  }
  StatusCode HIGlobalAugmentationTool::finalize()
  {
    for (auto trkSelTool : m_trkSelTools ) { 
        ATH_CHECK(trkSelTool->finalize());
    }
    for(const auto& idec : m_decTrack_count) delete idec;
    
    for(const auto& fdec : m_decFCalEtA_Qnx) delete fdec;
    for(const auto& fdec : m_decFCalEtA_Qny) delete fdec;
    for(const auto& fdec : m_decFCalEtC_Qnx) delete fdec;
    for(const auto& fdec : m_decFCalEtC_Qny) delete fdec;
    return StatusCode::SUCCESS;
  }

  StatusCode HIGlobalAugmentationTool::addBranches() const
  {

    // Set up the decorators 
    //SG::AuxElement::Decorator< int > decTrack_count("TrackParticleMultiplicity"); 
    
    //Load event EventInfo
    const xAOD::EventInfo* eventInfo = nullptr;     
    if(evtStore()->retrieve( eventInfo ).isFailure()){
       ATH_MSG_ERROR ("Cannot retrieve event info");
       return StatusCode::FAILURE;
    }
    
    //Load track particle container
    const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >(m_TP_key);
    if(!tracks) 
    {
      ATH_MSG_ERROR ("Couldn't retrieve TrackParticleContainer with key " << m_TP_key);
      return StatusCode::FAILURE;
    }

    //Count tracks
    unsigned int nTrackSels  = m_trkSelTools.size();
    std::vector <int> TrackParticleMultiplicity(nTrackSels,0);
    for(auto tp : *tracks)
    {
      if(tp)
      {
	      for (unsigned int its = 0; its < nTrackSels; ++its) {  
	        if(m_trkSelTools[its]->accept(*tp)) ++TrackParticleMultiplicity[its];
	      }  
      }
    }
    //decorate eventInfo with track multiplicity
    for (unsigned int its = 0; its < nTrackSels; ++its) {  
        (*m_decTrack_count[its])(*eventInfo) = TrackParticleMultiplicity[its];    
	}
    
    float FCalEtA = 0;
    float FCalEtC = 0;
    
    std::vector<float> FCalEtA_Qnx(m_nHarmonic,0);
    std::vector<float> FCalEtA_Qny(m_nHarmonic,0);
    std::vector<float> FCalEtC_Qnx(m_nHarmonic,0);
    std::vector<float> FCalEtC_Qny(m_nHarmonic,0);

        
    // Set up the decorators for FCal Et
    SG::AuxElement::Decorator< float > decFCalEtA("FCalEtA");
    SG::AuxElement::Decorator< float > decFCalEtC("FCalEtC");    
    
    //Retrieve HIEventShape
    const xAOD::HIEventShapeContainer *eventShape = nullptr;      
    if(evtStore()->retrieve( eventShape, "HIEventShape" ).isFailure()){
       ATH_MSG_ERROR ("Cannot retrieve HIEventShape");
       return StatusCode::FAILURE;
    }
         
    //Calculate FCal A and FCal C sums 
    for(const auto* ptrEvtShp : *eventShape){
       if(ptrEvtShp->layer()!=21 && ptrEvtShp->layer()!=22 &&
           ptrEvtShp->layer()!=23) continue;

       float eta = ptrEvtShp->etaMin();
       const std::vector<float>&c1 = ptrEvtShp->etCos();
       const std::vector<float>&s1 = ptrEvtShp->etSin();

       if (eta > 0) {
          FCalEtA += ptrEvtShp->et();
          for (int vn = 1; vn <= m_nHarmonic; ++vn){    
              FCalEtA_Qnx.at(vn-1) += c1.at(vn);
              FCalEtA_Qny.at(vn-1) += s1.at(vn);
          } 
       } else if (eta < 0) {
          FCalEtC += ptrEvtShp->et();
          for (int vn = 1; vn <= m_nHarmonic; ++vn){    
              FCalEtC_Qnx.at(vn-1) += c1.at(vn);
              FCalEtC_Qny.at(vn-1) += s1.at(vn);
          }   
       }
    } 
    //decorate
    decFCalEtA(*eventInfo) = FCalEtA;
    decFCalEtC(*eventInfo) = FCalEtC;
    for (int vn = 0; vn < m_nHarmonic; ++vn){ 
        (*m_decFCalEtA_Qnx[vn])(*eventInfo) = FCalEtA_Qnx.at(vn);
        (*m_decFCalEtA_Qny[vn])(*eventInfo) = FCalEtA_Qny.at(vn);
        (*m_decFCalEtC_Qnx[vn])(*eventInfo) = FCalEtC_Qnx.at(vn);
        (*m_decFCalEtC_Qny[vn])(*eventInfo) = FCalEtC_Qny.at(vn);
    }
        
    return StatusCode::SUCCESS;
  }  
  
}
