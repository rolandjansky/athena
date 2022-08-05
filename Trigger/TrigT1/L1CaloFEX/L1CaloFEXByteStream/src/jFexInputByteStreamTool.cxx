/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFexInputByteStreamTool  -  This tool decodes Run3 jFEX input data!
//                              -------------------
//     begin                : 01 07 2022
//     email                : Sergi.Rodriguez@cern.ch
//  ***************************************************************************/

#include "jFexInputByteStreamTool.h"
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

jFexInputByteStreamTool::jFexInputByteStreamTool(const std::string& type,
        const std::string& name,
        const IInterface* parent)
    : base_class(type, name, parent) {}

StatusCode jFexInputByteStreamTool::initialize() {
    // Conversion mode for jTowers
    ConversionMode jTowersmode = getConversionMode(m_jTowersReadKey, m_jTowersWriteKey, msg());
    ATH_CHECK(jTowersmode!=ConversionMode::Undefined);
    ATH_CHECK(m_jTowersWriteKey.initialize(jTowersmode==ConversionMode::Decoding));
    ATH_CHECK(m_jTowersReadKey.initialize(jTowersmode==ConversionMode::Encoding));
    ATH_MSG_DEBUG((jTowersmode==ConversionMode::Encoding ? "Encoding" : "Decoding") << " jTowers ROB IDs: " << MSG::hex << m_robIds.value() << MSG::dec);

    return StatusCode::SUCCESS;
}

// BS->xAOD conversion
StatusCode jFexInputByteStreamTool::convertFromBS(const std::vector<const ROBF*>& vrobf, const EventContext& ctx) const {
    
    //WriteHandle for jFEX EDMs
    
    //---jTower EDM
    SG::WriteHandle<xAOD::jFexTowerContainer> jTowersContainer(m_jTowersWriteKey, ctx);
    ATH_CHECK(jTowersContainer.record(std::make_unique<xAOD::jFexTowerContainer>(), std::make_unique<xAOD::jFexTowerAuxContainer>()));
    ATH_MSG_DEBUG("Recorded jFexTowerContainer with key " << jTowersContainer.key());
    

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

/* do not remove, checks will be needed when P1 data available
        int aux = 1; //delete, for debug
        for (const uint32_t word : vec_words) {

            printf("\t %3d  raw word: 0x%08x \t bits: %32s\n",aux,word, (std::bitset<32>(word).to_string()).c_str() );
            aux++; //delete, for debug

        }
*/

        // jFEX to ROD trailer position
        unsigned int trailers_pos = rob->rod_ndata();
        
        // Starting to loop over the different jFEX blocks
        bool READ_TOBS = true;
        while(READ_TOBS){
            
            //if(m_verbose) printf("----------------------------------------------------------------------------------------------------------------------\n");
            
            
            const auto [payload, fpga, jfex]                      = jFEXtoRODTrailer  ( vec_words.at(trailers_pos-2), vec_words.at(trailers_pos-1) );
            
            if(payload % jBits::DATA_BLOCKS != 0){
                ATH_MSG_DEBUG(C.B_RED<< "ERROR!!"<<C.END<<"  Payload number (" << payload << ") not multuple of data blocks expected (" << jBits::DATA_BLOCKS <<")");
                //printf("%sERROR!!%s payload number (%3d) not multuple of data blocks expected (%3d)",C.RED.c_str(),C.END.c_str(),payload,jBits::DATA_BLOCKS );
                READ_TOBS = false;
                continue;
            }
            
            //Position index, removing jFEX to ROD, TOB and xTOB Trailers from trailers_pos (4 positions), possible padding word added on the data to get even number of 32bit words
            unsigned int wordIndex = trailers_pos - (jBits::jFEX2ROD_WORDS);      
            

            for (uint iblock = 0; iblock < jBits::DATA_BLOCKS; iblock++){
                const auto [channel, saturation] = BulkStreamTrailer(vec_words.at(wordIndex-1),vec_words.at(wordIndex-2));
                
                const auto [DATA15_low          , DATA14, DATA13] = Dataformat1(vec_words.at(wordIndex-3));
                const auto [DATA15_up,DATA12_up , DATA11, DATA10] = Dataformat2(vec_words.at(wordIndex-4));
                const auto [DATA12_low          , DATA9 , DATA8 ] = Dataformat1(vec_words.at(wordIndex-5));
                const auto [DATA7_low           , DATA6 , DATA5 ] = Dataformat1(vec_words.at(wordIndex-6));
                const auto [DATA7_up ,DATA4_up  , DATA3 , DATA2 ] = Dataformat2(vec_words.at(wordIndex-7));
                const auto [DATA4_low           , DATA1 , DATA0 ] = Dataformat1(vec_words.at(wordIndex-8));
                
                //uncomment for mergeing splitted Et
                uint16_t DATA4  = ( DATA4_up  << jBits::BS_MERGE_DATA ) + DATA4_low;
                uint16_t DATA7  = ( DATA7_up  << jBits::BS_MERGE_DATA ) + DATA7_low;
                uint16_t DATA12 = ( DATA12_up << jBits::BS_MERGE_DATA ) + DATA12_low;
                uint16_t DATA15 = ( DATA15_up << jBits::BS_MERGE_DATA ) + DATA15_low;
                
                std::array<uint16_t,16> allDATA = {DATA0, DATA1, DATA2, DATA3, DATA4, DATA5, DATA6, DATA7, DATA8, DATA9, DATA10, DATA11, DATA12, DATA13, DATA14, DATA15 };
                //std::array<uint16_t,16> allsat  = {0};
                
                for(uint idata = 0; idata < allDATA.size(); idata++){
                    jTowersContainer->push_back( std::make_unique<xAOD::jFexTower>() );
                    char et_saturation = ((saturation >> idata) & jBits::BS_TRAILER_1b);
                    //allsat[idata] = et_saturation;
                    // eta and phi are place holders for now, need to change in the future!
                    jTowersContainer->back()->initialize(-99, -99, allDATA[idata], jfex, fpga, channel, idata, et_saturation );                    
                }
                
                //if(m_verbose ){
                    //printf("DATA0 :%5d   DATA1 :%5d   DATA2 :%5d   DATA3 :%5d\n",DATA0  ,DATA1  ,DATA2  ,DATA3  );
                    //printf("DATA4 :%5d   DATA5 :%5d   DATA6 :%5d   DATA7 :%5d\n",DATA4  ,DATA5  ,DATA6  ,DATA7  );
                    //printf("DATA8 :%5d   DATA8 :%5d   DATA10:%5d   DATA11:%5d\n",DATA8  ,DATA9  ,DATA10 ,DATA11 );
                    //printf("DATA12:%5d   DATA13:%5d   DATA14:%5d   DATA15:%5d\n\n",DATA12 ,DATA13 ,DATA14 ,DATA15 );
                    
                    //printf("DATA0 :%5d   DATA1 :%5d   DATA2 :%5d   DATA3 :%5d\n",allsat[0]  ,allsat[1]  ,allsat[2]  ,allsat[3]  );
                    //printf("DATA4 :%5d   DATA5 :%5d   DATA6 :%5d   DATA7 :%5d\n",allsat[4]  ,allsat[5]  ,allsat[6]  ,allsat[7]  );
                    //printf("DATA8 :%5d   DATA8 :%5d   DATA10:%5d   DATA11:%5d\n",allsat[8]  ,allsat[9]  ,allsat[10] ,allsat[11] );
                    //printf("DATA12:%5d   DATA13:%5d   DATA14:%5d   DATA15:%5d\n",allsat[12] ,allsat[13] ,allsat[14] ,allsat[15] );
                //}

                
                wordIndex -= jBits::DATA_WORDS_PER_BLOCK;
            }
            

            //moving trailer position index to the next jFEX data block
            trailers_pos -= (payload + jBits::jFEX2ROD_WORDS);
            
            if(trailers_pos == 0){
                READ_TOBS = false;
            }


            
        }
        
    }
        
    return StatusCode::SUCCESS;
}


