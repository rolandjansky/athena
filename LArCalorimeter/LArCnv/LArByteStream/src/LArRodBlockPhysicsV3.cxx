/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of the LArRODBlockStructure_3 class
// This version contains LArRawChannels (=E,t,Q)
// and LArDigits (=5 time samples)
// See .h file for more details.

#include "LArByteStream/LArRodBlockPhysicsV3.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArDigit.h"
#include "LArIdentifier/LArOnlineID.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include <stdlib.h>
#include <cstdio>
#include <iostream>

//#define LARBSDBGOUTPUT
#ifdef  LARBSDBGOUTPUT
#define LARBSDBG(text) m_logstr<<MSG::DEBUG<<text<<endmsg
#else
#define LARBSDBG(text)
#endif

namespace {
union ShortLong {
  uint16_t s[2];
  uint32_t l;
};
}

const uint32_t LArRodBlockPhysicsV3::m_DummyBitMap[4]={0,0,0,0};

LArRodBlockPhysicsV3::LArRodBlockPhysicsV3() : LArRodBlockStructure(),
m_logstr(Athena::getMessageSvc(), BlockType())
{  
  // retrieve onlineHelper
  const LArOnlineID* online_id;
  StoreGateSvc* detStore = 0;
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StatusCode sc =svcLoc->service( "DetectorStore", detStore );
  if (sc.isFailure()) {
    m_logstr << MSG::ERROR << "Unable to locate DetectorStore" << endmsg;
    std::abort();
  } else {
    m_logstr << MSG::INFO << "Successfully located DetectorStore" << endmsg;
  }     
  sc = detStore->retrieve(online_id, "LArOnlineID");
  if (sc.isFailure()) {
    m_logstr << MSG::FATAL << "Could not get LArOnlineID helper !" << endmsg;
    std::abort();
  } 
  else {
    m_onlineHelper=online_id;
    m_logstr << MSG::DEBUG << " Found the LArOnlineID helper. " << endmsg;
  }
  
  m_iHeadBlockSize=endtag/2; // The implicit cast rounds down to the right size 
  m_NSumBlkWords=4; // BL : words of 32 bits here
  m_NCounterBlkWords=5;
  m_NFebInfoBlkWords=4;
  m_NEnergyBlkWords=128*16/32;
  m_NGainBlkWords=128*2/32;
  
  // Move this to properties of the algorithm but set default values.
  m_EnergyThreshold = 100; // MeV
  m_OffTimeCut = 30;   // ns
  
  m_NFlaggingWords=4;
  
  LArRodBlockPhysicsV3::resetPointers();
  m_vFragment=NULL;
  m_FebBlock=NULL;
  m_pRODblock=NULL; 

  m_Ex=0;
  m_Ey=0;
  m_Ez=0;
  m_HottestCellIndex=255; // 255 is unphysical since cells index are between 0 and 127 per FEB.
  m_HottestCellEnergy=0;

  m_CounterPtr=NULL;
  m_EnergyPtr=NULL;
  m_FebInfoDataPtr=NULL;
  m_GainPtr=NULL;
  m_RawDataFlagsPtr=NULL;
  m_RawDataPtr=NULL;
  m_SumBlkPtr=NULL;
  m_TimeQualityPtr=NULL;
}

// clear temporary block vectors 
void LArRodBlockPhysicsV3::clearBlocks()
{ 
  m_SumBlkBlock.clear();
  m_CounterBlkBlock.clear();
  m_EnergyBlock.clear();
  m_GainBlock.clear();
  m_TimeQualityBlock.clear();
  m_RawDataBlock.clear();
  m_FebInfoBlock.clear();
}

inline void LArRodBlockPhysicsV3::resetPointers() 
{
  m_RawDataCounter=0;
  m_RawDataIndex=0;
  m_CounterIndex=0;
  m_EnergyIndex=0;
  m_GainIndex=0;
  m_TimeQualityIndex=0; 
  m_SumBlkIndex=0; 
  m_FebInfoIndex=0; 

  m_numberHotCellOffTime=0;
  m_numberHotCell=0;
  m_Ex=0;
  m_Ey=0;
  m_Ez=0;
  m_HottestCellIndex=255; // 255 is unphysical since cells index are between 0 and 127 per FEB.
  m_HottestCellEnergy=0;
}

