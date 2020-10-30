/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILECALIBDRAWERDAT_H
#define TILECALIBBLOBOBJS_TILECALIBDRAWERDAT_H

/** 
    @brief Generic template class for storing a number of "T"s for each channel or ADC
    @author Nils Gollub <nils.gollub@cern.ch>
    
    @details This class implements the infrastructure to store a
    (variable) number of Ts for each channel or ADC. Only types that
    are a multiple of uint32_t can be stored, i.e. float, uint32_t,
    uint64_t, etc... The number of Ts to store is encoded in the
    objSize header variable. The object version header variable has no
    meaning for this class (since schema evolution does not make sense
    here) and is always set to 0.
*/

#include "TileCalibBlobObjs/TileCalibDrawerBase.h"
#include "TileCalibBlobObjs/TileCalibType.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include <stdint.h>
#include <vector>
#include <iostream>
#include "CoralBase/Blob.h"

template<class T>
class TileCalibDrawerDat : public TileCalibDrawerBase
{
 public:
  /** @brief Object to hold default data used for initialization.
      The length of the first and second vector determines respectively
      the number of gains and values per gain to be stored. The
      indexing is thus DefType[iGain][iValue]. */
  typedef std::vector<std::vector<T> > DefType;
  
  /** @brief Dtor */
  virtual ~TileCalibDrawerDat(){}
  
  //==================================================================
  //== Data getter and setter methods
  //==================================================================
  /** @brief Returns a single T  belonging to a channel/ADC.

      @param adc The gain index; if >= getNGains() it is reset to 0 without warning (default policy)   
      @param idx The index of the requested value */
  T getData(unsigned int channel, unsigned int adc, unsigned int idx) const;
  
  /** @brief Initializing function.
      @param def A reference to a DefType object, specifying the inital layout and values
      @param nChans Number of channels to create
      @param objType Type of the (derived class) object
      @param objVers Version of the (derived class) object
      @param author String containing the author user id
      @param comment String containing the actual comment
      @param timeStamp Unix time stamp (if 0, current time is used) */
  void init(const DefType& def, 
	    uint16_t nChans,
	    uint16_t objVers,
	    const std::string& author="",
	    const std::string& comment="",
	    uint64_t timeStamp=0);
  
  /** @brief Sets a single T belonging to a channel/ADC.
      @param channel The channel number
      @param adc The gain index
      @param idx The index of the value to set 
      @param data The data value to set */
  void setData(unsigned int channel, unsigned int adc, unsigned int idx, T data);
  
  /** @brief Sets a number of Ts stored in the input vector.
      @param channel The channel number
      @param adc The gain index
      @param data A vector containing the values to be set */
  void setData(unsigned int channel, unsigned int adc, const std::vector<T>& data);
  
  
  /** @brief Prints out the content of the blob to std::out */
  virtual void dump() const { dump(std::cout); }
  /** @brief Prints out the content of the blob.
      @param stm The stream to print to */
  virtual void dump(std::ostream& stm) const;
  
 protected:
  /** @brief Ctor (const).
      Derived classes should set doTypeCheck=false and do their own check. */
  TileCalibDrawerDat(const coral::Blob& blob) : TileCalibDrawerBase(blob){}
  /** @brief Ctor (non-const).
      Derived classes should set doTypeCheck=false and do their own check. */
  TileCalibDrawerDat(coral::Blob& blob) : TileCalibDrawerBase(blob){}

  /** @brief Returns a pointer to the first value for the specified channel & ADC. 
      @param channel The channel number; If channel number >= getNChans() 
                     it is reset to channel % (maximum number of channels in drawer) 
                     if channel number > (maximum number of channels in drawer)
                     otherwise it is reset to 0 without warning (default policy)   
      @param adc The gain index; if >= getNGains() it is reset to 0 without warning (default policy) */
  const T* getAddress(unsigned int channel, unsigned int adc) const;
        T* getAddress(unsigned int channel, unsigned int adc);
};