// Unpack jFEX to ROD Trailer
std::array<uint32_t,3> jFexInputByteStreamTool::jFEXtoRODTrailer (uint32_t word0, uint32_t /*word1*/) const {
    
    uint32_t payload    = ((word0 >> jBits::PAYLOAD_ROD_TRAILER ) & jBits::ROD_TRAILER_16b);
    uint32_t fpga       = ((word0 >> jBits::FPGA_ROD_TRAILER    ) & jBits::ROD_TRAILER_2b );
    uint32_t jfex       = ((word0 >> jBits::jFEX_ROD_TRAILER    ) & jBits::ROD_TRAILER_4b );
    
    //DO NOT REMOVE, may be necessary in the future
    //if(m_verbose){
        //printf("%sWord0  PAYLOAD: %-7d\n", C.ORANGE.c_str(), ((word0 >> jBits::PAYLOAD_ROD_TRAILER ) & jBits::ROD_TRAILER_16b) );
        //printf("Word0     FPGA: %-7d\n",                     ((word0 >> jBits::FPGA_ROD_TRAILER    ) & jBits::ROD_TRAILER_2b ) );
        //printf("Word0     jFEX: %-7d\n",                     ((word0 >> jBits::jFEX_ROD_TRAILER    ) & jBits::ROD_TRAILER_4b ) );
        //printf("Word0 RO SLICE: %-7d\n",                     ((word0 >> jBits::RO_ROD_TRAILER      ) & jBits::ROD_TRAILER_4b ) );
        //printf("Word0      TSN: %-7d\n",                     ((word0 >> jBits::TSN_ROD_TRAILER     ) & jBits::ROD_TRAILER_4b ) );
    
        //printf("Word1    ERROR: %-7d\n",                     ((word1 >> jBits::ERROR_ROD_TRAILER   ) & jBits::ROD_TRAILER_6b ) );
        //printf("Word1      CRC: %-7d%s\n\n",                 ((word1 >> jBits::CRC_ROD_TRAILER     ) & jBits::ROD_TRAILER_20b) , C.END.c_str() );
    //}

    
    return {payload,fpga,jfex};
   
}