//For reading (to speed up the process)
bool LArRodBlockPhysicsV3::setPointers()
{//Set pointers to data blocks (pesuming, they exist)
 if (m_FebBlockSize>m_iHeadBlockSize)
   {
     if (LE_getHeader16(SumBlkOffset))
       m_SumBlkPtr=(const uint32_t*) (m_FebBlock+LE_getHeader16(SumBlkOffset));
       else m_SumBlkPtr=(const uint32_t*)m_DummyBitMap;

     if (LE_getHeader16(CounterBlkOffset)) 
       m_CounterPtr=(const uint16_t*)(m_FebBlock+LE_getHeader16(CounterBlkOffset));
       else m_CounterPtr=(const uint16_t*)m_DummyBitMap;
    
     if (LE_getHeader16(EBlkOffset))
       m_EnergyPtr=(const uint16_t*)(m_FebBlock+LE_getHeader16(EBlkOffset));
       else m_EnergyPtr=(const uint16_t*)m_DummyBitMap;

     if (LE_getHeader16(GainBlkOffset)) 
       m_GainPtr=(const uint32_t*)(m_FebBlock+LE_getHeader16(GainBlkOffset));
       else m_GainPtr=(const uint32_t*)m_DummyBitMap;
     
     if (LE_getHeader16(TimeQualityBlkOffset)) 
       m_TimeQualityPtr=(const int16_t*)(m_FebBlock+LE_getHeader16(TimeQualityBlkOffset));
       else m_TimeQualityPtr=(const int16_t*)m_DummyBitMap;

     if (LE_getHeader16(RawDataBlkOffset))
       m_RawDataPtr=(const int16_t*)(m_FebBlock+LE_getHeader16(RawDataBlkOffset));
       else m_RawDataPtr=(const int16_t*)m_DummyBitMap;

     if (LE_getHeader16(FebInfoBlkOffset))
       m_FebInfoDataPtr=(const int16_t*)(m_FebBlock+LE_getHeader16(FebInfoBlkOffset));
       else m_FebInfoDataPtr=(const int16_t*)m_DummyBitMap;

#ifdef  LARBSDBGOUTPUT
      m_logstr << MYLEVEL  << "***********************************************************************"<< endmsg;
      m_logstr << MYLEVEL  << "Energy Pointed values :"<< endmsg;
      m_logstr << MYLEVEL  << "************************************************************************"<< endmsg;
      m_logstr << MYLEVEL  << "LE_getHeader16(SumBlkOffset) = "<< MSG::hex << LE_getHeader16(SumBlkOffset) << endmsg;
      m_logstr << MYLEVEL  << "LE_getHeader16(CounterBlkOffset) = "<< MSG::hex << LE_getHeader16(CounterBlkOffset) << endmsg;
      m_logstr << MYLEVEL  << "LE_getHeader16(GainBlkOffset) = "<< MSG::hex << LE_getHeader16(GainBlkOffset) << endmsg;
      m_logstr << MYLEVEL  << "LE_getHeader16(EBlkOffset) = "<< MSG::hex << LE_getHeader16(EBlkOffset) << endmsg;
      m_logstr << MYLEVEL  << "LE_getHeader16(FebInfoBlkOffset) = "<< MSG::hex << LE_getHeader16(FebInfoBlkOffset) << endmsg;
      m_logstr << MYLEVEL  << "LE_getHeader16(TimeQualityBlkOffset) = "<< MSG::hex << LE_getHeader16(TimeQualityBlkOffset) << endmsg;
      m_logstr << MYLEVEL  << "************************************************************************"<< endmsg;
 
      m_logstr << MYLEVEL  << "********************* SumBlck ************************************"<< endmsg;
      for(int toto=0;toto<4;toto++)
        m_logstr << MYLEVEL  << "hexa = 0x" << MSG::hex << m_SumBlkPtr[toto] << endmsg;
      m_logstr << MYLEVEL  << "************************************************************************"<< endmsg;
 
      m_logstr << MYLEVEL  << "********************* CounterBlck ************************************"<< endmsg;
      for(int toto=0;toto<10;toto++)        m_logstr << MYLEVEL  << "hexa = 0x" << MSG::hex << m_CounterPtr[toto] << endmsg;
      m_logstr << MYLEVEL  << "************************************************************************"<< endmsg;

      m_logstr << MYLEVEL  << "********************* FebInfoBlck ************************************"<< endmsg;
      for(int toto=0;toto<4;toto++)        m_logstr << MYLEVEL  << "hexa = 0x" << MSG::hex << m_FebInfoDataPtr[toto] << endmsg;
      m_logstr << MYLEVEL  << "************************************************************************"<< endmsg;

 
      m_logstr << MYLEVEL  << "********************* Energies ************************************"<< endmsg;
      for(int toto=0;toto<128;toto++)
      m_logstr << MYLEVEL  << "hexa = 0x" << MSG::hex << m_EnergyPtr[toto] << endmsg;
      m_logstr << MYLEVEL  << "************************************************************************"<< endmsg;
 
      m_logstr << MYLEVEL  << "********************* Time & Quality ************************************"<< endmsg;
      for(int toto=0;toto<10;toto++)
        m_logstr << MYLEVEL  << "hexa = 0x" << MSG::hex << m_TimeQualityPtr[toto] << endmsg;
      m_logstr << MYLEVEL  << "************************************************************************"<< endmsg;
#endif

   }
 else
   {
     m_SumBlkPtr=NULL; 
     m_CounterPtr=NULL; 
     m_EnergyPtr=NULL; 
     m_TimeQualityPtr=NULL; 
     m_GainPtr=NULL; 
     m_FebInfoDataPtr=NULL; 
     m_RawDataPtr=NULL; 
     m_RawDataFlagsPtr=NULL; 
   }

  return true;
}

void LArRodBlockPhysicsV3::setNumberOfSamples(const uint8_t n) 
{ uint16_t oldword=LE_getVectorHeader16(NGainNSamples); 
  LE_setHeader16(NGainNSamples,(oldword & 0xFF00) | n); 
}

void LArRodBlockPhysicsV3::setNumberOfGains(const uint8_t n) 
{  uint16_t oldword=LE_getVectorHeader16(NGainNSamples); 
  LE_setHeader16(NGainNSamples,(oldword & 0x00FF) | (n<<8)); 
}



