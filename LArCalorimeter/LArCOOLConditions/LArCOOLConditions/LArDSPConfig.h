/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARRAMPFLAT_H
#define LARCOOLCONDITIONS_LARRAMPFLAT_H

#include "LArCOOLConditions/LArCondFlatBase.h"
#include <vector>
#include <memory>

class AthenaAttributeList;

class LArDSPConfig: 
  public LArCondFlatBase
{  

protected:
  LArDSPConfig();

 public:  

  enum {
    PEAKSAMPLE_MASK=0x1F,
    MGRAMPINTERCEPT_MASK=0x40,
  };


  LArDSPConfig(const AthenaAttributeList* attrList);
  virtual ~LArDSPConfig();

  bool good() const { return m_isInitialized && m_nFebs>0; }


  bool  useMGRampInterceptByHash(const IdentifierHash& febHash) const;
  bool useMGRampIntercept(const HWIdentifier FEBid) const {
    return useMGRampInterceptByHash(m_onlineHelper->feb_Hash(FEBid));
  };

  uint8_t peakSampleByHash(const IdentifierHash& febHash) const;
  uint8_t peakSample(const HWIdentifier FEBid) const {
    return peakSampleByHash(m_onlineHelper->feb_Hash(FEBid));
  }

protected:
  const AthenaAttributeList* m_attrList;
  const uint8_t* m_pBlob;
  unsigned m_nFebs;
		   
};

CLASS_DEF( LArDSPConfig , 131172224 , 1 )


class LArDSPConfigWrite  : public LArDSPConfig {

  
public:
  LArDSPConfigWrite();

  //void initializeBlob(const nSamples);

  void set(const IdentifierHash febId, 
	   const uint8_t peakSample, 
	   const bool useMGRampIntercept);

  std::unique_ptr<AthenaAttributeList> attributeList() { return std::move(m_attrListNC); }

private:
  std::unique_ptr<AthenaAttributeList> m_attrListNC;
  uint8_t* m_pBlob_nc;
  //uint8_t m_nSamples;
};


#endif 
