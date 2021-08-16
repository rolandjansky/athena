//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARRODBLOCKPHYSICSV6_H
#define LARBYTESTREAM_LARRODBLOCKPHYSICSV6_H

/** This class provides decoding/encoding from/to ROD format. 
   * @authors Denis Oliveira Damazio
   * @version  0-0-1 , March 28th, 2011
   *
   * Modified, 
   * new format for ATLAS (to include SumE)
   */

#include "LArByteStream/LArRodBlockStructure.h"
#include "CaloIdentifier/CaloGain.h"
#include "GaudiKernel/ISvcLocator.h"
#include "LArIdentifier/LArOnlineID.h"

//#define LARBSDBGOUTPUT
#ifdef LARBSDBGOUTPUT
#define MYLEVEL (MSG::FATAL)
#define LARBSDBG(text) logstr<<MYLEVEL<<text<<endmsg
#else
#define LARBSDBG(text)
#endif

class LArRodBlockPhysicsV6 : public LArRodBlockStructure
{
public:
  // ----------------- Header words indexes -----------------
  enum {
    NWTot,            // First words: DSP event header
    NWTot_h,
    FEBID,
    FEBID_h,
    FEB_SN,           // FEB serial number
    FEB_SN_h,         // FEB serial number
    ResultsOff1,      // Size of results (Physics averages in DSP)
    ResultsDim1,      // Offset to results
    ResultsOff2,      // Size of times (in physics)
    ResultsDim2,      // Offset to times (in physics)
    RawDataBlkOff,
    RawDataBlkDim, // Raw FEB event offset
    EventStatus,      // Bits describing the event
    EventStatus_h,      
    NGains,
    NSamples,   
    FirstSampleIndex,  
    FebConfig,
    InFPGAFormat, // added 08.09.2005 - wrong 28.09.2005?
    InFPGAFormat_h,
    endtag          //This tag needs to be an odd number, see *) for constructor
  };
  // constructor
  LArRodBlockPhysicsV6();
 
  // ------ Identify RodBlockStructure -------
  std::string BlockType() { return std::string("RodBlockPhysicsV6");}
 public:
  // ----------------- Decoding methods -----------------
  // Never to be used while encoding!
  // set full ROD fragment before trying to get anything!
  // in case there is more than 1 FEB in 1 fragment, jump to next FEB
  virtual inline int getNextEnergy(int& channelNumber, int32_t& energy, int32_t& time,int32_t& quality,uint32_t& gain);
  virtual int      getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain);
  int              getNextDigits(int& channelNumber, std::vector<short>& samples, uint32_t& gain);
  virtual uint16_t getResults1Size() const;
  virtual uint16_t getResults2Size() const;
  virtual uint16_t getRawDataSize() const;
  virtual uint16_t getNbSweetCells1() const;
  virtual uint16_t getNbSweetCells2() const;
  virtual uint32_t getNumberOfSamples() const;
  virtual uint32_t getNumberOfGains() const;
  virtual uint32_t getRadd(uint32_t adc, uint32_t sample) const;
  virtual uint16_t getCtrl1(uint32_t adc) const;
  virtual uint16_t getCtrl2(uint32_t adc) const;
  virtual uint16_t getCtrl3(uint32_t adc) const;
  virtual uint32_t getStatus() const;

  //virtual uint32_t onlineCheckSum() const;
  //virtual uint32_t offlineCheckSum() const;

  virtual inline  uint32_t  hasCalibBlock()   const {return 0;} ; 
  virtual inline  uint32_t  hasPhysicsBlock() const {return getHeader16(ResultsOff1);} ;
  virtual inline  uint32_t  hasRawDataBlock() const {return getHeader16(RawDataBlkOff)+getHeader16(ResultsOff2);} ;
  virtual inline  uint32_t  hasControlWords() const {return getHeader16(RawDataBlkOff);} ;
  virtual inline int FebToRodChannel(int ch) const;

  // Decode counter block
  virtual inline int32_t getEx() const;
  virtual inline int32_t getEy() const;
  virtual inline int32_t getEz() const;
  virtual inline int32_t getSumE() const;
  virtual inline uint32_t getVROBFebId();
  virtual inline int32_t getVROBEx() const;
  virtual inline int32_t getVROBEy() const;
  virtual inline int32_t getVROBEz() const;
  virtual inline int32_t getVROBSumE() const;

  virtual inline int setGain(const int /*GainValue*/) { return 1; };

  // Enconding methods
  void initializeFragment(std::vector<uint32_t>& fragment);
  void initializeFEB(const uint32_t id);
  void setNextEnergy(const int channel, const int32_t energy, const int32_t time, const int32_t quality, const uint32_t gain);
  void setRawData(const int , const std::vector<short>& , const uint32_t);
  void finalizeFEB();
  // build full ROD fragment
  void concatinateFEBs();
  virtual inline void setEx(double);
  virtual inline void setEy(double);
  virtual inline void setEz(double);
  virtual inline void setSumE(double);
  
  void setRequiredNSamples(unsigned short ns) { m_requiredNSamples = ns; }

  //Ordering relation for channels & digits
  template<class RAWDATA>
  bool operator () (const RAWDATA* ch1, const RAWDATA* ch2) const;
  using LArRodBlockStructure::sortDataVector;  // avoid warnings.
  void sortDataVector( std::vector<const LArDigit*>& vDigit) 
	{std::sort(vDigit.begin(),vDigit.end(),*this); }

  
