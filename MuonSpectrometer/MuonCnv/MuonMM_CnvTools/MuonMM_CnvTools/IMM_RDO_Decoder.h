/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMM_CNVTOOLS_IMM_RDO_DECODER_H
#define MUONMM_CNVTOOLS_IMM_RDO_DECODER_H

#include "GaudiKernel/IAlgTool.h"

class MmDigit;
class MmIdHelper;
class Identifier;

static const InterfaceID IID_IMM_RDO_Decoder("Muon::IMM_RDO_Decoder", 1, 0);  

namespace Muon {
  class MM_RawData;
  
  class IMM_RDO_Decoder : virtual public IAlgTool {
    
  public:
    
    /** AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID( );
    
    virtual MmDigit * getDigit(const Muon::MM_RawData * Rawdata) const = 0;
    
  };
  
}

inline const InterfaceID& Muon::IMM_RDO_Decoder::interfaceID()
{
  return IID_IMM_RDO_Decoder;
}


#endif



