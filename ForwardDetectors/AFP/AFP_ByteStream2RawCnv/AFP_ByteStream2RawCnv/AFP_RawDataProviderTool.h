/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECODER_AFP_RAWDATAPROVIDERTOOL_H
#define DECODER_AFP_RAWDATAPROVIDERTOOL_H

#include "AFP_ByteStream2RawCnv.h"
#include "AFP_RawEv/AFP_RawContainer.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!

#include "eformat/ROBFragment.h"

#include <set>
#include <stdint.h>
#include <string>
#include <vector>

class AFP_RawDataProviderTool : public ::AthAlgTool {
public:
  static const InterfaceID &interfaceID();
  AFP_RawDataProviderTool(const std::string &type, const std::string &name,
                          const IInterface *parent);

  /// Does nothing
  virtual ~AFP_RawDataProviderTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /// Fill rawContainer with collections created from provided vecRobs
  StatusCode convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *> &vecRobs,
                     AFP_RawContainer *rawContainer);

private:
  ToolHandle<AFP_ByteStream2RawCnv> m_decoder;
  std::set<uint32_t> m_robIdSet;

};

#endif //> !DECODER_AFP_RAWDATAPROVIDERTOOL_H