private:
  void setE(unsigned int index, double E);
  virtual void resetPointers();
  virtual bool setPointers();
  void setNextEnergy(const uint16_t energy,const int16_t time, const int16_t quality, const uint32_t gain);
  uint16_t getNbSweetCells1FromMask() const;
  uint16_t getNbSweetCells2FromMask() const;

  std::vector<uint32_t>  m_SumBlkBlockE1;
  std::vector<uint32_t>  m_SumBlkBlockE2;
  std::vector<uint32_t>  m_GainBlock;
  std::vector<uint32_t>  m_FebInfoBlock;
  std::vector<uint16_t>  m_TimeQualityBlock;
  std::vector<uint32_t>  m_RawDataBlock;
  std::vector<uint16_t>  m_EnergyBlockEncode;
  std::vector<uint16_t>  m_DigitsEncode;

  int m_EnergyIndex = 0;
  int m_TimeQualityIndex = 0;
  int m_DigitsIndex = 0;
  int m_DigitsChannel = 0;
  int m_RawDataIndex = 0;
  const uint32_t* m_GainPointer = nullptr;
  const uint32_t* m_MaskTimeQualityPointer = nullptr;
  const uint32_t* m_MaskDigitsPointer = nullptr;
  const uint16_t* m_RaddPointer = nullptr;
  const uint16_t* m_EnergyPointer = nullptr;
  const int32_t*  m_SumPointer = nullptr;
  const uint16_t* m_TimeQualityPointer = nullptr;
  const uint16_t* m_DigitsPointer = nullptr;
  const uint16_t* m_RawDataPointer = nullptr;

  //For fixed gain mode
  int m_fixedGain;
  // If one needs to use a log output not only for MSG::DEBUG
  //MsgStream *m_logstr;
  uint16_t m_numberHotCell = 0U;
  uint16_t m_numberHotCellOffTime; 
  // Needs one threshold to send time/quality another for samples
  uint16_t m_EnergyThreshold1;
  uint16_t m_EnergyThreshold2;
  int16_t  m_OffTimeCut;

  unsigned short m_requiredNSamples;
  const LArOnlineID* m_onlineHelper;
  
public:
  inline uint16_t getFebConfig() const;
  inline uint16_t getFirstSampleIndex() const;

  virtual bool canSetCalibration() {return false;}
  bool canSetEnergy() { return true;}
  bool canSetRawData() { return false;}
  bool canIncludeRawData() { return true;}
};

inline uint16_t LArRodBlockPhysicsV6::getFebConfig() const
{
  return getHeader16(FebConfig);
}

inline uint16_t LArRodBlockPhysicsV6::getFirstSampleIndex() const
{
  return getHeader16(FirstSampleIndex);
}

