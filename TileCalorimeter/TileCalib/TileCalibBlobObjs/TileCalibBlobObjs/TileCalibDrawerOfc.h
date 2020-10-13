/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBDRAWEROFC_H
#define TILECALIBDRAWEROFC_H

/** 
 *  @brief Class for storing Optimal Filtering Coefficients (OFCs) in a coral::Blob.
 *  @author Nils Gollub <nils.gollub@cern.ch>
 *
 *  @details This class extends the header defined in
 *  TileCalibDrawerBase by two additional 32 bit words:
 *  - nSamples: The number of samples used in OFC computation
 *  - nPhases:  The number of phases stored in the BLOB.
 *
 *  Directly after these two extra header words a list of phases is
 *  stored. The phases are encoded as integers so that exact
 *  comparisons are possible. Since phases are only needed with a
 *  resolution of 0.1ns, a good convention is to store int(10*phase).
 *
 *  If the nPhases is set to a negative value in initialization with
 *  init(...), only one list of phases common for all ADCs is stored. 
 */

#include "TileCalibBlobObjs/TileCalibDrawerBase.h"
#include "TileCalibBlobObjs/TileCalibType.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "CoralBase/Blob.h"
#include <stdint.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <cmath>

#define PHASE_PRECISION 0.1

class TileCalibDrawerOfc : public TileCalibDrawerBase {

 public:

  /** @brief OFC field identifier. */
  enum FIELD{FieldA = 0, FieldB = 1, FieldG = 2, FieldC = 3, FieldDG = 4};

  /** @brief Dtor */
  virtual ~TileCalibDrawerOfc(){}

  /** @brief Returns TileCalibType::OFC. */
  virtual uint16_t getType()const {return TileCalibType::OFC;} 

  /** @brief Returns a pointer to a non-const TileCalibDrawerOfc.
      @details A negative nPhases value implies that the same phase table is used for all stored ADCs.
      @param blob Reference to coral::Blob
      @param objVersion The objects version
      @param nSamples The number of samples to store.
      @param nPhases The number of phases to be stored for each ADC (negative: same phases for all ADCs) 
      @param nChans The number of channels 
      @param nGains The number of gains
      @param author String containing the author user id
      @param comment String containing the actual comment
      @param timeStamp Unix time stamp (if 0, current time is used) */
  static TileCalibDrawerOfc* getInstance(coral::Blob&         blob
                                         , uint16_t           objVersion
                                         , uint32_t           nSamples
                                         , int32_t            nPhases
                                         , uint16_t           nChans
                                         , uint16_t           nGains
                                         , const std::string& author = ""
                                         , const std::string& comment = ""
                                         , uint64_t           timeStamp = 0);

  /** @brief Returns a pointer to a const TileCalibDrawerOfc. */
  static const TileCalibDrawerOfc* getInstance(const coral::Blob& blob);
  

  //==================================================================
  //== Accessors to extra header fields
  //==================================================================
  /** @brief Returns the number of sample stored. */
  uint32_t getNSamples() const;
  /** @brief Returns the number of phases (WARNING: Can be negative!) */
  int32_t getNPhases() const;
  /** @brief Returns the stored phase
      @param channel The channel number
      @param adc The gain index
      @param phaseIdx The phase index */
  float getPhase(unsigned int channel, unsigned int adc, unsigned int phaseIdx) const;
  /** @brief Returns the number of fields.
      Returns 3 for OF1, 4 for OF2, 5 for OF2+pulse_derivative.
      @param objVersion The object version 
      If default objVersion is passed, the object version is determined from the Blob. */
  uint32_t getNFields(uint16_t objVersion = 0) const;
  
  //==================================================================
  //== Accessors to OFC data
  //==================================================================

  /** @brief Returns OFC data.
      @param field The field identifier
      @param channel The channel number; If channel number >= getNChans() 
                     it is reset to channel % (maximum number of channels in drawer) 
                     if channel number > (maximum number of channels in drawer)
                     otherwise it is reset to 0 without warning (default policy)   
      @param adc The gain index; if >= getNGains() it is reset to 0 without warning (default policy)
      @param phase The phase of interest
      @param sample The sample of interest */
  float  getOfc(unsigned int field, unsigned int channel, unsigned int adc, float phase, unsigned int sample) const;

