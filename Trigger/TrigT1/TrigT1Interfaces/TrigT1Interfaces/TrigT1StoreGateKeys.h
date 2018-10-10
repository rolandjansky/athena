// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_TRIGT1STOREGATEKEYS_H
#define TRIGT1INTERFACES_TRIGT1STOREGATEKEYS_H

// STL include(s):
#include <string>

/**
 * @short Namespace of the LVL1 MuCTPI simulation
 *
 * $Revision: 621285 $
 * $Date: 2014-10-11 16:51:48 +0200 (Sat, 11 Oct 2014) $
 */
namespace LVL1MUCTPI {

  //! default StoreGate location for MuCTPI to CTP input
  static const std::string DEFAULT_MuonCTPLocation   = "/Run/L1MuCTPItoCTPLocation";
  //! default StoreGate location for MuCTPI to RoIB input
  static const std::string DEFAULT_MuonRoIBLocation  = "/Run/L1MuCTPItoRoIBLocation";

} // namespace LVL1MUCTPI

namespace LVL1 {

  //! default StoreGate location for MBTS_A to CTP input
  static const std::string DEFAULT_MbtsACTPLocation = "/Run/L1MBTSAtoCTPLocation";
  //! default StoreGate location for MBTS to CTP input
  static const std::string DEFAULT_MbtsCCTPLocation = "/Run/L1MBTSCtoCTPLocation";
  //! default StoreGate location for BCM to CTP input
  static const std::string DEFAULT_BcmCTPLocation = "/Run/L1BCMtoCTPLocation";
  //! default StoreGate location for Lucid to CTP input
  static const std::string DEFAULT_LucidCTPLocation = "/Run/L1LucidtoCTPLocation";
  //! default StoreGate location for ZDC to CTP input
  static const std::string DEFAULT_ZdcCTPLocation = "/Run/L1ZDCtoCTPLocation";
  //! default StoreGate location for TRT to CTP input
  static const std::string DEFAULT_TrtCTPLocation = "/Run/L1TRTtoCTPLocation";
  //! default StoreGate location for BPTX to CTP input
    static const std::string DEFAULT_BptxCTPLocation = "/Run/L1BPTXtoCTPLocation";
  //! default StoreGate location for NIM to CTP input
  static const std::string DEFAULT_NimCTPLocation = "/Run/L1NIMtoCTPLocation";
  //! XXX default StoreGate location for Front Panel CTP inputs
  //static const std::string DEFAULT_FrontPanelCTPLocation = "/Run/FrontPanelCTPLocation";
  //! default StoreGate location for Front Panel CTP inputs
  static const std::string DEFAULT_L1TopoCTPLocation = "/Run/L1TopoToCTPLocation";
  //! default StoreGate location for Front Panel CTP inputs of l1topo overflow bits
  static const std::string DEFAULT_L1TopoOverflowCTPLocation = "/Run/L1TopoOverflowToCTPLocation";

} // namespace LVL1

namespace LVL1CTP {

  //! default StoreGate location for CTPSLink
  static const std::string DEFAULT_CTPSLinkLocation = "/Event/CTPSLinkLocation";  
  //! default StoreGate location for CTPSLink for simulation rerun on data
  static const std::string DEFAULT_CTPSLinkLocation_Rerun = "/Event/CTPSLinkLocation_Rerun";
  //! location of CTP RDO output in StoreGate
  static const std::string DEFAULT_RDOOutputLocation = "CTP_RDO";
  //! location of CTP RDO output in StoreGate for simulation rerun on data
  static const std::string DEFAULT_RDOOutputLocation_Rerun = "CTP_RDO_Rerun";

} // namespace LVL1CTP

/**
 * @short Namespace of the LVL1 RoIB simulation
 *
 * $Revision: 621285 $
 * $Date: 2014-10-11 16:51:48 +0200 (Sat, 11 Oct 2014) $
 */
namespace ROIB {

  //! location of RoIBuilder output in StoreGate
  static const std::string DEFAULT_RoIBRDOLocation = "RoIBResult";

} // namespace ROIB

#endif // TRIGT1INTERFACES_TRIGT1STOREGATEKEYS_H
