/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Alg_SeedBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//! C++ includes
#include <string>

//! core ATHENA includes
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"

//! ATHENA physics objects includes
#include "tauEvent/TauJetContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"

//! PanTau includes
#include "PanTauAlgs/Alg_SeedBuilder.h"
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/Tool_InputConverter.h"

#include "PanTauEvent/TauConstituent.h"
#include "PanTauEvent/PanTauSeed.h"
#include "PanTauEvent/PanTauSeedContainer.h"

#include "PanTauEvent/PanTauDetails.h"
#include "tauEvent/TauJet.h"

//! xAOD EDM
#include "xAODTau/TauJet.h"

//! Helper includes


/// //////////////////////////////////////////////////////////
/// Constructor
/// //////////////////////////////////////////////////////////
PanTau::Alg_SeedBuilder::Alg_SeedBuilder(const std::string& name, ISvcLocator* pSvcLocator)
    :
        AthAlgorithm(name, pSvcLocator),
        m_log(msgSvc(), name),
        m_sgSvc(0),
        m_Name_InputAlg(""),
        
        //Initialise members for tools
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore"),
        m_Tool_TauConstituentGetter("PanTau::Tool_TauConstituentGetter/Tool_TauConstituentGetter"),
        m_Tool_TauConstituentSelector("PanTau::Tool_TauConstituentSelector/Tool_TauConstituentSelector"),
        m_Tool_FeatureExtractor("PanTau::Tool_FeatureExtractor/Tool_FeatureExtractor"),
        m_Tool_DecayModeDeterminator("PanTau::Tool_DecayModeDeterminator/Tool_DecayModeDeterminator"),
        m_Tool_FourMomentumCalculator("PanTau::Tool_FourMomentumCalculator/Tool_FourMomentumCalculator"),
        m_Tool_DetailsArranger("PanTau::Tool_DetailsArranger/Tool_DetailsArranger")
{
    
    //Connect python variables to member functions...
    declareProperty("Name_InputAlg",                m_Name_InputAlg,                "Name of input algorithm for this instance");
    declareProperty("Tool_InformationStore",        m_Tool_InformationStore,        "Tool hanlde to Tool_InformationStore");
    declareProperty("Tool_TauConstituentGetter",    m_Tool_TauConstituentGetter,    "Tool hanlde to Tool_TauConstituentGetter");
    declareProperty("Tool_TauConstituentSelector",  m_Tool_TauConstituentSelector,  "Tool hanlde to Tool_TauConstituentSelector");
    declareProperty("Tool_FeatureExtractor",        m_Tool_FeatureExtractor,        "Tool hanlde to Tool_FeatureExtractor");
    declareProperty("Tool_DecayModeDeterminator",   m_Tool_DecayModeDeterminator,   "Tool hanlde to Tool_DecayModeDeterminator");
    declareProperty("Tool_FourMomentumCalculator",  m_Tool_FourMomentumCalculator,  "Tool hanlde to Tool_FourMomentumCalculator");
    declareProperty("Tool_DetailsArranger",         m_Tool_DetailsArranger,         "Tool hanlde to Tool_DetailsArranger");
}



/// //////////////////////////////////////////////////////////
/// Destructor
/// //////////////////////////////////////////////////////////
PanTau::Alg_SeedBuilder::~Alg_SeedBuilder() {
}