void LArRodBlockPhysicsV3::setNextEnergy(const int channel, const int32_t energy, 
					 const int32_t time, const int32_t quality, const uint32_t gain) 
{
 m_logstr << MYLEVEL  << "setNextEnergy-------------------->>>>>********************** new format V3 ***********" <<  endmsg;
 m_logstr << MYLEVEL  << "Channel=" << channel << " energy =" << energy <<   endmsg;
 int rcNb=FebToRodChannel(channel);
 //rcNb ist supposed to equal or bigger than m_EIndex.
 //In the latter case, we fill up the missing  channels with zero
 if (rcNb<m_EnergyIndex) {
   m_logstr << MSG::ERROR  << "LArRODBlockStructure ERROR: Internal error. Channels not ordered correctly. rcNb=" << rcNb
	     << " m_EnergyIndex=" << m_EnergyIndex << endmsg;
   return;
 }

 //Fill up missing channels with zeros:
 while (m_EnergyIndex<rcNb)
   setNextEnergy((int16_t)0,(int16_t)32767,(int16_t)-32767,(uint32_t)0);


 // update fi needed the hottest cell info
 // m_logstr << MYLEVEL  << "Before " << endmsg;
 // m_logstr << MYLEVEL  << "m_HottestCellIndex : "<< m_HottestCellIndex << endmsg;
 // m_logstr << MYLEVEL  << "m_HottestCellEnergy : "<< m_HottestCellEnergy << endmsg;

 if ((energy>0)&&(energy>(int)m_HottestCellEnergy)) 
   {
     m_HottestCellEnergy = abs(energy);
     m_HottestCellIndex = channel;
   }

 // m_logstr << MYLEVEL  << "After " << endmsg;
 // m_logstr << MYLEVEL  << "m_HottestCellIndex : "<< m_HottestCellIndex << endmsg;
 // m_logstr << MYLEVEL  << "m_HottestCellEnergy : "<< m_HottestCellEnergy << endmsg;


 // transform 32 bits data into 16 bits data
 
 uint16_t theenergy;
 uint32_t abse,EncodedE;
 int16_t thetime,thequality;
 int32_t sign; 

 thetime = (int16_t) time >> 4; 
 thequality = (int16_t) quality;
 
 sign=(energy>=0?1:-1); // get sign of energy
 abse=(uint32_t)abs(energy);
 
 EncodedE=abse; // range 0 

 if ((abse>8192)&&(abse<65536))
   {
     EncodedE=((abse>>3)|0x2000); // range 1 : drop last 3 bits and put range bits (bits 14 and 13 = 01)
   }
 else if ((abse>65535)&&(abse<524288))
   {
     EncodedE=((abse>>6)|0x4000); // range 2 : drop last 6 bits and put range bits (bits 14 and 13 = 10)
   }
 else if ((abse>524288))
   {
     EncodedE=((abse>>9)|0x6000); // range 3 : drop last 9 bits and put range bits (bits 14 and 13 = 11)
   }

 // treat sign now :

 if (sign<0) EncodedE |= 0x8000;
 theenergy = (uint16_t) EncodedE;


 // Add data...

 m_logstr << MYLEVEL  << "setNextEnergy-------------------->>>>> Energy = "<< energy << "   Encoded Energy =" << theenergy  << endmsg; 

 if (abse> m_EnergyThreshold)
   {
     setNextEnergy(theenergy,thetime,thequality,gain); 
   }
 else
   {
     setNextEnergy(theenergy,(int16_t)32767,(int16_t)-32767,gain);
   }
 return;
}

