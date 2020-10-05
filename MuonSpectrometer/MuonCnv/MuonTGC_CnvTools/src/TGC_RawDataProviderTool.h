/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOL_H
#define MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOL_H

#include "CxxUtils/checker_macros.h"

#include "TGC_RawDataProviderToolCore.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include "TGC_Hid2RESrcID.h"

namespace Muon
{

  /** @class TGC_RawDataProviderTool
   *  A tool to decode TGC ROB fragments into TGC RDO.
   *  This version is for legacy run-2 athena and is not thread safe.
   *
   *  @author Zvi Tarem <zvi@caliper.co.il>
   *  @author Mark Owen <markowen@cern.ch>
   */
  
  class ATLAS_NOT_THREAD_SAFE TGC_RawDataProviderTool : virtual public IMuonRawDataProviderTool, public TGC_RawDataProviderToolCore
    {
    public:
      /** Default constructor */
      TGC_RawDataProviderTool(const std::string& t, const std::string& n, const IInterface* p);
      /** Default destructor */
      virtual ~TGC_RawDataProviderTool()=default;
      
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

      /** convert from vector of ROB IDs is not available */
      virtual StatusCode convert(const std::vector<uint32_t>&) {return StatusCode::FAILURE;}

    };
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOL_H
