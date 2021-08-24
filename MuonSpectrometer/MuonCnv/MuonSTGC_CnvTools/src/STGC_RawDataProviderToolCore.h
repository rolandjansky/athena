/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSTGC_CNVTOOLS_STGC_RAWDATAPROVIDERTOOLCORE_H
#define MUONSTGC_CNVTOOLS_STGC_RAWDATAPROVIDERTOOLCORE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonSTGC_CnvTools/ISTGC_ROD_Decoder.h"

#include "MuonRDO/STGC_RawDataContainer.h"

namespace Muon {

  /** @class STGC_RawDataProviderToolCore
   *  A tool to decode STGC ROB fragments into STGC RDO (based on the TGC tool).
   *  @author Stelios Angelidakis <sangelid@cern.ch>
   */
  
  class STGC_RawDataProviderToolCore : public AthAlgTool
  {
    public:
      /** Default constructor */
      STGC_RawDataProviderToolCore(const std::string& t, const std::string& n, const IInterface* p);

      /** Default destructor */
      virtual ~STGC_RawDataProviderToolCore() = default;

      /** Standard AlgTool method */
      virtual StatusCode initialize() override;

      /** Method that converts the ROBFragments into the passed container */
      virtual StatusCode convertIntoContainer (const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>&, 
                                               const std::vector<IdentifierHash>&, STGC_RawDataContainer&) const;

    protected:

      /** The ID helper */
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      /** Decoder for ROB fragment RDO conversion */
      ToolHandle<ISTGC_ROD_Decoder> m_decoder{this, "Decoder", "Muon::STGC_ROD_Decoder/STGC_ROD_Decoder"};

      /** RDO container key */
      SG::WriteHandleKey<STGC_RawDataContainer> m_rdoContainerKey{ this, "RdoLocation", "sTGCRDO", "Name of the sTGCRDO produced by RawDataProvider"};	//MT

      unsigned int m_maxhashtoUse{0}; //MT

      /** Rob Data Provider handle */
      ServiceHandle<IROBDataProviderSvc>  m_robDataProvider;
  };
} // end of namespace

#endif // MUONSTGC_CNVTOOLS_STGC_RAWDATAPROVIDERTOOLCORE_H