//Private function, expects channel number is rod-style ordering
void LArRodBlockPhysicsV3::setNextEnergy(const uint16_t energy,const int16_t time, const int16_t quality, const uint32_t gain)
{

  if (m_EnergyIndex>=m_channelsPerFEB)        //Use m_EIndex to count total number of channels
    {m_logstr << MSG::ERROR  << "LArRodBlockStructure ERROR: Attempt to write Energy for channel " 
	    << m_EnergyIndex << " channels into a FEB!" <<endmsg;
      return;
    }
  LARBSDBG("LArRodBlockStructure: Setting Energy for channel " << m_EnergyIndex << ". E=" << energy);
  
  m_logstr << MYLEVEL  << "In setNextEnergy-------------------->>>>> time = " << time << " quality=" << quality << endmsg; 

  // write for all cells energy and hardware gain 
  
  // Energy
  
  m_EnergyIndex++; //Use m_EIndex to count the channels put in the Energy block

  ShortLong twoValues{};
  if (m_EnergyIndex%2) // m_EIndex-1 is even
    {
      twoValues.s[0]= energy;
      twoValues.s[1]=0;
    }
  else 
    {                                //Even number: Merging with previous block
      uint32_t oneValue=m_EnergyBlock[m_EnergyBlock.size()-1]; //Take last element of vector
      m_EnergyBlock.pop_back();
      uint16_t* valptr=reinterpret_cast<uint16_t*>(&oneValue);
      twoValues.s[0]=valptr[0];
      twoValues.s[1]=(uint16_t)energy;
    }
  
  //m_logstr << MSG::DEBUG  << "In setNextEnergy-------------------->>>>> Length of m_EnergyBlock =" << m_EnergyBlock.size() << endmsg; 

  LARBSDBG("Writing words: val0= " << twoValues.s[0] << " val1= " << twoValues.s[1]);
  m_EnergyBlock.push_back(twoValues.l);
  LARBSDBG("Writing Raw data to E block. E=" << energy);
  
  // update summary block 
  // Summary block is one bit per cell
  
  int mylocalBitwiseIndex = (int) (m_EnergyIndex-1)%32; // 1 bits in SB per channel
  //  m_logstr << MYLEVEL  << "In setNextEnergy-------------------->>  Summary Block Construction "  << endmsg; 
  //  m_logstr << MYLEVEL  << "In setNextEnergy-------------------->>  mylocalBitwiseIndex= " << mylocalBitwiseIndex << endmsg; 
  if (mylocalBitwiseIndex==0) // need for a new 32 bits word to store SM info of that cell
    {
      // m_logstr << MYLEVEL  << "In setNextEnergy-------------------->>  Set new 32 bits word "  << endmsg; 
      uint32_t SB;
      SB = (quality==-32767?0x00000000:0x00000001);
      m_SumBlkBlock.push_back(SB);
    }
  else 
    {
      // m_logstr << MYLEVEL  << "In setNextEnergy-------------------->>  Use existing 32 bits word "  << endmsg; 
      uint32_t SB=m_SumBlkBlock[m_GainBlock.size()-1]; //Take out last element of vector
      m_SumBlkBlock.pop_back();
      SB=(SB<<1)|(quality==-32767?0x00000000:0x00000001);
      m_SumBlkBlock.push_back(SB);
    }
  
  //  Hardware gain
  
  mylocalBitwiseIndex = (m_EnergyIndex-1)%16; // 2 bits of gain per channel
  if (mylocalBitwiseIndex==0) // need for a new 32 bits word to store the gain of that cell
    {
      m_GainBlock.push_back(gain&0x00000003);
    }
  else 
    {
      uint32_t encodedgains=m_GainBlock[m_GainBlock.size()-1]; //Take out last element of vector
      m_GainBlock.pop_back();
      encodedgains=(encodedgains<<2)|(gain&0x00000003);
      m_GainBlock.push_back(encodedgains);
    }
  
  // write Time and Chi2 for cells above HighEnergyCellCut threshold
  
  // m_logstr << MYLEVEL  << "In setNextEnergy-------------------->>>>> treat Time and Chi2"  << endmsg; 

  if (quality!=-32767) // Do write Time and Chi2 information
    {
      // m_logstr << MSG::DEBUG  << "In setNextEnergy-------------------->>>>> treat Time and Chi2 of that hot cell "  << endmsg; 
      // count the number of hot cells
      m_numberHotCell++;
      // count the number of cells offtime
      if (abs(time)>m_OffTimeCut) m_numberHotCellOffTime++;
      
      mylocalBitwiseIndex = (m_numberHotCell-1)%2; // 16 bits per channel
      ShortLong twoValues{};
      if (mylocalBitwiseIndex==0) // need for a new 32 bits word to store the time quality of that cell
	{
	  twoValues.s[0]=0;
	  twoValues.s[1]=0;
	  twoValues.s[0]=(int16_t)(time)|((quality>>10)&0x003F); // Quality on the first 6 bits and Time on the last 10 bits;
	}
      else 
	{ //Even number: Merging with previous block
	  uint32_t oneValue=m_TimeQualityBlock[m_TimeQualityBlock.size()-1]; //Take last element of vector
	  m_TimeQualityBlock.pop_back();
	  int16_t* valptr=reinterpret_cast<int16_t*>(&oneValue);
	  twoValues.s[0]=valptr[0];
	  twoValues.s[1]=(int16_t)(time)|((quality>>12)&0x003F); // Quality on the first 4 bits and Time on the last 12 bits;
	}
      m_TimeQualityBlock.push_back(twoValues.l);
    }
  
}

void LArRodBlockPhysicsV3::setRawData(const int channel, const std::vector<short>& samples, const uint32_t gain) {
 //Convert Feb to Rod Channel Number:
 //int rcNb=(channel>>3) + ((channel&0x7)<<4);
 int rcNb=FebToRodChannel(channel);
 if (rcNb>=m_channelsPerFEB) 
   {m_logstr << MSG::ERROR << "Attempt to write Energy for channel " << rcNb << " channels into a FEB!" << endmsg;
    return;
   } 
 unsigned int nsamples = LE_getVectorHeader16(NGainNSamples) & 0x00FF;
 if(samples.size() != nsamples) {
   m_logstr << MSG::ERROR << "Number of samples mismatch!\n";
   m_logstr << "  nsamples       =" << nsamples;
   m_logstr << "  samples.size() =" << samples.size() << endmsg;
   std::abort();
 }
 
 setBit(&m_RawDataBlock[0],rcNb);
 //Samples have 12 bit and are shifted to the left by 2 bits.
 // odd samples in high bits, even samples in low bits
 if((nsamples/2)*2!=nsamples) { //odd number of samples - gain is alone
   m_RawDataBlock.push_back((gain<<30) | samples[0]<<2); 
   for (unsigned int i=1;i<nsamples;i+=2)
     m_RawDataBlock.push_back((samples[i+1]<<18) | samples[i]<<2); 
 }
 else { //even number of samples - gain is packed with sample 0
   m_RawDataBlock.push_back((gain<<30) | (samples[1]<<18) | samples[0]<<2); 
   for (unsigned int i=2;i<nsamples;i+=2)
     m_RawDataBlock.push_back((samples[i+1]<<18) | samples[i]<<2); 
 }
}


