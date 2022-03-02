/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Implementation of the LArRODBlockStructure_3 class
// This version contains LArRawChannels (=E,t,Q)
// and LArDigits (=5 time samples)
// See .h file for more details.

#include "LArByteStream/LArRodBlockPhysicsV0.h"
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

const uint32_t LArRodBlockPhysicsV0::m_DummyBitMap[4]={0xffffffff,0xffffffff,0xffffffff,0xffffffff};

LArRodBlockPhysicsV0::LArRodBlockPhysicsV0() : LArRodBlockStructure(),
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
 m_NFlaggingWords=4; 
 LArRodBlockPhysicsV0::resetPointers();
 m_vFragment=NULL;
 m_FebBlock=NULL;
 m_pRODblock=NULL; 
 m_FlagPtr=NULL;
 m_HighEPtr=NULL;
 m_LowEPtr=NULL;
}

// clear temporary block vectors 
void LArRodBlockPhysicsV0::clearBlocks()
{ m_HighEnergyBlock.clear();
  m_LowEnergyBlock.clear();
  m_RawDataBlock.clear();
}

inline void LArRodBlockPhysicsV0::resetPointers() 
{m_ECounter=0;
 m_RawDataCounter=0;
 m_LowEIndex=0;
 m_RawDataIndex=0;
 m_HighEIndex=0;
 m_EIndex=0;
}

//For reading (to speed up the process)
bool LArRodBlockPhysicsV0::setPointers()
{//Set pointers to data blocks (presuming, they exist)
 if (m_FebBlockSize>m_iHeadBlockSize)
   {if (LE_getHeader16(LowEBlkOffset))
      {m_FlagPtr=m_FebBlock+LE_getHeader16(LowEBlkOffset);
       m_LowEPtr=(const int16_t*)(m_FlagPtr+m_NFlaggingWords);
      }
    else 
      {//m_FlagPtr=NULL;
       //m_LowEPtr=NULL;
       //Bugfix, 9.8.2004, WL: Set pointer to dummy map to read FEB with only high energy block
	m_FlagPtr=m_DummyBitMap;
	m_LowEPtr=(const int16_t*)m_FlagPtr;
      }
    if (LE_getHeader16(HighEBlkOffset))
      m_HighEPtr=(const int32_t*)m_FebBlock+LE_getHeader16(HighEBlkOffset);
    else
      m_HighEPtr=NULL;
   }
 else
   {m_FlagPtr=NULL;
    m_LowEPtr=NULL;
    m_HighEPtr=NULL;
   }
#ifdef LARBSDBGOUTPUT
 std::cout << "Fragment offsets: (m_NFlaggingWords=" << m_NFlaggingWords << ")" << std::endl;
 std::cout << "Raw Data: " << LE_getHeader16(RawDataBlkOffset)<< std::endl;
 if (m_FlagPtr)
   std::cout << "Flags: " << LE_getHeader16(LowEBlkOffset)<< std::endl;
 else
   std::cout << "Flags: not present" << std::endl;

 if (m_LowEPtr)
   std::cout << "Low Energy: " << LE_getHeader16(LowEBlkOffset)+m_NFlaggingWords << std::endl;
 else
   std::cout << "Low Energy: not present" << std::endl;


 if (m_HighEPtr)
   std::cout << "High Energy: " << LE_getHeader16(HighEBlkOffset)<< std::endl;
 else
   std::cout << "High Energy: not present" << std::endl;
#endif
 return true;
}

void LArRodBlockPhysicsV0::setNumberOfSamples(const uint8_t n) 
{ uint16_t oldword=LE_getVectorHeader16(NGainNSamples); 
  LE_setHeader16(NGainNSamples,(oldword & 0xFF00) | n); 
}

void LArRodBlockPhysicsV0::setNumberOfGains(const uint8_t n) 
{  uint16_t oldword=LE_getVectorHeader16(NGainNSamples); 
  LE_setHeader16(NGainNSamples,(oldword & 0x00FF) | (n<<8)); 
}


