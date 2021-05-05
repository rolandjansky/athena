/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  
  class ATLAS_NOT_THREAD_SAFE TGC_RawDataProviderTool
    : public extends<TGC_RawDataProviderToolCore, IMuonRawDataProviderTool>
    {
    public:
      /** Default constructor */
      TGC_RawDataProviderTool(const std::string& t, const std::string& n, const IInterface* p);
      /** Default destructor */
      virtual ~TGC_RawDataProviderTool()=default;
      
      /** Standard AlgTool method */
      virtual StatusCode initialize() override;
      
      /** Old decoding method which uses IROBDataProviderSvc in TgcRdoToPrepDataTool */
      virtual StatusCode convert(const ROBFragmentList& vecRobs) const override;
      /** Old decoding method which uses IROBDataProviderSvc in TgcRdoToPrepDataTool with IdentifierHash vector */
      virtual StatusCode convert(const ROBFragmentList& vecRobs,
				 const std::vector<IdentifierHash>& rdoIdhVect) const override;
      /** New decoding methods which do not use IROBDataProviderSvc in TgcRdoToPrepDataTool */
      virtual StatusCode convert() const override;
      /** New decoding methods which do not use IROBDataProviderSvc in TgcRdoToPrepDataTool with ID Hash vector */
      virtual StatusCode convert(const std::vector<IdentifierHash>& rdoIdhVect) const override;
      
      /** EventContext ones **/
      virtual StatusCode convert(const ROBFragmentList&, const EventContext&) const override;
      virtual StatusCode convert(const ROBFragmentList&, const std::vector<IdentifierHash>&, const EventContext&) const override;
      virtual StatusCode convert(const EventContext&) const override;
      virtual StatusCode convert(const std::vector<IdentifierHash>&, const EventContext&) const override;

    private:

      /** convert from vector of ROB IDs is not available */
      virtual StatusCode convert(const std::vector<uint32_t>&) const override {return StatusCode::FAILURE;}
      /** EventContext ones **/
      virtual StatusCode convert(const std::vector<uint32_t>&, const EventContext&) const override {return StatusCode::FAILURE;}


    };
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGC_RAWDATAPROVIDERTOOL_H