//For writing: takes existing Fragment and splits it into Feb-Blocks
void LArRodBlockPhysicsV3::initializeFragment(std::vector<uint32_t>& fragment)
{
  m_pRODblock=&fragment; //remember pointer to fragment
  if (fragment.size()>m_iHeadBlockSize) {  //Got filled fragment
    unsigned int sizeRead=0;
    //Store existing data in the FEB-Map
    while (sizeRead<fragment.size()) {
      std::vector<uint32_t>::iterator FebIter;
      FebIter=fragment.begin()+sizeRead;     //Store pointer to current Feb-Header
      m_FebBlock=&(*FebIter); //Set m_FebBlock in order to use getHeader-functions.
      uint32_t currFEBid=getHeader32(FEBID); //Get this FEB-ID
      uint16_t currFebSize=getNumberOfWords(); //Size of this FEB-Block
      //std::cout << "FebID=" << currFEBid << " FEBSize=" << currFebSize << " Vector size=" << fragment.size() << std::endl;
      if (FebIter+currFebSize>fragment.end()) {
	fragment.clear(); //Clear existing vector
	m_logstr << MSG::ERROR  << "Got inconsistent ROD-Fragment!" << endmsg; 
	return;
      }
      m_mFebBlocks[currFEBid].assign(FebIter,FebIter+currFebSize); //Copy data from ROD-fragment into FEB-Block
      sizeRead+=currFebSize+m_MiddleHeaderSize;  //6 is the middle header size
      LARBSDBG("Found FEB-id " << currFEBid << " in existing ROD-Fragment");
    } // end while
  }
  fragment.clear(); //Clear existing vector
  return;
}

//For writing: Initalizes a single FEB-Block
void LArRodBlockPhysicsV3::initializeFEB(const uint32_t id)
{ 
 m_vFragment=&(m_mFebBlocks[id]);
 if (m_vFragment->size()<m_iHeadBlockSize) //Got empty or spoiled fragment
  {
    m_vFragment->resize(m_iHeadBlockSize,0); //Initialize FEB-Header
    setHeader32(FEBID,id);                //Set Feb ID
  }
 
 m_SumBlkBlock.resize(0);         
 m_CounterBlkBlock.resize(0);         
 m_EnergyBlock.resize(0);         
 m_GainBlock.resize(0);         
 m_FebInfoBlock.resize(0);         
 m_RawDataBlock.resize(0);

 resetPointers();

}