/// //////////////////////////////////////////////////////////
/// Initialize
/// //////////////////////////////////////////////////////////
StatusCode PanTau::Alg_SeedBuilder::initialize() {
    
    ATH_MSG_INFO(name() << " initialize()");
    
    // retrieve StoreGate Service and the tools
    CHECK(service("StoreGateSvc", m_sgSvc));
    CHECK( m_Tool_InformationStore.retrieve() );
    CHECK( m_Tool_TauConstituentGetter.retrieve() );
    CHECK( m_Tool_TauConstituentSelector.retrieve() );
    CHECK( m_Tool_FeatureExtractor.retrieve() );
    CHECK( m_Tool_DecayModeDeterminator.retrieve() );
    CHECK( m_Tool_DetailsArranger.retrieve() );
    
    CHECK( m_Tool_InformationStore->dumpMaps() );
    
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("ModeDiscriminator_BinEdges_Pt", m_Config_PtBins) );
    m_Config_MinPt = m_Config_PtBins.front();
    m_Config_MaxPt = m_Config_PtBins.back();
    
    int    enableXAODHacks = 0;
    CHECK (m_Tool_InformationStore->getInfo_Int("EnableXAODMigrationHacks", enableXAODHacks) );
    if(enableXAODHacks == 1) {
        ATH_MSG_WARNING("EnableXAODMigrationHacks is set to true! PanTau will run in xAOD EDM migration mode (limited functionality)!");
        ATH_MSG_WARNING("Once xAOD migration is finished, set that flag to false in PanTauAnalysis/Config_PanTau.py!");
    }
    
    return StatusCode::SUCCESS;
}//end of initialize



/// //////////////////////////////////////////////////////////
/// Finalize
/// //////////////////////////////////////////////////////////
StatusCode PanTau::Alg_SeedBuilder::finalize() {
    return StatusCode::SUCCESS;
}//end finalize




