/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LAROFCFLAT_H
#define LARCOOLCONDITIONS_LAROFCFLAT_H

#include "LArElecCalib/ILArOFC.h" 
#include "LArCOOLConditions/LArCondFlatBase.h"
#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include <vector>

class CondAttrListCollection;

class LArOFCFlat: public ILArOFC, public LArCondFlatBase {

 private:
  LArOFCFlat();

 public:   
  typedef ILArOFC::OFCRef_t OFCRef_t;
  enum {ERRORCODE = LArElecCalib::ERRORCODE};

  LArOFCFlat(const CondAttrListCollection* attrList);
  virtual ~LArOFCFlat( );

  bool good() const { return m_isInitialized && m_nChannels>0; }

  

  // retrieving coefficients using online ID
  
  virtual OFCRef_t OFC_a(const HWIdentifier&  CellID,
                         int gain,
                         int tbin=0) const ;

  virtual OFCRef_t OFC_b(const HWIdentifier&  CellID,
                         int gain,
                         int tbin=0) const ;
  


  OFCRef_t OFC_a(const IdentifierHash& hs,int gain) const {
    const float* pStart=m_pOFCa[gain]+(hs*m_nSamples);
    if (*pStart==ERRORCODE) 
      return OFCRef_t(NULL,NULL);
    else
      return OFCRef_t(pStart,pStart+m_nSamples);
  }

  OFCRef_t OFC_b(const IdentifierHash& hs,int gain) const {
    const float* pStart=m_pOFCb[gain]+(hs*m_nSamples);
    if (*pStart==ERRORCODE) 
      return OFCRef_t(NULL,NULL);
    else
      return OFCRef_t(pStart,pStart+m_nSamples);
  }


  // retrieving time offset using online/offline ID

  virtual  float timeOffset(const HWIdentifier&  CellID, int gain) const;

  //For the TB / cosmic case: retrieve the number of time-bins (aka "phases")
  virtual unsigned nTimeBins(const HWIdentifier&  CellID, int gain) const;
 
  //For the TB / cosmic case: retrieve the witdth of the time bin (default 24 bins in 25 ns)
  virtual float timeBinWidth(const HWIdentifier&  CellID, int gain) const;


 private: 
  std::vector<const float*> m_pOFCa;
  std::vector<const float*> m_pOFCb;
  std::vector<const float*> m_pTimeOffset;

  unsigned m_nChannels;
  unsigned m_nSamples;
};  
  
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArOFCFlat, 20294702, 1) 
CONDCONT_DEF( LArOFCFlat, 117247562, ILArOFC );
 

#endif 
