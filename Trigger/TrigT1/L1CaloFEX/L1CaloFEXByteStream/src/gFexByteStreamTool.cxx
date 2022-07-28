/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           gFexByteStreamTool  -  description
//                              -------------------
//     begin                : 20 07 2022
//     email                : cecilia.tosciri@cern.ch
//  ***************************************************************************/

#include "gFexByteStreamTool.h"
#include "gFexPos.h"
#include "CxxUtils/span.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/Status.h"

using ROBF = OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
using WROBF = OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment;

namespace gPos = LVL1::gFEXPos;

gFexByteStreamTool::gFexByteStreamTool(const std::string& type,
        const std::string& name,
        const IInterface* parent)
    : base_class(type, name, parent) {}

StatusCode gFexByteStreamTool::initialize() {
    // Conversion mode for gRho TOBs
    ATH_MSG_DEBUG(" ROB IDs: " << MSG::hex << m_robIds.value() << MSG::dec);

    ConversionMode gRhomode = getConversionMode(m_gFexRhoReadKey, m_gFexRhoWriteKey, msg());
    ATH_CHECK(gRhomode!=ConversionMode::Undefined);
    ATH_CHECK(m_gFexRhoWriteKey.initialize(gRhomode==ConversionMode::Decoding));
    ATH_CHECK(m_gFexRhoReadKey.initialize(gRhomode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gRhomode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gRho ");

    // Conversion mode for gSJ TOBs
    ConversionMode gSJmode = getConversionMode(m_gFexBlockReadKey, m_gFexBlockWriteKey, msg());
    ATH_CHECK(gSJmode!=ConversionMode::Undefined);
    ATH_CHECK(m_gFexBlockWriteKey.initialize(gSJmode==ConversionMode::Decoding));
    ATH_CHECK(m_gFexBlockReadKey.initialize(gSJmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gSJmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gSJ ");

    // Conversion mode for gLJ TOBs
    ConversionMode gLJmode = getConversionMode(m_gFexJetReadKey, m_gFexJetWriteKey, msg());
    ATH_CHECK(gLJmode!=ConversionMode::Undefined);
    ATH_CHECK(m_gFexJetWriteKey.initialize(gLJmode==ConversionMode::Decoding));
    ATH_CHECK(m_gFexJetReadKey.initialize(gLJmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gLJmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gLJ ");

    // Conversion mode for gScalarEJwoj TOBs
    ConversionMode gScalarEJwojmode = getConversionMode(m_gScalarEJwojReadKey, m_gScalarEJwojWriteKey, msg());
    ATH_CHECK(gScalarEJwojmode!=ConversionMode::Undefined);
    ATH_CHECK(m_gScalarEJwojWriteKey.initialize(gScalarEJwojmode==ConversionMode::Decoding));
    ATH_CHECK(m_gScalarEJwojReadKey.initialize(gScalarEJwojmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gScalarEJwojmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gScalarEJwoj ");

    // Conversion mode for gMETComponentsJwoj TOBs
    ConversionMode gMETComponentsJwojmode = getConversionMode(m_gMETComponentsJwojReadKey, m_gMETComponentsJwojWriteKey, msg());
    ATH_CHECK(gMETComponentsJwojmode!=ConversionMode::Undefined);
    ATH_CHECK(m_gMETComponentsJwojWriteKey.initialize(gMETComponentsJwojmode==ConversionMode::Decoding));
    ATH_CHECK(m_gMETComponentsJwojReadKey.initialize(gMETComponentsJwojmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gMETComponentsJwojmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gMETComponentsJwoj ");

    // Conversion mode for gMHTComponentsJwoj TOBs
    ConversionMode gMHTComponentsJwojmode = getConversionMode(m_gMHTComponentsJwojReadKey, m_gMHTComponentsJwojWriteKey, msg());
    ATH_CHECK(gMHTComponentsJwojmode!=ConversionMode::Undefined);
    ATH_CHECK(m_gMHTComponentsJwojWriteKey.initialize(gMHTComponentsJwojmode==ConversionMode::Decoding));
    ATH_CHECK(m_gMHTComponentsJwojReadKey.initialize(gMHTComponentsJwojmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gMHTComponentsJwojmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gMHTComponentsJwoj ");

   // Conversion mode for gMSTComponentsJwoj TOBs
    ConversionMode gMSTComponentsJwojmode = getConversionMode(m_gMSTComponentsJwojReadKey, m_gMSTComponentsJwojWriteKey, msg());
    ATH_CHECK(gMSTComponentsJwojmode!=ConversionMode::Undefined);
    ATH_CHECK(m_gMSTComponentsJwojWriteKey.initialize(gMSTComponentsJwojmode==ConversionMode::Decoding));
    ATH_CHECK(m_gMSTComponentsJwojReadKey.initialize(gMSTComponentsJwojmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gMSTComponentsJwojmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gMSTComponentsJwoj ");

    // Conversion mode for gMETComponentsNoiseCut TOBs
    ConversionMode gMETComponentsNoiseCutmode = getConversionMode(m_gMETComponentsNoiseCutReadKey, m_gMETComponentsNoiseCutWriteKey, msg());
    ATH_CHECK(gMETComponentsNoiseCutmode!=ConversionMode::Undefined);
    ATH_CHECK(m_gMETComponentsNoiseCutWriteKey.initialize(gMETComponentsNoiseCutmode==ConversionMode::Decoding));
    ATH_CHECK(m_gMETComponentsNoiseCutReadKey.initialize(gMETComponentsNoiseCutmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gMETComponentsNoiseCutmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gMETComponentsNoiseCut ");

    // Conversion mode for gMETRms TOBs
    ConversionMode gMETComponentsRmsmode = getConversionMode(m_gMETComponentsRmsReadKey, m_gMETComponentsRmsWriteKey, msg());
    ATH_CHECK(gMETComponentsRmsmode!=ConversionMode::Undefined);
    ATH_CHECK(m_gMETComponentsRmsWriteKey.initialize(gMETComponentsRmsmode==ConversionMode::Decoding));
    ATH_CHECK(m_gMETComponentsRmsReadKey.initialize(gMETComponentsRmsmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gMETComponentsRmsmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gMETComponentsRms ");
 
    // Conversion mode for gScalarENoiseCut TOBs
    ConversionMode gScalarENoiseCutmode = getConversionMode(m_gScalarENoiseCutReadKey, m_gScalarENoiseCutWriteKey, msg());
    ATH_CHECK(gScalarENoiseCutmode!=ConversionMode::Undefined);
    ATH_CHECK(m_gScalarENoiseCutWriteKey.initialize(gScalarENoiseCutmode==ConversionMode::Decoding));
    ATH_CHECK(m_gScalarENoiseCutReadKey.initialize(gScalarENoiseCutmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gScalarENoiseCutmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gScalarENoiseCut ");

    // Conversion mode for gScalarERms TOBs
    ConversionMode gScalarERmsmode = getConversionMode(m_gScalarERmsReadKey, m_gScalarERmsWriteKey, msg());
    ATH_CHECK(gScalarERmsmode!=ConversionMode::Undefined);
    ATH_CHECK(m_gScalarERmsWriteKey.initialize(gScalarERmsmode==ConversionMode::Decoding));
    ATH_CHECK(m_gScalarERmsReadKey.initialize(gScalarERmsmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((gScalarERmsmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " gScalarERms ");

    //checking all Conversion modes.. avoid misconfigurations
    const std::array<ConversionMode,2> modes{gSJmode,gLJmode};
    if (std::any_of(modes.begin(),modes.end(),[&gRhomode](ConversionMode m) { return m!=gRhomode;  } )) {
        ATH_MSG_ERROR("Inconsistent conversion modes");
        return StatusCode::FAILURE;
    }
    
    ATH_CHECK(m_l1MenuKey.initialize());
    
    
    return StatusCode::SUCCESS;
}

// BS->xAOD conversion
StatusCode gFexByteStreamTool::convertFromBS(const std::vector<const ROBF*>& vrobf, const EventContext& ctx) const {
        
    //WriteHandle for gFEX EDMs
    
    //---Rho Container
    SG::WriteHandle<xAOD::gFexJetRoIContainer> gRhoContainer(m_gFexRhoWriteKey, ctx);
    ATH_CHECK(gRhoContainer.record(std::make_unique<xAOD::gFexJetRoIContainer>(), std::make_unique<xAOD::gFexJetRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetRoIContainer with key " << gRhoContainer.key());
    
    //---Small Jets Container
    SG::WriteHandle<xAOD::gFexJetRoIContainer> gSJContainer(m_gFexBlockWriteKey, ctx);
    ATH_CHECK(gSJContainer.record(std::make_unique<xAOD::gFexJetRoIContainer>(), std::make_unique<xAOD::gFexJetRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetRoIContainer with key " << gSJContainer.key());

    //---Large Jets Container
    SG::WriteHandle<xAOD::gFexJetRoIContainer> gLJContainer(m_gFexJetWriteKey, ctx);
    ATH_CHECK(gLJContainer.record(std::make_unique<xAOD::gFexJetRoIContainer>(), std::make_unique<xAOD::gFexJetRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetRoIContainer with key " << gLJContainer.key());

    //---Scalar MET and SumET JwoJ Container
    SG::WriteHandle<xAOD::gFexGlobalRoIContainer> gScalarEJwojContainer(m_gScalarEJwojWriteKey, ctx);
    ATH_CHECK(gScalarEJwojContainer.record(std::make_unique<xAOD::gFexGlobalRoIContainer>(), std::make_unique<xAOD::gFexGlobalRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetGlobalContainer with key " << gScalarEJwojContainer.key());

    //---MET Components JwoJ Container
    SG::WriteHandle<xAOD::gFexGlobalRoIContainer> gMETComponentsJwojContainer(m_gMETComponentsJwojWriteKey, ctx);
    ATH_CHECK(gMETComponentsJwojContainer.record(std::make_unique<xAOD::gFexGlobalRoIContainer>(), std::make_unique<xAOD::gFexGlobalRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetGlobalContainer with key " << gMETComponentsJwojContainer.key());

    //---MHT Components JwoJ Container
    SG::WriteHandle<xAOD::gFexGlobalRoIContainer> gMHTComponentsJwojContainer(m_gMHTComponentsJwojWriteKey, ctx);
    ATH_CHECK(gMHTComponentsJwojContainer.record(std::make_unique<xAOD::gFexGlobalRoIContainer>(), std::make_unique<xAOD::gFexGlobalRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetGlobalContainer with key " << gMHTComponentsJwojContainer.key());

    //---MST Components JwoJ Container
    SG::WriteHandle<xAOD::gFexGlobalRoIContainer> gMSTComponentsJwojContainer(m_gMSTComponentsJwojWriteKey, ctx);
    ATH_CHECK(gMSTComponentsJwojContainer.record(std::make_unique<xAOD::gFexGlobalRoIContainer>(), std::make_unique<xAOD::gFexGlobalRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetGlobalContainer with key " << gMSTComponentsJwojContainer.key());

    //---MET Components NoiseCut Container
    SG::WriteHandle<xAOD::gFexGlobalRoIContainer> gMETComponentsNoiseCutContainer(m_gMETComponentsNoiseCutWriteKey, ctx);
    ATH_CHECK(gMETComponentsNoiseCutContainer.record(std::make_unique<xAOD::gFexGlobalRoIContainer>(), std::make_unique<xAOD::gFexGlobalRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetGlobalContainer with key " << gMETComponentsNoiseCutContainer.key());

    //---MET Components Rms Container
    SG::WriteHandle<xAOD::gFexGlobalRoIContainer> gMETComponentsRmsContainer(m_gMETComponentsRmsWriteKey, ctx);
    ATH_CHECK(gMETComponentsRmsContainer.record(std::make_unique<xAOD::gFexGlobalRoIContainer>(), std::make_unique<xAOD::gFexGlobalRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetGlobalContainer with key " << gMETComponentsRmsContainer.key());

    //---Scalar MET and SumET NoiseCut Container
    SG::WriteHandle<xAOD::gFexGlobalRoIContainer> gScalarENoiseCutContainer(m_gScalarENoiseCutWriteKey, ctx);
    ATH_CHECK(gScalarENoiseCutContainer.record(std::make_unique<xAOD::gFexGlobalRoIContainer>(), std::make_unique<xAOD::gFexGlobalRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetGlobalContainer with key " << gScalarENoiseCutContainer.key());

    //---Scalar MET and SumET Rms Container
    SG::WriteHandle<xAOD::gFexGlobalRoIContainer> gScalarERmsContainer(m_gScalarERmsWriteKey, ctx);
    ATH_CHECK(gScalarERmsContainer.record(std::make_unique<xAOD::gFexGlobalRoIContainer>(), std::make_unique<xAOD::gFexGlobalRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded gFexJetGlobalContainer with key " << gScalarERmsContainer.key());


    // Retrieve the L1 menu configuration
    SG::ReadHandle<TrigConf::L1Menu> l1Menu (m_l1MenuKey,ctx);
    ATH_CHECK(l1Menu.isValid());

    auto & l1Menu_gJ = l1Menu->thrExtraInfo().gJ();
    auto & l1Menu_gLJ = l1Menu->thrExtraInfo().gLJ();
    auto & l1Menu_gXE = l1Menu->thrExtraInfo().gXE();
    auto & l1Menu_gTE = l1Menu->thrExtraInfo().gTE();

    int gJ_scale = 0;
    int gLJ_scale = 0;
    int gXE_scale = 0;
    int gTE_scale = 0;
    gJ_scale = l1Menu_gJ.resolutionMeV(); 
    gLJ_scale = l1Menu_gLJ.resolutionMeV(); 
    gXE_scale = l1Menu_gXE.resolutionMeV(); 
    gTE_scale = l1Menu_gTE.resolutionMeV();


    // Iterate over ROBFragments to decode
    for (const ROBF* rob : vrobf) {
        // Iterate over ROD words and decode
        
        
        ATH_MSG_DEBUG("Starting to decode " << rob->rod_ndata() << " ROD words from ROB 0x" << std::hex << rob->rob_source_id());
        
        //There is no data to decode.. not even the ROD trailers
        if(rob->rod_ndata() <= 0){
            continue;
        }
        
        const auto dataArray = CxxUtils::span{rob->rod_data(), rob->rod_ndata()};

        
        // Starting to loop over the gFEX words

        unsigned int n_words = rob->rod_ndata();

        //saving Jet TOBs into the EDM container
        for(unsigned int iWord=0; iWord<n_words/2; iWord++) {
            ATH_MSG_DEBUG("Raw word  0x" << std::hex << dataArray[iWord]  << "    " << std::bitset<32> (dataArray[iWord]));
            //Skipping the start 
            for (unsigned int iStartJetTob=0; iStartJetTob< gPos::START_JET_TOB.size(); iStartJetTob++){

                if(iWord == gPos::START_JET_TOB.at(iStartJetTob)){
                    continue;
                }
           
                //Skipping the unused words
                for (unsigned int iJetUnusedPos=0; iJetUnusedPos< gPos::JET_UNUSED_POSITION.size(); iJetUnusedPos++){
                    if (iWord == gPos::START_JET_TOB.at(iStartJetTob) + gPos::JET_UNUSED_POSITION.at(iJetUnusedPos) ){
                        continue;
                    }
                }  

                //Skipping the trailer words
                for (unsigned int itrailerPos=0; itrailerPos< gPos::TRAILER_POSITION.size(); itrailerPos++){
                    if (iWord == gPos::START_JET_TOB.at(iStartJetTob) + gPos::TRAILER_POSITION.at(itrailerPos) ){
                        continue;
                    }
                }  

                //saving gRho TOBs into the EDM container
                if (iWord == gPos::START_JET_TOB.at(iStartJetTob) + gPos::GRHO_POSITION){
                    std::unique_ptr<xAOD::gFexJetRoI> myEDM (new xAOD::gFexJetRoI());
                    gRhoContainer->push_back(std::move(myEDM));
                    gRhoContainer->back()->initialize(dataArray[iWord], gJ_scale);
                }  

                //saving gBlock TOBs into the EDM container
                for (unsigned int iGblockPos=0; iGblockPos< gPos::GBLOCK_POSITION.size(); iGblockPos++){
                    if (iWord == gPos::START_JET_TOB.at(iStartJetTob) + gPos::GBLOCK_POSITION.at(iGblockPos) ){
                        std::unique_ptr<xAOD::gFexJetRoI> myEDM (new xAOD::gFexJetRoI());
                        gSJContainer->push_back(std::move(myEDM));
                        gSJContainer->back()->initialize(dataArray[iWord], gJ_scale);
                    }
                }  

                //saving gJet TOBs into the EDM container
                for (unsigned int iGjetPos=0; iGjetPos< gPos::GJET_POSITION.size(); iGjetPos++){
                    if (iWord == gPos::START_JET_TOB.at(iStartJetTob) + gPos::GJET_POSITION.at(iGjetPos) ){
                        std::unique_ptr<xAOD::gFexJetRoI> myEDM (new xAOD::gFexJetRoI());
                        gLJContainer->push_back(std::move(myEDM));
                        gLJContainer->back()->initialize(dataArray[iWord], gLJ_scale);
                    }
                }  
            
            }

        }

        //saving Global TOBs into the EDM container
        for(unsigned int iWord=n_words/2; iWord<n_words; iWord++) {
            ATH_MSG_DEBUG("Raw word  0x" << std::hex << dataArray[iWord]  << "    " << std::bitset<32> (dataArray[iWord]));
            //Skipping the start 
            for (unsigned int iStartGlobalTob=0; iStartGlobalTob< gPos::START_GLOBAL_TOB.size(); iStartGlobalTob++){

                if(iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob)){

                    continue;
                }
           
                //Skipping the unused words
                for (unsigned int iGlobalUnusedPos=0; iGlobalUnusedPos< gPos::GLOBAL_UNUSED_POSITION.size(); iGlobalUnusedPos++){
                    if (iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob) + gPos::GLOBAL_UNUSED_POSITION.at(iGlobalUnusedPos) ){
                        continue;
                    }
                }  

                //Skipping the trailer words
                for (unsigned int itrailerPos=0; itrailerPos< gPos::TRAILER_POSITION.size(); itrailerPos++){
                    if (iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob) + gPos::TRAILER_POSITION.at(itrailerPos) ){
                        continue;
                    }
                }  

                //saving jwoj MHT TOBs into the EDM container
                
                if (iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob) + gPos::JWOJ_MHT_POSITION) {
                    std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
                    gMHTComponentsJwojContainer->push_back(std::move(myEDM));
                    gMHTComponentsJwojContainer->back()->initialize(dataArray[iWord], gXE_scale, gTE_scale);
                }
                  

                //saving jwoj MST TOBs into the EDM container
                if (iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob) + gPos::JWOJ_MST_POSITION){
                    std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
                    gMSTComponentsJwojContainer->push_back(std::move(myEDM));
                    gMSTComponentsJwojContainer->back()->initialize(dataArray[iWord], gXE_scale, gTE_scale);
                }
                  

                //saving jwoj MET TOBs into the EDM container
                if (iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob) + gPos::JWOJ_MET_POSITION){
                    std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
                    gMETComponentsJwojContainer->push_back(std::move(myEDM));
                    gMETComponentsJwojContainer->back()->initialize(dataArray[iWord], gXE_scale, gTE_scale);
                }
            
                //saving jwoj Scalar TOBs into the EDM container
                if (iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob) + gPos::JWOJ_SCALAR_POSITION){
                    std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
                    gScalarEJwojContainer->push_back(std::move(myEDM));
                    gScalarEJwojContainer->back()->initialize(dataArray[iWord], gXE_scale, gTE_scale);
                }                   

                //saving Noise Cut MET TOBs into the EDM container
                if (iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob) + gPos::NC_MET_POSITION){
                    std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
                    gMETComponentsNoiseCutContainer->push_back(std::move(myEDM));
                    gMETComponentsNoiseCutContainer->back()->initialize(dataArray[iWord], gXE_scale, gTE_scale);
                } 

                //saving Noise Cut Scalar TOBs into the EDM container
                if (iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob) + gPos::NC_SCALAR_POSITION){
                    std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
                    gScalarENoiseCutContainer->push_back(std::move(myEDM));
                    gScalarENoiseCutContainer->back()->initialize(dataArray[iWord], gXE_scale, gTE_scale);
                }  

                //saving Rho+RMS MET TOBs into the EDM container
                if (iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob) + gPos::RMS_MET_POSITION){
                    std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
                    gMETComponentsRmsContainer->push_back(std::move(myEDM));
                    gMETComponentsRmsContainer->back()->initialize(dataArray[iWord], gXE_scale, gTE_scale);
                } 

                //saving Rho+RMS Scalar TOBs into the EDM container
                if (iWord == gPos::START_GLOBAL_TOB.at(iStartGlobalTob) + gPos::RMS_SCALAR_POSITION){
                    std::unique_ptr<xAOD::gFexGlobalRoI> myEDM (new xAOD::gFexGlobalRoI());
                    gScalarERmsContainer->push_back(std::move(myEDM));
                    gScalarERmsContainer->back()->initialize(dataArray[iWord], gXE_scale, gTE_scale);
                }  
            }

        }

    }
        
    return StatusCode::SUCCESS;
}


/// xAOD->BS conversion
StatusCode gFexByteStreamTool::convertToBS(std::vector<WROBF*>& /*vrobf*/, const EventContext& /*eventContext*/) {

    return StatusCode::SUCCESS;
    
}