  /** @brief Fill all OFC for optimazation.
      @param channel The channel number; If channel number >= getNChans() 
                     it is reset to channel % (maximum number of channels in drawer) 
                     if channel number > (maximum number of channels in drawer)
                     otherwise it is reset to 0 without warning (default policy)   
      @param adc The gain index; if >= getNGains() it is reset to 0 without warning (default policy)
      @param phase The phase of interest */
  void fillOfc (unsigned int channel,unsigned int adc, float& phase, float* w_a, float* w_b, float* w_c, float* g, float* dg ) const;


  /** @brief Prints out the object content to std::cout */
  virtual void dump() const { dump(std::cout); } 
  /** @brief Prints out the object content 
      @param stm The stream to print to */
  virtual void dump(std::ostream& stm) const;
  
  //==================================================================
  //== Setter methods
  //==================================================================
  /** @brief Function for initializing a TileCalibDrawerOfc BLOB. 
   *  @param objVersion The objects version
   *  @param nSamples The number of samples to store.
   *  @param nPhases The number of phases to be stored for each ADC (negative: same phases for all ADCs) 
   *  @param nChans The number of channels 
   *  @param nGains The number of gains
   *  @param author String containing the author user id
   *  @param comment String containing the actual comment
   *  @param timeStamp Unix time stamp (if 0, current time is used) 
   *
   *  A negative nPhases value implies that the same phase table is used for all stored ADCs. 
   */
  void init(uint16_t objVersion
            , uint32_t nSamples
            , int32_t  nPhases
            , uint16_t nChans
            , uint16_t nGains
            , const std::string& author=""
            , const std::string& comment=""
            , uint64_t timeStamp=0);
  
  /** @brief Sets OFC data.
      @param field The field identifier
      @param channel The channel number
      @param adc The gain index
      @param phase The phase 
      @param sample The sample number 
      @param value The value to set */
  void setOfc(unsigned int field, unsigned int channel, unsigned int adc, float phase, unsigned int sample, float value);
  
  /** @brief Sets a phase value.
      @param channel The channel number
      @param adc The gain index
      @param phases Vector containing the phases to set */
  void setPhases(unsigned int channel, unsigned int adc, const std::vector<float>& phases); 
  
 protected:
  /** @brief Ctor (const). */
  TileCalibDrawerOfc(const coral::Blob& blob);
  /** @brief Ctor (non0const). */
  TileCalibDrawerOfc(coral::Blob& blob);
  
 private:
  void initCheck();

  /** @brief Returns pointer to first data OFC for a given field, ADC & phase.
      @param field The field identifier
      @param channel The channel number
      @param adc The gain index
      @param phase The phase */
  const float* getOfcStartAddress(unsigned int field, unsigned int channel, unsigned int adc, float& phase) const;
       float* getOfcStartAddress(unsigned int field, unsigned int channel, unsigned int adc, float& phase);

  unsigned int getOfcStartOffset(unsigned int field, unsigned int channel, unsigned int adc, float& phase) const;

  /** @brief Returns pointer to the requested phase value
      @param channel The channel number
      @param adc The gain index
      @param phaseIdx The phase index */
  const int32_t* getPhaseStartAddress(unsigned int channel, unsigned int adc, unsigned int phaseIdx) const;
       int32_t* getPhaseStartAddress(unsigned int channel, unsigned int adc, unsigned int phaseIdx);

  unsigned int getPhaseStartOffset(unsigned int channel, unsigned int adc, unsigned int phaseIdx) const;

  /** Returns the index for a given phase 
      @param channel The channel number
      @param adc The gain index
      @param phase The phase of interest */
  unsigned int getPhaseNumber(unsigned int channel, unsigned int adc, float& phase) const;
};

//
//______________________________________________________________
__attribute__((always_inline)) 
inline uint32_t TileCalibDrawerOfc::getNSamples() const {
  return *(static_cast<const uint32_t*>(getAddress(0)));
}

//
//______________________________________________________________
__attribute__((always_inline)) 
inline int32_t TileCalibDrawerOfc::getNPhases() const {
  return *(static_cast<const int32_t*>(getAddress(1)));
}

