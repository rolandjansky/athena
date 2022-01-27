//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARBYTESTREAM_LARRODBLOCKSTRUCTURE_H
#define LARBYTESTREAM_LARRODBLOCKSTRUCTURE_H
/** This class provides decoding/encoding from/to ROD format. 
   * @authors W. Lampl, R. Lafaye
   * @version  0-0-1 , Jul 17, 2003
   *
   * Modified, 8.9.2003, W.Lampl
   * make const-correct
   * and get-functions inline to 
   * increase performance
   *
   * Modified, 18.5.2003 W.Lampl
   * use bitshift & masking for get/setHeader function
   * to get around the endianness problem with the 
   * DSP.
   *
   * Modified, 04.01.2006 I. Wingerter
   * copy modif for Accumuluted Calib Digits
   *
   */
                  
// #include "eformat/RODHeader.h"
#include "ByteStreamData/RawEvent.h"
#include <stdint.h>
#include <iostream>
#include <vector>
#include <map>
#include <utility>

//#define LARBYTESTREAMRODBLOCK_CHCKBOUNDARIES

//class LArCablingService; 
class LArOnlineID; 
class LArRawChannel; 
class LArDigit;
class LArAccumulatedDigit;
class LArCalibDigit;
class LArAccumulatedCalibDigit;

class LArRodBlockStructure
{
protected:
  // ----------------- Header words indexes -----------------
  enum {
    NWTot,          // Number of words in DSP block
    NWTot_h,
    FEBID,          // FEB ID
    FEBID_h,
    FEBSN,          // FEB Serial Number
    FEBSN_h,
    endtag          // This tag needs to be an odd number, see *) for constructor
  };

public : 
  // constructor
  LArRodBlockStructure();
  virtual ~LArRodBlockStructure();

  // ----------------- Encoding methods -----------------
  // Never to be used while decoding!
  virtual void initializeFragment(std::vector<uint32_t>& fragment);
  virtual void initializeFEB(const uint32_t id);
  virtual void setNumberOfSamples(const uint8_t n); 
  virtual void setNumberOfGains(const uint8_t n);
  virtual void setTDCPhase(const uint8_t n); //useful only TB SW validation 
  virtual void setNextEnergy(const int channel, const int32_t energy, const int32_t time, const int32_t quality, const uint32_t gain);
  virtual void setRawData(const int channel, const std::vector<short>& samples, const uint32_t gain);
  virtual void setRawDataFixed(const int channel, const std::vector<short>& samples, const uint32_t gain);
  virtual void setEtQ(const int channel, const int32_t energy, const int32_t time, const int32_t quality, const uint32_t gain);
  virtual void setDAC(const uint16_t DACValue);
  virtual void setDelay(const uint16_t DelayValue);
  virtual void setPulsed (const unsigned channelNumber);
  virtual void setNTrigger (const uint16_t NTrigger);
  virtual void finalizeFEB();
  virtual void setEx(double);
  virtual void setEy(double);
  virtual void setEz(double);
  virtual void setSumE(double);
  //Function to sort RawData Container before encoding. Unfortunatly, virtal template member functions are not allowed.
  virtual void sortDataVector(std::vector<const LArRawChannel*>& );
  virtual void sortDataVector( std::vector<const LArDigit*>& );
  virtual void sortDataVector( std::vector<const LArCalibDigit*>& );
  virtual void sortDataVector( std::vector<const LArAccumulatedCalibDigit*>& );
  virtual void sortDataVector( std::vector<const LArAccumulatedDigit*>& );
  // build full ROD fragment
  //  virtual void concatinateFEBs(RODHeader* fullHeader=NULL);
  virtual void concatinateFEBs( );
  //Check features of this RodBlock
  virtual bool canSetEnergy() { return false;}
  virtual bool canSetRawData() {return false;}
  virtual bool canSetRawDataFixed() {return false;}
  virtual bool canSetCalibration() {return false;}
  virtual bool canSetNTrigger() {return false;}
  virtual bool canIncludeRawData() {return false;}

  // ----------------- Decoding methods -----------------
  // Never to be used while encoding!
  // set full ROD fragment before trying to get anything!
  inline bool     setFragment(const uint32_t* p, uint32_t n);
  //   virtual bool nextFEB(RODHeader* fullHeader=NULL);
  inline bool     nextFEB ( );
  inline uint32_t getNumberOfWords() const;
  inline uint32_t getFEBID() const;
  inline uint32_t getFEBSN() const;

