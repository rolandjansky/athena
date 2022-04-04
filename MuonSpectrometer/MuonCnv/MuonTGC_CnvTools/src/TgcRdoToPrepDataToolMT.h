/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcRdoToPrepDataToolMT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLMT_H
#define MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLMT_H

#include "TgcRdoToPrepDataToolCore.h"

#include "MuonPrepRawData/MuonPrepDataCollection_Cache.h"
#include "MuonTrigCoinData/MuonTrigCoinData_Cache.h"

#include "StoreGate/HandleKeyArray.h"
#include "StoreGate/UpdateHandleKey.h"
#include "StoreGate/UpdateHandle.h"


namespace Muon 
{

  // Typedef the two update handle arrays that can be used to match handle key functionality used in Core
  // Requested to not use StoreGate template for UpdateHandleKeyArray
  typedef SG::HandleKeyArray<SG::UpdateHandle<TgcPrepDataCollection_Cache>, SG::UpdateHandleKey<TgcPrepDataCollection_Cache>, Gaudi::DataHandle::Reader > TgcPrdUpdateHandles;
  typedef SG::HandleKeyArray<SG::UpdateHandle<TgcCoinDataCollection_Cache>, SG::UpdateHandleKey<TgcCoinDataCollection_Cache>, Gaudi::DataHandle::Reader > TgcCoinUpdateHandles;


  /** @class TgcRdoToPrepDataToolMT 
   *  This is the algorithm that convert TGCRdo To TGCPrepdata as a tool.
   * 
   * @author Susumu Oda <Susumu.Oda@cern.ch> 
   * @author  Edward Moyse 
   * 
   * This class was developed by Takashi Kubota. 
   */  

  class TgcRdoToPrepDataToolMT : public extends<TgcRdoToPrepDataToolCore, IMuonRdoToPrepDataTool>
  {
    public:
      /** Constructor */
      TgcRdoToPrepDataToolMT(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcRdoToPrepDataToolMT()=default;
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;
    
      using TgcRdoToPrepDataToolCore::decode; // To prevent the decode below from hiding the superclass decode methods
      virtual StatusCode decode(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect) const override;

      virtual void printPrepData() const override;

  private:
      /// This is the key for the cache for the TGC PRD containers, can be empty
      TgcPrdUpdateHandles m_prdContainerCacheKeys ;
      TgcCoinUpdateHandles m_coinContainerCacheKeys ;
      // TgcPrepRawData container key for current BC 
      Gaudi::Property<std::string> m_prdContainerCacheKeyStr{this, "PrdCacheString", "", "Prefix for names of PRD cache collections"};
      // TgcCoinData container key for current BC 
      std::string m_coinContainerCacheKeyStr;

   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLMT_H
