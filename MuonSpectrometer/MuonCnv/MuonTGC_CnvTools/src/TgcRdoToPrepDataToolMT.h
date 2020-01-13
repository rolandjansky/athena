/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TgcRdoToPrepDataToolMT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLMT_H
#define MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLMT_H

#include "TgcRdoToPrepDataToolCore.h"
#include "MuonPrepRawData/MuonPrepDataCollection_Cache.h"
#include "MuonTrigCoinData/MuonTrigCoinData_Cache.h"
#include "StoreGate/UpdateHandleKeyArray.h"

namespace Muon 
{
  /** @class TgcRdoToPrepDataToolMT 
   *  This is the algorithm that convert TGCRdo To TGCPrepdata as a tool.
   * 
   * @author Susumu Oda <Susumu.Oda@cern.ch> 
   * @author  Edward Moyse 
   * 
   * This class was developed by Takashi Kubota. 
   */  

  class TgcRdoToPrepDataToolMT : virtual public TgcRdoToPrepDataToolCore
  {
    public:
      /** Constructor */
      TgcRdoToPrepDataToolMT(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Destructor */
      virtual ~TgcRdoToPrepDataToolMT();
      
      /** Standard AthAlgTool initialize method */
      virtual StatusCode initialize() override;
      /** Standard AthAlgTool finalize method */
      virtual StatusCode finalize() override;
    
      using TgcRdoToPrepDataToolCore::decode; // To prevent the decode below from hiding the superclass decode methods
      virtual StatusCode decode(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect) override;

    private:
      /// This is the key for the cache for the TGC PRD containers, can be empty
      SG::UpdateHandleKeyArray<TgcPrepDataCollection_Cache> m_prdContainerCacheKeys ;
      SG::UpdateHandleKeyArray<TgcCoinDataCollection_Cache> m_coinContainerCacheKeys ;

   }; 
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCRDOTOPREPDATATOOLMT_H
