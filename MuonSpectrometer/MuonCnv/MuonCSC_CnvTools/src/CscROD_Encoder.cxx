/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of CscROD_Encoder class 

#include "CscROD_Encoder.h" 

#include "MuonRDO/CscRawDataCollection.h"
#include "CscRODReadOut.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IMessageSvc.h"

/** constructor 
*/ 
CscROD_Encoder::CscROD_Encoder() : m_cscRdo(0)
{}

/** convert CSC RDO to a vector of 32bit words
*/ 
StatusCode CscROD_Encoder::fillROD(std::vector<uint32_t>& v, MsgStream& mLog)
{
    
  mLog << MSG::DEBUG << " in CscROD_Encoder " << endmsg;

  // ROD readout structure : encoder/decoder
  CscRODReadOut rodReadOut;

  // initialize the identifier helper
  rodReadOut.set(m_cscIdHelper);

 // first group the pads into their RPUs
 typedef std::vector<const CscRawData*> rpu;
 std::map<uint16_t, rpu> mapRPU; 

 /** group the data into RPU's */
 DataVector<CscRawData>::const_iterator it     = m_cscRdo->begin();
 DataVector<CscRawData>::const_iterator it_end = m_cscRdo->end();
 uint16_t rpuID = 0x0;
 for(; it!=it_end; ++it){
   const  CscRawData* rawData = (*it); 
   uint16_t spuID = rawData->rpuID();
   if ( spuID <= 4 ) rpuID = 5;
   else if ( spuID>4 && spuID <=9 ) rpuID = 11;
   else { 
     mLog << MSG::ERROR << "CscROD_Encoder : RPU ID out of range for Initial Layout:: SPU ID = " 
	  << spuID << endmsg; 
     return StatusCode::FAILURE;
   }
   mLog << MSG::DEBUG << "CscROD_Encoder : SPU ID is " << spuID << " for RPU ID= " << rpuID << endmsg;
   mapRPU[rpuID].push_back(rawData);
 }

 /** in case only one RPU has data, write out the header of the empty RPU as well */
 rpu emptyVect; 
 emptyVect.clear();
 const std::vector<uint16_t> rpus = m_cscRdo->rpuID();
 if ( rpus.size() == 1 ) {
   uint16_t rId = rpus[0];
   mLog << MSG::DEBUG << "CscROD_Encoder : Only one RPU with data in this ROD - RPU ID = " << rId << endmsg;
   uint16_t emptyRPU = 5;
   if ( rId == emptyRPU) emptyRPU = 11;
   mapRPU.insert ( std::make_pair(emptyRPU, emptyVect) );
 }  

 /** Sampling phase; Trigger type, First summary bit */
 bool samplingPhase      = m_cscRdo->samplingPhase();
 bool triggerType        = m_cscRdo->triggerType();
 uint8_t firstBitSummary = m_cscRdo->firstBitSummary();

 /** encode strip data fragments :: ROD body */
 mLog << MSG::DEBUG << "Number of RPU in this ROD = " << mapRPU.size() << endmsg;
  
 unsigned int rpuIndex = 0;
 for (std::pair<const uint16_t, rpu>& rpuPair : mapRPU) {

   /** this RPU identifier */
   uint16_t rpuID = rpuPair.first;
   mLog << MSG::DEBUG << "CscROD_Encoder : RPU id " << rpuID << endmsg;

   /** RPU size from simulation - assume no ghost words: 
       +1 for the RPU header word itsef */

   /** Find the size of the SPU - presumably the RPU has 5 SPU 
       The SPU ID should range from 0 to 4.
       total number of clusters words:
       The unitWord = 2 at the start of a cluster + total number of
       words in the cluster: this is basically the number of samples/2
       where one sample = 12-bit ADC. The 2 words at the start of the cluster describe 
       the cluster size and location. The %2 accounts for the old number of samples */
   uint32_t spuSize[] = {0, 0, 0, 0, 0};
   uint16_t numberOfDataWords = 0;
   uint32_t sampleDataWords = 0;
   for (const CscRawData* rawData : rpuPair.second) {
     uint16_t spuID = rawData->rpuID();
     mLog << MSG::DEBUG << "CscROD_Encoder : The SPU ID " << spuID << endmsg;
     unsigned int i = 0x800; 
     if ( spuID < 4 ) i=spuID;
     else if ( spuID > 4 && spuID < 9 ) i=spuID-5;
     else if ( spuID==4 || spuID==9 ) i=4;
     if (i>4) {
       mLog << MSG::ERROR << "CscROD_Encoder : SPU ID out of range - " << spuID << " Stop and fix it " << endmsg;
     } else {
       mLog << MSG::DEBUG << "CscROD_Encoder : SPU ID and Index = " << spuID << " " << i << endmsg;
       uint16_t size = (rawData->samples()).size();
       uint16_t unitSize = size/2 + size%2;
       spuSize[i] += 1;
       numberOfDataWords += 2 + unitSize;
       sampleDataWords += unitSize;
     }
   }
   mLog << MSG::DEBUG << "CscROD_Encoder : Total Sample size = " << sampleDataWords << endmsg;

   /** the RPU size = 1 word for the RPU header
       1 word for the SCA channel
       2 words for the cluster counts and miscellanous ---> +4 */
   uint16_t rpuSize = numberOfDataWords+4;

   mLog << MSG::DEBUG << "CscRDO_Encoder : RPU ID and size (in words) = " << rpuID 
	<< " " << rpuSize << endmsg;

   /** assume normal data type in simulation */
   std::vector<uint8_t> dataType = m_cscRdo->dataType();
   uint16_t type; 
   uint16_t typeSize = dataType.size();
   if ( typeSize > rpuIndex ) type = dataType[rpuIndex];
   else if ( typeSize == 1) type = dataType[0];
   else type = 0x0;
 
   mLog << MSG::DEBUG << "CscROD_Encoder : data type = " << type << endmsg;

   /** RPU header marker - it contains the size of the RPU, 
       the data type and the RPU ID */
   uint32_t rpuHeader = (0xFFFF & rpuSize) |
                        (0x000F & type) << 16 |
                        (0x000F & rpuID) << 24;

   /** RPU header */
   v.push_back ( rpuHeader ); 

   mLog << MSG::DEBUG << "CscROD_Encoder : this rpuHeader = " << MSG::hex << rpuHeader << MSG::dec << endmsg;

   /** SCA address - assume 0 in simulation */
   v.push_back( m_cscRdo->scaAddress() );

   /** build the cluster counts - the first word is the precision SPU */
   uint32_t precisionClusterWord = 0x0;
   for (unsigned int i=0; i<4; ++i) {
     precisionClusterWord = precisionClusterWord | ( spuSize[i] << (24-i*8) );
     mLog << MSG::DEBUG << "CscROD_Encoder : SPU ID = " << (i+1) 
     << " cluster Counts " << spuSize[i] << endmsg;
   }
   v.push_back( precisionClusterWord );

   mLog << MSG::DEBUG << "CscROD_Encoder : Non-precision " 
        << " cluster Counts " << spuSize[4] << endmsg;

   mLog << MSG::DEBUG << "CscROD_Encoder:: number of Cluster Data Words " << numberOfDataWords << endmsg;
  
   /** build the second cluster information word */
   uint32_t secondClusterWord = (0xFFFF & numberOfDataWords) |
                                (0xF & firstBitSummary ) << 16 |
                                (0x1 & samplingPhase ) << 17 |
                                (0x1 & triggerType ) << 18 |
                                (0xFF & spuSize[4] ) << 24;
   v.push_back( secondClusterWord );

   /** strip addresses and ADC samples - the first 2 words contain the
       address and the width+time+timeFlag, following the ADC samplings */
   unsigned int check = 0;
   for (const CscRawData* rawData : rpuPair.second) {

     uint32_t address    = rawData->address();
     uint16_t time       = rawData->time();
     uint16_t width      = rawData->width();
     bool isTimeComputed = rawData->isTimeComputed();

     v.push_back( address );
     check++;

     uint32_t secondWord = (0xFFF & width) |
                           (0xFFFF & time) << 16 |
                           (0x1 & isTimeComputed) << 28;

     v.push_back ( secondWord );
     check++;

     const std::vector<uint16_t>  amplitude  = rawData->samples();
     std::vector<uint32_t> v32;
     rodReadOut.encodeFragments(amplitude, v32);
     unsigned int n = v32.size();
     for (unsigned int i=0; i<n; i++) {
       v.push_back(v32[i]);
     }
     check += n;
     mLog << MSG::DEBUG << "CscROD_Encoder : The with and the 32-bit sample words " << width << " " << n << endmsg;
   }

   rpuIndex++;
 
   mLog << MSG::DEBUG << "CScROD_Encoder : Check and number of data words " << check << " " << numberOfDataWords << endmsg;
 
 }

 /** we assume no error block nor ghost words int the simulation */

 return StatusCode::SUCCESS; 
}