inline int LArRodBlockPhysicsV6::getNextEnergy(int& channelNumber,int32_t& energy,int32_t& time,int32_t& quality, uint32_t& gain)
{
  #ifdef LARBSDBGOUTPUT
  MsgStream logstr(Athena::getMessageSvc(), BlockType());
  #endif

  LARBSDBG("in LArRodBlockPhysicsV6::getNextEnergy.");
  LARBSDBG("m_channelsPerFEB=" << m_channelsPerFEB);
  if (m_EnergyIndex>=m_channelsPerFEB)                       // Already beyond maximal number of channels
    return 0;
  if (!m_EnergyPointer)                                               // No data block present
    return 0;
 
  unsigned rodChannelNumber=m_EnergyIndex;      // Index of Channel in ROD-Block
  channelNumber=((rodChannelNumber&0xe)<<2) + ((rodChannelNumber&0x1)<<6) + (rodChannelNumber>>4);    //channel number of the FEB
  //channelNumber=(rodChannelNumber>>4) + ((rodChannelNumber&0xf)<<3);    //channel number of the FEB

//  if(channelNumber==0) {
//    int size = getNumberOfWords();
//    int off1 = getHeader16(ResultsOff1)-8;
//    int dim1 = getHeader16(ResultsDim1);
//    int off2 = getHeader16(ResultsOff2)-8;
//    int dim2 = getHeader16(ResultsDim2);
//    int off3 = getHeader16(RawDataBlkOff)-8;
//    int dim3 = getHeader16(RawDataBlkDim);
//    
//    for(int i=0;i<size;i++) {
//      if(i==0)
//	std::cout << std::hex << i << " : NWTot       " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if(i==1)
//	std::cout << std::hex << i << " : FEBid       " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if(i==2)
//	std::cout << std::hex << i << " : FEBsn       " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if(i==3)
//	std::cout << std::hex << i << " : Block1      " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if(i==4)
//	std::cout << std::hex << i << " : Block2      " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if(i==5)
//	std::cout << std::hex << i << " : Block3      " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if(i==6)
//	std::cout << std::hex << i << " : Status      " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if(i==7)
//	std::cout << std::hex << i << " : Gain/Sample " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if(i==8)
//	std::cout << std::hex << i << " : 1st/FebConf " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if(i==9)
//	std::cout << std::hex << i << " : InFPGA      " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if (i>=off1 && i<off1+dim1)
//	std::cout << std::hex << i << " : results1    " << i-off1 << " " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if (i>=off2 && i<off2+dim2)
//	std::cout << std::hex << i << " : results2    " << i-off2 << " " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else if (i>=off3 && i<off3+dim3)
//	std::cout << std::hex << i << " : raw data    " << i-off3 << " " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//      else 
//	std::cout << std::hex << i << " " << std::hex << m_FebBlock+i << " : " << std::hex << m_FebBlock[i] << std::endl;
//    }
//  }

  // get information available for all cells
  // Energy on a 16 bit word and decode ranges
  uint16_t encodedEnergy;  // 16 bits Encoded Energy word
  int32_t  aux;  
  uint16_t range;          // 2 bits range
  uint16_t  sign;

  LARBSDBG("-------->>>> in LArRodBlockPhysicsV6::getNextEnergy : decode energy.....");
  // decode energy
  if(m_EnergyIndex & 0x1) encodedEnergy = m_EnergyPointer[m_EnergyIndex-1]; // Big/Little Endien stuff
  else                    encodedEnergy = m_EnergyPointer[m_EnergyIndex+1]; // Big/Little Endien stuff

  aux = (int32_t) (encodedEnergy&0x1fff); 
  range    = (encodedEnergy & 0xc000) >> 14;
  if(aux==0 && range>0) aux=0x2000;
  sign     =  encodedEnergy & 0x2000;
  aux    <<= 3*range;
  if(sign) aux = -aux;
  energy = aux; 
  
  // gain in 2 bits of a 32 bits word 
  if(m_GainPointer) {
    gain = (uint32_t) ((m_GainPointer[m_EnergyIndex>>4] >> ((m_EnergyIndex&0xf)<<1)) & 0x3);
    gain=RawToOfflineGain(gain);
  } else gain=0xffffffff;
  
  // Get Time and Quality if the information is present according to summary block
  uint32_t hasTQ;
  if(m_MaskTimeQualityPointer)
    hasTQ = (uint32_t) ((m_MaskTimeQualityPointer[m_EnergyIndex>>5] >> (m_EnergyIndex&0x1f)) &0x1);
  else 
    hasTQ = 0;
  m_EnergyIndex++;
  if (m_TimeQualityPointer && hasTQ) // Data has Time and Quality information 
    {
      //Time is in 10 ps in ByteStream, hence the factor 10 to convert to ps
      time    = 10*(reinterpret_cast<const int16_t *>(m_TimeQualityPointer))[m_TimeQualityIndex++]; 
      quality = m_TimeQualityPointer[m_TimeQualityIndex++]; 

#ifdef LARBSDBGOUTPUT
      logstr << MYLEVEL <<"This cell has time and Quality information "<<endmsg;
#endif
    }
  else  // Data has no Time and Quality information
    {
      time=0;
      quality=-1;
    }


#ifdef LARBSDBGOUTPUT
  logstr << MYLEVEL <<"Range = "<<range<<endmsg;
  logstr << MYLEVEL <<"Sign = "<<sign<<endmsg;
  logstr << MYLEVEL <<" Encoded Energy ="<< MSG::hex << encodedEnergy << MSG::dec << " E=" << energy
	 << " t=" << time 
	 << " Q=" << quality 
	 << " G=" << gain 
	 << " channel Number=" << channelNumber
	 << endmsg;
#endif
 
  return 1;
}