/// //////////////////////////////////////////////////////////
/// Execute
/// //////////////////////////////////////////////////////////
StatusCode      PanTau::Alg_SeedBuilder::execute() {
    
    ATH_MSG_DEBUG("===========================================================");
    ATH_MSG_DEBUG("===      PanTau::Alg_SeedBuilder --- Seed Creation      ===");
    ATH_MSG_DEBUG("===========================================================");
    ATH_MSG_DEBUG("Executing PanTau::Alg_SeedBuilder::execute() for input alg: " << m_Name_InputAlg);
    
    
    //! =======================================================================================
    //! Update information in InformationStore to reflect current event
    ATH_MSG_DEBUG("Update Informationstore");
    StatusCode sc = m_Tool_InformationStore->updateInformation(m_Name_InputAlg);
    if(sc == StatusCode::RECOVERABLE) {
        ATH_MSG_WARNING("Could not get all required containers from StoreGate -> skip this execution of PanTau");
        return StatusCode::SUCCESS;
    }
    
    
    //! =======================================================================================
    //! Get TauRec Container (input)
//     const Analysis::TauJetContainer*    l_Container_TauRec            = m_Tool_InformationStore->getContainer_TauRec();
    const xAOD::TauJetContainer*        l_Container_TauRec            = m_Tool_InformationStore->getContainer_TauRec();
    unsigned int                        l_Number_InputSeeds           = l_Container_TauRec->size();
    
    ATH_MSG_DEBUG("Recieved tauRec container with " << l_Number_InputSeeds << " entries at " << l_Container_TauRec);
    
    
    //! =======================================================================================
    //! Create  Output Containers
    PanTau::PanTauSeedContainer*   l_Container_PanTauSeeds  = new PanTauSeedContainer(SG::OWN_ELEMENTS);
    
    
    //! =======================================================================================
    //! Loop over taus
    for(unsigned int iTau=0; iTau<l_Number_InputSeeds; iTau++) {
        
        //get the current TauJet
        ATH_MSG_DEBUG("===> Tau " << iTau+1 << " / " << l_Number_InputSeeds << " will enter seed creation now");
//         Analysis::TauJet* curTauJet = const_cast<Analysis::TauJet*>(l_Container_TauRec->at(iTau));
        xAOD::TauJet* curTauJet = const_cast<xAOD::TauJet*>(l_Container_TauRec->at(iTau));
        
        //check for null pointer
        if(curTauJet == 0) {
            ATH_MSG_WARNING("Pointer to Analysis::TauJet in tauRec container is 0! -> Skip & continue with next");
            continue;
        }
        
        fillDefaultValuesToTau(curTauJet);
        
        //keep track of the technical quality of the pantau seed to be created
        std::vector<int> pantauSeed_TechnicalQuality = std::vector<int>((unsigned int)PanTau::PanTauSeed::t_nTechnicalQualities, 0);
        
        //check if the tau is valid for the given input alg:
        if(m_Name_InputAlg == "eflowRec") {
            int numTrack = curTauJet->nTracks();
            if(numTrack == 0 || numTrack > 5) {
                ATH_MSG_DEBUG("Flagging tau for algorithm " << m_Name_InputAlg << " as invalid, because numTrack is " << numTrack);
                pantauSeed_TechnicalQuality[(int)PanTau::PanTauSeed::t_NoValidInputTau] = 1;
            }
        }
        
        if(m_Name_InputAlg == "CellBased") {
            int numTrack = curTauJet->nTracks();
            if(numTrack == 0 || numTrack > 5) {
                ATH_MSG_DEBUG("Flagging tau for algorithm " << m_Name_InputAlg << " as invalid, because numTrack is " << numTrack);
                pantauSeed_TechnicalQuality[(int)PanTau::PanTauSeed::t_NoValidInputTau] = 1;
            }
        }
        
        
        // if there is substructure info available, get constituents, perform selection and recombination
        //these vectors will be owned by the PanTauSeed object.
        std::vector<TauConstituent*> l_List_TauConstituents = std::vector<TauConstituent*>(0);
        std::vector<TauConstituent*> l_List_SelectedTauConstituents = std::vector<TauConstituent*>(0);
        
        if(pantauSeed_TechnicalQuality[(int)PanTau::PanTauSeed::t_NoValidInputTau] == 0) {
            // Get the constituents for the current tau
            CHECK( m_Tool_TauConstituentGetter->GetTauConstituents(curTauJet, l_List_TauConstituents, m_Name_InputAlg) );
            if(l_List_TauConstituents.size() == 0)  {
                pantauSeed_TechnicalQuality[(int)PanTau::PanTauSeed::t_NoConstituentsAtAll] = 1;
                ATH_MSG_DEBUG("Seed has no associated constituents!");
            }
            
            
            // Call the TauConstituentSelector tool to throw out bad constituents
            CHECK(m_Tool_TauConstituentSelector->SelectTauConstituents(l_List_TauConstituents, l_List_SelectedTauConstituents) );
            if(l_List_SelectedTauConstituents.size() == 0) {
                pantauSeed_TechnicalQuality[(int)PanTau::PanTauSeed::t_NoSelectedConstituents] = 1;
                ATH_MSG_DEBUG("Seed has no associated constituents that pass selection!");
            }
            
            //check if there are core objects in the selected constituents
            bool hasCoreConstituents = false;
            for(unsigned int iConst=0; iConst<l_List_SelectedTauConstituents.size(); iConst++) {
                PanTau::TauConstituent* curConst = l_List_SelectedTauConstituents[iConst];
                if(curConst->isOfType(PanTau::TauConstituent::t_Charged) == true) {hasCoreConstituents = true; break;};
                if(curConst->isOfType(PanTau::TauConstituent::t_Neutral) == true) {hasCoreConstituents = true; break;};
                if(curConst->isOfType(PanTau::TauConstituent::t_Pi0Neut) == true) {hasCoreConstituents = true; break;};
            }
            
            if(hasCoreConstituents == false) {
                pantauSeed_TechnicalQuality[(int)PanTau::PanTauSeed::t_NoSelectedConstituents] = 1; //use this flag at the moment as a quick hack
            }
            
        }
        
        //to be consistent, taus without substructure algs run on them also have no constituents
        if(pantauSeed_TechnicalQuality[(int)PanTau::PanTauSeed::t_NoValidInputTau] == 1) {
            pantauSeed_TechnicalQuality[(int)PanTau::PanTauSeed::t_NoConstituentsAtAll] = 1;
            pantauSeed_TechnicalQuality[(int)PanTau::PanTauSeed::t_NoSelectedConstituents] = 1;
        }
        
        //check for the pT flag
        //NOTE: make sure to use correct 4-momentum version
        double tauJet_pT = curTauJet->ptIntermediateAxis();
        if(tauJet_pT < m_Config_MinPt || tauJet_pT > m_Config_MaxPt) {
            pantauSeed_TechnicalQuality[(int)PanTau::PanTauSeed::t_BadPtValue] = 1;
        }
        
        
        // Now build the PanTauSeed with the new Constituents
        PanTau::PanTauSeed* curPanTauSeed = new PanTau::PanTauSeed(m_Name_InputAlg,
                                                                   curTauJet,
                                                                   l_List_SelectedTauConstituents,
                                                                   l_List_TauConstituents, pantauSeed_TechnicalQuality);
        
        unsigned int nPi0LinksCellBased = curTauJet->cellBased_Pi0_PFOLinks().size();
        ATH_MSG_DEBUG("Created new PanTauSeed at " << curPanTauSeed << " with proto mode " << curPanTauSeed->getDecayModeBySubAlg() << " and nPi0 CellBased = " << nPi0LinksCellBased);
        
        // Get the features for this PanTauSeed
        ATH_MSG_DEBUG("Calculate features for this PanTauSeed");
        CHECK(m_Tool_FeatureExtractor->calculateFeatures(curPanTauSeed) );
        
        //debug output
        //ATH_MSG_VERBOSE("Dumping features of finalized PanTauSeed ==========================================");
        //curPanTauSeed->getFeatures()->dump(m_log, MSG::VERBOSE);
        //ATH_MSG_VERBOSE("Dumped features of finalized PanTauSeed ===========================================");
        
        //store in output container
        ATH_MSG_DEBUG("Saving valid PanTauSeed to seed container");
        l_Container_PanTauSeeds->push_back(curPanTauSeed);
        
        //link it in the source TauJet
//         curTauJet->addPanTauSeed(curPanTauSeed);
        
    }//end loop over taus
    
    ATH_MSG_DEBUG("Finished loop over input taus");
    
    
    
    //! =======================================================================================
    //! Finalize the seeds
    ATH_MSG_DEBUG("===========================================================");
    ATH_MSG_DEBUG("===    PanTau::Alg_SeedBuilder --- Seed Finalization    ===");
    ATH_MSG_DEBUG("===========================================================");
    
    //! Create the details container
    //! NOTE: Not needed in rel19+ -> details are added to xAOD::TauJet directly
//     std::string l_Name_PanTauDetailsContainer;
//     CHECK( m_Tool_InformationStore->getInfo_String("Name_PanTauDetailsContainer", l_Name_PanTauDetailsContainer) );
//     l_Name_PanTauDetailsContainer = l_Name_PanTauDetailsContainer + m_Name_InputAlg;
//     Analysis::TauDetailsContainer* l_Container_PanTauDetails = new Analysis::TauDetailsContainer();
//     CHECK( m_sgSvc->record(l_Container_PanTauDetails, l_Name_PanTauDetailsContainer) );
    
    // Seed finalizing:
    //  1. Pass the seed to the decay mode determination tool
    //  2. Pass the seed to the FourMomentumCalculator tool
    //  3. Add the details to tauJet
    unsigned int l_Number_PanTauSeeds = l_Container_PanTauSeeds->size();
    ATH_MSG_DEBUG("Begin loop over " << l_Number_PanTauSeeds << " PanTau seeds to finalize them");
    for(unsigned int iPanTau=0; iPanTau<l_Number_PanTauSeeds; iPanTau++) {
        ATH_MSG_DEBUG("===> PanTauSeed " << iPanTau+1 << " / " << l_Number_InputSeeds << " will be finalized now...");
        PanTau::PanTauSeed* curPanTauSeed       = l_Container_PanTauSeeds->at(iPanTau);
        
        // 1. call decay mode determinator for this seed
        ATH_MSG_DEBUG("calling decay mode determinator for valid seed ");
        CHECK( m_Tool_DecayModeDeterminator->determineDecayMode(curPanTauSeed) );
            
        // 2. call the FourMomentumCalculator for this seed
        ATH_MSG_DEBUG("calling four momentum calculator for valid seed ");
        CHECK( m_Tool_FourMomentumCalculator->calculateFourMomentum(curPanTauSeed) );
        
        // 3. Link the details to the tauJet
//         CHECK( m_Tool_DetailsArranger->arrangeDetails(curPanTauSeed, l_Container_PanTauDetails) );
        CHECK( m_Tool_DetailsArranger->arrangeDetails(curPanTauSeed) );
        
    }//end loop over output pantau seeds
    
//     CHECK( m_sgSvc->setConst( l_Container_PanTauDetails ) );
    
    //! =======================================================================================
    //! Store the output seeds
    
    //  NOTE: This container is not used at the moment... :)
//     ATH_MSG_DEBUG("Storing output for " << l_Container_TauRec->size() << " input seeds in two PanTauSeedContainers");
    
    // store the seeds
    // not needed in 19.0.1
    // NOTE: Need to rethink how to dump ALL development variables
    delete l_Container_PanTauSeeds;
    
//     std::string l_Name_ValidSeedContainer;
//     CHECK(m_Tool_InformationStore->getInfo_String("Name_PanTauSeedsContainer",     l_Name_ValidSeedContainer));
//     l_Name_ValidSeedContainer   = l_Name_ValidSeedContainer + m_Name_InputAlg;
//     CHECK( m_sgSvc->record(l_Container_PanTauSeeds,    l_Name_ValidSeedContainer,      false) );
//     ATH_MSG_DEBUG("Stored " << l_Number_PanTauSeeds << " PanTauSeeds");
    
    
    //that's it :)
    ATH_MSG_DEBUG("PanTau::Alg_SeedBuilder for input alg" << m_Name_InputAlg << " was successful!");
    return StatusCode::SUCCESS;
}//end of execute



