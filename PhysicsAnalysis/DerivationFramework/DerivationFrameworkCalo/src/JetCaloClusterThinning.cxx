/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// JetCaloClusterThinning.cxx, (c) ATLAS Detector software
// @author Danilo E. Ferreira de Lima <dferreir@cern.ch>
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkCalo/JetCaloClusterThinning.h"

//#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODJet/Jet.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODJet/JetConstituentVector.h"

#include <vector>
#include <string>

// Constructor
DerivationFramework::JetCaloClusterThinning::JetCaloClusterThinning(const std::string& t,
                                                                    const std::string& n,
                                                                    const IInterface* p ) :
AthAlgTool(t,n,p),
m_thinningSvc("ThinningSvc",n),
m_ntot (0),
m_ntotTopo(0),
m_npass(0),
m_npassTopo(0),
m_sgKey(""),
m_TopoClSGKey("CaloCalTopoCluster"),
m_selectionString(""),
m_and(false),
m_parser(0)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("SGKey", m_sgKey);
    declareProperty("TopoClCollectionSGKey", m_TopoClSGKey);
    declareProperty("SelectionString", m_selectionString);
    declareProperty("AdditionalClustersKey"  , m_addClustersSGKey);
    declareProperty("ApplyAnd", m_and); 
}

// Destructor
DerivationFramework::JetCaloClusterThinning::~JetCaloClusterThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::JetCaloClusterThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_TopoClSGKey=="") {
        ATH_MSG_FATAL("No CalCaloTopoCluster collection provided for thinning.");
        return StatusCode::FAILURE;
    } else { ATH_MSG_INFO("Using " << m_TopoClSGKey << "as the source collection for topo calo clusters");}
    if (m_sgKey=="") {
        ATH_MSG_FATAL("No jet collection provided for thinning.");
        return StatusCode::FAILURE;
    } else { ATH_MSG_INFO("Calo clusters associated with objects in " << m_sgKey << " will be retained in this format with the rest being thinned away");}

    // Set up the text-parsing machinery for selectiong the photon directly according to user cuts
    if (m_selectionString!="") {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    if (m_selectionString!="") {
                m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
                m_parser->loadExpression(m_selectionString);
            }
    }
    
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::JetCaloClusterThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntotTopo <<" topo clusters, of which "<< m_npassTopo << " were retained ");
    if (m_selectionString!="") {
        delete m_parser;
        m_parser = 0;
    }
    
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::JetCaloClusterThinning::doThinning() const
{
    // Retrieve CalCaloTopo collection if required
    const xAOD::CaloClusterContainer* importedTopoCaloCluster;
    if (evtStore()->retrieve(importedTopoCaloCluster,m_TopoClSGKey).isFailure()) {
        ATH_MSG_ERROR("No CalCaloTopo collection with name " << m_TopoClSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    
    // retrieve additional topocluster container(s)
    std::vector< const xAOD::CaloClusterContainer* > additionalTopoCaloCluster;
    for (auto name : m_addClustersSGKey) {
      const xAOD::CaloClusterContainer* tempTopoCaloCluster;
      if (evtStore()->retrieve(tempTopoCaloCluster,name).isFailure()) {
        ATH_MSG_ERROR("No CalCaloTopo collection with name " << name << " found in StoreGate!");
        return StatusCode::FAILURE;
      }
      additionalTopoCaloCluster.push_back(tempTopoCaloCluster);
    }
    
    // Check the event contains topoclusters
    unsigned int nTopoClusters = importedTopoCaloCluster->size();
    if (nTopoClusters==0) return StatusCode::SUCCESS;

    // Set up a mask with the same entries as the full topocluster collection(s)
    std::vector<bool> topomask;
    topomask.assign(nTopoClusters,false);
    m_ntotTopo += nTopoClusters;
    
    // Retrieve jet container
    const xAOD::JetContainer* importedJet(0);
    if (evtStore()->retrieve(importedJet,m_sgKey).isFailure()) {
        ATH_MSG_ERROR("No jet collection with name " << m_sgKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    unsigned int nJets(importedJet->size());
    if (nJets==0) return StatusCode::SUCCESS;
    std::vector<const xAOD::Jet*> jetToCheck; jetToCheck.clear();
    
    // Execute the text parsers if requested
    if (m_selectionString!="") {
        std::vector<int> entries =  m_parser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nJets!= nEntries ) {
        	ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used jets objects??");
            return StatusCode::FAILURE;
        } else {
        	// identify which jets to keep for the thinning check
        	for (unsigned int i=0; i<nJets; ++i) if (entries[i]==1) jetToCheck.push_back((*importedJet)[i]);
        }
    }
    
    // Set elements in the mask to true if there is a corresponding ElementLink from a reconstructed object
    if (m_selectionString=="") { // check all objects as user didn't provide a selection string
       	setJetClustersMask(topomask, importedJet);
    } else {
       	setJetClustersMask(topomask, jetToCheck);
    }
    
    // Count up the mask contents
    for (unsigned int i=0; i<nTopoClusters; ++i) {
        if (topomask[i]) ++m_npassTopo;
    }
    
    // Execute the thinning service based on the mask. Finish.
    if (m_and) {
        if (m_thinningSvc->filter(*importedTopoCaloCluster, topomask, IThinningSvc::Operator::And).isFailure()) {
        	ATH_MSG_ERROR("Application of thinning service failed! ");
        	return StatusCode::FAILURE;
    	}
        if(additionalTopoCaloCluster.size()>0){
            for (auto addColl : additionalTopoCaloCluster) {
                if (m_thinningSvc->filter(*addColl, topomask, IThinningSvc::Operator::And).isFailure()) {
                    ATH_MSG_ERROR("Application of thinning service failed! ");
                    return StatusCode::FAILURE;
                }
            }            
        }
    }
    if (!m_and) {
        if (m_thinningSvc->filter(*importedTopoCaloCluster, topomask, IThinningSvc::Operator::Or).isFailure()) {
        	ATH_MSG_ERROR("Application of thinning service failed! ");
        	return StatusCode::FAILURE;
    	}
        if(additionalTopoCaloCluster.size()>0){
            for (auto addColl : additionalTopoCaloCluster) {
                if (m_thinningSvc->filter(*addColl, topomask, IThinningSvc::Operator::Or).isFailure()) {
                    ATH_MSG_ERROR("Application of thinning service failed! ");
                    return StatusCode::FAILURE;
                }
            }            
        }
    }
    
    return StatusCode::SUCCESS;
}

void DerivationFramework::JetCaloClusterThinning::setJetClustersMask(std::vector<bool>& mask,
                                                                      const xAOD::JetContainer*& jets
                                                                      ) const {
    for(auto jet : *jets){
        for( size_t j = 0; j < jet->numConstituents(); ++j ) {
            auto constit = jet->constituentLinks().at(j);
            int index = constit.index();
            mask[index] = true;
        }
    }
    return;
}

void DerivationFramework::JetCaloClusterThinning::setJetClustersMask(std::vector<bool>& mask,
                                                                      std::vector<const xAOD::Jet*>& jets
                                                                      ) const {
    for(auto jet : jets){
        for( size_t j = 0; j < jet->numConstituents(); ++j ) {
            auto constit = jet->constituentLinks().at(j);
            int index = constit.index();
            mask[index] = true;
        }
    }
    return;
}