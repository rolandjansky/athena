//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TBCHECKBCIDS
#define TBCHECKBCIDS

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" 
#include "ByteStreamData/RawEvent.h" 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
//#include  "StoreGate/StoreGateSvc.h"


#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"

class TBCheckBCIDs : public AthAlgorithm
{
 public:
  TBCheckBCIDs(const std::string & name, ISvcLocator * pSvcLocator);

  ~TBCheckBCIDs();

  //standart algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  int m_count;
  //StoreGateSvc *m_storeGateSvc;
  //ByteStreamCnvSvc* m_ByteStreamEventAccess;
  IROBDataProviderSvc *m_rdpSvc;

  template <class T>
    inline bool check_valid (const T* frag, MsgStream& log);

  NTuple::Tuple* m_ntuplePtr;
  NTuple::Item<long> m_LVL1ID,m_EventID;
  NTuple::Item<long> m_RODIndex;
  NTuple::Array<long> m_RODID;
  NTuple::Array<long> m_BCID;
  //NTuple::Item<long> m_nRODS;

};




template <class T>
inline bool TBCheckBCIDs::check_valid (const T* frag, MsgStream& log)
{ 
  try
    {
      frag->check();
    }
  catch (...)
    {
      log << MSG::FATAL << "Invalid Fragment" << endreq;
      return false;
    }
  return true;
}


#endif
