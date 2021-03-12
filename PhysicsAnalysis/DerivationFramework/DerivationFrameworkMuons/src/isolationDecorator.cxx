/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMuons/isolationDecorator.h"
#include "AthenaKernel/errorcheck.h"
#include <vector>
#include <string>

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace{
    constexpr int NDIFF = xAOD::Iso::ptvarcone20 - xAOD::Iso::ptcone20;
}
// Constructor
DerivationFramework::isolationDecorator::isolationDecorator(const std::string& t,
							    const std::string& n,
							    const IInterface* p):
  ExpressionParserUser<AthAlgTool>(t, n, p),
  m_trackIsolationTool(),
  m_caloIsolationTool(),
  m_decorators(){
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("TrackIsolationTool", m_trackIsolationTool);
  declareProperty("CaloIsolationTool",  m_caloIsolationTool);
  declareProperty("TargetContainer",    m_containerName = "InDetTrackParticles");
  declareProperty("SelectionString",    m_selectionString = "");
  declareProperty("SelectionFlag",      m_selFlag = "");
  declareProperty("SelectionFlagValue", m_selFlagValue = 1);
  declareProperty("ptcones",            m_ptcones);
  declareProperty("topoetcones",        m_topoetcones);
  declareProperty("Prefix",             m_prefix="");
}
  
// Athena initialize and finalize
StatusCode DerivationFramework::isolationDecorator::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  // load the matching tool
  if( ! m_caloIsolationTool.empty() ) {
     ATH_CHECK( m_caloIsolationTool.retrieve() );
     ATH_MSG_INFO( "Successfully retrived the CaloIsolationTool!" );
  }

  if( ! m_trackIsolationTool.empty() ) {
     ATH_CHECK( m_trackIsolationTool.retrieve() );
     ATH_MSG_INFO( "Successfully retrived the CaloIsolationTool!" );
  }

  m_trkCorrList.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
  m_caloCorrList.calobitset.set(static_cast<unsigned int>(xAOD::Iso::coreCone));
  m_caloCorrList.calobitset.set(static_cast<unsigned int>(xAOD::Iso::pileupCorrection));

  /// create decorator list
  m_ptconeTypes.clear();
  for(auto c: m_ptcones){
    xAOD::Iso::IsolationType t = static_cast<xAOD::Iso::IsolationType>(c);
    xAOD::Iso::IsolationType t_var = static_cast<xAOD::Iso::IsolationType>(c+NDIFF);
    
    m_decorators.insert(std::make_pair(c, SG::AuxElement::Decorator< float >(m_prefix+xAOD::Iso::toString(t))));
       
    m_decorators.insert(std::make_pair(c+NDIFF, SG::AuxElement::Decorator< float >(m_prefix+xAOD::Iso::toString(t_var))));
    m_ptconeTypes.push_back(t);
  }
  m_topoetconeTypes.clear();
  for(auto c: m_topoetcones) {
    xAOD::Iso::IsolationType t = static_cast<xAOD::Iso::IsolationType>(c);
    m_decorators.insert(std::make_pair(c, SG::AuxElement::Decorator< float >(m_prefix+xAOD::Iso::toString(t))));
    m_topoetconeTypes.push_back(t);
  }

  // Set up the text-parsing machinery for thinning the tracks directly according to user cuts
  if (!m_selectionString.empty()) {
     ATH_CHECK( initializeParser( m_selectionString ) );
  }


  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::isolationDecorator::decorate(const xAOD::IParticle* part, const int iso_type, const float val) const{
    std::map<int, SG::AuxElement::Decorator<float>>::const_iterator itr = m_decorators.find(iso_type);
    if (itr == m_decorators.end()) {
        return StatusCode::FAILURE;
    }
    itr->second(*part) = val;
    return StatusCode::SUCCESS;
}
    
StatusCode DerivationFramework::isolationDecorator::addBranches() const {
  // retrieve tag (muon) container
   const xAOD::IParticleContainer* toDecorate = nullptr;
   ATH_CHECK(evtStore()->retrieve(toDecorate, m_containerName));
   
  // Execute the text parser and update the mask
  std::vector<int> entries(toDecorate->size(), 1);
  if (m_parser) {
      entries =  m_parser->evaluateAsVector();
      if (entries.size() != toDecorate->size()) {
          ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used ID TrackParticles?");
          return StatusCode::FAILURE;
      }
  }

  /// Loop over tracks
  int ipar=0;
  for(const auto particle : *toDecorate) {
    /// Only decorate those passed selection
    if (!entries[ipar++]) continue;

    /// check flag
    if(m_selFlag != ""){
      static SG::AuxElement::Decorator<int> dec_trkFlag(m_selFlag);
      if(dec_trkFlag(*particle)!=m_selFlagValue) continue;
     }

    /// check if decorated already
    static SG::AuxElement::Decorator< bool > dec_isoDecoDone(m_prefix+"IsoDecoDone");
    if(dec_isoDecoDone.isAvailable(*particle) && dec_isoDecoDone(*particle)) continue;
    dec_isoDecoDone(*particle) = true;

    /// track isolation
    xAOD::TrackIsolation resultTrack;
    if (m_trackIsolationTool->trackIsolation(resultTrack, *particle, m_ptconeTypes, m_trkCorrList)){
        for(unsigned int i=0; i<m_ptcones.size(); i++){
          ATH_CHECK(decorate(particle,m_ptcones[i],resultTrack.ptcones[i]));
          ATH_CHECK(decorate(particle,m_ptcones[i]+NDIFF,resultTrack.ptvarcones_10GeVDivPt[i]));        
        }
    }else{
        ATH_MSG_WARNING("Failed to apply the track isolation for a particle");
    }

    /// calo isolation
    xAOD::CaloIsolation resultCalo; 
    if (m_caloIsolationTool->caloTopoClusterIsolation(resultCalo, *particle, m_topoetconeTypes, m_caloCorrList)){
        for(unsigned int i=0; i<m_topoetcones.size(); i++){
           ATH_CHECK(decorate(particle,m_topoetcones[i],resultCalo.etcones[i]));
        }
    }else {
        ATH_MSG_WARNING("Failed to apply the calo isolation for a particle");
    }
  } 

  return StatusCode::SUCCESS;
}
