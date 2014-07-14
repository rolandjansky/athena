/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCROD_ENCODER_H
#define MUONTGC_CNVTOOLS_TGCROD_ENCODER_H

#include <inttypes.h>
#include <vector>

class TgcRdo;

namespace Muon 
{

  /** @class TgcROD_Encoder
   *  This class provides conversion from TgcRdo to ROD format. 
   * 
   *  @author Susumu Oda <Susumu.Oda@cern.ch>
   *
   *  This class was devloped by Tadashi Maeno based on 
   *  RpcROD_Encoder written by Ketevi A. Assamagan
   */

  class TgcROD_Encoder
    {
    public: 

      /** Constructor */
      TgcROD_Encoder(); 

      /** Destructor */
      ~TgcROD_Encoder(); 

      /** Set TgcRdo */ 
      void setRdo(const TgcRdo* rdo);

      /** Convert all ROB fragment in the current list to 
       *  a vector of 32bit words. 
       */ 
      void fillROD(std::vector<uint32_t>& v) ; 

    private:
      const TgcRdo * m_tgcRdo;
    }; 

} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCROD_ENCODER_H