//
//______________________________________________________________
__attribute__((always_inline)) 
inline unsigned int TileCalibDrawerOfc::getPhaseNumber(unsigned int channel, unsigned int adc, float& phase) const {

  int db_phase = (int) std::round(phase * (1 / PHASE_PRECISION)); // Phases are stored as int(10*phase) in DB
  const int32_t* beg = getPhaseStartAddress(channel, adc, 0);
  const int32_t* end = beg + std::abs(getNPhases());
  const int32_t* pos = std::lower_bound(beg, end, db_phase);

  if (pos == end || (*pos != db_phase && pos != beg && (*pos - db_phase) > (db_phase - *(pos - 1)))) {
    --pos;
  }

  phase = *pos * PHASE_PRECISION;
  return (pos - beg);
}

//
//______________________________________________________________
__attribute__((always_inline)) 
inline float TileCalibDrawerOfc::getPhase(unsigned int channel, unsigned int adc, unsigned int phaseIdx) const {
  return *(getPhaseStartAddress(channel, adc, phaseIdx)) * PHASE_PRECISION; // Phases are stored as int(10*phase) in DB
}

//
//______________________________________________________________
__attribute__((always_inline)) 
inline uint32_t TileCalibDrawerOfc::getNFields(uint16_t objVersion) const {
  //=== determine objVersion from Blob if default is passed
  if(objVersion==0) objVersion = getObjVersion();

  if(     objVersion == 3){ return 5; }
  else if(objVersion == 2){ return 4; }
  else if(objVersion == 1){ return 3; }
  else{ 
    throw TileCalib::VersionConflict("TileCalibDrawerOfc::getNFields", objVersion); 
  }
}

//
//______________________________________________________________
inline float TileCalibDrawerOfc::getOfc(unsigned int field, unsigned int channel, unsigned int adc
                                        , float phase, unsigned int sample) const {
  if(sample >= getNSamples())
    throw TileCalib::IndexOutOfRange("TileCalibDrawerOfc::getA", sample, getNSamples());
  return getOfcStartAddress(field, channel, adc, phase)[sample];
}

inline void TileCalibDrawerOfc::fillOfc (unsigned int channel,unsigned int adc, float& phase
                                         , float* w_a, float* w_b, float* w_c, float* g, float* dg) const {


  const float* startAddress = getOfcStartAddress(TileCalibDrawerOfc::FieldA, channel, adc, phase);
  size_t allSamplesSize = getNSamples() * sizeof(float); 
  size_t fieldSize = getObjSizeUint32() * getNSamples();

  memcpy(w_a, startAddress, allSamplesSize);
  startAddress += fieldSize;
  memcpy(w_b, startAddress, allSamplesSize);
  startAddress += fieldSize;
  memcpy(g, startAddress, allSamplesSize);
  startAddress += fieldSize;
  memcpy(w_c, startAddress, allSamplesSize);
  if (getNFields() > 4) {
    startAddress += fieldSize;
    memcpy(dg, startAddress, allSamplesSize);
  } else {
    memset(dg, 0, allSamplesSize);
  }
}


//
//______________________________________________________________
inline void TileCalibDrawerOfc::setOfc(unsigned int field,unsigned int channel, unsigned int adc
                                       , float phase, unsigned int sample, float value) {
  if(sample >= getNSamples())
    throw TileCalib::IndexOutOfRange("TileCalibDrawerOfc::setOfc", sample, getNSamples());
  getOfcStartAddress(field, channel, adc, phase)[sample] = value;
}

//
//______________________________________________________________
__attribute__((always_inline)) 
inline unsigned int TileCalibDrawerOfc::getPhaseStartOffset(unsigned int channel, unsigned int adc, unsigned int phaseIdx) const
{
  if(phaseIdx >= static_cast<unsigned int>(std::abs(getNPhases()))){
    throw TileCalib::IndexOutOfRange("TileCalibDrawerOfc::getPhaseStartAddress", phaseIdx, std::abs(getNPhases()));
  }
  //=== use only one phase table for all ADCs
  if(getNPhases() < 0){
    channel = 0;
    adc = 0;
  }
  //=== check for out of bounds channel, adc
  if(channel > getNChans())
    throw TileCalib::IndexOutOfRange("TileCalibDrawerOfc::getPhaseStartAddress", channel, getNChans());
  if(adc > getNGains())
    throw TileCalib::IndexOutOfRange("TileCalibDrawerOfc::getPhaseStartAddress", adc, getNGains());

  //=== extra header
  unsigned int offset = 2;
  unsigned int nPhases = std::abs(getNPhases());
  offset += channel * nPhases * getNGains() + adc * nPhases + phaseIdx;
  return offset;
}