void PanTau::Alg_SeedBuilder::fillDefaultValuesToTau(xAOD::TauJet* tauJet) {
    
    
    if(m_Name_InputAlg == "CellBased") {
        tauJet->setP4(xAOD::TauJetParameters::PanTauCellBasedProto, -1111., -1111., -1111., -1111.);
        tauJet->setP4(xAOD::TauJetParameters::PanTauCellBased,      -1111., -1111., -1111., -1111.);
        
        //
        //charged
        std::vector< ElementLink< xAOD::PFOContainer > > chrgPFOLinks = tauJet->cellBased_Charged_PFOLinks();
        tauJet->setCharged_PFOLinks(chrgPFOLinks);
        
        //pi0
        std::vector< ElementLink< xAOD::PFOContainer > > pi0PFOLinks = tauJet->cellBased_Pi0_PFOLinks();
        tauJet->setPi0_PFOLinks(pi0PFOLinks);
        
        //neutrals
        std::vector< ElementLink< xAOD::PFOContainer > > neutralPFOLinks = tauJet->cellBased_Neutral_PFOLinks();
        tauJet->setNeutral_PFOLinks(neutralPFOLinks);
    }
    
    
    if(m_Name_InputAlg == "eflowRec") {
        tauJet->setP4(xAOD::TauJetParameters::PanTauEFlowRecProto,  -1111., -1111., -1111., -1111.);
        tauJet->setP4(xAOD::TauJetParameters::PanTauEFlowRec,       -1111., -1111., -1111., -1111.);
    }
    
    return;
}

















