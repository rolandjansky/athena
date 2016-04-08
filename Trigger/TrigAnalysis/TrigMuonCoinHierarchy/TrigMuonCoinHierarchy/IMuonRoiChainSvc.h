/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_IMUONROICHAINSVC_H
#define TRIGGER_IMUONROICHAINSVC_H

#include "GaudiKernel/IInterface.h"

#include <vector>

#include "TrigMuonCoinHierarchy/MuonRoiChain.h"

namespace Trigger {
  class IMuonRoiChainSvc : virtual public IInterface
    {
    public:
      /** Destructor */
      virtual ~IMuonRoiChainSvc() {}
      
      /** Provide InterfaceID */
      static const InterfaceID & interfaceID();

      /** Get MuonRoiChain from an index of a Muon_ROI in LVL1_ROI::muons_type */ 
      virtual StatusCode getChainWithRoiIndex(const MuonRoiChain** chain, int roiIndex)=0; 

      /** Get MuonRoiChain from a Muon_ROI */ 
      virtual StatusCode getChain(const MuonRoiChain** chain, const Muon_ROI* muonRoi)=0; 
      /** Get MuonRoiChain from a data word of MuCTPI_RDO */ 
      virtual StatusCode getChain(const MuonRoiChain** chain, uint32_t muctpiRdoDataWord)=0;
      /** Get MuonRoiChain from a TgcCoinData (Sector Logic) */ 
      virtual StatusCode getChain(const MuonRoiChain** chain, const Muon::TgcCoinData* coin)=0; 
      /** Get all MuonRoiChains in an event */
      virtual StatusCode getChains(std::vector<const MuonRoiChain*>* chains)=0;

    protected:
      /** Make MuonRoiChains using IMuonRoiChainFindTool */
      virtual StatusCode execute()=0; 
    };
  
  inline const InterfaceID & IMuonRoiChainSvc::interfaceID() {
    static const InterfaceID IID_IMuonRoiChainSvc("Trigger::IMuonRoiChainSvc", 1, 0);
    return IID_IMuonRoiChainSvc;
  }
  
} // end of namespace Trigger

#endif // TRIGGER_IMUONROICHAINSVC_H
