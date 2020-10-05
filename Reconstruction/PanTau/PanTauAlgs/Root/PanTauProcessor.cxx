/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PanTauProcessor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//! C++ includes
#include <string>

#include "PanTauAlgs/PanTauProcessor.h"

//! xAOD physics objects includes
#include "xAODTau/TauJet.h"
#include "xAODParticleEvent/Particle.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODParticleEvent/ParticleAuxContainer.h"

//! PanTau includes
#include "PanTauAlgs/Tool_InformationStore.h"

#include "PanTauAlgs/TauConstituent.h"
#include "PanTauAlgs/HelperFunctions.h"
#include "PanTauAlgs/PanTauSeed.h"



/// //////////////////////////////////////////////////////////
/// Constructor
/// //////////////////////////////////////////////////////////
PanTau::PanTauProcessor::PanTauProcessor(const std::string& name)
  : TauRecToolBase(name),
    m_Name_InputAlg(""),
        
    //Initialise members for tools
    m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"),
    m_Tool_TauConstituentGetter("PanTau::Tool_TauConstituentGetter/Tool_TauConstituentGetter", this),
    m_Tool_TauConstituentSelector("PanTau::Tool_TauConstituentSelector/Tool_TauConstituentSelector", this),
    m_Tool_FeatureExtractor("PanTau::Tool_FeatureExtractor/Tool_FeatureExtractor", this),
    m_Tool_DecayModeDeterminator("PanTau::Tool_DecayModeDeterminator/Tool_DecayModeDeterminator", this),
    m_Tool_DetailsArranger("PanTau::Tool_DetailsArranger/Tool_DetailsArranger", this)
{
    
    //Connect python variables to member functions...
    declareProperty("Name_InputAlg",                m_Name_InputAlg,                "Name of input algorithm for this instance");
    declareProperty("Tool_InformationStore",        m_Tool_InformationStore,        "Tool handle to Tool_InformationStore");
    declareProperty("Tool_TauConstituentGetter",    m_Tool_TauConstituentGetter,    "Tool handle to Tool_TauConstituentGetter");
    declareProperty("Tool_TauConstituentSelector",  m_Tool_TauConstituentSelector,  "Tool handle to Tool_TauConstituentSelector");
    declareProperty("Tool_FeatureExtractor",        m_Tool_FeatureExtractor,        "Tool handle to Tool_FeatureExtractor");
    declareProperty("Tool_DecayModeDeterminator",   m_Tool_DecayModeDeterminator,   "Tool handle to Tool_DecayModeDeterminator");
    declareProperty("Tool_DetailsArranger",         m_Tool_DetailsArranger,         "Tool handle to Tool_DetailsArranger");

    declareProperty("Tool_InformationStoreName",        m_Tool_InformationStoreName="",        "Tool handle to Tool_InformationStore");
    declareProperty("Tool_TauConstituentGetterName",    m_Tool_TauConstituentGetterName="",    "Tool handle to Tool_TauConstituentGetter");
    declareProperty("Tool_TauConstituentSelectorName",  m_Tool_TauConstituentSelectorName="",  "Tool handle to Tool_TauConstituentSelector");
    declareProperty("Tool_FeatureExtractorName",        m_Tool_FeatureExtractorName="",        "Tool handle to Tool_FeatureExtractor");
    declareProperty("Tool_DecayModeDeterminatorName",   m_Tool_DecayModeDeterminatorName="",   "Tool handle to Tool_DecayModeDeterminator");
    declareProperty("Tool_DetailsArrangerName",         m_Tool_DetailsArrangerName="",         "Tool handle to Tool_DetailsArranger");

}



/// //////////////////////////////////////////////////////////
/// Destructor
/// //////////////////////////////////////////////////////////
PanTau::PanTauProcessor::~PanTauProcessor() {
}



/// //////////////////////////////////////////////////////////
/// Initialize
/// //////////////////////////////////////////////////////////
StatusCode PanTau::PanTauProcessor::initialize() {
    
    ATH_MSG_INFO(name() << " initialize()");
    
    // retrieve StoreGate Service and the tools
    ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_InformationStore, m_Tool_InformationStoreName ) );
    ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_TauConstituentGetter, m_Tool_TauConstituentGetterName ) );
    ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_TauConstituentSelector, m_Tool_TauConstituentSelectorName ) );
    ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_FeatureExtractor, m_Tool_FeatureExtractorName ) );
    ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_DecayModeDeterminator, m_Tool_DecayModeDeterminatorName ) );
    ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_DetailsArranger, m_Tool_DetailsArrangerName ) );


    // retrieve StoreGate Service and the tools
    ATH_CHECK( m_Tool_InformationStore.retrieve() );
    ATH_CHECK( m_Tool_TauConstituentGetter.retrieve() );
    ATH_CHECK( m_Tool_TauConstituentSelector.retrieve() );
    ATH_CHECK( m_Tool_FeatureExtractor.retrieve() );
    ATH_CHECK( m_Tool_DecayModeDeterminator.retrieve() );
    ATH_CHECK( m_Tool_DetailsArranger.retrieve() );
    
    ATH_CHECK( m_Tool_InformationStore->dumpMaps() );
    
    ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("ModeDiscriminator_BinEdges_Pt", m_Config_PtBins) );
    m_Config_MinPt = m_Config_PtBins.front();
    m_Config_MaxPt = m_Config_PtBins.back();
    
    return StatusCode::SUCCESS;
}//end of initialize



