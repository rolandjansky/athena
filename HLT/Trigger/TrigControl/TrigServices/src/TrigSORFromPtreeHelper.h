/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSORFROMPTREEHELPER_H_
#define TRIGSORFROMPTREEHELPER_H_

#include "AthenaBaseComps/AthMessaging.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include <boost/property_tree/ptree.hpp>

class CondAttrListCollection;
class IMessageSvc;

/**
 * @brief Helper class for Start Of Run (SOR) record
 * @author Ricardo Abreu
 *
 * Helper class to retrieve the Start Of Run parameters from the
 * prepareForRun ptree and put them into the detector store with whole-run
 * validity.
 */
class TrigSORFromPtreeHelper : public AthMessaging {
public:
  typedef CondAttrListCollection SOR;

  /**
   * Create the SOR helper
   * @params msgSvc    Pointer to MessageSvc
   * @params detStore  Handle to DetectorStore
   * @params sorpath   COOL folder path of SOR record (e.g. /TDAQ/RunCtrl/SOR_Params)
   * @params rparams   RunParams record
   */
  TrigSORFromPtreeHelper(IMessageSvc* msgSvc, const ServiceHandle<StoreGateSvc>& detStore,
                         const std::string& sorpath, const boost::property_tree::ptree& rparams);

  /**
   * Overwrite run number from RunParams
   * @params run  New run number
   */
  void setRunNumber(unsigned int run) { m_runNumber = run; }

  /**
   * Overwrite SOR time from RunParams
   * @params sorTime_ns  New start of run time in nanoseconds since epoch
   */
  void setSORtime_ns(unsigned long long sorTime_ns) { m_sorTime_ns = sorTime_ns; }

  /**
   * Fill SOR record in Detector Store, reusing if present or creating new one otherwise/
   * @params ctx  EventContext to be used for IOVDbSvc
   */
  StatusCode fillSOR(const EventContext& ctx) const;

  /**
   * Create an EventIDBase filled with the value from rparams
   */
  EventIDBase eventID() const;

private:
  StatusCode createSOR ATLAS_NOT_THREAD_SAFE () const;
  coral::AttributeList getAttrList ATLAS_NOT_THREAD_SAFE () const;
  StatusCode setIOVRange(IOVRange& iovRange) const;
  StatusCode updateProxy(SOR* sor) const;

  ServiceHandle<StoreGateSvc> m_detStore;
  std::string m_sorpath;
  const boost::property_tree::ptree& m_rparams;
  unsigned int m_runNumber{0};
  unsigned long long m_sorTime_ns{0};
};

#endif /* TRIGSORFROMPTREEHELPER_H_ */