void LArRodBlockPhysicsV0::setNextEnergy(const int channel, const int32_t energy, 
					 const int32_t time, const int32_t quality, const uint32_t gain) {
 int rcNb=FebToRodChannel(channel);
 //rcNb ist supposed to equal or bigger than m_EIndex.
 //In the latter case, we fill up the missing  channels with zero
 if (rcNb<m_EIndex) {
   m_logstr << MSG::ERROR  << "LArRODBlockStructure ERROR: Internal error. Channels not ordered correctly. rcNb=" << rcNb
	     << " m_EIndex=" << m_EIndex << endmsg;
   return;
 }
 //Fill up missing channels with zeros:
 while (m_EIndex<rcNb)
   setNextEnergy(0,0,-1,0);
 //Add data...
 setNextEnergy(energy,time,quality,gain);
 //std::cout << "SetNextEnergy " << channel << " : " << energy << std::endl;
 return;
}
//Private function, expects channel number is rod-style ordering
void LArRodBlockPhysicsV0::setNextEnergy(const int32_t energy, const int32_t time, const int32_t quality, const uint32_t gain)
{
 if (m_EIndex>=m_channelsPerFEB)        //Use m_EIndex to count total number of channels
  {m_logstr << MSG::ERROR  << "LArRodBlockStructure ERROR: Attempt to write Energy for channel " 
	  << m_EIndex << " channels into a FEB!" <<endmsg;
   return;
  }
 LARBSDBG("LArRodBlockStructure: Setting Energy for channel " << m_EIndex << ". E=" << energy); 
 if (quality<0 && energy<0x7FFE && gain==0) {     //Write into Low Energy block
   m_LowEIndex++;                       //Use m_LowEIndex to count the channels in the Low Energy block
   ShortLong twoValues{};
   twoValues.s[0]=0;
   twoValues.s[1]=0;
   if (m_LowEIndex%2==1) {            //This is an odd number, simply add data at the bottom of the block
     twoValues.s[0]=(int16_t)energy;
   }
   else {                                //Even number: Merging with previous block
     uint32_t oneValue=m_LowEnergyBlock[m_LowEnergyBlock.size()-1]; //Take last element of vector
     m_LowEnergyBlock.pop_back();
     int16_t* valptr=reinterpret_cast<int16_t*>(&oneValue);
     twoValues.s[0]=valptr[0];
     twoValues.s[1]=(int16_t)energy;
   }
   LARBSDBG("Writing words: val0= " << twoValues.s[0] << " val1= " << twoValues.s[1]);
   m_LowEnergyBlock.push_back(twoValues.l);
   LARBSDBG("Writing Raw data to Low E block. E=" << energy);
 }
 else                                          //Write into High Energy block
   {setBit(&m_LowEnergyBlock[0],m_EIndex); 
    m_HighEnergyBlock.push_back(*((uint32_t*)(&energy)));
    uint32_t t_sign;
    uint32_t abs_time;
    if (time<0)
      t_sign=1;
    else
      t_sign=0;
    abs_time=abs(time);
    if (abs_time>0x1fff)
      abs_time=0x1fff;
    //uint32_t gtQ = (gain << 30) | ((time & 0x3fff)<<16) | (0xffff & quality);
    uint32_t gtQ = (gain << 30) | (t_sign<<29) | ((abs_time & 0x1fff)<<16) | (0xffff & quality);
    m_HighEnergyBlock.push_back(gtQ); 
    LARBSDBG("Writing Raw data to High E block. E=" << energy << " Q=" << quality);
   }
 m_EIndex++;
 // if (energy>0)
 //m_ECounter++;
}


void LArRodBlockPhysicsV0::setRawData(const int channel, const std::vector<short>& samples, const uint32_t gain) {
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
void LArRodBlockPhysicsV0::initializeFragment(std::vector<uint32_t>& fragment)
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
      sizeRead+=currFebSize+m_MiddleHeaderSize;//6 is the middle header size
      LARBSDBG("Found FEB-id " << currFEBid << " in existing ROD-Fragment");
    } // end while
  }
  fragment.clear(); //Clear existing vector
  return;
}