void LArRodBlockPhysicsV3::finalizeFEB()
{
//Complete non-complete Energy block
  while (m_EnergyIndex<m_channelsPerFEB)
    setNextEnergy((uint16_t)0,(int16_t)32767,(int32_t)-32767,(uint32_t)0);//E=0,t=32767,q=-32767,G=0

 uint16_t n;
 uint16_t BlockOffset;

 // Summary block....
 n = m_SumBlkBlock.size();
 BlockOffset=LE_getVectorHeader16(SumBlkOffset);
 //Check if Summary Block exists and is not yet part of the fragment
 LARBSDBG("Checking for Summary Block n=" << n << "BlockOffset=" << BlockOffset);
 if (n && !BlockOffset)     
   {
     LE_setHeader16(SumBlkOffset,m_vFragment->size());
     BlockOffset=LE_getVectorHeader16(SumBlkOffset);
          m_logstr << MSG::DEBUG << "In finalizeFEB-------------------->>>>> " << "Checking for Summary Block :  length= " << n << "  BlockOffset=" << BlockOffset << endmsg; 
     for (unsigned i=0;i<n;i++)
       m_vFragment->push_back(m_SumBlkBlock[i]);
   }

 // Counter block....

 // fill info from counters
 ShortLong twoValues{};
 twoValues.s[0]=(uint16_t)m_numberHotCell;
 twoValues.s[1]=(uint16_t)m_numberHotCellOffTime;
 m_CounterBlkBlock.push_back(twoValues.l);

 twoValues.s[0]=(uint16_t)m_EnergyThreshold;
 twoValues.s[1]=(uint16_t)m_OffTimeCut;
 m_CounterBlkBlock.push_back(twoValues.l);

 // Write the Energy threshold used to determine which cell is a hot cell in the first 
 // 32 bit word of the counters
 // Write the Energy Ex of the ROD block in a 32 bits word
 // Write the Energy Ey of the ROD block in a 32 bits word
 // Ex and Ey have to be 32 bits words as they are sums of multiple 16 bits words




/* uint32_t aux;
 aux = abs(m_Ex);
 if (m_Ex<0) aux|=0x80000000;
 m_CounterBlkBlock.push_back(aux);
 aux = abs(m_Ey);
 if (m_Ey<0) aux|=0x80000000;
 m_CounterBlkBlock.push_back(aux);
*/

 uint32_t* aux = (uint32_t*)&m_Ex;
 m_CounterBlkBlock.push_back(*aux);
 aux = (uint32_t*)&m_Ey;
 m_CounterBlkBlock.push_back(*aux);

 // write the hottest cell index for that FEB and its energy in MeV

 twoValues.s[0]=(uint16_t)((m_HottestCellIndex<<9)|((uint16_t)((m_HottestCellEnergy>>16))&0x01FF));
 twoValues.s[1]=(uint16_t)m_HottestCellEnergy;
 m_CounterBlkBlock.push_back(twoValues.l);

 aux = (uint32_t*)&m_Ez;
 m_CounterBlkBlock.push_back(*aux);

 n = m_CounterBlkBlock.size();
 BlockOffset=LE_getVectorHeader16(CounterBlkOffset);
 //Check if Counter Block exists and is not yet part of the fragment
 LARBSDBG("Checking for Counter Block n=" << n << "BlockOffset=" << BlockOffset);
 if (n && !BlockOffset)     
   {
     LE_setHeader16(CounterBlkOffset,m_vFragment->size());
     BlockOffset=LE_getVectorHeader16(CounterBlkOffset);
     m_logstr << MSG::INFO  << "In finalyseFEB-------------------->>>>> " << "Checking for Counter Block :  length= " << n << "  BlockOffset=" << BlockOffset << endmsg; 
     for (unsigned i=0;i<n;i++)
       m_vFragment->push_back(m_CounterBlkBlock[i]);
   }

 // Energy block...
 n = m_EnergyBlock.size();
 BlockOffset=LE_getVectorHeader16(EBlkOffset);
 LARBSDBG("Checking Energy Block n=" << n << "BlockOffset=" << BlockOffset);
 //Check if Energy-Block exists and is not yet part of the fragment
 if (n && !BlockOffset)
   {
     LE_setHeader16(EBlkOffset,m_vFragment->size());
     BlockOffset=LE_getVectorHeader16(EBlkOffset);
     m_logstr << MSG::DEBUG  << "In finalyseFEB-------------------->>>>> " << "Checking for Energy Block :  length= " << n << "  BlockOffset=" << BlockOffset << endmsg; 
     //m_logstr << MSG::DEBUG  << "In finalyseFEB-------------------->>>>> " << "Checking for Energy Block :  m_EnergyIndex= " << m_EnergyIndex << endmsg; 
     for(unsigned int i=0;i<n;i++)
       m_vFragment->push_back(m_EnergyBlock[i]);
   } 

 // Gain block...
 n = m_GainBlock.size();
 BlockOffset=LE_getVectorHeader16(GainBlkOffset);
 LARBSDBG("Checking Gain Block n=" << n << "BlockOffset=" << BlockOffset);
 //Check if Gain-Block exists and is not yet part of the fragment
 if (n && !BlockOffset)
   {
     LE_setHeader16(GainBlkOffset,m_vFragment->size());
     BlockOffset=LE_getVectorHeader16(GainBlkOffset);
     //m_logstr << MSG::DEBUG  << "In finalyseFEB-------------------->>>>> " << "Checking for Gain Block :  length= " << n << "  BlockOffset=" << BlockOffset << endmsg; 
     for(unsigned int i=0;i<n;i++)
       m_vFragment->push_back(m_GainBlock[i]);
   } 
 

 // FeBInfo Block... // offline encoder just put dummy info there
 for (int nword=0; nword<m_NFebInfoBlkWords ; nword++)
   m_FebInfoBlock.push_back(0L);

 n = m_FebInfoBlock.size();
 BlockOffset=LE_getVectorHeader16(FebInfoBlkOffset);
 LARBSDBG("Checking FebInfo Block n=" << n << "BlockOffset=" << BlockOffset);
 //Check if Gain-Block exists and is not yet part of the fragment
 if (n && !BlockOffset)
   {
     LE_setHeader16(FebInfoBlkOffset,m_vFragment->size());
     BlockOffset=LE_getVectorHeader16(FebInfoBlkOffset);
     //m_logstr << MSG::DEBUG  << "In finalyseFEB-------------------->>>>> " << "Checking for FEB Info Block :  length= " << n << "  BlockOffset=" << BlockOffset << endmsg; 
     for(unsigned int i=0;i<n;i++)
       m_vFragment->push_back(m_FebInfoBlock[i]);
   } 

 // Time and Quality block
 n = m_TimeQualityBlock.size();
 BlockOffset=LE_getVectorHeader16(TimeQualityBlkOffset);
 LARBSDBG("Checking Time and Quality Block n=" << n << "BlockOffset=" << BlockOffset);
 //Check if Time and Quality Block exists and is not yet part of the fragment
 if (n && !BlockOffset)
   {
     LE_setHeader16(TimeQualityBlkOffset,m_vFragment->size());
     BlockOffset=LE_getVectorHeader16(TimeQualityBlkOffset);
     m_logstr << MSG::DEBUG  << "In finalyseFEB-------------------->>>>> " << "Checking for Time and Quality Block :  length= " << n << "  BlockOffset=" << BlockOffset << endmsg; 
     for(unsigned int i=0;i<n;i++)
       m_vFragment->push_back(m_TimeQualityBlock[i]);
   } 
 
 // Raw data
 LARBSDBG("Checking Raw Data Block");
 n = m_RawDataBlock.size();
 BlockOffset=LE_getVectorHeader16(RawDataBlkOffset);
 LARBSDBG("Checking Raw Data Block. n=" << n << "BlockOffset=" << BlockOffset);
 //Check if Raw Data block exists and is not yet part of the fragment
 if ((n>m_NFlaggingWords) && (!BlockOffset))
   {
     LE_setHeader16(RawDataBlkOffset,m_vFragment->size());
     for(unsigned int i=0;i<n;i++)
       m_vFragment->push_back(m_RawDataBlock[i]);
   }
 setHeader32(NWTot,m_vFragment->size());

 // m_logstr << MSG::DEBUG  << "############################################################################>>>>>  " << endmsg;
 // m_logstr << MSG::DEBUG  << "In finalyseFEB-------------------->>>>> Ofsets summary  (en hexa) : " << endmsg;
 // m_logstr << MSG::DEBUG  << "Summary Block Offset : " << LE_getVectorHeader16(SumBlkOffset) << endmsg;
 // m_logstr << MSG::DEBUG  << "Counter Block Offset : " << LE_getVectorHeader16(CounterBlkOffset) << endmsg;
 // m_logstr << MSG::DEBUG  << "Energy Block Offset  : " << LE_getVectorHeader16(EBlkOffset) << endmsg;
 // m_logstr << MSG::DEBUG  << "Gain Block Offset    : " << LE_getVectorHeader16(GainBlkOffset) << endmsg;
 // m_logstr << MSG::DEBUG  << "Gain Block Offset    : " << LE_getVectorHeader16(FebInfoBlkOffset) << endmsg;
 // m_logstr << MSG::DEBUG  << "Raw Data Block Offset: " << LE_getVectorHeader16(RawDataBlkOffset) << endmsg;
 // m_logstr << MSG::DEBUG  << "Number of hot cells  : " << m_numberHotCell << endmsg;
 // m_logstr << MSG::DEBUG  << "############################################################################>>>>>  " << endmsg;

 m_logstr << MYLEVEL  << "***********************************************************************"<< endmsg;
 m_logstr << MYLEVEL  << "m_HottestCellIndex : "<< m_HottestCellIndex << endmsg;
 m_logstr << MYLEVEL  << "m_HottestCellEnergy : "<< m_HottestCellEnergy << endmsg;
 m_logstr << MYLEVEL  << "***********************************************************************"<< endmsg;
 m_logstr << MYLEVEL  << "m_Ex : "<< m_Ex << endmsg;
 m_logstr << MYLEVEL  << "m_Ey : "<< m_Ey << endmsg;
 m_logstr << MYLEVEL  << "***********************************************************************"<< endmsg;


 clearBlocks();
 return;
}