  virtual inline int32_t  getEx() const;
  virtual inline int32_t  getEy() const;
  virtual inline int32_t  getEz() const;
  virtual inline int32_t  getSumE() const;
  virtual inline uint32_t getVROBFebId();
  virtual inline int32_t  getVROBEx() const;
  virtual inline int32_t  getVROBEy() const;
  virtual inline int32_t  getVROBEz() const;
  virtual inline int32_t  getVROBSumE() const;
  virtual inline uint32_t hasCalibBlock() const {return 0;} ; 
  virtual inline uint32_t hasAccumBlock() const {return 0;} ;
  virtual inline uint32_t hasPhysicsBlock() const {return 0;} ;
  virtual inline uint32_t hasRawDataBlock() const {return 0;} ;
  virtual inline uint32_t hasControlWords() const {return 0;} ;

  virtual inline int getNextEnergy(int& channelNumber, int32_t& energy, int32_t& time,int32_t& quality,uint32_t& gain);
  virtual int  getNextRawData(int& channelNumber, std::vector<short>& samples, uint32_t& gain);
  virtual int  getNextAccumulatedCalibDigit(int& channelNumber, std::vector < uint64_t >& SamplesSum,  std::vector < uint64_t >& Samples2Sum, uint32_t&  nStepTriggers, uint32_t& gain);
  virtual int  getNextAccumulatedDigit(int& channelNumber, std::vector<uint64_t>& SamplesSum, std::vector < uint64_t >& corr2Sum, uint32_t& gain);

  // Get functions related to hasCalibBlock
  virtual inline bool     getPulsed(unsigned channelNumber) const;
  virtual inline uint16_t getDAC() const;
  virtual inline uint16_t getDelay() const;
  virtual inline uint16_t getNTrigger() const;
  virtual inline uint16_t getStepIndex() const;
  virtual inline uint16_t getNStep() const;

  virtual uint8_t getTDCPhase() const; //useful only TB SW validation 
  virtual int setGain(const int GainValue); //Define fixed gain in case of fixed gain mode.

  virtual uint32_t getNumberOfSamples() const;
  virtual uint32_t getNumberOfGains() const;

  // Information access for Dsp and Feb headers decoding return 0 if information is absent
  virtual uint16_t getResults1Size() const;
  virtual uint16_t getResults2Size() const;
  virtual uint16_t getRawDataSize() const;
  virtual uint16_t getNbSweetCells1() const;
  virtual uint16_t getNbSweetCells2() const;
  virtual uint32_t getRadd(uint32_t adc, uint32_t sample) const;
  virtual uint16_t getCtrl1(uint32_t adc) const;
  virtual uint16_t getCtrl2(uint32_t adc) const;
  virtual uint16_t getCtrl3(uint32_t adc) const;
  virtual uint32_t getStatus() const;
  virtual uint32_t getDspCodeVersion() const;
  virtual  int32_t getDspEventCounter() const;

  virtual uint32_t onlineCheckSum() const;
  virtual uint32_t offlineCheckSum() const;
  // To decode the virtual ROB fragment
  virtual int setFragmentVirtualROB(const uint32_t* p, uint32_t n);

  // print the full ROD fragment
  virtual void dumpFragment();
  inline void setFirstSample(const int rearrangeFirstSample)
      {m_rearrangeFirstSample=rearrangeFirstSample;}

  //inline uint32_t FebToOfflineGain(const uint32_t gain) const;
  //inline uint32_t OfflineToFebGain(const uint32_t gain) const;
  inline uint32_t RawToOfflineGain(const uint32_t gain) const;
  inline uint32_t OfflineToRawGain(const uint32_t gain) const;
  virtual inline int FebToRodChannel(int ch) const
	{ return (ch>>3) + ((ch&0x7)<<4); };
  inline bool report_error ( void ) const { return m_error_next_feb;}

 protected:
  // ----------------- Encoding methods -----------------
  // The following functions write to m_vFragment (vector)
  inline void setHeader16(const unsigned n, const uint16_t w); // n should be choosen from the above enum  
  inline void setHeader32(const unsigned n, const uint32_t w); // n should be choosen from the above enum