inline int32_t  LArRodBlockPhysicsV6::getEx() const
{
  if(m_SumPointer) return (m_SumPointer[0]>>9);
  return 0;
}

inline int32_t  LArRodBlockPhysicsV6::getEy() const
{
  if(m_SumPointer) return (m_SumPointer[1]>>9);
  return 0;
}

inline int32_t  LArRodBlockPhysicsV6::getEz() const
{
  if(m_SumPointer) return (m_SumPointer[2]>>9);
  return 0;
}
inline int32_t LArRodBlockPhysicsV6::getSumE() const
{
  if(m_SumPointer) return (m_SumPointer[3]>>9);
  return 0;
}

inline uint32_t  LArRodBlockPhysicsV6::getVROBFebId()
{
  m_ROB_to_decode--;
  if ( m_ROB_to_decode>=0){
  	m_virtualROBPointerLocal = m_virtualROBPointer;
  	m_virtualROBPointer+=m_virtualROBJump;
  } else m_virtualROBPointerLocal=0;
  if(m_virtualROBPointerLocal) return (m_virtualROBPointerLocal[0]);
  else return 0;
}

inline int32_t  LArRodBlockPhysicsV6::getVROBEx() const
{
  const int32_t* p = reinterpret_cast<const int32_t*>(m_virtualROBPointerLocal);
  if(p) return (p[1]>>9);
  return 0;
}

inline int32_t  LArRodBlockPhysicsV6::getVROBEy() const
{
  const int32_t* p = reinterpret_cast<const int32_t*>(m_virtualROBPointerLocal);
  if(p) return (p[2]>>9);
  return 0;
}

inline int32_t  LArRodBlockPhysicsV6::getVROBEz() const
{
  const int32_t* p = reinterpret_cast<const int32_t*>(m_virtualROBPointerLocal);
  if(p) return (p[3]>>9);
  return 0;
}

inline int32_t  LArRodBlockPhysicsV6::getVROBSumE() const
{
  const int32_t* p = reinterpret_cast<const int32_t*>(m_virtualROBPointerLocal);
  if(p) return (p[4]>>9);
  return 0;
}

inline int LArRodBlockPhysicsV6::FebToRodChannel(int ch) const
{
return ( (ch&0x7) << 4) | ( (ch&0x38) >>2 ) | ((ch&0x40)>>6);

}

inline void LArRodBlockPhysicsV6::setE(unsigned index, double E){ 

        union {
          int32_t i;
          uint16_t us[2];
        } conv;
        conv.i = static_cast<int32_t>(E);
        // Write as multiplication, not as left shift, since left-shifting
        // a negative number is undefined in C++.
        // Compiles to the same code on x86_64.
        conv.i *= (1<<9);
        if ( m_TimeQualityBlock.size()>=8 ){
                m_TimeQualityBlock[index]=conv.us[0];
                m_TimeQualityBlock[index+1]=conv.us[1];
        }
}


inline void LArRodBlockPhysicsV6::setEx(double Ex){ 
        setE (0, Ex);
}


inline void LArRodBlockPhysicsV6::setEy(double Ey){
        setE (2, Ey);
}


inline void LArRodBlockPhysicsV6::setEz(double Ez){
        setE (4, Ez);
}

inline void LArRodBlockPhysicsV6::setSumE(double SumE){
        setE (6, SumE);
}




#ifdef LARBSDBGOUTPUT
#undef LARBSDBGOUTPUT
#endif
#undef LARBSDBG

#endif