// Unpack Bulk stream trailer
std::array<uint16_t,2> jFexInputByteStreamTool::BulkStreamTrailer (uint32_t word0, uint32_t word1) const {
    
    uint16_t Satur_down = ((word1 >> jBits::BS_SATUR_1_TRAILER ) & jBits::BS_TRAILER_8b );
    uint16_t Satur_high = ((word1 >> jBits::BS_SATUR_0_TRAILER ) & jBits::BS_TRAILER_8b );
    uint16_t Channel    = ((word0 >> jBits::BS_CHANNEL_TRAILER ) & jBits::BS_TRAILER_8b );
    
    uint16_t Satur = ( Satur_high << jBits::BS_SATUR_1_TRAILER ) + Satur_down;
    
    // Checking if K28.5 is there, if so then any jTower is saturated
    if(Satur_high == 0xbc and Satur_down == 0x0){
        Satur = 0;
    }
    
    
    //DO NOT REMOVE, may be necessary in the future
    //if(m_verbose){
        //printf("%sBulkStreamTrailer: word0 0x%08x , word1 0x%08x\n", C.BLUE.c_str(),word0, word1 );
        //printf("Word1 KSATUR [15:8]: %3x\n" , ((word1 >> jBits::BS_SATUR_0_TRAILER ) & jBits::BS_TRAILER_8b ) );
        //printf("Word1  SATUR [ 7:0]: %3x\n" , ((word1 >> jBits::BS_SATUR_1_TRAILER ) & jBits::BS_TRAILER_8b ) );
        //printf("Word1   BCID [ 6:0]: %3x\n" , ((word1 >> jBits::BS_BCID_TRAILER    ) & jBits::BS_TRAILER_7b ) );
        //printf("Word1           CRC: %3x\n" , ((word1 >> jBits::BS_CRC_TRAILER     ) & jBits::BS_TRAILER_9b ) );
        //printf("SATUR %16s\n",(std::bitset<16>(Satur).to_string()).c_str());
        //printf("Word0       CHANNEL: %-7d\n" ,((word0 >> jBits::BS_CHANNEL_TRAILER ) & jBits::BS_TRAILER_8b ));
        //printf("Word0          BCID: %3x%s\n",((word0 >> 8 ) & 0xfff ),C.END.c_str() );
    //}
    
    return {Channel, Satur};
   
}


// Unpack Bulk stream data format 1 : DATA4 [7:0], DATA1 [12:0], DATA0 [12:0]
std::array<uint16_t,3>  jFexInputByteStreamTool::Dataformat1 (uint32_t word0) const {
    
    uint16_t data_low  = ((word0 >> jBits::BS_ET_DATA_0 ) & jBits::BS_TRAILER_12b);
    uint16_t data_mid  = ((word0 >> jBits::BS_ET_DATA_1 ) & jBits::BS_TRAILER_12b );
    uint16_t data_up   = ((word0 >> jBits::BS_ET_DATA_4 ) & jBits::BS_TRAILER_8b );
    
    //DO NOT REMOVE, may be necessary in the future
    //if(m_verbose){
        //printf("Dataformat 1: word0 0x%08x   -->  ",word0 );
        //printf("DATA LOW  : %3x,  ", data_low );
        //printf("DATA MID  : %3x,  ", data_mid );
        //printf("DATA UP   : %3x\n", data_up  );
    //}
    
    return {data_up,data_mid,data_low};
}


// Unpack Bulk stream data format 2 : DATA7 [11:8], DATA4 [11:8], DATA1 [12:0], DATA0 [12:0]
std::array<uint16_t,4>  jFexInputByteStreamTool::Dataformat2 (uint32_t word0) const {
    
    uint16_t data_low  = ((word0 >> jBits::BS_ET_DATA_0 ) & jBits::BS_TRAILER_12b);
    uint16_t data_mid  = ((word0 >> jBits::BS_ET_DATA_1 ) & jBits::BS_TRAILER_12b );
    uint16_t data_up_1 = ((word0 >> jBits::BS_ET_DATA_4 ) & jBits::BS_TRAILER_4b );
    uint16_t data_up_2 = ((word0 >> jBits::BS_ET_DATA_7 ) & jBits::BS_TRAILER_4b );
    
    //DO NOT REMOVE, may be necessary in the future
    //if(m_verbose){
        //printf("Dataformat 2: word0 0x%08x   -->  ",word0 );
        //printf("DATA LOW  : %3x,  ", data_low  );
        //printf("DATA MID  : %3x,  ", data_mid  );
        //printf("DATA UP 1 : %3x,  ", data_up_1 );
        //printf("DATA UP 2 : %3x\n", data_up_2 );
    //}
    
    return {data_up_2,data_up_1,data_mid,data_low};
}



/// xAOD->BS conversion
StatusCode jFexInputByteStreamTool::convertToBS(std::vector<WROBF*>& /*vrobf*/, const EventContext& /*eventContext*/) {
    
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

