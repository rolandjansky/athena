/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMM_ROD_Decoder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONMM_CNVTOOLS_IMUONMM_ROD_DECODER_H
#define MUONMM_CNVTOOLS_IMUONMM_ROD_DECODER_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h"

namespace Muon
{
  class MM_RawDataContainer;

  static const InterfaceID IID_IMM_ROD_Decoder("Muon::IMM_ROD_Decoder", 1, 0);

   /** @class IMM_ROD_Decoder, based on the respective class for TGCs.
   *   The interface for AlgTool which decodes an MM ROB fragment into MM RDO. 
   *   @author Stelios Angelidakis <sangelid@cern.ch>
   */  

  class IMM_ROD_Decoder : virtual public IAlgTool
  {
    public:
      /** Provide interface ID of IMM_ROD_Decoder */
      static const InterfaceID& interfaceID() { return IID_IMM_ROD_Decoder; }

      /** Convert ROBFragments to RDOs */
      virtual StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment&, const std::vector<IdentifierHash>&, MM_RawDataContainer&) const = 0;
  };

} // end of namespace

#endif // MUONMM_CNVTOOLS_IMUONMM_ROD_DECODER_H

