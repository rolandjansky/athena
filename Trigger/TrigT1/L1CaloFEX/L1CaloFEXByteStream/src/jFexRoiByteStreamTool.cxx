/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFexRoiByteStreamTool  -  This tool decodes Run3 jFEX RoI data!
//                              -------------------
//     begin                : 01 01 2022
//     email                : Sergi.Rodriguez@cern.ch
//  ***************************************************************************/

#include "jFexRoiByteStreamTool.h"
#include "jFexBits.h"
#include "CxxUtils/span.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/Status.h"

using ROBF = OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
using WROBF = OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment;

namespace jBits = LVL1::jFEXBits;

struct color {
    std::string RED      ="\033[1;31m";
    std::string ORANGE   ="\033[1;38;5;208m";
    std::string YELLOW   ="\033[1;33m";
    std::string GREEN    ="\033[1;32m";
    std::string BLUE     ="\033[1;34m";
    std::string PURPLE   ="\033[1;35m";
    std::string END      ="\033[0m";
    std::string B_BLUE   ="\033[1;44m";
    std::string B_PURPLE ="\033[1;45m";
    std::string B_ORANGE ="\033[1;48;5;208;30m";
    std::string B_GRAY   ="\033[1;100m";
    std::string B_RED    ="\033[1;41m";
    std::string B_GREEN  ="\033[1;42m";
} const C;

jFexRoiByteStreamTool::jFexRoiByteStreamTool(const std::string& type,
        const std::string& name,
        const IInterface* parent)
    : base_class(type, name, parent) {}

