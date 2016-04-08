// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RoIBResultToxAOD.h 679933 2015-07-02 21:37:29Z watsona $
#ifndef ANALYSISTRIGGERALGS_ROIBRESULTTOXAOD_H
#define ANALYSISTRIGGERALGS_ROIBRESULTTOXAOD_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

// xAOD include(s):
#include "xAODTriggerCnv/IMuonRoICnvTool.h"

// Forward declaration(s):
class StoreGateSvc;
class LVL1_ROI;
namespace TrigConf {
  class ILVL1ConfigSvc;
}
namespace LVL1 {
  class RecMuonRoiSvc;
  class IL1CPMTools;
  class IL1JEMJetTools;
}
namespace ROIB {
  class RoIBResult;
}

/**
 *  @short RoIB result to xAOD converter
 *
 *  This is a slightly adapted version of the original RoIBResultToAOD 
 *  algorithm. Going the route through the LVL1_ROI structure was 
 *  needed due to the interface of the AOD->xAOD ROI converters
 *
 *  The RoIBResultToxAOD algorithm builds the CTP_Decision and 
 *  LVL1_ROI objects from the LVL1 ROIB::RoIBResult object. 
 *  In addition the TriggerType of the CTP can be rebuild,
 *  when zero in the input object.
 *
 *  The CTP_Decision and LVL1_ROI objects are stored in ESD/AOD.
 *
 *    $Id: RoIBResultToxAOD.h 679933 2015-07-02 21:37:29Z watsona $
 *
 * @author Tadashi Maeno <Tadashi.Maeno@cern.ch>
 * @author Attila Kraznahorkay Jr. <Attila.Krasznahorkay@cern.ch>
 * @author Alan Watson <Alan.Watson@cern.ch>
 * @author Wolfgang Ehrenfeld <Wolfgang.Menges@desy.de>
 * @author last commit \$Author: watsona $
 *
 */
class RoIBResultToxAOD : public AthAlgorithm {

public:
   RoIBResultToxAOD( const std::string& name, ISvcLocator* pSvcLocator );

   StatusCode initialize();
   StatusCode finalize();
   StatusCode execute();

private:
   /// build CTP_Decision from CTPResult stored in ROIB::RoIBResult
   StatusCode buildCTP_Decision();
   /// add EmTauRoi to LVL1_ROI from EmTauResult stored in ROIB::RoIBResult
   StatusCode addEmTauRoI(const ROIB::RoIBResult*);
   /// add JetEnergyRoi to LVL1_ROI from JetEnergyResult stored in ROIB::RoIBResult
   StatusCode addJetEnergyRoI(const ROIB::RoIBResult*);
   /// add MuonRoi to LVL1_ROI from MuCTPIResult stored in ROIB::RoIBResult
   void addMuonRoI(const ROIB::RoIBResult*, LVL1_ROI*);
   /// convert AOD ROI objects to xAOD objects
   StatusCode convertToxAOD(LVL1_ROI*);

   // services
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;               //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink

   ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc;                 //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink
   ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc;                 //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink

   // tools
   ToolHandle<LVL1::IL1CPMTools> m_EmTauTool;                          //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink
   ToolHandle<LVL1::IL1JEMJetTools> m_JetTool;                           //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink
   bool m_retrievedEmTauTool;
   bool m_retrievedJetTool;

   // xAOD conversion tools:
   std::string m_xaodKeyMu;
   std::string m_xaodKeyEmTau;
   std::string m_xaodKeyEsum;
   std::string m_xaodKeyJetEt;
   std::string m_xaodKeyJet;

   ToolHandle< xAODMaker::IMuonRoICnvTool > m_MuCnvTool;

   // Data object StoreGate keys
   std::string m_CPMTowerLocation;
   std::string m_JetElementLocation;
   std::string m_roibInputKey;
   std::string m_lvl1RoIOutputKey;

   // properties
   bool        m_doCalo;                                              //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink
   bool        m_doMuon;                                              //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink

   // trigger items for each TTW bit
   std::vector< std::string > m_egammaItem;                           //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink
   std::vector< std::string > m_tauhadItem;                           //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink
   std::vector< std::string > m_jetItem;                              //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink
   std::vector< std::string > m_esumItem;                             //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink
   std::vector< std::string > m_highmuItem;                           //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink
   std::vector< std::string > m_lowmuItem;                            //!< property, see @link RoIBResultToxAOD::RoIBResultToxAOD @endlink

}; // class RoIBResultToxAOD

#endif // ANALYSISTRIGGERALGS_ROIBRESULTTOXAOD_H
