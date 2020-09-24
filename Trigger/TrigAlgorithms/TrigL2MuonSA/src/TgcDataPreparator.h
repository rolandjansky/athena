/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCDATAPREPARATOR_H
#define  TRIGL2MUONSA_TGCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "TgcDataPreparatorOptions.h"
#include "TgcData.h"
#include "RecMuonRoIUtils.h"
#include "IRegionSelector/IRegSelTool.h"
#include "MuonTGC_Cabling/MuonTGC_CablingSvc.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
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
			     TrigL2MuonSA::TgcHits&   tgcHits);

      void setOptions(const TrigL2MuonSA::TgcDataPreparatorOptions& options) { m_options = options; };

      void setRoIBasedDataAccess(bool use_RoIBasedDataAccess){ m_use_RoIBasedDataAccess = use_RoIBasedDataAccess; };

   private:

      // internal methods
      StatusCode getTgcRdo(unsigned int robId, float eta, float phi, const TgcRdo*& p_rdo);
      StatusCode decodeTgcRdo(const TgcRdo* p_rdo, double roiEta, double roiPhi, bool isLowPt,
			      TrigL2MuonSA::TgcHits& tgcHits);

   private:

      const MuonGM::TgcReadoutElement* m_tgcReadout;
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      // Cabling (new)
      MuonTGC_CablingSvc* m_tgcCabling;	

      // Tool handles for BS conversion and Rdo to Prep Data conversion
      ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataProviderTool{
        this, "TgcRawDataProvider", "Muon::TGC_RawDataProviderTool/TGC_RawDataProviderTool"};

      // Tool for Rdo to Prep Data conversion
      ToolHandle<Muon::IMuonRdoToPrepDataTool> m_tgcPrepDataProvider{
        this, "TgcPrepDataProvider", "Muon::TgcRdoToPrepDataTool/TgcPrepDataProviderTool"};

      // Region Selector
      ToolHandle<IRegSelTool> m_regionSelector;

      // ROB DataProvider
      ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

      // option
      TrigL2MuonSA::TgcDataPreparatorOptions m_options{};

      // utils
      TrigL2MuonSA::RecMuonRoIUtils m_recMuonRoIUtils{};

      SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_tgcContainerKey{
	this, "TGCPrepDataContainer", "TGC_Measurements", "Name of the TGCContainer to read in"};

      bool m_use_RoIBasedDataAccess;

      // Flag to decide if we need to run the actual decoding (in MT setup, we can use offline code for this)
      Gaudi::Property<bool> m_doDecoding{ this, "DoDecoding", true, "Flag to decide if we need to do decoding of the MDTs" };

      // Flag to decide whether or not to run BS decoding
      Gaudi::Property< bool > m_decodeBS { this, "DecodeBS", true, "Flag to decide whether or not to run BS->RDO decoding" };

};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_TGCDATAPREPARATOR_H
