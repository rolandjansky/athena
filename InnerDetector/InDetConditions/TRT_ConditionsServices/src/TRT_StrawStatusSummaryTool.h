
//Dear emacs this is -*-c++-*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_STRAWSTATUSSUMMARYTOOL_H
#define TRT_STRAWSTATUSSUMMARYTOOL_H
/** @file TRT_StrawStatusSummaryTool.h
 * @brief interface to TRT straw status constants used in normal reconstruction
 * @author Peter Hansen <phansen@nbi.dk>
 */
//STL
#include <vector>
//Gaudi
#include "GaudiKernel/ICondSvc.h"
//StoreGate
#include "StoreGate/ReadCondHandleKey.h"
//Athena
#include "AthenaBaseComps/AthAlgTool.h"
//TRT
#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h" 
//#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "InDetIdentifier/TRT_ID.h"
//#include "TRT_ReadoutGeometry/TRT_BaseElement.h"

#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"

#include "CxxUtils/checker_macros.h"

class ATLAS_NOT_THREAD_SAFE TRT_StrawStatusSummaryTool: // This class uses thread-unsafe DataHandle (m_strawstatusHTG4).
// If bare pointer is used, GeoModelSvc.TRT_DetectorTool.TRT_StrawStatusSummaryTool
// cannot retrieve folder 'SimStatusHTKey':/TRT/Cond/StatusHT
  public extends<AthAlgTool, ITRT_StrawStatusSummaryTool>
{  
 public:
  typedef TRTCond::StrawStatusMultChanContainer StrawStatusContainer;

  /// constructor
  TRT_StrawStatusSummaryTool ( const std::string& type, const std::string& name, const IInterface* parent);
  
  /// destructor
  virtual ~TRT_StrawStatusSummaryTool() = default;
  
  /// tool initialize
  virtual StatusCode initialize() override;

  /// tool finalize
  virtual StatusCode finalize() override;


 /// access to the status
  virtual int getStatus(Identifier offlineId) const override {
    return getStatus(offlineId,Gaudi::Hive::currentContext());
  }

  virtual int getStatusPermanent(Identifier offlineId) const override {
    return getStatusPermanent(offlineId,Gaudi::Hive::currentContext());
  }

  virtual int getStatusHT(Identifier offlineId) const override {
    return getStatusHT(offlineId,Gaudi::Hive::currentContext());
  }

  virtual bool get_status(Identifier offlineId) const override {
    return get_status(offlineId,Gaudi::Hive::currentContext());
  }

  virtual bool get_statusHT(Identifier offlineId) const override {
    return get_statusHT(offlineId,Gaudi::Hive::currentContext());
  }
  virtual const StrawStatusContainer* getStrawStatusHTContainer() const override;

  virtual int getStatus(Identifier offlineId,const EventContext& ) const override;
  virtual int getStatusPermanent(Identifier offlineId, const EventContext&) const override;
  virtual int getStatusHT(Identifier offlineId, const EventContext&) const override;
  virtual bool get_status(Identifier offlineId, const EventContext&) const override;
  virtual bool get_statusHT(Identifier offlineId, const EventContext&) const override;


 private:

  const TRT_ID* m_trtId=nullptr;

  ServiceHandle<ICondSvc> m_condSvc;
  //  ReadHandle  keys
  SG::ReadCondHandleKey<StrawStatusContainer> m_statReadKey{this,"StatReadKeyName","/TRT/Cond/Status","StrawStatus in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_permReadKey{this,"PermReadKeyName","/TRT/Cond/StatusPermanent","StrawStatusPermanent in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_statHTReadKey{this,"StatHTReadKeyName","/TRT/Cond/StatusHT","StrawStatusHT in-key"};

  // Used in simulation (GEANT4) jobs
  Gaudi::Property<bool> m_isGEANT4 {this,"isGEANT4",true};
  Gaudi::Property<std::string> m_par_strawstatusHTcontainerkey{this, "SimStatusHTKey","/TRT/Cond/StatusHT"};

  const DataHandle<StrawStatusContainer> m_strawstatusHTG4{nullptr};

};

#endif //  TRT_STRAWSTATUSSUMMARYTOOL_H
