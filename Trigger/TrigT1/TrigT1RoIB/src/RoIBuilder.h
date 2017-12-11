// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1ROIB_ROIBUILDER_H
#define TRIGT1ROIB_ROIBUILDER_H

#include <string>


#include "xAODEventInfo/EventInfo.h"
#include "TrigT1Interfaces/CTPSLink.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/MuCTPIToRoIBSLink.h"
#include "TrigT1Interfaces/SlinkWord.h"
#include "TrigT1Result/RoIBResult.h"

#include "AthenaBaseComps/AthAlgorithm.h"

//! namespace for RoIBuilder related classes
namespace ROIB {

   /**
    *  @short RoIBuilder simulation algorithm
    *
    *         The RoIBuilder algorithm concatenates the different L1 RODs from
    *         calo, muon and CTP and fills the L1 RDO.
    *
    * @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>,
    * @author Attila Kraznahorkay Jr. <Attila.Krasznahorkay@cern.ch>
    * @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
    *
    * $Revision: 500598 $
    * $Date: 2012-05-14 17:39:58 +0200 (Mon, 14 May 2012) $
    */
   class RoIBuilder : public AthAlgorithm {

   public:
      RoIBuilder( const std::string& name, ISvcLocator* pSvcLocator ) ;

      virtual StatusCode initialize();
      virtual StatusCode execute();
      virtual StatusCode finalize();

   private:
     Gaudi::Property<bool> m_doCalo{ this, "DoCalo", true, "Use inputs from Calo system" }; 
     Gaudi::Property<bool> m_doMuon{ this, "DoMuon", true, "Use inputs from Muon system" }; 

      // String members containing locations of objects in SG:
     SG::ReadHandleKey<LVL1CTP::CTPSLink> m_ctpSLinkLocation{ this, "CTPSLinkLocation", 
	 LVL1CTP::DEFAULT_CTPSLinkLocation, "StoreGate location of CTP RoI"};

     typedef DataVector< LVL1CTP::SlinkWord> SlinkWordDV;

     SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{ this, "EventInfoKey", "EventInfo", "Event info object "};

     SG::ReadHandleKeyArray< SlinkWordDV > m_caloEMTauLocation{ this,   "CaloEMTauLocation", 
	 { LVL1::TrigT1CaloDefs::EmTauSlinkLocation+"0", LVL1::TrigT1CaloDefs::EmTauSlinkLocation+"1", LVL1::TrigT1CaloDefs::EmTauSlinkLocation+"2", LVL1::TrigT1CaloDefs::EmTauSlinkLocation+"3"  }, 
	 "StoreGate location of EmTau inputs" };
     SG::ReadHandleKeyArray< SlinkWordDV > m_caloJetEnergyLocation{ this, "CaloJetEnergyLocation", 
	 { LVL1::TrigT1CaloDefs::jepSlinkLocation+"0", LVL1::TrigT1CaloDefs::jepSlinkLocation+"0" },
	 "StoreGate location of JetEnergy inputs" };

     SG::ReadHandleKey<L1MUINT::MuCTPIToRoIBSLink> m_muctpiSLinkLocation{ this, "MuCTPISLinkLocation",
									  LVL1MUCTPI::DEFAULT_MuonRoIBLocation,
									  "StoreGate location of MuCTPI inputs" };
     
     SG::WriteHandleKey<RoIBResult> m_roibRDOLocation{ this, "RoIBRDOLocation",  
	 ROIB::DEFAULT_RoIBRDOLocation,
	 "StoreGate location of RoIB RDO" };

   }; // class RoIBuilder

} // namespace ROIB

#endif // TRIGT1ROIB_ROIBUILDER_H
