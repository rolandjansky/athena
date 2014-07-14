/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITGC_RodDecoder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONTGC_CNVTOOLS_IMUONTGC_RODDECODER_H
#define MUONTGC_CNVTOOLS_IMUONTGC_RODDECODER_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h"

class TgcRdoContainer;

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

namespace Muon
{

  static const InterfaceID IID_ITGC_RodDecoder("Muon::ITGC_RodDecoder", 1, 0);

  /** @class ITGC_RodDecoder 
   *  The interface for AlgTool which decodes a TGC ROB fragment into TGC RDO. 
   *  @author Susumu Oda <Susumu.Oda@cern.ch> 
   */  

  class ITGC_RodDecoder : virtual public IAlgTool
    {
    public:
      /** Provide interface ID of ITGC_RodDecoder */
      static const InterfaceID& interfaceID()
	{
	  return IID_ITGC_RodDecoder;
	}

      /** Convert ROBFragments to RDOs */
      virtual StatusCode fillCollection(const ROBFragment& robFrag, TgcRdoContainer& rdoIdc) = 0;

    };

} // end of namespace

#endif // MUONTGC_CNVTOOLS_IMUONTGC_RODDECODER_H
