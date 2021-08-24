/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MUON_NSW_COMMON_DECODER_H_
#define _MUON_NSW_COMMON_DECODER_H_

#include <stdint.h>
#include <vector>

#include "eformat/eformat.h"

namespace Muon
{
  namespace nsw
  {
    class NSWElink;
    class VMMChannel;

    class NSWCommonDecoder
    {
     public:
      explicit NSWCommonDecoder (const eformat::read::ROBFragment &rob);
      virtual ~NSWCommonDecoder ();

      const std::vector <Muon::nsw::NSWElink *> &get_elinks () const {return m_elinks;};
      const std::vector <Muon::nsw::VMMChannel *> &get_channels () const {return m_channels;};

     private:
      std::vector <Muon::nsw::NSWElink *> m_elinks;
      std::vector <Muon::nsw::VMMChannel *> m_channels;
    };
  }
}

#endif // _MUON_NSW_COMMON_DECODER_H_
