/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZeeVertexRefittingTool.cxx, (c) ATLAS Detector software
// Author: Ioannis Nomidis (ioannis.nomidis@cern.ch)
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkHiggs/ZeeVertexRefittingTool.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "PhotonVertexSelection/PhotonVertexHelpers.h"

#include "TLorentzVector.h"


namespace DerivationFramework {

  static const SG::AuxElement::Decorator<float> sumPt2("sumPt2");  
  static SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::TrackParticleContainer> > > electronTrackLinksDecor("ElectronTrackLinks"); 

  ZeeVertexRefittingTool::ZeeVertexRefittingTool(const std::string& t,
					   const std::string& n,
					   const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_expression("true"),
    m_parser(nullptr),
    m_massCut(0.0),
    m_refitpvContainerName(""),
    m_elContainerName(""),
    m_pvContainerName(""),
    m_pvrefitter("Analysis::PrimaryVertexRefitter")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ObjectRequirements", m_expression);  
    declareProperty("LowMassCut", m_massCut);
    declareProperty("RefittedPVContainerName", m_refitpvContainerName);    
    declareProperty("ElectronContainerName", m_elContainerName,"Electrons");
    declareProperty("PVContainerName",m_pvContainerName,"PrimaryVertices");
    declareProperty("MCSamples",m_MCSamples);
  }

  StatusCode ZeeVertexRefittingTool::initialize()
  {
    if (m_refitpvContainerName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of ZeeVertexRefittingTool!");
      return StatusCode::FAILURE;
    }

    CHECK( m_pvrefitter.retrieve() );

    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));          
    m_parser = std::make_unique<ExpressionParsing::ExpressionParser>(proxyLoaders);
    m_parser->loadExpression(m_expression);
    return StatusCode::SUCCESS;
  }

  StatusCode ZeeVertexRefittingTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode ZeeVertexRefittingTool::addBranches() const
  {
    // skip mc samples not included in the MCSamples list
    const xAOD::EventInfo *eventInfo(nullptr);
    if((evtStore()->retrieve(eventInfo)).isFailure()) return false;    
    if(eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      bool skipSample = true;
      for (auto mcid : m_MCSamples) {        
        if (mcid==eventInfo->mcChannelNumber()) {
          skipSample = false;
          break;
        }
      }
      if (skipSample) return StatusCode::SUCCESS;
    }

    // check that container we want to write in SG does not yet exist
    if (evtStore()->contains<xAOD::VertexContainer>(m_refitpvContainerName)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_refitpvContainerName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }

    const xAOD::VertexContainer* pv_cont = nullptr;
    ATH_CHECK( evtStore()->retrieve( pv_cont, m_pvContainerName ) );

    std::unique_ptr<xAOD::VertexContainer> refittedPVContainer = std::make_unique<xAOD::VertexContainer>();
    std::unique_ptr<xAOD::VertexAuxContainer> refittedPVAuxContainer = std::make_unique<xAOD::VertexAuxContainer>();
    refittedPVContainer->setStore(refittedPVAuxContainer.get());

    const xAOD::Vertex* pv = nullptr;
    for ( auto v : *pv_cont ) {
      if (v->vertexType()==xAOD::VxType::PriVtx) {
	pv = v;
	break;
      }
    }
    if (!pv) {
      ATH_MSG_WARNING("Primary vertex not found");
      CHECK(evtStore()->record( refittedPVContainer.release(), m_refitpvContainerName));
      CHECK(evtStore()->record( refittedPVAuxContainer.release(), m_refitpvContainerName+"Aux."));
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Found PV");

    // retrieve particle collections
    const xAOD::ElectronContainer* electrons = nullptr;
    ATH_CHECK( evtStore()->retrieve( electrons, m_elContainerName ) );

    // create the vector which will hold the Zee pairs
    std::vector< std::vector<unsigned int> > eepairs;
    CHECK( makeZeePairs( electrons, eepairs ) );
  
    ATH_MSG_DEBUG("ee pairs found: " << eepairs.size());    

    for (auto pair : eepairs) {
      std::vector<const xAOD::TrackParticle*> tps = { 
        xAOD::EgammaHelpers::getOriginalTrackParticle( electrons->at(pair[0]) ),
        xAOD::EgammaHelpers::getOriginalTrackParticle( electrons->at(pair[1]) )
      };
      ATH_MSG_DEBUG("Refitting PV for e tracks: " << tps[0] << " " << tps[1]);      
      const xAOD::Vertex* pv_ref = m_pvrefitter->refitVertex(pv,tps);
      if (pv_ref) {                
      	refittedPVContainer->push_back(const_cast<xAOD::Vertex*>(pv_ref)); //must remove const-ness: since PrimaryVertexRefitter is given the parameter returnCopy=true, it will return a newly allocated xAOD::Vertex object via const pointer, requiring the const to be cast away to add it to the container.
            
        ATH_MSG_DEBUG("refitted PV nTP: " << pv_ref->nTrackParticles() << " -- " << pv->nTrackParticles());
        ATH_MSG_DEBUG("refitted PV z: " << pv_ref->z() << " -- " << pv->z());

	if (pv_ref->nTrackParticles() < pv->nTrackParticles()) {          
          sumPt2(*pv_ref) = xAOD::PVHelpers::getVertexSumPt(pv_ref, 2, false);
      	  //set links to electrons, used only for matching, not for vertexing
          std::vector<ElementLink<xAOD::TrackParticleContainer> > electronTrackLinks = {
            electrons->at(pair[0])->trackParticleLink(),
            electrons->at(pair[1])->trackParticleLink() 
          };
          electronTrackLinksDecor(*pv_ref) = electronTrackLinks;
        } else {
          ATH_MSG_DEBUG("Electrons from pair not used in the refitting ");
        }
      }
      else {
        ATH_MSG_DEBUG("refitting failed");      
      }   
    }

    ATH_MSG_DEBUG("Vertex container size: " << refittedPVContainer->size());
    CHECK(evtStore()->record( refittedPVContainer.release(), m_refitpvContainerName));
    CHECK(evtStore()->record( refittedPVAuxContainer.release(), m_refitpvContainerName+"Aux."));

    return StatusCode::SUCCESS;
  }

  StatusCode ZeeVertexRefittingTool::makeZeePairs( const xAOD::ElectronContainer *particles, std::vector<std::vector<unsigned int> > &ZeePairs) const
  {     
    if (particles->size()<2) return StatusCode::SUCCESS;

    // flags for the result of selection for each electron 
    std::vector<int> isSelected = m_parser->evaluateAsVector();
    unsigned int nEntries = isSelected.size();

    // if there are no particles in one of the two lists to combine, just leave function
    if (nEntries==0) return StatusCode::SUCCESS; 

    // check the sizes are compatible
    if (particles->size() != nEntries ) { 
      ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
      return StatusCode::FAILURE;
    }     
    
    // Double loop to get the opposite-charge pairs with m>50 GeV
    for (unsigned int i=0; i<nEntries-1; ++i) {    
      if (isSelected[i]!=1) continue;     
      float qi = particles->at(i)->charge();

      for (unsigned int j=i+1; j<nEntries; ++j) {
        if (isSelected[j]!=1) continue; 
        //std::vector<int> tmpPair; tmpPair.clear();
        float qj = particles->at(j)->charge();
        // opposite charge
        if (qi*qj>=0) continue;

        //invariant mass
        float mass = (particles->at(i)->p4()+particles->at(j)->p4()).M();
        if (mass<m_massCut) continue;

        ZeePairs.push_back( {i, j} );
      }
    }
    return StatusCode::SUCCESS; 
  }
}