StatusCode jFexRoiByteStreamTool::initialize() {
    // Conversion mode for jJ TOBs
    ConversionMode jJmode = getConversionMode(m_jJReadKey, m_jJWriteKey, msg());
    ATH_CHECK(jJmode!=ConversionMode::Undefined);
    ATH_CHECK(m_jJWriteKey.initialize(jJmode==ConversionMode::Decoding));
    ATH_CHECK(m_jJReadKey.initialize(jJmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((jJmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " jJ ROB IDs: "
                  << MSG::hex << m_robIds.value() << MSG::dec);

    // Conversion mode for jLJ TOBs
    ConversionMode jLJmode = getConversionMode(m_jLJReadKey, m_jLJWriteKey, msg());
    ATH_CHECK(jLJmode!=ConversionMode::Undefined);
    ATH_CHECK(m_jLJWriteKey.initialize(jLJmode==ConversionMode::Decoding));
    ATH_CHECK(m_jLJReadKey.initialize(jLJmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((jLJmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " jLJ ROB IDs: "
                  << MSG::hex << m_robIds.value() << MSG::dec);

    // Conversion mode for jTau TOBs
    ConversionMode jTaumode = getConversionMode(m_jTauReadKey, m_jTauWriteKey, msg());
    ATH_CHECK(jTaumode!=ConversionMode::Undefined);
    ATH_CHECK(m_jTauWriteKey.initialize(jTaumode==ConversionMode::Decoding));
    ATH_CHECK(m_jTauReadKey.initialize(jTaumode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((jTaumode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " jTau ROB IDs: "
                  << MSG::hex << m_robIds.value() << MSG::dec);

    // Conversion mode for jEM TOBs
    ConversionMode jEMmode = getConversionMode(m_jEMReadKey, m_jEMWriteKey, msg());
    ATH_CHECK(jEMmode!=ConversionMode::Undefined);
    ATH_CHECK(m_jEMWriteKey.initialize(jEMmode==ConversionMode::Decoding));
    ATH_CHECK(m_jEMReadKey.initialize(jEMmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((jEMmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " jEM ROB IDs: "
                  << MSG::hex << m_robIds.value() << MSG::dec);

    // Conversion mode for jTE TOBs
    ConversionMode jTEmode = getConversionMode(m_jTEReadKey, m_jTEWriteKey, msg());
    ATH_CHECK(jTEmode!=ConversionMode::Undefined);
    ATH_CHECK(m_jTEWriteKey.initialize(jTEmode==ConversionMode::Decoding));
    ATH_CHECK(m_jTEReadKey.initialize(jTEmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((jTEmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " jTE ROB IDs: "
                  << MSG::hex << m_robIds.value() << MSG::dec);

    // Conversion mode for jXE TOBs
    ConversionMode jXEmode = getConversionMode(m_jXEReadKey, m_jXEWriteKey, msg());
    ATH_CHECK(jXEmode!=ConversionMode::Undefined);
    ATH_CHECK(m_jXEWriteKey.initialize(jXEmode==ConversionMode::Decoding));
    ATH_CHECK(m_jXEReadKey.initialize(jXEmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((jXEmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " jXE ROB IDs: "
                  << MSG::hex << m_robIds.value() << MSG::dec);
    
    //checking all Conversion modes.. avoid misconfigurations
    const std::array<ConversionMode,5> modes{jLJmode,jTaumode,jEMmode,jTEmode,jXEmode};
    if (std::any_of(modes.begin(),modes.end(),[&jJmode](ConversionMode m) { return m!=jJmode;  } )) {
        ATH_MSG_ERROR("Inconsistent conversion modes");
        return StatusCode::FAILURE;
    }
    
    
    
    return StatusCode::SUCCESS;
}

// BS->xAOD conversion
StatusCode jFexRoiByteStreamTool::convertFromBS(const std::vector<const ROBF*>& vrobf, const EventContext& ctx) const {
    
    //std::cout<<C.RED<<"SERGI"<<C.END<<std::endl;
    
    //WriteHandle for jFEX EDMs
    
    //---SRJet EDM
    SG::WriteHandle<xAOD::jFexSRJetRoIContainer> jJContainer(m_jJWriteKey, ctx);
    ATH_CHECK(jJContainer.record(std::make_unique<xAOD::jFexSRJetRoIContainer>(), std::make_unique<xAOD::jFexSRJetRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded jFexSRJetRoIContainer with key " << jJContainer.key());
    
    //---LRJet EDM
    SG::WriteHandle<xAOD::jFexLRJetRoIContainer> jLJContainer(m_jLJWriteKey, ctx);
    ATH_CHECK(jLJContainer.record(std::make_unique<xAOD::jFexLRJetRoIContainer>(), std::make_unique<xAOD::jFexLRJetRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded jFexLRJetRoIContainer with key " << jLJContainer.key());    
    
    //---Tau EDM
    SG::WriteHandle<xAOD::jFexTauRoIContainer> jTauContainer(m_jTauWriteKey, ctx);
    ATH_CHECK(jTauContainer.record(std::make_unique<xAOD::jFexTauRoIContainer>(), std::make_unique<xAOD::jFexTauRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded jFexTauRoIContainer with key " << jTauContainer.key());      
    
    //---EM EDM
    SG::WriteHandle<xAOD::jFexFwdElRoIContainer> jEMContainer(m_jEMWriteKey, ctx);
    ATH_CHECK(jEMContainer.record(std::make_unique<xAOD::jFexFwdElRoIContainer>(), std::make_unique<xAOD::jFexFwdElRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded jFexFwdElRoIContainer with key " << jEMContainer.key());      
    
    //---SumET EDM
    SG::WriteHandle<xAOD::jFexSumETRoIContainer> jTEContainer(m_jTEWriteKey, ctx);
    ATH_CHECK(jTEContainer.record(std::make_unique<xAOD::jFexSumETRoIContainer>(), std::make_unique<xAOD::jFexSumETRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded jFexSumETRoIContainer with key " << jTEContainer.key());      
    
    //---MET EDM 
    SG::WriteHandle<xAOD::jFexMETRoIContainer> jXEContainer(m_jXEWriteKey, ctx);
    ATH_CHECK(jXEContainer.record(std::make_unique<xAOD::jFexMETRoIContainer>(), std::make_unique<xAOD::jFexMETRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded jFexMETRoIContainer with key " << jXEContainer.key());     
    

    // Iterate over ROBFragments to decode
    for (const ROBF* rob : vrobf) {
        // Iterate over ROD words and decode
        
        
        ATH_MSG_DEBUG("Starting to decode " << rob->rod_ndata() << " ROD words from ROB 0x" << std::hex << rob->rob_source_id() << std::dec);
        
        //There is no data to decode.. not even the ROD trailers
        if(rob->rod_ndata() <= 0){
            //printf("ERROR No ROD words read. rob->rod_ndata() =%3d  \n",rob->rod_ndata());
            continue;
        }
        
        const auto dataArray = CxxUtils::span{rob->rod_data(), rob->rod_ndata()};
        std::vector<uint32_t> vec_words(dataArray.begin(),dataArray.end());

/*
        int aux = 1; //delete, for debug
        for (const uint32_t word : vec_words) {

            printf("\t %3d  raw word: 0x%08x \t bits: %32s\n",aux,word, (std::bitset<32>(word).to_string()).c_str() );
            aux++; //delete, for debug

        }
*/
        
        // Starting to loop over the different jFEX blocks
        bool READ_TOBS = true;
        // jFEX to ROD trailer position after the ROD trailer
        unsigned int trailers_pos = rob->rod_ndata() - jBits::ROD_WORDS;
        
        while(READ_TOBS){
            
            //printf("----------------------------------------------------------------------------------------------------------------------\n");
            
            const auto [payload, fpga, jfex]                      = jFEXtoRODTrailer  ( vec_words.at(trailers_pos-2), vec_words.at(trailers_pos-1) );
            const auto [n_xjJ, n_xjLJ, n_xjTau, n_xjEM]           = xTOBCounterTrailer( vec_words.at(trailers_pos-3) );    
            const auto [n_jJ, n_jLJ, n_jTau, n_jEM, n_jTE, n_jXE] = TOBCounterTrailer ( vec_words.at(trailers_pos-4) );
            unsigned int n_tobs  = n_jJ  + n_jLJ  + n_jTau  + n_jEM  + n_jTE + n_jXE;
            unsigned int n_xtobs = n_xjJ + n_xjLJ + n_xjTau + n_xjEM;
            unsigned int total_tobs = n_tobs+n_xtobs;
            //printf("----------> trailer_pos:%3d payload:%3d N.Words:%3d fpga:%3d  jfex:%3d\n",trailers_pos,payload,total_tobs+jBits::TOB_TRAILERS,fpga,jfex);
            //printf(" TOBs(0x%08x):   jJ:%3d   jLJ:%3d   jTau:%3d   jEM:%3d   jTE:%3d   jXE:%3d\n",vec_words.at(trailers_pos-4),n_jJ,n_jLJ,n_jTau,n_jEM,n_jTE,n_jXE);  
            //printf("xTOBs(0x%08x):  xjJ:%3d  xjLJ:%3d  xjTau:%3d  xjEM:%3d\n",vec_words.at(trailers_pos-3),n_xjJ,n_xjLJ,n_xjTau,n_xjEM);            
            
            if(payload != (total_tobs + jBits::TOB_TRAILERS)){
                //printf("%s !! ERROR Payload=%-4d is different from TOBs+Trailers=%-4d -> SKIPPED %s\n",C.RED.c_str(),payload,(total_tobs + jBits::TOB_TRAILERS),C.END.c_str());
                trailers_pos -= (payload+2);
                if(trailers_pos == 0) {
                    READ_TOBS = false;
                }
                continue;
            }
            
            //The minimum number for the payload should be 2 (The TOB/xTOB counters). If lower send an error message
            if(payload < jBits::TOB_TRAILERS){
                //printf("ERROR Payload is lower than the TOB/xTOB trailers. (Payload = %3d < %3d = TOB Trailer) \n",payload,jBits::TOB_TRAILERS);
                break;
            }
            
            
            //There can be a padding word, even number in the payload, but odd number of xTOB and TOB.. need to check and remove the extra word.
            unsigned int paddingWord = 0;
            if(total_tobs % 2){
                //printf("Odd number of TOBs + xTOBs: %4d, there is a padding word! CAREFUL!\n",total_tobs);
                paddingWord = 1;
            }
            
            //removing jFEX to ROD, TOB and xTOB Trailers from trailers_pos (4 positions), possible padding word added on the data to get even number of 32bit words
            unsigned int tobIndex = trailers_pos - (jBits::jFEX2ROD_WORDS + jBits::TOB_TRAILERS + paddingWord);


            if(m_convertExtendedTOBs) {
                /************************************************** DECODING xTOBS **************************************************/
                
                //saving xjEM into the EDM container
                for(unsigned int i=tobIndex; i>tobIndex-n_xjEM; i--) {
                    jEMContainer->push_back( std::make_unique<xAOD::jFexFwdElRoI>() );
                    jEMContainer->back()->initialize(jfex, fpga, vec_words.at(i-1),0, 200, -99, -99);
                }
                //removing xjEM counter from TOBs
                tobIndex -= n_xjEM;

                //saving xjTau into the EDM container
                for(unsigned int i=tobIndex; i>tobIndex-n_xjTau; i--) {
                    jTauContainer->push_back( std::make_unique<xAOD::jFexTauRoI>() );
                    jTauContainer->back()->initialize(jfex, fpga, vec_words.at(i-1),0, 200, -99, -99);
                }
                //removing xjTau counter from TOBs
                tobIndex -= n_xjTau;

                //saving xjLJ into the EDM container
                for(unsigned int i=tobIndex; i>tobIndex-n_xjLJ; i--) {
                    jLJContainer->push_back( std::make_unique<xAOD::jFexLRJetRoI>() );
                    jLJContainer->back()->initialize(jfex, fpga, vec_words.at(i-1),0, 200, -99, -99);
                }
                //removing xjLJ counter from TOBs
                tobIndex -= n_xjLJ;

                //saving xjJ into the EDM container
                for(unsigned int i=tobIndex; i>tobIndex-n_xjJ; i--) {
                    jJContainer->push_back( std::make_unique<xAOD::jFexSRJetRoI>() );
                    jJContainer->back()->initialize(jfex, fpga, vec_words.at(i-1),0, 200, -99, -99);
                }
                //removing xjJ counter from TOBs
                tobIndex -= n_xjJ;
                
                // skipping TOBs to jump to the next set of data
                tobIndex -= n_tobs;

            }
            else {
                /************************************************** DECODING TOBS **************************************************/
                
                // skipping extended-TOBs
                tobIndex -= n_xtobs;
                
                //saving jXE into the EDM container
                for(unsigned int i=tobIndex; i>tobIndex-n_jXE; i--) {
                    jXEContainer->push_back( std::make_unique<xAOD::jFexMETRoI>() );
                    jXEContainer->back()->initialize(jfex, fpga, vec_words.at(i-1), 200);
                }
                //removing jXE counter from TOBs
                tobIndex -= n_jXE;

                //saving jTE into the EDM container
                for(unsigned int i=tobIndex; i>tobIndex-n_jTE; i--) {
                    jTEContainer->push_back( std::make_unique<xAOD::jFexSumETRoI>() );
                    jTEContainer->back()->initialize(jfex, fpga, vec_words.at(i-1), 200);
                }
                //removing jTE counter from TOBs
                tobIndex -= n_jTE;

                //saving jEM into the EDM container
                for(unsigned int i=tobIndex; i>tobIndex-n_jEM; i--) {
                    jEMContainer->push_back( std::make_unique<xAOD::jFexFwdElRoI>() );
                    jEMContainer->back()->initialize(jfex, fpga, vec_words.at(i-1),0, 200, -99, -99);
                }
                //removing jEM counter from TOBs
                tobIndex -= n_jEM;

                //saving jTau into the EDM container
                for(unsigned int i=tobIndex; i>tobIndex-n_jTau; i--) {
                    jTauContainer->push_back( std::make_unique<xAOD::jFexTauRoI>() );
                    jTauContainer->back()->initialize(jfex, fpga, vec_words.at(i-1),1, 200, -99, -99);
                }
                //removing jTau counter from TOBs
                tobIndex -= n_jTau;

                //saving jLJ into the EDM container
                for(unsigned int i=tobIndex; i>tobIndex-n_jLJ; i--) {
                    jLJContainer->push_back( std::make_unique<xAOD::jFexLRJetRoI>() );
                    jLJContainer->back()->initialize(jfex, fpga, vec_words.at(i-1),1, 200, -99, -99);
                }
                //removing jLJ counter from TOBs
                tobIndex -= n_jLJ;

                //saving jJ into the EDM container
                for(unsigned int i=tobIndex; i>tobIndex-n_jJ; i--) {
                    jJContainer->push_back( std::make_unique<xAOD::jFexSRJetRoI>() );
                    jJContainer->back()->initialize(jfex, fpga, vec_words.at(i-1),1, 200, -99, -99);
                }
                //removing jJ counter from TOBs
                tobIndex -= n_jJ;
            }
            //moving trailer position index to the next jFEX data block
            trailers_pos -= (payload + jBits::jFEX2ROD_WORDS);
            
            if(trailers_pos != tobIndex){
                ATH_MSG_ERROR("Something went wrong decoding jFEX BS data. Trailer position: " << trailers_pos << " should match the TOB index position:" << tobIndex );
                return StatusCode::FAILURE;
            }
            

            if(trailers_pos == 0){
                READ_TOBS = false;
            }
        }
    }
        
    return StatusCode::SUCCESS;
}


// Unpack number of TOBs in Trailer "TOB,XTOB Counter Trailer"
std::array<uint32_t,6> jFexRoiByteStreamTool::TOBCounterTrailer (uint32_t word) const {
    
    uint32_t jJ    = ((word >> jBits::jJ_TOB_COUNTS  ) & jBits::TOB_COUNTS_6b);
    uint32_t jLJ   = ((word >> jBits::jLJ_TOB_COUNTS ) & jBits::TOB_COUNTS_6b);
    uint32_t jTau  = ((word >> jBits::jTau_TOB_COUNTS) & jBits::TOB_COUNTS_6b);
    uint32_t jEM   = ((word >> jBits::jEM_TOB_COUNTS ) & jBits::TOB_COUNTS_6b);
    uint32_t jTE   = ((word >> jBits::jTE_TOB_COUNTS ) & jBits::TOB_COUNTS_1b);
    uint32_t jXE   = ((word >> jBits::jXE_TOB_COUNTS ) & jBits::TOB_COUNTS_1b);
    //uint32_t TSM   = ((word                          ) & jBits::TOB_COUNTS_1b); // Trigger Safe Mode. Not used for now, maybe is needed in the future
          
    return {jJ,jLJ,jTau,jEM,jTE,jXE};
        
}

// Unpack number of xTOBs in Trailer "TOB,XTOB Counter Trailer"
std::array<uint32_t,4> jFexRoiByteStreamTool::xTOBCounterTrailer (uint32_t word) const {
    
    uint32_t xjJ    = ((word >> jBits::jJ_TOB_COUNTS  ) & jBits::TOB_COUNTS_6b);
    uint32_t xjLJ   = ((word >> jBits::jLJ_TOB_COUNTS ) & jBits::TOB_COUNTS_6b);
    uint32_t xjTau  = ((word >> jBits::jTau_TOB_COUNTS) & jBits::TOB_COUNTS_6b);
    uint32_t xjEM   = ((word >> jBits::jEM_TOB_COUNTS ) & jBits::TOB_COUNTS_6b);
    
    return {xjJ,xjLJ,xjTau,xjEM};
    
}

// Unpack jFEX to ROD Trailer
std::array<uint32_t,3> jFexRoiByteStreamTool::jFEXtoRODTrailer (uint32_t word0, uint32_t /*word1*/) const {
    
    uint32_t payload    = ((word0 >> jBits::PAYLOAD_ROD_TRAILER ) & jBits::ROD_TRAILER_16b);
    uint32_t fpga       = ((word0 >> jBits::FPGA_ROD_TRAILER    ) & jBits::ROD_TRAILER_2b );
    uint32_t jfex       = ((word0 >> jBits::jFEX_ROD_TRAILER    ) & jBits::ROD_TRAILER_4b );
    
    //DO NOT REMOVE, may be necessary in the future
    
    //uint32_t ro_slice   = ((word0 >> jBits::RO_ROD_TRAILER      ) & jBits::ROD_TRAILER_4b );
    //uint32_t trig_slice = ((word0 >> jBits::TSN_ROD_TRAILER     ) & jBits::ROD_TRAILER_4b );
    
    //uint32_t error      = ((word1 >> jBits::ERROR_ROD_TRAILER   ) & jBits::ROD_TRAILER_6b );
    //uint32_t crc        = ((word1 >> jBits::CRC_ROD_TRAILER     ) & jBits::ROD_TRAILER_20b);
    

    //std::cout << "\tWord0 PAYLO: " <<  ((word0 >> jBits::PAYLOAD_ROD_TRAILER ) & jBits::ROD_TRAILER_16b) << std::endl;
    //std::cout << "\tWord0 FPGA : " <<  ((word0 >> jBits::FPGA_ROD_TRAILER    ) & jBits::ROD_TRAILER_2b ) << std::endl;   
    //std::cout << "\tWord0 jFEX : " <<  ((word0 >> jBits::jFEX_ROD_TRAILER    ) & jBits::ROD_TRAILER_4b ) << std::endl;
    //std::cout << "\tWord0 RO   : " <<  ((word0 >> jBits::RO_ROD_TRAILER      ) & jBits::ROD_TRAILER_4b ) << std::endl;          
    //std::cout << "\tWord0 TSN  : " <<  ((word0 >> jBits::TSN_ROD_TRAILER     ) & jBits::ROD_TRAILER_4b ) << std::endl;

    //std::cout << "\tWord1 ERROR: " <<  ((word1 >> jBits::ERROR_ROD_TRAILER   ) & jBits::ROD_TRAILER_6b ) << std::endl;
    //std::cout << "\tWord1 CRC  : " <<  ((word1 >> jBits::CRC_ROD_TRAILER     ) & jBits::ROD_TRAILER_20b) << std::endl << std::endl;   
    
    
    return {payload,fpga,jfex};
   
}




// Unpack jFEX to ROD Header
void jFexRoiByteStreamTool::jFEXtoRODHeader (uint32_t /*word0*/, uint32_t /*word1*/) const {
    
    //std::cout << "jFEXtoRODHeader: word0 0x" <<  std::hex << word0  << std::dec << std::endl;
    //std::cout << "jFEXtoRODHeader: word1 0x" <<  std::hex << word1  << std::dec << std::endl;
    
    //std::cout << "\tWord0  : " <<  std::bitset<32>(word0) << std::endl;
    //std::cout << "\tWord1  : " <<  std::bitset<32>(word1) << std::endl;
    

    //std::cout << "\tWord0 L1Calo: " <<  ((word0 >> jBits::L1CALO_STREAM_ID_ROD_HEADER ) & jBits::ROD_HEADER_8b  ) << std::endl;
    //std::cout << "\t\t L1Calo SLOT: " <<  ((word0 >> (jBits::L1CALO_STREAM_ID_ROD_HEADER + jBits::L1CALO_STREAM_ID_SLOT_ROD_HEADER) ) & 4  ) << std::endl;
    //std::cout << "\t\t L1Calo FPGA: " <<  ((word0 >> (jBits::L1CALO_STREAM_ID_ROD_HEADER + jBits::L1CALO_STREAM_ID_FPGA_ROD_HEADER) ) & 2  ) << std::endl;
    //std::cout << "\t\t L1Calo INFO: " <<  ((word0 >> (jBits::L1CALO_STREAM_ID_ROD_HEADER + jBits::L1CALO_STREAM_ID_INFO_ROD_HEADER) ) & 2  ) << std::endl;
    //std::cout << "\tWord0 BCN   : " <<  ((word0 >> jBits::BCN_ROD_HEADER              ) & jBits::ROD_HEADER_12b ) << std::endl;   
    //std::cout << "\tWord0 CRC   : " <<  ((word0 >> jBits::CRC_ROD_HEADER              ) & jBits::ROD_HEADER_9b  ) << std::endl;
    //std::cout << "\tWord0 Vers  : " <<  ((word0 >> jBits::VERS_ROD_HEADER             ) & jBits::ROD_HEADER_3b  ) << std::endl;          

    //std::cout << "\tWord1 L1ID  : " <<  ((word1 >> jBits::L1ID_ROD_HEADER  ) & jBits::ROD_HEADER_24b ) << std::endl;
    //std::cout << "\tWord1 ECRID : " <<  ((word1 >> jBits::ECRID_ROD_HEADER ) & jBits::ROD_HEADER_8b  ) << std::endl;   


   
}





















/// xAOD->BS conversion
StatusCode jFexRoiByteStreamTool::convertToBS(std::vector<WROBF*>& /*vrobf*/, const EventContext& /*eventContext*/) {
    
/*
    // Retrieve the RoI container
    auto muonRoIs = SG::makeHandle(m_roiReadKey, eventContext);
    ATH_CHECK(muonRoIs.isValid());

    // Clear BS data cache
    clearCache(eventContext);

    // Create raw ROD data words
    ATH_MSG_DEBUG("Converting " << muonRoIs->size() << " L1 Muon RoIs to ByteStream");
    uint32_t* data = newRodData(eventContext, muonRoIs->size());
    for (size_t i=0; i<muonRoIs->size(); ++i) {
        data[i] = muonRoIs->at(i)->roiWord();
    }

    // Create ROBFragment containing the ROD words
    const eformat::helper::SourceIdentifier sid(eformat::TDAQ_MUON_CTP_INTERFACE, m_muCTPIModuleID.value());
    vrobf.push_back(newRobFragment(
                        eventContext,
                        sid.code(),
                        muonRoIs->size(),
                        data,
                        eformat::STATUS_BACK // status_position is system-specific
                    ));
*/
    return StatusCode::SUCCESS;
}