  // ----------------- Decoding methods -----------------
  // The method to call to reset pointers and counters
  virtual void resetPointers() { return; }
  // The method to call to set pointers and counters
  virtual bool setPointers() { return true;}
  // The following functions read from m_vFragment (vector) (to be used during encoding)
  inline uint16_t getVectorHeader16(const unsigned n) const; // n should be choosen from the above enum
  inline uint32_t getVectorHeader32(const unsigned n) const; // n should be choosen from the above enum
  // The following functions read from m_FebBlock (uint32_t*) (to be used during decoding)
  inline uint16_t getHeader16(const unsigned n) const; // n should be choosen from the above enum
  inline uint32_t getHeader32(const unsigned n) const; // n should be choosen from the above enum
  // The following funcitons are Little Endian versions for backward compatibility
  inline uint16_t LE_getHeader16(const unsigned n) const; // n should be choosen from the above enum
  inline void LE_setHeader16(const unsigned n, const uint16_t w); // n should be choosen from the above enum  
  inline uint16_t LE_getVectorHeader16(const unsigned n) const; // n should be choosen from the above enum

  // set/read the bitmap for a ROD channel 
  inline void setBit(uint32_t *const p, const unsigned chan); 
  inline int  getBit(const uint32_t *const p, const unsigned chan) const;

  // ----------------- Data members -----------------

  int32_t m_Ex;
  int32_t m_Ey;
  int32_t m_Ez;
  int32_t m_SumE;
#ifndef NDEBUG
  // In Debug mode, many messages are printed for the setEx/y/z method
  // Let's avoid them
  uint32_t m_numberOfEx;
  uint32_t m_numberOfEy;
  uint32_t m_numberOfEz;
  uint32_t m_numberOfSumE;
#endif

  //Offsets for the fixed size Blocks..
  unsigned short m_iHeadBlockSize; 
  //Only for encoding: Block for Header
  //std::vector<uint32_t>  m_HeaderBlock;
  //Number of channels per FEB
  int m_channelsPerFEB;
  //Full ROD fragment for decoding
  const uint32_t* m_FebBlock;   //Pointer to the first element of the current FEB-Fragement (used only for decoding!)
  const uint32_t* m_RodBlock;  //Pointer to the very beginning of the ROD-Block
  int32_t m_FebBlockSize; 
  int32_t m_RodBlockSize; 
  std::vector<uint32_t> *m_vFragment;  //Pointer to the vector that holds the current FEB-block (used only for encoding!)
  std::vector<uint32_t> *m_pRODblock; //Pointer to the ROD data vector
  typedef std::map<uint32_t, std::vector<uint32_t> > FEBMAPTYPE;
  FEBMAPTYPE m_mFebBlocks; //Map of feb-Blocks (for encoding only);
  //static uint32_t FebToOfflineGainMap[3];
  //static uint32_t OfflineToFebGainMap[3];
  static const uint32_t m_RawToOfflineGainMap[4];
  static const uint32_t m_OfflineToRawGainMap[3];
  int32_t m_MiddleHeaderSize;
  // Free gain mode needs raw data to be rearranged
  unsigned int m_rearrangeFirstSample;
  // error in the next FEB
  bool m_error_next_feb;
  // Jump from one virtual ROB
  uint32_t m_virtualROBJump;
  int32_t m_ROB_to_decode;
  const uint32_t* m_virtualROBPointer;
  const uint32_t* m_virtualROBPointerLocal;
};

// Implementation of inline-functions:
inline bool LArRodBlockStructure::setFragment(const uint32_t* p, uint32_t n)
{
  resetPointers();
  m_FebBlock         = p;
  m_RodBlock         = m_FebBlock;
  m_RodBlockSize     = n;
  uint32_t BlockSize = 0;

  if (n==0) {
    std::cout << "ERROR while decoding LArByteStream: Got Rod block size 0" << std::endl;
    return false;
  }

  BlockSize = getNumberOfWords();
  if (BlockSize>n) {
    std::cout << "ERROR while decoding LArByteStream: Found FEB block of size " << BlockSize << " in a ROD block of size " << n << std::endl;
    return false;
  }
  m_FebBlockSize = BlockSize;
  
  m_error_next_feb = false;
  //std::cout << "1: FEB found with size " << m_FebBlockSize << std::endl;

  return setPointers();
}
 
/* Decode the virtual ROB fragment. If data is inconsistent with
	data size, return 0 */
inline int LArRodBlockStructure::setFragmentVirtualROB(const uint32_t* p,
		uint32_t n){
	m_ROB_to_decode=0;
	m_virtualROBPointer=0;
	m_virtualROBPointerLocal=0;
	m_virtualROBJump=0;
	if ( n<2 ) { std::cout << "ERROR" << std::endl; return 0;}
	m_virtualROBJump = ((*p)>>16)>>1; // Divide by two (two FEBs-1ROB)
	m_ROB_to_decode = ( (*p) & 0xFFFF )<<1; // Multiply by two
	if ( (n - m_virtualROBJump*m_ROB_to_decode-1) ){
		std::cout << "ERROR AGAIN" << std::endl;
		return 0;
	}
	m_virtualROBPointer = (uint32_t*)(p+1);
	return m_ROB_to_decode;
}

