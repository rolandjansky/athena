#include "infraL1Calo/EfexLatomeFibrePacker.h"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <map>


std::vector<FibrePackerBase::myDataWord> EfexLatomeFibrePacker::getPackedData(const std::vector<myDataWord>& inFrame,
                                                             myDataWord bcNumber, 
                                                             InputDataFrameType frameType) const {

    std::vector<myDataWord> dataToLoad(FexDefs::num32BitWordsPerFibre(),0);
    auto supercells = inFrame;

    switch(frameType){
                    
        case InputDataFrameType::Normal:		     // Standard data pattern
            {
            for(auto& icell:supercells)
                icell = icell & 0x3ff;              // truncate to 10 bits 

            myDataWord bcId = bcNumber   & 0x7f ;              // 7 bits from BCID

            myDataWord bcId02 = bcId & 0x7;
            myDataWord bcId34 = (bcId >> 3) & 0x3;
            myDataWord bcId56 = (bcId >> 5) & 0x3;

            dataToLoad.at(0) =  (bcId56 << 8)               | (supercells.at(0) <<10)  | 
                                (supercells.at(1) << 20)    | (bcId34 << 30);
            dataToLoad.at(1) =  supercells.at(2)            | (supercells.at(3) << 10) | 
                                (supercells.at(4) << 20)    | (((supercells.at(19) >>8) & 0x3) << 30);
            dataToLoad.at(2) =  supercells.at(5)            | (supercells.at(6) << 10) | 
                                (supercells.at(7) << 20)    | (((supercells.at(19) >>6) & 0x3) << 30);
            dataToLoad.at(3) =  supercells.at(8)            | (supercells.at(9) << 10) | 
                                (supercells.at(10) << 20)   | (((supercells.at(19) >>4) & 0x3) << 30);
            dataToLoad.at(4) =  supercells.at(11)           | (supercells.at(12) << 10) | 
                                (supercells.at(13) << 20)   | (((supercells.at(19) >>2) & 0x3) << 30);
            dataToLoad.at(5) =  supercells.at(14)           | (supercells.at(15) << 10)  | 
                                (supercells.at(16) << 20)   | (((supercells.at(19)    ) & 0x3) << 30);
            dataToLoad.at(6) =  supercells.at(17)           | (supercells.at(18) << 10)| bcId02 << 20;

            // original code
            // myDataWord tempCRC = crc9d32(dataToLoad,6l,true);               // calculate CRC32 on first 6 words
            // myDataWord myCRCReminder = crc9d23(dataToLoad[6],tempCRC,true); // CRC23 on the last word
            // new code
            myDataWord myCRCReminder = crc9full(dataToLoad,215);               // 215 = 32*6+23

            dataToLoad.at(0) =  K_28_5         | dataToLoad.at(0) ;          // add K-character
            dataToLoad[6]    =  dataToLoad[6]  | ( myCRCReminder << 23) ;        // add CRC check

            break;
            }
        case InputDataFrameType::Alignement:		// Special pattern, LATOME alignement/mapping frame
            {

            myDataWord latome_id        = supercells.at(0) & 0xff;          // 8b 
            myDataWord latome_src_id    = supercells.at(1) & 0xffffffff;    // 32b
            myDataWord fiber_id         = supercells.at(2) & 0x3f;          // 6b
            myDataWord bcId = bcNumber   & 0xfff ;                          // 12 bits of BCID

            dataToLoad.at(0) =  (fiber_id << 16) | (latome_id << 24);
            dataToLoad.at(1) =  latome_src_id;
            dataToLoad.at(6) =  bcId; 

            dataToLoad.at(0) =  (K_28_0 << 8)   | dataToLoad.at(0) ;         // add K_28_0, this is used for CRC calculation

            // original code
            // myDataWord tempCRC = crc9d32(dataToLoad,6l,true);               // calculate CRC32 on first 6 words
            //myDataWord myCRCReminder = crc9d23(dataToLoad[6],tempCRC,true); // CRC23 on the last word
            // new code
            myDataWord myCRCReminder = crc9full(dataToLoad,215);               // 215 = 32*6+23

            dataToLoad.at(0) =  K_28_5    | dataToLoad.at(0) ;              // add K_28_5, this is not used for CRC calculation 
            dataToLoad[6]    =  dataToLoad[6]  | ( myCRCReminder << 23) ;        // add CRC check to the famous last word

            break;
            }
        }

    return dataToLoad; 

}    

std::vector<FibrePackerBase::myDataWord> EfexLatomeFibrePacker::getPackedControl(const std::vector<myDataWord>& /*inFrame*/,
                                                                myDataWord /*bcNumber*/,
                                                                InputDataFrameType frameType) const {

    std::vector<myDataWord> controlWords(FexDefs::num32BitWordsPerFibre(),0);

    switch(frameType){
                    
        case InputDataFrameType::Normal:		            // one K character in first data word
            controlWords.at(0) = 0x1;
            break;

        case InputDataFrameType::Alignement:		        // two K characters in first data word
            controlWords.at(0) = 0x3;
            break;
        
    } 

    return controlWords;
}    

