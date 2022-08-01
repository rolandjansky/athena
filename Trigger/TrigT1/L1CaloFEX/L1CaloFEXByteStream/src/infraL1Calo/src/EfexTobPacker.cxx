#include "infraL1Calo/EfexTobPacker.h"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <map>


std::vector<FibrePackerBase::myDataWord> 
EfexTobPacker::getPackedData(const std::vector<myDataWord>& inFrame, myDataWord bcNumber, InputDataFrameType /*frameType*/) const
{
    size_t sz = inFrame.size();
    std::vector<myDataWord> dataToLoad(FexDefs::num32BitWordsPerFibre(),0);
    for ( size_t i = 0 ; i < sz ; ++i )
      dataToLoad.at(i) = inFrame.at(i);
    dataToLoad.at(6) = 0x1000;                                 // Is an eFEX
    dataToLoad.at(6) |= (bcNumber&0xf)<<8;                     // Lowest bits of BCID
    dataToLoad.at(6) |= crc9full(dataToLoad,224)<<23;          // Add CRC, 224 = 32*7 = full frame
    dataToLoad.at(6) |=  K_28_5;                               // add K-character

    return dataToLoad; 

}    

std::vector<FibrePackerBase::myDataWord> 
EfexTobPacker::getPackedControl(const std::vector<myDataWord>& /*inFrame*/, myDataWord /*bcNumber*/, InputDataFrameType /*frameType*/) const
{
    std::vector<myDataWord> controlWords(FexDefs::num32BitWordsPerFibre(),0);
    return controlWords;
}    

bool 
EfexTobPacker::checkCRC(const std::vector<myDataWord>& encodedData, InputDataFrameType /*frameType*/) const
{
    auto inputData = encodedData;
    inputData.at(0) = inputData.at(0) & 0xffffff00;
    // calculate CRC on 224 bits = 7*32 bit words
    return (crc9full(inputData,224) == 0);
}

FibrePackerBase::myDataWord  
EfexTobPacker::getBcNumber(const std::vector<myDataWord>& encodedData, InputDataFrameType /*frameType*/) const
{
    return ((encodedData.at(6)>>8)&0xf);
}

std::vector<FibrePackerBase::myDataWord>  
EfexTobPacker::getUnpackedData(const std::vector<myDataWord>& encodedData, InputDataFrameType /*frameType*/) const
{
    return encodedData;
}

