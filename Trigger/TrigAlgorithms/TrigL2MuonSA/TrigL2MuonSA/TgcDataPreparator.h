/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCDATAPREPARATOR_H
#define  TRIGL2MUONSA_TGCDATAPREPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "TrigL2MuonSA/TgcDataPreparatorOptions.h"
#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"
#include "RegionSelector/IRegSelSvc.h"

#include "MuonTGC_Cabling/MuonTGC_CablingSvc.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

class StoreGateSvc;
class TgcIdHelper;

namespace MuonGM {
  class MuonDetectorManager;
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
      
      static const InterfaceID& interfaceID();

   public:

      TgcDataPreparator(const std::string& type, 
			const std::string& name,
			const IInterface*  parent);
    
      ~TgcDataPreparator();
    
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();
    
      StatusCode prepareData(const LVL1::RecMuonRoI*  p_roi,
			     TrigL2MuonSA::TgcHits&   tgcHits);

      void setOptions(const TrigL2MuonSA::TgcDataPreparatorOptions& options) { m_options = options; };

      void setRoIBasedDataAccess(bool use_RoIBasedDataAccess);

      // access the list of hash ID's
      std::vector<IdentifierHash> getHashList() { return m_tgcHashList; }

   private:

      // internal methods
      StatusCode getTgcRdo(unsigned int robId, float eta, float phi, const TgcRdo*& p_rdo);
      StatusCode decodeTgcRdo(const TgcRdo* p_rdo, double roiEta, double roiPhi, bool isLowPt,
			      TrigL2MuonSA::TgcHits& tgcHits);

   private:

      // Reference to StoreGateSvc;
      ServiceHandle<StoreGateSvc>    m_storeGateSvc;

      const MuonGM::MuonDetectorManager* m_muonMgr;
      const MuonGM::TgcReadoutElement* m_tgcReadout;
      const TgcIdHelper* m_tgcIdHelper;

      //ActiveStoreSvc* m_activeStore;
      ServiceHandle<ActiveStoreSvc> m_activeStore;

      // Cabling (new)
      MuonTGC_CablingSvc* m_tgcCabling;	

      // Tool handles for BS conversion and Rdo to Prep Data conversion
      ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataProviderTool;

      // Tool for Rdo to Prep Data conversion
      ToolHandle<Muon::IMuonRdoToPrepDataTool> m_tgcPrepDataProvider;
      //ToolHandle<Muon::IMuonRdoToPrepDataTool> m_tgcPrepDataProvider {
      // 	this, "TgcPrepDataProvider", "Muon::TgcRdoToPrepDataTool/TgcPrepDataProviderTool", ""};
      //  

      // Region Selector
      ServiceHandle<IRegSelSvc> m_regionSelector;

      // ROB DataProvider
      ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

      // option
      TrigL2MuonSA::TgcDataPreparatorOptions m_options;

      // utils
      TrigL2MuonSA::RecMuonRoIUtils m_recMuonRoIUtils;

      SG::ReadHandleKey<Muon::TgcPrepDataContainer> m_tgcContainerKey{
	this, "TGCPrepDataContainer", "TGC_Measurements", "Name of the TGCContainer to read in"};

      bool m_use_RoIBasedDataAccess;

      // Flag to decide whether or not to run BS decoding
      Gaudi::Property< bool > m_decodeBS { this, "DecodeBS", true, "Flag to decide whether or not to run BS->RDO decoding" };

      // vector of the TGC hash ID list
      std::vector<IdentifierHash> m_tgcHashList;         

      // TGC raw data
      std::vector<tgcRawData> m_tgcRawData;

};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_TGCDATAPREPARATOR_H