void  LArRodBlockPhysicsV3::concatinateFEBs()
{
  //std::cout << "Concatinating FEBs. Have "<< m_mFebBlocks.size() <<" febs." << std::endl;
 FEBMAPTYPE::const_iterator feb_it_b=m_mFebBlocks.begin();
 FEBMAPTYPE::const_iterator feb_it_e=m_mFebBlocks.end();
 FEBMAPTYPE::const_iterator feb_it;
 for (feb_it=feb_it_b;feb_it!=feb_it_e;++feb_it) {
   if (feb_it!=feb_it_b) //Not first Feb
/*
     if (fullHeader) {//Add middle header
       m_pRODblock->push_back(fullHeader->version().full());//Format Version number
       m_pRODblock->push_back(fullHeader->source_id());  //Source identifer
       m_pRODblock->push_back(fullHeader->run_no());
       m_pRODblock->push_back(fullHeader->lvl1_id());    //Level 1 identifer
       m_pRODblock->push_back(fullHeader->bc_id());      //Bunch Crossing identifer 
       m_pRODblock->push_back(fullHeader->lvl1_type());  //Level 1 trigger type
       m_pRODblock->push_back(fullHeader->detev_type()); //Detector event type
     }
     else //No ROD-Header
*/
       m_pRODblock->resize( m_pRODblock->size()+m_MiddleHeaderSize);
   
   //Add feb data to rod data block
   m_pRODblock->insert (m_pRODblock->end(),
                        feb_it->second.begin(), feb_it->second.end());
 } //end for feb_it

  m_mFebBlocks.clear();
  return;
}