/// //////////////////////////////////////////////////////////
/// Finalize
/// //////////////////////////////////////////////////////////
StatusCode PanTau::PanTauProcessor::finalize() {
    return StatusCode::SUCCESS;
}//end finalize

/// //////////////////////////////////////////////////////////
/// Execute
/// //////////////////////////////////////////////////////////
//StatusCode      PanTau::PanTauProcessor::execute(xAOD::TauJet& pTau) {
StatusCode      PanTau::PanTauProcessor::executePanTau(xAOD::TauJet& pTau, xAOD::ParticleContainer& pi0Container) {
    
    ATH_MSG_DEBUG("===========================================================");
    ATH_MSG_DEBUG("===      PanTau::PanTauProcessor --- Seed Creation      ===");
    ATH_MSG_DEBUG("===========================================================");
    ATH_MSG_DEBUG("Executing PanTau::PanTauProcessor::execute() for input alg: " << m_Name_InputAlg);
    
    //get the current TauJet
    xAOD::TauJet* curTauJet = &pTau;
    ATH_MSG_DEBUG("===> Tau: ");
    //curTauJet->dump();

    //check for null pointer
    if(curTauJet == 0) {
      ATH_MSG_WARNING("Pointer to xAOD::TauJet is 0!");
      return StatusCode::FAILURE;
    }

    fillDefaultValuesToTau(curTauJet);


    //keep track of the technical quality of the pantau seed to be created
    std::vector<int> pantauSeed_TechnicalQuality = std::vector<int>((unsigned int)PanTau::PanTauSeed2::t_nTechnicalQualities, 0);
    
    //Only process taus with 1 <= ntracks <= 5:
    int numTrack = curTauJet->nTracks();
    if(numTrack == 0 || numTrack > 5) {
      ATH_MSG_DEBUG("Flagging tau for algorithm " << m_Name_InputAlg << " as invalid, because numTrack is " << numTrack);
      pantauSeed_TechnicalQuality.at((int)PanTau::PanTauSeed2::t_NoValidInputTau) = 1;
    }
    
    
    // if there is substructure info available, get constituents, perform selection and recombination
    //these vectors will be owned by the PanTauSeed2 object.
    std::vector<TauConstituent2*> l_List_TauConstituents = std::vector<TauConstituent2*>(0);
    std::vector<TauConstituent2*> l_List_SelectedTauConstituents = std::vector<TauConstituent2*>(0);

    
    if(pantauSeed_TechnicalQuality.at((int)PanTau::PanTauSeed2::t_NoValidInputTau) == 0) {
      // Get the constituents for the current tau
      ATH_CHECK( m_Tool_TauConstituentGetter->GetTauConstituents(curTauJet, l_List_TauConstituents, m_Name_InputAlg) );
      if(l_List_TauConstituents.size() == 0)  {
	pantauSeed_TechnicalQuality.at((int)PanTau::PanTauSeed2::t_NoConstituentsAtAll) = 1;
	ATH_MSG_DEBUG("Seed has no associated constituents!");
      }
      
      // Call the TauConstituentSelector tool to throw out bad constituents
      ATH_CHECK(m_Tool_TauConstituentSelector->SelectTauConstituents(l_List_TauConstituents, l_List_SelectedTauConstituents) );
      if(l_List_SelectedTauConstituents.size() == 0) {
	pantauSeed_TechnicalQuality.at((int)PanTau::PanTauSeed2::t_NoSelectedConstituents) = 1;
	ATH_MSG_DEBUG("Seed has no associated constituents that pass selection!");
      }
      
      //check if there are core objects in the selected constituents
      bool hasCoreConstituents = false;
      for(unsigned int iConst=0; iConst<l_List_SelectedTauConstituents.size(); iConst++) {
	PanTau::TauConstituent2* curConst = l_List_SelectedTauConstituents.at(iConst);
	if(curConst->isOfType(PanTau::TauConstituent2::t_Charged) == true) {hasCoreConstituents = true; break;};
	if(curConst->isOfType(PanTau::TauConstituent2::t_Neutral) == true) {hasCoreConstituents = true; break;};
	if(curConst->isOfType(PanTau::TauConstituent2::t_Pi0Neut) == true) {hasCoreConstituents = true; break;};
      }
      
      if(hasCoreConstituents == false) {
	pantauSeed_TechnicalQuality.at((int)PanTau::PanTauSeed2::t_NoSelectedConstituents) = 1; //use this flag at the moment as a quick hack
      }
      
    }
        
    //to be consistent, taus without substructure algs run on them also have no constituents
    if(pantauSeed_TechnicalQuality.at((int)PanTau::PanTauSeed2::t_NoValidInputTau) == 1) {
      pantauSeed_TechnicalQuality.at((int)PanTau::PanTauSeed2::t_NoConstituentsAtAll) = 1;
      pantauSeed_TechnicalQuality.at((int)PanTau::PanTauSeed2::t_NoSelectedConstituents) = 1;
    }
    
    //check for the pT flag
    double tauJet_pT = curTauJet->ptIntermediateAxis();
    if(tauJet_pT < m_Config_MinPt || tauJet_pT > m_Config_MaxPt) {      
      pantauSeed_TechnicalQuality.at((int)PanTau::PanTauSeed2::t_BadPtValue) = 1;
    }
    

    // Now build the PanTauSeed2 with the new Constituents
    PanTau::PanTauSeed2* curPanTauSeed = new PanTau::PanTauSeed2(m_Name_InputAlg,
							       curTauJet,
							       l_List_SelectedTauConstituents,
							       l_List_TauConstituents, pantauSeed_TechnicalQuality);

    unsigned int nPi0LinksCellBased = curTauJet->protoPi0PFOLinks().size();
    curPanTauSeed->makePrivateStore();
    ATH_MSG_DEBUG("Created new PanTauSeed2 at " << curPanTauSeed << " with proto mode " << curPanTauSeed->getDecayModeBySubAlg() << " and nPi0 CellBased = " << nPi0LinksCellBased);
    
    // Get the features for this PanTauSeed
    ATH_MSG_DEBUG("Calculate features for this PanTauSeed");
    ATH_CHECK(m_Tool_FeatureExtractor->execute(curPanTauSeed) );
    
    //ATH_MSG_VERBOSE("Dumping features of finalized PanTauSeed2 ==========================================");
    //curPanTauSeed->getFeatures()->dump(m_log, MSG::VERBOSE);
    //ATH_MSG_VERBOSE("Dumped features of finalized PanTauSeed2 ===========================================");

    ATH_MSG_DEBUG("Finished adding input taus as PantauSeeds");
    
    
    
    //! =======================================================================================
    //! Finalize the seeds
    ATH_MSG_DEBUG("===========================================================");
    ATH_MSG_DEBUG("===    PanTau::PanTauProcessor --- Seed Finalization    ===");
    ATH_MSG_DEBUG("===========================================================");
    
    // Seed finalizing:
    //  1. Pass the seed to the decay mode determination tool
    //  2. Pass the seed to the DetailsArranger tool to calculate the four momentum and add the details to tauJet
       
    // 1. call decay mode determinator for this seed
    ATH_MSG_DEBUG("calling decay mode determinator for valid seed ");
    ATH_CHECK( m_Tool_DecayModeDeterminator->execute(curPanTauSeed) );
    
    // 2. calculate the four momentum and link the details to the tauJet
    ATH_CHECK( m_Tool_DetailsArranger->execute(curPanTauSeed, pi0Container) );
    
    //that's it :)
    ATH_MSG_DEBUG("PanTau::PanTauProcessor for input alg" << m_Name_InputAlg << " was successful!");

    delete curPanTauSeed;
    return StatusCode::SUCCESS;
}//end of execute



void PanTau::PanTauProcessor::fillDefaultValuesToTau(xAOD::TauJet* tauJet) {
    //default four momentum set to previous calibration state: TauEtaCalib
    TLorentzVector defaultP4 = tauJet->p4(xAOD::TauJetParameters::TauEtaCalib);
    tauJet->setP4(xAOD::TauJetParameters::PanTauCellBased, defaultP4.Pt(), defaultP4.Eta(), defaultP4.Phi(), defaultP4.M());
    
    //charged
    std::vector< ElementLink< xAOD::PFOContainer > > chrgPFOLinks = tauJet->protoChargedPFOLinks();
    tauJet->setChargedPFOLinks(chrgPFOLinks);
    
    //pi0
    std::vector< ElementLink< xAOD::PFOContainer > > pi0PFOLinks = tauJet->protoPi0PFOLinks();
    tauJet->setPi0PFOLinks(pi0PFOLinks);
    
    //neutrals
    std::vector< ElementLink< xAOD::PFOContainer > > neutralPFOLinks = tauJet->protoNeutralPFOLinks();
    tauJet->setNeutralPFOLinks(neutralPFOLinks);
    
    return;
}
