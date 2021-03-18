// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTRIGGERALGS_ROIBRESULTTOAOD_H
#define ANALYSISTRIGGERALGS_ROIBRESULTTOAOD_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "TrigT1CaloToolInterfaces/IL1JetTools.h"
#include "TrigT1CaloToolInterfaces/IL1EmTauTools.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

// Forward declaration(s):
class LVL1_ROI;
namespace ROIB {
  class RoIBResult;
}

/**
 *  @short RoIB result to AOD converter
 * 
 *  The RoIBResultToAOD algorithm builds the CTP_Decision and 
 *  LVL1_ROI objects from the LVL1 ROIB::RoIBResult object. 
 *  In addition the TriggerType of the CTP can be rebuild,
 *  when zero in the input object.
 *
 *  The CTP_Decision and LVL1_ROI objects are stored in ESD/AOD.
 *
 *    $Id: RoIBResultToAOD.h 452138 2011-08-04 11:10:41Z krasznaa $        
 *
 * @author Tadashi Maeno <Tadashi.Maeno@cern.ch>
 * @author Attila Kraznahorkay Jr. <Attila.Krasznahorkay@cern.ch>
 * @author Alan Watson <Alan.Watson@cern.ch>
 * @author Wolfgang Ehrenfeld <Wolfgang.Menges@desy.de>
 * @author last commit \$Author: watsona $
 *
 */

class RoIBResultToAOD : public AthAlgorithm {

public:
   RoIBResultToAOD( const std::string& name, ISvcLocator* pSvcLocator );

   virtual StatusCode initialize() override;
   virtual StatusCode execute() override;

private:
   /// build CTP_Decision from CTPResult stored in ROIB::RoIBResult
   StatusCode buildCTP_Decision();
   /// add EmTauRoi to LVL1_ROI from EmTauResult stored in ROIB::RoIBResult
   void addEmTauRoI(const ROIB::RoIBResult*, LVL1_ROI*);
   /// add JetEnergyRoi to LVL1_ROI from JetEnergyResult stored in ROIB::RoIBResult
   void addJetEnergyRoI(const ROIB::RoIBResult*, LVL1_ROI*);
   /// add MuonRoi to LVL1_ROI from MuCTPIResult stored in ROIB::RoIBResult
   void addMuonRoI(const ROIB::RoIBResult*, LVL1_ROI*);

   // services
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;               //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink

   // tools
   ServiceHandle<LVL1::ITrigT1MuonRecRoiTool> m_recRPCRoiTool;        //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink
   ServiceHandle<LVL1::ITrigT1MuonRecRoiTool> m_recTGCRoiTool;        //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink

   ToolHandle<LVL1::IL1EmTauTools> m_EmTauTool;                       //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink
   ToolHandle<LVL1::IL1JetTools> m_JetTool;                           //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink
   bool m_retrievedEmTauTool;
   bool m_retrievedJetTool;

   // Data object StoreGate keys
   std::string m_TriggerTowerLocation;
   std::string m_JetElementLocation;
   std::string m_roibInputKey;
   std::string m_lvl1RoIOutputKey;
  
   // properties
   bool        m_doCalo;                                              //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink
   bool        m_doMuon;                                              //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink

   // trigger items for each TTW bit
   std::vector< std::string > m_egammaItem;                           //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink
   std::vector< std::string > m_tauhadItem;                           //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink
   std::vector< std::string > m_jetItem;                              //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink
   std::vector< std::string > m_esumItem;                             //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink
   std::vector< std::string > m_highmuItem;                           //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink
   std::vector< std::string > m_lowmuItem;                            //!< property, see @link RoIBResultToAOD::RoIBResultToAOD @endlink

}; // class RoIBResultToAOD

#endif // ANALYSISTRIGGERALGS_ROIBRESULTTOAOD_H