//
//_______________________________________________________________
template<class T> void
TileCalibDrawerDat<T>::init(const DefType&      def, 
			    uint16_t            nChans,
			    uint16_t            objVers,
			    const std::string&  author,
			    const std::string&  comment,
			    uint64_t            timeStamp)
{
  uint16_t nGains   = def.size();
  uint32_t objSize  = nGains>0 ? def.begin()->size() * sizeof(T)/sizeof(uint32_t) : 0; 
  uint32_t nObjs    = nChans*nGains;

  //=== ensure all gains have the same object size
  for(unsigned int iGain=0; iGain<nGains; ++iGain){
    unsigned int thisSize = def[iGain].size()*sizeof(T)/sizeof(uint32_t);
    if( thisSize != objSize ){
      throw TileCalib::SizeConflict("TileCalibDrawerDat<T>::init",thisSize,objSize);
    }
  }
  
  //=== create blob
  createBlob(getType(), objVers, objSize, nObjs, nChans, nGains, author, comment, timeStamp);

  //=== fill the blob with default
  for(unsigned int channel=0; channel<nChans; ++channel){
    for(unsigned int adc=0; adc<nGains; ++adc){
      setData(channel, adc, def[adc]);
    }
  }
}

//
//______________________________________________________________
template<class T> T 
TileCalibDrawerDat<T>::getData(unsigned int channel, unsigned int adc, unsigned int idx) const
{
  if(idx>=getObjSizeUint32()){
    throw TileCalib::IndexOutOfRange("TileCalibDrawerDat::getData",idx,getObjVersion());
  }

  //=== Tile Default Policy

  if(channel >= getNChans()) {
    if (channel <= TileCalibUtils::MAX_CHAN) {
      channel = 0;
    } else {
      channel = channel % TileCalibUtils::MAX_CHAN;
      if (channel >= getNChans()) channel = 0;
    }
  }

  if(adc >= getNGains()) {adc = 0;}

  return getAddress(channel,adc)[idx];
}

//
//______________________________________________________________
template<class T> void 
TileCalibDrawerDat<T>::setData(unsigned int channel, unsigned int adc, unsigned int idx, T data) 
{
  //=== check for out of bounds
  if(channel >= getNChans()){
    throw TileCalib::IndexOutOfRange("TileCalibDrawerDat::getAddress(channel)",channel,getNChans());
  }
  if(adc>=getNGains()){
    throw TileCalib::IndexOutOfRange("CaloCalibDrawerDat::getAddress(gain)",adc,getNGains());
  }
  if(idx>=getObjSizeUint32()){
    throw TileCalib::IndexOutOfRange("TileCalibDrawerDat::getData",idx,getObjVersion());
  }

  getAddress(channel,adc)[idx] = data;
}

//
//______________________________________________________________
template<class T> void 
TileCalibDrawerDat<T>::setData(unsigned int channel, unsigned int adc, const std::vector<T>& data)
{
  for(typename std::vector<T>::const_iterator i=data.begin(); i!=data.end(); ++i){
    setData(channel,adc,i-data.begin(),*i);
  }
}

//
//______________________________________________________________
template<class T> const T* 
TileCalibDrawerDat<T>::getAddress(unsigned int channel, unsigned int adc) const
{
  unsigned int idx = channel*getNGains() + adc;
  return static_cast<const T*>(TileCalibDrawerBase::getAddress(idx));
}

//
//______________________________________________________________
template<class T> T* 
TileCalibDrawerDat<T>::getAddress(unsigned int channel, unsigned int adc)
{
  unsigned int idx = channel*getNGains() + adc;
  return static_cast<T*>(TileCalibDrawerBase::getAddress(idx));
}

//
//______________________________________________________________
template<class T> void 
TileCalibDrawerDat<T>::dump(std::ostream& stm) const
{
  TileCalibDrawerBase::dumpHeader(stm);
  for(unsigned int channel=0; channel<getNChans(); ++channel){
    for(unsigned int adc=0; adc<getNGains(); ++adc){
      stm << channel<<"/"<<adc<<":\t";
      for(unsigned int idx=0; idx<getObjSizeUint32(); ++idx){
	stm << getData(channel,adc,idx) << "\t";
      }
      stm << std::endl;
    }
  }
}


#endif
