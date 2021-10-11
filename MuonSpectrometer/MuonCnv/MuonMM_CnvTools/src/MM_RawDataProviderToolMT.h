/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMM_CNVTOOLS_MM_RAWDATAPROVIDERTOOLMT_H
#define MUONMM_CNVTOOLS_MM_RAWDATAPROVIDERTOOLMT_H

#include "MM_RawDataProviderToolCore.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonRDO/MM_RawDataCollection_Cache.h"

namespace Muon
{
  /** @class STGC_RawDataProviderToolMT
   *  A tool to decode Micromegas ROB fragments into Micromegas RDO (based on the sTGC tool).
   *  @author Leonidas <leonidas.fountas@cern.ch>
   */
  
  class MM_RawDataProviderToolMT: virtual public IMuonRawDataProviderTool, public MM_RawDataProviderToolCore
  {
    public:

      using IMuonRawDataProviderTool::convert; // Otherwise get a -Woverloaded-virtual warning about hiding convert(const std::vector<IdentifierHash>& rdoIdhVect)
      
      /** Default constructor */
      MM_RawDataProviderToolMT(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Default destructor */
      virtual ~MM_RawDataProviderToolMT() = default;
      
      /** Standard AlgTool method */
      virtual StatusCode initialize() override;

      // unused, needing initialization
      virtual StatusCode convert() const override; //!< for the entire event 
      virtual StatusCode convert(const ROBFragmentList&) const override;
      virtual StatusCode convert(const std::vector<IdentifierHash>& rdoIdhVect) const override;
      virtual StatusCode convert(const ROBFragmentList&, const std::vector<IdentifierHash>&) const override;

      // used 
      virtual StatusCode convert(const EventContext&) const override;
      virtual StatusCode convert(const std::vector<IdentifierHash>&, const EventContext&) const override;

    protected:

      StatusCode initRdoContainer(const EventContext&, MM_RawDataContainer*&) const; 
      std::vector<uint32_t>  m_allRobIds;
      
      // Key for the cache for the CSM containers; can be empty,
      SG::UpdateHandleKey<MM_RawDataCollection_Cache> m_rdoContainerCacheKey;
  };
} // end of namespace

#endif // MUONMM_CNVTOOLS_MM_RAWDATAPROVIDERTOOLMT_H

