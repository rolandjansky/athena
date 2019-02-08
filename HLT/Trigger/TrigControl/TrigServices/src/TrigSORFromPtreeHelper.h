/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
   */
  TrigSORFromPtreeHelper(IMessageSvc* msgSvc, const ServiceHandle<StoreGateSvc>& detStore, const std::string& sorpath);

  /**
   * Fill SOR record in Detector Store, reusing if present or creating new one
   * otherwise. SOR contents filled according to what is specified by rparams.
   * Validity of SOR set to this run
   */
  StatusCode fillSOR(const boost::property_tree::ptree& rparams, const EventContext& ctx) const;

  /**
   * Create an EventIDBase filled with the value from rparams
   */
  EventIDBase eventID(const boost::property_tree::ptree& rparams) const;

private:
  StatusCode createSOR(const boost::property_tree::ptree& rparams) const;
  coral::AttributeList getAttrList(const boost::property_tree::ptree& rparams) const;
  StatusCode setIOVRange(IOVRange& iovRange) const;
  StatusCode updateProxy(SOR* sor) const;

  ServiceHandle<StoreGateSvc> m_detStore;
  std::string m_sorpath;
};

#endif /* TRIGSORFROMPTREEHELPER_H_ */
