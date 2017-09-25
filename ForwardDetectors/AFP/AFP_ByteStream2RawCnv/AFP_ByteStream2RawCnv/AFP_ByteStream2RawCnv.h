/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECODER_AFP_DECODER_H
#define DECODER_AFP_DECODER_H

#include <map>
#include <stdint.h>
#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!

#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolFactory.h"
#include "StoreGate/StoreGateSvc.h"

#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "eformat/FullEventFragment.h"
#include "eformat/ROBFragment.h"

#include "AFP_RawDataCollectionReadOut.h"
#include "AFP_RawDataContainerReadOut.h"
#include "AFP_RawDataReadOut.h"

#include "AFP_RawEv/AFP_RawData.h"
#include "AFP_RawEv/AFP_RawDataCollection.h"
#include "AFP_RawEv/AFP_RawDataContainer.h"

using eformat::helper::SourceIdentifier;

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
using OFFLINE_FRAGMENTS_NAMESPACE::FullEventFragment;
class ISvcLocator;
class StatusCode;

class AFP_ByteStream2RawCnv : public ::AthAlgTool {
public:
  typedef AFP_RawData m_AFP_RawData;
  typedef AFP_RawDataCollection m_AFP_RawDataCollection;

  AFP_ByteStream2RawCnv(const std::string &type, const std::string &name,
                        const IInterface *parent);

  static const InterfaceID &interfaceID();

  virtual ~AFP_ByteStream2RawCnv();

  virtual StatusCode initialize();
  // virtual StatusCode  execute();
  virtual StatusCode finalize();

  //  StatusCode getEvent();
  StatusCode fillCollection(const ROBFragment *robFrag,
                            AFP_RawDataContainer *rdoCont,
                            std::vector<unsigned int> *vecHash = NULL);

  AFP_RawDataCollection* getCollection(const unsigned int columnNum, 
				       const unsigned int robID,
				       AFP_RawDataContainer *cont);


  AFP_RawData *getRawData(unsigned int Link, AFP_RawDataCollection *coll);

  //  const eformat::FullEventFragment<const uint32_t*>  getEvent();

  inline const eformat::FullEventFragment<const uint32_t *> *
  eventFragment() const {
    return m_event;
  }
  inline const eformat::ROBFragment<const uint32_t *> *robFragment() const {
    return m_robFrag;
  }

private:
  const eformat::FullEventFragment<const uint32_t *> *m_event;
  const eformat::ROBFragment<const uint32_t *> *m_robFrag;
  StoreGateSvc *m_EvtStore;
  unsigned int m_fragment_number;
  unsigned int m_count_hits;

  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

  AFP_RawDataReadOut *m_AFP_RawDataReadOut;
  AFP_RawDataCollectionReadOut *m_AFP_RawDataCollectionReadOut;
  AFP_RawDataContainerReadOut *m_AFP_RawDataContainerReadOut;
};

#endif //> !DECODER_AFP_DECODER_H