inline uint32_t  LArRodBlockStructure::getFEBID() const
{return getHeader32(FEBID);} 

inline uint32_t  LArRodBlockStructure::getFEBSN() const
{return getHeader32(FEBSN);} 

inline int32_t LArRodBlockStructure::getEx() const {
 return 0;
}

inline int32_t LArRodBlockStructure::getEy() const {
 return 0;
}

inline int32_t LArRodBlockStructure::getEz() const {
 return 0;
}

inline int32_t LArRodBlockStructure::getSumE() const {
 return 0;
}

inline uint32_t  LArRodBlockStructure::getVROBFebId(){
return 0;
}

inline int32_t  LArRodBlockStructure::getVROBEx() const{
return 0;
}

inline int32_t  LArRodBlockStructure::getVROBEy() const{
return 0;
}

inline int32_t  LArRodBlockStructure::getVROBEz() const{
return 0;
}

inline int32_t  LArRodBlockStructure::getVROBSumE() const{
return 0;
}

/*
inline int LArRodBlockStructure::FebToRodChannel(int ch) const
  //{return ch/8 + 16 * (ch%8);}
{return (ch>>3) + ((ch&0x7)<<4);}

inline int LArRodBlockStructure::RodToFebChannel(int ch) const
  //{return ch/16 + 8 * (ch%16);}
{return (ch>>4) + ((ch&0xf)<<3);}
*/

inline uint32_t LArRodBlockStructure::RawToOfflineGain(const uint32_t gain) const
{return m_RawToOfflineGainMap[gain];} //For efficency, don't check range

inline uint32_t LArRodBlockStructure::OfflineToRawGain(const uint32_t gain) const
{return m_OfflineToRawGainMap[gain];} //For efficency, don't check range

inline uint16_t LArRodBlockStructure::getHeader16(const unsigned n) const // n should be choosen from the above enum
{
  //  std::cout << "getHeader16: " << m_FebBlock << " " << m_FebBlock[5] << " "
  //    << n << " " << (n>>1) << " " << (m_FebBlock[5]&0xffff) << std::endl;
 if (n&0x1) //n is a odd number 
   return m_FebBlock[n>>1] & 0xffff;  //1,3,5... are fetched from lower bits
 else //n is a even number
   return m_FebBlock[n>>1] >> 16;     //0,2,4... are fetched from higher bits
}

inline uint32_t LArRodBlockStructure::getHeader32(const unsigned n) const // n should be choosen from the above enum
{return m_FebBlock[n>>1];}

inline uint16_t LArRodBlockStructure::getVectorHeader16(const unsigned n) const // n should be choosen from the above enum
{ if (n&0x1) //n is a odd number 
    return (std::as_const(*m_vFragment).at(n>>1) & 0xffff);
  else //n is a even number
    return (std::as_const(*m_vFragment).at(n>>1) >> 16);
}

inline uint32_t LArRodBlockStructure::getVectorHeader32(const unsigned n) const // n should be choosen from the above enum
{
 return (*m_vFragment)[n>>1];
}

inline void LArRodBlockStructure::setHeader16(const unsigned n, const uint16_t w)
{
#ifdef LARBYTESTREAMRODBLOCK_CHCKBOUNDARIES
  if ((unsigned)n>=m_vFragment->size()*2) {
    std::cout << "ERROR WRITE BEYOND ARRAY BONDARY!" << std::endl;
    std::abort();
  }
#endif
  if (n&0x1) // n is a odd number
    m_vFragment->at(n>>1)=((m_vFragment->at(n>>1) & 0xffff0000) | w);   
  else
    m_vFragment->at(n>>1)=((m_vFragment->at(n>>1) & 0xffff) | (w << 16)); 
}

inline void LArRodBlockStructure::setHeader32(const unsigned n, const uint32_t w)
{ 
#ifdef LARBYTESTREAMRODBLOCK_CHCKBOUNDARIES
  if ((unsigned)n>=m_vFragment->size()*2) {
    std::cout << "ERROR WRITE BEYOND ARRAY BONDARY!" << std::endl;
    std::abort();
  }
#endif
  m_vFragment->at(n>>1) = w;
}

