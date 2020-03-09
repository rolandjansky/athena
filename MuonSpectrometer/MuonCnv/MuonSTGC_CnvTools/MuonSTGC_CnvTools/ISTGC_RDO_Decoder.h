/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSTGC_CNVTOOLS_ISTGC_RDO_DECODER_H
#define MUONSTGC_CNVTOOLS_ISTGC_RDO_DECODER_H

#include "GaudiKernel/IAlgTool.h"

class sTgcDigit;
class sTgcIdHelper;
class Identifier;

static const InterfaceID IID_ISTGC_RDO_Decoder("Muon::ISTGC_RDO_Decoder", 1, 0);  

namespace Muon {
  class STGC_RawData;
  
  class ISTGC_RDO_Decoder : virtual public IAlgTool {
    
  public:
    
    /** AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID( );
    
    virtual sTgcDigit * getDigit(const Muon::STGC_RawData * Rawdata) const = 0;
    
  };
  
}

inline const InterfaceID& Muon::ISTGC_RDO_Decoder::interfaceID()
{
  return IID_ISTGC_RDO_Decoder;
}


#endif



