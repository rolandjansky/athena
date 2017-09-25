/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECODER_AFP_RAWDATAPROVIDERTOOL_H
#define DECODER_AFP_RAWDATAPROVIDERTOOL_H

#include "AFP_ByteStream2RawCnv.h"
#include "AFP_RawEv/AFP_RawData.h"
#include "AFP_RawEv/AFP_RawDataCollection.h"
#include "AFP_RawEv/AFP_RawDataContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "DataModel/DataVector.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include <set>
#include <stdint.h>
#include <string>
#include <vector>

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
class AFP_RawData;
class AFP_RawDataCollection;
class AFP_RawDataContainer;

class AFP_RawDataProviderTool : public ::AthAlgTool {
public:
  static const InterfaceID &interfaceID();
  AFP_RawDataProviderTool(const std::string &type, const std::string &name,
                          const IInterface *parent);

  virtual ~AFP_RawDataProviderTool();

  virtual StatusCode initialize();
  //  virtual StatusCode  execute();
  virtual StatusCode finalize();

  StatusCode convert(std::vector<const ROBFragment *> &vecRobs,
                     AFP_RawDataContainer *rdoCont);

private:
  ToolHandle<AFP_ByteStream2RawCnv> m_decoder;
  std::set<uint32_t> m_robIdSet;

};

#endif //> !DECODER_AFP_RAWDATAPROVIDERTOOL_H
