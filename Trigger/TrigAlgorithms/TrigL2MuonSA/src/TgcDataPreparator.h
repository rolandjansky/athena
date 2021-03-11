/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCDATAPREPARATOR_H
#define  TRIGL2MUONSA_TGCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigT1Interfaces/RecMuonRoI.h"
#include "xAODTrigger/MuonRoI.h"
#include "TgcDataPreparatorOptions.h"
#include "TgcData.h"
#include "RecMuonRoIUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace MuonGM {
  class TgcReadoutElement;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {

class TgcDataPreparator: public AthAlgTool
{
 public:
  struct tgcRawData
  {
    unsigned short int bcTag;
    unsigned short int sswId;
    unsigned short int slbId;
    unsigned short int bitpos;
  };

   public:

      TgcDataPreparator(const std::string& type, 
			const std::string& name,
			const IInterface*  parent);
    
      virtual StatusCode initialize() override;
    
      StatusCode prepareData(const LVL1::RecMuonRoI*  p_roi,
			     TrigL2MuonSA::TgcHits&   tgcHits) const;

      StatusCode prepareData(const xAOD::MuonRoI*     p_roi,
			     TrigL2MuonSA::TgcHits&   tgcHits) const;

      void setOptions(const TrigL2MuonSA::TgcDataPreparatorOptions& options) { m_options = options; };

      void setRoIBasedDataAccess(bool use_RoIBasedDataAccess){ m_use_RoIBasedDataAccess = use_RoIBasedDataAccess; };

   private:

      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      // option
      TrigL2MuonSA::TgcDataPreparatorOptions m_options{};

      // utils
      TrigL2MuonSA::RecMuonRoIUtils m_recMuonRoIUtils{};

      SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_tgcContainerKey{
	this, "TGCPrepDataContainer", "TGC_Measurements", "Name of the TGCContainer to read in"};

      bool m_use_RoIBasedDataAccess;

};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_TGCDATAPREPARATOR_H
