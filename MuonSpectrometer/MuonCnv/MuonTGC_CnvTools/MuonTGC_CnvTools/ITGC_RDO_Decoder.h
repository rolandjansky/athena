/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOL_ITGC_RDO_Decoder_H
#define MUONTGC_CNVTOOL_ITGC_RDO_Decoder_H

#include "GaudiKernel/IAlgTool.h"

class ITGCcablingSvc;
class TgcDigit;
class TgcRawData;
class Identifier;

static const InterfaceID IID_ITGC_RDO_Decoder("Muon::ITGC_RDO_Decoder", 1, 0);

namespace Muon {

  /** @class ITGC_RDO_Decoder
   *  The interface for AlgTool which provides conversion from RDO data to 
   *  TGC digits and offline ID. 
   *  @author Susumu Oda <Susumu.Oda@cern.ch>
   */
  
  class ITGC_RDO_Decoder : virtual public IAlgTool {

  public: 
    /** Provide interface ID of ITGC_RDO_Decoder */
    static const InterfaceID& interfaceID() { return IID_ITGC_RDO_Decoder; };

    /** Set a flag for application of patch. 
     *  Need to describe when patch is required. */
    virtual void applyPatch(bool patch) =0;
    /** Get TGC Digit from TGC RDO */
    virtual TgcDigit * getDigit(const TgcRawData* rawData, bool orFlag) const=0;
    /** Get offline ID and bcTag from TGC RDO */
    virtual Identifier getOfflineData(const TgcRawData* rawData, bool orFlag, uint16_t& bctag) const=0;
  }; 

} // end of namespace

#endif // MUONTGC_CNVTOOL_ITGC_RDO_Decoder_H 
