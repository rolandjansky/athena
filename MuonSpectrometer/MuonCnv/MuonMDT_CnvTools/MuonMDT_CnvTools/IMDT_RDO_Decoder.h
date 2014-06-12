/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CNVTOOLS_IMDT_RDO_DECODER_H
#define MUONMDT_CNVTOOLS_IMDT_RDO_DECODER_H

#include "GaudiKernel/IAlgTool.h"

class MdtDigit;
class MdtIdHelper;
class MuonMDT_CablingSvc;
class MdtAmtHit;
class Identifier;

static const InterfaceID IID_IMDT_RDO_Decoder("Muon::IMDT_RDO_Decoder", 1, 0);  

namespace Muon {
  
  class IMDT_RDO_Decoder : virtual public IAlgTool {
    
  public:
    
    /** AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID( );
    
    //    virtual void set(const MdtIdHelper * mdtIdHelper, const MDTcablingSvc* mdtCabling)=0;
    
    virtual MdtDigit * getDigit(const MdtAmtHit * amtHit, uint16_t& subdetId, 
				uint16_t& mrodId, uint16_t& csmId) const = 0;
    
    virtual Identifier getOfflineData(const MdtAmtHit * amtHit, uint16_t& subdetId, 
				      uint16_t& mrodId, uint16_t& csmId, int& tdc, int& width) const =0;
    
  };
  
}

inline const InterfaceID& Muon::IMDT_RDO_Decoder::interfaceID()
{
  return IID_IMDT_RDO_Decoder;
}


#endif



