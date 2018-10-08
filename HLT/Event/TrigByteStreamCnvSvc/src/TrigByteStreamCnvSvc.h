/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBYTESTREAMCNVSVC_H
#define TRIGBYTESTREAMCNVSVC_H

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"

// Forward declarations
class StoreGateSvc;
class IROBDataProviderSvc;

/** @class TrigByteStreamCnvSvc
 *  @brief A ByteStreamCnvSvc implementation for online use
 **/
class TrigByteStreamCnvSvc : public ByteStreamCnvSvcBase {
public:
  /// Standard constructor
  TrigByteStreamCnvSvc(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~TrigByteStreamCnvSvc();

  // ------------------------- IService methods --------------------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // ------------------------- IConversionSvc methods --------------------------
  /// In the case of online BS data, this method creates the output FullEventFragment and fills its header
  using ByteStreamCnvSvcBase::connectOutput;
  virtual StatusCode connectOutput(const std::string& outputFile) override;
  /// This overload is kept only for interface compatibility
  virtual StatusCode connectOutput(const std::string& outputFile, const std::string& openMode) override;
  /// In the case of online BS data, this method binds and sends out the output FullEventFragment
  virtual StatusCode commitOutput(const std::string& outputFile, bool do_commit) override;

private:
  /// Helper method printing contents of m_rawEventWrite
  void printRawEvent();
  /// Handle to the event store
  ServiceHandle<StoreGateSvc> m_evtStore;
  /// Handle to the ROB Data Provider
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc;
};

#endif // TRIGBYTESTREAMCNVSVC_H
