/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "ByteStreamData/RawEvent.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "GaudiKernel/IIncidentListener.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h"

// For Read Handle
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventInfo.h"

#include <set>
#include <string>


class ISCT_ByteStreamErrorsSvc;
class ISCT_RodDecoder;

class SCTRawDataProviderTool : virtual public ISCTRawDataProviderTool, 
  virtual public AthAlgTool, 
  virtual public IIncidentListener
{

 public:
   
  //! AlgTool InterfaceID
  //  static const InterfaceID& interfaceID( ) ;
  
  //! constructor
  SCTRawDataProviderTool(const std::string& type, const std::string& name,
                         const IInterface* parent ) ;

  //! destructor 
  virtual ~SCTRawDataProviderTool();

  //! initialize
  virtual StatusCode initialize() override;

  //! finalize
  virtual StatusCode finalize() override;
  
  //! this is the main decoding method
  virtual StatusCode convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                             SCT_RDO_Container& rdoIdc,
                             InDetBSErrContainer* errs) override;

  /** function to be executed at BeginEvent incident */
  virtual void handle(const Incident& inc) override;

 private: 
  
  ToolHandle<ISCT_RodDecoder> m_decoder{this, "Decoder", "SCT_RodDecoder", "Decoder"};
  ServiceHandle<ISCT_ByteStreamErrorsSvc> m_bsErrSvc;
  
  // bookkeeping if we have decoded a ROB already
  std::set<uint32_t> m_robIdSet;

  SG::ReadHandleKey<xAOD::EventInfo> m_xevtInfoKey;
  SG::ReadHandleKey<EventInfo> m_evtInfoKey;
};

#endif // SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDERTOOL_H
