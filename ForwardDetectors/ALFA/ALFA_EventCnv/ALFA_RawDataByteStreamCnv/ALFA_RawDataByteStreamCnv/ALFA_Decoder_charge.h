/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_DECODER_CHARGE_H
#define ALFA_DECODER_CHARGE_H

#include <stdint.h>
#include <map>
#include <string>
#include "eformat/ROBFragment.h"
#include "eformat/FullEventFragment.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "ByteStreamData/RawEvent.h" 

#include "ALFA_RawEv/ALFA_RawDataContainer_charge.h"
#include "ALFA_RawEv/ALFA_RawDataCollection_charge.h"
#include "ALFA_RawEv/ALFA_RawData_charge.h"

#include "ALFA_RawDataContainerReadOut.h"
#include "ALFA_RawDataCollectionReadOut.h"
#include "ALFA_RawDataReadOut_charge.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"


class ISvcLocator;
class StatusCode;

class ALFA_Decoder_charge: public AthAlgTool {

 public:


  // Collection type
  typedef ALFA_RawData_charge m_ALFA_RawData_charge;
  typedef ALFA_RawDataCollection_charge m_ALFA_RawDataCollection_charge; 
  
  // constructor
  ALFA_Decoder_charge (const std::string& type, const std::string& name, const IInterface* parent);
  
  // AlgTool InterfaceID
  static const InterfaceID& interfaceID();
  
  // destructor 
  virtual ~ALFA_Decoder_charge();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *robFrag, ALFA_RawDataContainer_charge* rdoCont,  std::vector<unsigned int>* vecHash = NULL); 
 
ALFA_RawDataCollection_charge* getCollection (unsigned int MotherBoardId, ALFA_RawDataContainer_charge* cont);
ALFA_RawData_charge* getRawData_charge (unsigned int PMFId, ALFA_RawDataCollection_charge* coll);

 inline const eformat::FullEventFragment<const uint32_t*> * eventFragment() const { return m_event; }
 inline const eformat::ROBFragment<const uint32_t*> * robFragment() const { return m_robFrag; }


private:
  const eformat::FullEventFragment<const uint32_t*> * m_event;
  const eformat::ROBFragment<const uint32_t*> * m_robFrag;

  StoreGateSvc *m_EvtStore;
  unsigned int                m_fragment_number;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc>   m_robDataProvider;

  // Methods for the decoding
  ALFA_RawDataReadOut_charge*       m_ALFA_RawDataReadOut_charge;
  ALFA_RawDataCollectionReadOut*       m_ALFA_RawDataCollectionReadOut;
  ALFA_RawDataContainerReadOut*       m_ALFA_RawDataContainerReadOut;
  
 };

#endif