inline uint16_t LArRodBlockStructure::LE_getHeader16(const unsigned n) const // n should be choosen from the above enum
{
  return (reinterpret_cast<const uint16_t*>(m_FebBlock))[n];
}

inline uint16_t LArRodBlockStructure::LE_getVectorHeader16(const unsigned n) const // n should be choosen from the above enum
{
  const uint32_t* data32 = std::as_const(*m_vFragment).data();
  const uint16_t* data16 = reinterpret_cast<const uint16_t*> (data32);
  return data16[n];
}

inline void LArRodBlockStructure::LE_setHeader16(const unsigned n, const uint16_t w)
{
#ifdef LARBYTESTREAMRODBLOCK_CHCKBOUNDARIES
  if ((unsigned)n>=m_vFragment->size()*2) {
    std::cout << "LArRodBlockStructure::LE_setHeader16 ERROR: WRITE BEYOND ARRAY BONDARY!" << std::endl;
    std::abort();
  }
#endif
 ((uint16_t*)(&(m_vFragment->front())))[n] = w;
}

inline uint32_t LArRodBlockStructure::getNumberOfWords() const
{ 
  return getHeader32(NWTot); 
}

inline int LArRodBlockStructure::getBit(const uint32_t *const p, const unsigned chan) const
{// chan = (0,127)   
  // int a = chan/32; 
  // int r = chan%32; 
  int a = chan>>5; 
  int r = chan&0x1f; 
  // a = (0,3), r = ( 0, 31 )
  return (*(p+a)>>r) & 0x1; 
}


inline void LArRodBlockStructure::setBit(uint32_t *const p, const unsigned chan)
{
  // chan = (0,127)   
  // int a = chan/32; 
  // int r = chan%32; 
  int a = chan>>5; 
  int r = chan&0x1f; 
  // a = (0,3), r = ( 0, 31 )
  *(p+a) |= (1<<r) ; 
  return; 
}

inline int  LArRodBlockStructure::getNextEnergy(int&, int32_t&, int32_t&, int32_t&,uint32_t&)
{
 std::cout << "ERROR: Function getNextEnergy not implemented in this instance of LArRodBlockStructure!\n";
 return 0;
}
                                  
inline bool LArRodBlockStructure::getPulsed(unsigned) const 
{
 std::cout << "ERROR: Function getPulsed not implemented in this instance of LArRodBlockStructure!\n";
 return 0;
}

inline uint16_t  LArRodBlockStructure::getDAC() const
{
 std::cout << "ERROR: Function getDAC not implemented in this instance of LArRodBlockStructure!\n";
 return 0;
}

inline uint16_t  LArRodBlockStructure::getDelay() const
{
 std::cout << "ERROR: Function getDelay not implemented in this instance of LArRodBlockStructure!\n";
 return 0;
}
 
inline uint16_t  LArRodBlockStructure::getNTrigger() const
{
 std::cout << "ERROR: Function getNTrigger not implemented in this instance of LArRodBlockStructure!\n";
 return 0;
}

inline uint16_t  LArRodBlockStructure::getStepIndex() const
{
 std::cout << "ERROR: Function getStepIndex not implemented in this instance of LArRodBlockStructure!\n";
 return 0;
}
inline uint16_t  LArRodBlockStructure::getNStep() const
{
 std::cout << "ERROR: Function getNStep not implemented in this instance of LArRodBlockStructure!\n";
 return 0;
}

inline bool LArRodBlockStructure::nextFEB()
{
  resetPointers();

  const int32_t FebOffset = m_FebBlockSize+m_MiddleHeaderSize;
  const int32_t LeftSize  = m_RodBlockSize-FebOffset+m_RodBlock-m_FebBlock;
  if ( LeftSize<=0 ) return false; //No next feb

  m_FebBlock += FebOffset; //Jump to the next FEB fragment
  const int32_t BlockSize = getNumberOfWords();
  //std::cout << "LeftSize=" << LeftSize << " BlockSize=" << BlockSize << std::endl;
  if (BlockSize>LeftSize) {
      std::cout << "ERROR while decoding LArByteStream: Found FEB block of size " << BlockSize << " in a ROD block of size " << LeftSize << std::endl;
      m_error_next_feb = true;
      return false;
    }
  //std::cout << "Second FEB ok, size= " << LeftSize <<std::endl;
  
  //m_FebBlockSize   = LeftSize; //should be =BlockSize;
  m_FebBlockSize=BlockSize;

  //std::cout << "2: FEB found with size " << m_FebBlockSize << std::endl;
  setPointers();
  return true;
}

#endif
