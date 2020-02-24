/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBYTESTREAMCNVSVC_H
#define TRIGBYTESTREAMCNVSVC_H

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include <TH1I.h>
#include <TH1F.h>
#include <TH2F.h>

// Forward declarations
class StoreGateSvc;
class IROBDataProviderSvc;
class ITHistSvc;

/** @class TrigByteStreamCnvSvc
 *  @brief A ByteStreamCnvSvc implementation for online use
 *
 *  It overrides the connectOutput and commitOutput methods of the base class. In this implementation, they create
 *  the specific online HLT output and send it out directly to the TDAQ infrastructure without using an output service.
 *
 *  It needs to inherit from ByteStreamCnvSvc rather than ByteStreamCnvSvcBase, because some elements of the athena
 *  framework (particularly converters) rely on the properties or functionality of the offline ByteStreamCnvSvc.
 **/
class TrigByteStreamCnvSvc : public ByteStreamCnvSvc {
public:
  /// Standard constructor
  TrigByteStreamCnvSvc(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~TrigByteStreamCnvSvc();

  // ------------------------- IService methods --------------------------------
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;
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
  // ------------------------- Helper methods ---------------------------
  /// Print contents of m_rawEventWrite
  void printRawEvent();
  /// Fill histograms from contents of a FullEventFragment
  void monitorRawEvent(const std::unique_ptr<uint32_t[]>& rawEventPtr) const;
  /// Register monitoring histograms with THistSvc
  void bookHistograms();

  // ------------------------- Service handles ---------------------------
  ServiceHandle<StoreGateSvc> m_evtStore; //< Event store
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc; //< ROB Data Provider
  ServiceHandle<ITHistSvc> m_THistSvc; //< Histogramming service

  // ------------------------- Monitoring histograms ---------------------------
  TH1I* m_histOnlineErrorCode{nullptr}; //< Histogram of online error codes
  TH1F* m_histStreamTags{nullptr}; //< Histogram of stream tags
  TH2F* m_histStreamTagsCorr{nullptr}; //< Histogram of stream tags correlation
  TH1F* m_histStreamTagsNum{nullptr}; //< Histogram of number of stream tags
  TH1F* m_histStreamTagsType{nullptr}; //< Histogram of stream tags type
  TH1F* m_histPebRobsNum{nullptr}; //< Histogram of the number of ROBs in PEB stream tag
  TH1F* m_histPebSubDetsNum{nullptr}; //< Histogram of the number of SubDetectors in PEB stream tag
  TH1F* m_histPebSubDetsFromRobList{nullptr}; //< Histogram of SubDetectors from ROB list in PEB stream tag
  TH1F* m_histPebSubDetsFromSubDetList{nullptr}; //< Histogram of SubDetectors from SubDetector list in PEB stream tag
  TH2F* m_histResultSizeByModule{nullptr}; //< Histogram of HLT result size per module ID
  TH2F* m_histResultSizeByStream{nullptr}; //< Histogram of HLT result size per stream
  TH1F* m_histResultSizeTotal{nullptr}; //< Histogram of total HLT result size (all modules)
  TH1F* m_histResultSizeFullEvFrag{nullptr}; //< Histogram of the size of FullEventFragment sent from HLT to DataCollector
  TH1F* m_histEventDoneTime{nullptr}; //< Histogram of the time taken by the eventDone call
};

#endif // TRIGBYTESTREAMCNVSVC_H
