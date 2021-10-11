/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_STGC_RAWDATAPROVIDERTOOLMT_H
#define MUONTGC_CNVTOOLS_STGC_RAWDATAPROVIDERTOOLMT_H

#include "STGC_RawDataProviderToolCore.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonRDO/STGC_RawDataCollection_Cache.h"

namespace Muon
{
  /** @class STGC_RawDataProviderToolMT
   *  A tool to decode STGC ROB fragments into STGC RDO (based on the TGC tool).
   *  @author Stelios Angelidakis <sangelid@cern.ch>
   */
  
  class STGC_RawDataProviderToolMT : virtual public IMuonRawDataProviderTool, public STGC_RawDataProviderToolCore
  {
    public:

      using IMuonRawDataProviderTool::convert; // Otherwise get a -Woverloaded-virtual warning about hiding convert(const std::vector<IdentifierHash>& rdoIdhVect)

      /** Default constructor */
      STGC_RawDataProviderToolMT(const std::string& t, const std::string& n, const IInterface* p);

      /** Default destructor */
      virtual ~STGC_RawDataProviderToolMT() = default;
      
      /** Standard AlgTool method */
      virtual StatusCode initialize() override;

      // unused, needing initialization
      virtual StatusCode convert() const override;
      virtual StatusCode convert(const ROBFragmentList&) const override;
      virtual StatusCode convert(const std::vector<IdentifierHash>& rdoIdhVect) const override;
      virtual StatusCode convert(const ROBFragmentList&, const std::vector<IdentifierHash>&) const override;

      // used
      virtual StatusCode convert(const EventContext&) const override;
      virtual StatusCode convert(const std::vector<IdentifierHash>&, const EventContext&) const override;
      
    protected:

      StatusCode initRdoContainer(const EventContext&, STGC_RawDataContainer*&) const;
      std::vector<uint32_t>  m_allRobIds;
      
      // Key for the cache for the CSM containers; can be empty,
      SG::UpdateHandleKey<STGC_RawDataCollection_Cache> m_rdoContainerCacheKey;
  };
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOLMT_H