int LArRodBlockPhysicsV3::getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain)
{
  LARBSDBG("in LArRodBlockPhysicsV3::getNextRawData.");
  LARBSDBG("m_RawDataCounter=" << m_RawDataCounter << "  m_RawDataIndex="<<  m_RawDataIndex); 
  LARBSDBG("m_channelsPerFEB=" << m_channelsPerFEB);
  if(m_FebBlockSize<=m_iHeadBlockSize) return 0;
  if(LE_getHeader16(RawDataBlkOffset)<=m_iHeadBlockSize) return 0;

  const int flags=(int) LE_getHeader16(RawDataBlkOffset);
  if (!flags)
    return 0;                                              //Block not existing
  if (m_RawDataCounter>=m_channelsPerFEB)                  //Already beyond maximal number of channels
    return 0; 
  
  LARBSDBG("Flags="<<flags);

  while (!getBit(m_RawDataFlagsPtr,m_RawDataCounter))      //Look for next filled channel
    {++m_RawDataCounter;
     LARBSDBG("RawDataCounter ist now " << m_RawDataCounter);
     if (m_RawDataCounter>=m_channelsPerFEB)              //No more channel available
       return 0;
    }
  LARBSDBG("Found filled channel at positon " << m_RawDataCounter);
  //Found next filled channel
  channelNumber=m_RawDataCounter;
  //channelNumber=(m_RawDataCounter>>4) + ((m_RawDataCounter&0xf)<<3); //Convert ROD to FEB channel ordering
  unsigned int nsamples = LE_getHeader16(NGainNSamples) & 0x00FF;
  LARBSDBG("This run has " << nsamples <<  " samples");
  int index = m_RawDataIndex*nsamples;
  LARBSDBG( "index="<<index);
  ++m_RawDataIndex;
  LARBSDBG("In getnextRawData(). index= " << index);
  gain=3-((m_GainPtr[channelNumber/16] >> (channelNumber%16)*2) & 0x3);
  if(gain>=CaloGain::LARNGAIN) return 0;
  for(unsigned int i=0;i<nsamples;i++) {
    uint16_t x;
    if((index+i)%2==0) 
      x=m_RawDataPtr[index+i+1];
    else 
      x=m_RawDataPtr[index+i-1];
    samples.push_back((short) (x & 0xfff));
  }
  ++m_RawDataCounter;

  if (m_rearrangeFirstSample && m_rearrangeFirstSample<samples.size()) //FIXME: Very ugly hack! See explanation in LArRodDecoder.h file
      {//Change e.g. 3 0 1 2 4 to 0 1 2 3 4 
	short movedSample=samples[0];
	for (unsigned i=1;i<=m_rearrangeFirstSample;i++)
	  samples[i-1]=samples[i];
	samples[m_rearrangeFirstSample]=movedSample;
      }

  return 1;
}




//Sort functions & ordering relation:
template<class RAWDATA>
bool LArRodBlockPhysicsV3::operator () 
  //(const LArRawChannel* ch1, const LArRawChannel* ch2) const
  (const RAWDATA* ch1, const RAWDATA* ch2) const
{
  HWIdentifier id1 = ch1->channelID(); 
  HWIdentifier id2 = ch2->channelID(); 
  
  HWIdentifier febId1= m_onlineHelper->feb_Id(id1); 
  HWIdentifier febId2= m_onlineHelper->feb_Id(id2); 

  if(febId1 == febId2 ){
    int cId1 =  m_onlineHelper->channel(id1); 
    int cId2 =  m_onlineHelper->channel(id2);
    return FebToRodChannel(cId1) < FebToRodChannel(cId2);
  } 

  return febId1 < febId2 ; 
}

void LArRodBlockPhysicsV3::sortDataVector(std::vector<const LArRawChannel*>& vRC)
{std::sort(vRC.begin(),vRC.end(),*this);}

void LArRodBlockPhysicsV3::sortDataVector( std::vector<const LArDigit*>& vDigit) 
{std::sort(vDigit.begin(),vDigit.end(),*this);
}

uint32_t LArRodBlockPhysicsV3::getNumberOfSamples()  const
{ 
  return LE_getHeader16(NGainNSamples)&0xff; 
}

uint32_t LArRodBlockPhysicsV3::getNumberOfGains()  const
{ 
  return  LE_getHeader16(NGainNSamples)>>8;
}

uint32_t LArRodBlockPhysicsV3::getRadd(uint32_t /*adc*/, uint32_t sample)  const
{ 
  int index=LE_getHeader16(RawDataBlkOffset)-3+(sample+1)/2;
  uint32_t x=m_FebBlock[index];
  //if(sample&0x1) x=x&0xffff;
  //else
  x=x&0xffff;
  return x;
}

uint16_t LArRodBlockPhysicsV3::getCtrl1(uint32_t /*adc*/)  const
{ 
  int index=LE_getHeader16(RawDataBlkOffset)-4;
  uint32_t x=m_FebBlock[index];
  x=x>>16;
  uint16_t ctrl=x;
  return ctrl;
}

uint16_t LArRodBlockPhysicsV3::getCtrl2(uint32_t /*adc*/)  const
{ 
  int index=LE_getHeader16(RawDataBlkOffset)-4;
  uint32_t x=m_FebBlock[index];
  x=x&0xffff;
  uint16_t ctrl=x;
  return ctrl;
}

uint16_t LArRodBlockPhysicsV3::getCtrl3(uint32_t /*adc*/)  const
{ 
  int index=LE_getHeader16(RawDataBlkOffset)-3;
  uint32_t x=m_FebBlock[index];
  x=x>>16;
  uint16_t ctrl=x;
  return ctrl;
}

uint32_t LArRodBlockPhysicsV3::getStatus()  const
{ 
  if(getNumberOfWords()<Status2/2) return 0;
  uint32_t x=getHeader32(Status2);
  return x;
}


void LArRodBlockPhysicsV3::setEThreshold(uint16_t thres)
{
  m_EnergyThreshold=thres;
}

void LArRodBlockPhysicsV3::setOffTimeCut(uint16_t TimeCut)
{
m_OffTimeCut=TimeCut;
}

void LArRodBlockPhysicsV3::setEx(double Ex) 
{
  m_Ex=(int32_t)Ex;
  return;
}
void LArRodBlockPhysicsV3::setEy(double Ey) 
{
  m_Ey=(int32_t)Ey;
  return;
}

void LArRodBlockPhysicsV3::setEz(double Ez) 
{
  m_Ez=(int32_t)Ez;
  return;
}

