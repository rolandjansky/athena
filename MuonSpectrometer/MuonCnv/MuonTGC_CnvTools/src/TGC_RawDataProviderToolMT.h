/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOLMT_H
#define MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOLMT_H

#include "TGC_RawDataProviderToolCore.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include "TGC_Hid2RESrcID.h"
#include "MuonRDO/TgcRdo_Cache.h"

namespace Muon
{

  /** @class TGC_RawDataProviderToolMT
   *  A tool to decode TGC ROB fragments into TGC RDO.
   *  This version is for athenaMT.
   *
   *  @author Zvi Tarem <zvi@caliper.co.il>
   *  @author Mark Owen <markowen@cern.ch>
   */
  
  class TGC_RawDataProviderToolMT : virtual public IMuonRawDataProviderTool, public TGC_RawDataProviderToolCore
    {
    public:
      /** Default constructor */
      TGC_RawDataProviderToolMT(const std::string& t, const std::string& n, const IInterface* p);
      /** Default destructor */
      virtual ~TGC_RawDataProviderToolMT()=default;
      
      /** Standard AlgTool method */
      virtual StatusCode initialize();
      
      /** Old decoding method which uses IROBDataProviderSvc in TgcRdoToPrepDataTool */
      virtual StatusCode convert(const ROBFragmentList& vecRobs);
      /** Old decoding method which uses IROBDataProviderSvc in TgcRdoToPrepDataTool with IdentifierHash vector */
      virtual StatusCode convert(const ROBFragmentList& vecRobs,
				 const std::vector<IdentifierHash>& rdoIdhVect);
      /** New decoding methods which do not use IROBDataProviderSvc in TgcRdoToPrepDataTool */
      virtual StatusCode convert();
      /** New decoding methods which do not use IROBDataProviderSvc in TgcRdoToPrepDataTool with ID Hash vector */
      virtual StatusCode convert(const std::vector<IdentifierHash>& rdoIdhVect);

    private:
      // TGC container cache key
      SG::UpdateHandleKey<TgcRdo_Cache> m_rdoContainerCacheKey ;

      /** convert from vector of ROB IDs is not available */
      virtual StatusCode convert(const std::vector<uint32_t>&) {return StatusCode::FAILURE;}

    };
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOLMT_H
