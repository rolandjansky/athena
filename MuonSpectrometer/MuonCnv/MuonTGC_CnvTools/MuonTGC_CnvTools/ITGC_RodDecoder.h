/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITGC_RodDecoder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef MUONTGC_CNVTOOLS_IMUONTGC_RODDECODER_H
#define MUONTGC_CNVTOOLS_IMUONTGC_RODDECODER_H

#include "GaudiKernel/IAlgTool.h"
#include "ByteStreamData/RawEvent.h"

class TgcRdoContainer;

//using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

namespace Muon
{
  /** @class ITGC_RodDecoder 
   *  The interface for AlgTool which decodes a TGC ROB fragment into TGC RDO. 
   *  @author Susumu Oda <Susumu.Oda@cern.ch> 
   */  

  class ITGC_RodDecoder : virtual public IAlgTool
    {
    public:
      DeclareInterfaceID( ITGC_RodDecoder, 1, 0 );

      /** Convert ROBFragments to RDOs */
      virtual StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, TgcRdoContainer& rdoIdc) const = 0;

    };

} // end of namespace

#endif // MUONTGC_CNVTOOLS_IMUONTGC_RODDECODER_H
