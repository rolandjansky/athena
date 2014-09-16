/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCDATAPREPARATOR_H
#define  TRIGL2MUONSA_TGCDATAPREPARATOR_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "TGCgeometry/TGCgeometrySvc.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "TrigL2MuonSA/TgcDataPreparatorOptions.h"
#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"

class StoreGateSvc;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

namespace TrigL2MuonSA {

class TgcDataPreparator: public AlgTool
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
    
      StatusCode prepareData(const LVL1::RecMuonRoI* p_roi, TrigL2MuonSA::TgcHits& tgcHits);

      inline MSG::Level msgLvl() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }
      inline void setMsgLvl(const MSG::Level& level) { if(m_msg != 0) m_msg->setLevel(level); }
      void setOptions(const TrigL2MuonSA::TgcDataPreparatorOptions& options) { m_options = options; };

   private:

      MsgStream* m_msg;
      inline MsgStream& msg() const { return *m_msg; }

      // internal methods
      StatusCode getTgcRdo(unsigned int robId, float eta, float phi, const TgcRdo*& p_rdo);
      StatusCode decodeTgcRdo(const TgcRdo* p_rdo, double roiEta, double roiPhi, bool isLowPt,
			      TrigL2MuonSA::TgcHits& tgcHits);

   private:

      // Reference to StoreGateSvc;
      ServiceHandle<StoreGateSvc>    m_storeGateSvc;

      // Tools for the Raw data conversion
      ToolHandle<Muon::IMuonRawDataProviderTool>  m_tgcRawDataProvider;

      // TGC raw data
      std::vector<tgcRawData> m_tgcRawData;

      // L2-specific Geometry Svc
      ServiceHandle<TGCgeometrySvc>  m_tgcGeometrySvc;

      // ROB DataProvider
      ROBDataProviderSvc*  m_robDataProvider;

      // option
      TrigL2MuonSA::TgcDataPreparatorOptions m_options;

      // utils
      TrigL2MuonSA::RecMuonRoIUtils m_recMuonRoIUtils;
};

} // namespace TrigL2MuonSA

#endif  // TRIGL2MUONSA_TGCDATAPREPARATOR_H
