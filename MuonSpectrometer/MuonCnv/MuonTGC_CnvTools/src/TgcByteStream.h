/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCBYTESTREAM_H
#define MUONTGC_CNVTOOLS_TGCBYTESTREAM_H

#include <inttypes.h>
#include <vector>

class TgcRdo;
class MsgStream;

namespace Muon 
{

  /** @class TgcByteStream 
   *  This class provides conversions between RDO and ByteStream. 
   *  
   *  @author Susumu Oda <Susumu.Oda@cern.ch>
   */
  
  class TgcByteStream
    {
    private:
      typedef std::vector<uint32_t> ByteStream;

    public:
      /** Constructor */
      TgcByteStream();
      /** Destructor */
      ~TgcByteStream();
      /** Convert RDO to ByteStream */
      void rdo2ByteStream(const TgcRdo* rdo, ByteStream& bs, MsgStream& log);
      /** Convert ByteStream to RDO */
      void byteStream2Rdo(const ByteStream& bs, TgcRdo& rdo, uint32_t source_id, MsgStream& log);
    };
} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCBYTESTREAM_H