//For writing: Initalizes a single FEB-Block
void LArRodBlockPhysicsV0::initializeFEB(const uint32_t id)
{ m_vFragment=&(m_mFebBlocks[id]);
 if (m_vFragment->size()<m_iHeadBlockSize) //Got empty or spoiled fragment
  {m_vFragment->resize(m_iHeadBlockSize,0); //Initialize FEB-Header
   setHeader32(FEBID,id);                //Set Feb ID
  }
 
 m_LowEnergyBlock.assign(m_NFlaggingWords,0);
 m_RawDataBlock.assign(m_NFlaggingWords,0);
 resetPointers();
}

void LArRodBlockPhysicsV0::finalizeFEB()
{
//Complete non-complete Energy block
  while (m_EIndex<m_channelsPerFEB)
    setNextEnergy(0,0,-1,0);//E=0,t=0,q=-1,G=0
 // Energies
 unsigned int n;
 uint16_t BlockOffset;
 //Low energy block....
 n = m_LowEnergyBlock.size();
 BlockOffset=LE_getVectorHeader16(LowEBlkOffset);
 //Check if Low Energy Block exists and is not yet part of the fragment
 LARBSDBG("Checking Low Energy Block n=" << n << "BlockOffset=" << BlockOffset);
 if (n>m_NFlaggingWords && !BlockOffset)     
   {LE_setHeader16(LowEBlkOffset,m_vFragment->size());
    for (unsigned i=0;i<n;i++)
      m_vFragment->push_back(m_LowEnergyBlock[i]);
   }
 //High energy block...
 n = m_HighEnergyBlock.size();
 BlockOffset=LE_getVectorHeader16(HighEBlkOffset);
 LARBSDBG("Checking High Energy Block n=" << n << "BlockOffset=" << BlockOffset);
 //Check if High Energy-Block exists and is not yet part of the fragment
 if (n && !BlockOffset)
   {LE_setHeader16(HighEBlkOffset,m_vFragment->size());
   for(unsigned int i=0;i<n;i++)
      m_vFragment->push_back(m_HighEnergyBlock[i]);
   } 
 
 // Raw data
 LARBSDBG("Checking Raw Data Block");
 n = m_RawDataBlock.size();
 BlockOffset=LE_getVectorHeader16(RawDataBlkOffset);
 LARBSDBG("Checking Raw Data Block. n=" << n << "BlockOffset=" << BlockOffset);
 //Check if Raw Data block exists and is not yet part of the fragment
 if (n>m_NFlaggingWords && !BlockOffset)
   {LE_setHeader16(RawDataBlkOffset,m_vFragment->size());
    for(unsigned int i=0;i<n;i++)
      m_vFragment->push_back(m_RawDataBlock[i]);
   }
	  setHeader32(NWTot,m_vFragment->size());
 LARBSDBG("Offsets:" << std::endl
	  << "Raw Data: " << LE_getVectorHeader16(RawDataBlkOffset) << std::endl
	  <<"Low Energy: " << LE_getVectorHeader16(LowEBlkOffset)<< std::endl
	  << "High Energy: " << LE_getVectorHeader16(HighEBlkOffset)<< std::endl
	  << "Energy-index:" << m_EIndex << std::endl
	  << "Filled channels: " << m_ECounter << std::endl);
 clearBlocks();
 return;
}


void  LArRodBlockPhysicsV0::concatinateFEBs() 
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

