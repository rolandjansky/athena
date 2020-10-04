/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDBLOBOBJS_CALOCONDBLOBDAT_H
#define CALOCONDBLOBOBJS_CALOCONDBLOBDAT_H

/** 
    @brief Generic template class for storing a number of "T"s for each channel/gain
    @author Nils Gollub <nils.gollub@cern.ch>
    
    @details This class implements the infrastructure to store a
    (variable) number of Ts for each channel/gain. Only types that
    are a multiple of uint32_t can be stored, i.e. float, uint32_t,
    uint64_t, etc... The number of Ts to store is encoded in the
    objSize header variable. The object version header variable has no
    meaning for this class (since schema evolution does not make sense
    here) and is always set to 0.
*/

#include "CaloCondBlobObjs/CaloCondBlobBase.h"
#include "CaloCondBlobObjs/CaloCondType.h"
#include <stdint.h>
#include <vector>
#include <ostream>
#include <iostream>
#include "CoralBase/Blob.h"

template<class T>
class CaloCondBlobDat : public CaloCondBlobBase
{
 public:
  /** @brief Object to hold default data used for initialization.
      @details The length of the outer and inner vector determines respectively
      the number of gains and values per gain to be stored. The
      indexing is thus DefType[iGain][iValue]. */
  typedef std::vector<std::vector<T> > DefType;
  
  /** @brief Dtor */
  virtual ~CaloCondBlobDat(){}
  
  //==================================================================
  //== Data getter and setter methods
  //==================================================================
  /** @brief Returns a single T  belonging to a channel/gain.
      @param channel The channel number
      @param adc The gain index
      @param idx The index of the requested value */
  T getData(const unsigned int channel, const unsigned int adc, const unsigned int idx) const;
  
  /** @brief Initializing function.
      @param def A reference to a DefType object, specifying the inital layout and values
      @param nChans Number of channels to create
      @param objType Type of the object
      @param objVers Version of the object
      @param author String containing the author user id
      @param comment String containing the actual comment
      @param timeStamp Unix time stamp (if 0, current time is used) */
  void init(const DefType& def, 
	    uint32_t nChans,
	    uint16_t objVers,
	    const std::string& author="",
	    const std::string& comment="",
	    uint64_t timeStamp=0);
  
  /** @brief Sets a single T belonging to a channel/gain.
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
  /** @brief Ctor. */
  CaloCondBlobDat(const coral::Blob& blob) : CaloCondBlobBase(blob){}
  /** @brief Ctor. */
  CaloCondBlobDat(coral::Blob& blob) : CaloCondBlobBase(blob){}


  /** @brief Returns a pointer to the first value for the specified channel/gain. 
      @param channel The channel number; if >= getNChans() it is reset to 0 without warning (default policy)   
      @param adc The gain index; if >= getNGains() it is reset to 0 without warning (default policy) */
  const T* getAddress(const unsigned int channel, const unsigned int adc) const;
  T* getAddress(const unsigned int channel, const unsigned int adc);
};

//
//_______________________________________________________________
template<class T> void
CaloCondBlobDat<T>::init(const DefType&      def, 
			    uint32_t            nChans,
			    uint16_t            objVers,
			    const std::string&  author,
			    const std::string&  comment,
			    uint64_t            timeStamp)
{
  uint32_t objSize  = def.begin()->size() * sizeof(T)/sizeof(uint32_t); 
  uint16_t nGains   = def.size();
  uint32_t nObjs    = nChans*nGains;
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
CaloCondBlobDat<T>::getData(const unsigned int channel, const unsigned int adc, const unsigned int idx) const
{
  if(idx<getObjSizeUint32()){return getAddress(channel,adc)[idx];}
  else{throw CaloCond::IndexOutOfRange("CaloCondBlobDat::getData",idx,getObjVersion());}
}

//
//______________________________________________________________
template<class T> void 
CaloCondBlobDat<T>::setData(unsigned int channel, unsigned int adc, unsigned int idx, T data) 
{
  if(idx<getObjSizeUint32()){ getAddress(channel,adc)[idx] = data;}
  else{throw CaloCond::IndexOutOfRange("CaloCondBlobDat::setData",idx,getObjVersion());}
}

//
//______________________________________________________________
template<class T> void 
CaloCondBlobDat<T>::setData(unsigned int channel, unsigned int adc, const std::vector<T>& data)
{
  for(typename std::vector<T>::const_iterator i=data.begin(); i!=data.end(); ++i){
    setData(channel,adc,i-data.begin(),*i);
  }
}

//
//______________________________________________________________
template<class T> const T* 
CaloCondBlobDat<T>::getAddress(const unsigned int channel, const unsigned int adc) const
{
  //=== check for out of bounds
  if(channel >= getNChans()){
    throw CaloCond::IndexOutOfRange("CaloCondBlobDat::getAddress(channel)",channel,getNChans());
  }
  if(adc>=getNGains()){
    throw CaloCond::IndexOutOfRange("CaloCondBlobDat::getAddress(gain)",adc,getNGains());
  }
  
  const unsigned int idx = channel*getNGains() + adc;
  return static_cast<const T*>(CaloCondBlobBase::getAddress(idx));
}

//
//______________________________________________________________
template<class T> T* 
CaloCondBlobDat<T>::getAddress(const unsigned int channel, const unsigned int adc)
{
  //=== check for out of bounds
  if(channel >= getNChans()){
    throw CaloCond::IndexOutOfRange("CaloCondBlobDat::getAddress(channel)",channel,getNChans());
  }
  if(adc>=getNGains()){
    throw CaloCond::IndexOutOfRange("CaloCondBlobDat::getAddress(gain)",adc,getNGains());
  }
  
  const unsigned int idx = channel*getNGains() + adc;
  return static_cast<T*>(CaloCondBlobBase::getAddress(idx));
}

//
//______________________________________________________________
template<class T> void 
CaloCondBlobDat<T>::dump(std::ostream& stm) const
{
  CaloCondBlobBase::dumpHeader(stm);
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
