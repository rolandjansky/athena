/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_BYTESTREAM2RAWCNV_H
#define AFP_BYTESTREAM2RAWCNV_H

#include <stdint.h>

#include <string>
#include <set>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"

#include "AFP_RawEv/AFP_RawContainer.h"
#include "AFP_RawEv/AFP_RawDataCommonHead.h"

#include "AFP_WordReadOut.h"

class AFP_ByteStream2RawCnv : public ::AthAlgTool {
public:
  AFP_ByteStream2RawCnv(const std::string &type, const std::string &name,
                        const IInterface *parent);

  static const InterfaceID &interfaceID();

  /// Does nothing
  virtual ~AFP_ByteStream2RawCnv();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /// Fills rawContainer with collections from ROBFragment
  StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *robFrag, AFP_RawContainer *rawContainer);

  /// returns true if provided link corresponds to time-of-flight information
  bool isLinkToF (const unsigned int link) const {return s_linksToF.find(link) != s_linksToF.end();}

  /// returns true if provided link corresponds to silicon detector information
  bool isLinkSi (const unsigned int link) const {return s_linksSi.find(link) != s_linksSi.end();}

  /// @brief Adds new silicon collection to AFP_RawContainer
  ///
  /// It is checked if a silicon collection with given link and robId
  /// is already in the container. If it is a warning message is
  /// issued and nullptr is returned. If it is not, then it is created
  /// and pointer to the new collection is returned.
  AFP_SiRawCollection* getCollectionSi(const unsigned int link, const unsigned int robId, AFP_RawContainer *container);

  /// @brief Adds new time-of-flight collection to AFP_RawContainer
  ///
  /// It is checked if a time-of-flight collection with given link and
  /// robId is already in the container. If it is a warning message is
  /// issued and nullptr is returned. If it is not, then it is created
  /// and pointer to the new collection is returned.
  AFP_ToFRawCollection* getCollectionToF(const unsigned int link, const unsigned int robId, AFP_RawContainer *container);

  /// Sets data header information for given argument based on #m_wordReadout
  void setDataHeader (AFP_RawDataCommonHead* dataHead) const;
  
private:
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

  AFP_WordReadOut m_wordReadout;
  
  static const uint32_t s_siNoHitMarker = 15;
  static const std::set<unsigned int> s_linksToF;
  static const std::set<unsigned int> s_linksSi;
};

#endif //> !DECODER_AFP_DECODER_H