int LArRodBlockPhysicsV0::getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain)
{
  LARBSDBG("in LArRodBlockPhysicsV0::getNextRawData.");
  LARBSDBG("m_RawDataCounter=" << m_RawDataCounter << "  m_RawDataIndex="<<  m_RawDataIndex); 
  LARBSDBG("m_channelsPerFEB=" << m_channelsPerFEB);
  const int flags=(int) LE_getHeader16(RawDataBlkOffset);
  if (!flags)
    return 0;                                             //Block not existing
  if (m_RawDataCounter>=m_channelsPerFEB)                  //Already beyond maximal number of channels
    return 0; 
  
  LARBSDBG("Flags="<<flags);
  while (!getBit(m_FebBlock+flags,m_RawDataCounter))      //Look for next filled channel
    {++m_RawDataCounter;
     LARBSDBG("RawDataCounter ist now " << m_RawDataCounter);
     if (m_RawDataCounter>=m_channelsPerFEB)              //No more channel availible
       return 0;
    }
  LARBSDBG("Found filled channel at positon " << m_RawDataCounter);
  //Found next filled channel
  channelNumber=(m_RawDataCounter>>4) + ((m_RawDataCounter&0xf)<<3); //Convert ROD to FEB channel ordering
  unsigned int nsamples = LE_getHeader16(NGainNSamples) & 0x00FF;
  LARBSDBG("This run has " << nsamples <<  " samples");
  int index = flags+m_NFlaggingWords+m_RawDataIndex*((int) (nsamples+1)/2);
  LARBSDBG( "index="<<index);
  ++m_RawDataIndex;
  LARBSDBG("In getnextRawData(). index= " << index);
  if((nsamples/2)*2!=nsamples) { //odd number of samples
    const uint32_t& x = m_FebBlock[index];
    gain = x >> 30;
    samples.push_back((short) (x>>2 & 0xfff));
    for(unsigned int i=1;i<=(nsamples-1)/2;i++) {
      const uint32_t& x = m_FebBlock[index+i];
      samples.push_back((short) (x>>2 & 0xfff));
      samples.push_back((short) (x>>18));
    }
  }
  else { //even number of samples
    const uint32_t& x = m_FebBlock[index];
    gain = x >> 30;
    samples.push_back((short) (x>>2 & 0xfff));
 
    samples.push_back((short) ((x>>18) & 0xfff));
   
    for (unsigned int i=1;i<nsamples/2;i++) {
      const uint32_t& x = m_FebBlock[index+i];
      samples.push_back((short) (x>>2 & 0xfff));
      samples.push_back((short) (x >> 18));
    }
  } //end even numer of sample
  ++m_RawDataCounter;
  return 1;
}



