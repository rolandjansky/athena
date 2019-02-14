/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArDSPConfig.h" 
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

LArDSPConfig::LArDSPConfig():
  m_attrList(nullptr),m_pBlob(nullptr),m_nFebs(0) 
{}

LArDSPConfig::~LArDSPConfig()  {}


LArDSPConfig::LArDSPConfig(const AthenaAttributeList* attrList): 
  m_attrList(attrList),m_pBlob(nullptr),m_nFebs(0) {
  
 
  StatusCode sc=initializeBase("LArDSPConfig");
  if (sc.isFailure()) return;

  const coral::Blob& myBlob = (*m_attrList)["febdata"].data<coral::Blob>();
  m_pBlob=static_cast<const uint8_t*>(myBlob.startingAddress());
  m_nFebs=myBlob.size()/sizeof(uint8_t); //FIXME check if we get the expected value
  //m_nSamples=m_attr["nSamples"].data<unsigned>();
}

uint8_t LArDSPConfig::peakSampleByHash(const IdentifierHash& febHash) const  { 
  return (m_pBlob[febHash] & PEAKSAMPLE_MASK);
}

bool  LArDSPConfig::useMGRampInterceptByHash(const IdentifierHash& febHash) const {
  return ((m_pBlob[febHash] & MGRAMPINTERCEPT_MASK) !=0);
}

bool  LArDSPConfig::useHGRampInterceptByHash(const IdentifierHash& febHash) const {
  return ((m_pBlob[febHash] & HGRAMPINTERCEPT_MASK) !=0);
}

bool  LArDSPConfig::useLGRampInterceptByHash(const IdentifierHash& febHash) const {
  return ((m_pBlob[febHash] & LGRAMPINTERCEPT_MASK) !=0);
}

LArDSPConfigWrite::LArDSPConfigWrite() : m_pBlob_nc(nullptr)  {

  StatusCode sc=this->initializeBase("LArDSPConfigWrite");
  if (sc.isFailure()) return;
    

  m_nFebs=m_onlineHelper->febHashMax();
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("febdata", "blob");
  spec->extend<unsigned>("version");
  //spec->extend<unsigned>("nSamples");
  m_attrListNC=std::unique_ptr<AthenaAttributeList>(new AthenaAttributeList(*spec));

  (*m_attrListNC)["version"].setValue(0U);
  coral::Blob& myBlob = (*m_attrListNC)["febdata"].data<coral::Blob>();
  myBlob.resize(m_nFebs*sizeof(uint8_t));
  m_pBlob_nc=static_cast<uint8_t*>(myBlob.startingAddress());
  m_pBlob=m_pBlob_nc; //implict cast to const-ptr (for the underlying reading object

  //Initialize the blob to 0
  for (unsigned i=0;i<m_nFebs;++i)
    m_pBlob_nc[i]=0;

  // cppcheck-suppress memleak
  spec = nullptr;
  return;
}

void LArDSPConfigWrite::set(const IdentifierHash febId, 
			    const uint8_t peakSample, const bool useMGRampIntercept) {
  
  m_pBlob_nc[febId]= (peakSample & PEAKSAMPLE_MASK);
  if (useMGRampIntercept) { 
    m_pBlob_nc[febId] |= MGRAMPINTERCEPT_MASK;
  }
  m_pBlob_nc[febId] |= LGRAMPINTERCEPT_MASK;
  return;

}
