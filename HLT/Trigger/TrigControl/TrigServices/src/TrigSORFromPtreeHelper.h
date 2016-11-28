/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigSORFromPtreeHelper.h
 * @author Ricardo Abreu
 *
 * @brief Helper class to retrieve the Start Of Run parameters from the
 * prepareForRun ptree and put them into the detector store with whole-run
 * validity. This class replaces it's old analogous TrigSORFromISHelper.
 */

#ifndef TRIGSORFROMPTREEHELPER_H_
#define TRIGSORFROMPTREEHELPER_H_

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"
#include <boost/property_tree/ptree.hpp>
#include <utility>

class CondAttrListCollection;

////////////////////////////////////////////////////////////////////////////////
class TrigSORFromPtreeHelper
{
public:
  typedef CondAttrListCollection SOR;

  explicit TrigSORFromPtreeHelper(const MsgStream & log);

  /*
   * Fill SOR record in Detector Store, reusing if present or creating new one
   * otherwise. SOR contents filled according to what is specified by rparams.
   * Validity of SOR set to this run
   */
  const SOR * fillSOR(const boost::property_tree::ptree & rparams) const;

private:
  typedef ServiceHandle<StoreGateSvc> SG;
  typedef boost::property_tree::ptree PT;

  SOR * getSOR(const SG & dstore) const;
  coral::AttributeList getAttrList(const PT & rparams) const;
  std::pair<uint64_t, uint64_t> getDetMask(const PT & rparams) const;

  StatusCode fillSor(const PT & rparams, SOR * sor) const;
  StatusCode setIOVRange(IOVRange & iovRange) const;
  StatusCode updateProxy(const SG & dstore, SOR * sor) const;

  mutable MsgStream m_log;
};

////////////////////////////////////////////////////////////////////////////////
inline TrigSORFromPtreeHelper::
TrigSORFromPtreeHelper(const MsgStream & log)
  : m_log(log)
{
}


#endif /* TRIGSORFROMPTREEHELPER_H_ */