//
//______________________________________________________________
__attribute__((always_inline)) 
inline const int32_t* TileCalibDrawerOfc::getPhaseStartAddress(unsigned int channel, unsigned int adc, unsigned int phaseIdx) const
{
  return static_cast<const int32_t*>(getAddress(getPhaseStartOffset(channel, adc, phaseIdx)));
}

//
//______________________________________________________________
__attribute__((always_inline)) 
inline int32_t* TileCalibDrawerOfc::getPhaseStartAddress(unsigned int channel, unsigned int adc, unsigned int phaseIdx)
{
  return static_cast<int32_t*>(getAddress(getPhaseStartOffset(channel, adc, phaseIdx)));
}

//
//______________________________________________________________
__attribute__((always_inline)) 
inline unsigned int TileCalibDrawerOfc::getOfcStartOffset(unsigned int field, unsigned int channel, unsigned int adc, float& phase) const
{
  //=== check default policy

  if(channel >= getNChans()) {
    if (channel <= TileCalibUtils::MAX_CHAN) {
      channel = 0;
    } else {
      channel = channel % TileCalibUtils::MAX_CHAN;
      if (channel >= getNChans()) channel = 0;
    }
  }


  if(adc >= getNGains()) {adc = 0;}
  
  //=== this calculation is only valid for a blob layout according 
  //=== to version 1 or version 2
  uint16_t objVer = getObjVersion();  
  if(objVer!=1 && objVer!=2 && objVer!=3){ 
    throw TileCalib::VersionConflict("TileCalibDrawerOfc::getOfcStartAddress", objVer);
  }
  
  //=== number of fields:
  //=== for Version 1 = OFC1 we have 3 fields: ai, bi, gi
  //=== for Version 2 = OFC2 we have 4 fields: ai, bi, gi, ci
  //=== for Version 3 = OFC2 we have 5 fields: ai, bi, gi, ci, dgi
  unsigned int nField = getNFields(objVer);
  if(field >= nField){
    throw TileCalib::IndexOutOfRange("TileCalibDrawerOfc::getOfcStartAddress", field, nField);
  }
  
  //=== calculate the offset
  //=== first get rid of ofc specific header
  unsigned int nPhaseAbs = std::abs(getNPhases());
  int          nPhaseSgn = getNPhases() < 0 ? -1 : 1; 
  unsigned int offset = 2;
  if(nPhaseSgn < 0) { offset += nPhaseAbs;                             }
  else              { offset += nPhaseAbs * getNChans() * getNGains(); }

  //=== add offset for requested field
  unsigned int iPhase  = getPhaseNumber(channel, adc, phase);
  unsigned int nGain   = getNGains();
  unsigned int nSample = getNSamples(); 
  unsigned int lPhase  = nSample * nField;
  unsigned int lAdc    = lPhase * nPhaseAbs;
  unsigned int lChan   = lAdc * nGain;
  offset += channel * lChan + adc * lAdc + iPhase * lPhase + field * nSample;
  
  return offset;
}

//
//______________________________________________________________
__attribute__((always_inline)) 
inline const float* TileCalibDrawerOfc::getOfcStartAddress(unsigned int field, unsigned int channel, unsigned int adc, float& phase) const
{
  return static_cast<const float*>(getAddress(getOfcStartOffset(field, channel, adc, phase)));
}

//
//______________________________________________________________
__attribute__((always_inline)) 
inline float* TileCalibDrawerOfc::getOfcStartAddress(unsigned int field, unsigned int channel, unsigned int adc, float& phase)
{
  return static_cast<float*>(getAddress(getOfcStartOffset(field, channel, adc, phase)));
}

#endif