bool EfexLatomeFibrePacker::checkCRC(const std::vector<myDataWord>& encodedData, 
                                    InputDataFrameType frameType) const {

    auto inputData = encodedData;
    myDataWord CRCCheck;

    //  Comment SJH: why do we need two cases here 
    //  (may have been different originally, but not now?)
    switch(frameType){
                    
        case InputDataFrameType::Normal:		                    // one K character in first data word
            inputData.at(0) = inputData.at(0) & 0xffffff00;
            // old code
            // CRCCheck = crc9d32(inputData,7l,true);               // calculate CRC32 on first 6 words
            // new code
            CRCCheck = crc9full(inputData,224);                     // calculate CRC on 224 bits = 7*32 bit words
            break;

        case InputDataFrameType::Alignement:		                // two K characters in first data word, but zeroing only the first one
            inputData.at(0) = inputData.at(0) & 0xffffff00;
            // old code
            // CRCCheck = crc9d32(inputData,7l,true);               // calculate CRC32 on first 6 words
            // new code
            CRCCheck = crc9full(inputData,224);                     // calculate CRC on 224 bits = 7*32 bit words
            break;
        
    } 

    return (CRCCheck == 0);
}

FibrePackerBase::myDataWord  EfexLatomeFibrePacker::getBcNumber(const std::vector<myDataWord>& encodedData, 
                                               InputDataFrameType frameType) const {
    myDataWord BcNumber =0;

    switch(frameType){
                    
        case InputDataFrameType::Normal:		                    
            {
            myDataWord bcId02 = (encodedData.at(6)  >> 20 ) & 0x7;
            myDataWord bcId34 = (encodedData.at(0)  >> 30 ) & 0x3;
            myDataWord bcId56 = (encodedData.at(0)  >> 8  ) & 0x3;

            BcNumber = bcId02 | (bcId34 << 3) | (bcId56 << 5);
            break;
            }
        case InputDataFrameType::Alignement:		                
            {
            BcNumber = encodedData.at(6) & 0xfff;
            break;
            }
    } 

    return BcNumber;
}

std::vector<FibrePackerBase::myDataWord>  EfexLatomeFibrePacker::getUnpackedData(const std::vector<myDataWord>& encodedData, 
                                                                InputDataFrameType frameType) const {
    std::vector<myDataWord> unpackedData; 

    switch(frameType){
                    
        case InputDataFrameType::Normal:		                    
            {
            std::vector<myDataWord> supercells(EfexDefs::maxSuperCellsPerFibre(),0);

            // see: https://gitlab.cern.ch/atlas-l1calo-online/infraL1Calo/-/commit/1dc82d1ce21c14ca56d90e1af7d1af9bdb7990df#note_5865270
            // 20 10-bit counts encoded in 7 32-bit words
            // word 0 has 2, word 1-5 have 3 each (so 15 in total),
            // word 6 has 2 more, and the last count is spread over the extra
            // 2 bits in words 1-5

            supercells.at(0)  = (encodedData.at(0) >> 10) & 0x3ff;
            supercells.at(1)  = (encodedData.at(0) >> 20) & 0x3ff;
            supercells.at(2)  = (encodedData.at(1)      ) & 0x3ff;
            supercells.at(3)  = (encodedData.at(1) >> 10) & 0x3ff;
            supercells.at(4)  = (encodedData.at(1) >> 20) & 0x3ff;
            supercells.at(5)  = (encodedData.at(2)      ) & 0x3ff;
            supercells.at(6)  = (encodedData.at(2) >> 10) & 0x3ff;
            supercells.at(7)  = (encodedData.at(2) >> 20) & 0x3ff;
            supercells.at(8)  = (encodedData.at(3)      ) & 0x3ff;
            supercells.at(9)  = (encodedData.at(3) >> 10) & 0x3ff;
            supercells.at(10) = (encodedData.at(3) >> 20) & 0x3ff;
            supercells.at(11) = (encodedData.at(4)      ) & 0x3ff;
            supercells.at(12) = (encodedData.at(4) >> 10) & 0x3ff;
            supercells.at(13) = (encodedData.at(4) >> 20) & 0x3ff;
            supercells.at(14) = (encodedData.at(5)      ) & 0x3ff;
            supercells.at(15) = (encodedData.at(5) >> 10) & 0x3ff;
            supercells.at(16) = (encodedData.at(5) >> 20) & 0x3ff;
            supercells.at(17) = (encodedData.at(6)      ) & 0x3ff;
            supercells.at(18) = (encodedData.at(6) >> 10) & 0x3ff;
            supercells.at(19) = ((encodedData.at(5) >> 30) & 0x3)      |
                                ((encodedData.at(4) >> 30) & 0x3) << 2 | 
                                ((encodedData.at(3) >> 30) & 0x3) << 4 |
                                ((encodedData.at(2) >> 30) & 0x3) << 6 | 
                                ((encodedData.at(1) >> 30) & 0x3) << 8 ;
            unpackedData=supercells;    
            break;
            }
        case InputDataFrameType::Alignement:		                
            {
            myDataWord latome_id        = (encodedData.at(0) >> 24) & 0xff;          // 8b 
            myDataWord latome_src_id    = encodedData.at(1) & 0xffffffff;    // 32b
            myDataWord fiber_id         = (encodedData.at(0) >> 16) & 0x3f;          // 6b

            unpackedData.push_back(latome_id);
            unpackedData.push_back(latome_src_id);
            unpackedData.push_back(fiber_id);
            break;
            }
    } 


    return unpackedData;

}    

