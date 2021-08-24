/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMM_CNVTOOLS_MM_RAWDATAPROVIDERTOOLCORE_H
#define MUONMM_CNVTOOLS_MM_RAWDATAPROVIDERTOOLCORE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonMM_CnvTools/IMM_ROD_Decoder.h"

#include "MuonRDO/MM_RawDataContainer.h"

namespace Muon {

  /** @class MM_RawDataProviderToolCore
   *  A tool to decode MM ROB fragments into MM RDO (based on the sTGC tool).
   *  @author Leonidas Fountas <leonidas.fountas@cern.ch>
   */
   
  class MM_RawDataProviderToolCore : public AthAlgTool
  {
    public:
      /** Default constructor */
      MM_RawDataProviderToolCore(const std::string& t, const std::string& n, const IInterface* p);
      
      /** Default destructor */
      virtual ~MM_RawDataProviderToolCore() = default;
      
      /** Standard AlgTool method */
      virtual StatusCode initialize() override;
      
      /** Method that converts the ROBFragments into the passed container */
      virtual StatusCode convertIntoContainer(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>&, 
                                              const std::vector<IdentifierHash>&, MM_RawDataContainer&) const;

    protected:

      /** The ID helper */
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
      
      /** Decoder for ROB fragment RDO conversion */
      ToolHandle<IMM_ROD_Decoder> m_decoder{this, "Decoder", "Muon::MmROD_Decoder/MmROD_Decoder"};
      
      /** RDO container key */
      SG::WriteHandleKey<MM_RawDataContainer> m_rdoContainerKey{ this, "RdoLocation", "MMRDO", "Name of the MMRDO produced by RawDataProvider"}; //MT

      unsigned int m_maxhashtoUse{0}; //MT

      /** Rob Data Provider handle */
      ServiceHandle<IROBDataProviderSvc>  m_robDataProvider;
  };
} // end of namespace

#endif // MUONMM_CNVTOOLS_MM_RAWDATAPROVIDERTOOLCORE_H