/*
void LArRodBlockPhysicsV0::dumpFragment()
{ 
  if (m_pRODblock)
    std::cout << "Dump of LArRodFragment. Block Type=3. Size of vector=" << m_pRODblock->size() <<std::endl;
  else
    std::cout << "Dumping FEB block" << std::endl;
 int size_read=0;
 int size_tot=0;
 if (m_pRODblock)
   size_tot=m_pRODblock->size();  
 do {
   std::cout <<"Begin of do-while loop:" << std::endl;
   if (m_pRODblock)
     m_FebBlock=&m_pRODblock->at(size_read);
   char s[33];
   int i;
   int FlagPosition;
   int CurrentPosition = 0;
   
   std::cout << " Head Block size = " << m_iHeadBlockSize << std::endl;
   for (i=0;i<m_iHeadBlockSize;i++) {
     sprintf(s,"%8.8x",m_FebBlock[CurrentPosition]);
     std::cout << "  " << CurrentPosition << " " << s 
	       << " = " << (m_FebBlock[CurrentPosition]>>16) 
	       << " " << (m_FebBlock[CurrentPosition] &0xffff) << std::endl;
     CurrentPosition++;
   }
   std::cout << "Energy Block offset: " << LE_getHeader16(EBlkOffset) << std::endl;
   std::cout << "tQ Block offset: " << LE_getHeader16(tQBlkOffset) << std::endl;
   std::cout << "Raw Data Block offset " << LE_getHeader16(RawDataBlkOffset) << std::endl;
   
   while (CurrentPosition<getNumberOfWords())
     {FlagPosition=CurrentPosition;
      if (CurrentPosition==LE_getHeader16(EBlkOffset))
	{std::cout << " Found Energy Block at offset = " << LE_getHeader16(EBlkOffset) << std::endl;
	 for (i=0; i<m_NFlaggingWords; i++) {
	   sprintf(s,"%8.8x",m_FebBlock[CurrentPosition]);
	   std::cout << "  " << CurrentPosition << " " << s 
		     << " Flag word" << std::endl;
	   CurrentPosition++;
	 }
	 for (i=0; i<m_NFlaggingWords*32; i++) {
	   if(getBit(&m_FebBlock[FlagPosition],i)) {
	     sprintf(s,"%8.8x",m_FebBlock[CurrentPosition]);
	     std::cout << "  " << CurrentPosition << " " << s 
		       << " ROD Channel "  << i << " FEB Channel " << RodToFebChannel(i)
		       << " = " << m_FebBlock[CurrentPosition] << std::endl;
	     CurrentPosition++;
	   }
	 }
	} // end if energy block
      else if (CurrentPosition==LE_getHeader16(tQBlkOffset))
	{std::cout << " Found tQ Block at position = "  << LE_getHeader16(tQBlkOffset) << std::endl;
	FlagPosition = CurrentPosition; 
	for (i=0; i<m_NFlaggingWords; i++) {
	  sprintf(s,"%8.8x",m_FebBlock[CurrentPosition]);
	  std::cout << "  " << CurrentPosition << " " << s 
		    << " Flag word" << std::endl;
	  CurrentPosition++;
	}
	for (i=0; i<m_NFlaggingWords*32; i++) {
	  if(getBit(&m_FebBlock[FlagPosition],i)) {
	    sprintf(s,"%8.8x",m_FebBlock[CurrentPosition]);
	    std::cout << "  " << CurrentPosition << " " << s 
		      << " ROD Channel " << i << " FEB Channel " << RodToFebChannel(i)
		      << " = " << (m_FebBlock[CurrentPosition]>>16) 
		      << " " << (m_FebBlock[CurrentPosition] &0xffff) << std::endl;
	    CurrentPosition++;
	  }
	}
	}// end if tQ block
      else if (CurrentPosition==LE_getHeader16(RawDataBlkOffset))
	{std::cout << "Found Raw Data Block at position = "  <<LE_getHeader16(RawDataBlkOffset) << std::endl;
	FlagPosition = CurrentPosition;
	for (i=0; i<m_NFlaggingWords; i++) {
	  sprintf(s,"%8.8x",m_FebBlock[CurrentPosition]);
	  std::cout << "  " << CurrentPosition << " " << s 
		    << " Flag word" << std::endl;
	  CurrentPosition++;
	}
	for (i=0; i<m_NFlaggingWords*32; i++) {
	  if(getBit(&m_FebBlock[FlagPosition],i)) {
	    int nsamples = LE_getHeader16(NSamples);
	    for(int j=0;j<nsamples;j+=2) {
	      sprintf(s,"%8.8x",m_FebBlock[CurrentPosition]);
	      std::cout << "  " << CurrentPosition << " " << s 
			<< " ROD Channel " << i << " FEB Channel " << RodToFebChannel(i)
			<< " samples " << j << " and " << j+1 << " = " 
			<< ((m_FebBlock[CurrentPosition]>>16) & 0x3fff) << " " //Mask gain!
			<< (m_FebBlock[CurrentPosition] & 0xffff) << std::endl;
	      CurrentPosition++;
	    }
	  }
	}
	}// end if Raw Data block
      else
	{std::cout << "UNKNOWN block found at position" << CurrentPosition << std::endl;
	 return;
	}
     }// end while
   size_read+=CurrentPosition+6;
   std::cout << " ------- End of FEB-Block (size_read=" <<size_read << ", size_tot=" << size_tot << ") -------" << std::endl;
 } while(size_read<size_tot);
 std::cout << " ------------------ End Of ROD-Framgent -----------------" << std::endl;
}
*/

/*
void LArRodBlockPhysicsV0::dumpFragment(const std::vector<uint32_t>& v)
{
  uint32_t size_tot = v.size();
  uint32_t size_read = 0; 
  std::cout << "LAr ROD vector size =  " << size_tot << std::endl;

  int ifeb=0;
  while(size_read<size_tot) 
    {      
      m_FebBlock = &(v[0]); 
      m_FebBlock+= size_read;
      std::cout << "LAr ROD - FEB " << ifeb << std::endl;
      dumpFragment();
      // next FEB
      size_read += getNumberOfWords()+6;
      ifeb++;
    }
  std::cout << "End of ROD-Fragment" << std::endl;
}
*/


//Sort functions & ordering relation:
template<class RAWDATA>
bool LArRodBlockPhysicsV0::operator () 
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


void LArRodBlockPhysicsV0::sortDataVector(std::vector<const LArRawChannel*>& vRC)
{std::sort(vRC.begin(),vRC.end(),*this);}

void LArRodBlockPhysicsV0::sortDataVector( std::vector<const LArDigit*>& vDigit) 
{std::sort(vDigit.begin(),vDigit.end(),*this);
}
